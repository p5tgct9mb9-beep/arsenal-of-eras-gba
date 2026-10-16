#!/usr/bin/env python3
# dspng.py DIR: convert every PPM shot in DIR to PNG (both screens, 256x384) and delete the PPM
import sys, os
from PIL import Image
d = sys.argv[1]
for f in os.listdir(d):
    if f.endswith('.ppm'):
        Image.open(os.path.join(d, f)).save(os.path.join(d, f[:-4] + '.png')); os.remove(os.path.join(d, f))
