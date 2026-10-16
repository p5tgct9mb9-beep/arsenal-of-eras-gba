#!/usr/bin/env python3
# v1.3 SPEC OPS: BRADLEY - sprites (4bpp OBJ, 1D) + 28 courses -> src/braddb.inc ; preview build/brad_sheet.png
import sys,os,random
from PIL import Image
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# palette: 0 trans 1 outline 2 track dark 3 track mid 4 steel light 5 camo base 6 camo dark 7 camo light 8 metal dark
#          9 red 10 yellow 11 white 12 glass 13 black 14 orange 15 skin
BASE=[(0,0,0),(20,18,22),(46,44,48),(84,82,86),(150,150,154),(0,0,0),(0,0,0),(0,0,0),(62,64,60),(200,50,40),(246,210,70),(240,240,236),(110,170,210),(14,14,16),(246,140,40),(214,170,130)]
CAMO={'DESERT':[(196,170,118),(150,122,80),(222,202,152)],'WOODLAND':[(88,102,60),(84,66,44),(44,48,32)],'WINTER':[(214,218,222),(146,152,158),(92,98,104)]}
ENEM=[(86,100,58),(58,68,40),(126,136,88)]
MISC=[(98,110,66),(70,70,74),(206,206,206)]
def pal(c3):p=list(BASE);p[5:8]=c3;return p
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
def outl(g):
    h=len(g);w=len(g[0]);o=[r[:] for r in g]
    for y in range(h):
        for x in range(w):
            if g[y][x]==0 and any(0<=y+dy<h and 0<=x+dx<w and g[y+dy][x+dx] not in (0,1) for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=1
    return o
def camo(g,x0,y0,x1,y1,seed):
    r=random.Random(seed)
    for _ in range((x1-x0)*(y1-y0)//18):
        x=r.randint(x0,x1);y=r.randint(y0,y1);c=r.choice((6,7))
        for dx in range(r.randint(1,3)):
            if g[y][min(x+dx,x1)]==5:g[y][min(x+dx,x1)]=c
def tracks(g,x0,x1,y0,f,nw=6,wr=3):
    POLY(g,[(x0+2,y0),(x1-2,y0),(x1,y0+3),(x1-3,y0+7),(x0+3,y0+7),(x0,y0+3)],2)
    sp=(x1-x0-8)/(nw-1)
    for i in range(nw):
        cx=int(x0+4+i*sp);E(g,cx-wr,y0+3-wr+1,cx+wr,y0+3+wr,3);P(g,cx,y0+3,4)
        P(g,cx+(1 if f else -1),y0+3-1,8)
    for x in range(x0+2,x1-1,2):P(g,x+(f&1),y0,13);P(g,x+((f+1)&1),y0+7,13)
# ---------------------------------------------------------------- Bradley family (64x32, facing right)
VAR=['M2A2 ODS','M2A3','M2A4','M3A3 CFV','M7 BFIST']
def bradley(v,f):
    g=G(64,32)
    tracks(g,4,58,23,f,6,3)
    POLY(g,[(4,13),(48,13),(58,18),(60,22),(57,24),(4,24)],5)            # hull, sloped glacis
    R(g,6,19,55,23,5);L(g,8,19,54,19,6)                                    # side armour skirt line
    for x in range(10,54,9):L(g,x,19,x,23,6)                               # skirt panel seams
    if v!=3:
        for x in (16,26):R(g,x,15,x+2,16,12)                                # firing-port vision blocks
    R(g,3,14,5,22,6)                                                        # rear ramp
    POLY(g,[(22,7),(42,7),(46,12),(20,13)],5);R(g,20,12,44,13,6)            # turret
    R(g,44,9,46,11,8)                                                       # gun mantlet
    if v==4:E(g,13,4,21,11,8);R(g,15,6,19,8,12)                             # FS3 sensor head
    else:R(g,12,5,21,11,6);R(g,13,6,20,6,7);R(g,12,10,21,10,8)              # TOW launcher box
    R(g,28,5,32,6,8)                                                        # commander hatch
    if v in(1,2,3):R(g,30,1,34,4,8);R(g,31,2,33,3,12)                       # CIV sight (A3+)
    if v==2:R(g,4,12,46,13,8);R(g,20,13,44,14,6)                            # A4: added armour + bustle
    if v==3:L(g,16,6,14,0,13);L(g,24,6,24,1,13)                             # CFV extra antennas
    L(g,10,7,8,0,13)                                                        # antenna
    camo(g,4,7,58,24,v*7+1)
    return g
def era_overlay():
    g=G(64,16)
    for x in range(8,54,6):
        for y in (2,7):R(g,x,y,x+4,y+3,8);R(g,x,y,x+4,y,4)
    return g
def busk():
    g=G(16,16);R(g,2,2,13,13,8);R(g,4,4,11,11,12);R(g,2,2,13,2,4);return g
# ---------------------------------------------------------------- bosses / enemy armour (64x32, facing right)
def t72(f,kind=0):
    g=G(64,32);tracks(g,4,58,23,f,6,3)
    POLY(g,[(4,16),(50,16),(60,20),(58,24),(4,24)],5);R(g,6,20,55,23,6)
    E(g,20,8,42,17,5);R(g,24,8,38,9,7)                                      # dome turret
    R(g,42,11,63,12,8);R(g,58,10,63,13,8)                                   # 125mm gun + muzzle
    if kind==1:                                                             # T-90M: angular turret + ERA wedges + cage
        POLY(g,[(18,10),(40,8),(46,13),(44,17),(18,17)],5)
        for x in range(22,44,5):R(g,x,9,x+3,11,7)
        for x in range(2,14,3):L(g,x,10,x,17,8)
        R(g,30,5,34,7,8);R(g,31,6,33,6,12)
    else:
        for x in range(22,40,5):R(g,x,11,x+3,13,6)                          # Kontakt bricks
    camo(g,4,8,58,24,11+kind)
    return g
def t14(f):
    g=G(64,32);tracks(g,2,60,23,f,7,3)
    POLY(g,[(2,14),(46,14),(60,19),(60,23),(2,23)],5);L(g,4,18,56,18,6)
    POLY(g,[(18,8),(40,8),(46,12),(44,15),(16,15)],5);R(g,20,9,40,9,7)      # boxy unmanned turret
    R(g,46,10,63,11,8);R(g,59,9,63,12,8)
    for x in (22,28,34):R(g,x,6,x+2,7,8)                                    # Afganit APS launchers
    R(g,38,5,41,7,12)
    camo(g,2,8,60,23,17);return g
def bmp(f,kind=0):
    g=G(64,32);tracks(g,6,56,23,f,6,3)
    POLY(g,[(6,14),(42,14),(58,20),(54,24),(6,24)],5)
    for x in range(44,56,3):L(g,x,16+(x-44)//3,x+2,18+(x-44)//3,6)          # ribbed bow
    if kind==1:                                                              # BMP-3: 100mm + 30mm coax
        POLY(g,[(20,8),(38,8),(42,13),(18,14)],5);R(g,42,9,62,10,8);R(g,42,12,56,12,8)
    else:R(g,22,9,36,13,5);R(g,36,10,58,10,8);R(g,24,6,32,7,8)              # BMP-2 30mm + ATGM tube
    camo(g,6,8,56,24,23+kind);return g
def btr(f):
    g=G(64,32)
    POLY(g,[(4,13),(46,13),(60,19),(56,24),(4,24)],5);L(g,6,18,54,18,6)
    for i,x in enumerate((12,24,38,50)):E(g,x-4,22,x+4,30,2);E(g,x-2,24,x+2,28,3);P(g,x+(1 if f else -1),26,4)
    R(g,26,8,36,12,5);R(g,36,9,50,9,8);R(g,22,15,24,16,12)
    camo(g,4,8,60,24,29);return g
def hind(f):
    g=G(64,32)
    POLY(g,[(8,12),(40,10),(54,12),(60,16),(54,20),(30,22),(10,18)],5)      # fuselage
    R(g,0,13,12,15,5);R(g,0,8,3,15,5)                                       # tail boom + fin
    E(g,46,11,54,16,12);E(g,38,9,44,13,12)                                  # tandem canopies
    R(g,24,20,40,21,8);R(g,26,22,30,24,8);R(g,34,22,38,24,8)                # stub wings + pods
    R(g,56,18,58,20,8)                                                      # chin gun
    if f:R(g,6,6,60,6,13)
    else:R(g,18,6,48,6,13)
    R(g,32,7,33,9,8)
    if f:R(g,1,5,1,10,13)
    else:R(g,0,8,4,8,13)
    E(g,14,20,18,24,2)
    camo(g,8,8,60,22,31);return g
def technical(f):
    g=G(32,16);R(g,2,7,28,11,5);R(g,18,3,26,7,5);R(g,20,4,25,6,12);E(g,5,10,10,15,2);E(g,20,10,25,15,2)
    P(g,7+f,12,4);P(g,22+f,12,4);R(g,6,4,8,7,8);R(g,8,4,15,4,8);return g
# ---------------------------------------------------------------- infantry 16x16 (facing right)
def soldier(f,kind,ally=False):
    g=G(16,16);c=5
    E(g,6,1,9,4,6 if not ally else 6);R(g,6,4,9,4,15 if ally else 15)           # helmet + face
    R(g,6,5,9,10,c);R(g,5,6,10,9,c)
    if kind=='kneel':R(g,6,11,9,12,c);R(g,9,12,11,13,c);R(g,4,13,7,14,c)
    else:
        a=1 if f else 0;L(g,6,11,5-a,15,c);L(g,9,11,10+a,15,c);P(g,5-a,15,2);P(g,10+a,15,2)
    if kind=='rifle':L(g,8,7,14,7,8)
    elif kind=='rpg':R(g,2,4,13,5,6);R(g,13,3,14,6,8)
    elif kind=='atgm':R(g,10,9,15,10,8);L(g,11,11,10,14,8);L(g,13,11,14,14,8)
    elif kind=='ally':L(g,8,7,14,6,8)
    return g
def orlan(f):
    g=G(16,16);R(g,2,7,13,8,11);R(g,6,5,7,10,4);R(g,1,6,1,9,4);R(g,13,4+f,13,11-f,13);return g
def fpv(f):
    g=G(16,16);R(g,5,7,10,9,8);R(g,7,6,8,6,9)
    for x in (2,12):R(g,x-1-f,4,x+1+f,4,13);L(g,x,5,7 if x<7 else 8,7,8)
    return g
def mine(_):g=G(16,8);E(g,3,3,12,7,8);R(g,6,2,9,3,4);P(g,7,1,9);return g
def ied(_):g=G(16,8);E(g,1,4,8,7,7);E(g,6,2,14,7,6);L(g,10,1,15,0,9);P(g,4,5,1);return g
# ---------------------------------------------------------------- pickups / markers
def crate(k):
    g=G(16,16);R(g,1,4,14,14,[6,4,10][k]);R(g,1,4,14,5,[7,11,14][k])
    if k==0:R(g,4,7,11,8,11);R(g,7,7,8,12,11)                           # T (TOW)
    if k==1:R(g,4,8,11,10,9);R(g,6,6,9,12,9)                            # repair cross
    if k==2:g=G(16,16);POLY(g,[(8,1),(10,6),(15,6),(11,9),(13,15),(8,11),(3,15),(5,9),(1,6),(6,6)],10);E(g,6,6,9,9,14)
    return g
def flag(k):
    g=G(16,32);R(g,2,0,3,31,4)
    if k==0:POLY(g,[(4,1),(14,4),(4,8)],7);R(g,1,30,5,31,8)             # checkpoint pennant (no emblem)
    else:
        for y in range(1,13):
            for x in range(4,15):g[y][x]=11 if ((x//3)+(y//3))%2 else 13
        R(g,0,29,6,31,8)
    return g
def smoke(k):
    g=G(32,32);r=random.Random(40+k)
    for _ in range(14):x=r.randint(6,26);y=r.randint(8+k*2,26);s=r.randint(3,6);E(g,x-s,y-s,x+s,y+s,6 if r.random()<.5 else 7)
    for _ in range(6):x=r.randint(8,24);y=r.randint(10,24);E(g,x-2,y-2,x+2,y+2,7)
    return g
def boom(k):
    g=G(32,32);r=[6,11,14][k];E(g,16-r,16-r,16+r,16+r,[10,14,2][k]);E(g,16-r//2,16-r//2,16+r//2,16+r//2,[11,10,3][k])
    if k==2:
        for (x,y) in((8,10),(22,8),(24,22),(10,22)):E(g,x-3,y-3,x+3,y+3,3)
    return g
def tiny(k):
    g=G(8,8)
    if k=='round':R(g,2,3,6,4,10);P(g,1,3,14);P(g,7,3,11)
    elif k=='ebul':R(g,2,3,5,4,9)
    elif k=='shell':R(g,1,3,6,4,14);P(g,7,3,10);P(g,7,4,10)
    elif k=='arty':R(g,3,1,4,6,8);P(g,3,7,14);P(g,4,7,14)
    elif k=='warn':POLY(g,[(4,0),(7,7),(0,7)],9);R(g,3,3,4,5,10)
    elif k=='barrel':R(g,2,2,5,5,8);R(g,2,2,5,2,4)
    elif k=='spark':P(g,3,3,11);P(g,4,4,10);P(g,2,4,10);P(g,4,2,10);P(g,3,5,14)
    elif k=='dot':R(g,3,3,4,4,9)
    return g
def missile(k):
    g=G(16,8)
    if k=='tow':R(g,3,3,12,4,11);R(g,12,3,14,4,4);R(g,4,2,6,5,4);P(g,1,3,10);P(g,2,4,14)
    elif k=='rpg':R(g,6,2,13,5,6);R(g,13,3,15,4,6);R(g,2,3,6,4,8);P(g,0,3,14)
    elif k=='atgm':R(g,3,3,12,4,8);R(g,12,3,13,4,9);R(g,4,1,5,6,8);P(g,1,3,9);P(g,2,4,10)
    elif k=='rocket':R(g,3,3,12,4,4);P(g,13,3,9);P(g,1,3,14);P(g,2,4,10)
    return g
# ---------------------------------------------------------------- emit tiles
def tiles(g,w,h):
    out=[]
    for ty in range(h//8):
        for tx in range(w//8):
            for y in range(8):
                v=0
                for x in range(8):v|=(g[ty*8+y][tx*8+x]&15)<<(4*x)
                out.append(v)
    return out
sheet=[]
def blob(g,w,h,o=True):
    gg=outl(g) if o else g;sheet.append(gg);return tiles(gg,w,h)
VEH=[];[VEH.extend(blob(bradley(v,f),64,32)) for v in range(5) for f in range(2)]
BOSSN=['T-72B','BMP-3','MI-24 HIND','T-90M','T-14 ARMATA']
BOSSF=[lambda f:t72(f,0),lambda f:bmp(f,1),hind,lambda f:t72(f,1),t14]
BOSS=[];[BOSS.extend(blob(BOSSF[b](f),64,32)) for b in range(5) for f in range(2)]
COM=[];idx={}
def add(name,g,w,h,o=True):idx[name]=len(COM)//8;COM.extend(blob(g,w,h,o))
add('ERA',era_overlay(),64,16);add('BUSK',busk(),16,16)
for f in range(2):add('BTR%d'%f,btr(f),64,32)
for f in range(2):add('BMP2_%d'%f,bmp(f,0),64,32)
for f in range(2):add('TECH%d'%f,technical(f),32,16)
for k in ('rifle','rpg','atgm','ally'):
    for f in range(2):add('%s%d'%(k.upper(),f),soldier(f,'kneel' if k=='atgm' else k,k=='ally'),16,16)
for f in range(2):add('ORLAN%d'%f,orlan(f),16,16)
for f in range(2):add('FPV%d'%f,fpv(f),16,16)
add('MINE',mine(0),16,8);add('IED',ied(0),16,8)
for k in range(3):add('CRATE%d'%k,crate(k),16,16)
for k in range(2):add('FLAG%d'%k,flag(k),16,32)
for k in range(2):add('SMOKE%d'%k,smoke(k),32,32)
for k in range(3):add('BOOM%d'%k,boom(k),32,32)
for k in ('round','ebul','shell','arty','warn','barrel','spark','dot'):add(k.upper(),tiny(k),8,8,False)
for k in ('tow','rpg','atgm','rocket'):add('M_'+k.upper(),missile(k),16,8)
assert len(COM)//8<=440,len(COM)//8
# ---------------------------------------------------------------- courses
TID={'.':0,'#':1,'B':3,'S':6,'-':7,'~':10,'w':11,'C':21}
EN=['INF','RPG','ATGM','TECH','BTR','BMP2','ORLAN','FPV','MINE','IED','TOWC','REP','MEDAL','CHECK','GOAL','BOSS']
EID={k:i for i,k in enumerate(EN)}
H=12
WORLDS=[('DESERT STORM',6,'DESERT',[('LINE OF DEPARTURE',1),('BERM BREACH',2),('WADI RUN',2),('MINEFIELD LANES',3),('OIL FIELD FIRES',3),('73 EASTING',4),('MEDINA RIDGE',4,0)]),
 ('FULDA FOREST',9,'WOODLAND',[('FULDA GAP',2),('FOREST TRAIL',2),('WESER FORD',3),('THE WEAK BRIDGE',3),('VILLAGE CROSSROADS',3),('HILL 402',4),('BMP-3 COLUMN',4,1)]),
 ('CITY FIGHT',4,'DESERT',[('CITY OUTSKIRTS',2),('IED ALLEY',3),('MARKET STREET',3),('HIND OVER THE CITY',4,2),('HIGHWAY OVERPASS',4),('RUBBLE DISTRICT',4),('T-90M PLAZA',5,3)]),
 ('FROZEN FRONT',8,'WINTER',[('FROZEN STEPPE',3),('ICE RIVER FORD',3),('BIRCH FOREST',3),('AT DITCH LINE',4),('BLIZZARD RUN',4),('RAIL YARD',5),('ARMATA',5,4)])]
class Lv:
    def __init__(s):s.g=[[] for _ in range(H)];s.h=9;s.ents=[]
    @property
    def W(s):return len(s.g[0])
    def col(s,top,fill='#',surf=None):
        for y in range(H):s.g[y].append(fill if y>=top else '.')
        if surf and top<H:s.g[top][-1]=surf
    def flat(s,n):
        for _ in range(n):s.col(s.h)
    def ent(s,k,x,y=None):s.ents.append((EID[k],x*16+8,(s.h-1) if y is None else y))
def build(wi,ci,name,diff,boss,urban,snow):
    r=random.Random(wi*100+ci*7+3);L=Lv();L.flat(8)
    target=150+diff*14+ci*4;medals=0;checked=False
    kinds=['flat','step','ditch','ford','bridge','wall','mound','mines']
    if urban:kinds+=['wall','plateau','ied','ied']
    if wi==1 or snow:kinds+=['ford','bridge']
    pool=['INF','INF','RPG','TECH']+(['ATGM','BTR'] if diff>=2 else [])+(['BMP2','ORLAN'] if diff>=3 else [])+(['FPV','FPV'] if diff>=4 else [])
    while L.W<target-30:
        k=r.choice(kinds);x0=L.W
        if k=='flat':L.flat(r.randint(4,8))
        elif k=='step':
            dh=r.choice((-1,1));L.h=max(6,min(10,L.h+dh));L.flat(r.randint(3,6))
        elif k=='ditch':
            L.flat(2);w=r.randint(2,min(4,1+diff));[L.col(H) for _ in range(w)];L.flat(4)
        elif k=='ford':
            L.flat(2);w=r.randint(4,8)
            for _ in range(w):L.col(L.h+1,'#');L.g[L.h][-1]='~'
            L.flat(3)
        elif k=='bridge':
            L.flat(2);w=r.randint(5,9);weak=r.random()<.35+diff*.08
            for _ in range(w):L.col(H);L.g[L.h][-1]='C' if weak else '-'
            L.flat(3)
        elif k=='wall':
            L.flat(3);hh=r.randint(1,2)
            L.col(L.h);[L.g.__getitem__(L.h-1-j).__setitem__(-1,'B') for j in range(hh)];L.flat(4)
        elif k=='mound':
            L.h-=1;L.flat(r.randint(2,4));L.h+=1;L.flat(2)
        elif k=='plateau':
            h0=L.h;L.h-=1;L.flat(2);L.h-=1;L.flat(r.randint(3,6));L.h+=1;L.flat(2);L.h=h0;L.flat(2)
        elif k=='mines':
            L.flat(8);[L.ent('MINE',x0+2+j*2) for j in range(r.randint(1,2+diff//2))]
        elif k=='ied':
            L.flat(7);L.ent('IED',x0+3+r.randint(0,2))
        # enemies on the new stretch
        x1=L.W
        for x in range(x0+2,x1-1,3):
            if r.random()<0.16+diff*0.05:
                e=r.choice(pool);top=next(y for y in range(H) if L.g[y][x]!='.') if any(L.g[y][x]!='.' for y in range(H)) else H
                if top>=H or L.g[top][x] in '-C~':continue
                if e in('ORLAN','FPV'):L.ents.append((EID[e],x*16+8,max(1,top-4)))
                else:L.ents.append((EID[e],x*16+8,top-1))
        if medals<3 and r.random()<.22 and L.W>30:L.ents.append((EID['MEDAL'],(L.W-2)*16+8,L.h-4));medals+=1
        if r.random()<.10:L.ents.append((EID[r.choice(('TOWC','REP'))],(L.W-2)*16+8,L.h-1))
        if not checked and L.W>target//2:L.flat(3);L.ent('CHECK',L.W-2);checked=True
    while medals<3:L.flat(6);L.ents.append((EID['MEDAL'],(L.W-3)*16+8,L.h-4));medals+=1
    L.h=9;L.flat(4)
    if boss is not None:
        L.ent('REP',L.W-2);L.ent('TOWC',L.W-1);bx=L.W;L.flat(22);L.ents.append((EID['BOSS'],bx*16,boss))
    else:L.flat(6)
    L.ent('GOAL',L.W-4);L.flat(8)
    assert L.W<=352,L.W
    rle=[]
    for y in range(H):
        row=L.g[y];x=0
        while x<len(row):
            c=row[x];n=1
            while x+n<len(row) and row[x+n]==c and n<255:n+=1
            rle+= [TID[c],n];x+=n
    ents=sorted(L.ents,key=lambda e:e[1])
    return L,rle,ents
o=["// generated by tools/gen_brad.py - do not edit"]
o.append("#define BR_NW 4\n#define BR_NC 7\n#define BR_NVAR 5\n#define BR_NBOSS 5")
def rgb15(c):return (c[0]>>3)|((c[1]>>3)<<5)|((c[2]>>3)<<10)
for nm,c3 in CAMO.items():o.append("static const u16 brp_%s[16]={%s};"%(nm.lower(),",".join(str(rgb15(c)) for c in pal(c3))))
o.append("static const u16*const BR_CAMO[3]={brp_desert,brp_woodland,brp_winter};static const char*const BR_CAMON[3]={\"DESERT\",\"WOODLAND\",\"WINTER\"};")
o.append("static const u16 brp_enemy[16]={%s};"%",".join(str(rgb15(c)) for c in pal(ENEM)))
o.append("static const u16 brp_misc[16]={%s};"%",".join(str(rgb15(c)) for c in pal(MISC)))
o.append("static const char*const BR_VARN[5]={%s};"%",".join('"%s"'%v for v in VAR))
o.append("static const char*const BR_BOSSN[5]={%s};"%",".join('"%s"'%v for v in BOSSN))
o.append("static const u32 br_veh[%d]={%s};"%(len(VEH),",".join("0x%08X"%v for v in VEH)))
o.append("static const u32 br_boss[%d]={%s};"%(len(BOSS),",".join("0x%08X"%v for v in BOSS)))
o.append("static const u32 br_com[%d]={%s};"%(len(COM),",".join("0x%08X"%v for v in COM)))
for k,v in idx.items():o.append("#define BT_%s %d"%(k,v))
o.append("enum{"+",".join("BE_"+k for k in EN)+",BE_N};")
o.append("typedef struct{u8 k;u16 x;u8 y;}BrEnt;")
o.append("typedef struct{const char*n;u8 th,stars,boss;u16 w;const u8*t;const BrEnt*e;u16 ne;}BrLv;")
cl=[];names=[]
stats=[]
for wi,(wn,th,cm,courses) in enumerate(WORLDS):
    for ci,c in enumerate(courses):
        name,diff=c[0],c[1];boss=c[2] if len(c)>2 else None
        L,rle,ents=build(wi,ci,name,diff,boss,th==4,th==8)
        o.append("static const u8 brt_%d_%d[%d]={%s};"%(wi,ci,len(rle),",".join(map(str,rle))))
        o.append("static const BrEnt bre_%d_%d[%d]={%s};"%(wi,ci,len(ents),",".join("{%d,%d,%d}"%e for e in ents)))
        cl.append('{"%s",%d,%d,%d,%d,brt_%d_%d,bre_%d_%d,%d}'%(name,th,diff,255 if boss is None else boss,L.W,wi,ci,wi,ci,len(ents)))
        stats.append((name,L.W,len(ents)))
        if wi*7+ci in (0,10,20,27):
            im=Image.new('RGB',(L.W*2,H*2));C={'.':(120,160,210),'#':(110,80,50),'B':(180,90,60),'S':(90,90,90),'-':(200,180,120),'~':(60,110,200),'C':(200,120,60)}
            for y in range(H):
                for x in range(L.W):
                    for a in range(2):
                        for b in range(2):im.putpixel((x*2+a,y*2+b),C[L.g[y][x]])
            for e in ents:
                for a in range(2):im.putpixel((min(L.W*2-1,e[1]//8),min(H*2-1,e[2]*2+a)),(255,0,0))
            im.resize((im.width*3,im.height*3)).save(os.path.join(ROOT,'build','brad_course_%d_%d.png'%(wi+1,ci+1)))
o.append("static const BrLv BR_LV[BR_NW*BR_NC]={%s};"%",".join(cl))
o.append("static const char*const BR_WN[BR_NW]={%s};static const u8 BR_WCAMO[BR_NW]={0,1,0,2};"%",".join('"%s"'%w[0] for w in WORLDS))
open(os.path.join(ROOT,'src','braddb.inc'),'w').write("\n".join(o)+"\n")
# preview sheet
PV=pal(CAMO['WOODLAND'])
W=Image.new('RGB',(8*68*3,((len(sheet)+7)//8)*36*3),(60,90,140))
for i,g in enumerate(sheet):
    for y,row in enumerate(g):
        for x,c in enumerate(row):
            if c:
                for a in range(3):
                    for b in range(3):W.putpixel(((i%8)*68*3+x*3+a,(i//8)*36*3+y*3+b),PV[c])
W.save(os.path.join(ROOT,'build','brad_sheet.png'))
print("brad: %d veh tiles, %d boss tiles, %d common tiles, 28 courses, width %d-%d, ents %d"%(len(VEH)//8,len(BOSS)//8,len(COM)//8,min(s[1] for s in stats),max(s[1] for s in stats),sum(s[2] for s in stats)))
