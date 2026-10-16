import sys,os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from hd import *
import hd, math, random
def render2(im,parts,xf,outline=INK,lines=(),dots=()):
    W,H=im.size
    ids=Image.new('L',(W,H),0); dI=ImageDraw.Draw(ids)
    for i,pt in enumerate(parts,1):
        if 'circle' in pt:
            cx,cy,r=pt['circle']; X,Y=xf.f(cx,cy); R=r*xf.sc
            dI.ellipse((X-R,Y-R,X+R-1,Y+R-1),fill=i)
            pt['bb']=(cx-r,cy-r,cx+r,cy+r)
        else:
            dI.polygon([xf.f(x,y) for x,y in pt['poly']],fill=i)
            xs=[a for a,b in pt['poly']]; ys=[b for a,b in pt['poly']]; pt['bb']=(min(xs),min(ys),max(xs),max(ys))
    P=im.load(); I=ids.load(); colr={}
    for X in range(W):
        for Y in range(H):
            i=I[X,Y]
            if i:
                a=colr.get((i,X)); colr[(i,X)]=(Y,Y) if a is None else (a[0],Y)
    for Y in range(H):
        for X in range(W):
            i=I[X,Y]
            if not i: continue
            pt=parts[i-1]; lx,ly=xf.inv(X+0.5,Y+0.5); x0,y0,x1,y1=pt['bb']
            u=(lx-x0)/max(1e-6,x1-x0); v=(ly-y0)/max(1e-6,y1-y0)
            if pt.get('col'):
                a,b=colr[(i,X)]; v=(Y-a)/max(1,b-a)
            t=pt['shade'](lx,ly,u,v) if 'shade' in pt else 1-v
            ramp=pt['rampf'](lx,ly) if 'rampf' in pt else pt['ramp']
            P[X,Y]=dith(t,X,Y,ramp)
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
                    if j==0: O[X,Y]=outline; break
                    if j>i and parts[j-1].get('edge',True) and pt.get('edge',True):
                        O[X,Y]=pt['ramp'][0]; break
    im.paste(out)
    for (pts,col) in lines:
        q=[tuple(round(c) for c in xf.f(x,y)) for x,y in pts]
        for a,b in zip(q,q[1:]):
            for (X,Y) in hd._line(a,b):
                if 0<=X<W and 0<=Y<H and I[X,Y]: P[X,Y]=col
    for (x,y,col) in dots:
        X,Y=xf.f(x,y); X,Y=int(X),int(Y)
        if 0<=X<W and 0<=Y<H: P[X,Y]=col
    return ids.point(lambda v:255 if v else 0), ids
# --- value noise for mottle
random.seed(7); _N={}
def _h(i,j):
    k=(i,j)
    if k not in _N: _N[k]=random.random()
    return _N[k]
def noise(x,y,s=5.0):
    x/=s; y/=s; i,j=math.floor(x),math.floor(y); fx,fy=x-i,y-j
    fx=fx*fx*(3-2*fx); fy=fy*fy*(3-2*fy)
    a=_h(i,j)+( _h(i+1,j)-_h(i,j))*fx; b=_h(i,j+1)+(_h(i+1,j+1)-_h(i,j+1))*fx
    return a+(b-a)*fy
def ramp(base,n=7,lo=0.35,hi=1.45):
    return [tuple(max(0,min(255,int(c*(lo+(hi-lo)*k/(n-1))))) for c in base) for k in range(n)]
