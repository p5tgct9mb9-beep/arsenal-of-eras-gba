#!/usr/bin/env python3
# v1.3 ANTI-AIRCRAFT sprites -> src/aadb.inc (4bpp OBJ tiles, 1D mapping) + preview sheet
import sys
from PIL import Image
PAL=[(0,0,0),(24,22,30),(70,74,82),(118,122,130),(176,180,188),(62,72,44),(104,114,70),(186,160,110),(110,80,50),(196,48,40),(240,206,60),(244,244,244),(110,170,220),(36,36,40),(250,140,40),(214,196,150)]
def G(w,h):return [[0]*w for _ in range(h)]
def P(g,x,y,c):
    if 0<=y<len(g) and 0<=x<len(g[0]):g[y][x]=c
def R(g,x0,y0,x1,y1,c):
    for y in range(min(y0,y1),max(y0,y1)+1):
        for x in range(min(x0,x1),max(x0,x1)+1):P(g,x,y,c)
def E(g,x0,y0,x1,y1,c):
    cx=(x0+x1)/2;cy=(y0+y1)/2;rx=max(.5,(x1-x0)/2+.5);ry=max(.5,(y1-y0)/2+.5)
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if ((x-cx)/rx)**2+((y-cy)/ry)**2<=1.0:P(g,x,y,c)
def L(g,x0,y0,x1,y1,c):
    n=max(abs(x1-x0),abs(y1-y0),1)
    for i in range(n+1):P(g,round(x0+(x1-x0)*i/n),round(y0+(y1-y0)*i/n),c)
def outl(g):
    h=len(g);w=len(g[0]);o=[r[:] for r in g]
    for y in range(h):
        for x in range(w):
            if g[y][x]==0 and any(0<=y+dy<h and 0<=x+dx<w and g[y+dy][x+dx] not in (0,1) for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=1
    return o
# ---- enemies, 32x16, facing right; f = anim frame (props/rotors)
def biplane(f,col=6):
    g=G(32,16);R(g,6,7,24,9,col);E(g,22,6,27,10,col);R(g,3,5,7,8,col);R(g,2,4,4,5,col)   # body + tail
    R(g,10,3,22,4,7);R(g,10,11,22,12,7);L(g,13,4,13,11,8);L(g,19,4,19,11,8)              # wings + struts
    R(g,28,4+f*2,28,11-f*2,13);E(g,14,6,16,7,13);return g
def gotha(f):
    g=G(32,16);R(g,4,7,26,9,15);E(g,24,6,29,10,15);R(g,1,4,6,8,15);R(g,6,2,26,3,7);R(g,6,12,26,13,7)
    for x in (10,20):L(g,x,3,x,12,8)
    E(g,12,9,15,11,2);E(g,19,9,22,11,2);R(g,30,6+f,30,9-f,13);return g
def zeppelin(f):
    g=G(32,16);E(g,1,3,30,12,4);R(g,4,7,28,7,3);E(g,1,3,30,5,11);R(g,1,4,3,11,3);R(g,12,12,19,14,2);P(g,14,13,12);P(g,17,13,12);return g
def fighter(f,col=5,stripe=10):
    g=G(32,16);E(g,4,6,27,10,col);E(g,24,6,30,10,col);R(g,3,3,7,7,col);R(g,12,10,20,11,col);E(g,16,5,20,7,12);R(g,24,7,27,7,stripe);R(g,31,5+f,31,11-f,13);return g
def stuka(f):
    g=G(32,16);E(g,4,6,27,10,3);E(g,24,6,30,10,3);R(g,3,3,7,7,3);L(g,12,9,16,12,3);L(g,16,12,20,10,3);E(g,16,5,20,7,12);R(g,17,12,19,14,13);R(g,31,5+f,31,11-f,13);return g
def heavy(f):
    g=G(32,16);E(g,2,6,28,10,3);E(g,24,5,31,10,4);R(g,1,2,5,7,3);R(g,8,10,22,11,3);E(g,26,6,30,8,12)
    for x in (12,18):E(g,x,9,x+3,12,2);R(g,x+4,9+f,x+4,12-f,13)
    return g
def jet(f,col=3,canopy=12):
    g=G(32,16);R(g,3,7,28,9,col);L(g,28,8,31,8,col);E(g,20,6,25,8,canopy);L(g,10,9,18,13,col);R(g,10,12,14,13,col);L(g,3,7,6,2,col);R(g,3,3,6,7,col)
    R(g,0,7+f,2,9-f,14);return g
def attackjet(f):
    g=jet(f,5);E(g,12,9,15,11,2);E(g,17,9,20,11,2);return g
def heli(f,col=5,kind=0):
    g=G(32,16);E(g,13,6,27,12,col);R(g,2,8,14,9,col);R(g,1,5,3,9,col);E(g,22,7,26,10,12)
    R(g,6,3,30,3,13) if f==0 else R(g,12,3,24,3,13)
    L(g,19,4,19,6,13)
    if kind==0:R(g,14,12,24,13,2);P(g,28,11,2)   # hind: stub wings / gun
    else:R(g,16,12,22,12,2);R(g,26,12,28,12,13)
    if f==0:R(g,0,4,0,8,13)
    else:R(g,0,6,4,6,13)
    return g
def shahed(f):
    g=G(32,16);E(g,8,7,26,9,15);L(g,26,8,29,8,15);L(g,8,8,14,2,15);L(g,8,8,14,14,15);L(g,14,2,16,8,15);L(g,14,14,16,8,15);R(g,6,7+f,6,9-f,13);return g
def lancet(f):
    g=G(32,16);R(g,10,7,26,8,3);L(g,26,7,29,7,3);R(g,12,4,13,11,4);R(g,22,5,23,10,4);R(g,8,6+f,8,9-f,13);return g
def orlan(f):
    g=G(32,16);E(g,12,6,22,9,4);R(g,4,8,12,8,4);R(g,14,5,18,5,4);R(g,8,4,26,4,4) if False else None;R(g,7,7,26,7,4);R(g,2,5,4,9,4);R(g,23,4+f,23,10-f,13);E(g,18,9,20,10,13);return g
def fpv(f):
    g=G(32,16);R(g,10,8,22,9,13);R(g,14,6,18,10,2);E(g,15,7,17,8,9)
    for x in (9,23):R(g,x-3,5 if f else 6,x+3,5 if f else 6,3);R(g,x,6,x,8,13)
    return g
def mq9(f):
    g=G(32,16);E(g,6,6,26,9,4);E(g,20,5,27,8,4);R(g,4,2,6,6,4);L(g,4,9,7,13,4);R(g,8,8,24,8,3);R(g,1,6+f,2,9-f,13);E(g,12,9,14,11,2);return g
def cruise(f):
    g=G(32,16);R(g,6,7,27,9,3);E(g,24,7,30,9,3);R(g,14,9,18,10,3);R(g,4,5,7,7,3);R(g,1,7,5,9,14 if f else 10);return g
EN=[('BIPLANE FIGHTER',lambda f:biplane(f,6)),('GOTHA BOMBER',gotha),('ZEPPELIN',zeppelin),
    ('FIGHTER',lambda f:fighter(f,5,10)),('DIVE BOMBER',stuka),('HEAVY BOMBER',heavy),
    ('JET FIGHTER',lambda f:jet(f,3)),('ATTACK JET',attackjet),('MI-24 HIND',lambda f:heli(f,5,0)),('AH-1 COBRA',lambda f:heli(f,6,1)),
    ('SU-25',lambda f:jet(f,7)),('AH-64 APACHE',lambda f:heli(f,2,1)),('SHAHED-136',shahed),('LANCET',lancet),('ORLAN-10',orlan),('FPV QUAD',fpv),('MQ-9 REAPER',mq9),('CRUISE MISSILE',cruise),
    ('ZERO FIGHTER',lambda f:fighter(f,15,9)),('MIG-21',lambda f:jet(f,4))]
# ---- small fx, 16x16
def burst(k):
    g=G(16,16);r=[3,6,7][k];E(g,8-r,8-r,8+r,8+r,[14,10,2][k]);
    if k<2:E(g,8-r+2,8-r+2,8+r-2,8+r-2,[10,11,3][k])
    if k==2:
        for (x,y) in((3,4),(11,3),(12,11),(4,12),(8,8)):E(g,x-1,y-1,x+1,y+1,3)
    return g
def boom(k):
    g=G(16,16);r=[4,7,7][k];E(g,8-r,8-r,8+r,8+r,[10,14,13][k]);E(g,8-r//2,8-r//2,8+r//2,8+r//2,[11,10,2][k]);return g
# ---- 8x8 fx
def tiny(kind):
    g=G(8,8)
    if kind=='tracer':R(g,3,3,4,4,10);P(g,2,3,14)
    elif kind=='shell':R(g,3,3,4,4,11)
    elif kind=='bomb':E(g,2,2,5,6,13);R(g,3,1,4,1,2)
    elif kind=='missile':R(g,3,2,4,5,11);P(g,3,6,14);P(g,4,6,14)
    elif kind=='flare':E(g,2,2,5,5,10);P(g,3,3,11)
    elif kind=='smoke':E(g,1,1,6,6,3);E(g,2,2,5,5,4)
    elif kind=='ebul':R(g,3,3,4,4,9)
    elif kind=='lead':L(g,0,3,2,3,10);L(g,5,3,7,3,10);L(g,3,0,3,2,10);L(g,3,5,3,7,10)
    elif kind=='rocket':R(g,2,3,5,4,4);P(g,1,3,14);P(g,1,4,14)
    return g
def lockbox(k):
    g=G(16,16);c=9 if k else 10
    for (x,y,dx,dy) in((0,0,1,1),(15,0,-1,1),(0,15,1,-1),(15,15,-1,-1)):
        for i in range(4):P(g,x+dx*i,y,c);P(g,x,y+dy*i,c)
    return g
def tiles(g,w,h):
    out=[]
    for ty in range(h//8):
        for tx in range(w//8):
            for y in range(8):
                v=0
                for x in range(8):v|=(g[ty*8+y][tx*8+x]&15)<<(4*x)
                out.append(v)
    return out
T=[];idx={};sheet=[]
def add(name,g,w,h):
    idx[name]=len(T)//8;T.extend(tiles(outl(g) if w>8 else g,w,h));sheet.append((name,outl(g) if w>8 else g))
for i,(n,fn) in enumerate(EN):
    for f in range(2):add('E%d_%d'%(i,f),fn(f),32,16)
for k in range(3):add('BURST%d'%k,burst(k),16,16)
for k in range(3):add('BOOM%d'%k,boom(k),16,16)
for k in range(2):add('LOCK%d'%k,lockbox(k),16,16)
for k in ('tracer','shell','bomb','missile','flare','smoke','ebul','lead','rocket'):add(k.upper(),tiny(k),8,8)
assert len(T)//8<=512,len(T)//8
def rgb15(c):return (c[0]>>3)|((c[1]>>3)<<5)|((c[2]>>3)<<10)
o=["// generated by tools/gen_aa.py - do not edit","#define AA_NEN %d"%len(EN)]
o.append("static const char*const AA_EN[AA_NEN]={"+",".join('"%s"'%n for n,_ in EN)+"};")
o.append("static const u16 aa_pal[16]={"+",".join(str(rgb15(c)) for c in PAL)+"};")
for k,v in idx.items():
    if not (k[0]=='E' and k[1].isdigit()):o.append("#define AT_%s %d"%(k,v*1))
o.append("#define AT_EN0 0 // enemy i frame f at tile (i*2+f)*8")
o.append("static const u32 aa_tiles[%d]={%s};"%(len(T),",".join("0x%08X"%v for v in T)))
open('src/aadb.inc','w').write("\n".join(o)+"\n")
if len(sys.argv)>1:
    W=Image.new('RGB',(34*8*4,18*((len(sheet)+7)//8)*4),(60,90,140))
    for i,(n,g) in enumerate(sheet):
        for y,row in enumerate(g):
            for x,c in enumerate(row):
                if c:
                    for a in range(4):
                        for b in range(4):W.putpixel(((i%8)*34*4+x*4+a,(i//8)*18*4+y*4+b),PAL[c])
    W.save(sys.argv[1])
print("aa tiles",len(T)//8)
