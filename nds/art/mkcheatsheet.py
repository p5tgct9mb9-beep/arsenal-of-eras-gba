#!/usr/bin/env python3
"""mkcheatsheet.py OUT.png [VERSION] - DS controls cheat sheet, built from the button tables in source/controls.c."""
import re, sys, os
from PIL import Image, ImageDraw, ImageFont
H = os.path.dirname(os.path.abspath(__file__)); SRC = open(os.path.join(H, '..', 'source', 'controls.c')).read()
rows = re.findall(r'\{\{[^}]*\},\{("[^}]*")\},"([A-Z ]+)"\}', SRC)
LONG = {  # fuller wording for the sheet (same actions as the in-game legend)
 'INFANTRY': ['Fire', 'Gear: claymore etc.', 'Reload / take weapon', 'Switch weapon', 'Aim / strafe-lock', 'Grenade', 'Gear', 'Pause'],
 'CAMPAIGN': ['Fire', 'Gear', 'Switch weapon', 'Switch weapon', 'Aim / strafe-lock', 'Grenade', 'Gear', 'Pause'],
 'BOOT CAMP': ['Fire / next', 'Use', 'Use', 'Switch weapon', 'Aim / cover', 'Grenade', 'Use', 'Skip'],
 'ARMOR': ['Main gun', 'Coax MG / ATGM / rockets', 'Dismount squad / centre turret', 'Smoke screen (breaks locks)', 'Turret left', 'Turret right', 'Squad / centre', 'Pause'],
 'AIR': ['Guns', 'Missile / bombs', 'Scoreboard (hold)', 'Flares (breaks locks)', 'Barrel roll left', 'Barrel roll right', 'Scoreboard', 'Pause'],
 'HOVER': ['Guns', 'Missile', 'Climb (hold)', 'Descend (hold)', 'Strafe left', 'Strafe right', 'Altitude (+Up/Down)', 'Pause'],
 'NAVAL': ['Main guns', 'Secondary / torpedo', 'Centre turrets', 'Smoke screen (breaks locks)', 'Turrets left', 'Turrets right', 'Centre', 'Pause'],
 'FPS': ['Fire', 'Fast turn (hold)', 'Reload / open door', 'Switch weapon', 'Strafe left', 'Strafe right', 'Switch weapon', 'Quit'],
 'MENU': ['Confirm', 'Back', 'Option / tips / mode', 'Start / loadout', 'Tab / era left', 'Tab / era right', 'Option', 'Start'],
 'MODE': ['A action', 'B action', 'SELECT action', 'SELECT action', 'L', 'R', 'SELECT', 'Pause / start'],
 'ARCADE': ['A', 'B', 'Same as A', 'Same as B', 'L (flipper / turn)', 'R (flipper / turn)', '-', 'Pause'],
 'CABINET': ['Play', 'Back', 'Play', 'Back', 'Previous game', 'Next game', '-', 'Play'],
}
ORDER = ['INFANTRY', 'CAMPAIGN', 'ARMOR', 'AIR', 'HOVER', 'NAVAL', 'FPS', 'BOOT CAMP', 'MENU', 'CABINET', 'ARCADE', 'MODE']
NAMES = {'INFANTRY': 'Infantry 6v6', 'CAMPAIGN': 'Campaign', 'ARMOR': 'Armor', 'AIR': 'Dogfight (jets/props)', 'HOVER': 'Helicopters / drones', 'NAVAL': 'Naval',
         'FPS': 'FPS missions', 'BOOT CAMP': 'Boot Camp (tutorial)', 'MENU': 'Menus', 'CABINET': 'Arcade cabinet', 'ARCADE': 'Arcade games', 'MODE': 'Other modes'}
have = {n for _, n in rows}
ver = sys.argv[2] if len(sys.argv) > 2 else ''
F = lambda s, b=False: ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans%s.ttf' % ('-Bold' if b else ''), s)
BTN = ['A', 'B', 'X', 'Y', 'L', 'R', 'SELECT', 'START']
COL = {'A': (220, 60, 60), 'B': (240, 190, 50), 'X': (70, 130, 230), 'Y': (60, 170, 90), 'L': (120, 120, 140), 'R': (120, 120, 140), 'SELECT': (90, 90, 110), 'START': (90, 90, 110)}
W = 1800; rowh = 58; top = 300; Hh = top + rowh * len(ORDER) + 150
im = Image.new('RGB', (W, Hh), (12, 18, 34)); d = ImageDraw.Draw(im)
d.rectangle([0, 0, W, 8], fill=(236, 196, 92))
d.text((40, 30), 'ARSENAL OF ERAS  -  NINTENDO DS / DSi CONTROLS', font=F(46, True), fill=(236, 196, 92))
d.text((40, 92), 'Native layout (Settings > Controls: DS, the default).  CLASSIC (GBA) keeps the v1.3.5 layout, with X as SELECT.  %s' % ver, font=F(24), fill=(170, 190, 220))
# mini DSi
x0, y0 = 40, 140
d.rounded_rectangle([x0, y0, x0 + 520, y0 + 140], 24, fill=(40, 46, 60), outline=(120, 140, 170), width=3)
d.rectangle([x0 + 170, y0 + 18, x0 + 350, y0 + 122], fill=(20, 30, 50), outline=(90, 210, 240), width=2); d.text((x0 + 205, y0 + 58), 'TOUCH', font=F(22, True), fill=(90, 210, 240))
for (bx, by, b) in [(x0 + 450, y0 + 30, 'X'), (x0 + 480, y0 + 62, 'A'), (x0 + 420, y0 + 62, 'Y'), (x0 + 450, y0 + 94, 'B')]:
    d.ellipse([bx - 16, by - 16, bx + 16, by + 16], fill=COL[b]); d.text((bx - 8, by - 13), b, font=F(22, True), fill=(255, 255, 255))
d.rectangle([x0 + 60, y0 + 45, x0 + 80, y0 + 95], fill=(20, 20, 26)); d.rectangle([x0 + 45, y0 + 60, x0 + 95, y0 + 80], fill=(20, 20, 26))
d.rounded_rectangle([x0 + 10, y0 - 12, x0 + 110, y0 + 6], 6, fill=COL['L']); d.text((x0 + 52, y0 - 13), 'L', font=F(16, True), fill='white')
d.rounded_rectangle([x0 + 410, y0 - 12, x0 + 510, y0 + 6], 6, fill=COL['R']); d.text((x0 + 452, y0 - 13), 'R', font=F(16, True), fill='white')
d.text((x0 + 395, y0 + 116), 'START  SELECT', font=F(13), fill=(200, 200, 210))
d.multiline_text((600, 140), 'D-pad: move / steer / aim (all modes) and menu navigation.\nTouch screen: tap tiles and lists in menus; in battle the bottom screen shows the\nlive tactical map, the button legend for the current mode and touch action buttons.\nArcade: bottom screen shows the game card, live score, HI score, PAUSE.',
                 font=F(22), fill=(220, 228, 240), spacing=8)
# table
cx = [40, 380] + [380 + i * 177 for i in range(1, 8)]
d.rectangle([30, top - 44, W - 30, top - 6], fill=(18, 34, 62))
d.text((cx[0] + 10, top - 38), 'MODE', font=F(22, True), fill=(90, 210, 240))
for i, b in enumerate(BTN):
    x = 380 + i * 177; d.rounded_rectangle([x + 4, top - 40, x + 60 + (40 if len(b) > 1 else 0), top - 10], 8, fill=COL[b]); d.text((x + 14, top - 38), b, font=F(20, True), fill='white')
for r, n in enumerate(ORDER):
    if n not in have: continue
    y = top + r * rowh; d.rectangle([30, y, W - 30, y + rowh - 4], fill=(22, 36, 64) if r % 2 else (16, 28, 52))
    d.text((cx[0] + 10, y + 16), NAMES[n], font=F(22, True), fill=(236, 196, 92))
    for i, t in enumerate(LONG[n]):
        x = 380 + i * 177; words = t.split(' '); lines = ['']
        for w in words:
            if len(lines[-1]) + len(w) > 15: lines.append(w)
            else: lines[-1] = (lines[-1] + ' ' + w).strip()
        d.multiline_text((x + 6, y + (16 if len(lines) == 1 else 4)), '\n'.join(lines[:2]), font=F(17), fill=(235, 240, 248), spacing=2)
yb = top + rowh * len(ORDER) + 20
d.multiline_text((40, yb), 'Smoke (armor/naval) and flares (aircraft) break incoming missile locks; 12 s / 5 s reload.\nInfantry X: takes the weapon at your feet, otherwise reloads early.  Old GBA chords (SELECT+UP/DOWN altitude) are on X/Y.\nDebug/test chords (L+R+SELECT) still work in CLASSIC (GBA) mode.',
                 font=F(21), fill=(170, 190, 220), spacing=8)
im.save(sys.argv[1]); print('wrote', sys.argv[1], im.size)
