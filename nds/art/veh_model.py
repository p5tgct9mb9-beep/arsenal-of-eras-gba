"""veh_model.py - spec-driven top-down armour models for veh_ds.py (ds-v0.3.0).
Each tank / SPG / IFV / armoured car is rebuilt from its vehicle-database spec (length, width, track width,
turret shape and size, gun length and calibre, muzzle brake, skirts, wheels, casemate ...) as a layered
model drawn at 4x resolution: track runs with links and rounded sprockets, fenders, a hull with a lit glacis,
driver hatches, engine deck with grilles and exhausts, tools, side skirts; turrets with dome/plate shading,
mantlet, cupola and hatches, and round barrels with muzzle brakes / fume extractors.  The 4x render is then
reduced to the 32x32 sprite with the team-palette ramps (camo 2-5, steel 12/13/14/11, dark 7, team 9)."""
import numpy as np
from PIL import Image, ImageDraw
S=4; N=32*S
MAT_CAMO,MAT_METAL,MAT_DARK,MAT_TEAM,MAT_GLASS=1,2,3,4,5
CAMO=[2,3,4,5]; METAL=[12,13,14,11]
class Model:
    def __init__(s):s.mat=np.zeros((N,N),np.uint8);s.tone=np.zeros((N,N));s.lit=np.zeros((N,N))
    def _mask(s,fn):
        im=Image.new('L',(N,N),0);fn(ImageDraw.Draw(im));return np.array(im)>127
    def put(s,mask,mat,tone,bevel=0.0,dome=0.0):
        """paint mask with material `mat`; tone 0..1; bevel darkens the outer 1px; dome lights the centre"""
        if bevel or dome:
            d=_dist(mask,int(S*4));mx=max(1,d.max())
            t=tone+dome*(d/mx-0.5)*2*0.5
            t=np.where(d<=S*0.9,t-bevel,t)
        else:t=np.full((N,N),tone)
        s.mat[mask]=mat;s.tone[mask]=t[mask]
    def rect(s,x0,y0,x1,y1,mat,tone,**k):
        if x1-x0<0.26 or y1-y0<0.26:return
        s.put(s._mask(lambda d:d.rectangle([x0*S,y0*S,x1*S-1,y1*S-1],fill=255)),mat,tone,**k)
    def ell(s,x0,y0,x1,y1,mat,tone,**k):s.put(s._mask(lambda d:d.ellipse([x0*S,y0*S,x1*S-1,y1*S-1],fill=255)),mat,tone,**k)
    def poly(s,pts,mat,tone,**k):s.put(s._mask(lambda d:d.polygon([(x*S,y*S) for x,y in pts],fill=255)),mat,tone,**k)
    def line(s,x0,y0,x1,y1,w,mat,tone):s.put(s._mask(lambda d:d.line([x0*S,y0*S,x1*S,y1*S],fill=255,width=max(1,int(w*S)))),mat,tone)
    def stripes(s,x0,y0,x1,y1,period,mat,tone,axis=0,duty=0.5):
        """alternating bands (track links, grilles, planking); axis 0 = bands across y"""
        for k in range(int((y1-y0 if axis==0 else x1-x0)/period)+1):
            a=(y0 if axis==0 else x0)+k*period;b=a+period*duty
            if axis==0:s.rect(x0,a,x1,min(b,y1),mat,tone)
            else:s.rect(a,y0,min(b,x1),y1,mat,tone)
    def reduce(s,outline=True):
        g=np.zeros((32,32),np.uint8)
        for y in range(32):
            for x in range(32):
                bm=s.mat[y*S:(y+1)*S,x*S:(x+1)*S];bt=s.tone[y*S:(y+1)*S,x*S:(x+1)*S]
                cov=(bm>0).mean()
                if cov<0.45:continue
                vals,cnt=np.unique(bm[bm>0],return_counts=True);m=int(vals[cnt.argmax()]);t=float(bt[bm==m].mean())
                g[y,x]=_pal(m,t)
        if outline:
            m=g>0;p=np.pad(m,1);nb=p[:-2,1:-1]|p[2:,1:-1]|p[1:-1,:-2]|p[1:-1,2:]
            g[(~m)&nb]=1
        return g
def _pal(m,t):
    t=min(0.999,max(0.0,t))
    if m==MAT_CAMO:return CAMO[int(t*4)]
    if m==MAT_METAL:return METAL[int(t*4)]
    if m==MAT_DARK:return 1 if t<0.35 else 7
    if m==MAT_TEAM:return 9
    if m==MAT_GLASS:return 10 if t<0.8 else 11
    return 0
def _dist(mask,cap):
    d=np.where(mask,cap,0).astype(np.int32)
    for _ in range(cap):
        p=np.pad(d,1);nb=np.minimum.reduce([p[:-2,1:-1],p[2:,1:-1],p[1:-1,:-2],p[1:-1,2:]])+1
        d=np.where(mask,np.minimum(d,nb),0)
    return d
C,M,D,T=MAT_CAMO,MAT_METAL,MAT_DARK,MAT_TEAM
def tank_hull(sp):
    L=sp.get('L',22);W=sp.get('W',18);oy=sp.get('oy',0);cx=16.0
    y0=16-L//2+oy;y1=y0+L;x0=cx-W//2;x1=cx+W//2;wh=sp.get('wh',0);tw=sp.get('tw',4 if W>=16 else 3)
    m=Model()
    if wh==0:
        for xa in (x0,x1-tw):
            m.rect(xa,y0+0.5,xa+tw,y1-0.5,M,0.10)
            m.ell(xa,y0,xa+tw,y0+tw*0.9,M,0.10);m.ell(xa,y1-tw*0.9,xa+tw,y1,M,0.10)   # sprocket / idler
            m.stripes(xa,y0+1,xa+tw,y1-1,2,M,0.40)                 # link cleats
            m.rect(xa+tw//2,y0+1,xa+tw//2+1,y1-1,M,0.05) if tw>=4 else None                           # guide horns
        bx0,bx1=x0+tw-1,x1-tw+1
    elif wh=='half':
        for xa in (x0,x1-3):
            m.rect(xa,y0+L/2,xa+3,y1,M,0.1);m.stripes(xa+0.5,y0+L/2+0.5,xa+2.5,y1-0.5,1.5,M,0.4)
            m.ell(xa,y0+1.5,xa+3,y0+5.5,M,0.05);m.ell(xa+0.8,y0+2.5,xa+2.2,y0+4.5,M,0.55)  # front tyres + hubs
        bx0,bx1=x0+1,x1-1
    else:
        n=wh//2
        for k in range(n):
            y=y0+2+k*(L-6)/max(1,n-1) if n>1 else y0+L/2
            for xa in (x0,x1-3):m.ell(xa,y,xa+3,y+4,M,0.05);m.ell(xa+0.8,y+1,xa+2.2,y+3,M,0.55)
        bx0,bx1=x0+1,x1-1
    bw=bx1-bx0
    if sp.get('jeep'):
        m.rect(bx0,y0,bx1,y1,C,0.5,bevel=0.25);m.rect(bx0+1,y0,bx1-1,y0+4,C,0.75,bevel=0.2)   # bonnet
        m.rect(bx0,y0+5,bx1,y0+5.8,D,0.6)                                                     # windscreen
        m.rect(bx0+1,y0+7,bx1-1,y1-2,D,0.5);m.rect(bx0+1.5,y0+8,cx-0.5,y0+11,C,0.35);m.rect(cx+0.5,y0+8,bx1-1.5,y0+11,C,0.35)
        m.rect(bx0+2,y1-2.5,bx1-2,y1-0.5,M,0.3)                                               # spare wheel
        return m
    if sp.get('mine'):
        m.rect(bx0,y0,bx1,y1,C,0.5,bevel=0.3);m.rect(bx0+1,y0+1,bx1-1,y0+3,C,0.75);return m
    gl=max(2.5,L*0.16)
    m.poly([(bx0,y0+gl),(bx0+1.5,y0),(bx1-1.5,y0),(bx1,y0+gl),(bx1,y1),(bx0,y1)],C,0.40,bevel=0.22)
    m.poly([(bx0+0.6,y0+gl),(bx0+1.8,y0+0.5),(bx1-1.8,y0+0.5),(bx1-0.6,y0+gl)],C,0.62)          # glacis catches the sun
    m.rect(bx0+1,y0+1,bx1-1,y0+2,C,0.85);m.rect(bx0+1,y0+gl,bx1-1,y0+gl+1,C,0.15)                                          # glacis / roof seam
    if bw>=9:                                                                                 # driver + co-driver hatches
        for hx in (bx0+bw*0.27,bx1-bw*0.27):m.rect(round(hx)-1,y0+gl+1,round(hx)+1,y0+gl+3,C,0.65);m.rect(round(hx)-1,y0+gl+2,round(hx)+1,y0+gl+3,C,0.15)
    for hx in (bx0+0.8,bx1-1.8):m.rect(hx,y0+0.6,hx+1,y0+1.4,M,0.9)                         # headlights
    if not sp.get('open'):
        ey=y1-max(4,L*0.26)
        ey=round(ey);m.rect(bx0+1,ey,bx1-1,y1-1,C,0.15)                                       # engine deck
        m.stripes(bx0+2,ey+1,bx1-2,y1-2,2,C,0.62)                                        # grilles
        m.rect(cx-0.5,ey,cx+0.5,y1-1,C,0.40)
        for ex in (bx0+1,bx1-3):m.rect(ex,y1-1,ex+2,y1,D,0.6)                        # exhausts
    else:
        m.rect(bx0+1.5,y0+L/3,bx1-1.5,y1-1.5,D,0.55,bevel=0.0)                               # open crew bay
        for k in range(3):m.rect(bx0+2,y0+L/3+1.5+k*3,bx1-2,y0+L/3+2.3+k*3,C,0.3)             # benches
    if L>=16:m.rect(bx0+1,round(y0+L*0.36),bx0+2,round(y0+L*0.6),M,0.55)                          # pioneer tools
    if sp.get('skirt'):
        for xa in (x0-0.2,x1-1.6):
            m.rect(xa,y0+3,xa+1.8,y1-2,C,0.5,bevel=0.2)
            for k in range(int((L-5)/4)):m.rect(xa,y0+3+(k+1)*4-0.4,xa+1.8,y0+3+(k+1)*4,C,0.2)
    m.rect(cx-1,y1-0.9,cx+1,y1,T,0.5)                                                         # team marking
    return m
def tank_turret(sp):
    m=Model();cx=16.0;tu=sp.get('tur','r');tsw,tsh=sp.get('ts',(10,10));gl=sp.get('gl',12);gt=max(1.6,sp.get('gt',2)*0.95)
    tx0,ty0=cx-tsw/2,16-tsh/2;tx1,ty1=tx0+tsw,ty0+tsh
    def gun(ytop,ybase):
        w=max(1,int(round(gt)));gx0=16-(w+1)//2;ytop=int(ytop)
        m.rect(gx0,ytop,gx0+w,ybase,M,0.55)
        if w>=2:m.rect(gx0,ytop+1,gx0+1,ybase,M,0.80)                                          # lit side of the tube
        if sp.get('mb'):m.rect(gx0-1,ytop,gx0+w+1,ytop+1,M,0.3)
        if sp.get('fume') or gl>=13:fy=int(ytop+gl*0.45);m.rect(gx0-1,fy,gx0+w+1,fy+1,M,0.55)
        m.rect(gx0,ytop,gx0+w,ytop+1,M,0.05)                                                   # muzzle
    if tu=='mg':
        m.ell(cx-2.5,13.5,cx+2.5,18.5,C,0.45,bevel=0.2);m.rect(cx-0.5,16-gl,cx+0.5,16,M,0.55);m.ell(cx-1.5,15,cx+1.5,18,D,0.4);return m
    if tu=='n':return m
    if tu=='c':m.ell(tx0,ty0,tx1,ty1,C,0.50,bevel=0.3,dome=0.6)
    elif tu=='r':m.ell(tx0,ty0,tx1,ty1,C,0.50,bevel=0.3,dome=0.5)
    elif tu=='b':
        m.poly([(tx0+1,ty0),(tx1-1,ty0),(tx1,ty0+1.5),(tx1,ty1+0.8),(tx0,ty1+0.8),(tx0,ty0+1.5)],C,0.5,bevel=0.3,dome=0.3)
        m.rect(tx0+0.8,ty0+0.3,tx1-0.8,ty0+1.2,C,0.85)                                          # lit front plate edge
        m.rect(tx0+1.2,ty1-0.6,tx1-1.2,ty1+0.4,C,0.32)                                          # bustle seam
    elif tu=='h':m.poly([(tx0+2,ty0),(tx1-2,ty0),(tx1,ty0+3),(tx1,ty1),(tx0,ty1),(tx0,ty0+3)],C,0.5,bevel=0.3,dome=0.5)
    elif tu=='o':
        m.ell(tx0,ty0,tx1,ty1,C,0.55,bevel=0.25);m.ell(tx0+1.8,ty0+1.8,tx1-1.8,ty1-1.8,D,0.55)
        m.rect(cx-1.5,ty0+3,cx+1.5,ty1-3,C,0.3)                                                 # gun breech
    m.rect(round(cx-gt/2)-1,round(ty0)-1,round(cx+gt/2)+1,round(ty0)+2,C,0.2)                           # mantlet
    gun(max(0,ty0-gl),ty0+1)
    if tu!='o':
        r=1.7 if tsw>=10 else 1.3
        ccx,ccy=tx1-tsw*0.3,ty1-tsh*0.32
        m.ell(ccx-r,ccy-r,ccx+r,ccy+r,M,0.6,bevel=0.3);m.ell(ccx-r*0.5,ccy-r*0.5,ccx+r*0.5,ccy+r*0.5,D,0.5)  # cupola
        if tsw>=9:m.ell(tx0+tsw*0.18,ccy-1.2,tx0+tsw*0.18+2.4,ccy+1.2,C,0.68,bevel=0.3)        # loader hatch
        m.rect(ccx-0.4,ccy-r-0.8,ccx+0.4,ccy-r,MAT_GLASS,0.5)                                 # vision block
    if sp.get('rk'):
        m.rect(cx-8,ty0-1,cx+8,ty0+3,M,0.45,bevel=0.2);m.stripes(cx-7.5,ty0-0.5,cx+7.5,ty0+2.5,2,D,0.4,axis=1)
    if sp.get('coax'):m.rect(cx+gt/2+0.6,ty0-4,cx+gt/2+1.3,ty0,M,0.4)
    if sp.get('atgm'):m.rect(tx0-2.2,ty0+1,tx0-0.4,ty0+6.5,M,0.55,bevel=0.2);m.rect(tx0-2.2,ty0+0.6,tx0-0.4,ty0+1.4,D,0.2)
    if sp.get('pod'):
        pw,ph=sp['pod'];px0,py0=cx-pw/2,16-ph/2-2
        m.rect(px0,py0,px0+pw,py0+ph,C,0.5,bevel=0.25)
        for x in np.arange(px0+1,px0+pw-1,2):
            for y in np.arange(py0+1,py0+ph/2+1,2):m.rect(x,y,x+1,y+1,D,0.2)
    if sp.get('twin'):
        for gx in (cx-4.5,cx+3.5):m.rect(gx,ty0-gl+2,gx+1,ty0,M,0.6)
    m.rect(tx0+1.5,ty1-2,tx0+2.5,ty1-1,T,0.5)                                                  # team marking
    return m
def casemate(sp,m):
    """casemate SPG/TD superstructure + hull gun, painted onto the hull model"""
    L=sp.get('L',22);W=sp.get('W',18);oy=sp.get('oy',0);cx=16.0;y0=16-L//2+oy
    cw,ch=sp.get('cas',(W-6,L//2));cy0=y0+sp.get('cy',4);gl=sp.get('gl',12);gt=max(1.6,sp.get('gt',2)*0.95)
    m.poly([(cx-cw/2,cy0+1.5),(cx-cw/2+1.5,cy0),(cx+cw/2-1.5,cy0),(cx+cw/2,cy0+1.5),(cx+cw/2,cy0+ch+1),(cx-cw/2,cy0+ch+1)],C,0.6,bevel=0.28,dome=0.1)
    m.rect(cx-cw/2+1,cy0+0.2,cx+cw/2-1,cy0+1.2,C,0.85)
    hx,hy=cx+cw/2-3,cy0+ch-2.5;m.ell(hx-1.3,hy-1.3,hx+1.3,hy+1.3,M,0.6,bevel=0.3);m.ell(hx-0.6,hy-0.6,hx+0.6,hy+0.6,D,0.5)
    if sp.get('mort'):m.ell(cx-3,cy0-3,cx+3,cy0+3,M,0.5,bevel=0.3);m.ell(cx-1.8,cy0-1.8,cx+1.8,cy0+1.8,D,0.2);return
    ytop=max(0,cy0-gl);gx0=cx-gt/2
    m.rect(cx-gt/2-1.3,cy0-0.8,cx+gt/2+1.3,cy0+1.6,C,0.4,bevel=0.15)
    m.rect(gx0,ytop,gx0+gt,cy0+2,M,0.55);m.rect(cx-0.35,ytop+0.5,cx+0.35,cy0+1,M,0.85)
    if sp.get('mb'):m.rect(gx0-0.8,ytop,gx0+gt+0.8,ytop+1.4,M,0.45,bevel=0.2)
    m.rect(cx-0.5,ytop,cx+0.5,ytop+0.6,D,0.1)
def tank(sp):
    """-> (hull 32x32, turret 32x32) index grids"""
    h=tank_hull(sp)
    if sp.get('tur')=='n':casemate(sp,h)
    return h.reduce(),tank_turret(sp).reduce()
