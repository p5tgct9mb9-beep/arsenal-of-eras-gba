"""Detailed-style renderer: shaded polygon parts, crisp dark outlines, dithered bands."""
from PIL import Image, ImageDraw
import math
BAYER=[[0,8,2,10],[12,4,14,6],[3,11,1,9],[15,7,13,5]]
INK=(10,10,8)
OLIVE=[(10,10,8),(23,23,18),(34,33,28),(50,49,41),(63,61,48),(78,76,60),(96,93,72)]
GREY=[(28,29,30),(44,45,47),(62,63,66),(82,83,86),(104,105,108),(126,126,129),(150,150,152),(174,174,175),(198,197,197)]
def dith(t,x,y,ramp):
    """t in 0..1 -> ramp colour with 4x4 ordered dithering between steps"""
    t=max(0.0,min(0.9999,t)); f=t*(len(ramp)-1); i=int(f); fr=f-i
    if i+1<len(ramp) and fr*16>BAYER[y&3][x&3]+0.5: i+=1
    return ramp[i]
def bands(W,H,spec):
    """spec: list of (y0,y1,colA,colB,mode) mode: 'solid'|'checker'|'sparse'|'grad'"""
    im=Image.new('RGB',(W,H),spec[0][2]); p=im.load()
    for (y0,y1,a,b,mode) in spec:
        for y in range(max(0,y0),min(H,y1)):
            for x in range(W):
                if mode=='solid': c=a
                elif mode=='checker': c=a if (x+y)&1 else b
                elif mode=='sparse': c=b if (x%4==(y*2)%4 and y%2==0) else a
                elif mode=='dash': c=b if ((x//2+y)%3==0) else a
                elif mode=='grad':
                    t=(y-y0)/max(1,(y1-y0)); c=a if t*16<BAYER[y&3][x&3]+0.5 else b
                    c=b if t*16>BAYER[y&3][x&3]+0.5 else a
                p[x,y]=c
    return im
class Xf:
    def __init__(s,ox,oy,sc=1.0,ang=0.0,flip=False):
        s.ox,s.oy,s.sc,s.a,s.flip=ox,oy,sc,math.radians(ang),flip
    def f(s,x,y):
        if s.flip: x=-x
        c,si=math.cos(s.a),math.sin(s.a)
        return (s.ox+(x*c+y*si)*s.sc, s.oy+(-x*si+y*c)*s.sc)
    def inv(s,X,Y):
        X=(X-s.ox)/s.sc; Y=(Y-s.oy)/s.sc; c,si=math.cos(s.a),math.sin(s.a)
        x=X*c-Y*si; y=X*si+Y*c
        return (-x if s.flip else x, y)
def render(im,parts,xf,outline=INK,lines=(),dots=()):
    """parts: dicts {poly, ramp, shade(lx,ly,u,v)->t, edge:bool}; u,v = local bbox-normalised coords.
    Later parts draw over earlier ones. Returns silhouette mask."""
    W,H=im.size
    ids=Image.new('L',(W,H),0); dI=ImageDraw.Draw(ids)
    for i,pt in enumerate(parts,1):
        if 'circle' in pt:
            cx,cy,r=pt['circle']; X,Y=xf.f(cx,cy); R=r*xf.sc
            dI.ellipse((X-R,Y-R,X+R-1,Y+R-1),fill=i)
        else:
            dI.polygon([xf.f(x,y) for x,y in pt['poly']],fill=i)
    for pt in parts:
        if 'circle' in pt:
            cx,cy,r=pt['circle']; pt['bb']=(cx-r,cy-r,cx+r,cy+r)
        else:
            xs=[a for a,b in pt['poly']]; ys=[b for a,b in pt['poly']]; pt['bb']=(min(xs),min(ys),max(xs),max(ys))
    P=im.load(); I=ids.load()
    colr={}
    for X in range(W):
        for Y in range(H):
            i=I[X,Y]
            if i:
                a=colr.get((i,X))
                colr[(i,X)]=(Y,Y) if a is None else (a[0],Y)
    for Y in range(H):
        for X in range(W):
            i=I[X,Y]
            if not i: continue
            pt=parts[i-1]; lx,ly=xf.inv(X+0.5,Y+0.5); x0,y0,x1,y1=pt['bb']
            u=(lx-x0)/max(1e-6,x1-x0); v=(ly-y0)/max(1e-6,y1-y0)
            if pt.get('col'):
                a,b=colr[(i,X)]; v=(Y-a)/max(1,b-a)
            t=pt['shade'](lx,ly,u,v) if 'shade' in pt else 1-v
            P[X,Y]=dith(t,X,Y,pt['ramp'])
    # inner edges: pixel whose neighbour belongs to a part drawn in front -> dark of own ramp
    out=im.copy(); O=out.load()
    for Y in range(H):
        for X in range(W):
            i=I[X,Y]
            if not i: continue
            pt=parts[i-1]
            for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
                xx,yy=X+dx,Y+dy
                if 0<=xx<W and 0<=yy<H:
                    j=I[xx,yy]
                    if j==0:
                        O[X,Y]=outline; break
                    if j>i and parts[j-1].get('edge',True) and pt.get('edge',True):
                        O[X,Y]=pt['ramp'][0]; break
    im.paste(out)
    d=ImageDraw.Draw(im)
    for (pts,col) in lines:
        q=[tuple(round(c) for c in xf.f(x,y)) for x,y in pts]
        for a,b in zip(q,q[1:]):
            for (X,Y) in _line(a,b):
                if 0<=X<W and 0<=Y<H and I[X,Y]: P2=im.load(); P2[X,Y]=col
    for (x,y,col) in dots:
        X,Y=xf.f(x,y); X,Y=int(X),int(Y)
        if 0<=X<W and 0<=Y<H: im.putpixel((X,Y),col)
    return ids.point(lambda v:255 if v else 0)
def _line(a,b):
    (x0,y0),(x1,y1)=a,b; n=max(abs(x1-x0),abs(y1-y0),1)
    return [(round(x0+(x1-x0)*k/n),round(y0+(y1-y0)*k/n)) for k in range(n+1)]
def shadow(im,mask,dy,squash=0.25,col=INK,dense=True):
    """soft dithered shadow: squashed copy of mask below the object"""
    W,H=im.size; bb=mask.getbbox()
    if not bb: return
    m=mask.crop(bb); h=max(2,int(m.height*squash)); m=m.resize((m.width,h))
    P=im.load(); M=m.load()
    for y in range(h):
        for x in range(m.width):
            if M[x,y]>128:
                X,Y=bb[0]+x,bb[3]+dy+y-h//2
                core = 0.25<y/h<0.75 and 0.08<x/m.width<0.92
                if 0<=X<W and 0<=Y<H and (core and dense or (X+Y)&1): P[X,Y]=col
# ---- shading helpers
def cyl(top=0.25,lo=0.05,hi=1.0):
    def f(lx,ly,u,v):
        return hi-(top-v)*1.6 if v<top else hi-(v-top)*(hi-lo)/(1-top)
    return f
def lin_v(a=1.0,b=0.0):
    return lambda lx,ly,u,v: a+(b-a)*v
def lin_u(a=1.0,b=0.0):
    return lambda lx,ly,u,v: a+(b-a)*u
def flat(t): return lambda *_: t
