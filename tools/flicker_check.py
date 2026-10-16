#!/usr/bin/env python3
# flicker_check.py dir : for every captured frame count ISS solar-array pixels and rocket exhaust/flame pixels in the sky
# (outside the menu panels). A frame counts as BLINK when an object that is on screen shows < 85% of the pixels it shows
# in the neighbouring frames (min of the previous and next frame), i.e. it was partly or fully erased when the frame was scanned out.
import sys,glob
from PIL import Image
def near(p,c,t):return abs(p[0]-c[0])<=t and abs(p[1]-c[1])<=t and abs(p[2]-c[2])<=t
ISS=[(200,150,60)];RK=[(255,170,60),(255,250,210),(255,236,180),(230,230,220),(170,176,190)]
d=sys.argv[1];fs=sorted(glob.glob(d+'/f*.png'));R=[]
for f in fs:
  im=Image.open(f).convert('RGB');sc=im.size[0]//240;px=im.load();a=b=0
  for y in range(160):
    for x in range(240):
      if (4<=x<156 and 30<=y<136) or (4<=x<236 and 138<=y<158):continue
      p=px[x*sc,y*sc]
      if near(p,ISS[0],10):a+=1
      elif any(near(p,c,6) for c in RK):b+=1
  R.append((f.split('/')[-1],a,b))
def blinks(i):
  v=[r[i] for r in R];n=0;on=0
  for k,x in enumerate(v):
    if x>=4 or max(v[max(0,k-1):k+2])>=4:on+=1
    if 0<k<len(v)-1:
      ref=min(v[k-1],v[k+1])  # a dip below BOTH neighbours = the object vanished/partly erased for one scan-out
      if ref>=4 and x<0.85*ref:n+=1
  return on,n
for i,nm in ((1,'ISS   '),(2,'ROCKET')):
  on,n=blinks(i);print(f'{nm}: on screen in {on} frames, blink/partial frames: {n}')
open(d+'/flicker.csv','w').write('\n'.join(f'{a},{b},{c}' for a,b,c in R))
