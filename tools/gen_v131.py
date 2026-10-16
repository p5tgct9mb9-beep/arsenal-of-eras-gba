#!/usr/bin/env python3
# v1.3.1 assets -> src/v131db.inc : orbital main-menu Earth (sphere LUT + texture from tools/world_mask.bin),
# category icons, KC astronaut helmet, space guide portrait, Overflight map + aircraft, frontier sprites.
# All art is original pixel art drawn procedurally here.
import os,math,random
ROOT=os.path.join(os.path.dirname(os.path.abspath(__file__)),'..')
OUT=[]
def emit(name,vals,typ="unsigned char",per=32):
    OUT.append("static const %s %s[%d]={"%(typ,name,len(vals)))
    for i in range(0,len(vals),per):OUT.append(",".join(str(v) for v in vals[i:i+per])+",")
    OUT.append("};")
def define(n,v):OUT.append("#define %s %s"%(n,v))
def rgb(r,g,b):return (r>>3)|((g>>3)<<5)|((b>>3)<<10)
# ---------------------------------------------------------------- Earth
WW,WH=512,256
bits=open(os.path.join(ROOT,'tools','world_mask.bin'),'rb').read()
def land(x,y):return (bits[((y%WH)*WW+(x%WW))>>3]>>(x&7))&1
random.seed(131)
# texture 512x128 (2 mask rows per texel row, lat 84N..84S); types: 0 ocean 1 shallow 2 forest 3 grass 4 desert 5 ice 6 city
TEX=[]
def noise(x,y,s):return (math.sin(x*0.071*s+y*0.13)+math.sin(x*0.023*s-y*0.051+1.7)+math.sin((x+y)*0.041+0.4))/3
for ty in range(128):
    lat=84-(ty+.5)*168/128
    for tx in range(512):
        L=land(tx,ty*2)
        if not L:
            sh=any(land(tx+dx,ty*2+dy) for dx in(-2,2) for dy in(-2,2))
            TEX.append(1 if sh else 0);continue
        if abs(lat)>62:t=5
        else:
            n=noise(tx,ty,1.0)
            dry=(1-abs(abs(lat)-24)/14)  # desert belts ~ 15-33 deg
            if dry+n*0.6>0.45:t=4
            elif abs(lat)<12 or n>0.1:t=2
            else:t=3
            if random.random()<0.035 and abs(lat)<60:t=6
        TEX.append(t)
emit("ea_tex",TEX)
# cloud density 512x128: 0 none 1 thin 2 thick (bands + swirls)
CL=[]
for ty in range(128):
    lat=84-(ty+.5)*168/128
    for tx in range(512):
        v=(math.sin(tx*0.19+math.sin(ty*0.37)*3)+math.sin(tx*0.071-ty*0.29+2)+math.sin(ty*0.53+tx*0.043)*0.8+math.sin(tx*0.31+ty*0.11)*0.6)/3.4
        band=0.18 if abs(lat)<8 else (0.12 if 40<abs(lat)<62 else -0.08)
        v+=band+random.uniform(-.1,.1)
        CL.append(2 if v>0.50 else 1 if v>0.36 else 0)
emit("ea_cld",CL)
# sphere LUT for the menu Earth: centre (CX,CY) radius R, tilt about X so the limb shows ~40N
CX,CY,R=172,392,300;TILT=math.radians(50)
S=(0.9,0.05,0.45);n=math.sqrt(sum(s*s for s in S));S=tuple(s/n for s in S)
H=(S[0],S[1],S[2]+1);n=math.sqrt(sum(h*h for h in H));H=tuple(h/n for h in H)
rows=[];LUT=[];TOP=[]
for x in range(240):
    dx=(x-CX)/R;
    TOP.append(max(0,int(math.ceil(CY-R*math.sqrt(max(0,1-dx*dx))))) if abs(dx)<1 else 160)
for y in range(160):
    Y=(CY-y)/R
    if Y>1:rows.append((0,0,len(LUT)//2));continue
    w=math.sqrt(max(0,1-Y*Y));x0=max(0,int(math.ceil(CX-R*w)));x1=min(239,int(math.floor(CX+R*w)))
    if x0>x1:rows.append((0,0,len(LUT)//2));continue
    rows.append((x0,x1-x0+1,len(LUT)//2))
    for x in range(x0,x1+1):
        X=(x-CX)/R;Z=math.sqrt(max(0,1-X*X-Y*Y))
        Yt=Y*math.cos(TILT)-Z*math.sin(TILT);Zt=Y*math.sin(TILT)+Z*math.cos(TILT)
        lat=math.degrees(math.asin(max(-1,min(1,Yt))));lon=math.degrees(math.atan2(X,Zt))
        u=int((lon+180)/360*512)%512;v=int((84-lat)/168*128);v=max(0,min(127,v))
        lam=X*S[0]+Y*S[1]+Z*S[2]
        lt=0 if lam<-0.04 else 1 if lam<0.04 else min(11,int(2+lam*11))
        spec=(X*H[0]+Y*H[1]+Z*H[2])
        if spec>0.985 and lam>0:lt=12+min(3,int((spec-0.985)/0.004))
        hz=3 if Z<0.08 else 2 if Z<0.18 else 1 if Z<0.32 else 0
        LUT.append(u|(v<<9));LUT.append(lt|(hz<<4))
emit("ea_lut",[LUT[i] for i in range(0,len(LUT),2)],"unsigned short")
emit("ea_sh",[LUT[i] for i in range(1,len(LUT),2)])
emit("ea_row",[v for r in rows for v in r],"unsigned short")
emit("ea_top",TOP)
define("EA_CX",CX);define("EA_CY",CY);define("EA_R",R)
# ---------------------------------------------------------------- icons 12x12 (palette index art, 0 = clear)
ICP=[rgb(0,0,0),rgb(20,24,40),rgb(236,196,92),rgb(255,240,170),rgb(120,200,255),rgb(220,230,240),rgb(140,150,160),rgb(90,120,70),rgb(150,170,100),rgb(230,90,60),rgb(255,180,80),rgb(60,70,90),rgb(180,120,80),rgb(250,250,250),rgb(110,80,60),rgb(70,160,220)]
ICONS={
'helmet':["............","....1111....","..11888811..",".1888888881.",".1877777781.","188888888881","177777777771","111111111111","..1......1..","..1......1..","...11..11...","............"],
'tank':["............","............",".....1111...","....166661..","11111666661.","166666666666",".1666666661.","111111111111","1d1d1d1d1d11",".1b1b1b1b11.","..11111111..","............"],
'chute':["...111111...",".1555555551.","15d5d5d5d5d1","155555555551","1.1..1..1..1",".1..1..1..1.","..1..1.1.1..","...1.1.11...","....1111....","....1771....","....1771....",".....11....."],
'cross':[".....99.....","...99..99...","..9...9..9..",".9....9...9.",".9....9...9.","9999999999.9",".9....9...9.",".9....9...9.","..9...9..9..","...99..99...",".....99.....","............"],
'rocket':[".....dd.....","....dddd....","....d44d....","....dddd....","....d99d....","...dddddd...","..1dddddd1..","..1d.dd.d1..",".11..aa..11.",".....aa.....","....a33a....",".....aa....."],
'pack':["....1111....","...188881...","..18888881..",".1877777781.",".1888888881.",".1871111781.",".1871cc1781.",".1871111781.",".1888888881.",".1777777771.","..11111111..","............"],
'gear':["....1661....",".1.166661.1.","161666666161",".1666116661.","1666111166661"[:12],"666611116666","666611116666","1666111166661"[:12],".1666116661.","161666666161",".1.166661.1.","....1661...."],
'paw':["............","..33....33..",".3223..3223.",".3223..3223.","..33.33.33..","....3223....",".33.3223.33.","3223.33.3223","3223333.3223",".33322223.3."[:12],"...322223...","....3333...."],
}
HEX="0123456789abcdef"
iv=[]
for k in ['helmet','tank','chute','cross','rocket','pack','gear','paw']:
    g=ICONS[k];assert len(g)==12,k
    for r in g:
        r=(r+"............")[:12]
        for ch in r:iv.append(0 if ch=='.' else HEX.index(ch))
emit("mi_icon",iv);emit("mi_pal",ICP,"unsigned short")
# KC astronaut: 20x20 dog head in a bubble helmet (original art)
KCA=["......44444444......","....44........44....","...4..............4.","..4.....1111.......4",".4....11cccc11.....4",".4...1cccccccc1.d..4","4...1cc1cccc1cc1...4"[:20],"4..1ccc3cccc3ccc1..4"[:20],"4..1cccccccccccc1..4","4...1cc111111cc1...4"[:20],"4...1ccc1ee1ccc1...4"[:20],"4....1cc1111cc1....4"[:20],".4....11cccc11.....4",".4.....1111111....4.","..4...55555555...4..","...46655666655664...","....6655555556666...","...66666666666666...","...6.............6..","...................."]
kv=[]
for r in KCA:
    r=(r+"."*20)[:20]
    for ch in r:kv.append(0 if ch=='.' else HEX.index(ch))
KCP=[0,rgb(40,28,20),0,rgb(250,250,250),rgb(150,220,255),rgb(220,230,240),rgb(120,130,150),0,0,0,0,0,rgb(200,140,70),rgb(255,255,255),rgb(230,120,140),0]
emit("kc_astro",kv);emit("kc_astro_pal",KCP,"unsigned short")
# ---------------------------------------------------------------- earth colour table: [type 0-8][light 0-15][haze 0-3] (+512 = under HUD panel)
BASE=[(18,52,120),(30,90,150),(46,96,44),(110,130,62),(196,164,104),(232,236,244),(80,110,60),(235,238,245),(170,180,200)] # 7 cloud 8 thin cloud
NIGHT=[(2,4,14),(3,6,18),(4,6,8),(5,6,8),(7,6,6),(14,16,24),(255,200,110),(18,20,30),(10,12,20)]
HAZE=(110,170,255);PANEL=(8,18,48)
def ecol(t,lt,hz):
    b=BASE[t]
    if lt==0:c=NIGHT[t]
    elif lt>=12:
        g=(lt-11)/4;c=b if t>1 else tuple(int(b[i]+(255-b[i])*g*(0.9 if i<2 else 0.6)) for i in range(3))
        if t<=1:c=tuple(min(255,int(v*1.0)) for v in c)
    else:
        f=0.30+lt*0.072;c=tuple(min(255,int(v*f)) for v in b)
        if lt==1:c=tuple(int(c[i]*0.5+(60,40,50)[i]*0.5) for i in range(3))
    if lt and hz:
        a=hz*0.22;c=tuple(int(c[i]*(1-a)+HAZE[i]*a*(0.35+0.65*min(1,lt/6))) for i in range(3))
    return c
EC=[]
for under in (0,1):
    for t in range(9):
        for lt in range(16):
            for hz in range(4):
                c=ecol(t,lt,hz)
                if under:c=tuple(int(c[i]*0.38+PANEL[i]*0.62) for i in range(3))
                EC.append(c)
emit("ea_col",[rgb(*c) for c in EC],"unsigned short")
define("EA_NT",9)
def preview(path,ls=0,cs=0):
    from PIL import Image
    im=Image.new('RGB',(240,160),(2,3,10));px=im.load()
    for y in range(160):
        x0,n,o=rows[y]
        for i in range(n):
            u=LUT[(o+i)*2]&511;v=LUT[(o+i)*2]>>9;sh=LUT[(o+i)*2+1]
            t=TEX[v*512+((u+ls)&511)];c=CL[v*512+((u+cs)&511)]
            if c==2:t=7
            elif c==1 and t!=6:t=8 if (x0+i+y)&1 else t
            if t==6 and (sh&15)!=0:t=2
            px[x0+i,y]=EC[(t*16+(sh&15))*4+(sh>>4)]
    im.save(path)

# ---------------------------------------------------------------- space guide portrait (48x48 x2 frames, bit15 = clear)
# Original pixel art: a 1960s engineer in a civilian charcoal suit, white shirt, thin tie. No uniforms, no insignia.
from PIL import Image,ImageDraw
def guide_portrait(talk):
    W=48;OUT=(16,12,18);im=Image.new('RGBA',(W,W),(0,0,0,0));d=ImageDraw.Draw(im)
    SK0,SK1=(232,186,150),(200,150,118);HR0,HR1=(84,62,44),(58,42,30);SU0,SU1=(62,66,76),(44,46,54)
    d.polygon([(2,47),(6,37),(16,32),(32,32),(42,37),(46,47)],fill=SU0,outline=OUT)        # suit
    d.polygon([(26,33),(32,32),(42,37),(46,47),(28,47)],fill=SU1)
    d.polygon([(18,32),(24,44),(30,32)],fill=(240,240,236),outline=OUT)                   # shirt V
    d.polygon([(23,35),(25,35),(26,44),(24,46),(22,44)],fill=(46,58,96),outline=OUT)        # thin tie
    d.polygon([(16,32),(22,42),(19,47),(12,40)],fill=SU1,outline=OUT);d.polygon([(32,32),(26,42),(29,47),(36,40)],fill=SU1,outline=OUT) # lapels
    d.polygon([(33,40),(37,40),(37,41),(33,41)],fill=(236,236,232))                          # pocket square
    d.rectangle([19,26,29,33],fill=SK1,outline=OUT)                                         # neck
    face=[(13,7),(35,7),(37,22),(34,29),(27,33),(21,33),(14,29),(11,22)]
    d.polygon(face,fill=SK0,outline=OUT);d.polygon([(28,10),(36,12),(37,22),(34,29),(27,33),(29,24)],fill=SK1);d.line(face+[face[0]],fill=OUT)
    d.ellipse([9,17,13,24],fill=SK1,outline=OUT);d.ellipse([35,17,39,24],fill=SK1,outline=OUT)  # ears
    # swept-back hair, side part, slight receding temples
    d.polygon([(11,15),(12,6),(18,2),(28,1),(35,4),(38,10),(37,16),(34,11),(28,9),(20,9),(15,11),(13,16)],fill=HR0,outline=OUT)
    d.line([(17,5),(30,3)],fill=(120,92,66));d.line([(16,8),(33,6)],fill=HR1);d.line([(20,9),(15,5)],fill=HR1)
    for ex in (18,30):
        d.rectangle([ex-3,19,ex+2,21],fill=(250,250,248));d.rectangle([ex-1,19,ex,21],fill=(70,90,120));d.point((ex-1,19),fill=(255,255,255))
        d.line([(ex-4,18),(ex+2,18)],fill=OUT);d.line([(ex-4,16),(ex+2,15)],fill=HR1,width=1)     # heavy brows
    d.line([(24,20),(25,25),(23,26)],fill=(170,120,96))                                     # nose
    d.line([(15,24),(16,27)],fill=(212,166,132));d.line([(33,24),(32,27)],fill=(186,138,108))  # cheek lines (smile)
    if talk:d.polygon([(20,28),(28,28),(26,31),(22,31)],fill=(120,40,40),outline=OUT);d.line([(21,28),(27,28)],fill=(250,250,250))
    else:d.line([(20,28),(24,29),(28,28)],fill=OUT);d.point((19,27),fill=OUT);d.point((29,27),fill=OUT)
    return im
GP=[]
for t in (0,1):
    im=guide_portrait(t);im.resize((144,144),Image.NEAREST).save('/tmp/guide_%d.png'%t)
    for y in range(48):
        for x in range(48):
            r,g,b,a=im.getpixel((x,y));GP.append(0x8000 if a<128 else rgb(r,g,b))
emit("guide_px",GP,"unsigned short")
#@@PARTS@@
if __name__=="__main__":
    preview('/tmp/earth_prev.png')
    open(os.path.join(ROOT,'src','v131db.inc'),'w').write("// generated by tools/gen_v131.py - do not edit\n"+"\n".join(OUT)+"\n")
    print("v131db: %d lines, earth px %d"%(len(OUT),len(LUT)//2))
