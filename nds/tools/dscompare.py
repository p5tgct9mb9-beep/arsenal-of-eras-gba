#!/usr/bin/env python3
"""dscompare.py DSDIR GBADIR OUT.png [names...] - side-by-side sheet: GBA shot | DS top screen
(240x160 crop at 8,16) | DS bottom screen, plus a per-shot mean pixel difference."""
import sys, os
from PIL import Image, ImageDraw, ImageChops, ImageStat
ds, gb, out = sys.argv[1:4]; names = sys.argv[4:]
if not names: names = sorted(f[:-4] for f in os.listdir(ds) if f.endswith('.png'))
rows = []
for n in names:
    a = os.path.join(gb, n + '.png'); b = os.path.join(ds, n + '.png')
    if not os.path.exists(b): continue
    D = Image.open(b).convert('RGB'); top = D.crop((8, 16, 248, 176)); bot = D.crop((0, 192, 256, 384))
    G = Image.open(a).convert('RGB').resize((240, 160)) if os.path.exists(a) else Image.new('RGB', (240, 160))
    diff = sum(ImageStat.Stat(ImageChops.difference(G, top)).mean) / 3
    rows.append((n, G, D.crop((0, 0, 256, 192)), bot, diff)); print('%-34s diff %.1f' % (n, diff))
W = 240 + 256 + 256 + 24; H = len(rows) * 206 + 4
sheet = Image.new('RGB', (W, max(H, 10)), (12, 12, 20)); d = ImageDraw.Draw(sheet)
for i, (n, G, T, B, df) in enumerate(rows):
    y = 4 + i * 206; sheet.paste(G, (4, y + 16)); sheet.paste(T, (252, y + 2)); sheet.paste(B, (516, y + 2))
    d.text((4, y + 2), '%s  (GBA | DS top | DS bottom)  diff %.1f' % (n, df), fill=(230, 200, 90))
sheet.save(out)
