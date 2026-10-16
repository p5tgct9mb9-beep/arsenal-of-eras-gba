#!/usr/bin/env python3
"""mkicon.py - draws the 32x32, 16-colour DS menu icon (icon.gif): Earth with a jet crossing it."""
from PIL import Image, ImageDraw
import math, os
S = 8; W = 32 * S
im = Image.new('RGB', (W, W), (10, 16, 34)); d = ImageDraw.Draw(im)
d.ellipse((3*S, 3*S, 29*S, 29*S), fill=(30, 80, 170))
for (x, y, r) in ((11, 12, 5), (19, 19, 6), (21, 9, 3), (9, 21, 3)):
    d.ellipse(((x-r)*S, (y-r)*S, (x+r)*S, (y+r)*S), fill=(70, 150, 60))
d.ellipse((3*S, 3*S, 29*S, 29*S), outline=(150, 210, 250), width=S)
# jet silhouette, nose up-right
cx, cy, a = 16, 16, math.radians(-40)
def P(px, py): return (S*(cx + px*math.cos(a) - py*math.sin(a)), S*(cy + px*math.sin(a) + py*math.cos(a)))
d.polygon([P(12, 0), P(4, -1.4), P(0, -10), P(-3, -10), P(-2, -1.6), P(-8, -1.6), P(-10, -5), P(-11.5, -5),
           P(-11, 0), P(-11.5, 5), P(-10, 5), P(-8, 1.6), P(-2, 1.6), P(-3, 10), P(0, 10), P(4, 1.4)], fill=(225, 228, 235))
d.line([P(-11, 0), P(11, 0)], fill=(236, 196, 92), width=S)
im = im.resize((32, 32), Image.LANCZOS).quantize(colors=16)
im.save(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'icon.gif'))
