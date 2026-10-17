#!/usr/bin/env python3
"""beforeafter.py OLD.nds NEW.nds OUTDIR - runs the graphics-sheet scripts on both ROMs and writes
OUTDIR/ds030_before_after_<mode>.png (both screens, BEFORE left / AFTER right)."""
import os, subprocess, sys
from PIL import Image, ImageDraw
OLD, NEW, OUT = sys.argv[1:4]; os.makedirs(OUT, exist_ok=True); ROOT = '/workspace/aoe-gba'
SHEETS = {
 'infantry_6v6': [('tools/v11_6v6_inf.txt', 'v11_6v6_inf_play'), ('nds/tests/ds_inf_soldiers.txt', 'inf_05'), ('nds/tests/ds_inf_soldiers.txt', 'inf_08b')],
 'armor':        [('nds/tests/ds_armor.txt', 'ds_armor_play2'), ('nds/tests/ds_armor.txt', 'ds_armor_play4')],
 'dogfight':     [('tools/v11_dogfight_p47.txt', 'v11_dogfight_p47_f03'), ('tools/v11_dogfight_p47.txt', 'v11_dogfight_p47_f06')],
 'naval':        [('nds/tests/ds_naval.txt', 'naval_play1'), ('nds/tests/ds_naval.txt', 'naval_play3')],
}
def run(rom, script, tag):
    o = os.path.join(OUT, 'runs', tag, os.path.basename(script)[:-4])
    if not os.path.exists(os.path.join(o, 'log')):
        env = dict(os.environ, NDS=rom); env.pop('NOPNG', None)
        subprocess.run(['nds/tools/dsrun1.sh', script, o], cwd=ROOT, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return o
for mode, rows in SHEETS.items():
    S = 2; W = 256*S; H = 384*S; sheet = Image.new('RGB', (W*2+30, 40+len(rows)*(H+10)), (18,20,26)); d = ImageDraw.Draw(sheet)
    d.text((10,8), 'ARSENAL OF ERAS DS - %s - BEFORE ds-v0.2.0 (left)  /  AFTER ds-v0.3.0 (right)' % mode.upper().replace('_',' '), fill=(240,220,120))
    for r,(script, shot) in enumerate(rows):
        for c,(rom,tag) in enumerate(((OLD,'old'),(NEW,'new'))):
            p = os.path.join(run(rom, script, tag), shot + '.png')
            if os.path.exists(p): sheet.paste(Image.open(p).convert('RGB').resize((W,H), Image.NEAREST), (10+c*(W+10), 30+r*(H+10)))
            else: d.text((20+c*(W+10), 40+r*(H+10)), 'missing ' + shot, fill=(255,80,80))
    sheet.save(os.path.join(OUT, 'ds030_before_after_%s.png' % mode)); print(mode, 'ok')
