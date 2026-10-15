import re,math
#!/usr/bin/env python3
"""Arsenal of Eras GBA - procedural asset generator.
Every tile, sprite, palette and the title illustration is drawn here from code
(shapes + hash noise); nothing is imported from outside. Writes src/assets.c/.h."""
import math, os
from PIL import Image, ImageDraw
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
C=[];Hh=[]
def rgb15(c):r,g,b=c;return (r>>3)|((g>>3)<<5)|((b>>3)<<10)
def emit(name,arr,t="unsigned short"):
    Hh.append(f"extern const {t} {name}[{len(arr)}];")
    s=",".join(str(v) for v in arr)
    C.append(f"const {t} {name}[{len(arr)}] __attribute__((aligned(4)))={{{s}}};")
def define(n,v):Hh.append(f"#define {n} {v}")
def pal16(cols):
    cols=list(cols)+[(0,0,0)]*(16-len(cols));return [rgb15(c) for c in cols[:16]]
def H(*a):
    n=0x9E3779B1
    for v in a:n=((n^(int(v)&0xffffffff))*0x85EBCA77)&0xffffffff;n^=n>>13
    n=(n*0xC2B2AE3D)&0xffffffff;n^=n>>16;return n/4294967296.0
def tiles4(g,w,h):
    """g: rows of indices (w x h px, multiples of 8). Returns u32 words, tiles in row-major 8x8 order (1D obj / bg)."""
    out=[]
    for ty in range(h//8):
        for tx in range(w//8):
            for y in range(8):
                v=0
                for x in range(8):v|=(g[ty*8+y][tx*8+x]&15)<<(4*x)
                out.append(v)
    return out
def meta_tiles(g):  # 16x16 metatile -> 4 tiles TL,TR,BL,BR
    return tiles4(g,16,16)
class Cv:
    def __init__(s,w,h):s.w=w;s.h=h;s.im=Image.new('L',(w,h),0);s.d=ImageDraw.Draw(s.im)
    def r(s,x0,y0,x1,y1,c):s.d.rectangle((min(x0,x1),min(y0,y1),max(x0,x1),max(y0,y1)),fill=c)
    def e(s,x0,y0,x1,y1,c):s.d.ellipse((min(x0,x1),min(y0,y1),max(x0,x1),max(y0,y1)),fill=c)
    def p(s,pts,c):s.d.polygon(pts,fill=c)
    def l(s,x0,y0,x1,y1,c,w=1):s.d.line((x0,y0,x1,y1),fill=c,width=w)
    def px(s,x,y,c):
        if 0<=x<s.w and 0<=y<s.h:s.im.putpixel((x,y),c)
    def get(s,x,y):return s.im.getpixel((x,y))
    def mirror(s):  # mirror left half onto right half (symmetric vehicles)
        for y in range(s.h):
            for x in range(s.w//2):s.im.putpixel((s.w-1-x,y),s.im.getpixel((x,y)))
    def grid(s,outline=0):
        g=[[s.im.getpixel((x,y)) for x in range(s.w)] for y in range(s.h)]
        if outline:
            def emp(x,y):return not(0<=x<s.w and 0<=y<s.h) or g[y][x]==0
            c2=[r[:] for r in g]
            for y in range(s.h):
                for x in range(s.w):
                    v=g[y][x]
                    if v==3:
                        if emp(x,y-1) or emp(x-1,y):c2[y][x]=4
                        elif emp(x,y+1) or emp(x+1,y) or emp(x+1,y+1):c2[y][x]=2
                    elif v==14 and (emp(x,y-1) or emp(x-1,y)):c2[y][x]=11
                    elif v==5 and (emp(x,y-1) or emp(x-1,y)) and s.w<=16:c2[y][x]=4
            g=c2
            o=[r[:] for r in g]
            for y in range(s.h):
                for x in range(s.w):
                    if g[y][x]==0 and any(0<=x+dx<s.w and 0<=y+dy<s.h and g[y+dy][x+dx] not in (0,outline) for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=outline
            g=o
        return g

# ------------------------------------------------------------------ font (hand-drawn 5x7)
FONT_ROWS="""
A .###. #...# #...# ##### #...# #...# #...#
B ####. #...# #...# ####. #...# #...# ####.
C .###. #...# #.... #.... #.... #...# .###.
D ####. #...# #...# #...# #...# #...# ####.
E ##### #.... #.... ####. #.... #.... #####
F ##### #.... #.... ####. #.... #.... #....
G .###. #...# #.... #.### #...# #...# .####
H #...# #...# #...# ##### #...# #...# #...#
I .###. ..#.. ..#.. ..#.. ..#.. ..#.. .###.
J ..### ...#. ...#. ...#. ...#. #..#. .##..
K #...# #..#. #.#.. ##... #.#.. #..#. #...#
L #.... #.... #.... #.... #.... #.... #####
M #...# ##.## #.#.# #.#.# #...# #...# #...#
N #...# #...# ##..# #.#.# #..## #...# #...#
O .###. #...# #...# #...# #...# #...# .###.
P ####. #...# #...# ####. #.... #.... #....
Q .###. #...# #...# #...# #.#.# #..#. .##.#
R ####. #...# #...# ####. #.#.. #..#. #...#
S .#### #.... #.... .###. ....# ....# ####.
T ##### ..#.. ..#.. ..#.. ..#.. ..#.. ..#..
U #...# #...# #...# #...# #...# #...# .###.
V #...# #...# #...# #...# #...# .#.#. ..#..
W #...# #...# #...# #.#.# #.#.# #.#.# .#.#.
X #...# #...# .#.#. ..#.. .#.#. #...# #...#
Y #...# #...# .#.#. ..#.. ..#.. ..#.. ..#..
Z ##### ....# ...#. ..#.. .#... #.... #####
0 .###. #...# #..## #.#.# ##..# #...# .###.
1 ..#.. .##.. ..#.. ..#.. ..#.. ..#.. .###.
2 .###. #...# ....# ...#. ..#.. .#... #####
3 ####. ....# ....# .###. ....# ....# ####.
4 ...#. ..##. .#.#. #..#. ##### ...#. ...#.
5 ##### #.... ####. ....# ....# #...# .###.
6 .###. #.... #.... ####. #...# #...# .###.
7 ##### ....# ...#. ..#.. .#... .#... .#...
8 .###. #...# #...# .###. #...# #...# .###.
9 .###. #...# #...# .#### ....# ....# .###.
. ..... ..... ..... ..... ..... .##.. .##..
, ..... ..... ..... ..... .##.. ..#.. .#...
: ..... .##.. .##.. ..... .##.. .##.. .....
! ..#.. ..#.. ..#.. ..#.. ..#.. ..... ..#..
? .###. #...# ....# ...#. ..#.. ..... ..#..
- ..... ..... ..... .###. ..... ..... .....
' ..#.. ..#.. .#... ..... ..... ..... .....
" .#.#. .#.#. ..... ..... ..... ..... .....
/ ....# ....# ...#. ..#.. .#... #.... #....
( ...#. ..#.. .#... .#... .#... ..#.. ...#.
) .#... ..#.. ...#. ...#. ...#. ..#.. .#...
+ ..... ..#.. ..#.. ##### ..#.. ..#.. .....
% ##..# ##..# ...#. ..#.. .#... #..## #..##
< ...#. ..#.. .#... #.... .#... ..#.. ...#.
> .#... ..#.. ...#. ....# ...#. ..#.. .#...
= ..... ..... ##### ..... ##### ..... .....
# .#.#. .#.#. ##### .#.#. ##### .#.#. .#.#.
* ..... #.#.# .###. ##### .###. #.#.# .....
_ ..... ..... ..... ..... ..... ..... #####
& .##.. #..#. #.#.. .#... #.#.# #..#. .##.#
; ..... .##.. .##.. ..... .##.. ..#.. .#...
"""
FONT={' ':'.'*35}
for line in FONT_ROWS.strip().split("\n"):
    p=line.split();FONT[p[0]]="".join(p[1:])
for k,v in FONT.items():assert len(v)==35,(k,len(v))
def glyph(ch):
    s=FONT.get(ch.upper(),FONT['?'] if ch!=' ' else FONT[' '])
    return [[1 if s[y*5+x]=='#' else 0 for x in range(5)] for y in range(7)]
# 1bpp font for mode-3 software text: 96 chars x 7 rows (5 bits each)
fb=[]
for c in range(32,128):
    g=glyph(chr(c))
    for y in range(7):fb.append(sum(g[y][x]<<x for x in range(5)))
emit("font_bits",fb,"unsigned char")
# HUD BG tiles (palette bank based): 0 transparent,1 text,2 shadow,3 panel,4 border,5 bar fill,6 bar empty
hud=[]
for bgc in (0,3):
    for c in range(32,128):
        g=glyph(chr(c));t=[[bgc]*8 for _ in range(8)]
        for y in range(7):
            for x in range(5):
                if g[y][x]:
                    if t[y+1][x+2] in (0,3):t[y+1][x+2]=2
        for y in range(7):
            for x in range(5):
                if g[y][x]:t[y][x+1]=1
        hud+=tiles4(t,8,8)
define("HT_PANEL_FONT",96)
# panel tiles 192..: fill, top, bottom, left, right, tl, tr, bl, br
def ptile(kind):
    t=[[3]*8 for _ in range(8)]
    for i in range(8):
        if 'T' in kind:t[0][i]=2;t[1][i]=4;t[2][i]=12
        if 'B' in kind:t[7][i]=2;t[6][i]=4;t[5][i]=12
        if 'L' in kind:t[i][0]=2;t[i][1]=4;t[i][2]=12
        if 'R' in kind:t[i][7]=2;t[i][6]=4;t[i][5]=12
    if len(kind)==2:  # rounded gold corner with a gem stud
        cy=0 if 'T' in kind else 7;cx=0 if 'L' in kind else 7
        t[cy][cx]=0;t[abs(cy-1)][cx]=2;t[cy][abs(cx-1)]=2
        gy=1 if 'T' in kind else 6;gx=1 if 'L' in kind else 6;t[gy][gx]=7
    return t
for k in ['', 'T','B','L','R','TL','TR','BL','BR']:hud+=tiles4(ptile(k),8,8)
define("HT_PANEL",192)
# bar tiles 201..209 fill 0..8
for n in range(9):
    t=[[0]*8 for _ in range(8)]
    for x in range(8):
        t[1][x]=2;t[6][x]=2
        for y in range(2,6):t[y][x]=5 if x<n else 6
    hud+=tiles4(t,8,8)
define("HT_BAR",201)
# icons 210: grenade, 211: missile, 212: skull/kill, 213: star, 214: heart
def icon(rows):
    mp={'.':0,'#':1,'s':2,'y':7,'r':8,'g':9}
    return tiles4([[mp[c] for c in r] for r in rows],8,8)
hud+=icon(["...ss...","..s##s..",".s####s.","s##yy##s","s##y###s","s######s",".s####s.","..ssss.."])
hud+=icon(["...#....","..###...","..###...","..###...","..###...",".#####..","#.#.#.#.","..y.y..."])
hud+=icon(["........",".rr..rr.","rrrrrrrr","rrrrrrrr",".rrrrrr.","..rrrr..","...rr...","........"])
hud+=icon(["...y....","...y....","yyyyyyy.",".yyyyy..","..yyy...",".yy.yy..","y.....y.","........"])
hud+=icon(["........","..####..",".#gggg#.",".#gggg#.",".#gggg#.","..####..","........","........"])
def heart(kind):
    rows=[".ss..ss.","s##ss##s","s#######s"[:8],"s######s",".s####s.","..s##s..","...ss...","........"]
    out=[]
    for y,r in enumerate(rows):
        o=[]
        for x,ch in enumerate(r):
            if ch=='s':o.append(2)
            elif ch=='#':
                full=kind==0 or (kind==1 and x<4)
                o.append((14 if (y==1 and x in (1,2)) else 13) if full else 3)
            else:o.append(0)
        out.append(o)
    return tiles4(out,8,8)
for k in range(3):hud+=heart(k)   # 215 full, 216 half, 217 empty
define("HT_ICON",210);define("HT_HEART",215)
emit("hud_tiles",hud,"unsigned int")
HUDP=[(0,0,0),(248,248,248),(28,20,36),(34,52,108),(236,196,92),(110,226,120),(60,40,40),(255,222,90),(240,64,64),(96,186,255),(255,150,40),(170,170,184),(150,100,40),(236,52,72),(255,176,190),(0,0,0)]
banks=[]
for col in [(248,248,248),(255,214,64),(255,90,80),(110,190,255),(110,230,120),(160,164,176)]:
    p=list(HUDP);p[1]=col;banks+=pal16(p)
emit("hud_pal",banks)  # banks 15,14,13,12,11,10 = white,yellow,red,blue,green,gray

# ------------------------------------------------------------------ trig
emit("sin_lut",[int(round(math.sin(i*2*math.pi/256)*256)) for i in range(256)],"short")
emit("atan_lut",[int(round(math.atan(i/64)/(2*math.pi)*256)) for i in range(65)],"unsigned char")

# ------------------------------------------------------------------ BG tilesets
def noise_tex(cv,seed,cols,dens=None,x0=0,y0=0,x1=15,y1=15):
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            h=H(seed,x,y);cv.px(x,y,cols[min(len(cols)-1,int(h*h*len(cols)*1.2))] if dens is None else (cols[1] if h<dens else cols[0]))
def ground(cv,seed,base=(1,2,3),spk=4,sp=0.06):
    for y in range(16):
        for x in range(16):
            h=H(seed,x,y);v=base[1]
            if h<0.25:v=base[0]
            elif h>0.82:v=base[2]
            if H(seed+7,x,y)<sp:v=spk
            cv.px(x,y,v)
def block_shade(cv,x0,y0,x1,y1,d,m,l,o=14):
    cv.r(x0,y0,x1,y1,m);cv.l(x0,y0,x1,y0,l);cv.l(x0,y0,x0,y1,l);cv.l(x0,y1,x1,y1,d);cv.l(x1,y0,x1,y1,d)
ARENA_PALS=[
 [(0,0,0),(56,128,64),(88,168,72),(128,200,88),(248,120,152),(184,136,80),(224,184,112),(88,88,104),(136,136,152),(192,192,200),(136,80,48),(200,144,80),(32,96,56),(64,152,72),(32,24,40),(255,248,200)],
 [(0,0,0),(168,184,216),(208,220,240),(240,248,255),(120,176,232),(152,136,120),(192,176,160),(80,88,112),(128,136,160),(184,192,212),(120,72,48),(176,120,72),(40,88,80),(72,128,104),(28,28,48),(255,255,255)],
 [(0,0,0),(192,144,88),(224,184,120),(248,216,152),(120,168,72),(176,120,72),(208,160,104),(104,88,80),(152,136,120),(200,184,160),(128,80,48),(192,136,80),(64,112,56),(104,152,72),(40,28,24),(255,248,224)],
 [(0,0,0),(64,72,120),(88,100,152),(120,136,184),(96,240,232),(112,96,160),(152,136,200),(56,56,88),(96,96,136),(152,152,192),(240,96,200),(255,200,96),(32,104,112),(64,168,152),(20,20,40),(220,255,255)],
 [(0,0,0),(112,112,128),(144,144,160),(176,176,192),(120,200,255),(128,120,136),(168,160,176),(72,76,96),(112,120,144),(168,176,200),(255,176,64),(120,200,255),(64,64,80),(96,96,112),(16,16,28),(240,244,255)],
]
ERA_NAMES=["WW2","COLD","MODERN","FUTURE","SPACE"]
def arena_set(era):  # v1.2: GBA-adventure style (chunky outlines, 3-tone cel shading, tufts/flowers)
    out=[]
    def grass(cv,seed,tufts=4,flowers=0,tall=0):
        for y in range(16):
            for x in range(16):cv.px(x,y,2 if H(seed,x//2,y//2)<.78 else (3 if H(seed+1,x,y)<.5 else 1))
        for i in range(tufts):
            x=1+int(H(seed,i,7)*12);y=2+int(H(seed,i,8)*12)
            cv.px(x,y,3);cv.px(x+2,y,3);cv.px(x+1,y-1,3);cv.px(x,y+1,1);cv.px(x+1,y+1,1);cv.px(x+2,y+1,1)
        for i in range(flowers):
            x=2+int(H(seed,i,3)*11);y=2+int(H(seed,i,4)*11)
            pc=4 if (i+era)%2==0 else (15 if era!=1 else 9)
            for dx,dy in((0,-1),(-1,0),(1,0),(0,1)):cv.px(x+dx,y+dy,pc)
            cv.px(x,y,6 if era!=1 else 4);cv.px(x+1,y+2,1)
        for i in range(tall):
            x=1+int(H(seed,i,5)*12);y=3+int(H(seed,i,6)*10)
            cv.p([(x,y+3),(x+1,y-1),(x+2,y+3)],13);cv.p([(x+2,y+3),(x+4,y),(x+4,y+3)],12);cv.px(x+1,y,3);cv.l(x,y+4,x+4,y+4,1)
    def outline_rect(cv,x0,y0,x1,y1,fill,hi,lo):
        cv.r(x0,y0,x1,y1,14);cv.r(x0+1,y0+1,x1-1,y1-1,fill);cv.l(x0+1,y0+1,x1-1,y0+1,hi);cv.l(x0+1,y0+1,x0+1,y1-1,hi);cv.l(x0+1,y1-1,x1-1,y1-1,lo);cv.l(x1-1,y0+1,x1-1,y1-1,lo)
    # 0..2 ground
    cv=Cv(16,16);grass(cv,era*100,4);out.append(cv.grid())
    cv=Cv(16,16);grass(cv,era*100+1,2,3 if era<3 else 1);out.append(cv.grid())
    cv=Cv(16,16);grass(cv,era*100+2,1,0,3);out.append(cv.grid())
    # 3 path (packed dirt with pebbles)
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,5 if H(era,x//3,y//2,9)<.7 else 6)
    for i in range(5):
        x=int(H(era,i,11)*14);y=int(H(era,i,12)*14);cv.px(x,y,9);cv.px(x+1,y,8);cv.px(x,y+1,7)
    out.append(cv.grid())
    # 4 wall: stone wall in 3/4 view (light top face, brick front, dark base)
    cv=Cv(16,16);cv.r(0,0,15,15,14);cv.r(0,0,15,5,9);cv.l(0,0,15,0,15)
    for x in (3,8,13):cv.l(x,1,x,5,8)
    cv.r(0,6,15,14,8)
    for row,y in enumerate((6,9,12)):
        cv.l(0,y,15,y,7);off=0 if row%2==0 else 4
        for x in range(off,16,8):cv.l(x,y,x,y+2,7)
        cv.l(1,y+1,15,y+1,9) if row==0 else None
    cv.l(0,15,15,15,14)
    if era==1:cv.r(0,0,15,2,15);cv.px(4,3,15);cv.px(11,3,15)
    if era==3:cv.l(0,10,15,10,4)
    out.append(cv.grid())
    # 5 crate on grass
    cv=Cv(16,16);grass(cv,era*100+3,1)
    if era<=2:
        outline_rect(cv,1,1,14,14,11,15 if era!=2 else 6,10);cv.l(2,2,13,13,10);cv.l(13,2,2,13,10);cv.r(2,2,13,3,15 if era==1 else 6)
    else:
        outline_rect(cv,1,1,14,14,8,9,7);cv.r(5,5,10,10,4 if era==3 else 10);cv.px(6,6,15)
    out.append(cv.grid())
    # 6 sandbags
    cv=Cv(16,16);grass(cv,era*100+4,1)
    for row,y in enumerate((3,8,13)):
        off=3 if row%2 else 0
        for x in range(-3,16,6):cv.e(x+off,y-3,x+off+6,y+2,14);cv.e(x+off+1,y-2,x+off+5,y+1,6);cv.l(x+off+2,y-2,x+off+4,y-2,15);cv.l(x+off+2,y+1,x+off+4,y+1,5)
    out.append(cv.grid())
    # 7 tree (round chunky canopy) / snowy pine / palm / neon tree / boulder
    cv=Cv(16,16);grass(cv,era*100+5,1)
    if era in (0,):
        cv.e(0,0,15,15,14);cv.e(1,1,14,14,12);cv.e(1,1,12,12,13);cv.e(3,2,8,6,3);cv.px(4,3,15);cv.e(9,9,12,12,12)
    elif era==1:
        cv.p([(8,0),(15,15),(0,15)],14);cv.p([(8,1),(14,14),(1,14)],12);cv.p([(8,2),(11,8),(5,8)],13);cv.p([(8,1),(10,5),(6,5)],15);cv.l(3,13,13,13,15);cv.l(5,9,11,9,15)
    elif era==2:
        for a in range(6):
            ang=a*2*math.pi/6+0.4;cv.l(8,8,8+int(7.5*math.cos(ang)),8+int(7.5*math.sin(ang)),14,3)
        for a in range(6):
            ang=a*2*math.pi/6+0.4;cv.l(8,8,8+int(6.5*math.cos(ang)),8+int(6.5*math.sin(ang)),13,1)
        cv.e(6,6,10,10,10);cv.px(7,7,11)
    elif era==3:
        cv.e(1,1,14,14,14);cv.e(2,2,13,13,12);cv.e(3,3,11,11,13);cv.e(5,4,8,7,4);cv.px(6,5,15)
    else:
        cv.p([(3,2),(12,1),(15,8),(12,15),(2,13),(0,6)],14);cv.p([(4,3),(11,2),(14,8),(11,14),(3,12),(1,6)],8);cv.p([(4,3),(10,3),(12,7),(5,8),(2,6)],9);cv.px(5,4,15);cv.e(8,9,12,12,7)
    out.append(cv.grid())
    # 8 crater
    cv=Cv(16,16);grass(cv,era*100+6,1);cv.e(1,2,14,14,14);cv.e(2,3,13,13,6);cv.e(3,5,12,12,5);cv.e(5,7,10,11,7);cv.l(4,4,11,4,15)
    out.append(cv.grid())
    # 9 border: cliff with grassy ledge
    cv=Cv(16,16);cv.r(0,0,15,4,2);cv.l(0,1,15,1,3);[cv.px(x,2,3) for x in range(1,16,4)];cv.l(0,5,15,5,14)
    cv.r(0,6,15,15,8)
    for x in (2,7,12):cv.l(x,6,x+1,15,7)
    for x in (4,10):cv.l(x,7,x,12,9)
    cv.l(0,15,15,15,14)
    if era==1:cv.r(0,0,15,3,15)
    out.append(cv.grid())
    # 10 spawn pad
    cv=Cv(16,16);grass(cv,era*100+7,1);cv.e(1,1,14,14,14);cv.e(2,2,13,13,9);cv.e(4,4,11,11,8);cv.p([(8,4),(11,8),(9,8),(9,11),(7,11),(7,8),(5,8)],15)
    out.append(cv.grid())
    # 11 water (bank 1: 1-5 water, 6-9 wood, 10 foam) - shimmer is palette-cycled at runtime
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,2 if H(era*31+11,x//3,y//2)<.7 else 1)
    for (x,y) in [(1,3),(8,6),(3,11),(10,13)]:cv.l(x,y,x+4,y,3);cv.l(x+1,y-1,x+3,y-1,4);cv.px(x+2,y-1,5)
    out.append(cv.grid())
    # 12 bridge
    cv=Cv(16,16);cv.r(0,0,15,15,8)
    for y in range(0,16,4):cv.r(1,y,14,y+2,7);cv.l(1,y,14,y,9);cv.l(1,y+3,14,y+3,8)
    cv.l(0,0,0,15,8);cv.l(15,0,15,15,8);cv.l(1,0,1,15,6);cv.l(14,0,14,15,6)
    out.append(cv.grid())
    tl=[]
    for g in out:tl+=meta_tiles(g)
    return tl
WATER_PALS=[
 [(0,0,0),(40,96,184),(64,136,216),(96,176,232),(160,216,248),(232,248,255),(112,72,40),(168,112,64),(72,48,32),(208,160,96),(255,255,255)],
 [(0,0,0),(80,120,176),(112,152,208),(152,192,232),(200,228,248),(248,252,255),(104,72,48),(160,116,76),(64,44,32),(200,160,112),(255,255,255)],
 [(0,0,0),(32,128,152),(48,160,176),(80,192,200),(152,224,224),(232,252,248),(120,88,56),(176,128,80),(80,56,36),(216,176,120),(255,255,255)],
 [(0,0,0),(24,48,112),(32,72,152),(48,112,200),(80,220,240),(200,255,255),(64,64,96),(96,96,136),(40,40,64),(255,96,208),(255,255,255)],
 [(0,0,0),(16,24,48),(24,40,72),(40,60,104),(80,110,160),(160,190,230),(88,96,112),(128,136,156),(56,60,76),(176,186,206),(255,255,255)],
]
for e in range(5):
    emit(f"arena_tiles_{e}",arena_set(e),"unsigned int");emit(f"arena_pal_{e}",pal16(ARENA_PALS[e]));emit(f"water_pal_{e}",pal16(WATER_PALS[e]))
Hh.append("enum{AM_G0,AM_G1,AM_G2,AM_PATH,AM_WALL,AM_CRATE,AM_BAGS,AM_TREE,AM_CRATER,AM_BORDER,AM_SPAWN,AM_WATER,AM_BRIDGE,AM_N};")

# dogfight terrain (seen from altitude) + clouds
SKY_PALS=[
 [(40,104,200),(52,124,214),(70,148,226),(220,240,252),(240,216,150),(96,184,84),(126,204,100),(48,128,60),(68,150,70),(200,128,96),(220,200,160),(250,250,255),(200,214,236),(150,168,200),(24,48,104),(255,255,255)],
 [(56,96,152),(68,112,168),(86,132,184),(220,232,248),(236,244,252),(250,252,255),(176,192,210),(56,112,96),(80,136,112),(140,140,150),(200,80,72),(250,250,255),(200,214,236),(150,168,200),(28,40,72),(255,255,255)],
 [(220,180,120),(208,168,108),(236,200,140),(176,140,96),(144,116,80),(112,112,120),(156,152,148),(196,192,180),(240,232,210),(96,136,72),(56,120,168),(250,250,255),(200,214,236),(150,168,200),(72,56,40),(255,255,255)],
 [(24,24,56),(32,32,72),(44,44,96),(60,60,120),(80,88,150),(110,124,180),(60,250,240),(255,90,210),(255,210,80),(40,150,140),(80,80,120),(220,230,255),(150,160,200),(100,110,160),(8,8,20),(255,255,255)],
 [(6,6,20),(12,12,32),(44,20,64),(80,32,108),(130,60,150),(104,104,120),(150,148,160),(196,194,206),(236,236,246),(255,190,60),(100,220,255),(240,244,255),(176,186,206),(130,140,170),(0,0,0),(255,255,255)],
]
def sky_set(era):
    out=[]
    def base(seed,cols):
        cv=Cv(16,16)
        for y in range(16):
            for x in range(16):
                h=H(seed,x//2,y//2);cv.px(x,y,cols[0] if h<.6 else cols[1])
        return cv
    if era in (0,1):
        # 0 sea,1 sea waves,2 shore(ice/sand),3 land,4 land2,5 forest,6 town/feature,7 detail
        cv=base(era*50,(0,1));out.append(cv.grid())
        cv=base(era*50+1,(1,2));cv.l(3,5,7,5,3);cv.l(10,11,13,11,3);out.append(cv.grid())
        cv=base(era*50+2,(4,3) if era==0 else (4,5));out.append(cv.grid())
        cv=base(era*50+3,(5,6) if era==0 else (5,4));out.append(cv.grid())
        cv=base(era*50+4,(6,5) if era==0 else (6,5));out.append(cv.grid())
        cv=base(era*50+5,(7,8));[cv.e(int(H(era,i)*12),int(H(era,i,1)*12),int(H(era,i)*12)+4,int(H(era,i,1)*12)+4,8) for i in range(5)];out.append(cv.grid())
        cv=base(era*50+6,(5,6) if era==0 else (4,5))
        for i in range(4):x=int(H(era,i,3)*12);y=int(H(era,i,4)*12);cv.r(x,y,x+3,y+2,9);cv.l(x,y,x+3,y,10 if era==1 else 4)
        out.append(cv.grid())
        cv=base(era*50+7,(0,1));cv.r(4,7,11,8,9);cv.r(10,6,11,9,10);out.append(cv.grid())  # boat / ship
    elif era==2:
        cv=base(100,(0,1));out.append(cv.grid())
        cv=base(101,(0,2));[cv.l(x,y,x+5,y-1,3) for x,y in ((1,4),(8,12))];out.append(cv.grid())
        cv=base(102,(3,0));out.append(cv.grid())
        cv=base(103,(1,2));out.append(cv.grid())
        cv=base(104,(2,0));cv.r(0,7,15,8,5);cv.l(0,7,15,7,6);out.append(cv.grid())  # road
        cv=Cv(16,16);cv.r(0,0,15,15,6);[ (cv.r(x,y,x+5,y+5,7),cv.l(x,y+5,x+5,y+5,14),cv.l(x+5,y,x+5,y+5,14)) for x in (1,9) for y in (1,9)];out.append(cv.grid())
        cv=base(106,(0,1));[cv.e(x,y,x+3,y+3,9) for x,y in ((2,3),(9,9),(10,2))];out.append(cv.grid())
        cv=base(107,(1,0));cv.e(3,3,12,12,4);cv.e(5,5,10,10,3);out.append(cv.grid())
    elif era==3:
        cv=Cv(16,16);cv.r(0,0,15,15,1);cv.l(0,0,15,0,2);cv.l(0,0,0,15,2);out.append(cv.grid())
        cv=Cv(16,16);cv.r(0,0,15,15,1);cv.l(0,8,15,8,6);out.append(cv.grid())
        cv=Cv(16,16);cv.r(0,0,15,15,1);cv.l(8,0,8,15,7);out.append(cv.grid())
        for k in range(3):
            cv=Cv(16,16);cv.r(0,0,15,15,0);cv.r(1,1,14,14,3+k);cv.r(2,2,13,4,4+k)
            for i in range(6):cv.px(3+int(H(k,i)*10),6+int(H(k,i,2)*7),[6,7,8][(i+k)%3])
            out.append(cv.grid())
        cv=Cv(16,16);cv.r(0,0,15,15,9);cv.e(3,3,12,12,9);[cv.px(int(H(7,i)*16),int(H(8,i)*16),6) for i in range(5)];out.append(cv.grid())
        cv=Cv(16,16);cv.r(0,0,15,15,1);cv.r(0,7,15,8,10);cv.r(5,7,6,8,8);out.append(cv.grid())
    else:
        for k in range(8):
            cv=Cv(16,16)
            for y in range(16):
                for x in range(16):
                    v=0
                    if k in (2,3):v=[1,2][int(H(k,x//3,y//3)*1.6)]
                    if k==4:v=[2,3][int(H(k,x//4,y//4)*1.8)]
                    cv.px(x,y,v)
            for i in range(3 if k<2 else 1):
                x=int(H(k,i,5)*16);y=int(H(k,i,6)*16);cv.px(x,y,[11,15,12][i%3])
            if k==5:cv.e(1,2,14,13,5);cv.e(3,3,12,10,6);cv.e(4,4,7,7,5);cv.px(9,8,7)
            if k==6:cv.e(4,4,10,11,6);cv.e(5,5,8,8,7)
            if k==7:cv.px(7,7,15);cv.px(6,7,11);cv.px(8,7,11);cv.px(7,6,11);cv.px(7,8,11)
            out.append(cv.grid())
    while len(out)<24:out.append([[0]*16 for _ in range(16)])
    # clouds 24..30, 31 empty (transparent: index 0)
    for k in range(7):
        cv=Cv(16,16)
        if era!=4:
            for i in range(5):
                cx=int(H(k,i,1)*12)+2;cy=int(H(k,i,2)*12)+2;r=3+int(H(k,i,3)*4)
                cv.e(cx-r,cy-r,cx+r,cy+r,12)
            for i in range(5):
                cx=int(H(k,i,1)*12)+2;cy=int(H(k,i,2)*12)+1;r=2+int(H(k,i,3)*3)
                cv.e(cx-r,cy-r,cx+r,cy+r-1,11)
        out.append(cv.grid())
    out.append([[0]*16 for _ in range(16)])
    tl=[]
    for g in out:tl+=meta_tiles(g)
    return tl
for e in range(5):
    emit(f"sky_tiles_{e}",sky_set(e),"unsigned int");emit(f"sky_pal_{e}",pal16(SKY_PALS[e]))

# campaign (Omaha 1944) tileset
CPAL=[(40,96,184),(64,136,216),(200,232,248),(184,160,112),(224,200,144),(248,228,180),(64,64,80),(112,116,132),(170,174,188),(140,132,120),(196,188,170),(72,148,64),(112,184,80),(140,100,60),(30,22,38),(240,232,200)]
def camp_set():
    T={}
    def sand(seed,ripple=False):
        cv=Cv(16,16);ground(cv,seed,(3,4,5),3,0.05) if False else None
        for y in range(16):
            for x in range(16):
                h=H(seed,x,y);cv.px(x,y,4 if h<.7 else (5 if h<.92 else 3))
        if ripple:
            for y in (3,9,14):
                for x in range(16):
                    if H(seed,x,99)<.7:cv.px(x,y+int(math.sin(x*.8)*1),3)
        return cv
    def water(seed,deep):
        cv=Cv(16,16)
        for y in range(16):
            for x in range(16):
                h=H(seed,x//2,y);cv.px(x,y,0 if deep else 1)
                if h>.9:cv.px(x,y,1 if deep else 2)
        return cv
    T['DEEP']=water(1,1);T['WATER']=water(2,0)
    cv=water(3,0)
    for x in range(16):
        y=6+int(math.sin(x*.7)*2);cv.l(x,y,x,y+2,2)
        for yy in range(y+3,16):cv.px(x,yy,3 if H(x,yy)<.8 else 1)
    T['SURF']=cv
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,3 if H(4,x,y)<.75 else (4 if H(5,x,y)<.7 else 1))
    T['WET']=cv
    T['SAND']=sand(6);T['SAND2']=sand(7,True)
    cv=sand(8)  # Czech hedgehog: three crossed steel beams
    cv.l(2,3,13,12,14,2);cv.l(13,3,2,12,14,2);cv.l(7,1,8,14,6,2)
    cv.l(3,3,12,12,7);cv.l(12,3,3,12,7);cv.l(7,2,7,13,8)
    cv.e(5,13,11,15,3)
    T['HEDGEHOG']=cv
    cv=sand(9);cv.e(1,2,14,14,3);cv.e(3,4,12,12,13);cv.e(4,5,11,11,14);cv.l(3,3,12,3,5);T['CRATER']=cv
    cv=Cv(16,16)
    for i in range(40):
        x=int(H(10,i)*15);y=int(H(11,i)*15);r=1+int(H(12,i)*3)
        cv.e(x-r,y-r,x+r,y+r,[7,8,9,10][i%4]);cv.px(x-1,y-1,15)
    for y in range(16):
        for x in range(16):
            if cv.get(x,y)==0:cv.px(x,y,6)
    T['SHINGLE']=cv
    def wire(broken):
        cv=sand(13)
        if not broken:
            for x0 in (0,8):cv.e(x0,2,x0+8,13,0);cv.e(x0+1,3,x0+7,12,4) if False else None
            for k in range(3):
                for x in range(16):
                    y=8+int(math.sin((x+k*5)*.9)*5)
                    cv.px(x,y,14);
                    if x%3==k:cv.px(x,y-1,15)
            cv.l(2,1,2,14,13,2);cv.l(13,1,13,14,13,2)
        else:
            for i in range(6):x=int(H(14,i)*14);y=int(H(15,i)*14);cv.l(x,y,x+3,y+1,14)
            cv.e(3,4,12,12,3);cv.e(5,6,10,10,14)
        return cv
    T['WIRE']=wire(0);T['WIREB']=wire(1)
    def grass(seed,dirt=0):
        cv=Cv(16,16)
        for y in range(16):
            for x in range(16):
                h=H(seed,x,y);cv.px(x,y,12 if h<.6 else (11 if h<.9 else 13))
        if dirt:
            for x in range(16):cv.px(x,4+int(math.sin(x*.5)*1.5),13);cv.px(x,11+int(math.cos(x*.4)*1.5),13)
        return cv
    T['GRASS']=grass(16);T['GRASS2']=grass(17,1)
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,13 if H(18,x,y)<.7 else 14)
    cv.l(0,0,15,0,11);cv.l(0,15,15,15,11);T['TRENCH']=cv
    cv=grass(19)
    for row,y in enumerate((3,8,13)):
        off=3 if row%2 else 0
        for x in range(-3,16,6):cv.e(x+off,y-2,x+off+6,y+2,4);cv.l(x+off+1,y-2,x+off+5,y-2,5);cv.l(x+off+1,y+2,x+off+5,y+2,3)
    T['BAGS']=cv
    # bunker 3x2
    bk=Cv(48,32);bk.r(0,0,47,31,9);bk.r(0,0,47,2,10);bk.r(0,29,47,31,6);bk.r(0,0,2,31,10);bk.r(45,0,47,31,6)
    for i in range(30):x=int(H(20,i)*44)+2;y=int(H(21,i)*26)+3;bk.px(x,y,[6,10,7][i%3])
    bk.r(6,4,41,14,10);bk.r(8,6,39,12,9);bk.l(8,6,39,6,7)
    bk.r(16,20,31,27,14);bk.r(16,20,31,21,6);bk.l(24,22,24,26,7)   # embrasure slit + MG barrel
    bk.l(24,26,24,31,6,1)
    for i,n in enumerate(['BK_TL','BK_T','BK_TR','BK_BL','BK_EMB','BK_BR']):
        sub=Cv(16,16);x0=(i%3)*16;y0=(i//3)*16
        for y in range(16):
            for x in range(16):sub.px(x,y,bk.get(x0+x,y0+y))
        T[n]=sub
    cv=grass(22)
    for i in range(12):
        x=int(H(23,i)*14);y=int(H(24,i)*14);r=1+int(H(25,i)*3);cv.e(x-r,y-r,x+r,y+r,[9,10,6,14][i%4])
    T['RUBBLE']=cv
    # landing craft (3 wide): sides, floor, ramp, stern
    def boat(kind):
        cv=Cv(16,16);cv.r(0,0,15,15,0)
        if kind=='L':cv.r(0,0,15,15,7);cv.r(10,0,15,15,8);cv.r(0,0,3,15,6);cv.l(4,0,4,15,8)
        if kind=='R':cv.r(0,0,15,15,7);cv.r(0,0,5,15,8);cv.r(12,0,15,15,6);cv.l(11,0,11,15,8)
        if kind=='F':cv.r(0,0,15,15,8);[cv.l(0,y,15,y,7) for y in (3,7,11,15)]
        if kind=='RAMP':
            cv.r(0,0,15,15,1);cv.r(1,0,14,15,7);[cv.l(1,y,14,y,8) for y in range(1,16,3)]
        if kind=='ST':cv.r(0,0,15,15,6);cv.r(0,0,15,5,7);cv.r(4,6,11,12,14);cv.l(0,0,15,0,8)
        return cv
    T['BOAT_L']=boat('L');T['BOAT_R']=boat('R');T['BOAT_F']=boat('F');T['BOAT_RAMP']=boat('RAMP');T['BOAT_ST']=boat('ST')
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,13 if H(26,x,y)<.75 else 4)
    T['DIRT']=cv
    cv=Cv(16,16)
    for y in range(16):
        for x in range(16):cv.px(x,y,9 if H(27,x,y)<.8 else 10)
    cv.l(0,7,15,7,6);T['CONC']=cv
    # dark cover of bluff edge (grass w/ shadow)
    cv=grass(28);cv.r(0,12,15,15,11);[cv.px(x,12+int(H(29,x)*3),13) for x in range(16)];T['BLUFF']=cv
    names=['DEEP','WATER','SURF','WET','SAND','SAND2','HEDGEHOG','CRATER','SHINGLE','WIRE','WIREB','GRASS','GRASS2','TRENCH','BAGS',
           'BK_TL','BK_T','BK_TR','BK_BL','BK_EMB','BK_BR','RUBBLE','BOAT_L','BOAT_R','BOAT_F','BOAT_RAMP','BOAT_ST','DIRT','CONC','BLUFF']
    tl=[]
    for n in names:tl+=meta_tiles(T[n].grid())
    e="enum{"+",".join("CM_"+n for n in names)+",CM_N};"
    if e not in Hh:Hh.append(e)
    return tl
emit("camp_tiles",camp_set(),"unsigned int");emit("camp_pal",pal16(CPAL))

# ------------------------------------------------------------------ sprites
# OBJ palette semantic indices:
# 1 outline 2 uni dark 3 uni mid 4 uni light 5 helmet/armor 6 skin 7 black 8 metal/wood 9 TEAM marker
# 10 glow 11 white 12 track dark 13 track light 14 gray 15 hot
UNI={  # (dark, mid, light, helmet) per era per side
 0:{'A':[(64,92,40),(100,136,56),(150,184,88),(92,124,56)],'E':[(72,84,96),(112,128,140),(164,180,190),(90,104,116)]},
 1:{'A':[(52,92,56),(84,136,84),(128,184,120),(72,120,72)],'E':[(104,88,56),(152,128,84),(204,180,124),(124,108,72)]},
 2:{'A':[(140,112,68),(196,160,104),(236,212,152),(160,132,88)],'E':[(56,76,72),(88,116,108),(136,168,152),(70,92,88)]},
 3:{'A':[(50,62,90),(76,94,132),(110,134,176),(140,160,200)],'E':[(70,40,52),(110,60,80),(150,90,110),(160,110,130)]},
 4:{'A':[(150,156,172),(196,202,214),(236,240,248),(220,226,238)],'E':[(40,40,48),(66,66,78),(96,96,110),(80,80,96)]},
}
GLOW=[(240,220,140),(200,220,160),(120,255,140),(60,240,255),(80,200,255)]
def obj_pal(era,side,player=False):
    d,m,l,hlm=UNI[era]['A' if side=='A' else 'E']
    team=(70,150,255) if side=='A' else (230,50,40)
    if player:team=(255,220,40)
    g=GLOW[era] if side=='A' else ((255,80,80) if era>=3 else GLOW[era])
    return pal16([(0,0,0),(30,20,40),d,m,l,hlm,(248,196,150),(40,32,48),(170,120,72) if era<2 else (120,126,140),team,g,(248,248,240),(56,52,60),(104,100,96),(168,172,184),(255,208,72)])
pals=[]
for e in range(5):
    for side,pl in (('A',0),('E',0),('A',1)):pals+=pal16_list if False else obj_pal(e,side,pl)
emit("obj_team_pal",pals)   # [era][3 banks]
FXP=[(0,0,0),(16,16,18),(60,60,64),(110,110,116),(170,170,176),(250,250,250),(255,236,90),(255,150,40),(230,60,30),(130,30,20),(80,240,255),(60,120,255),(255,80,220),(90,230,110),(80,160,255),(255,70,60)]
emit("fx_pal",pal16(FXP))
def soldier(era,frame):  # v1.2 chunky big-head style (top-down)
    c=Cv(16,16)
    st=2 if frame else -2
    c.e(4,9+st//2,7,13+st//2,7);c.e(9,9-st//2,12,13-st//2,7)            # boots
    c.e(2,7,13,14,3)                                                      # shoulders / body
    if era==4:c.r(5,11,10,14,14)                                         # life support
    elif era>=2:c.r(4,11,11,13,2)
    else:c.r(4,11,11,13,8 if era==0 else 2)                              # pack / bedroll
    c.px(2,10,9);c.px(13,10,9)                                            # team patches
    if era<=1:c.l(11,0,11,7,8);c.px(11,2,7)
    elif era==2:c.l(11,0,11,7,7);c.px(11,3,14)
    elif era==3:c.r(11,0,12,7,7);c.px(11,0,10)
    else:c.r(11,0,12,7,14);c.px(11,0,10);c.px(12,0,10)
    c.px(11,7,6);c.px(4,8,6)                                              # hands
    # big head / helmet (about half the sprite)
    if era==0:c.e(3,1,12,11,5);c.e(4,2,8,5,4);c.px(8,1,9)
    elif era==1:c.e(4,1,11,11,5);c.e(5,2,8,5,4)
    elif era==2:c.e(4,1,11,11,5);c.r(6,0,9,2,7);c.e(5,3,7,5,4)
    elif era==3:c.e(4,1,11,11,5);c.l(5,3,10,3,10);c.l(5,4,10,4,10)
    else:c.e(3,1,12,11,11);c.e(5,2,10,6,10);c.px(5,3,15)
    return c.grid(1)
def tank(era,part):
    c=Cv(32,32)
    if part=='hull':
        if era<=2:
            c.r(5,4,10,27,12);c.r(21,4,26,27,12)
            for y in range(5,27,3):c.l(5,y,10,y,13);c.l(21,y,26,y,13)
            w=(10,22)
            if era==0:c.r(10,6,21,26,3);c.r(11,6,20,9,4);c.p([(10,6),(21,6),(19,3),(12,3)],4);c.r(11,22,20,25,2)
            elif era==1:c.r(10,5,21,26,3);c.r(11,5,20,7,4);c.r(11,21,20,25,2);c.l(12,23,19,23,7)
            else:c.r(10,4,21,27,3);c.p([(10,4),(21,4),(20,2),(11,2)],4);c.r(11,21,20,26,2);[c.l(12,y,19,y,7) for y in (22,24)]
            c.l(10,15,21,15,2)
        else:
            c.e(6,3,25,28,2);c.e(8,4,23,27,3);c.r(9,20,22,25,2)
            c.l(6,8,6,24,10);c.l(25,8,25,24,10);c.px(11,27,10);c.px(20,27,10);c.px(16,28,10)
            if era==4:c.r(6,14,8,18,14);c.r(23,14,25,18,14)
    else:
        if era==0:c.e(10,10,21,22,3);c.e(11,11,19,18,4);c.r(15,1,16,10,14);c.px(15,1,7);c.px(16,1,7);c.e(13,17,16,20,9)
        elif era==1:c.e(9,10,22,23,3);c.e(10,11,20,18,4);c.r(15,0,16,10,14);c.r(14,3,17,4,14);c.e(17,16,20,19,9)
        elif era==2:c.p([(10,11),(21,11),(23,16),(22,24),(9,24),(8,16)],3);c.r(11,12,20,15,4);c.r(15,0,16,11,14);c.r(14,0,17,1,7);c.r(18,18,20,20,9);c.r(11,19,14,22,2)
        elif era==3:c.e(10,10,21,21,5);c.e(12,11,19,17,4);c.r(13,1,14,10,14);c.r(17,1,18,10,14);c.px(13,1,10);c.px(18,1,10);c.e(14,15,17,18,9)
        else:c.e(9,9,22,22,5);c.e(11,10,20,17,11);c.r(15,0,16,9,14);c.r(14,0,17,0,10);c.e(14,13,17,16,10);c.px(11,19,9);c.px(20,19,9)
    return c.grid(1)
def plane(era):
    c=Cv(32,32)
    if era==0:   # prop fighter
        c.r(15,4,16,27,3);c.e(14,3,17,9,3)
        c.e(3,11,28,17,3);c.r(4,13,27,15,3);c.l(4,13,27,13,4)
        c.e(10,24,21,28,3);c.l(15,23,16,23,2)
        c.e(14,8,17,12,10);c.e(6,13,8,15,9);c.e(23,13,25,15,9)
        c.l(11,2,20,2,14);c.px(15,3,7);c.px(16,3,7)
    elif era==1: # swept-wing jet
        c.r(14,3,17,27,3);c.p([(15,1),(16,1),(17,4),(14,4)],4)
        c.p([(14,11),(3,22),(3,24),(14,19)],3);c.p([(17,11),(28,22),(28,24),(17,19)],3)
        c.p([(14,23),(9,28),(14,27)],3);c.p([(17,23),(22,28),(17,27)],3)
        c.e(14,6,17,11,10);c.px(6,22,9);c.px(25,22,9);c.r(15,27,16,28,15)
    elif era==2: # stealth-ish fighter
        c.p([(16,1),(19,8),(29,19),(28,22),(20,21),(22,28),(16,26),(10,28),(12,21),(3,22),(2,19),(12,8),(15,1)],3)
        c.p([(16,3),(18,9),(16,24),(14,9)],4);c.e(14,6,17,11,10);c.r(13,26,14,27,15);c.r(17,26,18,27,15);c.px(7,20,9);c.px(24,20,9)
    elif era==3: # forward-swept future jet
        c.r(14,2,17,27,3);c.p([(14,17),(4,10),(3,12),(14,22)],3);c.p([(17,17),(27,10),(28,12),(17,22)],3)
        c.p([(14,24),(8,28),(14,28)],4);c.p([(17,24),(23,28),(17,28)],4);c.e(14,5,17,10,10)
        c.l(4,11,13,18,10);c.l(27,11,18,18,10);c.r(14,28,17,29,10);c.px(8,13,9);c.px(23,13,9)
    else:        # space starfighter
        c.e(12,4,19,24,3);c.e(13,5,18,12,10)
        c.r(4,13,11,15,5);c.r(20,13,27,15,5);c.r(3,8,5,22,14);c.r(26,8,28,22,14)
        c.px(4,8,10);c.px(27,8,10);c.r(14,24,17,27,10);c.r(3,22,5,23,10);c.r(26,22,28,23,10);c.e(14,15,17,18,9)
    return c.grid(1)
def hellcat():  # Grumman F6F: stubby barrel fuselage, big radial cowl, squared low wings
    c=Cv(32,32)
    c.e(12,2,19,10,3);c.e(13,3,18,8,7);c.e(14,4,17,7,14)      # radial cowl + engine face
    c.r(13,8,18,24,3);c.r(14,9,17,23,4);c.p([(13,24),(18,24),(17,28),(14,28)],3)
    c.r(2,12,29,16,3);c.r(2,12,29,12,4);c.r(2,16,29,16,2);c.r(2,12,3,16,2);c.r(28,12,29,16,2)  # square-tip wings
    c.r(9,25,22,27,3);c.r(9,25,22,25,4)                        # tailplane
    c.e(14,10,17,14,10)                                        # canopy
    c.e(5,13,8,15,9);c.e(23,13,26,15,9)                        # team roundels
    c.l(10,1,21,1,14);c.px(15,1,7);c.px(16,1,7)                # prop arc
    c.px(9,13,7);c.px(10,13,7);c.px(21,13,7);c.px(22,13,7)     # .50 cal ports
    return c.grid(1)
WW2_PLANES=['P51','SPIT','F6F','P47','F4U','YAK3','HURRI','P38','BF109','FW190','ME262','ZERO','STUKA','SBD']
def ww2plane(k):
    if k=='F6F':return hellcat()
    c=Cv(32,32)
    def sym(pts,col):c.p(pts,col);c.p([(31-x,y) for x,y in pts],col)
    def prop(y,x0=10,x1=21):c.l(x0,y,x1,y,14);c.px((x0+x1)//2,y,7);c.px((x0+x1)//2+1,y,7)
    def roundel(x,y,r=1):c.e(x-r,y-r,x+r,y+r,9);c.e(31-x-r,y-r,31-x+r,y+r,9)
    if k=='P47':
        c.e(1,12,30,18,3);c.l(2,13,29,13,4);c.r(12,8,19,25,3);c.l(15,8,15,24,4);c.e(11,2,20,10,3);c.e(12,3,19,8,7);c.e(13,4,18,7,2)
        c.e(9,24,22,28,3);c.r(15,25,16,30,3);c.e(14,11,17,15,10);roundel(5,15);prop(1)
    elif k=='ME262':
        c.p([(16,1),(18,6),(18,26),(13,26),(13,6),(15,1)],3);sym([(13,12),(2,19),(2,21),(13,18)],3);sym([(13,13),(3,19)],4)
        c.e(6,10,9,21,2);c.e(22,10,25,21,2);c.r(7,10,8,11,7);c.r(23,10,24,11,7);c.px(7,21,15);c.px(24,21,15)
        sym([(13,23),(9,27),(13,27)],3);c.e(14,9,17,13,10);c.px(5,20,9);c.px(26,20,9)
    elif k=='BF109':
        c.r(14,3,17,27,3);c.p([(14,3),(17,3),(16,1),(15,1)],3);sym([(14,12),(3,14),(3,17),(14,18)],3);c.l(4,14,13,13,4);c.l(18,13,27,14,4)
        sym([(14,25),(10,26),(10,28),(14,28)],3);c.r(14,9,17,12,10);c.r(14,3,17,4,5);c.r(6,15,7,16,9);c.r(24,15,25,16,9);prop(1)
    elif k=='P51':
        c.r(14,3,17,27,3);c.e(14,1,17,6,3);sym([(14,11),(2,15),(2,17),(14,19)],3);c.l(3,15,13,12,4);c.l(18,12,28,15,4)
        sym([(14,24),(9,25),(9,27),(14,27)],3);c.e(14,9,17,13,10);roundel(6,16);prop(1)
    elif k=='SPIT':
        c.r(14,3,17,27,3);c.e(14,1,17,6,3);c.e(2,11,29,19,3);c.l(4,13,27,13,4);c.e(10,24,21,28,3);c.r(14,9,17,13,10);roundel(6,15);prop(1)
    elif k=='FW190':
        c.r(13,6,18,26,3);c.e(12,2,19,8,3);c.e(13,3,18,7,7);sym([(13,12),(3,13),(3,17),(13,18)],3);c.l(4,13,12,12,4);c.l(19,12,27,13,4)
        sym([(13,24),(10,25),(10,27),(13,27)],3);c.r(14,9,17,13,10);c.r(6,14,7,15,9);c.r(24,14,25,15,9);prop(1)
    elif k=='ZERO':
        c.r(14,5,17,27,3);c.e(12,3,19,9,3);c.e(13,4,18,8,7);c.r(4,12,27,16,3);c.e(1,12,7,16,3);c.e(24,12,30,16,3);c.l(3,12,28,12,4)
        c.e(11,24,20,27,3);c.r(14,9,17,15,10);c.e(4,13,7,16,9);c.e(24,13,27,16,9);prop(2)
    elif k=='F4U':
        c.r(14,2,17,26,3);c.e(13,1,18,5,3);c.e(14,1,17,3,7);sym([(14,13),(9,15),(3,13),(2,15),(9,18),(14,19)],3);c.l(9,15,9,18,2);c.l(22,15,22,18,2)
        sym([(14,24),(9,25),(9,27),(14,27)],3);c.e(14,13,17,17,10);roundel(5,15);prop(0)
    elif k=='YAK3':
        c.r(14,4,17,26,3);c.p([(14,4),(17,4),(16,2),(15,2)],3);sym([(14,12),(5,14),(5,17),(14,18)],3);c.l(6,14,13,13,4);c.l(18,13,25,14,4)
        sym([(14,23),(11,24),(11,26),(14,26)],3);c.r(14,9,17,12,10);c.px(7,15,9);c.px(24,15,9);c.px(15,24,9);prop(2,11,20)
    elif k=='HURRI':
        c.r(13,3,18,27,3);c.e(13,1,18,6,3);sym([(14,11),(2,13),(2,18),(14,19)],3);c.l(3,13,13,11,4);c.l(18,11,28,13,4)
        c.e(10,24,21,28,3);c.e(13,9,18,15,10);c.e(14,10,17,13,2);c.e(14,11,17,14,10);roundel(5,15);prop(1)
    elif k=='P38':
        c.r(1,12,30,15,3);c.l(1,12,30,12,4);c.r(7,6,9,28,3);c.r(22,6,24,28,3);c.e(6,4,10,10,3);c.e(21,4,25,10,3)
        c.l(4,3,11,3,14);c.l(20,3,27,3,14);c.e(13,6,18,19,3);c.e(14,8,17,12,10);c.r(7,26,24,28,3);c.l(7,26,24,26,4);c.px(4,13,9);c.px(27,13,9)
    elif k=='STUKA':
        c.r(14,3,17,27,3);c.e(14,1,17,5,3);sym([(14,13),(10,16),(3,14),(2,16),(10,19),(14,18)],3);c.l(10,16,10,19,2);c.l(21,16,21,19,2)
        c.e(9,11,11,15,2);c.e(20,11,22,15,2);c.r(14,8,17,16,10);c.l(14,11,17,11,2);c.l(14,14,17,14,2)
        sym([(14,24),(9,24),(9,27),(14,27)],3);c.r(6,15,7,16,9);c.r(24,15,25,16,9);prop(1)
    elif k=='SBD':
        c.r(14,3,17,27,3);c.e(13,1,18,6,3);c.r(2,13,29,17,3);c.e(1,13,5,17,3);c.e(26,13,30,17,3);c.l(3,13,28,13,4)
        c.r(5,17,26,17,2);c.r(14,8,17,17,10);c.l(14,12,17,12,2);sym([(14,24),(9,24),(9,27),(14,27)],3);roundel(6,15);c.r(15,18,16,20,15);prop(0)
    return c.grid(1)
def carrier():
    c=Cv(32,32);c.p([(16,0),(20,4),(20,30),(12,30),(12,4)],8);c.r(13,3,19,29,4);c.l(16,4,16,28,5);c.r(19,12,20,17,8);c.r(19,13,20,15,2)
    for y in (8,14,20,26):c.px(14,y,5);c.px(18,y,5)
    return c.grid(1)
spr=[]
for e in range(5):
    t=[]
    t+=tiles4(soldier(e,0),16,16);t+=tiles4(soldier(e,1),16,16)
    t+=tiles4(tank(e,'hull'),32,32);t+=tiles4(tank(e,'tur'),32,32)
    if e==0:
        for k in WW2_PLANES:t+=tiles4(ww2plane(k),32,32)
        t+=tiles4(carrier(),32,32)  # ES_SHIP0 aircraft carrier
    else:
        t+=tiles4(plane(e),32,32);t+=tiles4(plane(e),32,32)
        if e==2:
            c=Cv(32,32);c.p([(16,1),(21,8),(21,28),(11,28),(11,8)],8);c.p([(16,3),(19,9),(19,26),(13,26),(13,9)],14)
            c.r(14,12,18,17,8);c.r(15,13,17,16,11);c.r(15,20,17,23,8);c.l(16,5,16,10,8);c.r(15,26,17,30,4)
            t+=tiles4(c.grid(1),32,32)  # patrol boat / ship (ES_SHIP)
    emit(f"era_spr_{e}",t,"unsigned int");define(f"ES_N{e}",len(t)//8)
define("ES_SOLDIER",0);define("ES_HULL",8);define("ES_TUR",24);define("ES_PLANE",40);define("ES_SHIP",72);define("ES_SHIP0",40+16*len(WW2_PLANES))
# FX sprites (palette FXP)
fx=[]
def t8(rows,mp):return tiles4([[mp.get(ch,0) for ch in r] for r in rows],8,8)
M={'.':0,'k':1,'d':2,'g':3,'l':4,'w':5,'y':6,'o':7,'r':8,'R':9,'c':10,'b':11,'m':12,'G':13,'B':14,'E':15}
fx+=t8(["........","........","...yy...","..ywwy..","..ywwy..","...yy...","........","........"],M)  #0 bullet
fx+=t8(["........","...cc...","..cwwc..",".cwwwwc.",".cwwwwc.","..cwwc..","...cc...","........"],M)  #1 laser
fx+=t8(["........","...mm...","..mwwm..",".mwwwwm.",".mwwwwm.","..mwwm..","...mm...","........"],M)  #2 plasma
fx+=t8(["........","...oo...","..oyyo..",".oywwyo.",".oywwyo.","..oyyo..","...oo...","........"],M)  #3 shell
fx+=t8(["........","....g...","...kkk..","..kGGGk.","..kGdGk.","..kGGGk.","...kkk..","........"],M)  #4 grenade
fx+=t8(["...y....","..yww...",".ywwwy.y","yywwwwy.",".ywwwy..","..yyw...","...y....","........"],M)  #5 flash
fx+=t8(["..lll...",".lllll..","lllgllll","llggglll",".lllll..","..lll...","........","........"],M)  #6 smoke
fx+=t8(["........","...ll...","..lggl..","..lggl..","...ll...","........","........","........"],M)  #7 smoke small
fx+=t8(["...w....","w..w..w.",".w.w.w..","...w....","www.www.","...w....",".w.w.w..","w..w..w."],M)  #8 spark
fx+=t8(["BB......","BB......","........","........","........","........","........","........"],M)  #9 radar ally
fx+=t8(["EE......","EE......","........","........","........","........","........","........"],M)  #10 radar enemy
fx+=t8(["yy......","yy......","........","........","........","........","........","........"],M)  #11 radar player
fx+=t8(["ww......","ww......","........","........","........","........","........","........"],M)  #12 radar obj
fx+=t8(["...w....","...l....","...l....","...l....","..lll...","...o....","..oyo...","...o...."],M)  #13 missile (up)
fx+=t8(["...l....","...l....","...l....","..lll...","...o....","...y....","........","........"],M)  #14 rocket
fx+=t8(["........","........","........","........","........","........","........","........"],M)  #15 blank
def expl(k):
    c=Cv(16,16);r=[3,6,7,7][k]
    if k<3:c.e(8-r,8-r,7+r,7+r,7);c.e(8-r+2,8-r+2,7+r-2,7+r-2,6)
    if k<2:c.e(8-r+3,8-r+3,7+r-3,7+r-3,5)
    if k>=2:
        for i in range(7):
            a=i*0.9;x=8+int(math.cos(a)*(r-2));y=8+int(math.sin(a)*(r-2))
            c.e(x-3,y-3,x+2,y+2,[4,3,2][(i+k)%3])
    return c.grid(0)
for k in range(4):fx+=tiles4(expl(k),16,16)   # 16..31
c=Cv(16,16);c.p([(3,2),(12,2),(12,6),(15,6),(8,14),(1,6),(3,6)],6);c.p([(5,3),(10,3),(10,7),(12,7),(8,11),(4,7),(5,7)],7);fx+=tiles4(c.grid(1),16,16)  #32 arrow
c=Cv(16,16);c.r(2,4,13,13,13);c.r(2,4,13,5,4);c.r(3,8,12,9,2);c.r(6,6,9,11,5);c.r(7,7,8,10,8);fx+=tiles4(c.grid(1),16,16)  #36 crate (green, white cross)
c=Cv(16,16);c.r(1,7,14,8,3);c.l(1,7,14,7,4);c.px(15,7,6);c.px(15,6,7);c.px(14,6,5);fx+=tiles4(c.grid(1),16,16)  #40 bangalore
c=Cv(16,16);c.e(3,3,12,12,13);c.r(7,4,8,11,5);c.r(4,7,11,8,5);fx+=tiles4(c.grid(1),16,16)  #44 health/respawn
c=Cv(16,16)
for (x0,y0,x1,y1) in [(7,0,8,4),(7,11,8,15),(0,7,4,8),(11,7,15,8)]:c.r(x0,y0,x1,y1,5)
c.px(7,7,15);c.px(8,8,15);c.px(7,8,15);c.px(8,7,15)
for (x,y) in [(2,2),(13,2),(2,13),(13,13)]:c.px(x,y,3)
fx+=tiles4(c.grid(0),16,16)  #48 drone reticle
def bracket(col):
    c=Cv(16,16)
    for (x0,y0,dx,dy) in [(0,0,1,1),(15,0,-1,1),(0,15,1,-1),(15,15,-1,-1)]:
        c.l(x0,y0,x0+4*dx,y0,col);c.l(x0,y0,x0,y0+4*dy,col)
    return c.grid(0)
fx+=tiles4(bracket(15),16,16)  #52 enemy (red) target bracket
fx+=tiles4(bracket(6),16,16)   #56 civilian (yellow) bracket
c=Cv(16,16);c.p([(8,1),(14,8),(8,15),(2,8)],14);c.p([(8,4),(11,8),(8,12),(5,8)],0);fx+=tiles4(c.grid(0),16,16)  #60 friendly IFF diamond
emit("fx_tiles",fx,"unsigned int")
define("FX_BULLET",0);define("FX_LASER",1);define("FX_PLASMA",2);define("FX_SHELL",3);define("FX_GRENADE",4);define("FX_FLASH",5)
define("FX_SMOKE",6);define("FX_SMOKE2",7);define("FX_SPARK",8);define("FX_RADAR",9);define("FX_MISSILE",13);define("FX_ROCKET",14)
define("FX_EXPL",16);define("FX_ARROW",32);define("FX_CRATE",36);define("FX_BANG",40);define("FX_MED",44);define("FX_RETICLE",48);define("FX_BRK_E",52);define("FX_BRK_C",56);define("FX_IFF",60);define("FX_NTILES",64)


def preview():
    sheet=Image.new('RGB',(5*112,200),(60,90,60))
    def put(g,ox,oy,pal):
        for y,row in enumerate(g):
            for x,v in enumerate(row):
                if v:sheet.putpixel((ox+x,oy+y),pal[v])
    for e in range(5):
        for bi,(side,pl) in enumerate((('A',0),('E',0),('A',1))):
            pal=[((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3) for v in obj_pal(e,side,pl)]
            put(soldier(e,0),e*112+2,bi*64+2,pal);put(soldier(e,1),e*112+20,bi*64+2,pal)
            put(tank(e,'hull'),e*112+40,bi*64+2,pal);put(tank(e,'tur'),e*112+40,bi*64+2,pal);put(plane(e),e*112+76,bi*64+2,pal)
            put(tank(e,'tur'),e*112+40,bi*64+32,pal);put(tank(e,'hull'),e*112+2,bi*64+28,pal);put(hellcat(),e*112+76,bi*64+32,pal)
    sheet.resize((sheet.width*3,sheet.height*3),Image.NEAREST).save(os.path.join(ROOT,'build','sprites_preview.png'))
    # tilesets
    ts=Image.new('RGB',(16*32,16*12))
    def putm(tl,idx,ox,oy,pal):
        for t in range(4):
            for y in range(8):
                w=tl[(idx*4+t)*8+y]
                for x in range(8):
                    v=(w>>(4*x))&15;c=pal[v]
                    ts.putpixel((ox+(t&1)*8+x,oy+(t>>1)*8+y),((c&31)<<3,((c>>5)&31)<<3,((c>>10)&31)<<3))
    for e in range(5):
        tl=arena_set(e);p=pal16(ARENA_PALS[e])
        for i in range(11):putm(tl,i,i*16,e*16,p)
        tl=sky_set(e);p=pal16(SKY_PALS[e])
        for i in list(range(8))+list(range(24,31)):putm(tl,i,(i if i<8 else i-16)*16,(e+5)*16,p)
    tl=camp_set();p=pal16(CPAL)
    for i in range(30):putm(tl,i,i*16,10*16,p)
    ts.resize((ts.width*3,ts.height*3),Image.NEAREST).save(os.path.join(ROOT,'build','tiles_preview.png'))
preview()
def preview_planes():
    sh=Image.new('RGB',(len(WW2_PLANES)*36,2*36),(40,70,110))
    for side in (0,1):
        pal=[((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3) for v in obj_pal(0,'A' if side==0 else 'E')]
        for i,k in enumerate(WW2_PLANES):
            for y,row in enumerate(ww2plane(k)):
                for x,v in enumerate(row):
                    if v:sh.putpixel((i*36+2+x,side*36+2+y),pal[v])
    sh.resize((sh.width*3,sh.height*3),Image.NEAREST).save(os.path.join(ROOT,'build','ww2_planes_preview.png'))
preview_planes()
# ------------------------------------------------------------------ title illustration (mode 3, 240x160, RGB15)
W,Hh_=240,160
im=Image.new('RGB',(W,Hh_));px=im.load()
for y in range(Hh_):
    t=y/Hh_
    if t<.55:c0,c1,tt=(14,16,44),(120,46,60),t/.55
    else:c0,c1,tt=(120,46,60),(236,130,50),(t-.55)/.45
    col=tuple(int(c0[i]+(c1[i]-c0[i])*tt) for i in range(3))
    for x in range(W):
        d=((x+y)&1)*3
        px[x,y]=tuple(min(255,v+d) for v in col)
for i in range(90):
    x=int(H(500,i)*W);y=int(H(501,i)*70);b=int(120+H(502,i)*135)
    px[x,y]=(b,b,min(255,b+20))
d=ImageDraw.Draw(im)
d.ellipse((150,96,206,152),fill=(255,190,90));d.ellipse((156,102,200,146),fill=(255,214,120))
for y in range(100,150,5):d.line((140,y,216,y),fill=(236,130,50) if y>120 else (210,110,60))
SIL=(18,12,22);SIL2=(44,22,34)
# far hills / city / moon base skyline
pts=[(0,128)]+[(x,122-int(8*math.sin(x/23)+5*math.sin(x/9))) for x in range(0,241,6)]+[(240,160),(0,160)]
d.polygon(pts,fill=SIL2)
for x in range(170,240,9):h=int(8+H(600,x)*16);d.rectangle((x,124-h,x+6,128),fill=SIL2)
d.polygon([(0,140)]+[(x,136-int(4*math.sin(x/17))) for x in range(0,241,8)]+[(240,160),(0,160)],fill=SIL)
# hedgehogs silhouettes
for hx in (22,64):
    d.line((hx-6,140,hx+6,128),fill=SIL,width=2);d.line((hx+6,140,hx-6,128),fill=SIL,width=2);d.line((hx,126,hx,141),fill=SIL,width=2)
# tank silhouette
d.rectangle((88,124,138,138),fill=SIL);d.polygon([(84,138),(142,138),(136,146),(90,146)],fill=SIL)
d.ellipse((100,112,126,128),fill=SIL);d.rectangle((124,117,160,120),fill=SIL)
for wx in range(92,138,8):d.ellipse((wx,138,wx+6,145),fill=SIL2)
# soldier silhouette with rifle
sx=40;d.ellipse((sx-4,96,sx+4,104),fill=SIL);d.polygon([(sx-6,104),(sx+6,104),(sx+5,124),(sx-5,124)],fill=SIL)
d.polygon([(sx-5,123),(sx-1,123),(sx-6,142),(sx-10,142)],fill=SIL);d.polygon([(sx+1,123),(sx+5,123),(sx+9,142),(sx+5,142)],fill=SIL)
d.line((sx-2,112,sx+20,98),fill=SIL,width=3);d.ellipse((sx-6,94,sx+6,100),fill=SIL)
# future trooper silhouette right
sx=212;d.ellipse((sx-5,94,sx+5,104),fill=SIL);d.rectangle((sx-7,104,sx+7,124),fill=SIL);d.rectangle((sx-6,124,sx-2,142),fill=SIL);d.rectangle((sx+2,124,sx+6,142),fill=SIL)
d.line((sx-3,98,sx+3,98),fill=(80,240,255));d.line((sx-14,112,sx+4,110),fill=SIL,width=3)
# planes in sky
def splane(cx,cy,s,kind):
    if kind==0:
        d.polygon([(cx-14*s,cy),(cx+14*s,cy),(cx+12*s,cy+3*s),(cx-12*s,cy+3*s)],fill=SIL);d.ellipse((cx-3*s,cy-6*s,cx+3*s,cy+12*s),fill=SIL)
        d.polygon([(cx-5*s,cy+10*s),(cx+5*s,cy+10*s),(cx,cy+8*s)],fill=SIL)
    else:
        d.polygon([(cx,cy-10*s),(cx+2*s,cy-2*s),(cx+12*s,cy+6*s),(cx+2*s,cy+5*s),(cx+4*s,cy+10*s),(cx,cy+9*s),(cx-4*s,cy+10*s),(cx-2*s,cy+5*s),(cx-12*s,cy+6*s),(cx-2*s,cy-2*s)],fill=SIL)
splane(196,92,1,0);splane(26,72,1,1)
d.line((26,84,10,118),fill=(150,80,80))
# spaceship
d.polygon([(128,88),(148,84),(160,88),(148,92)],fill=SIL2);d.line((120,88,128,88),fill=(80,200,255))
# title text
def big_text(s,cx,y,sc,top,bot,out=(20,10,8)):
    wch=6*sc;x0=cx-(len(s)*wch-sc)//2
    for i,ch in enumerate(s):
        g=glyph(ch)
        for gy in range(7):
            for gx in range(5):
                if not g[gy][gx]:continue
                X=x0+i*wch+gx*sc;Y=y+gy*sc
                for oy in range(-1,sc+2):
                    for ox in range(-1,sc+2):
                        xx,yy=X+ox,Y+oy
                        if 0<=xx<W and 0<=yy<Hh_:px[xx,yy]=out
    for i,ch in enumerate(s):
        g=glyph(ch)
        for gy in range(7):
            for gx in range(5):
                if not g[gy][gx]:continue
                X=x0+i*wch+gx*sc;Y=y+gy*sc
                for oy in range(sc):
                    for ox in range(sc):
                        t=(gy*sc+oy)/(7*sc);col=tuple(int(top[k]+(bot[k]-top[k])*t) for k in range(3))
                        if oy==0 and gy>0 and not g[gy-1][gx]:col=(255,250,220)
                        px[X+ox,Y+oy]=col
big_text("ARSENAL",120,10,4,(255,236,150),(214,110,30))
big_text("OF ERAS",120,46,4,(255,236,150),(214,110,30))
title=[rgb15(px[x,y]) for y in range(Hh_) for x in range(W)]
emit("title_bmp",title)
emit("title_dim",[((v>>1)&0x3DEF) for v in title])
im.resize((720,480),Image.NEAREST).save(os.path.join(ROOT,'build','title_preview.png'))

# music: note freq register values (GBA square: rate = 2048-131072/f)
notes={}
for i,n in enumerate(['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']):
    for o in (3,4,5,6):notes[f"{n}{o}"]=440*2**((o-4)+(i-9)/12)
def nr(n):return 0 if n=='-' else int(2048-131072/notes[n])
song=("G4 - C5 - C5 C5 D5 - E5 - E5 - D5 - C5 - "
      "E5 - G5 - G5 G5 A5 - G5 - E5 - C5 - D5 - "
      "C5 - E5 - D5 C5 B4 - C5 - A4 - G4 - - - "
      "C5 - E5 - G5 - E5 - F5 - D5 - C5 - - - ").split()
emit("title_song",[nr(n) for n in song]);define("SONG_LEN",len(song))
fan="C5 E5 G5 C6 - G5 C6 - - -".split();emit("fanfare",[nr(n) for n in fan]);define("FANFARE_LEN",len(fan))


# ======================= v1.1 PSG music (original compositions) =======================
NN={'C':0,'C#':1,'Db':1,'D':2,'D#':3,'Eb':3,'E':4,'F':5,'F#':6,'Gb':6,'G':7,'G#':8,'Ab':8,'A':9,'A#':10,'Bb':10,'B':11}
def midi(n):
    m=re.match(r'([A-G][#b]?)(\d)',n);return 12*(int(m.group(2))+1)+NN[m.group(1)]
def voice(spec,total):
    out=[]
    for tok in spec.split():
        n,d=(tok.split(':')+['1'])[:2];d=int(d)
        if n=='.':out+=[1]+[0]*(d-1)
        else:out+=[midi(n)]+[0]*(d-1)
    assert len(out)<=total,(len(out),total,spec[:40])
    return out+[0]*(total-len(out))
def drums(spec,total):
    m={'.':0,'K':1,'S':2,'H':3,'R':4,'T':5,'C':6}
    out=[m[c] for c in spec.replace(' ','').replace('|','')];assert len(out)<=total,(len(out),total)
    return (out*((total+len(out)-1)//len(out)))[:total]
def bars(pattern_per_bar,chords,steps):
    # bass from chord roots: pattern like "R:2 .:2 F:2 .:2" R=root F=fifth O=octave
    s=[]
    for ch in chords:
        r=midi(ch);parts=[]
        for tok in pattern_per_bar.split():
            k,d=tok.split(':');d=int(d)
            if k=='.':parts+=[1]+[0]*(d-1)
            else:parts+=[{'R':r,'F':r+7,'O':r+12,'3':r+3,'4':r+4,'m':r-5}[k]]+[0]*(d-1)
        s+=parts
    return s
TRK=[]
def track(name,spd,loop,lead,bass,harm,drum,duty=2,lenv=0,hduty=1,henv=0,wave=0,lvol=11,hvol=6,bvol=2):
    n=len(lead);assert len(bass)==n and len(harm)==n and len(drum)==n,(name,n,len(bass),len(harm),len(drum))
    TRK.append((name,spd,loop,lead,bass,harm,drum,duty,lenv,hduty,henv,wave,lvol,hvol,bvol))
# 0 BUGLE CALL (menus) - bugle harmonic series only (C G C E G), C major, steps=16th
L=voice("G4:2 G4:1 G4:1 C5:4 E5:2 C5:2 G4:4  E5:2 E5:1 E5:1 G5:4 E5:2 C5:2 E5:4  G5:3 E5:1 C5:2 E5:2 G4:3 G4:1 C5:4  E5:2 G4:2 C5:8 .:4 "
        "C5:2 E5:2 G5:4 G5:2 E5:2 C5:4  E5:2 G5:2 C6:6 G5:2 E5:4  G5:2 E5:2 C5:2 E5:2 G4:4 C5:4  C5:12 .:4",128)
B=voice("C3:8 G2:8 C3:8 C3:8 C3:8 G2:8 C3:8 .:8 C3:8 G2:8 C3:8 E3:8 G2:8 G2:8 C3:12 .:4",128)
Hm=voice(".:16 .:16 .:16 E4:8 .:8 .:16 G4:8 E4:8 .:16 E4:12 .:4",128)
D=drums("................"*3+"........R.R.RRRR"+"................"*3+"RRRRRRRRK...K...",128)
track("BUGLE CALL",8,1,L,B,Hm,D,duty=2,lenv=0,lvol=12)
# 1 OMAHA - DOG GREEN: somber, tense, D minor, slow
L=voice("D4:8 F4:4 E4:4  D4:12 .:4  A3:4 C4:4 D4:4 F4:4  E4:12 .:4  F4:6 G4:2 A4:8  G4:4 F4:4 E4:8  D4:4 E4:4 F4:4 A4:4  C#4:12 .:4",128)
B=bars("R:2 .:2 R:2 .:2 R:2 .:2 R:2 .:2",["D2","D2","D2","A1","A#1","A#1","D2","A1"],16)
Hm=voice("A3:16 A3:16 F3:16 E3:16 D3:16 D3:16 F3:16 E3:16",128)
D=drums("K..K............"*3+"K..K........RRRR"+"K..K............"*3+"K..K....RRRRRRRR",128)
track("DOG GREEN",11,1,L,B,Hm,D,duty=1,lenv=0,hduty=0,lvol=10,hvol=5,bvol=3)
# 2 HEDGEROW MARCH: 1940s military march, brass square lead, snare rolls
L=voice("G4:2 C5:2 C5:3 C5:1 E5:4 C5:4  D5:2 E5:2 F5:2 D5:2 G5:8  E5:3 F5:1 G5:2 E5:2 C5:2 E5:2 D5:4  B4:2 C5:2 D5:2 B4:2 G4:8 "
        "G4:2 C5:2 C5:3 C5:1 E5:4 G5:4  A5:4 G5:2 F5:2 E5:4 C5:4  F5:3 E5:1 D5:2 C5:2 B4:4 D5:4  C5:8 .:4 G4:4",128)
B=bars("R:2 .:2 F:2 .:2 R:2 .:2 F:2 .:2",["C3","G2","C3","G2","C3","F2","G2","C3"],16)
Hm=voice("E4:8 C4:8 B3:8 B3:8 C4:8 G3:8 D4:8 D4:8 E4:8 E4:8 F4:8 A4:8 A3:8 B3:8 E4:8 .:8",128)
D=drums("K.S.K.S.K.S.KSSS"*3+"RRRRRRRRK.S.K.S."+"K.S.K.S.K.S.KSSS"*3+"K.S.K.S.RRRRRRRR",128)
track("HEDGEROW MARCH",7,1,L,B,Hm,D,duty=2,lenv=0,lvol=12)
# 3 SWING SQUADRON: big-band swing dogfight (12 steps per bar = triplet feel)
L=voice("F4:2 A4:1 C5:2 D5:1 F5:3 D5:3  D#5:2 D5:1 C5:2 A4:1 C5:6  D5:2 F5:1 A5:3 G5:2 F5:1 D5:3  F5:9 .:3 "
        "A4:2 C5:1 D5:2 F5:1 G5:2 F5:1 D5:3  C5:2 A4:1 G4:2 F4:1 A4:6  A#4:2 D5:1 F5:2 A#5:1 A5:2 G5:1 F5:3  F5:3 C5:3 F4:3 .:3",96)
B=voice("F2:3 A2:3 C3:3 D3:3 D#3:3 D3:3 C3:3 A2:3 A#2:3 D3:3 F3:3 D3:3 F2:3 C3:3 F3:3 C3:3 "
        "F2:3 A2:3 C3:3 A2:3 G2:3 A#2:3 C3:3 E2:3 A#2:3 D3:3 F3:3 A#2:3 C3:3 E3:3 F2:3 .:3",96)
Hm=voice((".:2 A4:1 .:5 A4:1 .:3 ")*2+(".:2 A#4:1 .:5 A#4:1 .:3 ")*2+(".:2 A4:1 .:5 C5:1 .:3 ")*2+(".:2 D5:1 .:5 C5:1 .:3 ")*2,96)
D=drums("K..HSH..HSH.",96)
track("SWING SQUADRON",6,1,L,B,Hm,D,duty=2,lenv=1,hduty=1,henv=1,lvol=12,hvol=7)
# 4 IRON CURTAIN: Cold War spy synth, A minor ostinato
L=voice("E5:6 D5:2 C5:4 B4:4  A4:12 .:4  C5:4 E5:4 A5:6 G5:2  F5:8 E5:8  E5:6 D5:2 C5:4 B4:4  A4:4 C5:4 B4:4 G#4:4  A4:4 B4:4 C5:4 D5:4  E5:12 .:4",128)
B=bars("R:2 R:2 3:2 R:2 F:2 R:2 4:2 3:2",["A2","A2","F2","E2","A2","E2","F2","E2"],16)
Hm=voice(".:16 .:8 C4:8 .:16 .:8 G#3:8 .:16 .:16 .:16 B3:16",128)
D=drums("K...S...K.K.S..H",128)
track("IRON CURTAIN",7,1,L,B,Hm,D,duty=1,lenv=0,hduty=2,lvol=10,bvol=2,wave=1)
# 5 TASK FORCE: Modern, driving E minor
L=voice("E5:4 B4:4 E5:2 F#5:2 G5:4  F#5:4 D5:4 B4:8  C5:4 E5:4 G5:4 A5:4  B5:12 .:4  E5:4 B4:4 E5:2 F#5:2 G5:4  A5:4 G5:4 F#5:4 D5:4  C5:4 D5:4 E5:4 F#5:4  E5:12 .:4",128)
B=bars("R:1 R:1 O:1 R:1 R:1 R:1 O:1 R:1 R:1 R:1 O:1 R:1 R:1 F:1 O:1 F:1",["E2","D2","C2","B1","E2","D2","C2","B1"],16)
Hm=voice("B4:16 A4:16 G4:16 F#4:16 B4:16 D5:16 A4:16 G4:16",128)
D=drums("K.H.S.KHK.H.S.HH",128)
track("TASK FORCE",6,1,L,B,Hm,D,duty=2,lenv=0,hduty=0,lvol=11,hvol=5,wave=1)
# 6 NEON PROTOCOL: Futuristic synth arps, C minor
def arp(ns):return ' '.join(f"{n}:1" for n in ns*4)
L=voice(" ".join(arp(a) for a in [["C5","D#5","G5","C6"],["G#4","C5","D#5","G#5"],["A#4","D5","F5","A#5"],["G4","B4","D5","G5"],
                                  ["C5","D#5","G5","C6"],["G#4","C5","D#5","G#5"],["D#5","G5","A#5","D#6"],["G4","B4","D5","G5"]]),128)
B=bars("R:2 O:2 R:2 O:2 R:2 O:2 R:2 O:2",["C2","G#1","A#1","G1","C2","G#1","D#2","G1"],16)
Hm=voice("G4:16 G#4:16 F4:16 D4:16 G4:12 A#4:4 C5:16 B4:16",128)
D=drums("K.H.KSH.K.H.KSHH",128)
track("NEON PROTOCOL",6,1,L,B,Hm,D,duty=0,lenv=1,hduty=2,lvol=10,hvol=7,wave=1)
# 7 VOID DRIFT: Space ambient, D lydian
L=voice("A5:8 E5:8  F#5:12 .:4  G#5:8 E5:4 B4:4  C#5:16  A5:8 B5:8  G#5:8 F#5:8  E5:8 C#5:8  D5:16",128)
B=voice("D2:16 D2:16 E2:16 A1:16 D2:16 B1:16 E2:16 A1:16",128)
Hm=voice(" ".join(f"{n}:4" for n in ["D4","A4","E5","A4"]*2+["E4","B4","G#4","B4","A3","E4","C#5","E4"]+["D4","A4","E5","A4","B3","F#4","D5","F#4","E4","B4","G#4","B4","A3","E4","C#5","E4"]),128)
D=drums("........C.......",128)
track("VOID DRIFT",12,1,L,B,Hm,D,duty=2,lenv=0,hduty=1,henv=2,lvol=9,hvol=6,bvol=3)
# 8 ARSENAL THEME (title) - from v1.0 title melody, now with bass and drums
old=("G4 - C5 - C5 C5 D5 - E5 - E5 - D5 - C5 - E5 - G5 - G5 G5 A5 - G5 - E5 - C5 - D5 - "
     "C5 - E5 - D5 C5 B4 - C5 - A4 - G4 - - - C5 - E5 - G5 - E5 - F5 - D5 - C5 - - - ").split()
L=[0 if t=='-' else midi(t) for t in old]
B=bars("R:4 F:4 R:4 F:4",["C3","C3","F2","C3"],16)
Hm=[1]*64
D=drums("K...S...K.K.S...",64)
track("ARSENAL THEME",9,1,L,B,Hm,D,duty=2,lenv=2,lvol=11)
# 9 VICTORY FANFARE (one-shot)
L=voice("C5:2 E5:2 G5:2 C6:4 G5:2 C6:8 .:4",24)
B=voice("C3:6 G2:4 C3:10 .:4",24)
Hm=voice("E4:6 E4:4 G4:10 .:4",24)
D=drums("RRRRRRK...K.....R.R.K...",24)
track("VICTORY",7,0,L,B,Hm,D,duty=2,lenv=0,lvol=13)

# ======================= v1.2 additional PSG music (original compositions) =======================
def R8(s):return s.replace('|',' ')
def chordb(pat,roots):return bars(pat,roots,16)
# 10 DMZ PATROL - Spec Ops W1 forest / W5 city, G minor
L=voice(R8("G4:4 A#4:2 C5:2 D5:4 C5:2 A#4:2 | A4:4 F4:4 G4:8 | D5:4 F5:2 D5:2 C5:4 A#4:4 | A4:6 A#4:2 C5:8 | G4:4 A#4:2 C5:2 D5:4 G5:4 | F5:2 D#5:2 D5:4 C5:8 | A#4:4 C5:2 D5:2 D#5:4 D5:2 C5:2 | D5:16"),128)
B=chordb("R:2 .:1 R:1 F:2 .:2 R:2 .:1 R:1 O:2 .:2",["G2","F2","D#2","F2","G2","D#2","C2","D2"])
Hm=voice("D4:16 C4:16 A#3:16 C4:16 D4:16 A#3:16 G3:16 F#3:16",128)
track("DMZ PATROL",7,1,L,B,Hm,drums("K...S..KK...S..H",128),duty=2,hduty=1,lvol=11,hvol=5)
# 11 PERMAFROST - arctic, E minor, sparse
L=voice(R8("B4:8 E5:8 | D5:4 B4:4 A4:8 | G4:8 A4:4 B4:4 | F#4:16 | B4:8 E5:4 F#5:4 | G5:8 F#5:4 E5:4 | D5:8 B4:8 | E5:16"),128)
B=voice("E2:16 G2:16 C2:16 B1:16 E2:16 C2:16 D2:16 E2:16",128)
Hm=voice(" ".join(f"{n}:4" for n in "E4 G4 B4 G4 G4 B4 D5 B4 C4 E4 G4 E4 B3 D#4 F#4 D#4 E4 G4 B4 G4 C4 E4 G4 E4 D4 F#4 A4 F#4 E4 G4 B4 E5".split()),128)
track("PERMAFROST",10,1,L,B,Hm,drums("........H.......",128),duty=2,lenv=0,hduty=0,henv=2,lvol=9,hvol=6,bvol=3)
# 12 DUNE RAIDERS - canyon desert, D phrygian-dominant, driving
L=voice(R8("D5:2 D#5:2 F#5:4 G5:2 F#5:2 D#5:4 | D5:4 C5:2 A#4:2 A4:8 | A4:2 A#4:2 C5:2 D5:2 D#5:4 D5:4 | F#5:8 D5:8 | G5:4 F#5:2 D#5:2 D5:4 C5:4 | A#4:2 C5:2 D5:4 D#5:8 | D5:2 D#5:2 D5:2 C5:2 A#4:2 A4:2 G4:4 | A4:4 D#5:4 D5:8"),128)
B=chordb("R:2 R:1 R:1 .:2 R:2 F:2 R:1 R:1 .:2 O:2",["D2","D2","G2","D2","G2","D#2","C2","D2"])
Hm=voice("A4:16 G4:16 A#4:16 A4:16 A#4:16 G4:16 F#4:16 F#4:16",128)
track("DUNE RAIDERS",6,1,L,B,Hm,drums("K.TTK.S.K.TTKSS.",128),duty=1,hduty=2,lvol=11,hvol=5,wave=1)
# 13 JUNGLE RIVER - pentatonic A minor, toms
L=voice(R8("A4:2 C5:2 D5:2 E5:4 G5:2 E5:4 | D5:2 C5:2 A4:4 G4:4 A4:4 | C5:2 D5:2 E5:2 G5:2 A5:4 G5:4 | E5:12 .:4 | A5:2 G5:2 E5:2 D5:2 E5:4 G5:4 | A5:4 G5:2 E5:2 D5:8 | C5:2 D5:2 E5:4 D5:2 C5:2 A4:4 | A4:12 .:4"),128)
B=chordb("R:3 R:1 .:2 F:2 R:3 R:1 .:2 O:2",["A1","A1","C2","E2","A1","G1","F1","E2"])
Hm=voice("E4:8 G4:8 E4:8 C4:8 G4:8 A4:8 G4:16 E4:8 D4:8 E4:16 C4:8 D4:8 E4:16",128)
track("JUNGLE RIVER",6,1,L,B,Hm,drums("T..T..K.T.T.K.S.",128),duty=0,lenv=1,hduty=2,lvol=11,hvol=5)
# 14 ISLAND FORTRESS - heroic D major march
L=voice(R8("D5:4 A4:2 D5:2 F#5:4 E5:2 D5:2 | E5:4 C#5:4 A4:8 | B4:4 D5:2 E5:2 F#5:4 G5:4 | A5:12 .:4 | B5:4 A5:2 G5:2 F#5:4 E5:4 | D5:2 E5:2 F#5:4 E5:8 | G5:4 F#5:2 E5:2 D5:4 C#5:4 | D5:12 .:4"),128)
B=chordb("R:2 .:2 F:2 .:2 R:2 .:2 F:2 O:2",["D2","A1","G1","D2","G1","D2","A1","D2"])
Hm=voice("F#4:16 E4:16 D4:16 F#4:16 G4:16 F#4:16 E4:16 F#4:16",128)
track("ISLAND FORTRESS",7,1,L,B,Hm,drums("K.S.K.SSK.S.KRRR",128),duty=2,hduty=1,lvol=12,hvol=6)
# 15 SAND AIRSTRIP - Wagner desert airbase (World 7), B minor menace
L=voice(R8("B4:6 C#5:2 D5:4 C#5:4 | B4:4 F#4:4 B4:8 | D5:6 E5:2 F#5:4 E5:4 | D5:4 C#5:4 A4:8 | B4:6 C#5:2 D5:4 F#5:4 | G5:4 F#5:4 E5:8 | D5:4 C#5:4 B4:4 A#4:4 | B4:16"),128)
B=chordb("R:1 R:1 .:2 R:1 R:1 .:2 R:1 R:1 F:2 R:2 O:2",["B1","B1","G1","A1","B1","E2","F#1","B1"])
Hm=voice(".:8 F#4:8 .:8 F#4:8 .:8 D4:8 .:8 E4:8 .:8 F#4:8 .:8 G4:8 .:8 F#4:8 .:8 F#4:8",128)
track("SAND AIRSTRIP",6,1,L,B,Hm,drums("K..HK.S.K.KHS.HH",128),duty=1,hduty=2,lvol=10,hvol=6,wave=1)
# 16 PADDY FIELDS - Vietnam / Cold War (World 8), E minor 60s rock feel
L=voice(R8("E5:3 E5:1 D5:2 B4:2 A4:4 B4:4 | G4:2 A4:2 B4:4 E4:8 | E5:3 E5:1 G5:2 E5:2 D5:4 B4:4 | A4:16 | C5:4 B4:2 A4:2 G4:4 A4:4 | B4:4 D5:4 E5:8 | G5:2 F#5:2 E5:2 D5:2 B4:4 A4:4 | B4:12 .:4"),128)
B=chordb("R:2 R:2 F:2 R:2 O:2 R:2 F:2 R:2",["E2","E2","E2","A1","C2","G1","D2","E2"])
Hm=voice("B3:16 G3:16 B3:16 C#4:16 E4:16 D4:16 B3:16 B3:16",128)
track("PADDY FIELDS",6,1,L,B,Hm,drums("K.H.S.H.K.KHS.HH",128),duty=2,lenv=0,hduty=0,lvol=11,hvol=5)
# 17 OLD CONTINENT - WW2 Europe (World 9), F minor somber march
L=voice(R8("C5:4 C5:3 C5:1 F5:4 G#5:4 | G5:4 F5:4 E5:8 | F5:4 G5:2 G#5:2 A#5:4 G#5:4 | G5:12 .:4 | G#5:4 G5:2 F5:2 D#5:4 C#5:4 | C5:4 A#4:4 C5:8 | C#5:4 C5:2 A#4:2 G4:4 E4:4 | F4:12 .:4"),128)
B=chordb("R:4 F:4 R:4 F:4",["F2","C2","A#1","C2","C#2","F2","A#1","F2"])
Hm=voice("G#4:16 G4:16 F4:16 E4:16 F4:16 F4:16 E4:16 C4:16",128)
track("OLD CONTINENT",8,1,L,B,Hm,drums("K.S.K.SRK.S.KRRR",128),duty=2,hduty=1,lvol=11,hvol=6,bvol=3)
# 18 XENO SIGNAL - alien worlds (World 10), whole-tone arps
L=voice(" ".join(arp(a) for a in [["C5","E5","F#5","A#5"],["D5","F#5","G#5","C6"],["C5","E5","F#5","A#5"],["A#4","D5","E5","G#5"],
                                  ["G#4","C5","D5","F#5"],["A#4","D5","E5","G#5"],["C5","E5","F#5","A#5"],["D5","F#5","G#5","C6"]]),128)
B=voice("C2:16 D2:16 C2:16 A#1:16 G#1:16 A#1:16 C2:16 C2:16",128)
Hm=voice("E4:32 F#4:32 G#4:32 E4:32",128)
track("XENO SIGNAL",8,1,L,B,Hm,drums("K.......K.C.....",128),duty=0,lenv=1,hduty=2,henv=2,lvol=9,hvol=7,wave=1)
# 19 BOSS BATTLE - fast C minor
L=voice(R8("C5:2 C5:2 D#5:2 C5:2 G5:4 F#5:4 | F5:2 D#5:2 D5:2 D#5:2 C5:8 | C5:2 C5:2 D#5:2 F5:2 G5:2 G#5:2 G5:4 | B4:8 D5:8 | C6:4 A#5:2 G#5:2 G5:4 F5:4 | D#5:2 F5:2 G5:4 C5:8 | G#5:4 G5:2 F5:2 D#5:4 D5:4 | C5:8 B4:8"),128)
B=chordb("R:1 R:1 O:1 R:1 R:1 R:1 O:1 R:1 R:1 R:1 O:1 R:1 F:1 F:1 O:1 F:1",["C2","C2","C2","G1","G#1","G#1","F1","G1"])
Hm=voice("G4:8 G4:8 G#4:8 G4:8 G4:8 G4:8 D4:16 D#4:8 C4:8 D#4:8 G4:8 F4:16 D4:16",128)
track("BOSS BATTLE",5,1,L,B,Hm,drums("K.HSK.HSKKHSK.SS",128),duty=2,hduty=1,lvol=12,hvol=6)
# 20 FORTRESS - ominous chromatic, D minor
L=voice(R8("D5:4 C#5:4 D5:4 F5:4 | E5:4 D5:4 C#5:8 | D5:4 C#5:4 D5:4 G5:4 | F5:4 E5:4 D5:8 | A#5:4 A5:4 G5:4 F5:4 | E5:4 F5:4 G5:8 | F5:4 E5:4 D5:4 C#5:4 | D5:16"),128)
B=voice("D2:16 A1:16 D2:16 A#1:16 G1:16 A1:16 A#1:16 D2:16",128)
Hm=voice("A4:16 A4:16 A#4:16 A4:16 D5:16 C#5:16 A4:16 F4:16",128)
track("FORTRESS",8,1,L,B,Hm,drums("K.......K...S...",128),duty=1,hduty=0,lvol=10,hvol=5,bvol=3,wave=1)
# 21 WORLD MAP - bouncy C major
L=voice(R8("C5:2 E5:2 G5:2 E5:2 F5:4 D5:4 | E5:2 C5:2 D5:2 B4:2 C5:8 | A4:2 C5:2 F5:2 A5:2 G5:4 E5:4 | D5:12 .:4 | C5:2 E5:2 G5:2 C6:2 B5:4 G5:4 | A5:2 G5:2 F5:2 E5:2 D5:8 | F5:2 E5:2 D5:2 C5:2 B4:4 D5:4 | C5:12 .:4"),128)
B=chordb("R:2 F:2 O:2 F:2 R:2 F:2 O:2 F:2",["C2","G1","F1","G1","C2","F1","G1","C2"])
Hm=voice("E4:8 G4:8 D4:8 E4:8 F4:8 A4:8 B3:16 E4:8 G4:8 F4:8 A4:8 D4:8 F4:8 E4:16",128)
track("WORLD MAP",7,1,L,B,Hm,drums("K.H.S.H.K.H.S.HH",128),duty=2,lenv=1,hduty=1,lvol=11,hvol=5)
# 22 ASCENT - launch countdown pulses then a soaring climb (X-15 / rocket launch)
L=voice(R8("A4:4 .:4 A4:4 .:4 | A4:4 .:4 A4:4 .:4 | E5:8 A5:8 | G#5:8 E5:8 | F#5:8 A5:8 | B5:8 C#6:8 | D6:8 C#6:4 B5:4 | A5:16"),128)
B=voice("A1:4 .:4 A1:4 .:4 A1:4 .:4 A1:4 .:4 A1:16 E2:16 D2:16 E2:16 D2:16 A1:16",128)
Hm=voice("E4:4 .:4 E4:4 .:4 E4:4 .:4 E4:4 .:4 C#5:16 B4:16 A4:16 G#4:16 F#4:16 E4:16",128)
D=drums("K"+"."*15+"K"+"."*15+("K"+"."*7)*2+"K"+"R"*7+"K"+"R"*7+("C"+"."*7)*2+"K...S..."*2+"K.S."*8,128)
track("ASCENT",9,1,L,B,Hm,D,duty=2,lenv=0,hduty=0,henv=0,lvol=12,hvol=6)
# 23 CRUISE - World Flight cruising, relaxed F major
L=voice(R8("A4:4 C5:4 F5:6 E5:2 | D5:4 C5:4 A4:8 | A#4:4 D5:4 F5:4 A5:4 | G5:12 .:4 | A5:4 G5:4 F5:4 D5:4 | C5:4 D5:4 F5:8 | G5:4 F5:2 E5:2 D5:4 E5:4 | F5:16"),128)
B=chordb("R:4 F:4 O:4 F:4",["F2","D2","A#1","C2","D2","A#1","C2","F2"])
Hm=voice("C4:16 A3:16 D4:16 E4:16 F4:16 F4:16 E4:16 A4:16",128)
track("CRUISE",9,1,L,B,Hm,drums("K...H...S...H...",128),duty=2,lenv=0,hduty=1,henv=2,lvol=10,hvol=6)
# 24 BROADSIDE - naval battle, G minor 6/8 (12-step bars)
L=voice(R8("G4:3 A#4:3 D5:3 D5:3 | C5:3 A#4:3 A4:6 | A#4:3 C5:3 D5:3 G5:3 | F5:6 D5:6 | D#5:3 D5:3 C5:3 A#4:3 | A4:3 F#4:3 G4:6 | D5:3 D5:3 G5:3 F5:3 | D5:6 G4:6"),96)
B=voice("G2:6 D2:6 G2:6 D2:6 G2:6 C2:6 F2:6 A#1:6 C2:6 D2:6 G2:6 D2:6 G2:6 F2:6 G2:6 G2:6",96)
Hm=voice("A#3:12 A3:12 A#3:12 A3:12 G3:12 A3:12 A#3:12 A#3:12",96)
track("BROADSIDE",7,1,L,B,Hm,drums("K..S..K.KS..",96),duty=2,hduty=1,lvol=12,hvol=6)
# 25 LOITER - Drone Ops tension, A minor pulse
L=voice(R8(".:8 E5:4 F5:4 | E5:16 | .:8 E5:4 G5:4 | F5:12 E5:4 | .:8 C5:4 D5:4 | E5:8 B4:8 | .:8 A4:4 B4:4 | C5:8 B4:8"),128)
B=chordb("R:1 .:1 R:1 .:1 R:1 .:1 R:1 .:1 R:1 .:1 R:1 .:1 R:1 .:1 R:1 .:1",["A1","A1","A1","A1","F1","F1","E1","E1"])
Hm=voice("A3:32 C4:32 A3:32 G#3:32",128)
track("LOITER",7,1,L,B,Hm,drums("K.......S......H",128),duty=1,lenv=0,hduty=0,henv=2,lvol=9,hvol=5,wave=1)
# 26 KC'S BOOT CAMP - cheerful tutorial theme, G major
L=voice(R8("G4:2 B4:2 D5:2 B4:2 G5:4 D5:4 | E5:2 D5:2 C5:2 A4:2 B4:8 | C5:2 E5:2 G5:2 E5:2 D5:2 B4:2 G4:4 | A4:4 B4:2 C5:2 D5:8 | G5:2 F#5:2 E5:2 D5:2 E5:4 C5:4 | D5:2 C5:2 B4:2 A4:2 G4:8 | A4:2 B4:2 C5:2 D5:2 E5:2 F#5:2 G5:4 | G5:4 D5:4 G4:8"),128)
B=chordb("R:2 F:2 O:2 F:2 R:2 F:2 O:2 F:2",["G2","C2","C2","D2","C2","G2","D2","G2"])
Hm=voice("B3:16 G3:16 E4:16 F#4:16 G4:16 D4:16 F#4:16 B3:16",128)
track("KC'S BOOT CAMP",6,1,L,B,Hm,drums("K.H.S.HKK.H.S.H.",128),duty=2,lenv=1,hduty=1,lvol=11,hvol=5)
# 27 DEFEAT (one-shot)
track("DEFEAT",8,0,voice("G4:4 F#4:4 F4:4 E4:12 .:8",32),voice("C3:4 B2:4 A#2:4 A2:12 .:8",32),voice("E4:4 D#4:4 D4:4 C#4:12 .:8",32),drums("K...K...K...C...................",32),duty=1,lvol=11)
# 28 MISSION CLEAR (one-shot)
track("MISSION CLEAR",6,0,voice("C5:2 D5:2 E5:2 G5:2 E5:2 G5:2 C6:8 .:12",32),voice("C3:8 G2:4 C3:8 .:12",32),voice("E4:8 B4:4 E5:8 .:12",32),drums("K.S.K.S.RRRRK...................",32),duty=2,lvol=13)
# note tables: square x=2048-131072/f, wave(32 samples) x=2048-65536/f
f=lambda m:440*2**((m-69)/12)
emit("sq_freq",[max(0,min(2047,int(round(2048-131072/f(m))))) if m>=36 else 0 for m in range(110)])
emit("wv_freq",[max(0,min(2047,int(round(2048-65536/f(m))))) if m>=24 else 0 for m in range(110)])
for i,t in enumerate(TRK):
    name,spd,loop,lead,bass,harm,drum=t[:7]
    emit(f"trk{i}",[v for st in zip(lead,bass,harm,drum) for v in st],"unsigned char")
emit("trk_meta",[v for t in TRK for v in (len(t[3]),t[1],t[2],t[7],t[8],t[9],t[10],t[11],t[12],t[13],t[14],0)],"unsigned char")
Hh.append("extern const unsigned char*const trk_data[%d];"%len(TRK))
C.append("const unsigned char*const trk_data[%d]={%s};"%(len(TRK),",".join(f"trk{i}" for i in range(len(TRK)))))
define("NTRK",len(TRK))
# wave shapes (32 x 4-bit, packed two per byte, high nibble first)
tri=[int(round(7.5+7.5*(1-abs((i/16.0)-1)*2))) for i in range(32)]
org=[min(15,int(7.5+5*math.sin(2*math.pi*i/32)+2.5*math.sin(4*math.pi*i/32))) for i in range(32)]
def pack(w):return [(w[2*i]<<4)|w[2*i+1] for i in range(16)]
emit("wave_shapes",pack(tri)+pack(org),"unsigned char")
exec(open(os.path.join(ROOT,'tools','gen_vehicles.py')).read())
exec(open(os.path.join(ROOT,'tools','gen_world.py')).read())
exec(open(os.path.join(ROOT,'tools','gen_platform.py')).read())
exec(open(os.path.join(ROOT,'tools','gen_levels.py')).read())
exec(open(os.path.join(ROOT,'tools','gen_kc.py')).read())
open(os.path.join(ROOT,'src','assets.h'),'w').write("#pragma once\n"+"\n".join(Hh)+"\n")
open(os.path.join(ROOT,'src','assets.c'),'w').write('#include "assets.h"\n'+"\n".join(C)+"\n")
print("assets ok", len(C),"arrays")
