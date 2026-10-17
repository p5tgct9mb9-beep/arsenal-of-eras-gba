#!/usr/bin/env python3
"""mkcover.py - DS title screens from the official cover art (art/cover_art.png).
  mkcover.py render   -> art/cover_top.png, art/cover_bottom.png, ../icon.gif   (needs the Poppins font; run by hand)
  mkcover.py pack OUT -> nitro:/cover.bin = top 256x192 + bottom 256x192, raw RGB15|alpha (used by build.sh)
The photo is cropped and downsampled (no upscaling); the title and tagline are re-set as crisp spaced text,
because the cover's own lettering is unreadable at DS size."""
import os, sys, struct
from PIL import Image, ImageDraw, ImageFont
H = os.path.dirname(os.path.abspath(__file__))
FONT = '/usr/share/fonts/truetype/sand-box/google/Poppins/Poppins-Light.ttf'
def spaced(d, cx, y, s, f, fill, gap):
    ws = [d.textlength(c, font=f) for c in s]; tot = sum(ws) + gap*(len(s)-1); x = cx - tot/2
    for c, w in zip(s, ws): d.text((x, y), c, font=f, fill=fill); x += w + gap
def render():
    src = Image.open(os.path.join(H, 'cover_art.png')).convert('RGB')
    k = src.size[0] / 1024; K = lambda *v: tuple(int(round(a * k)) for a in v)   # layout in 1024-px cover units
    im = src.copy(); dd = ImageDraw.Draw(im)
    dd.rectangle(K(180, 200, 850, 275), fill=(0, 0, 0))              # remove the original title (re-set below)
    top = im.crop(K(0, 180, 1024, 948)).resize((256, 192), Image.LANCZOS)   # 4:3 window; tagline row excluded
    d = ImageDraw.Draw(top)
    fT = ImageFont.truetype(FONT, 13); fo = ImageFont.truetype(FONT, 8); fg = ImageFont.truetype(FONT, 7)
    # title: 'A R S E N A L  of  E R A S'
    def width(s, f, g): return sum(d.textlength(c, font=f) for c in s) + g*(len(s)-1)
    a, b, c = 'ARSENAL', 'OF', 'ERAS'; ga = 6; wa, wb, wc = width(a, fT, ga), width(b, fo, 2), width(c, fT, ga)
    x0 = 128 - (wa + 10 + wb + 10 + wc)/2
    spaced(d, x0 + wa/2, 6, a, fT, (225, 228, 232), ga); spaced(d, x0 + wa + 10 + wb/2, 10, b, fo, (200, 204, 210), 2)
    spaced(d, x0 + wa + 10 + wb + 10 + wc/2, 6, c, fT, (225, 228, 232), ga)
    # tagline over the lunar surface (shadowed for legibility)
    for ox, oy, col in ((1, 1, (0, 0, 0)), (0, 0, (215, 218, 222))):
        spaced(ImageDraw.Draw(top), 128 + ox, 178 + oy, 'FROM THE TRENCHES TO THE STARS', fg, col, 3)
    top.save(os.path.join(H, 'cover_top.png'))
    bot = Image.new('RGB', (256, 192), (0, 0, 0)); d = ImageDraw.Draw(bot)
    moon = src.crop(K(0, 850, 1024, 942)).resize((256, 44), Image.LANCZOS); bot.paste(moon, (0, 148))
    spaced(d, 128, 74, 'TOUCH TO START', ImageFont.truetype(FONT, 11), (230, 232, 236), 5)
    spaced(d, 128, 96, 'OR PRESS START', ImageFont.truetype(FONT, 8), (150, 154, 160), 3)
    bot.save(os.path.join(H, 'cover_bottom.png'))
    # DS menu icon: Earth over the Moon horizon, 32x32, 16 colours
    icon = Image.new('RGB', (32, 32), (0, 0, 0))
    icon.paste(src.crop(K(505, 432, 685, 568)).resize((20, 15), Image.LANCZOS), (6, 4))
    icon.paste(src.crop(K(0, 770, 1024, 1024)).resize((32, 11), Image.LANCZOS), (0, 21))
    icon.quantize(colors=16).save(os.path.join(H, '..', 'icon.gif'))
def pack(out):
    b = b''
    for n in ('cover_top.png', 'cover_bottom.png'):
        im = Image.open(os.path.join(H, n)).convert('RGB')
        b += b''.join(struct.pack('<H', 0x8000 | (r >> 3) | ((g >> 3) << 5) | ((bl >> 3) << 10)) for r, g, bl in im.getdata())
    open(out, 'wb').write(b)
if __name__ == '__main__':
    render() if sys.argv[1] == 'render' else pack(sys.argv[2])
