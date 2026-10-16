#!/usr/bin/env python3
# v1.3.2 RACING assets: 4 Mode-7 tracks (1024x1024 px, 128x128 8bpp tiles quantised to <=256 tiles),
# per-tile surface maps, waypoints + curvature speed hints, sky panoramas (4bpp BG1), car sprites (4bpp OBJ),
# car side art for the Mode 3 car select. Deterministic (fixed seeds). Output: src/racedb.inc
import numpy as np, math, os, random
from PIL import Image, ImageDraw
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT=[]
def emit(name,vals,typ="unsigned char",per=32):
    s=",".join(str(int(v)) for v in vals)
    OUT.append(f"static const {typ} {name}[{len(vals)}] __attribute__((aligned(4)))={{{s}}};")
def rgb15(c):r,g,b=c[:3];return (r>>3)|((g>>3)<<5)|((b>>3)<<10)
S=1024
# surfaces
SF_ROAD,SF_CURB,SF_OFF,SF_WALL,SF_PAD,SF_RAMP,SF_WATER=0,1,2,3,4,5,6
def catmull(pts,n=40):
    P=np.array(pts,float);N=len(P);out=[]
    for i in range(N):
        p0,p1,p2,p3=P[(i-1)%N],P[i],P[(i+1)%N],P[(i+2)%N]
        for t in np.linspace(0,1,n,endpoint=False):
            t2,t3=t*t,t*t*t
            out.append(0.5*((2*p1)+(-p0+p2)*t+(2*p0-5*p1+4*p2-p3)*t2+(-p0+3*p1-3*p2+p3)*t3))
    return np.array(out)
def resample(C,step):
    C=np.vstack([C,C[:1]]);d=np.sqrt(((C[1:]-C[:-1])**2).sum(1));s=np.concatenate([[0],np.cumsum(d)]);L=s[-1]
    ts=np.arange(0,L,step);x=np.interp(ts,s,C[:,0]);y=np.interp(ts,s,C[:,1]);return np.stack([x,y],1),L
def field(C,R):
    """distance to centreline, arclength param, signed side, for every pixel within R"""
    D=np.full((S,S),1e9,np.float32);T=np.zeros((S,S),np.float32);SD=np.zeros((S,S),np.float32)
    n=len(C);tang=np.roll(C,-1,0)-np.roll(C,1,0);tang/=np.linalg.norm(tang,axis=1)[:,None]
    r=int(R)+1;yy,xx=np.mgrid[-r:r+1,-r:r+1]
    for i,(cx,cy) in enumerate(C):
        x0,y0=int(cx),int(cy);X=xx+x0;Y=yy+y0;ok=(X>=0)&(X<S)&(Y>=0)&(Y<S)
        dx=X-cx;dy=Y-cy;d=np.sqrt(dx*dx+dy*dy);sd=dx*tang[i,1]-dy*tang[i,0] # >0 = right of travel
        Xo,Yo,do,so=X[ok],Y[ok],d[ok],sd[ok];cur=D[Yo,Xo];m=do<cur
        D[Yo[m],Xo[m]]=do[m];T[Yo[m],Xo[m]]=i;SD[Yo[m],Xo[m]]=so[m]
    return D,T,SD
def lerp(a,b,t):return tuple(int(a[k]+(b[k]-a[k])*t) for k in range(3))
def tex(base,alt,pat,yy,xx):
    """8x8-periodic pattern texture: pat is 8 strings of '.'/'x'"""
    m=np.array([[c=='x' for c in row] for row in pat]);sel=m[yy%8,xx%8]
    out=np.zeros(yy.shape+(3,),np.uint8);out[:]=base;out[sel]=alt;return out
YY,XX=np.mgrid[0:S,0:S]
P_ASPH=["x.......","........","....x...","........","..x.....","......x.","........","...x...."]
P_GRASS=["..x.....","x.....x.","....x...",".x......","......x.","..x...x.","x.......","....x..."]
P_SAND=["........",".x......","......x.","........","...x....","........","x....x..","........"]
P_DOT=["x...x...","........","..x...x.","........","x...x...","........","..x...x.","........"]
TRACKS=[]
def track(**k):TRACKS.append(k)
# ---- track layouts (clockwise, 1024 px map, wraps). w = road width.
track(name="GIGA TEXAS",w=72,laps=3,grav=256,
 pts=[(150,800),(150,330),(230,190),(420,150),(560,250),(600,420),(730,450),(860,340),(905,480),(860,660),(700,720),(620,870),(420,900),(240,880)],
 ground=((168,170,96),(150,152,84),P_GRASS),road=((92,94,104),(104,106,116)),curb=((230,40,40),(245,245,245)),edge=(250,250,250),
 center="dash",ramps=[],pads=[0.62],water=None,sky="texas")
track(name="PACIFIC COAST HWY",w=64,laps=3,grav=256,
 pts=[(250,880),(215,720),(270,600),(220,470),(265,330),(230,200),(340,115),(490,165),(560,300),(700,260),(830,170),(905,320),(820,470),(880,620),(800,780),(640,860),(470,815),(370,905)],
 ground=((206,186,128),(190,170,112),P_SAND),road=((70,72,82),(82,84,94)),curb=((250,250,250),(70,72,82)),edge=(250,250,250),
 center="yellow",ramps=[],pads=[0.30],water="west",sky="pch")
track(name="STARBASE BOCA CHICA",w=68,laps=3,grav=256,
 pts=[(190,860),(170,560),(230,330),(400,190),(640,170),(820,250),(860,440),(760,590),(600,560),(500,640),(560,790),(720,820),(800,900),(560,935),(330,930)],
 ground=((222,204,150),(204,186,132),P_SAND),road=((150,150,146),(166,166,160)),curb=((40,40,44),(240,200,40)),edge=(240,200,40),
 center="none",ramps=[0.16],pads=[0.70],water="east",sky="starbase")
track(name="MARS COLONY",w=66,laps=3,grav=97,
 pts=[(180,500),(210,270),(380,160),(560,240),(700,140),(880,230),(870,450),(700,520),(820,700),(700,880),(480,860),(400,700),(240,770)],
 ground=((176,84,48),(150,70,40),P_DOT),road=((112,56,40),(126,64,46)),curb=((60,200,230),(240,240,240)),edge=(80,220,255),
 center="lights",ramps=[0.25,0.80],pads=[0.52],water=None,sky="mars")

def quant_tiles(img,surf_unused,maxt=254,seed=1):
    """img: SxS indexed (0..126). returns tiles (list of 64-arrays), map (128x128 tile ids 1..)"""
    T=img.reshape(128,8,128,8).transpose(0,2,1,3).reshape(128*128,64)
    uniq,inv,cnt=np.unique(T,axis=0,return_inverse=True,return_counts=True);inv=inv.reshape(-1)
    if len(uniq)<=maxt:return uniq,inv
    rng=np.random.default_rng(seed)
    return uniq,inv  # replaced below by kmeans in caller
def kmeans_tiles(uniq,cnt,palrgb,k,iters=10,seed=1):
    X=palrgb[uniq].reshape(len(uniq),-1).astype(np.float32)/255.0  # n x 192
    order=np.argsort(-cnt);cent=X[order[:k]].copy()
    w=cnt.astype(np.float32)
    for it in range(iters):
        d=(X*X).sum(1)[:,None]-2*X@cent.T+(cent*cent).sum(1)[None,:]
        a=d.argmin(1)
        for j in range(k):
            m=a==j
            if m.any():cent[j]=(X[m]*w[m,None]).sum(0)/w[m].sum()
    d=(X*X).sum(1)[:,None]-2*X@cent.T+(cent*cent).sum(1)[None,:];a=d.argmin(1)
    # medoid: real tile nearest to each centroid (keeps exact palette pixels)
    reps=[]
    for j in range(k):
        m=np.where(a==j)[0]
        if len(m)==0:reps.append(order[0]);continue
        dd=((X[m]-cent[j])**2).sum(1);reps.append(m[dd.argmin()])
    return np.array(reps),a
def build_track(ti,t):
    rnd=random.Random(100+ti)
    C=catmull(t['pts'],60);C,L=resample(C,1.0);n=len(C)
    w=t['w'];hw=w/2;R=hw+40
    D,T,SD=field(C,R)
    ts=T/n  # 0..1 along lap
    img=tex(*t['ground'],YY,XX).astype(np.uint8)
    surf=np.full((S,S),SF_OFF,np.uint8)
    # decorations ---------------------------------------------------------
    deco=Image.fromarray(img);dr=ImageDraw.Draw(deco);sd_img=Image.fromarray(surf);ds=ImageDraw.Draw(sd_img)
    def clear(x,y,r):
        xi,yi=int(min(max(x,0),S-1)),int(min(max(y,0),S-1))
        return D[yi,xi]>hw+r+18
    sky=t['sky']
    if t['water']=="west":
        # Pacific: ocean everything west of a wavy shoreline, beach strip
        for y in range(S):
            sx=120+int(28*math.sin(y/70.0)+14*math.sin(y/23.0))
            dr.line([(0,y),(sx,y)],fill=(40,110,200));dr.line([(sx,y),(sx+10,y)],fill=(236,226,180));ds.line([(0,y),(sx-2,y)],fill=SF_WATER)
        for i in range(260):
            x=rnd.randint(0,100);y=rnd.randint(0,S);dr.line([(x,y),(x+5,y)],fill=(150,200,250))
    if t['water']=="east":
        for y in range(S):
            sx=950+int(14*math.sin(y/60.0))
            dr.line([(sx,y),(S,y)],fill=(40,120,170));dr.line([(sx-8,y),(sx,y)],fill=(240,230,190));ds.line([(sx+2,y),(S,y)],fill=SF_WATER)
        for i in range(120):
            x=rnd.randint(960,S);y=rnd.randint(0,S);dr.line([(x,y),(x+5,y)],fill=(140,200,230))
    if sky=="texas":
        # Giga factory block in the infield + parking + cones
        bx0,by0,bx1,by1=300,330,520,560
        dr.rectangle([bx0,by0,bx1,by1],fill=(236,238,242),outline=(40,44,56),width=3)
        for k in range(6):dr.rectangle([bx0+12+k*34,by0+14,bx0+36+k*34,by1-14],fill=(208,212,222))
        dr.rectangle([bx0+70,by0+90,bx0+150,by0+140],fill=(30,34,50))  # roof solar / logo panel (generic)
        for k in range(9):dr.rectangle([bx0+74+k*8,by0+94,bx0+78+k*8,by0+136],fill=(70,110,200))
        ds.rectangle([bx0,by0,bx1,by1],fill=SF_WALL)
        for k in range(10):
            x=640+k*18;y=560
            if clear(x,y,8):dr.rectangle([x,y,x+12,y+22],outline=(250,250,250))
        for k in range(30):
            x,y=rnd.randint(40,980),rnd.randint(40,980)
            if clear(x,y,6):dr.ellipse([x-3,y-3,x+3,y+3],fill=(250,130,30),outline=(60,30,10));ds.ellipse([x-3,y-3,x+3,y+3],fill=SF_WALL)
    if sky=="pch":
        for k in range(45):
            x,y=rnd.randint(320,990),rnd.randint(30,990)
            if clear(x,y,14):
                r=rnd.randint(6,12);dr.ellipse([x-r,y-r,x+r,y+r],fill=(70,120,60),outline=(30,60,30),width=2);dr.ellipse([x-r//2,y-r//2,x,y],fill=(110,160,80))
                ds.ellipse([x-r+3,y-r+3,x+r-3,y+r-3],fill=SF_WALL)
        for k in range(30):
            x,y=rnd.randint(320,990),rnd.randint(30,990)
            if clear(x,y,8):dr.polygon([(x,y-6),(x+7,y+4),(x-7,y+4)],fill=(120,110,100),outline=(60,50,40))
    if sky=="starbase":
        # orbital launch mount + tower in the infield, tank farm, highway sign
        cx,cy=430,420
        dr.rectangle([cx-50,cy-50,cx+50,cy+50],fill=(184,184,180),outline=(60,60,60),width=2)
        dr.ellipse([cx-28,cy-28,cx+28,cy+28],fill=(80,80,86),outline=(30,30,30),width=3);dr.ellipse([cx-12,cy-12,cx+12,cy+12],fill=(210,210,214))
        dr.rectangle([cx+60,cy-20,cx+96,cy+16],fill=(60,64,70),outline=(20,20,20),width=2)
        for k in range(4):dr.line([(cx+60,cy-20+k*12),(cx+96,cy-8+k*12)],fill=(130,134,140))
        ds.rectangle([cx-30,cy-30,cx+30,cy+30],fill=SF_WALL);ds.rectangle([cx+60,cy-20,cx+96,cy+16],fill=SF_WALL)
        for k in range(6):
            x,y=600+k*30,330
            if clear(x,y,14):dr.ellipse([x-12,y-12,x+12,y+12],fill=(226,226,230),outline=(60,60,64),width=2);dr.ellipse([x-5,y-5,x+3,y+3],fill=(250,250,252));ds.ellipse([x-10,y-10,x+10,y+10],fill=SF_WALL)
        for k in range(40):
            x,y=rnd.randint(30,930),rnd.randint(30,990)
            if clear(x,y,12):
                r=rnd.randint(8,16);dr.ellipse([x-r,y-r//2,x+r,y+r//2],fill=(236,220,168))
    if sky=="mars":
        for (x,y,r) in [(430,420,48),(560,440,30),(470,560,34)]:
            dr.ellipse([x-r,y-r,x+r,y+r],fill=(210,230,236),outline=(60,80,90),width=3)
            dr.ellipse([x-r//2,y-r//2,x,y],fill=(250,252,255));
            for a in range(0,360,45):dr.line([(x,y),(x+r*math.cos(math.radians(a)),y+r*math.sin(math.radians(a)))],fill=(150,180,190))
            ds.ellipse([x-r+2,y-r+2,x+r-2,y+r-2],fill=SF_WALL)
        dr.line([(478,420),(530,440)],fill=(200,200,210),width=6);dr.line([(450,460),(465,530)],fill=(200,200,210),width=6)
        for k in range(55):
            x,y=rnd.randint(30,990),rnd.randint(30,990);r=rnd.randint(6,20)
            if clear(x,y,r):
                dr.ellipse([x-r,y-r,x+r,y+r],fill=(140,62,36),outline=(206,110,70),width=2);dr.ellipse([x-r+3,y-r+4,x+r-2,y+r-1],fill=(124,54,32))
        for k in range(10):
            x,y=rnd.randint(600,980),rnd.randint(600,980)
            if clear(x,y,10):dr.rectangle([x-8,y-4,x+8,y+4],fill=(40,50,90),outline=(160,170,200))  # solar arrays
    img=np.array(deco);surf=np.array(sd_img)
    # road ------------------------------------------------------------------
    road=D<=hw
    st=np.floor(T/6).astype(int)
    a1,a2=t['road'];asph=tex(a1,a2,P_ASPH,YY,XX)
    img[road]=asph[road];surf[road]=SF_ROAD
    curbm=(D>hw-6)&(D<=hw);cb=np.where(((st//2)%2)[...,None]==0,np.array(t['curb'][0],np.uint8),np.array(t['curb'][1],np.uint8))
    img[curbm]=cb[curbm];surf[curbm]=SF_CURB
    edge=(D>hw-9)&(D<=hw-7);img[edge]=t['edge']
    if t['center']=="dash":m=(D<1.6)&((st%6)<3);img[m]=(250,250,250)
    if t['center']=="yellow":m=(np.abs(SD)>0.6)&(np.abs(SD)<2.4)&road;img[m]=(250,206,40)
    if t['center']=="lights":m=(D<2.2)&((st%8)==0);img[m]=(120,240,255)
    # start/finish checker at t=0..8px
    sfm=road&((T<8)|(T>n-1))
    chk=((XX//4+YY//4)%2==0);img[sfm&chk]=(250,250,250);img[sfm&~chk]=(20,20,24)
    # grid boxes
    for k in range(1,9):
        tt=(n-24*k)%n
        for side in (-1,1):
            i=int(tt);c=C[i];g=C[(i+1)%n]-C[i];g/=np.linalg.norm(g)+1e-9;nrm=np.array([-g[1],g[0]])
            p=c+nrm*side*hw*0.42
            m=(np.abs(XX-p[0])<5)&(np.abs(YY-p[1])<5)&road&((np.abs(XX-p[0])>3.5)|(np.abs(YY-p[1])>3.5))
            img[m]=(240,240,240)
    # ramps (yellow/black chevron band across the road) + charge pads (red/white bolt band)
    for f in t['ramps']:
        i0=int(f*n);m=road&(np.abs(((T-i0+n/2)%n)-n/2)<7)
        ch=(((XX+YY)//6)%2==0);img[m&ch]=(250,210,30);img[m&~ch]=(30,30,30);surf[m]=SF_RAMP
    for f in t['pads']:
        i0=int(f*n);m=road&(np.abs(((T-i0+n/2)%n)-n/2)<12)&(D<hw-8)
        ch=((T.astype(int)//4)%2==0);img[m&ch]=(220,30,40);img[m&~ch]=(250,250,250);surf[m]=SF_PAD
    # outer fence 20px in from map edge (wraps visually, but cars stay inside)
    fence=(XX<14)|(YY<14)|(XX>S-15)|(YY>S-15)
    img[fence]=(90,90,96);img[fence&(((XX+YY)//4)%2==0)]=(200,200,206);surf[fence&(surf!=SF_WATER)]=SF_WALL
    # palette quantise to 125 colours (+ fog colour 126)
    pim=Image.fromarray(img).quantize(colors=125,method=Image.MEDIANCUT,dither=Image.Dither.NONE)
    idx=np.array(pim).astype(np.int32)+1;pal=pim.getpalette()[:125*3]
    pa=np.array(pal).reshape(-1,3);palrgb=np.zeros((128,3),np.int32);palrgb[1:1+len(pa)]=pa
    fogc={"texas":(200,220,240),"pch":(240,170,150),"starbase":(150,160,200),"mars":(220,170,120)}[sky]
    palrgb[126]=fogc
    # tiles ---------------------------------------------------------------
    Tl=idx.reshape(128,8,128,8).transpose(0,2,1,3).reshape(-1,64)
    uniq,inv,cnt=np.unique(Tl,axis=0,return_inverse=True,return_counts=True);inv=inv.reshape(-1)
    K=253
    if len(uniq)>K:
        reps,a=kmeans_tiles(uniq,cnt,palrgb,K);tiles=uniq[reps];inv=a[inv]
    else:tiles=uniq
    nt=len(tiles)
    tilemap=(inv+1).astype(np.int32).reshape(128,128)      # 0 = transparent tile
    fogtile=nt+1;alltiles=[np.zeros(64,np.int32)]+list(tiles)+[np.full(64,126,np.int32)]
    tilemap[127,127]=0;tilemap[127,126]=fogtile
    # per-tile surface (majority, but any wall/water pixel >= 25% wins)
    sm=surf.reshape(128,8,128,8).transpose(0,2,1,3).reshape(128,128,64)
    tsurf=np.zeros((128,128),np.uint8)
    for s_ in range(7):
        c=(sm==s_).sum(2)
        if s_==0:best=c;tsurf[:]=0
        else:
            upd=c>best;tsurf[upd]=s_;best=np.maximum(best,c)
    for s_ in (SF_WALL,SF_WATER):tsurf[((sm==s_).sum(2)>=24)]=s_
    tsurf[(sm==SF_RAMP).sum(2)>=16]=SF_RAMP
    # waypoints every 16 px + speed hint from curvature
    WPS=16;W=C[::WPS];nw=len(W)
    hd=np.arctan2(np.roll(W,-1,0)[:,0]-W[:,0],-(np.roll(W,-1,0)[:,1]-W[:,1]))
    curv=np.abs((np.roll(hd,-3)-hd+np.pi)%(2*np.pi)-np.pi)
    spd=np.array([min(1.0,max(0.50,1.0-1.05*max(curv[(i+j)%nw] for j in range(0,4)))) for i in range(nw)])
    # start: wp 0 heading
    h0=hd[0]
    ti_=ti
    emit(f"rc_tiles{ti}",[v for tl in alltiles for k in range(16) for v in [int(tl[k*4])|(int(tl[k*4+1])<<8)|(int(tl[k*4+2])<<16)|(int(tl[k*4+3])<<24)]],"unsigned int")
    emit(f"rc_map{ti}",tilemap.reshape(-1))
    emit(f"rc_surf{ti}",tsurf.reshape(-1))
    emit(f"rc_pal{ti}",[0]+[rgb15(palrgb[i]) for i in range(1,128)],"unsigned short")
    emit(f"rc_wp{ti}",[int(round(v)) for p in W for v in p],"short")
    emit(f"rc_spd{ti}",[int(v*255) for v in spd])
    Image.fromarray(img).save(f"/tmp/race_track{ti}.png")
    # preview of tile-quantised result
    tl=np.array(alltiles)[tilemap.reshape(-1)].reshape(128,128,8,8).transpose(0,2,1,3).reshape(S,S)
    Image.fromarray(palrgb[tl].astype(np.uint8)).save(f"/tmp/race_q{ti}.png")
    print(t['name'],"lap",int(L),"px wps",nw,"uniq",len(uniq),"tiles",len(alltiles))
    return dict(nw=nw,L=L,h0=h0,ntile=len(alltiles),hint=int(L))
INFO=[build_track(i,t) for i,t in enumerate(TRACKS)]
OUT.append("static const unsigned int*const rc_tiles[4]={rc_tiles0,rc_tiles1,rc_tiles2,rc_tiles3};")
OUT.append("static const unsigned char*const rc_mapv[4]={rc_map0,rc_map1,rc_map2,rc_map3};")
OUT.append("static const unsigned char*const rc_surfv[4]={rc_surf0,rc_surf1,rc_surf2,rc_surf3};")
OUT.append("static const unsigned short*const rc_palv[4]={rc_pal0,rc_pal1,rc_pal2,rc_pal3};")
OUT.append("static const short*const rc_wpv[4]={rc_wp0,rc_wp1,rc_wp2,rc_wp3};")
OUT.append("static const unsigned char*const rc_spdv[4]={rc_spd0,rc_spd1,rc_spd2,rc_spd3};")
OUT.append("static const unsigned short rc_ntile[4]={%s};"%",".join(str(i['ntile']) for i in INFO))
OUT.append("static const unsigned short rc_nwp[4]={%s};"%",".join(str(i['nw']) for i in INFO))
OUT.append("static const unsigned short rc_lapl[4]={%s};"%",".join(str(int(i['L'])) for i in INFO))
OUT.append("static const unsigned short rc_h0[4]={%s};"%",".join(str(int(((i['h0']/(2*math.pi))*65536)%65536)) for i in INFO))
OUT.append("static const char*const RC_TN[4]={%s};"%",".join('"%s"'%t['name'] for t in TRACKS))
OUT.append("static const unsigned short rc_grav[4]={%s};"%",".join(str(t['grav']) for t in TRACKS))
OUT.append("static const unsigned char rc_halfw[4]={%s};"%",".join(str(t['w']//2) for t in TRACKS))

# ---- sky panoramas 256x48, 4bpp, <=15 colours ------------------------------------------
def sky_img(kind):
    im=Image.new("RGB",(256,48));d=ImageDraw.Draw(im);rnd=random.Random(kind)
    def grad(c0,c1,y0=0,y1=48):
        for y in range(y0,y1):d.line([(0,y),(255,y)],fill=lerp(c0,c1,(y-y0)/max(1,y1-y0-1)))
    def cloud(x,y,s):
        for dx,dy,r in [(0,0,s),(s,-s//2,s),(2*s,0,s),(s,s//3,s)]:d.ellipse([x+dx-r,y+dy-r,x+dx+r,y+dy+r],fill=(255,255,255),outline=(120,150,200))
        for dx,dy,r in [(0,0,s-2),(s,-s//2,s-2),(2*s,0,s-2),(s,s//3,s-2)]:d.ellipse([x+dx-r,y+dy-r,x+dx+r,y+dy+r],fill=(255,255,255))
    if kind=="texas":
        grad((70,140,230),(190,220,250))
        for x,y,s in [(20,12,5),(110,8,6),(190,16,4)]:cloud(x,y,s)
        d.rectangle([60,30,200,44],fill=(236,238,242),outline=(70,74,90));d.rectangle([60,28,200,31],fill=(200,204,214))
        for k in range(14):d.rectangle([66+k*10,35,70+k*10,38],fill=(70,90,140))
        d.rectangle([0,44,255,47],fill=(150,150,80));d.line([(0,44),(255,44)],fill=(120,124,64))
    elif kind=="pch":
        grad((90,60,140),(250,150,110),0,40)
        d.ellipse([150,28,190,68],fill=(255,220,120));d.rectangle([0,40,255,47],fill=(40,90,170))
        for k in range(16):x=rnd.randint(0,250);d.line([(x,42+k%5),(x+6,42+k%5)],fill=(250,200,140))
        d.polygon([(0,40),(0,22),(14,18),(30,26),(48,24),(70,40)],fill=(70,50,80));d.polygon([(200,40),(214,30),(236,28),(255,34),(255,40)],fill=(70,50,80))
    elif kind=="starbase":
        grad((20,30,80),(170,140,190))
        for k in range(18):d.point((rnd.randint(0,255),rnd.randint(0,18)),fill=(250,250,255))
        tx=120;d.rectangle([tx,6,tx+8,44],fill=(90,94,110));
        for y in range(8,44,4):d.line([(tx,y),(tx+8,y+4)],fill=(150,154,170))
        d.rectangle([tx+10,4,tx+16,44],fill=(214,216,224));d.rectangle([tx+10,22,tx+16,23],fill=(150,150,160));d.polygon([(tx+10,4),(tx+13,0),(tx+16,4)],fill=(214,216,224))
        d.line([(tx+8,18),(tx+10,18)],fill=(40,40,50));d.line([(tx+8,26),(tx+10,26)],fill=(40,40,50))
        for k in range(5):d.rectangle([30+k*9,34,36+k*9,44],fill=(226,226,232),outline=(80,80,96))
        d.rectangle([0,44,255,47],fill=(200,184,140))
    else:
        grad((190,130,90),(236,190,140))
        d.polygon([(0,44),(30,30),(60,36),(100,24),(140,38),(180,30),(220,40),(255,34),(255,44)],fill=(150,80,50))
        d.polygon([(0,44),(50,38),(90,42),(160,36),(255,42),(255,44)],fill=(120,60,40))
        for x,r in [(200,6),(214,4)]:d.ellipse([x-r,40-r,x+r,40+r],fill=(220,236,240),outline=(90,100,110))
        d.ellipse([40,6,44,10],fill=(160,140,130));d.rectangle([0,44,255,47],fill=(176,84,48))
    return im
for i,k in enumerate(["texas","pch","starbase","mars"]):
    im=sky_img(k);q=im.quantize(colors=15,method=Image.MEDIANCUT,dither=Image.Dither.NONE);a=np.array(q).astype(np.int32)+1
    pal=q.getpalette()[:45];P=[0]+[rgb15(pal[j*3:j*3+3]) for j in range(15)]
    T=a.reshape(6,8,32,8).transpose(0,2,1,3).reshape(-1,64);uniq,inv=np.unique(T,axis=0,return_inverse=True);inv=inv.reshape(-1)
    words=[]
    for tl in uniq:
        for r in range(8):
            v=0
            for c in range(8):v|=int(tl[r*8+c])<<(4*c)
            words.append(v)
    emit(f"rc_skyt{i}",words,"unsigned int");emit(f"rc_skym{i}",[int(x) for x in inv],"unsigned short");emit(f"rc_skyp{i}",P,"unsigned short")
    im.resize((512,96),Image.NEAREST).save(f"/tmp/race_sky{i}.png");print("sky",k,"tiles",len(uniq))
OUT.append("static const unsigned int*const rc_skyt[4]={rc_skyt0,rc_skyt1,rc_skyt2,rc_skyt3};")
OUT.append("static const unsigned short rc_skyn[4]={%s};"%",".join(str(len(np.unique(np.array(sky_img(k).quantize(colors=15,method=Image.MEDIANCUT,dither=Image.Dither.NONE)).reshape(6,8,32,8).transpose(0,2,1,3).reshape(-1,64),axis=0))) for k in ["texas","pch","starbase","mars"]))
OUT.append("static const unsigned short*const rc_skym[4]={rc_skym0,rc_skym1,rc_skym2,rc_skym3};")
OUT.append("static const unsigned short*const rc_skyp[4]={rc_skyp0,rc_skyp1,rc_skyp2,rc_skyp3};")
exec(open(os.path.join(ROOT,'tools','gen_race_cars.py')).read())
open(os.path.join(ROOT,'src','racedb.inc'),'w').write("// generated by tools/gen_race.py - do not edit\n"+"\n".join(OUT)+"\n")
