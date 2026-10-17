#!/usr/bin/env python3
"""vehsheet.py OUT.png [bank] - before/after sheet of the top-down vehicle sprites (GBA original vs veh_ds.patch)."""
import sys, re, os
import numpy as np
from PIL import Image, ImageDraw
H=os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0,H); ROOT=os.path.dirname(os.path.dirname(H))
import veh_ds
def load(names):
    out={}
    for l in open(os.path.join(ROOT,'src','assets.c')):
        m=re.match(r'const (.+?) (\w+)\[(\d+)\].*?=\{(.*)\};',l)
        if m and m.group(2) in names: out[m.group(2)]=[int(v,0) for v in m.group(4).split(',') if v.strip()]
    return out
def rgb(c):return ((c&31)*255//31,((c>>5)&31)*255//31,((c>>10)&31)*255//31)
A=load({'tank_spr','plane_spr','ship_spr','obj_team_pal'})
PAL=[rgb(c) for c in A['obj_team_pal'][:16]]
def img(g,sc):
    a=np.zeros((32,32,3),np.uint8)
    for v in range(1,16):a[g==v]=PAL[v]
    a[g==0]=(70,90,60)
    return Image.fromarray(a).resize((32*sc,32*sc),Image.NEAREST)
def sheet(bank,out,sc=3,cols=16,maxn=64):
    w=A[bank];new=veh_ds.patch(bank,w);stride,n=veh_ds.BANKS[bank];n=min(n,maxn)
    parts=[0,128] if bank=='tank_spr' else [0]
    cw=32*sc+4;rows=(n+cols-1)//cols
    S=Image.new('RGB',(cols*cw*2+20,rows*cw*len(parts)+20),(20,22,28))
    for i in range(n):
        for pi,po in enumerate(parts):
            for k,src in enumerate((w,new)):
                g=veh_ds.grids(src,1,stride,i*stride+po)[0]
                S.paste(img(g,sc),(k*(cols*cw+20)+(i%cols)*cw,(i//cols)*cw*len(parts)+pi*cw))
    S.save(out)
if __name__=='__main__':
    for b in ('tank_spr','plane_spr','ship_spr'):sheet(b,sys.argv[1]+'_'+b+'.png')
def solo(bank,ids,out,sc=5,cols=8,fn=None,label=True):
    """one sheet: for each id hull+turret composited (tanks) or sprite; fn(bank,words)->words for the after view"""
    w=A[bank];stride=veh_ds.BANKS[bank][0];src=fn(bank,w) if fn else w
    cw=32*sc+6;rows=(len(ids)+cols-1)//cols
    S=Image.new('RGB',(cols*cw,rows*(cw+12)),(20,22,28));d=ImageDraw.Draw(S)
    for k,i in enumerate(ids):
        g=veh_ds.grids(src,1,stride,i*stride)[0].copy()
        if bank=='tank_spr':
            t=veh_ds.grids(src,1,stride,i*stride+128)[0];g[t>0]=t[t>0]
        x,y=(k%cols)*cw,(k//cols)*(cw+12);S.paste(img(g,sc),(x,y+12))
        if label:d.text((x+2,y),str(i)+' '+NAMES.get(bank,{}).get(i,'')[:16],fill=(230,220,150))
    S.save(out)
def _names():
    import re
    s=open(os.path.join(ROOT,'src','vehdb.inc')).read();N={}
    N['tank_spr']={int(g):n for n,g in re.findall(r'\{"([^"]+)",\d+,\d+,\d+,\d+,\d+,\d+,VC_\w+,(\d+)',s)}
    return N
NAMES=_names()
try:
    import veh_air; NAMES['plane_spr']=veh_air.names()
except Exception: pass
def hd(bank,w):
    import veh_hd, veh_air
    if bank=='tank_spr': return veh_hd.patch_tanks(w,NAMES['tank_spr'])
    if bank=='plane_spr': return veh_air.patch_planes(w,NAMES['plane_spr'])
    return veh_ds.patch(bank,w)
def ba(bank,ids,out,sc=5,cols=8):
    """before (GBA) / after (DS HD) pairs side by side"""
    a=out+'.a.png';b=out+'.b.png';solo(bank,ids,a,sc,cols);solo(bank,ids,b,sc,cols,fn=hd)
    A_,B_=Image.open(a),Image.open(b);S=Image.new('RGB',(A_.width*2+16,A_.height+18),(20,22,28));d=ImageDraw.Draw(S)
    S.paste(A_,(0,18));S.paste(B_,(A_.width+16,18));d.text((4,3),'BEFORE (GBA sprites)',fill=(240,200,120));d.text((A_.width+20,3),'AFTER (DS v0.3 rebuilt)',fill=(120,240,160))
    S.save(out);os.remove(a);os.remove(b)
