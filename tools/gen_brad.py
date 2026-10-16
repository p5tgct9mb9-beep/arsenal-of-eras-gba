#!/usr/bin/env python3
# v1.3.1 SPEC OPS: BRADLEY overhaul - detailed sprites (4bpp OBJ, 1D), 3-layer parallax backdrops per world,
# 28 hand-scripted set-piece courses -> src/braddb.inc ; previews build/brad_sheet.png, build/brad_bg_*.png, build/brad_course_*.png
import sys,os,random,math
from PIL import Image
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# ---------------------------------------------------------------- palettes (OBJ)
# pal0 Bradley: 0 T 1 outline 2 track dark 3 track mid 4 steel light 5 base 6 base shadow 7 base highlight 8 metal dark
#               9 camo patch A 10 camo patch B 11 white 12 glass 13 black 14 orange 15 skin
BASE=[(0,0,0),(22,18,20),(44,42,46),(86,84,88),(168,168,172),(0,0,0),(0,0,0),(0,0,0),(58,60,56),(0,0,0),(0,0,0),(244,244,236),(120,190,230),(12,12,14),(250,150,40),(220,172,128)]
CAMO={'DESERT':[(206,180,122),(156,128,84),(234,214,162),(178,146,96),(222,198,140)],
      'WOODLAND':[(96,114,64),(62,74,42),(132,150,86),(104,80,50),(40,44,32)],
      'WINTER':[(224,228,232),(156,164,172),(250,252,252),(128,134,140),(178,184,190)]}
def campal(c):p=list(BASE);p[5],p[6],p[7],p[9],p[10]=c;return p
# pal1 enemy: same layout; 5-7 grey-green drab, 9 red 10 yellow
ENEM=list(BASE);ENEM[5:8]=[(122,128,98),(82,88,66),(160,166,130)];ENEM[9]=(214,52,40);ENEM[10]=(248,212,72)
# pal2 misc/friendly: 5-7 tan/olive friendly trucks & squad, 9 red cross/flag 10 yellow
MISC=list(BASE);MISC[5:8]=[(150,140,92),(104,96,60),(190,180,126)];MISC[9]=(220,40,40);MISC[10]=(250,220,60)
# pal4 burnt wreck (applied to the same tiles)
BURNT=[(0,0,0),(10,8,8),(30,26,24),(52,46,42),(84,74,66),(58,50,44),(36,30,28),(92,76,62),(30,28,26),(64,52,44),(46,40,36),(120,108,96),(40,40,44),(8,8,8),(232,110,30),(70,58,50)]
# pal6 hot fx: 1 dark 2 smoke dark 3 smoke mid 4 smoke light 5 dark red 6 red 7 orange 8 yellow 9 white-hot 10 brass 11 brass dark 12 laser 13 sand 14 snow 15 rain
HOT=[(0,0,0),(26,20,18),(56,52,50),(100,96,92),(158,154,148),(150,30,20),(226,64,22),(252,146,34),(255,224,70),(255,255,226),(230,190,70),(150,110,34),(255,36,36),(222,192,132),(244,248,255),(170,196,236)]
def rgb15(c):return (c[0]>>3)|((c[1]>>3)<<5)|((c[2]>>3)<<10)
# ---------------------------------------------------------------- drawing helpers
def G(w,h):return [[0]*w for _ in range(h)]
def P(g,x,y,c):
    x=int(round(x));y=int(round(y))
    if 0<=y<len(g) and 0<=x<len(g[0]):g[y][x]=c
def GP(g,x,y):return g[y][x] if 0<=y<len(g) and 0<=x<len(g[0]) else 0
def R(g,x0,y0,x1,y1,c):
    for y in range(min(y0,y1),max(y0,y1)+1):
        for x in range(min(x0,x1),max(x0,x1)+1):P(g,x,y,c)
def E(g,x0,y0,x1,y1,c):
    cx=(x0+x1)/2;cy=(y0+y1)/2;rx=max(.5,(x1-x0)/2+.5);ry=max(.5,(y1-y0)/2+.5)
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if ((x-cx)/rx)**2+((y-cy)/ry)**2<=1.0:P(g,x,y,c)
def C(g,cx,cy,r,c):
    for y in range(int(cy-r-1),int(cy+r+2)):
        for x in range(int(cx-r-1),int(cx+r+2)):
            if (x-cx)**2+(y-cy)**2<=r*r:P(g,x,y,c)
def L(g,x0,y0,x1,y1,c):
    n=max(abs(x1-x0),abs(y1-y0),1)
    for i in range(n+1):P(g,round(x0+(x1-x0)*i/n),round(y0+(y1-y0)*i/n),c)
def POLY(g,pts,c):
    ys=[p[1] for p in pts]
    for y in range(min(ys),max(ys)+1):
        xs=[]
        for i in range(len(pts)):
            (x0,y0),(x1,y1)=pts[i],pts[(i+1)%len(pts)]
            if (y0<=y<y1) or (y1<=y<y0):xs.append(x0+(y-y0)*(x1-x0)/(y1-y0))
        xs.sort()
        for a,b in zip(xs[::2],xs[1::2]):
            for x in range(int(round(a)),int(round(b))+1):P(g,x,y,c)
def outl(g,oc=1):
    h=len(g);w=len(g[0]);o=[r[:] for r in g]
    for y in range(h):
        for x in range(w):
            if g[y][x]==0 and any(0<=y+dy<h and 0<=x+dx<w and g[y+dy][x+dx] not in (0,oc) for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=oc
    return o
def recol(g,frm,to,x0=0,y0=0,x1=999,y1=999):
    for y in range(max(0,y0),min(len(g),y1+1)):
        for x in range(max(0,x0),min(len(g[0]),x1+1)):
            if g[y][x] in frm:g[y][x]=to
def patches(g,x0,y0,x1,y1,seed,cols=(9,10),n=None,on=(5,)):
    r=random.Random(seed)
    for _ in range(n or (x1-x0)*(y1-y0)//40):
        cx=r.randint(x0,x1);cy=r.randint(y0,y1);rx=r.randint(2,4);ry=r.randint(1,2);c=r.choice(cols)
        for y in range(cy-ry,cy+ry+1):
            for x in range(cx-rx,cx+rx+1):
                if ((x-cx)/(rx+.5))**2+((y-cy)/(ry+.5))**2<=1 and GP(g,x,y) in on:P(g,x,y,c)
def shade_lr(g,x0,y0,x1,y1):
    # bevel: top-left edges highlight, bottom-right edges shadow (only base colour)
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if GP(g,x,y)==5:
                if GP(g,x,y-1) in (0,1) :P(g,x,y,7)
                elif GP(g,x,y+1) in (0,1):P(g,x,y,6)
def wheel(g,cx,cy,r,f,rim=3,hub=8,spoke=4):
    C(g,cx,cy,r,rim);C(g,cx,cy,r-1.2,rim)
    for a in range(-1,2):P(g,cx+a,cy-int(r),4)
    C(g,cx,cy,max(1,r-2.2),hub);P(g,cx,cy,spoke)
    an=f*math.pi/4
    P(g,round(cx+math.cos(an)*(r-1.3)),round(cy+math.sin(an)*(r-1.3)),2)
    P(g,round(cx-math.cos(an)*(r-1.3)),round(cy-math.sin(an)*(r-1.3)),2)
def track_belt(g,x0,x1,ytop,ybot,f,nwheels,wr,sprocket_front=True,rollers=3):
    # belt
    POLY(g,[(x0+3,ytop),(x1-3,ytop),(x1,ytop+3),(x1-2,ybot),(x0+3,ybot),(x0,ytop+4)],2)
    R(g,x0+3,ytop+1,x1-3,ybot-1,13)
    cy=ybot-wr
    sp=(x1-x0-2*wr-6)/(nwheels-1)
    for i in range(nwheels):
        cx=round(x0+wr+3+i*sp);wheel(g,cx,cy,wr,f)
    # sprocket + idler
    sx=x1-3 if sprocket_front else x0+3
    C(g,sx,ytop+3,2.6,8);P(g,sx,ytop+3,4)
    for k in range(4):
        a=(f*math.pi/8)+k*math.pi/2;P(g,round(sx+math.cos(a)*3),round(ytop+3+math.sin(a)*3),3)
    ix=x0+3 if sprocket_front else x1-3
    C(g,ix,ytop+3,2.4,3);P(g,ix,ytop+3,8)
    for k in range(rollers):P(g,round(x0+(x1-x0)*(k+1)/(rollers+1)),ytop+1,3)
    # moving links
    for x in range(x0+3,x1-2):
        if (x+f)%4==0:P(g,x,ytop,3)
        if (x-f)%4==0:P(g,x,ybot,3)
# ---------------------------------------------------------------- Bradley family (64x32, facing right; ground y=31; barrel pivot (45,7))
VAR=['M2A2 ODS','M2A3','M2A4','M3A3 CFV','M7 BFIST']
def bradley(v,f):
    g=G(64,32)
    track_belt(g,1,62,21,31,f,6,4)
    # side skirt armour
    R(g,4,16,58,21,5);L(g,4,16,58,16,7);L(g,4,21,58,21,6)
    for x in range(12,57,9):L(g,x,17,x,20,6);P(g,x+2,17,7)
    if v==2:   # A4: bolted add-on armour tiles
        for x in range(5,56,5):R(g,x,17,x+3,20,6);R(g,x,17,x+3,17,7);P(g,x+1,18,7)
    # upper hull + glacis + rear ramp
    POLY(g,[(3,10),(44,10),(61,15),(61,17),(3,17)],5)
    L(g,3,10,44,10,7);L(g,44,10,61,15,7);L(g,45,11,60,15,7)
    L(g,3,15,58,15,6)
    R(g,2,11,3,17,6);P(g,2,17,8);P(g,3,12,8)
    R(g,47,12,52,13,8);L(g,47,12,52,12,3)                       # engine grille (front right)
    R(g,49,10,51,10,8);P(g,50,10,12)                            # driver periscopes
    P(g,60,15,11);P(g,59,16,14)                                 # headlight / blackout light
    if v!=3 and v!=4:
        for x in (14,23,32):C(g,x,13,1.2,8);P(g,x,13,13);R(g,x-1,11,x+1,11,12)   # firing ports + vision blocks
    else:
        R(g,10,12,18,14,8);R(g,11,12,17,12,3);R(g,22,12,30,14,8);R(g,23,12,29,12,3) # scout stowage boxes
    # turret
    POLY(g,[(18,10),(18,4),(22,2),(37,2),(44,6),(44,10)],5)
    L(g,22,2,37,2,7);L(g,37,2,44,6,7);L(g,19,4,19,9,7)
    L(g,18,9,44,9,6);L(g,18,10,44,10,6)
    R(g,13,4,17,9,8);[P(g,x,y,6) for x in range(13,18) for y in range(4,10) if (x+y)%2==0]   # bustle rack
    R(g,43,4,46,9,8);L(g,43,4,46,4,4);P(g,46,8,13)              # mantlet + coax port
    R(g,39,1,42,2,8);P(g,40,1,3);P(g,42,1,3)                    # smoke dischargers
    if v==4:   # M7 BFIST: FS3 sensor head, no TOW
        R(g,12,0,20,5,8);R(g,13,0,19,0,3);R(g,17,2,19,3,12);P(g,18,2,11)
    else:      # TOW-2 launcher box on the left side
        R(g,8,1,26,6,6);L(g,8,1,26,1,7);R(g,24,1,26,6,8);P(g,25,2,3);P(g,25,5,3)
        L(g,14,1,14,6,8);L(g,20,1,20,6,8);R(g,9,6,25,6,8)
    if v>=1:R(g,29,0,34,2,8);R(g,33,0,34,1,12);P(g,30,0,3)     # CIV (A3+)
    else:R(g,29,1,33,2,8);P(g,31,1,3)                         # ODS commander hatch
    L(g,21,2,19,-2,13);L(g,24,2,24,-1,13)                       # whip antennas (base)
    if v==3:L(g,16,4,15,0,13)
    # camo patches (only on base colour so bevels stay readable)
    patches(g,3,1,61,21,v*13+5,n=34)
    return outl(g)
def era_overlay():
    g=G(64,32)
    for x in range(6,56,5):R(g,x,17,x+3,20,6);L(g,x,17,x+3,17,7);P(g,x+3,20,8)
    for x in range(21,41,5):R(g,x,5,x+3,8,6);L(g,x,5,x+3,5,7)
    return outl(g)
def aps_kit():
    g=G(16,16);R(g,2,8,6,12,8);R(g,3,7,5,7,4);R(g,9,9,13,13,8);R(g,10,8,12,8,4);P(g,4,10,12);P(g,11,11,12);return outl(g)
def barrel():
    # 32x32, pivot at centre (16,16), barrel to the right; vertically symmetric so left-facing rotation works
    g=G(32,32)
    R(g,16,15,29,16,8);L(g,16,15,29,15,4)
    R(g,27,14,31,17,8);L(g,28,14,30,14,3);P(g,31,15,13);P(g,31,16,13)    # muzzle brake
    R(g,16,14,19,17,8)
    return outl(g)
def turret_toss():
    g=G(32,16)
    POLY(g,[(4,12),(5,6),(10,3),(20,3),(26,7),(26,12)],5);L(g,10,3,20,3,7);L(g,4,12,26,12,6)
    R(g,26,8,31,9,8);R(g,13,1,17,3,8);R(g,8,8,22,9,6)
    return outl(g)
# ---------------------------------------------------------------- enemy armour (pal1, 64x32 facing right, ground y=31)
def mbt(f,kind=0):
    # kind 0 T-72B/T-80 style generic, 1 T-72B3 boss (Kontakt), 2 T-90M
    g=G(64,32)
    POLY(g,[(2,22),(59,22),(62,25),(59,31),(5,31),(1,26)],2);R(g,4,23,59,30,13)
    for i in range(6):wheel(g,9+i*9,26,4.4,f)
    C(g,59,25,2.5,8);C(g,4,25,2.3,3)
    for x in range(4,60):
        if (x+f)%4==0:P(g,x,22,3)
        if (x-f)%4==0:P(g,x,31,3)
    R(g,3,18,58,22,5);L(g,3,18,58,18,7);L(g,3,22,58,22,6)          # rubber side skirt
    for x in range(10,58,8):L(g,x,19,x,21,6)
    POLY(g,[(2,15),(46,15),(61,20),(58,22),(2,22)],5);L(g,2,15,46,15,7);L(g,46,15,61,20,7)
    E(g,2,13,8,18,8);E(g,3,14,7,17,6);L(g,5,13,5,18,8)             # rear fuel drums
    if kind==2:
        POLY(g,[(16,15),(18,8),(36,7),(46,10),(47,15)],5);L(g,18,8,36,7,7);L(g,36,7,46,10,7)
        for x in range(20,46,5):R(g,x,9,x+3,11,6);L(g,x,9,x+3,9,7)  # Relikt
        for x in range(2,15,3):L(g,x,9,x,16,8)                        # slat cage
        L(g,2,9,14,9,8)
        R(g,30,4,35,7,8);R(g,33,5,34,6,12)                             # commander sight
    else:
        E(g,16,8,44,17,5);L(g,22,8,36,8,7);L(g,17,11,19,9,7);L(g,16,16,44,16,6)
        if kind==1 or kind==0:
            for x in range(20,42,5):R(g,x,11,x+3,13,6);L(g,x,11,x+3,11,7)   # Kontakt bricks
        R(g,25,5,30,8,8);L(g,27,5,24,3,8)                               # cupola + NSVT
        R(g,12,9,16,13,8)                                               # snorkel stowage
    R(g,44,11,63,12,8);L(g,44,11,63,11,4);R(g,51,10,54,13,8);R(g,61,10,63,13,8)  # 125mm + fume extractor
    patches(g,2,6,61,22,77+kind,cols=(6,7),n=18)
    return outl(g)
def t14(f):
    g=G(64,32)
    POLY(g,[(1,22),(60,22),(63,25),(60,31),(4,31),(0,26)],2);R(g,3,23,60,30,13)
    for i in range(7):wheel(g,7+i*8.4,26,3.9,f)
    for x in range(3,61):
        if (x+f)%4==0:P(g,x,22,3)
        if (x-f)%4==0:P(g,x,31,3)
    R(g,2,17,60,22,5);L(g,2,17,60,17,7);L(g,2,22,60,22,6)
    for x in range(6,60,7):R(g,x,18,x+4,21,6)
    POLY(g,[(1,13),(44,13),(62,18),(62,17),(1,17)],5);L(g,1,13,44,13,7);L(g,44,13,62,17,7)
    POLY(g,[(16,13),(18,7),(40,7),(47,11),(47,13)],5);L(g,18,7,40,7,7);L(g,40,7,47,11,7);L(g,16,12,47,12,6)
    for x in (21,27,33):R(g,x,5,x+2,6,8);P(g,x+1,5,3)                 # Afganit launchers
    R(g,37,4,40,6,8);R(g,39,5,40,5,12)                                  # sight
    R(g,47,9,63,10,8);L(g,47,9,63,9,4);R(g,60,8,63,11,8)
    patches(g,2,6,61,22,91,cols=(6,7),n=20)
    return outl(g)
def bmp2(f):
    g=G(64,32)
    POLY(g,[(4,23),(56,23),(59,26),(56,31),(7,31),(3,27)],2);R(g,6,24,56,30,13)
    for i in range(6):wheel(g,10+i*8.6,27,3.6,f)
    for x in range(6,57):
        if (x+f)%4==0:P(g,x,23,3)
        if (x-f)%4==0:P(g,x,31,3)
    POLY(g,[(4,14),(40,14),(58,21),(56,24),(4,24)],5);L(g,4,14,40,14,7);L(g,40,14,58,21,7);L(g,4,23,56,23,6)
    for x in range(42,56,3):L(g,x,16+(x-42)//3,x+2,18+(x-42)//3,6)
    R(g,8,16,10,18,12);R(g,16,16,18,18,12)                               # firing ports
    POLY(g,[(20,14),(22,9),(34,9),(38,14)],5);L(g,22,9,34,9,7)
    R(g,36,10,58,10,8);R(g,52,9,55,11,8);R(g,24,6,33,8,8);P(g,33,7,9)   # 30mm + Konkurs
    patches(g,4,9,58,24,31,cols=(6,7),n=14)
    return outl(g)
def btr(f):
    g=G(64,32)
    POLY(g,[(3,13),(46,13),(61,19),(57,25),(3,25)],5);L(g,3,13,46,13,7);L(g,46,13,61,19,7);L(g,4,19,56,19,6)
    for i,x in enumerate((11,23,37,49)):
        C(g,x,26,4.6,2);C(g,x,26,2.4,3);P(g,x,26,8);a=f*math.pi/2+i;P(g,round(x+math.cos(a)*2),round(26+math.sin(a)*2),4)
    R(g,26,8,37,13,5);L(g,26,8,37,8,7);R(g,37,9,52,9,8);R(g,49,8,52,10,8)
    R(g,8,15,10,16,12);R(g,15,15,17,16,12);R(g,42,14,44,16,12)
    patches(g,3,8,60,24,41,cols=(6,7),n=14)
    return outl(g)
def technical(f):
    g=G(32,16)
    for x in (8,24):C(g,x,13,2.6,13);C(g,x,13,1.2,3);P(g,x+(1 if f else -1),13,4)
    R(g,2,8,30,11,5);L(g,2,8,30,8,7);R(g,18,4,27,8,5);R(g,21,5,26,7,12);L(g,18,4,27,4,7)
    R(g,6,3,8,8,8);L(g,8,3,15,2,8);R(g,4,1,6,3,15)
    return outl(g)
def helo(f,hind=False):
    g=G(64,32)
    if hind:
        POLY(g,[(8,13),(40,10),(56,13),(60,18),(54,22),(20,22),(8,17)],5)
        R(g,44,12,53,15,12);L(g,44,12,53,12,11);R(g,36,12,41,15,12)       # tandem bubble canopies
        for x in (24,29):R(g,x,15,x+2,17,12)                               # cabin windows
        R(g,22,20,40,21,6)
        R(g,22,18,38,19,8)                                                 # stub wing
        for x in (23,28,33):R(g,x,20,x+3,23,8);P(g,x+1,23,13)              # rocket pods
        R(g,56,19,58,21,8)                                                  # chin gun
        R(g,2,13,10,15,5);POLY(g,[(0,6),(4,6),(6,13),(2,14)],5)            # tail boom + fin
    else:
        POLY(g,[(10,14),(42,12),(56,15),(59,19),(52,22),(22,22),(10,17)],5)
        R(g,46,14,53,17,12);L(g,46,14,53,14,11);R(g,39,13,44,16,12)
        R(g,24,19,40,20,8);for_=0
        for x in (25,33):R(g,x,20,x+4,23,8);P(g,x+2,23,13)
        R(g,55,20,57,22,8)
        R(g,2,14,12,16,5);POLY(g,[(1,7),(5,7),(7,14),(3,15)],5)
    L(g,6,2 if hind else 3,6,4,8)
    shade_lr(g,0,0,63,31)
    R(g,30,6,33,10,8);R(g,26,8,37,9,8)                                     # engines/hub
    if f==0:L(g,2,5,61,5,3);L(g,10,4,52,4,4)
    else:L(g,12,5,52,5,3);L(g,24,4,40,4,4);P(g,4,5,3);P(g,60,5,3)
    C(g,3,11 if not hind else 10,2.5,3 if f else 4)                         # tail rotor blur
    for x in (26,40):P(g,x,24,8);P(g,x,25,13)                               # gear
    patches(g,10,8,58,22,57+hind,cols=(6,7),n=12)
    return outl(g)
def truck(f,friendly=True):
    # HEMTT / Ural - 64x32
    g=G(64,32)
    for x in ((10,22,48) if friendly else (12,24,50)):C(g,x,27,4.4,13);C(g,x,27,2.4,3);P(g,x,27,8);a=f*math.pi/2;P(g,round(x+math.cos(a)*2),round(27+math.sin(a)*2),4)
    R(g,3,20,60,23,8);
    if friendly:
        R(g,2,9,42,20,5);L(g,2,9,42,9,7);L(g,2,20,42,20,6);[L(g,x,10,x,19,6) for x in range(8,42,8)]  # cargo box
        R(g,44,10,60,22,5);R(g,48,11,57,15,12);L(g,44,10,60,10,7);R(g,58,17,61,21,8);P(g,60,18,11)
        R(g,18,12,26,17,11);L(g,22,12,22,17,9);L(g,18,14,26,14,9)          # red cross / ID panel
    else:
        R(g,2,11,40,20,5);POLY(g,[(2,11),(6,6),(36,6),(40,11)],6);L(g,6,6,36,6,7)  # tarp
        R(g,42,10,58,22,5);R(g,46,11,55,15,12);POLY(g,[(56,14),(62,18),(62,22),(56,22)],5);L(g,42,10,58,10,7)
    return outl(g)
def car(f,c2=0):
    g=G(32,16)
    for x in (7,24):C(g,x,13,2.6,13);C(g,x,13,1.1,3)
    POLY(g,[(1,8),(8,7),(11,3),(22,3),(26,7),(31,8),(31,12),(1,12)],5);L(g,11,3,22,3,7);L(g,1,8,31,8,7);L(g,1,12,31,12,6)
    POLY(g,[(12,4),(16,4),(16,7),(10,7)],12);POLY(g,[(18,4),(21,4),(24,7),(18,7)],12)
    P(g,31,9,10);P(g,1,9,9)
    return outl(g)
def atgm_nest():
    g=G(32,16)
    for i,x in enumerate(range(1,31,5)):E(g,x,10-(1 if 0<i<5 else 0),x+5,15,5);L(g,x+1,10-(1 if 0<i<5 else 0),x+4,10-(1 if 0<i<5 else 0),7)
    L(g,14,9,10,3,8);L(g,14,9,18,3,8);R(g,9,1,22,3,8);R(g,21,1,23,3,6);P(g,22,2,9)   # tripod + Kornet tube
    R(g,6,2,9,7,6);R(g,6,1,9,2,15)                                                         # gunner
    return outl(g)
def soldier(f,kind,friendly=False):
    # kind 0 rifle 1 rpg 2 atgm 3 rpg-aim 4 dead
    g=G(16,16)
    if kind==4:
        R(g,2,13,12,14,5);R(g,12,13,14,14,15);R(g,2,15,14,15,6);return outl(g)
    a=f&1
    L(g,6,11,5-a,15,6);L(g,8,11,9+a,15,6);P(g,5-a,15,13);P(g,9+a,15,13)   # legs
    R(g,5,6,9,11,5);L(g,5,6,9,6,7);P(g,6,8,6);P(g,8,10,6)                   # torso (vest)
    R(g,6,3,8,5,15);P(g,8,4,13)                                              # face
    R(g,5,1,9,3,6 if not friendly else 6);L(g,5,1,9,1,7);P(g,10,3,6)          # helmet
    if kind==0:R(g,8,7,14,8,8);P(g,14,7,13)
    elif kind==1:R(g,3,4,13,5,6);R(g,13,3,15,6,8)                         # RPG on shoulder
    elif kind==3:R(g,2,2,13,4,6);R(g,13,1,15,5,8);P(g,1,3,4);P(g,0,3,8)   # RPG raised to fire
    elif kind==2:R(g,8,7,13,8,8)
    return outl(g)
def orlan(f):
    g=G(16,16);R(g,2,7,13,8,5);L(g,2,7,13,7,7);R(g,13,6,15,9,12);R(g,1,4,2,8,5);R(g,6,9,8,10,8)
    L(g,7,5,7,6,8);L(g,4 if f else 9,5,10 if f else 5,5,3);return outl(g)
def fpv(f):
    g=G(16,16);R(g,5,6,10,9,8);R(g,6,9,9,11,9);P(g,7,10,10)
    for x in (2,13):L(g,x-2,4 if f else 5,x+2,5 if f else 4,3);P(g,x,5,13)
    L(g,4,6,2,5,8);L(g,11,6,13,5,8);P(g,10,7,12);return outl(g)
def mine(f):
    g=G(16,8);E(g,3,3,12,7,8);R(g,6,2,9,3,3);P(g,7,2,9 if f else 13);return outl(g)
def ied(f):
    g=G(16,8);R(g,2,4,13,7,6);R(g,3,3,8,4,5);L(g,10,4,14,1,13);P(g,14,1,9 if f else 13);return outl(g)
def crate(k):
    g=G(16,16);R(g,1,4,14,15,5);L(g,1,4,14,4,7);L(g,1,15,14,15,6);L(g,1,4,14,15,6);L(g,14,4,1,15,6)
    if k==0:R(g,4,7,11,11,8);L(g,4,9,11,9,10)         # TOW
    elif k==1:R(g,6,6,9,13,11);R(g,3,8,12,11,11);R(g,7,7,8,12,9);R(g,4,9,11,10,9)   # repair
    else:R(g,4,6,11,13,8);R(g,6,8,9,11,10)            # ammo
    return outl(g)
def intel(f):
    g=G(16,16)
    R(g,2,5,13,13,8);L(g,2,5,13,5,4);R(g,6,3,9,5,13);R(g,6,8,9,10,10);P(g,7,9,11)
    if f:L(g,1,4,1,6,11);L(g,14,12,14,14,11)
    return outl(g)
def flag(k):
    g=G(16,32);L(g,3,2,3,31,4);R(g,2,30,5,31,8)
    if k==0:R(g,4,3,13,9,10);L(g,4,6,13,6,13)          # checkpoint
    elif k==1:
        for y in range(3,11):
            for x in range(4,15):P(g,x,y,11 if ((x//2+y//2)&1) else 13)   # chequered finish
    else:R(g,4,3,14,10,9);R(g,7,5,11,8,11)                # objective marker
    return outl(g)
def aircraft():
    g=G(64,32)
    POLY(g,[(2,20),(10,16),(48,15),(62,19),(48,22),(10,23)],5);L(g,10,16,48,15,7)
    POLY(g,[(40,15),(44,12),(54,13),(56,16)],12)
    POLY(g,[(20,19),(34,19),(24,27),(16,27)],6)            # wing
    POLY(g,[(2,20),(6,8),(12,8),(12,16)],5)                 # fin
    R(g,14,24,16,27,8);R(g,46,22,47,27,8);C(g,15,28,1.6,13);C(g,46,28,1.6,13)
    R(g,26,23,30,24,8)
    return outl(g)
def radar():
    g=G(32,32)
    for x in (6,24):C(g,x,28,2.8,13);C(g,x,28,1.2,3)
    R(g,1,21,30,25,5);L(g,1,21,30,21,7);R(g,22,15,30,21,5);R(g,24,16,29,18,12)
    L(g,12,21,12,12,8);E(g,2,2,22,13,4);E(g,4,4,20,11,3);L(g,12,7,12,12,8);P(g,12,7,9)
    return outl(g)
def sam():
    g=G(32,32)
    for x in (5,15,26):C(g,x,28,2.8,13);C(g,x,28,1.2,3)
    R(g,1,21,30,25,5);L(g,1,21,30,21,7);R(g,23,16,30,21,5);R(g,25,17,29,18,12)
    for k in range(3):L(g,4+k*3,19-k,19+k*3,8-k,8);L(g,5+k*3,19-k,20+k*3,8-k,6);P(g,20+k*3,7-k,9)
    return outl(g)
def wagon(kind):
    # armored train (pal1): 0 locomotive, 1 flak wagon, 2 rocket wagon ; 64x32 facing right
    g=G(64,32)
    for x in (8,18,44,54):C(g,x,28,3.2,13);C(g,x,28,1.4,3);P(g,x,28,8)
    R(g,1,22,62,25,8);L(g,1,22,62,22,3)
    if kind==0:
        POLY(g,[(2,9),(46,9),(62,15),(62,22),(2,22)],5);L(g,2,9,46,9,7);L(g,46,9,62,15,7)
        R(g,38,11,44,15,12);R(g,10,12,34,13,6);[R(g,x,15,x+2,19,8) for x in range(8,36,6)]
        R(g,20,4,26,9,8);R(g,58,18,62,20,10);R(g,48,12,52,14,6)
    elif kind==1:
        R(g,2,13,62,22,5);L(g,2,13,62,13,7);[L(g,x,14,x,21,6) for x in range(8,60,8)]
        POLY(g,[(18,13),(22,7),(40,7),(44,13)],6);L(g,22,7,40,7,7)
        L(g,36,8,56,1,8);L(g,36,10,56,3,8);R(g,54,0,57,3,8)       # twin ZU-23
    else:
        R(g,2,15,62,22,5);L(g,2,15,62,15,7)
        POLY(g,[(10,14),(48,4),(54,8),(16,15)],6)                  # Grad rocket box
        for k in range(4):L(g,13+k*2,13-k,50+k,4+k*0,8)
        R(g,20,16,40,18,8)
    patches(g,1,0,62,22,101+kind,cols=(6,7),n=12)
    return outl(g)
# ---------------------------------------------------------------- particles / fx (pal6 hot)
def fireball(fr):
    g=G(32,32);r=random.Random(70+fr)
    if fr==0:
        C(g,16,16,8,7);C(g,16,16,6,8);C(g,16,16,3.5,9)
        for a in range(8):
            an=a*math.pi/4+.3;L(g,16,16,round(16+math.cos(an)*13),round(16+math.sin(an)*13),8 if a%2 else 7)
    elif fr==1:
        for k in range(10):
            an=r.random()*6.28;d=r.random()*6;C(g,16+math.cos(an)*d,16+math.sin(an)*d,7+r.random()*3,6)
        for k in range(8):
            an=r.random()*6.28;d=r.random()*5;C(g,16+math.cos(an)*d,15+math.sin(an)*d,5+r.random()*2,7)
        C(g,16,15,5,8);C(g,15,14,2.5,9)
    elif fr==2:
        for k in range(10):
            an=r.random()*6.28;d=r.random()*7;C(g,16+math.cos(an)*d,14+math.sin(an)*d,6+r.random()*3,2)
        for k in range(8):
            an=r.random()*6.28;d=r.random()*6;C(g,16+math.cos(an)*d,15+math.sin(an)*d,3+r.random()*2,6)
        for k in range(5):
            an=r.random()*6.28;d=r.random()*4;C(g,16+math.cos(an)*d,16+math.sin(an)*d,2,7)
        C(g,16,17,2,8)
    else:
        for k in range(10):
            an=r.random()*6.28;d=r.random()*7;C(g,16+math.cos(an)*d,13+math.sin(an)*d,5+r.random()*3,3)
        for k in range(6):
            an=r.random()*6.28;d=r.random()*5;C(g,15+math.cos(an)*d,11+math.sin(an)*d,3+r.random()*2,4)
        for k in range(4):P(g,r.randint(9,23),r.randint(16,24),7)
    return outl(g)
def puff(fr):
    g=G(16,16);r=random.Random(9+fr)
    for k in range(6):C(g,8+r.randint(-3,3),8+r.randint(-3,2),2.6+r.random()*2,3)
    for k in range(4):C(g,7+r.randint(-2,2),7+r.randint(-2,1),1.5+r.random(),4)
    return outl(g,2)
def smoke_big(fr):
    g=G(32,32);r=random.Random(19+fr)
    for k in range(12):C(g,16+r.randint(-8,8),17+r.randint(-7,6),4+r.random()*3,3)
    for k in range(8):C(g,14+r.randint(-6,6),14+r.randint(-6,4),2.5+r.random()*2,4)
    return outl(g,2)
def p8(kind):
    g=G(8,8)
    if kind=='casing':R(g,3,3,5,4,10);P(g,5,3,11);P(g,3,4,11)
    elif kind=='debris':POLY(g,[(2,2),(5,3),(4,6),(1,5)],3);P(g,2,3,4)
    elif kind=='debris2':R(g,3,2,4,5,2);P(g,3,2,3)
    elif kind=='ember':P(g,3,3,8);P(g,4,3,7);P(g,3,4,7)
    elif kind=='flake':P(g,3,3,14);P(g,4,4,14);P(g,2,4,14);P(g,3,5,14);P(g,4,2,14)
    elif kind=='rain':L(g,5,0,2,7,15)
    elif kind=='sand':L(g,0,4,6,3,13);P(g,7,3,13)
    elif kind=='laser':R(g,1,3,6,4,12);P(g,3,3,9)
    elif kind=='laserdot':C(g,3.5,3.5,2.6,12);C(g,3.5,3.5,1.2,9)
    elif kind=='warn':R(g,3,0,4,4,12);R(g,3,6,4,7,12)
    elif kind=='glint':L(g,0,3,7,3,9);L(g,3,0,3,7,9);P(g,1,1,8);P(g,6,6,8);P(g,1,6,8);P(g,6,1,8)
    elif kind=='ap':L(g,0,3,6,3,8);L(g,2,4,7,4,9);P(g,7,3,9)
    elif kind=='aptr':L(g,0,3,5,3,7);L(g,0,4,7,4,8);P(g,6,3,9);P(g,7,4,9)
    elif kind=='he':C(g,4,3.5,2,7);P(g,4,3,9)
    elif kind=='ebul':L(g,1,3,6,3,6);P(g,6,3,7)
    elif kind=='shell':R(g,1,3,6,4,8);P(g,6,3,9);P(g,0,3,7)
    elif kind=='arty':R(g,3,1,4,6,8);P(g,3,6,9)
    elif kind=='flare':C(g,3.5,3.5,2,8);C(g,3.5,3.5,1,9)
    elif kind=='spark':P(g,3,3,9);P(g,2,3,8);P(g,4,3,8);P(g,3,2,8);P(g,3,4,8);P(g,1,1,7);P(g,6,6,7)
    elif kind=='mg':L(g,1,3,6,3,8);P(g,6,3,9)
    elif kind=='aps':C(g,3.5,3.5,3,8);C(g,3.5,3.5,1.5,9)
    elif kind=='dust':C(g,3.5,4,2.6,13);P(g,3,3,4)
    return g
def missile(kind):
    g=G(16,8)
    if kind=='tow':R(g,4,3,12,4,4);R(g,12,3,13,4,8);R(g,5,2,6,5,8);R(g,1,3,3,4,8);P(g,0,3,8)
    elif kind=='rpg':R(g,6,3,10,4,8);E(g,10,2,14,5,6);R(g,3,3,5,4,4);P(g,2,2,4);P(g,2,5,4)
    elif kind=='atgm':R(g,3,3,13,4,6);R(g,13,3,14,4,9);R(g,5,2,6,5,8);P(g,2,3,12)
    elif kind=='rocket':R(g,3,3,12,4,4);R(g,12,3,14,4,9);P(g,3,2,8);P(g,3,5,8)
    return outl(g)
def grass(f):
    g=G(16,8)
    for i,x in enumerate((2,5,8,11,13)):L(g,x,7,x+(1 if (f+i)%2 else -1)+(i%2),2+(i%3),6 if i%2 else 5)
    return g
# ---------------------------------------------------------------- tile packing
def tiles(g):
    h=len(g);w=len(g[0]);out=[]
    for ty in range(h//8):
        for tx in range(w//8):
            for y in range(8):
                v=0
                for x in range(8):v|=(g[ty*8+y][tx*8+x]&15)<<(4*x)
                out.append(v)
    return out
sheet=[]
VEH=[]
for v in range(5):
    for f in range(4):
        b=bradley(v,f);VEH+=tiles(b)
        if f==0:sheet.append(('V%d'%v,b,'cam'))
BAR=tiles(barrel());sheet.append(('barrel',barrel(),'cam'))
BOSSN=['T-72B3 HULL-DOWN','MI-24 HIND','T-90M PROryv','T-14 ARMATA','ARMORED TRAIN']
BOSS=[]
def bpad(t):return t+[0]*(96*8-len(t))
BOSS+=bpad(tiles(mbt(0,1))+tiles(mbt(2,1)))
BOSS+=bpad(tiles(helo(0,True))+tiles(helo(1,True)))
BOSS+=bpad(tiles(mbt(0,2))+tiles(mbt(2,2)))
BOSS+=bpad(tiles(t14(0))+tiles(t14(2)))
BOSS+=bpad(tiles(wagon(0))+tiles(wagon(1))+tiles(wagon(2)))
for nm,gg in (('t72',mbt(0,1)),('hind',helo(0,True)),('t90',mbt(0,2)),('t14',t14(0)),('loco',wagon(0)),('flak',wagon(1)),('grad',wagon(2))):sheet.append((nm,gg,'en'))
COM=[];idx={}
def add(name,g,pal='en'):
    idx[name]=len(COM)//8;COM.extend(tiles(g));sheet.append((name,g,pal))
for f in range(2):add('TANK%d'%f,mbt(f*2,0))
for f in range(2):add('BMP%d'%f,bmp2(f*2))
for f in range(2):add('BTR%d'%f,btr(f))
for f in range(2):add('HELI%d'%f,helo(f))
add('TRUCK0',truck(0,True),'misc')
add('URAL',truck(0,False))
add('AIRC',aircraft())
for f in range(2):add('TECH%d'%f,technical(f))
add('CAR',car(0))
add('NEST',atgm_nest())
add('TOSS',turret_toss())
add('RADAR',radar());add('SAM',sam())
for k,nm in enumerate(('RIFLE0','RIFLE1','RPG0','RPG1','ATGM0','RPGAIM','DEAD')):
    add(nm,soldier(k%2 if k<2 else (k-2)%2 if k<4 else 0,[0,0,1,1,2,3,4][k]))
add('ALLY0',soldier(0,0,True),'misc');add('ALLY1',soldier(1,0,True),'misc')
for f in range(2):add('ORLAN%d'%f,orlan(f))
for f in range(2):add('FPV%d'%f,fpv(f))
for f in range(2):add('MINE%d'%f,mine(f))
for f in range(2):add('IED%d'%f,ied(f))
for k in range(3):add('CRATE%d'%k,crate(k),'misc')
for f in range(2):add('INTEL%d'%f,intel(f),'misc')
for k in range(3):add('FLAG%d'%k,flag(k),'misc')
add('ERA',era_overlay(),'cam');add('APS',aps_kit(),'cam')
for f in range(4):add('FB%d'%f,fireball(f),'hot')
for f in range(2):add('SMK%d'%f,smoke_big(f),'hot')
for f in range(2):add('PUFF%d'%f,puff(f),'hot')
for k in ('casing','debris','debris2','ember','flake','rain','sand','laser','laserdot','warn','glint','ap','aptr','he','ebul','shell','arty','flare','spark','mg','aps','dust'):add('P_'+k.upper(),p8(k),'hot')
for k in ('tow','rpg','atgm','rocket'):add('M_'+k.upper(),missile(k),'hot' if k!='tow' else 'en')
for f in range(2):add('GRASS%d'%f,grass(f),'misc')
assert len(COM)//8<=720,len(COM)//8
# ---------------------------------------------------------------- parallax backdrops (BG3 far: CB2/SB31 bank3 ; BG2 mid: CB1+64/SB29 bank4)
def circ_img(im,cx,cy,r,c,W=256):
    for y in range(int(cy-r-1),int(cy+r+2)):
        for x in range(int(cx-r-1),int(cx+r+2)):
            if (x-cx)**2+(y-cy)**2<=r*r and 0<=y<len(im):im[y][x%W]=c
def img(c=0):return [[c]*256 for _ in range(256)]
def sky(im,bands):
    # bands: list of (y_end,colour); dither 4px between bands
    y0=0
    for i,(ye,c) in enumerate(bands):
        for y in range(y0,ye):
            for x in range(256):
                im[y][x]=c
                if i>0 and y<y0+4 and ((x+y)&1) and y-y0<2+((x>>1)&1):im[y][x]=bands[i-1][1]
        y0=ye
def cloud(im,cx,cy,s,cw,cs,co):
    r=random.Random(cx*7+cy)
    blobs=[(cx+dx*s,cy+dy*s,rr*s) for dx,dy,rr in ((-12,2,7),(-4,-3,9),(6,-1,8),(14,3,6),(0,4,8))]
    for bx,by,br in blobs:circ_img(im,bx,by,br+1.5,co)
    for bx,by,br in blobs:circ_img(im,bx,by,br,cs)
    for bx,by,br in blobs:circ_img(im,bx-1,by-2,br-1.5,cw)
    for x in range(int(cx-20*s),int(cx+22*s)):          # flat bottom
        for y in range(int(cy+6*s),int(cy+14*s)):
            if im[y][x%256] in (cw,cs,co):im[y][x%256]=0 if False else im[y][x%256]
def ridge(im,base,amp,per,phase,c,shade=None,outline=None,seed=0,rough=0):
    r=random.Random(seed);hs=[]
    for x in range(256):
        h=base-amp*(0.5+0.5*math.sin(2*math.pi*(x/256*per)+phase))-amp*0.3*math.sin(2*math.pi*x/256*per*2.3+phase*2)
        if rough:h+=r.randint(0,rough)
        hs.append(int(h))
    for x in range(256):
        for y in range(max(0,hs[x]),256):im[y][x]=c
        if shade is not None:
            for y in range(hs[x],min(256,hs[x]+2)):im[y][x]=shade
        if outline is not None and 0<=hs[x]-1<256:im[hs[x]-1][x]=outline
    return hs
def tilemap(im,bank,base,maxt,trans_ok):
    tl=[];lut={};mp=[]
    for ty in range(32):
        for tx in range(32):
            t=tuple(tuple(im[ty*8+y][tx*8:(tx*8+8)]) for y in range(8))
            fl=None
            for f,(hx,vy) in enumerate(((0,0),(1,0),(0,1),(1,1))):
                tt=t
                if hx:tt=tuple(tuple(reversed(r)) for r in tt)
                if vy:tt=tuple(reversed(tt))
                if tt in lut:fl=(lut[tt],f);break
            if fl is None:
                lut[t]=len(tl);tl.append(t);fl=(lut[t],0)
            i,f=fl
            mp.append((base+i)|((f&1)<<10)|((f>>1)<<11)|(bank<<12))
    assert len(tl)<=maxt,(len(tl),maxt)
    data=[]
    for t in tl:
        for row in t:
            v=0
            for x in range(8):v|=(row[x]&15)<<(4*x)
            data.append(v)
    return data,mp,len(tl)
def bg_desert():
    F=img(5);FP=[(0,0,0),(52,112,214),(86,148,232),(126,182,242),(174,210,246),(236,226,204),(252,252,252),(206,222,244),(130,160,212),(220,172,128),(190,136,104),(150,102,86),(236,206,150),(214,176,122),(255,240,180),(112,80,72)]
    sky(F,[(36,1),(70,2),(98,3),(118,4),(256,5)])
    circ_img(F,200,40,13,14)
    for cx,cy,s in ((40,30,1.0),(150,56,.8),(236,22,.7)):cloud(F,cx,cy,s,6,7,8)
    r=random.Random(3)
    for m in range(5):     # mesas
        x0=m*52+r.randint(0,20);w=r.randint(26,44);h=r.randint(16,30);y=128-h
        for x in range(x0,x0+w):
            sl=min(x-x0,x0+w-1-x);top=y+max(0,3-sl)*3
            for yy in range(top,140):F[yy][x%256]=9 if yy<top+3 else 10 if (yy-top)//5%2 else 11
            F[top-1][x%256]=15
    ridge(F,146,6,3,0.5,12,13,15,seed=4)
    for y in range(152,256):
        for x in range(256):F[y][x]=13 if (y//6+x//40)%3==0 else 12
    M=img(0);MP=[(0,0,0),(92,60,40),(248,218,156),(230,190,124),(200,154,96),(150,100,60),(108,68,40),(98,176,62),(52,122,44),(78,78,88),(142,142,152),(224,184,134),(182,142,102),(60,40,30),(250,140,40),(255,244,206)]
    hs=ridge(M,150,10,2,1.2,3,2,1,seed=7)
    for x in range(256):
        for y in range(hs[x]+2,256):
            if (y-hs[x])>10 and ((x*3+y*5)%23)<2:M[y][x]=4
    def palm(px,base,h):
        for y in range(base-h,base):
            x=px+int(3*math.sin((base-y)/h*1.6));M[y][x%256]=5;M[y][(x+1)%256]=6
        tx=px+3;ty=base-h
        for a in range(6):
            an=math.pi*(0.15+a*0.14);ln=13
            for k in range(ln):
                xx=tx+int(math.cos(an)*k*(1 if a%2 else -1));yy=ty-int(math.sin(an)*k*0.5)+int(k*k/30)
                M[yy][xx%256]=7 if k<ln-2 else 8;M[yy+1][xx%256]=8
    def derrick(px,base):
        for y in range(base-40,base):
            w=(y-(base-40))//4
            M[y][(px-w)%256]=9;M[y][(px+w)%256]=9
            if y%6==0:
                for x in range(px-w,px+w+1):M[y][x%256]=10
        M[base-41][px%256]=1
    def ruin(px,base):
        for y in range(base-18,base):
            for x in range(px,px+22):M[y][x%256]=11 if x<px+18 else 12
        circ_img(M,px+9,base-18,7,11)
        for x in range(px+2,px+22):M[base-26+((x*7)%5)][x%256]=0 if x>px+14 else M[base-26][x%256]
        for wx in (px+4,px+12):
            for y in range(base-12,base-6):M[y][wx%256]=13;M[y][(wx+1)%256]=13
    palm(30,hs[30]+2,26);palm(48,hs[48]+2,20);derrick(110,hs[110]+2);ruin(160,hs[160]+3);palm(218,hs[218]+2,24)
    return F,FP,M,MP
def bg_forest():
    F=img(5);FP=[(0,0,0),(64,136,236),(98,166,244),(140,196,250),(186,222,252),(214,236,250),(252,252,252),(206,226,248),(124,160,214),(120,170,170),(92,140,140),(66,108,112),(150,190,170),(100,148,128),(255,246,200),(52,80,86)]
    sky(F,[(40,1),(74,2),(100,3),(120,4),(256,5)])
    for cx,cy,s in ((30,26,1.0),(120,48,.9),(200,20,.8),(250,62,.6)):cloud(F,cx,cy,s,6,7,8)
    hs=ridge(F,132,12,3,0.2,9,None,15,seed=11)
    r=random.Random(5)
    for x in range(0,256,5):
        h=r.randint(5,9);b=hs[x]+1
        for k in range(h):
            for dx in range(-(k//3),k//3+1):F[b-h+k][(x+dx)%256]=11
    hs2=ridge(F,148,7,4,2.0,10,None,15,seed=12)
    for y in range(156,256):
        for x in range(256):F[y][x]=13 if (y+x//32)%7==0 else 12
    M=img(0);MP=[(0,0,0),(28,56,30),(98,200,74),(70,166,56),(44,126,44),(150,230,110),(30,90,40),(20,64,30),(170,60,40),(220,96,60),(236,226,200),(160,120,80),(110,80,56),(250,250,240),(60,50,46),(240,200,90)]
    r=random.Random(9)
    for k,(cx,rad) in enumerate(((20,40),(110,52),(190,36),(250,30))):  # SMW dome hills
        cy=170
        for y in range(256):
            for x in range(cx-rad-2,cx+rad+3):
                d=((x-cx)**2+(y-cy)**2)**.5
                if d<=rad:M[y][x%256]=3 if d<rad-4 else 2
                elif d<=rad+1.5 and M[y][x%256]==0:M[y][x%256]=1
        for s in range(5):
            sx=cx+r.randint(-rad//2,rad//2);sy=cy-r.randint(rad//4,rad//2)
            for y in range(sy,sy+6):M[y][sx%256]=4;M[y][(sx+1)%256]=4
        circ_img(M,cx-rad//3,cy-rad+8,4,5)
    def pine(px,base,h):
        for k in range(h):
            w=int(k*0.42)+1
            for dx in range(-w,w+1):M[base-h+k][(px+dx)%256]=7 if dx<0 else 6
            M[base-h+k][(px-w-1)%256]=1;M[base-h+k][(px+w+1)%256]=1
        for y in range(base,base+4):M[y][px%256]=12
    for x in range(0,256,9):
        if not (60<x<92):pine(x+r.randint(-2,2),148+r.randint(-4,2),r.randint(18,30))
    # farmhouse
    for y in range(128,150):
        for x in range(64,88):M[y][x]=10 if (x+y)%9 else 11
    for k in range(12):
        for x in range(62+k,90-k):M[127-k][x]=9 if k<11 else 1
    for (wx,wy) in ((68,134),(80,134)):
        for y in range(wy,wy+6):
            for x in range(wx,wx+4):M[y][x]=14
    for y in range(150,256):
        for x in range(256):
            if M[y][x]==0:M[y][x]=4 if (x//6+y//4)%5 else 3
    return F,FP,M,MP
def bg_city():
    F=img(5);FP=[(0,0,0),(120,132,170),(150,156,184),(184,176,186),(214,192,176),(232,206,170),(250,242,226),(206,200,206),(150,146,160),(98,104,130),(78,82,108),(60,62,86),(250,214,110),(124,118,124),(255,230,160),(44,44,60)]
    sky(F,[(34,1),(66,2),(94,3),(118,4),(256,5)])
    for cx,cy,s in ((70,30,.9),(190,50,.8)):cloud(F,cx,cy,s,6,7,8)
    r=random.Random(21);x=0
    while x<256:
        w=r.randint(12,26);h=r.randint(24,64);c=r.choice((9,10,11));top=146-h
        for xx in range(x,x+w):
            for y in range(top,160):F[y][xx%256]=c
            F[top-1][xx%256]=15
        if r.random()<.3:
            for y in range(top-10,top):F[y][(x+w//2)%256]=15
        for wy in range(top+4,146,6):
            for wx in range(x+2,x+w-2,4):
                if r.random()<.35:F[wy][wx%256]=12
        x+=w+r.randint(0,3)
    for k,(sx) in enumerate((40,150,226)):     # smoke plumes
        for y in range(20,120):
            w=3+(120-y)//10
            cx=sx+int(6*math.sin(y/14.0+k))
            for dx in range(-w,w+1):
                if (dx+y)%3:F[y][(cx+dx)%256]=13
    for y in range(160,256):
        for x in range(256):F[y][x]=10
    M=img(0);MP=[(0,0,0),(30,28,34),(196,186,170),(170,160,146),(136,126,116),(100,94,90),(70,66,66),(234,226,206),(56,70,96),(120,150,180),(40,40,46),(140,60,40),(190,90,60),(250,200,90),(90,90,96),(210,210,214)]
    r=random.Random(23);x=0
    while x<256:
        w=r.randint(30,52);h=r.randint(50,86);top=158-h;dmg=r.random()<.6
        for xx in range(x,x+w):
            ct=top+(int(6*abs(math.sin(xx*0.7))) if dmg and xx>x+w*0.55 else 0)
            for y in range(ct,170):M[y][xx%256]=3 if xx<x+w-3 else 4
            M[ct][xx%256]=7;M[ct-1][xx%256]=1
            M[ct][(x)%256]=1
        for xx in range(x-1,x+w+1):pass
        for y in range(top,170):M[y][(x-1)%256]=1;M[y][(x+w)%256]=1
        for wy in range(top+6,158,10):
            for wx in range(x+4,x+w-6,8):
                hole=dmg and r.random()<.18
                for y in range(wy,wy+6):
                    for xx in range(wx,wx+4):M[y][xx%256]=10 if hole else (9 if r.random()<.5 else 8)
                if not hole:
                    for xx in range(wx-1,wx+5):M[wy+6][xx%256]=5
                    if r.random()<.3:
                        for y in range(wy+3,wy+7):M[y][(wx-1)%256]=6;M[y][(wx+4)%256]=6
                        for xx in range(wx-1,wx+5):M[wy+3][xx%256]=6
        if dmg:
            hx=x+r.randint(6,w-14);hy=top+r.randint(10,30)
            circ_img(M,hx,hy,6,10);circ_img(M,hx+2,hy+1,3,6)
        x+=w+r.randint(4,12)
    for lx in range(20,256,64):           # street lamps
        for y in range(130,170):M[y][lx]=14
        for xx in range(lx,lx+7):M[130][xx]=14
        M[131][lx+6]=13;M[131][lx+5]=13
    for y in range(160,256):
        for xx in range(256):
            if M[y][xx]==0:M[y][xx]=5 if (xx//10+y//5)%4 else 6
    return F,FP,M,MP
def bg_winter():
    F=img(5);FP=[(0,0,0),(110,150,206),(140,174,220),(170,198,232),(200,220,240),(226,236,246),(252,252,255),(214,228,244),(150,170,204),(248,250,255),(196,212,236),(140,156,190),(104,114,140),(220,230,244),(230,236,200),(70,80,104)]
    sky(F,[(36,1),(68,2),(96,3),(118,4),(256,5)])
    for cx,cy,s in ((60,28,.9),(170,46,.8),(240,18,.6)):cloud(F,cx,cy,s,6,7,8)
    r=random.Random(31)
    for m in range(6):
        px=m*44+r.randint(0,20);h=r.randint(30,58);base=140
        for y in range(base-h,base+20):
            w=int((y-(base-h))*0.9)
            for dx in range(-w,w+1):
                c=9 if y<base-h+h*0.35 else (10 if dx<0 else 11)
                if y>=base-h+h*0.35 and ((dx*3+y*2)%17)<3:c=9
                F[y][(px+dx)%256]=c
            F[y][(px-w-1)%256]=15 if y<base else F[y][(px-w-1)%256]
    ridge(F,150,5,3,1.0,13,None,12,seed=33)
    for y in range(158,256):
        for x in range(256):F[y][x]=13
    M=img(0);MP=[(0,0,0),(24,40,46),(250,252,255),(214,226,240),(174,190,212),(36,84,64),(24,62,48),(240,240,236),(40,40,40),(126,96,72),(90,110,100),(160,176,200),(255,255,255),(120,140,170),(200,220,240),(60,74,92)]
    hs=ridge(M,154,6,2,0.4,3,2,4,seed=35)
    def spine(px,base,h):
        for k in range(h):
            w=int(k*0.45)+1
            for dx in range(-w,w+1):M[base-h+k][(px+dx)%256]=5 if dx<0 else 6
            if k%5==0:
                for dx in range(-w,w+1):M[base-h+k][(px+dx)%256]=2 if abs(dx)<w else 3
            M[base-h+k][(px-w-1)%256]=1;M[base-h+k][(px+w+1)%256]=1
    def birch(px,base,h):
        for y in range(base-h,base):
            M[y][px%256]=7;M[y][(px+1)%256]=7
            if y%5==0:M[y][px%256]=8
        for k in range(6):
            yy=base-h+k*3;L2=6-k//2
            for dx in range(1,L2):M[yy-dx//2][(px+dx+1)%256]=9;M[yy-dx//2][(px-dx)%256]=9
    for x in range(0,256,11):
        if r.random()<.65:spine(x,hs[x%256]+2,r.randint(18,32))
        else:birch(x,hs[x%256]+2,r.randint(22,34))
    return F,FP,M,MP
BGS=[bg_desert(),bg_forest(),bg_city(),bg_winter()]
BGD=[]
for wi,(F,FP,M,MP) in enumerate(BGS):
    fd,fm,fn=tilemap(F,3,0,512,False)
    md,mm,mn=tilemap(M,4,64,448,True)
    BGD.append((fd,fm,fn,FP,md,mm,mn,MP))
    pv=Image.new('RGB',(256,256))
    for y in range(256):
        for x in range(256):
            c=M[y][x];pv.putpixel((x,y),MP[c] if c else FP[F[y][x]])
    pv.resize((512,512),Image.NEAREST).save(os.path.join(ROOT,'build','brad_bg_%d.png'%wi))
# ---------------------------------------------------------------- courses
EN=['INF','RPG','ATGM','TECH','BTR','BMP2','TANK','HELI','ORLAN','FPV','MINE','IED','CAR','FLEE','AIRC','RADAR','SAM',
    'TOWC','REP','INTEL','CHECK','GOAL','BOSS','CONVOY','HOUSE','AMBUSH','SWARM','WAVE','FIRE','WRECK']
EID={k:i for i,k in enumerate(EN)}
AIR=('HELI','ORLAN','FPV','SWARM')
TID={'.':0,'#':1,'B':3,'S':6,'-':7,'~':10,'w':11,'C':21}
H=12
OBJN=['REACH','ESCORT','DEFEND','CHASE','TARGETS','DISMOUNT']
FL_NIGHT,FL_SAND,FL_SNOW,FL_RAIN=1,2,4,8
class Lv:
    def __init__(s,seed):
        s.g=[[] for _ in range(H)];s.h=9;s.W=0;s.ents=[];s.r=random.Random(seed);s.intel=0;s.extra=0
    def col(s,top=None,ch='#'):
        top=s.h if top is None else top
        for y in range(H):s.g[y].append(ch if y>=top else '.')
        s.W+=1
    def flat(s,n):
        for _ in range(n):s.col()
    def step(s,d,n=4):
        s.h=max(5,min(10,s.h+d));s.flat(n)
    def set(s,x,y,ch):s.g[y][x]=ch
    def top(s,x):
        for y in range(H):
            if s.g[y][x]!='.':return y
        return H
    def e(s,name,x=None,row=None,tier=0):
        x=s.W-2 if x is None else x
        s.ents.append([name,x,row,tier])
    def pool(s,n,pool,dens=.3,air_row=(2,4)):
        n=int(n*1.6);dens=min(.62,dens*1.55)
        x0=s.W;s.flat(n)
        for x in range(x0+2,s.W-1,3):
            if s.r.random()<dens and not any(abs(e[1]-x)<2 for e in s.ents[-4:]):
                k=s.r.choice(pool);t=0 if s.r.random()<.55 else (1 if s.r.random()<.6 else 2)
                s.e(k,x,s.r.randint(*air_row) if k in AIR else None,t)
    # ----- set pieces
    def ditch(s,w=2,d=2):
        s.flat(2);h0=s.h
        for _ in range(w):s.col(h0+d)
        s.flat(3)
    def berm(s,w=3,hh=1,crew=('INF','RPG')):
        s.flat(2);h0=s.h;s.h-=hh;s.flat(w);s.h=h0
        for i,c in enumerate(crew):s.e(c,s.W+1+i*2)
        s.flat(5)
    def wall(s,hh=1,w=1):
        s.flat(2)
        for _ in range(w):
            s.col()
            for j in range(hh):s.g[s.h-1-j][-1]='B'
        s.flat(3)
    def ford(s,w=6,deep=1):
        s.flat(2);h0=s.h
        for _ in range(w):
            s.col(h0+deep)
            for j in range(deep):s.g[h0+j][-1]='~' if j==0 else 'w'
        s.flat(3)
    def bridge(s,w=8,weak=False,river=True):
        s.flat(2);h0=s.h
        for i in range(w):
            s.col(H)   # chasm
            s.g[h0][-1]='C' if (weak and 1<=i<w-1) else '-'
            if river:s.g[10][-1]='~';s.g[11][-1]='w'
        s.flat(3)
    def building(s,w=5,hh=4,open_floor=True,roof=('RPG',),tier=0):
        s.flat(1);h0=s.h;xs=s.W
        for i in range(w):
            s.col()
            for j in range(hh):
                y=h0-1-j
                if open_floor and j<2 and 0<i<w-1:continue
                s.g[y][-1]='B'
        for k,c in enumerate(roof):s.e(c,xs+1+k*2,None,tier)
        s.flat(1)
    def cache(s):
        # brick hut with hidden intel: blast it open
        s.flat(2);xs=s.W
        for i in range(3):
            s.col()
            for j in range(3):s.g[s.h-1-j][-1]='B'
        s.e('INTEL',xs+1,s.h-1);s.intel+=1;s.flat(3)
    def bunker(s):
        # secret underground: brick lid over a dip, intel inside
        s.flat(2);h0=s.h;xs=s.W
        for i in range(4):
            s.col(h0+2)
            s.g[h0][-1]='B'
        s.e('INTEL',xs+2,h0+1);s.intel+=1;s.flat(3)
    def high_path(s,n=3,item=True):
        # stepped one-way platforms up to a hidden rooftop route
        s.flat(2);xs=s.W;h0=s.h;s.flat(4+n*5)
        for k in range(n):
            y=h0-2-k*2
            if y<2:break
            for i in range(4):s.g[y][xs+2+k*5+i]='-'
        if item:s.e('INTEL',xs+2+(n-1)*5+2,max(1,h0-3-(n-1)*2));s.intel+=1
        s.flat(2)
    def checkpoint(s):s.flat(3);s.e('CHECK',s.W-2)
    def finish(s,boss=None,arena=None):
        s.h=9;s.flat(4)
        if boss is not None:
            s.e('REP',s.W-2);s.e('TOWC',s.W-1);bx=s.W
            arena(s);s.ents.append(['BOSS',bx,boss,0]);s.flat(4)
        else:s.flat(6)
        s.e('GOAL',s.W-4);s.flat(8)
def arena_flat(L):L.flat(23)
def arena_hulldown(L):
    L.flat(14);h0=L.h
    for i in range(5):
        L.col(h0-1)
    L.flat(4)
def arena_plaza(L):
    L.flat(8);L.wall(1,1);L.flat(11)
def arena_rail(L):
    L.flat(23)
# ----- per-world pools
P1=('INF','INF','RPG','TECH','BTR','BMP2')
P2=('INF','RPG','ATGM','BMP2','TANK','ORLAN')
P3=('INF','INF','RPG','ATGM','TECH','BTR','CAR','FPV')
P4=('INF','RPG','FPV','BMP2','TANK','ORLAN')
def C11(L):  # Line of Departure: tutorial-ish breach of the border berm
    L.flat(10);L.pool(20,('INF','INF','TECH'),.35);L.berm(3,1,('INF','INF'));L.pool(16,('INF','TECH'),.35)
    L.ditch(2,1);L.pool(18,P1[:4],.3);L.cache();L.checkpoint();L.pool(18,P1,.35);L.e('TANK',L.W+8);L.flat(20)
    L.berm(3,1,('RPG','INF'));L.pool(14,P1,.3);L.high_path(3)
    return dict(obj=0)
def C12(L):  # Berm Breach: destroy the Scud launchers behind the berm line
    L.flat(8);L.pool(14,P1,.3);L.berm(4,2,('RPG','INF','INF'));L.e('SAM',L.W+6);L.pool(20,P1,.3);L.ditch(3,2)
    L.pool(16,P1,.35);L.e('SAM',L.W+4);L.flat(10);L.cache();L.checkpoint();L.berm(3,1,('ATGM',));L.pool(18,P1,.35)
    L.e('SAM',L.W+6);L.flat(12);L.e('TANK',L.W+6,None,1);L.pool(18,P1,.3);L.bunker();L.e('SAM',L.W+6);L.flat(14);L.high_path(2)
    return dict(obj=4,objn=4)
def C13(L):  # Convoy Escort MSR TAMPA
    L.e('CONVOY',1);L.flat(12);L.pool(20,('INF','TECH','RPG'),.35);L.ditch(2,1);L.pool(18,P1,.3);L.e('TECH',L.W+4,None,1)
    L.cache();L.checkpoint();L.pool(20,P1,.35);L.e('IED',L.W+6);L.flat(12);L.berm(3,1,('RPG','RPG'));L.pool(20,P1,.3)
    L.e('ORLAN',L.W+4,3);L.pool(16,P1,.3);L.high_path(2);L.checkpoint();L.pool(16,P1,.35)
    return dict(obj=1)
def C14(L):  # Minefield Lanes - upper secret path
    L.flat(10);L.pool(10,('INF',),.3)
    for k in range(3):
        L.flat(2);x0=L.W;L.flat(12)
        for j in range(4):L.e('MINE',x0+1+j*3)
        L.berm(2,1,('INF','RPG'))
        if k==1:L.checkpoint();L.high_path(3)
    L.pool(18,P1,.35);L.cache();L.ditch(2,2);L.pool(16,P1,.35);L.e('TANK',L.W+6);L.flat(14)
    return dict(obj=0)
def C15(L):  # Oil Field Fires - sandstorm, smoke columns, tanks
    L.flat(8)
    for k in range(4):
        L.pool(12,P1,.3);L.e('FIRE',L.W+3);L.flat(6);L.e('TANK',L.W+10,None,0 if k%2==0 else 1);L.flat(14)
        if k==1:L.cache();L.checkpoint()
        if k==2:L.bunker()
    L.high_path(2)
    return dict(obj=0,fl=FL_SAND)
def C16(L):  # 73 Easting - sandstorm tank battle, thermal sights
    L.flat(10);L.pool(10,('INF',),.3)
    for k in range(5):
        L.e('TANK',L.W+12,None,0);L.e('BMP2',L.W+20,None,1 if k%2 else 0);L.flat(26)
        if k==1:L.berm(3,1,('ATGM',));L.cache()
        if k==2:L.checkpoint()
        if k==3:L.high_path(2);L.bunker()
    return dict(obj=0,fl=FL_SAND|FL_NIGHT)
def C17(L):  # Medina Ridge -> boss
    L.flat(8);L.pool(20,P1,.35);L.berm(3,1,('RPG','INF'));L.e('TANK',L.W+8);L.flat(18);L.cache();L.checkpoint()
    L.pool(18,P1,.35);L.high_path(2);L.bunker()
    return dict(obj=0,boss=0,arena=arena_hulldown)
def C21(L):  # Fulda Gap
    L.flat(8);L.pool(18,P2,.3);L.step(-1,6);L.pool(10,P2,.3);L.step(1,4);L.e('TANK',L.W+8);L.flat(16)
    L.cache();L.checkpoint();L.pool(20,P2,.35);L.ditch(2,2);L.pool(16,P2,.35);L.high_path(3);L.bunker()
    return dict(obj=0,fl=FL_RAIN)
def C22(L):  # Forest Trail Ambush - ATGM nests
    L.flat(8);L.pool(10,('INF',),.3)
    for k in range(4):
        L.step(-1,3);L.e('ATGM',L.W-2,None,0);L.flat(5);L.step(1,3);L.pool(14,('INF','RPG','BMP2'),.35);L.e('AMBUSH',L.W)
        L.flat(6)
        if k==1:L.cache();L.checkpoint()
        if k==2:L.high_path(2)
    L.bunker()
    return dict(obj=0,fl=FL_RAIN)
def C23(L):  # Weser Ford - river crossing, mines in the shallows
    L.flat(8);L.pool(14,P2,.3);L.ford(8);L.e('MINE',L.W-6);L.e('MINE',L.W-8,None,1);L.pool(14,P2,.3);L.e('ATGM',L.W+4);L.flat(8)
    L.cache();L.checkpoint();L.ford(10);L.e('MINE',L.W-5);L.e('MINE',L.W-9);L.pool(16,P2,.35);L.ford(6);L.pool(14,P2,.35);L.high_path(2);L.bunker()
    return dict(obj=0,fl=FL_RAIN)
def C24(L):  # The Weak Bridge - crossing under artillery fire
    L.flat(8);L.pool(14,P2,.3);L.e('ORLAN',L.W,3);L.bridge(9,True);L.pool(12,P2,.3);L.e('TANK',L.W+6);L.flat(12);L.cache()
    L.checkpoint();L.e('ORLAN',L.W,2);L.bridge(12,True);L.e('ATGM',L.W+3);L.pool(16,P2,.35);L.bridge(7,False);L.pool(12,P2,.35);L.high_path(2);L.bunker()
    return dict(obj=0)
def C25(L):  # Village Crossroads - defend waves
    L.flat(8);L.pool(18,P2,.3);L.building(5,4,True,('RPG',));L.pool(10,P2,.3);L.checkpoint();L.cache()
    L.flat(4);L.e('WAVE',L.W);L.flat(4);L.wall(1);L.flat(12);L.wall(1);L.flat(8);L.pool(10,P2,.3);L.high_path(2);L.bunker()
    return dict(obj=2,objn=3)
def C26(L):  # Hill 402 night raid - thermal, rain, radar targets
    L.flat(8);L.pool(14,P2,.3);L.step(-1,4);L.e('RADAR',L.W+4);L.pool(12,P2,.3);L.step(-1,4);L.e('ATGM',L.W-2);L.pool(10,P2,.3)
    L.step(1,4);L.e('SAM',L.W+4);L.flat(10);L.cache();L.checkpoint();L.pool(14,P2,.35);L.e('RADAR',L.W+4);L.flat(10)
    L.step(1,4);L.high_path(2);L.e('SAM',L.W+3);L.pool(14,P2,.3);L.bunker()
    return dict(obj=4,objn=4,fl=FL_NIGHT|FL_RAIN)
def C27(L):  # Gunship Valley -> Hind boss
    L.flat(8);L.pool(16,P2,.3);L.e('HELI',L.W,3);L.flat(14);L.cache();L.checkpoint();L.pool(16,P2,.35);L.e('HELI',L.W,2,1);L.flat(12);L.high_path(2);L.bunker()
    return dict(obj=0,boss=1,arena=arena_flat,fl=FL_RAIN)
def street(L,n,roof=True):
    for k in range(n):
        L.pool(6,('INF','CAR','TECH'),.3);L.e('CAR',L.W-3)
        L.building(L.r.randint(4,6),L.r.randint(3,4),True,('RPG',) if roof else ('INF',),0 if k%2==0 else 1)
def C31(L):  # City Outskirts - cars & rooftops
    L.flat(8);street(L,3);L.cache();L.checkpoint();street(L,3);L.high_path(2);L.bunker();L.pool(10,P3,.3)
    return dict(obj=0)
def C32(L):  # IED Alley
    L.flat(8);L.pool(8,('INF',),.3)
    for k in range(5):
        L.flat(2);L.e('IED',L.W+2);L.building(5,3,True,('RPG',),k%2);L.e('AMBUSH',L.W+1);L.flat(4)
        if k==2:L.cache();L.checkpoint()
    L.high_path(2);L.bunker();L.pool(8,P3,.3)
    return dict(obj=0)
def C33(L):  # Market Street - dismount squad to clear the objective building
    L.flat(8);street(L,2);L.cache();L.checkpoint();L.pool(8,P3,.3);L.flat(2);hx=L.W
    L.building(6,4,True,());L.e('HOUSE',hx+3);L.flat(6);street(L,2);L.high_path(2);L.bunker()
    return dict(obj=5)
def C34(L):  # Rooftop Gunships - helis + FPV
    L.flat(8);street(L,1);L.e('HELI',L.W,2);L.flat(10);L.e('SWARM',L.W,2);street(L,2);L.cache();L.checkpoint();L.e('HELI',L.W,3,0)
    L.flat(10);L.e('SWARM',L.W,3,1);street(L,2);L.high_path(2);L.bunker()
    return dict(obj=0)
def C35(L):  # Highway Overpass - chase the fleeing column
    L.flat(8);L.e('FLEE',14);L.e('FLEE',20);L.e('FLEE',27,None,1);L.pool(20,('INF','CAR'),.3)
    L.step(-2,3)
    xs=L.W;L.flat(40)
    for x in range(xs,L.W):L.g[L.h-4][x]='-' if x%9 else 'B'
    L.step(2,3);L.e('CAR',L.W+4);L.pool(20,('INF','CAR','RPG'),.3);L.cache();L.checkpoint();L.pool(30,('INF','CAR','TECH'),.3);L.ditch(2,1);L.high_path(2);L.bunker();L.pool(24,('CAR','INF'),.25)
    return dict(obj=3,objn=3)
def C36(L):  # Rubble District - ambushes + secret underground
    L.flat(8)
    for k in range(4):
        L.building(4,3,True,('RPG',),k%2);L.berm(2,1,('INF',));L.e('AMBUSH',L.W);L.pool(8,P3,.3);L.bunker()
        if k==1:L.checkpoint()
    L.cache();L.high_path(2)
    return dict(obj=0)
def C37(L):  # T-90M Plaza -> boss
    L.flat(8);street(L,2);L.cache();L.checkpoint();L.pool(10,P3,.3);L.high_path(2);L.bunker()
    return dict(obj=0,boss=2,arena=arena_plaza)
def C41(L):  # Frozen Steppe - FPV swarms
    L.flat(8);L.pool(14,P4,.3);L.e('SWARM',L.W,3);L.flat(14);L.pool(10,P4,.3);L.cache();L.checkpoint();L.e('SWARM',L.W,2,0);L.flat(12)
    L.pool(14,P4,.3);L.e('SWARM',L.W,3,1);L.flat(12);L.high_path(2);L.bunker()
    return dict(obj=0,fl=FL_SNOW)
def C42(L):  # Ice River Ford
    L.flat(8);L.pool(12,P4,.3);L.ford(10);L.pool(12,P4,.3);L.bridge(6,True);L.cache();L.checkpoint();L.ford(12);L.e('ATGM',L.W+3);L.pool(14,P4,.35)
    L.bridge(8,True);L.pool(10,P4,.3);L.high_path(2);L.bunker()
    return dict(obj=0,fl=FL_SNOW)
def C43(L):  # Birch Forest night
    L.flat(8);L.pool(16,P4,.35);L.e('ATGM',L.W+2);L.e('AMBUSH',L.W+4);L.flat(10);L.cache();L.checkpoint();L.pool(16,P4,.35);L.e('ATGM',L.W+3);L.flat(10)
    L.e('TANK',L.W+8);L.flat(16);L.high_path(2);L.bunker()
    return dict(obj=0,fl=FL_NIGHT|FL_SNOW)
def C44(L):  # Frozen Airfield assault
    L.flat(8);L.pool(12,P4,.3);L.e('SAM',L.W+4);L.flat(10);L.e('AIRC',L.W+6);L.e('INF',L.W+9);L.flat(14);L.e('RADAR',L.W+4);L.flat(10)
    L.cache();L.checkpoint();L.e('AIRC',L.W+6);L.e('HELI',L.W+2,3,1);L.flat(16);L.e('SAM',L.W+4);L.pool(12,P4,.3);L.e('AIRC',L.W+6);L.flat(12);L.high_path(2);L.bunker()
    return dict(obj=4,objn=6,fl=FL_SNOW)
def C45(L):  # Blizzard Run - escort + heli
    L.e('CONVOY',1);L.flat(10);L.pool(16,P4,.3);L.e('HELI',L.W,3);L.flat(12);L.cache();L.checkpoint();L.pool(16,P4,.3);L.e('SWARM',L.W,2,1);L.flat(10)
    L.pool(14,P4,.3);L.high_path(2);L.bunker()
    return dict(obj=1,fl=FL_SNOW)
def C46(L):  # Armata
    L.flat(8);L.pool(18,P4,.3);L.e('TANK',L.W+8);L.flat(16);L.cache();L.checkpoint();L.pool(14,P4,.3);L.high_path(2);L.bunker()
    return dict(obj=0,boss=3,arena=arena_flat,fl=FL_SNOW)
def C47(L):  # Iron Train - final
    L.flat(8);L.pool(16,P4,.3);L.e('HELI',L.W,3);L.flat(10);L.cache();L.checkpoint();L.pool(16,P4,.35);L.e('SWARM',L.W,2);L.flat(10);L.high_path(2);L.bunker()
    return dict(obj=0,boss=4,arena=arena_rail,fl=FL_SNOW|FL_NIGHT)
WORLDS=[('DESERT STORM',6,[('LINE OF DEPARTURE',1,C11),('BERM BREACH',1,C12),('CONVOY: MSR TAMPA',2,C13),('MINEFIELD LANES',2,C14),('OIL FIELD FIRES',2,C15),('73 EASTING',3,C16),('MEDINA RIDGE',3,C17)]),
        ('FULDA FOREST',9,[('FULDA GAP',2,C21),('FOREST TRAIL AMBUSH',2,C22),('WESER FORD',3,C23),('THE WEAK BRIDGE',3,C24),('VILLAGE CROSSROADS',3,C25),('HILL 402 NIGHT RAID',4,C26),('GUNSHIP VALLEY',4,C27)]),
        ('CITY FIGHT',10,[('CITY OUTSKIRTS',3,C31),('IED ALLEY',3,C32),('MARKET STREET',4,C33),('ROOFTOP GUNSHIPS',4,C34),('HIGHWAY OVERPASS',4,C35),('RUBBLE DISTRICT',5,C36),('T-90M PLAZA',5,C37)]),
        ('FROZEN FRONT',8,[('FROZEN STEPPE',4,C41),('ICE RIVER FORD',4,C42),('BIRCH FOREST NIGHT',5,C43),('FROZEN AIRFIELD',5,C44),('BLIZZARD RUN',5,C45),('ARMATA',5,C46),('IRON TRAIN',5,C47)])]
def build(wi,ci,fn,diff):
    L=Lv(wi*31+ci*7+5);info=fn(L)
    # top up hidden intel to exactly 3 (secret spots); extra ones become score-only (drop)
    intel=[e for e in L.ents if e[0]=='INTEL']
    while len(intel)<3:L.high_path(2);intel=[e for e in L.ents if e[0]=='INTEL']
    if len(intel)>3:
        for e in intel[3:]:L.ents.remove(e)
    ext=(0,10,18,26)[wi]
    if ext and info.get('obj',0) not in (3,):L.checkpoint() if L.W>150 and wi>=2 else None;L.pool(ext,(P1,P2,P3,P4)[wi],.35)
    L.finish(info.get('boss'),info.get('arena'))
    assert L.W<=352,(fn.__name__,L.W)
    ents=[]
    for name,x,row,tier in L.ents:
        if name=='BOSS':ents.append((EID[name],x*16,row));continue
        if row is None:
            t=L.top(x)
            if t>=H or L.g[t][x] in '~w':   # find nearest standable column
                for d in (1,-1,2,-2,3,-3):
                    if 0<=x+d<L.W and L.top(x+d)<H and L.g[L.top(x+d)][x+d] not in '~w':x+=d;t=L.top(x);break
            row=max(0,t-1)
        ents.append((EID[name],x*16+8,row|(tier<<6)))
    ents.sort(key=lambda e:e[1])
    rle=[]
    for y in range(H):
        row=L.g[y];x=0
        while x<len(row):
            c=row[x];n=1
            while x+n<len(row) and row[x+n]==c and n<255:n+=1
            rle+=[TID[c],n];x+=n
    obj=info.get('obj',0);objn=info.get('objn',0)
    if obj==4:objn=sum(1 for e in ents if e[0] in (EID['AIRC'],EID['RADAR'],EID['SAM']))
    if obj==3:objn=sum(1 for e in ents if e[0]==EID['FLEE'])
    par=int(L.W*16/1.25)+(1500 if obj==2 else 700 if obj==5 else 0)+(1500 if info.get('boss') is not None else 0)
    if obj==1:par=int(par*1.5)
    return L,rle,ents,obj,objn,info.get('fl',0),par,info.get('boss')
# ---------------------------------------------------------------- output
o=["// generated by tools/gen_brad.py - do not edit"]
o.append("#define BR_NW 4\n#define BR_NC 7\n#define BR_NVAR 5\n#define BR_NBOSS 5")
for nm,c in CAMO.items():o.append("static const u16 brp_%s[16]={%s};"%(nm.lower(),",".join(str(rgb15(x)) for x in campal(c))))
o.append("static const u16*const BR_CAMO[3]={brp_desert,brp_woodland,brp_winter};static const char*const BR_CAMON[3]={\"DESERT\",\"WOODLAND\",\"WINTER\"};")
for nm,p in (('enemy',ENEM),('misc',MISC),('burnt',BURNT),('hot',HOT)):o.append("static const u16 brp_%s[16]={%s};"%(nm,",".join(str(rgb15(c)) for c in p)))
o.append("static const char*const BR_VARN[5]={%s};"%",".join('"%s"'%v for v in VAR))
o.append("static const char*const BR_BOSSN[5]={%s};"%",".join('"%s"'%v for v in BOSSN))
def arr(n,t,a,fmt="0x%08X"):return "static const %s %s[%d]={%s};"%(t,n,len(a),",".join(fmt%v for v in a))
o.append(arr('br_veh','u32',VEH));o.append(arr('br_barrel','u32',BAR));o.append(arr('br_boss','u32',BOSS));o.append(arr('br_com','u32',COM))
for k,v in idx.items():o.append("#define BT_%s %d"%(k,v))
for wi,(fd,fm,fn,FP,md,mm,mn,MP) in enumerate(BGD):
    o.append(arr('brbf_t%d'%wi,'u32',fd));o.append(arr('brbf_m%d'%wi,'u16',fm,"%d"));o.append(arr('brbf_p%d'%wi,'u16',[rgb15(c) for c in FP],"%d"))
    o.append(arr('brbm_t%d'%wi,'u32',md));o.append(arr('brbm_m%d'%wi,'u16',mm,"%d"));o.append(arr('brbm_p%d'%wi,'u16',[rgb15(c) for c in MP],"%d"))
o.append("static const u32*const BRBF_T[4]={brbf_t0,brbf_t1,brbf_t2,brbf_t3};static const u16*const BRBF_M[4]={brbf_m0,brbf_m1,brbf_m2,brbf_m3};static const u16*const BRBF_P[4]={brbf_p0,brbf_p1,brbf_p2,brbf_p3};")
o.append("static const u32*const BRBM_T[4]={brbm_t0,brbm_t1,brbm_t2,brbm_t3};static const u16*const BRBM_M[4]={brbm_m0,brbm_m1,brbm_m2,brbm_m3};static const u16*const BRBM_P[4]={brbm_p0,brbm_p1,brbm_p2,brbm_p3};")
o.append("static const u16 BRBF_N[4]={%s},BRBM_N[4]={%s};"%(",".join(str(b[2]) for b in BGD),",".join(str(b[6]) for b in BGD)))
o.append("enum{"+",".join("BE_"+k for k in EN)+",BE_N};")
o.append("enum{BOB_REACH,BOB_ESCORT,BOB_DEFEND,BOB_CHASE,BOB_TARGETS,BOB_DISMOUNT};")
o.append("#define FL_NIGHT 1\n#define FL_SAND 2\n#define FL_SNOW 4\n#define FL_RAIN 8")
o.append("typedef struct{u8 k;u16 x;u8 y;}BrEnt;")
o.append("typedef struct{const char*n;u8 th,stars,boss;u16 w;const u8*t;const BrEnt*e;u16 ne;u8 obj,objn,fl;u16 par;}BrLv;")
cl=[];stats=[]
for wi,(wn,th,courses) in enumerate(WORLDS):
    for ci,(name,diff,fn) in enumerate(courses):
        L,rle,ents,obj,objn,fl,par,boss=build(wi,ci,fn,diff)
        o.append("static const u8 brt_%d_%d[%d]={%s};"%(wi,ci,len(rle),",".join(map(str,rle))))
        o.append("static const BrEnt bre_%d_%d[%d]={%s};"%(wi,ci,len(ents),",".join("{%d,%d,%d}"%e for e in ents)))
        cl.append('{"%s",%d,%d,%d,%d,brt_%d_%d,bre_%d_%d,%d,%d,%d,%d,%d}'%(name,th,diff,255 if boss is None else boss,L.W,wi,ci,wi,ci,len(ents),obj,objn,fl,par))
        nh=sum(1 for e in ents if e[0]<=EID['SAM'])
        stats.append((name,L.W,len(ents),nh,OBJN[obj],objn,fl,par))
        im=Image.new('RGB',(L.W*3,H*3));CC={'.':(120,160,210),'#':(110,80,50),'B':(180,90,60),'S':(90,90,90),'-':(200,180,120),'~':(60,110,200),'w':(40,80,170),'C':(200,120,60)}
        for y in range(H):
            for x in range(L.W):
                for a in range(3):
                    for b in range(3):im.putpixel((x*3+a,y*3+b),CC[L.g[y][x]])
        for e in ents:
            col=(255,255,0) if e[0]==EID['INTEL'] else (0,255,0) if e[0]>EID['SAM'] else (255,0,0)
            for a in range(2):
                for b in range(2):im.putpixel((min(L.W*3-1,e[1]*3//16+a),min(H*3-1,(e[2]&31)*3+1+b)),col)
        im.resize((im.width*2,im.height*2),Image.NEAREST).save(os.path.join(ROOT,'build','brad_course_%d_%d.png'%(wi+1,ci+1)))
o.append("static const BrLv BR_LV[BR_NW*BR_NC]={%s};"%",".join(cl))
o.append("static const char*const BR_WN[BR_NW]={%s};static const u8 BR_WCAMO[BR_NW]={0,1,0,2};"%",".join('"%s"'%w[0] for w in WORLDS))
open(os.path.join(ROOT,'src','braddb.inc'),'w').write("\n".join(o)+"\n")
# preview sheet
PALS={'cam':campal(CAMO['WOODLAND']),'en':ENEM,'misc':MISC,'hot':HOT}
cw=68;chh=36;cols=10;sc=3
rows=(len(sheet)+cols-1)//cols
W=Image.new('RGB',(cols*cw*sc,rows*chh*sc),(96,140,200))
for i,(nm,g,pk) in enumerate(sheet):
    pl=PALS[pk]
    for y,row in enumerate(g[:chh]):
        for x,c in enumerate(row[:cw]):
            if c:
                for a in range(sc):
                    for b in range(sc):W.putpixel(((i%cols)*cw*sc+x*sc+a,(i//cols)*chh*sc+y*sc+b),pl[c])
W.save(os.path.join(ROOT,'build','brad_sheet.png'))
for s in stats:print("  %-22s w%3d ents%3d hostile%3d %-8s n%d fl%d par%ds"%(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]//60))
print("brad: veh %d tiles, boss %d, com %d/720, bg far %s mid %s"%(len(VEH)//8,len(BOSS)//8,len(COM)//8,[b[2] for b in BGD],[b[6] for b in BGD]))
