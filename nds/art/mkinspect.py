#!/usr/bin/env python3
"""mkinspect.py OUT.bin [PREVIEWDIR] - renders the DS Museum / INSPECT screens (top + one bottom per tab) for every
entry in inspect_data.py with the approved art toolkit (F-4E / M1 Garand standard), and packs them for NitroFS.
Format: 'AOEM' u32 n, then n x {name[24], cat, era, ntabs, pad, tabname[3][12], off[4], len[4]} (img 0 = top),
each image = 256 x BGR555 palette + RLE (save.c scheme) of 256x192 8-bit indices."""
import sys, os, struct
H = os.path.dirname(os.path.abspath(__file__))
for d in ('ds_work', 'inf_work', 'rm_work', 'ww2_work'): sys.path.insert(0, os.path.join(H, d))
from PIL import Image, ImageDraw
import f4ds, m1ds
from f4ds import tabs
from cardlib import *
import cardlib, ac_ds, land_ds, m4ds, weap
from f4show import sky
from m1show import felt
from inspect_data import ENTRIES, AIR, LAND, ARMS
GLYPH['"'] = ['#.#..', '#.#..', '.....', '.....', '.....', '.....', '.....']
LBL = (150, 170, 200); OLB = (196, 184, 140)
CATN = ['HANGAR', 'MOTOR POOL', 'ARMORY']
def model(spec):
    k, n = spec.split(':')
    if k == 'ac': a = getattr(ac_ds, n)()
    elif k == 'land': a = getattr(land_ds, n)()
    elif k == 'm4': P, L = m4ds.m4a1(); a = dict(parts=P, lines=L)
    elif k == 'w': P, L = getattr(weap, n)(); a = dict(parts=P, lines=L)
    return a
def marks(T, a, xf):
    for m in a.get('marks', []):
        k = m[0]; X, Y = xf.f(m[1], m[2]); X, Y = int(round(X)), int(round(Y)); s = m[3] if len(m) > 3 else 4
        S_ = max(2, int(round(s * xf.sc)))
        if k == 'cross': bcross(T, X, Y, S_)
        elif k == 'star' and len(m) > 4 and m[4] == 'ww2c': vstar(T, X, Y, S_)
        elif k == 'star': us_star(T, X, Y, S_, m[4] if len(m) > 4 else 'ww2')
        elif k == 'roundel': roundel(T, X, Y, S_, m[4] if len(m) > 4 else True)
        elif k == 'flash': finflash(T, X, Y, max(3, S_), max(3, int(m[4] * xf.sc)))
        elif k == 'hino': hinomaru(T, X, Y, S_)
        elif k == 'redstar': redstar(T, X, Y, S_, m[4] if len(m) > 4 else (236, 236, 230))
        elif k == 'prop': prop(T, X, Y, max(3, S_))
        elif k == 'rotor': rotor(T, X, Y, max(6, S_), m[4] if len(m) > 4 else 2)
        elif k == 'disc': disc(T, X, Y, max(3, S_))
def backdrop(cat, W, Hh):
    if cat == AIR: return sky(W, Hh)
    if cat == LAND:
        im = bg_ground(W, Hh); return im
    return felt(W, Hh)
def header(im, e, crumb):
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 255, 11), fill=(6, 8, 18)); d.line((0, 11, 255, 11), fill=NAVYL)
    text(im, 4, 3, crumb, LBL)
    nm = e['name']; sc = 2 if tw(nm) * 2 <= 200 else 1
    text(im, 4, 14 if sc == 2 else 18, nm, GOLD, scale=sc, adv=6)
    rtext(im, 252, 16, e['country'], BLUE); rtext(im, 252, 25, e['era'], GREEN)
def top_generic(e):
    im = Image.new('RGB', (256, 192), NAVYD); d = ImageDraw.Draw(im)
    sub = {AIR: '', LAND: '', ARMS: ''}[e['cat']]
    header(im, e, '%s > %s > INSPECT' % (CATN[e['cat']], e['era']))
    nbevel(im, 2, 32, 253, 163, edge=(110, 100, 60) if e['cat'] == ARMS else (58, 120, 170))
    T = backdrop(e['cat'], 248, 127); a = model(e['model'])
    land = e['cat'] == LAND
    box_ = (8, 10, 240, 118) if e['cat'] != ARMS else (8, 28, 240, 100)
    xf, mask = fit_mask(T, a['parts'], a.get('lines', []), a.get('dots', []), box_, align='b' if land else 'c')
    if land: shadow(T, mask, 1, 0.08, col=(24, 22, 16))
    elif e['cat'] == AIR: shadow(T, mask, 5, 0.08, col=(40, 60, 90))
    marks(T, a, xf); im.paste(T, (4, 34))
    d.rectangle((0, 166, 255, 191), fill=(6, 8, 18)); d.line((0, 166, 255, 166), fill=NAVYL)
    text(im, 4, 170, e['caption'], (206, 210, 220)); rtext(im, 252, 181, 'VIEW 1/1', LBL)
    return im
def rows_panel(im, y0, y1, rows, step=10):
    d = ImageDraw.Draw(im); nbevel(im, 2, y0, 253, y1)
    for i, (a, b) in enumerate(rows):
        y = y0 + 5 + i * step
        if i % 2: d.rectangle((5, y - 2, 250, y + 7), fill=(26, 38, 70))
        text(im, 8, y, a, LBL); rtext(im, 249, y, b, WHITE)
def page(e, tabnames, tab):
    im = Image.new('RGB', (256, 192), NAVYD); tabs(im, 1, tabnames, tab); return im
def footer(im):
    text(im, 4, 182, 'B:BACK  L/R:PREV/NEXT  TOUCH:TAB', LBL)
def history_page(e, tabnames, tab):
    im = page(e, tabnames, tab); nbevel(im, 2, 19, 253, 178)
    for i, l in enumerate(cardlib.wrap(e['history'], 40)[:15]): text(im, 8, 25 + i * 10, l, WHITE)
    footer(im); return im
def overview_page(e, tabnames, tab):
    im = page(e, tabnames, tab)
    if e['cat'] == AIR or e['cat'] == LAND:
        # thumbnail strip: the profile again at small scale on a plain plate, plus era/country chips
        nbevel(im, 2, 19, 253, 78)
        T = backdrop(e['cat'], 246, 55); a = model(e['model'])
        xf, mask = fit_mask(T, a['parts'], a.get('lines', []), a.get('dots', []), (60, 4, 186, 52), align='b' if e['cat'] == LAND else 'c')
        marks(T, a, xf); im.paste(T, (5, 21))
        rows_panel(im, 81, 81 + 6 + 10 * len(e['overview']), e['overview'])
    else:
        rows_panel(im, 19, 25 + 10 * len(e['overview']), e['overview'])
    footer(im); return im
def arms_page(e, tabnames, tab):
    im = page(e, tabnames, tab)
    rows_panel(im, 19, 25 + 10 * len(e['arms']), e['arms'])
    if e['cat'] == ARMS and e['model'].startswith('m4'):
        # attachment demo: the same carbine fitted with optic, light/laser, grip and suppressor
        y = 30 + 10 * len(e['arms']); nbevel(im, 2, y, 253, 178, edge=(110, 100, 60), fill=(30, 30, 22))
        T = felt(246, 178 - y - 4); P, L = m4ds.m4a1(att=('RDS', 'LASER', 'GRIP', 'SUP'))
        fit_mask(T, P, L, [], (6, 4, 240, T.height - 4)); im.paste(T, (5, y + 2))
    footer(im); return im
def oneview(t):
    d = ImageDraw.Draw(t); d.rectangle((196, 178, 255, 190), fill=(6, 8, 18)); rtext(t, 252, 181, 'VIEW 1/1', LBL)
def screens(e):
    if e['model'] == 'hd:f4':
        t, _ = f4ds.top(); oneview(t); tn = ['OVERVIEW', 'ARMAMENT', 'HISTORY']
        b0 = f4ds.bottom(0); ImageDraw.Draw(b0).rectangle((0, 180, 255, 191), fill=NAVYD); footer(b0)
        return t, tn, [b0, arms_page(e, tn, 1), history_page(e, tn, 2)]
    if e['model'] == 'hd:m1':
        t = m1ds.top(); oneview(t); tn = ['OVERVIEW', 'HISTORY', 'ATTACHMENTS']
        b0 = m1ds.bottom(0)
        # ATTACHMENTS: the period attachments larger, with what each does
        b2 = page(e, tn, 2); nbevel(b2, 2, 19, 253, 178, edge=(110, 100, 60), fill=(30, 30, 22))
        info = [('M1 BAYONET', '10 IN BLADE, FITS THE BAYONET LUG', m1ds.bayonet), ('M7 GRENADE LAUNCHER', 'SPIGOT FOR RIFLE GRENADES', m1ds.m7gl),
                ('M84 SCOPE', 'TELESCOPIC SIGHT OF THE M1C / M1D', m1ds.m84)]
        for i, (a, b, f) in enumerate(info):
            y = 22 + i * 52; T = felt(246, 30); m1ds.draw(T, f, (20, 2, 226, 28)); b2.paste(T, (5, y))
            text(b2, 8, y + 33, a, WHITE); text(b2, 8, y + 42, b, LBL)
        return t, tn, [b0, history_page(e, tn, 1), b2]
    tn = ['OVERVIEW', 'ARMAMENT', 'HISTORY'] if e['cat'] != ARMS else ['OVERVIEW', 'FITTINGS', 'HISTORY']
    return top_generic(e), tn, [overview_page(e, tn, 0), arms_page(e, tn, 1), history_page(e, tn, 2)]
def rle(b):
    out = bytearray(); i = 0; n = len(b)
    while i < n:
        r = 1
        while i + r < n and r < 130 and b[i + r] == b[i]: r += 1
        if r >= 3: out += bytes([0x80 | (r - 3), b[i]]); i += r; continue
        s = i; l = 0
        while i < n and l < 128:
            rr = 1
            while i + rr < n and rr < 3 and b[i + rr] == b[i]: rr += 1
            if rr >= 3: break
            i += 1; l += 1
        out += bytes([l - 1]) + b[s:s + l]
    return bytes(out)
def pack_img(im):
    q = im.convert('RGB').quantize(colors=256, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    pal = q.getpalette()[:768]; pal += [0] * (768 - len(pal))
    p = b''.join(struct.pack('<H', 0x8000 | (pal[i * 3] >> 3) | ((pal[i * 3 + 1] >> 3) << 5) | ((pal[i * 3 + 2] >> 3) << 10)) for i in range(256))
    return p + rle(q.tobytes())
if __name__ == '__main__':
    out = sys.argv[1]; prev = sys.argv[2] if len(sys.argv) > 2 else None
    if prev: os.makedirs(prev, exist_ok=True)
    blobs = []; idx = []
    ERA = {'WW2': 0, 'COLD WAR': 1, 'MODERN': 2}
    for e in ENTRIES:
        t, tn, bots = screens(e); imgs = [t] + bots
        if prev:
            pr = Image.new('RGB', (256 * 4, 192))
            for i, im in enumerate(imgs): pr.paste(im, (256 * i, 0))
            pr.save(os.path.join(prev, e['key'] + '.png'))
        offs = []
        for im in imgs:
            b = pack_img(im); offs.append((sum(len(x) for x in blobs), len(b))); blobs.append(b)
        idx.append((e, tn, offs))
    hdr = 8 + len(idx) * (24 + 4 + 36 + 32)
    o = bytearray(b'AOEM' + struct.pack('<I', len(idx)))
    for e, tn, offs in idx:
        o += e['name'].encode()[:23].ljust(24, b'\0') + bytes([e['cat'], ERA[e['era']], len(tn), 0])
        for t in tn: o += t.encode()[:11].ljust(12, b'\0')
        for a, l in offs: o += struct.pack('<I', hdr + a)
        for a, l in offs: o += struct.pack('<I', l)
    assert len(o) == hdr
    o += b''.join(blobs); open(out, 'wb').write(o)
    print('mkinspect: %d entries, %d KB' % (len(idx), len(o) // 1024))
