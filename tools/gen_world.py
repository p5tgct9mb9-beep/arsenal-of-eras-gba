# v1.2 FLIGHT AROUND THE WORLD assets: world tilemap (512x256 cells, 16px each = 8192x4096 px, wraps E-W),
# 8bpp affine terrain tiles, world palette, landmark icons, aircraft sprites (X-15/SR-71/Concorde/P-51D/B-52),
# clouds + near-space curvature vignette, and src/worlddb.inc (landmarks, airfields, ring courses).
import os,math
WW,WH=512,256
LAT0,LAT1=84.0,-84.0
def cell_ll(x,y):return LAT0-(y+.5)*(LAT0-LAT1)/WH,-180+(x+.5)*360/WW
def ll_cell(lat,lon):return int((lon+180)/360*WW)%WW,int((LAT0-lat)/(LAT0-LAT1)*WH)
mpath=os.path.join(ROOT,'tools','world_mask.bin')
if not os.path.exists(mpath):
    from global_land_mask import globe  # one-time: pip install global-land-mask (result is cached in tools/world_mask.bin)
    bits=bytearray(WW*WH//8)
    for y in range(WH):
        for x in range(WW):
            la,lo=cell_ll(x,y);land=0
            for dy in (-.2,.2):
                for dx in (-.2,.2):land+=globe.is_land(la+dy*0.65,lo+dx*0.7)
            if land>=2:bits[(y*WW+x)>>3]|=1<<(x&7)
    open(mpath,'wb').write(bytes(bits))
bits=open(mpath,'rb').read()
LAND=[[ (bits[(y*WW+x)>>3]>>(x&7))&1 for x in range(WW)] for y in range(WH)]
for y in range(WH):
    la,_=cell_ll(0,y)
    if la<-70:LAND[y]=[1]*WW  # Antarctic interior / ice shelves
LM=[ # name, place, lat, lon, icon, fact
 ("STATUE OF LIBERTY","NEW YORK, USA",40.69,-74.04,0,"COPPER LADY, DEDICATED 1886"),
 ("GOLDEN GATE BRIDGE","SAN FRANCISCO, USA",37.82,-122.48,1,"1.7 MILE SPAN, OPENED 1937"),
 ("GRAND CANYON","ARIZONA, USA",36.10,-112.10,2,"CARVED BY THE COLORADO RIVER"),
 ("EIFFEL TOWER","PARIS, FRANCE",48.86,2.29,3,"IRON LATTICE TOWER OF 1889"),
 ("BIG BEN","LONDON, UK",51.50,-0.12,4,"THE GREAT BELL OF WESTMINSTER"),
 ("PYRAMIDS OF GIZA","CAIRO, EGYPT",29.98,31.13,5,"4,500 YEARS OLD"),
 ("TAJ MAHAL","AGRA, INDIA",27.17,78.04,6,"WHITE MARBLE MAUSOLEUM"),
 ("GREAT WALL","NORTHERN CHINA",40.43,116.57,7,"THOUSANDS OF MILES OF WALL"),
 ("MOUNT FUJI","HONSHU, JAPAN",35.36,138.73,8,"12,388 FT VOLCANIC PEAK"),
 ("SYDNEY OPERA HOUSE","SYDNEY, AUSTRALIA",-33.86,151.21,9,"SAIL-SHAPED SHELLS, 1973"),
 ("CHRIST THE REDEEMER","RIO DE JANEIRO, BRAZIL",-22.95,-43.21,10,"ATOP CORCOVADO MOUNTAIN"),
 ("THE KREMLIN","MOSCOW, RUSSIA",55.75,37.62,11,"FORTRESS ON THE MOSKVA"),
 ("KILIMANJARO","TANZANIA",-3.07,37.35,12,"AFRICA'S HIGHEST PEAK"),
 ("ANTARCTICA","MCMURDO SOUND",-77.85,166.67,13,"THE FROZEN CONTINENT"),
 ("MOUNT EVEREST","NEPAL / TIBET",27.99,86.93,14,"29,032 FT - TOP OF THE WORLD"),
 ("COLOSSEUM","ROME, ITALY",41.89,12.49,15,"AMPHITHEATRE, AD 80"),
 ("MACHU PICCHU","ANDES, PERU",-13.16,-72.55,16,"15TH CENTURY INCA CITADEL"),
 ("ULURU","OUTBACK, AUSTRALIA",-25.34,131.04,17,"SANDSTONE MONOLITH"),
 ("NIAGARA FALLS","USA / CANADA",43.08,-79.07,18,"3,160 TONS OF WATER A SECOND"),
]
AF=[ # airfields (runway E-W, 3 cells): name, lat, lon
 ("EDWARDS AFB",34.90,-117.88),("ANCHORAGE",61.25,-149.80),("HONOLULU",21.32,-157.92),("GOOSE BAY",53.32,-60.42),
 ("KEFLAVIK",63.99,-22.60),("RAF FAIRFORD",51.68,-1.79),("CAIRO",30.12,31.41),("DUBAI",25.25,55.36),("DELHI",28.56,77.10),
 ("DIEGO GARCIA",-7.31,72.41),("SINGAPORE",1.36,103.99),("GUAM",13.58,144.93),("TOKYO",35.55,139.78),("DARWIN",-12.41,130.88),
 ("SYDNEY",-33.94,151.18),("NAIROBI",-1.32,36.93),("CAPE TOWN",-33.97,18.60),("RIO DE JANEIRO",-22.81,-43.25),("LIMA",-12.02,-77.11),
 ("MEXICO CITY",19.44,-99.07),("CAPE CANAVERAL",28.49,-80.58),("MOSCOW",55.97,37.41),("BEIJING",40.08,116.58),("MCMURDO ICE RWY",-77.0,164.0),
]
RINGS=[ # course name, blurb, rings as (lat, lon, alt ft)
 ("MOJAVE GATES","LOW AND FAST OVER THE HIGH DESERT",[(35.5,-117.3,42000),(36.2,-116.2,34000),(36.8,-114.9,26000),(36.1,-113.5,18000),(35.2,-112.3,14000),(34.4,-113.8,10000),(34.0,-115.6,7000),(34.6,-117.0,5000)]),
 ("EDGE OF SPACE","CLIMB THROUGH THE GATES TO 300,000 FT",[(36.0,-115.5,60000),(37.4,-112.0,110000),(38.6,-108.0,170000),(39.4,-103.5,230000),(39.8,-98.5,290000),(39.5,-93.5,250000),(38.8,-88.5,160000),(38.0,-84.0,80000)]),
 ("TRANSATLANTIC","NEW YORK TO LONDON DASH",[(40.7,-73.5,50000),(43.5,-64.0,70000),(46.5,-52.5,90000),(49.0,-41.0,100000),(51.0,-30.0,90000),(52.0,-19.5,70000),(51.8,-10.0,50000),(51.5,-1.0,30000)]),
]
# ---------------------------------------------------------------- classify
from collections import deque
dist=[[99]*WW for _ in range(WH)];dq=deque()
for y in range(WH):
    for x in range(WW):
        if LAND[y][x]:dist[y][x]=0;dq.append((x,y))
while dq:
    x,y=dq.popleft();d=dist[y][x]
    if d>=6:continue
    for dx,dy in((1,0),(-1,0),(0,1),(0,-1)):
        nx,ny=(x+dx)%WW,y+dy
        if 0<=ny<WH and dist[ny][nx]>d+1:dist[ny][nx]=d+1;dq.append((nx,ny))
def inbox(la,lo,a0,a1,o0,o1):return a0<=la<=a1 and o0<=lo<=o1
DES=[(15,32,-17,35),(14,31,35,58),(-31,-19,117,142),(37,47,88,112),(30,37.5,-118,-106),(-28,-18,-71,-68),(-28,-19,14,25),(36,44,52,68),(24,30,66,74)]
MNT=[(32,60,-123,-105),(27,37,70,100),(44,48,5,16),(50,68,57,61),(60,70,6,18),(30,36,-8,8),(-6,14,35,40),(40,44,40,48),(35,40,135,140)]
WD=[[0]*WW for _ in range(WH)]
T_DEEP,T_DEEP2,T_OCEAN,T_OCEAN2,T_SHALLOW,T_BEACH,T_GRASS,T_GRASS2,T_FOREST,T_FOREST2,T_JUNGLE,T_DESERT,T_DESERT2,T_MOUNT,T_MOUNT2,T_SNOW,T_TUNDRA,T_CITY,T_RWL,T_RWM,T_RWR,T_SEAICE,T_SAVANNA,T_HILLS,T_CITY2=range(25)
for y in range(WH):
    for x in range(WW):
        la,lo=cell_ll(x,y);h=H(x,y,77)
        if not LAND[y][x]:
            d=dist[y][x]
            if la>74 or la<-64:t=T_SEAICE if h<.75 or la<-66 else T_OCEAN
            elif d==1:t=T_SHALLOW
            elif d<=3:t=T_OCEAN if h<.6 else T_OCEAN2
            else:t=T_DEEP if h<.7 else T_DEEP2
        else:
            la0,lo0=la,lo;la=la+(H(x//3,y//3,5)-.5)*5;lo=lo+(H(x//3,y//3,6)-.5)*6
            coast=any(not LAND[y2][(x+dx)%WW] for dx,dy in((1,0),(-1,0),(0,1),(0,-1)) for y2 in [min(WH-1,max(0,y+dy))])
            if la<-60 or (inbox(la,lo,59,84,-74,-11)) or la>78:t=T_SNOW
            elif la>66:t=T_TUNDRA if h<.6 else T_SNOW
            elif la>58:t=T_TUNDRA if h<.4 else T_FOREST2
            elif any(inbox(la,lo,*b) for b in MNT) and h<.62:t=T_MOUNT if h<.4 else T_MOUNT2
            elif lo<-60 and la<12 and la>-56 and any(not LAND[y][(x-k)%WW] for k in range(1,4)) and h<.8:t=T_MOUNT if h<.5 else T_MOUNT2 # Andes
            elif lo>144 and la<-14 and la>-39 and any(not LAND[y][(x+k)%WW] for k in range(1,4)) and h<.5:t=T_HILLS
            elif any(inbox(la,lo,*b) for b in DES):t=T_DESERT if h<.6 else T_DESERT2
            elif abs(la)<10 and not inbox(la,lo,-12,12,30,52):t=T_JUNGLE
            elif inbox(la,lo,-25,16,-18,52) or inbox(la,lo,-20,-11,120,145) or inbox(la,lo,-25,-5,-65,-40):t=T_SAVANNA if h<.75 else T_GRASS
            elif la>45:t=T_FOREST if h<.65 else T_GRASS
            elif inbox(la,lo,30,45,-95,-70) or inbox(la,lo,22,42,100,125) or inbox(la,lo,-40,-25,-58,-45):t=T_FOREST if h<.5 else T_GRASS2
            else:t=T_GRASS if h<.55 else (T_GRASS2 if h<.85 else T_HILLS)
            la,lo=la0,lo0
            if coast and t in (T_GRASS,T_GRASS2,T_SAVANNA,T_JUNGLE,T_DESERT,T_FOREST) and abs(la)<55 and h<.5:t=T_BEACH
        WD[y][x]=t
def setc(x,y,t):WD[y][x%WW]=t;LAND[y][x%WW]=1
URB={0,1,3,4,6,9,10,11,15}
for i,l in enumerate(LM):
    x,y=ll_cell(l[2],l[3])
    if i in URB:
        for dx,dy in((0,0),(1,0),(0,1),(1,1),(-1,0)):setc(x+dx,y+dy,T_CITY if (dx+dy)%2==0 else T_CITY2)
    elif not LAND[y][x] and i!=13:setc(x,y,T_GRASS)
    if i==2:
        for dx in range(-2,3):setc(x+dx,y,T_DESERT2)
    if i in (8,12,14):setc(x,y,T_MOUNT)
AFC=[]
for a in AF:
    x,y=ll_cell(a[1],a[2])
    for dx in (-2,-1,0,1,2):
        for dy in (-1,1):
            if WD[y+dy][(x+dx)%WW] in (T_DEEP,T_DEEP2,T_OCEAN,T_OCEAN2,T_SHALLOW,T_SEAICE,T_MOUNT,T_MOUNT2):setc(x+dx,y+dy,T_GRASS if a[1]>-60 else T_SNOW)
    setc(x-1,y,T_RWL);setc(x,y,T_RWM);setc(x+1,y,T_RWR);AFC.append((x,y))
world=[WD[y][x] for y in range(WH) for x in range(WW)]
# ---------------------------------------------------------------- palette (BG 8bpp, entries 1..63)
WPAL=[(0,0,0),
 (24,72,152),(32,88,176),(48,112,200),(120,184,232),(40,104,192),(64,160,216),(112,200,232),(200,240,248),
 (232,208,144),(248,232,176),(200,168,104),(72,160,64),(104,192,80),(144,216,104),(40,112,56),
 (32,96,56),(56,136,64),(96,176,80),(24,64,40),(24,88,48),(40,120,56),(72,152,64),
 (232,192,120),(248,216,152),(200,152,88),(176,128,72),(120,104,96),(160,144,128),(200,188,176),(248,248,255),(80,68,72),
 (224,236,248),(248,252,255),(176,200,232),(136,168,216),(152,160,120),(184,184,144),(120,128,96),
 (200,96,80),(232,200,168),(120,120,136),(240,160,96),(96,96,104),(64,64,72),(248,248,248),(255,208,64),
 (200,192,96),(168,160,72),(120,136,56),(32,24,40)]
WPAL+=[(0,0,0)]*(64-len(WPAL))
emit("world_pal",[rgb15(c) for c in WPAL])
def mt(fn,seed):
    g=[[0]*16 for _ in range(16)]
    def px(x,y,c):
        if 0<=x<16 and 0<=y<16:g[y][x]=c
    fn(g,px,seed);return g
def fill(g,c):
    for r in g:
        for i in range(16):r[i]=c
def noisefill(g,a,b,seed,p=.25):
    for y in range(16):
        for x in range(16):g[y][x]=b if H(seed,x//2,y,3)<p else a
def ocean(g,px,s,deep=True):
    noisefill(g,2 if deep else 5,1 if deep else 2,s,.3)
    for i in range(3):
        x=int(H(s,i,1)*12);y=int(H(s,i,2)*14)+1
        for k in range(4):px(x+k,y-(1 if k in(1,2) else 0),3)
    px(int(H(s,9)*15),int(H(s,8)*15),4)
def shallow(g,px,s):
    noisefill(g,6,7,s,.35)
    for i in range(2):
        x=int(H(s,i,4)*12);y=int(H(s,i,5)*14)+1
        for k in range(4):px(x+k,y,8)
def beach(g,px,s):
    noisefill(g,9,10,s,.3)
    for i in range(4):px(int(H(s,i,6)*15),int(H(s,i,7)*15),11)
def grass(g,px,s,alt=False):
    noisefill(g,13,12,s,.3)
    for i in range(4):
        x=1+int(H(s,i,8)*13);y=2+int(H(s,i,9)*12);px(x,y,14);px(x+2,y,14);px(x+1,y-1,14);px(x,y+1,15);px(x+1,y+1,15);px(x+2,y+1,15)
    if alt:
        for i in range(2):x=2+int(H(s,i,10)*11);y=2+int(H(s,i,11)*11);px(x,y,10);px(x+1,y,46);px(x,y+1,10)
def trees(g,px,s,base,dark,mid,light,n=4):
    noisefill(g,base,dark,s,.2)
    pos=[(1,1),(8,0),(4,7),(11,8),(0,10),(9,12)][:n+1]
    for (x,y) in pos:
        for yy in range(6):
            for xx in range(6):
                d=(xx-2.5)**2+(yy-2.5)**2
                if d<7.5:px(x+xx,y+yy,mid if d<4.5 else 50)
        px(x+1,y+1,light);px(x+2,y+1,light);px(x+1,y+2,light);px(x+4,y+4,dark)
def desert(g,px,s):
    noisefill(g,23,24,s,.25)
    for i in range(2):
        y=3+int(H(s,i,12)*10);x0=int(H(s,i,13)*6)
        for k in range(9):px(x0+k,y-int(1.5*math.sin(k/2.6)),25);px(x0+k,y+1-int(1.5*math.sin(k/2.6)),26 if k%3==0 else 25)
def mount(g,px,s,snow=True):
    noisefill(g,28,27,s,.3)
    for (cx,by,hw) in [(5,13,5),(11,10,4)] if H(s,1)<.5 else [(8,14,7)]:
        for yy in range(hw*2):
            w=yy//2
            for xx in range(-w,w+1):px(cx+xx,by-hw*2+yy,29 if xx<0 else 27)
            px(cx-w-1,by-hw*2+yy,50);px(cx+w+1,by-hw*2+yy,31)
        if snow:
            for yy in range(3):
                for xx in range(-(yy//2+1)+1,yy//2+1):px(cx+xx,by-hw*2+yy,30)
def snow(g,px,s):
    noisefill(g,33,32,s,.3)
    for i in range(3):px(int(H(s,i,14)*15),int(H(s,i,15)*15),34)
def tundra(g,px,s):
    noisefill(g,37,36,s,.35)
    for i in range(4):x=int(H(s,i,16)*14);y=int(H(s,i,17)*14);px(x,y,38);px(x+1,y,32)
def city(g,px,s,alt=False):
    fill(g,41)
    for (x,y,w,h) in [(1,1,6,5),(9,1,6,4),(1,8,5,7),(8,7,7,8)]:
        c=42 if (x+y+(1 if alt else 0))%3==0 else 39
        for yy in range(h):
            for xx in range(w):px(x+xx,y+yy,40 if yy==h-1 else c)
        for xx in range(w):px(x+xx,y,10)
        px(x+w-1,y+h-1,50)
def runway(g,px,s,part):
    grass(g,px,s)
    for x in range(16):
        for y in range(5,12):px(x,y,43)
        px(x,4,50);px(x,12,50)
    for x in range(16):
        if part==1 and x%6<3:px(x,8,45)
    if part==0:
        for y in range(6,11):px(1,y,45);px(2,y,45 if y%2 else 43)
        px(0,5,46);px(0,11,46)
    if part==2:
        for y in range(6,11):px(13,y,45);px(14,y,45 if y%2 else 43)
        px(15,5,46);px(15,11,46)
def seaice(g,px,s):
    noisefill(g,34,32,s,.4)
    for i in range(3):
        x=int(H(s,i,18)*12);y=int(H(s,i,19)*12)
        for k in range(4):px(x+k,y,35);px(x,y+k,35)
def savanna(g,px,s):
    noisefill(g,47,48,s,.3)
    x=3+int(H(s,20)*8);y=3+int(H(s,21)*8)
    for xx in range(-3,4):px(x+xx,y,49);px(x+xx,y-1,49 if abs(xx)<3 else 47)
    px(x,y+1,26);px(x,y+2,26)
def hills(g,px,s):
    grass(g,px,s)
    for (x,y) in [(2,3),(9,9)]:
        for yy in range(5):
            for xx in range(7):
                d=(xx-3)**2/9+(yy-2.5)**2/6.5
                if d<1:px(x+xx,y+yy,14 if yy<2 else (12 if yy<4 else 15))
TILEFN=[lambda g,p,s:ocean(g,p,s),lambda g,p,s:ocean(g,p,s+5),lambda g,p,s:ocean(g,p,s,False),lambda g,p,s:ocean(g,p,s+7,False),shallow,beach,
 lambda g,p,s:grass(g,p,s),lambda g,p,s:grass(g,p,s,True),lambda g,p,s:trees(g,p,s,17,16,17,18),lambda g,p,s:trees(g,p,s+3,15,19,16,17,5),
 lambda g,p,s:trees(g,p,s,20,19,21,22,5),desert,lambda g,p,s:desert(g,p,s+9),mount,lambda g,p,s:mount(g,p,s+4,False),snow,tundra,
 lambda g,p,s:city(g,p,s),lambda g,p,s:runway(g,p,s,0),lambda g,p,s:runway(g,p,s,1),lambda g,p,s:runway(g,p,s,2),seaice,savanna,hills,lambda g,p,s:city(g,p,s,True)]
def tiles8(g):
    out=[]
    for ty in (0,1):
        for tx in (0,1):
            for y in range(8):
                for x4 in range(2):
                    v=0
                    for k in range(4):v|=(g[ty*8+y][tx*8+x4*4+k]&255)<<(8*k)
                    out.append(v)
    return out
wt=[]
for i,fn in enumerate(TILEFN):wt+=tiles8(mt(fn,1000+i*17))
emit("world_tiles",wt,"unsigned int");define("WT_N",len(TILEFN))
emit("world_map",world,"unsigned char")
# preview png
try:
    from PIL import Image
    im=Image.new('RGB',(WW*2,WH*2))
    avg=[]
    for i,fn in enumerate(TILEFN):
        g=mt(fn,1000+i*17);r=gg=b=0
        for row in g:
            for v in row:c=WPAL[v];r+=c[0];gg+=c[1];b+=c[2]
        avg.append((r//256,gg//256,b//256))
    for y in range(WH):
        for x in range(WW):
            c=avg[world[y*WW+x]]
            for dy in (0,1):
                for dx in (0,1):im.putpixel((x*2+dx,y*2+dy),c)
    for l in LM:
        x,y=ll_cell(l[2],l[3])
        for dx in range(-2,3):im.putpixel(((x*2+dx)%(WW*2),y*2),(255,40,40));im.putpixel((x*2,min(WH*2-1,max(0,y*2+dx))),(255,40,40))
    os.makedirs(os.path.join(ROOT,'shots'),exist_ok=True);im.save(os.path.join(ROOT,'shots','v12_world_map_overview.png'))
    full=Image.new('RGB',(16*8,16*4))
    for i,fn in enumerate(TILEFN):
        g=mt(fn,1000+i*17)
        for y in range(16):
            for x in range(16):full.putpixel(((i%8)*16+x,(i//8)*16+y),WPAL[g[y][x]])
    full.resize((16*8*3,16*4*3),Image.NEAREST).save(os.path.join(ROOT,'build','world_tiles_preview.png'))
except Exception as e:print("world preview skipped",e)
# overview bitmap for the SELECT world map (240x120, 15bpp)
ov=[]
for y in range(120):
    for x in range(240):
        t=world[int(y*WH/120)*WW+int(x*WW/240)]
        ov.append(rgb15(WPAL[[2,2,5,5,6,9,13,13,17,16,21,23,24,28,28,33,37,40,43,43,43,34,47,14,40][t]]))
emit("world_ov",ov)
# ---------------------------------------------------------------- OBJ: aircraft + icons
def outl(g):
    h=len(g);w=len(g[0]);o=[r[:] for r in g]
    for y in range(h):
        for x in range(w):
            if g[y][x]==0 and any(0<=x+dx<w and 0<=y+dy<h and g[y+dy][x+dx] for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=1
    return o
wo=[]
X15=pg(dict(k='jet',span=14,ch=6,wy=18,sw=3,tp=.5,fl=30,fw=4,fy=1,nose='jet',ts=10,cy=6))
SR71=pg(dict(k='jet',span=20,ch=12,wy=13,sw=11,tp=.15,fl=31,fw=4,fy=0,nose='jet',eng=[6],jeteng=1,twin=1,ts=8,canard=1,cy=5))
CONC=pg(dict(k='jet',span=22,ch=17,wy=10,sw=15,tp=.08,fl=31,fw=3,fy=0,nose='jet',ts=3,eng=[5],jeteng=1,cy=3))
P51=ww2plane(WW2_PLANES[0])
B52=pg(dict(k='hb',span=32,ch=5,wy=11,sw=7,tp=.5,fl=30,fw=4,fy=1,nose='jet',eng=[4,8],jeteng=1,ts=12))
for g in (X15,SR71,CONC,P51,B52):wo+=tiles4(g,32,32)
def icon(fn):
    c=Cv(16,16);fn(c);g=c.grid(0);return tiles4(outl(g),16,16)
I=[]
I.append(lambda c:(c.r(4,13,11,15,6),c.r(5,11,10,13,5),c.e(5,5,10,12,2),c.e(6,6,8,10,3),c.e(6,2,9,5,2),c.l(10,6,12,2,3),c.px(12,1,7),c.px(12,0,10),c.l(5,2,10,2,3),c.px(7,1,3)))
I.append(lambda c:(c.r(0,8,15,9,9),c.r(3,1,4,13,9),c.r(11,1,12,13,9),c.l(4,2,8,7,10),c.l(8,7,11,2,10),c.l(0,6,3,2,10),c.l(12,2,15,6,10),c.r(0,12,15,15,14),c.l(0,13,15,13,13)))
I.append(lambda c:(c.e(0,1,15,14,10),c.e(1,3,14,12,8),c.e(3,5,12,10,11),c.l(2,8,13,8,9),c.l(4,7,11,7,10),c.l(3,9,12,9,13),c.l(5,9,10,9,14)))
I.append(lambda c:(c.p([(7,0),(8,0),(13,15),(10,15),(8,9),(7,9),(5,15),(2,15)],11),c.r(4,10,11,11,8),c.r(6,5,9,6,8),c.e(6,12,9,15,0),c.px(7,0,7),c.px(8,0,7)))
I.append(lambda c:(c.r(5,3,10,15,7),c.r(6,4,9,14,8),c.e(5,5,10,9,4),c.px(7,7,6),c.l(7,6,7,7,6),c.p([(5,3),(7,0),(8,0),(10,3)],6),c.r(5,10,10,10,8)))
I.append(lambda c:(c.p([(0,14),(6,3),(12,14)],7),c.p([(6,3),(12,14),(7,14)],8),c.p([(9,14),(13,7),(16,14)],7),c.p([(13,7),(16,14),(13,14)],8),c.l(0,15,15,15,8)))
I.append(lambda c:(c.r(3,9,12,14,4),c.e(4,3,11,10,4),c.e(5,4,8,7,15),c.r(1,4,2,14,4),c.r(13,4,14,14,4),c.px(7,1,7),c.px(7,2,7),c.px(8,2,7),c.r(7,11,8,14,6)))
I.append(lambda c:(c.l(0,12,5,6,5,3),c.l(5,6,10,9,5,3),c.l(10,9,15,3,5,3),c.r(3,6,6,9,6),c.r(10,4,13,7,6),c.l(0,13,5,7,6),c.l(5,7,10,10,6),[c.px(x,int(12-x*1.2) if x<5 else (6+(x-5)//2 if x<10 else 9-(x-10)),4) for x in range(0,16,3)]))
I.append(lambda c:(c.p([(0,15),(7,2),(8,2),(15,15)],14),c.p([(8,2),(15,15),(9,15)],13),c.p([(5,7),(7,2),(8,2),(10,7),(9,6),(8,8),(7,6),(6,8)],4)))
I.append(lambda c:(c.r(0,12,15,15,14),c.r(1,11,14,12,5),c.p([(2,11),(6,3),(7,11)],4),c.p([(6,11),(10,4),(11,11)],4),c.p([(10,11),(13,6),(14,11)],4),c.l(6,3,6,11,5),c.l(10,4,10,11,5)))
I.append(lambda c:(c.p([(0,15),(7,8),(15,15)],12),c.r(7,3,8,13,4),c.r(2,5,13,6,4),c.e(6,0,9,3,4),c.px(7,14,5)))
I.append(lambda c:(c.r(0,10,15,15,9),c.l(0,10,15,10,10),c.e(2,4,6,9,12),c.e(9,3,14,9,7),c.r(3,8,5,11,4),c.r(10,8,13,11,4),c.px(4,3,7),c.px(11,2,7),c.e(6,6,9,10,9),c.px(7,5,7)))
I.append(lambda c:(c.p([(0,15),(5,5),(10,5),(15,15)],11),c.p([(5,5),(10,5),(15,15),(9,15)],8),c.p([(4,7),(5,5),(10,5),(11,7),(8,8),(7,6)],4)))
I.append(lambda c:(c.e(0,4,15,15,4),c.e(2,6,13,13,15),c.r(4,7,10,11,10),c.r(5,8,9,10,9),c.l(12,2,12,9,6),c.r(12,2,15,4,9)))
I.append(lambda c:(c.p([(0,15),(8,1),(16,15)],6),c.p([(8,1),(16,15),(9,15)],5),c.p([(5,6),(8,1),(11,6),(9,5),(8,7),(7,5)],4),c.l(8,0,8,1,9),c.px(9,0,9)))
I.append(lambda c:(c.e(0,2,15,13,7),c.e(3,4,12,11,8),c.e(4,5,11,10,11),[c.px(x,2,8) for x in range(2,14,2)],[c.px(x,13,8) for x in range(2,14,2)],c.l(1,7,1,8,8)))
I.append(lambda c:(c.p([(0,15),(5,3),(9,1),(15,15)],12),c.p([(9,1),(15,15),(10,15)],2),[c.r(3,y,12,y,6) for y in (8,10,12)],c.r(6,5,9,7,5)))
I.append(lambda c:(c.e(0,5,15,14,10),c.e(1,6,14,12,9),c.l(3,7,12,7,10),c.l(2,11,14,11,11)))
I.append(lambda c:(c.r(0,0,15,15,12),c.r(3,0,12,15,13),c.r(4,4,11,15,14),c.l(3,3,12,3,4),c.l(3,4,12,4,15),c.r(2,13,13,15,4),[c.px(x,8,4) for x in (5,8,10)]))
for fn in I:wo+=icon(fn)
ring=Cv(32,32);ring.e(1,1,30,30,7);ring.e(4,4,27,27,0);ring.e(2,2,29,29,10) if False else None
g=[[0]*32 for _ in range(32)]
for y in range(32):
    for x in range(32):
        d=math.hypot(x-15.5,y-15.5)
        if 11.5<d<15.5:g[y][x]=7 if d<13.5 else 8
        if 12<d<13 and y<14:g[y][x]=15
wo+=tiles4(outl(g),32,32)
ar=[[0]*16 for _ in range(16)]
for y in range(2,14):
    w=(y-2)*5//12
    for x in range(8-w,8+w+1):ar[y][x]=7 if x<=8 else 8
wo+=tiles4(outl(ar),16,16)
fl=[[0]*8 for _ in range(8)]
for y in range(8):
    for x in range(8):
        d=abs(x-3.5)
        if d<3.5-y*0.4:fl[y][x]=15 if d<1 else (10 if d<2 else 9)
wo+=tiles4(fl,8,8)
st=[[0]*8 for _ in range(8)];st[3][3]=15;st[3][4]=4;st[4][3]=4;st[2][3]=4;st[3][2]=4
wo+=tiles4(st,8,8)
emit("world_obj",wo,"unsigned int");define("WO_X15",0);define("WO_B52",64);define("WO_ICON",80);define("WO_RING",80+4*len(I));define("WO_ARROW",80+4*len(I)+16);define("WO_FLAME",80+4*len(I)+20);define("WO_STAR",80+4*len(I)+21)
def jp(body):  # plane palette with given 2/3/4 shades + accent
    d,m,l,acc,cano=body
    return pal16([(0,0,0),(20,16,28),d,m,l,(150,150,160),(200,180,150),(60,60,68),(110,110,120),acc,cano,(240,240,248),(70,70,80),(90,90,100),(110,110,124),(255,190,70)])
wp=[]
wp+=jp(((28,28,36),(52,52,64),(92,92,108),(245,245,250),(120,200,240)))   # X-15 (black Inconel, white USAF)
wp+=jp(((20,20,36),(36,36,58),(66,66,96),(200,50,50),(120,190,230)))      # SR-71
wp+=jp(((176,180,196),(226,228,238),(252,252,255),(40,60,160),(90,150,210)))  # Concorde
wp+=jp(((138,142,150),(188,192,200),(230,234,240),(230,200,40),(110,190,230)))# P-51D (bare aluminium)
wp+=jp(((106,110,118),(156,160,168),(208,212,220),(245,245,250),(110,180,220)))# B-52
wp+=[rgb15((14,22,38))]*16                                                      # shadow
wp+=pal16([(0,0,0),(30,20,40),(70,150,120),(130,210,170),(244,244,244),(196,196,206),(124,124,144),(232,192,92),(186,138,60),(206,64,52),(250,144,64),(122,80,50),(76,152,64),(112,190,240),(60,92,172),(255,250,204)])
emit("world_objpal",wp)
# ---------------------------------------------------------------- BG1: clouds (rows 0..15 repeat) + vignette
cl=[[0]*256 for _ in range(64)]
for i in range(5):
    cx=H(i,1)*256;cy=H(i,2)*64;rx=10+H(i,3)*26;ry=6+H(i,4)*12
    for k in range(5):
        ox=cx+(H(i,k,5)-.5)*rx*1.4;oy=cy+(H(i,k,6)-.5)*ry;r2=rx*(0.4+H(i,k,7)*0.4)
        for y in range(int(oy-r2),int(oy+r2)+1):
            for x in range(int(ox-r2*1.4),int(ox+r2*1.4)+1):
                d=((x-ox)/1.4)**2+(y-oy)**2
                if d<r2*r2:
                    v=1 if d<(r2*.55)**2 else (2 if d<(r2*.85)**2 else 3)
                    yy=y%64;xx=x%256
                    if cl[yy][xx]==0 or cl[yy][xx]>v:cl[yy][xx]=v
vg=[[0]*256 for _ in range(160)]
for y in range(160):
    for x in range(240):
        d=math.hypot((x-119.5)/140,(y-79.5)/104)
        if d>1.0:vg[y][x]=10 if (x%8==3 and y%8==4 and H(x//8,y//8,99)<.3) else (5 if d>1.12 else 6)
        elif d>0.975:vg[y][x]=8
        elif d>0.95:vg[y][x]=7
tset={};tl=[];
def tid(g,x0,y0):
    rows=tuple(tuple(g[y0+y][x0:x0+8]) for y in range(8))
    if rows not in tset:
        tset[rows]=len(tset)
        for r in rows:
            v=0
            for x in range(8):v|=(r[x]&15)<<(4*x)
            tl.append(v)
    return tset[rows]
tid([[0]*8]*8,0,0)
cmap=[]
for ty in range(32):
    for tx in range(32):cmap.append(tid(cl,tx*8,(ty%8)*8)|(9<<12))
vmap=[]
for ty in range(32):
    for tx in range(32):
        if ty<20 and tx<30:
            qx=tx if tx<15 else 29-tx;qy=ty if ty<10 else 19-ty
            vmap.append(tid(vg,qx*8,qy*8)|((tx>=15)<<10)|((ty>=10)<<11)|(9<<12))
        else:vmap.append(9<<12)
assert len(tset)<=256,len(tset)
emit("world_bg1t",tl,"unsigned int");emit("world_cloudmap",cmap);emit("world_vigmap",vmap)
emit("world_bg1pal",pal16([(0,0,0),(250,250,255),(222,230,244),(188,202,226),(0,0,0),(6,6,22),(16,20,52),(110,170,250),(160,210,255),(60,100,190),(255,255,255)]))
# ---------------------------------------------------------------- C tables
def px16(lat,lon):x,y=ll_cell(lat,lon);return x*16+8,y*16+8
o=["// generated by tools/gen_world.py","typedef struct{const char*n,*place,*fact;u16 x,y;u8 icon;}WLm;","typedef struct{const char*n;u16 x,y;}WAf;",
   "typedef struct{u16 x,y;u16 alt;}WRing;","typedef struct{const char*n,*desc;WRing r[8];}WCourse;"]
o.append("#define NLM %d\n#define NAF %d\n#define NCOURSE %d\n#define WWID %d\n#define WHGT %d"%(len(LM),len(AF),len(RINGS),WW,WH))
o.append("static const WLm WLM[NLM]={"+",".join('{"%s","%s","%s",%d,%d,%d}'%(l[0],l[1],l[5],*px16(l[2],l[3]),l[4]) for l in LM)+"};")
o.append("static const WAf WAF[NAF]={"+",".join('{"%s",%d,%d}'%(a[0],AFC[i][0]*16+8,AFC[i][1]*16+8) for i,a in enumerate(AF))+"};")
o.append("static const WCourse WCO[NCOURSE]={"+",".join('{"%s","%s",{%s}}'%(c[0],c[1],",".join("{%d,%d,%d}"%(*px16(r[0],r[1]),r[2]//10) for r in c[2])) for c in RINGS)+"};")
open(os.path.join(ROOT,'src','worlddb.inc'),'w').write("\n".join(o)+"\n")
print("world: %d landmarks, %d airfields, %d ring courses, %d terrain metatiles, bg1 tiles %d"%(len(LM),len(AF),len(RINGS),len(TILEFN),len(tset)))
