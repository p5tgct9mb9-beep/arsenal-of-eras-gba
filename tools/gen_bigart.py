#!/usr/bin/env python3
"""v1.3.5 big side-view pixel art (original, procedural).
Each vehicle = ordered layers of polygons (pixel coords on a W x H canvas) with a material ramp
and a shading mode. Renderer: per-layer cylindrical/planar shading quantised to a 6-step ramp with
light ordered dither, inter-part panel seams, 1px dark silhouette outline, panel lines, glossy
canopy streak, ground drop shadow. Output: indexed 8bpp + shared palette, RLE for the ROM."""
from PIL import Image, ImageDraw
import math, sys, os
BAY=[[0,8,2,10],[12,4,14,6],[3,11,1,9],[15,7,13,5]]
PAL=[(0,0,0)]
def ramp(name,cols):
    globals()[name]=len(PAL);PAL.extend(cols)
ramp('OUT',[(14,16,14)])
ramp('GRAY',[(38,40,44),(62,65,70),(90,94,100),(120,124,130),(152,156,162),(188,191,195)])
ramp('OLIVE',[(36,40,24),(56,62,36),(78,86,50),(102,110,66),(130,138,88),(162,168,118)])
ramp('KHAKI',[(58,50,34),(86,76,52),(116,104,72),(146,134,96),(176,164,124),(206,196,158)])
ramp('NAVY',[(24,30,44),(38,48,66),(56,68,90),(78,92,116),(104,120,142),(140,154,172)])
ramp('METAL',[(30,28,28),(52,48,46),(78,72,68),(108,100,94),(140,132,124),(176,170,162)])
ramp('GLASS',[(14,16,18),(30,34,36),(52,56,54),(84,86,78),(140,138,118),(232,230,214)])
ramp('SILVER',[(70,74,80),(104,108,114),(138,142,148),(170,174,180),(200,204,208),(232,234,236)])
ramp('RED',[(70,18,16),(110,30,24),(150,44,32),(186,62,44),(214,96,70),(236,140,110)])
ramp('YEL',[(90,70,10),(140,110,20),(190,150,30),(220,184,50),(240,210,90),(250,232,150)])
ramp('BG',[(20,22,16),(34,36,27),(46,48,36),(58,60,45),(70,72,54)])
ramp('FIRE',[(120,40,10),(200,90,20),(250,170,60),(255,236,170)])
assert len(PAL)<=256
RAMPS={'gray':GRAY,'olive':OLIVE,'khaki':KHAKI,'navy':NAVY,'metal':METAL,'glass':GLASS,'silver':SILVER,'red':RED,'yel':YEL}

class Art:
    def __init__(s,w,h):
        s.w=w;s.h=h;s.idx=[[0]*w for _ in range(h)];s.own=[[-1]*w for _ in range(h)];s.n=0;s.lines=[]
    def layer(s,pts,mat,mode='cyl',base=0.0,peak=0.28,amp=1.0,seam=True):
        m=Image.new('1',(s.w,s.h),0);ImageDraw.Draw(m).polygon(pts,fill=1)
        px=m.load();rid=RAMPS[mat];L=s.n;s.n+=1
        cols={}
        for x in range(s.w):
            ys=[y for y in range(s.h) if px[x,y]]
            if ys:cols[x]=(ys[0],ys[-1])
        if not cols:return
        xs=sorted(cols);x0,x1=xs[0],xs[-1]
        Y0=min(a for a,b in cols.values());Y1=max(b for a,b in cols.values())
        for x,(t0,t1) in cols.items():
            for y in range(t0,t1+1):
                if not px[x,y]:continue
                if mode=='cyl':
                    t=(y-t0)/max(1,t1-t0)
                    v=1.0-abs(t-peak)*1.9 if t<peak else 1.0-(t-peak)/(1-peak)*1.15
                    v=0.50+0.55*v
                elif mode=='plane':  # lit from upper-left
                    u=(x-x0)/max(1,x1-x0);w=(y-Y0)/max(1,Y1-Y0)
                    v=0.85-0.35*u-0.35*w
                elif mode=='under':
                    t=(y-t0)/max(1,t1-t0);v=0.45-0.25*t
                else: v=0.6
                v=v*amp+base
                q=max(0.0,min(5.0,v*5.0))
                k=int(q);f=q-k
                if k<5 and ((f>0.62) or (0.38<f<=0.62 and (x+y)&1)):k+=1
                s.idx[y][x]=rid+k;s.own[y][x]=L
        s.last=(rid,L)
    def line(s,pts,mat,k=1,dk=None):  # panel line: k = ramp step
        rid=RAMPS[mat]
        for (ax,ay),(bx,by) in zip(pts,pts[1:]):
            n=max(abs(bx-ax),abs(by-ay),1)
            for i in range(n+1):
                x=round(ax+(bx-ax)*i/n);y=round(ay+(by-ay)*i/n)
                if 0<=x<s.w and 0<=y<s.h and s.idx[y][x]:
                    s.idx[y][x]=rid+k if dk is None else s.idx[y][x]-dk if s.idx[y][x]-dk>=rid else rid
    def darken_line(s,pts,dk=2):
        for (ax,ay),(bx,by) in zip(pts,pts[1:]):
            n=max(abs(bx-ax),abs(by-ay),1)
            for i in range(n+1):
                x=round(ax+(bx-ax)*i/n);y=round(ay+(by-ay)*i/n)
                if 0<=x<s.w and 0<=y<s.h and s.idx[y][x]>OUT:
                    r=base_of(s.idx[y][x]);s.idx[y][x]=max(r,s.idx[y][x]-dk)
    def gloss(s,pts):
        for (ax,ay),(bx,by) in zip(pts,pts[1:]):
            n=max(abs(bx-ax),abs(by-ay),1)
            for i in range(n+1):
                x=round(ax+(bx-ax)*i/n);y=round(ay+(by-ay)*i/n)
                if s.idx[y][x]:s.idx[y][x]=GLASS+5
    def dot(s,x,y,c):
        if s.idx[y][x]:s.idx[y][x]=c
    def finish(s):
        I=s.idx;O=s.own;W,H=s.w,s.h
        # seams: pixel whose owner is a later layer bordering an earlier layer -> one step darker
        N=[r[:] for r in I]
        for y in range(H):
            for x in range(W):
                if not I[y][x]:continue
                o=O[y][x]
                for dx,dy in((1,0),(-1,0),(0,1),(0,-1)):
                    X,Y=x+dx,y+dy
                    if 0<=X<W and 0<=Y<H and I[Y][X] and O[Y][X]>=0 and O[Y][X]<o and dy>=0:
                        b=base_of(I[y][x]);N[y][x]=max(b,I[y][x]-2);break
        I=N
        # outline
        N=[r[:] for r in I]
        for y in range(H):
            for x in range(W):
                if I[y][x]:continue
                for dx,dy in((1,0),(-1,0),(0,1),(0,-1)):
                    X,Y=x+dx,y+dy
                    if 0<=X<W and 0<=Y<H and I[Y][X] and I[Y][X]!=OUT:N[y][x]=OUT;break
        s.idx=N;return s
def base_of(i):
    b=1
    for r in sorted(RAMPS.values()):
        if i>=r:b=r
    return b if i>OUT else i

def background(w,h,seed=1):
    """horizontal speed bands: dark edges, calm olive centre, checker-dither transitions, dashed streaks"""
    import random;R=random.Random(seed)
    def lv(y):
        c=abs(y-h*0.47)/(h*0.53)
        if c<0.30:return 3.0
        if c<0.42:return 2.5
        if c<0.52:return 2.0+0.5*((y//3)&1)
        if c<0.66:return 1.5+((y//2)%3==0)
        if c<0.80:return 1.0+0.5*((y//2)&1)
        return 0.5 if c<0.92 else 0.0
    g=[]
    for y in range(h):
        L=lv(y);k=int(L);half=L-k>=0.5;row=[]
        for x in range(w):row.append(BG+min(4,k+(1 if half and (x+y)&1 else 0)))
        c=abs(y-h*0.47)/(h*0.53)
        if c>0.42:
            x=R.randrange(-40,w)
            while x<w:
                ln=R.randrange(6,40);d=1 if R.random()<.5 else -1
                for X in range(max(0,x),min(w,x+ln)):
                    if (X+y)&1:row[X]=BG+max(0,min(4,row[X]-BG+d))
                x+=ln+R.randrange(10,70)
        g.append(row)
    return g

def shadow(art,bg,ox,oy,drop):
    """squashed silhouette shadow on the band background"""
    for y in range(art.h):
        for x in range(art.w):
            if art.idx[y][x]:
                Y=oy+drop+y//5;X=ox+x+6
                if 0<=Y<len(bg) and 0<=X<len(bg[0]) and ((X+Y)&1 or True):
                    bg[Y][X]=BG
def compose(art,w=240,h=160,ox=None,oy=None,drop=None,seed=1):
    bg=background(w,h,seed);ox=(w-art.w)//2 if ox is None else ox;oy=(h-art.h)//2-6 if oy is None else oy
    shadow(art,bg,ox,oy,drop or art.h-2)
    for y in range(art.h):
        for x in range(art.w):
            if art.idx[y][x] and 0<=oy+y<h and 0<=ox+x<w:bg[oy+y][ox+x]=art.idx[y][x]
    return bg
def to_img(g,scale=1):
    h=len(g);w=len(g[0]);im=Image.new('RGB',(w,h))
    im.putdata([PAL[c] for r in g for c in r]);return im.resize((w*scale,h*scale),Image.NEAREST) if scale>1 else im

# ----------------------------------------------------------------- vehicles (original designs from public 3-view proportions)
def f22():
    A=Art(208,64)
    # far wing (behind body, darker)
    A.layer([(92,40),(150,34),(174,38),(176,42),(120,46)],'gray','plane',base=-0.25)
    # far tail
    A.layer([(150,32),(164,6),(174,6),(178,30),(170,34)],'gray','plane',base=-0.18)
    # fuselage: long chined body, pointed nose
    A.layer([(4,38),(22,34),(44,29),(60,25),(84,25),(150,27),(186,30),(198,33),(198,38),(176,41),(120,44),(70,45),(40,44),(18,41)],'gray','cyl',peak=0.22)
    # intake / lower chine dark band
    A.layer([(62,36),(76,33),(118,34),(118,42),(70,44),(60,42)],'gray','under',base=0.0)
    # near wing (big trapezoid, seen slightly from above)
    A.layer([(86,40),(138,33),(176,40),(178,45),(150,49),(96,46)],'gray','plane',base=0.05)
    # near tail (canted)
    A.layer([(160,30),(176,4),(188,4),(192,30),(184,34)],'gray','plane',base=0.0)
    # horizontal stab
    A.layer([(170,36),(204,40),(204,43),(176,43)],'gray','plane',base=-0.05)
    # nozzle
    A.layer([(186,32),(200,33),(200,39),(186,40)],'metal','cyl',base=-0.3)
    # canopy
    A.layer([(30,32),(40,25),(52,22),(64,22),(72,25),(60,30),(38,33)],'glass','cyl',peak=0.15,base=-0.05)
    A.finish()
    A.gloss([(40,26),(46,24),(56,23)])
    A.darken_line([(60,30),(100,29),(150,29)],1)     # chine line
    A.darken_line([(118,34),(118,42)],2)
    A.darken_line([(96,46),(150,36)],1)
    A.darken_line([(132,36),(136,46)],1)
    A.darken_line([(22,37),(30,40),(60,42)],1)
    A.darken_line([(170,8),(184,30)],1)
    A.darken_line([(196,33),(196,39)],2)
    for x in range(86,150,10):A.dot(x,27,GRAY+1)
    return A
def f16():
    A=Art(200,64)
    A.layer([(96,38),(132,34),(150,38),(110,42)],'gray','plane',base=-0.25)   # far wing
    A.layer([(4,34),(20,30),(46,26),(74,25),(150,26),(176,28),(184,31),(184,37),(150,40),(70,42),(30,40),(12,37)],'gray','cyl',peak=0.25)
    A.layer([(46,38),(60,36),(78,38),(76,46),(52,46)],'gray','under',base=0.05)   # chin intake
    A.layer([(88,38),(132,32),(152,36),(148,42),(102,44)],'gray','plane')        # near wing
    A.layer([(148,26),(160,2),(172,2),(178,26)],'gray','plane',base=0.02)          # single tail
    A.layer([(156,34),(190,36),(190,40),(160,40)],'gray','plane',base=-0.05)
    A.layer([(176,29),(190,30),(190,37),(176,38)],'metal','cyl',base=-0.3)
    A.layer([(26,30),(34,21),(46,17),(58,18),(70,24),(54,28),(32,30)],'glass','cyl',peak=0.15)
    A.layer([(110,38),(118,38),(118,50),(110,50)],'gray','cyl',base=-0.1)   # fuel tank pylon
    A.layer([(98,48),(132,48),(138,50),(132,53),(98,53),(94,50)],'gray','cyl',base=-0.05)  # drop tank
    A.finish()
    A.gloss([(36,21),(44,18),(52,18)])
    A.darken_line([(70,28),(150,29)],1);A.darken_line([(120,34),(124,42)],1);A.darken_line([(182,30),(182,37)],2)
    A.darken_line([(164,6),(172,26)],1)
    return A
def old_jet_sprite():
    """before: the current 32x32 in-game jet look (flat 3-tone) for comparison"""
    sys.path.insert(0,os.path.dirname(__file__))
    return None

ALL={'f22':f22,'f16':f16}
if __name__=='__main__':
    out=sys.argv[1] if len(sys.argv)>1 else '/tmp/bigart'
    os.makedirs(out,exist_ok=True)
    for k,f in ALL.items():
        a=f();to_img(compose(a),3).save(f'{out}/{k}_scene.png');to_img(a.idx,4).save(f'{out}/{k}_sprite.png')
