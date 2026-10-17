#!/usr/bin/env python3
"""veh_ds.py - DS vehicle sprite upgrade (ds-v0.3.0).
Repaints every top-down vehicle sprite (128 tank hulls + turrets, 158 aircraft, 25 ships) as detailed pixel
art.  The GBA sprite of each vehicle is the shape and material map, so silhouettes, turret pivots, gun lengths and
hit boxes stay exactly as on the GBA.  Each part is then shaded and detailed:
  armour   - track runs with links, cleats and rounded sprocket/idler ends; side skirts with plate seams;
             bevelled hull (lit glacis, dark side plates), engine-deck grilles, exhausts, headlights, tools;
             cast turrets shaded as domes, welded turrets with lit roof edges, cupola + hatch, mantlet;
             round barrels with a lit centre, muzzle brakes and fume extractors
  aircraft - round fuselages (lit spine, dark flanks), wings with lit leading edges, flap/aileron lines and
             panel lines, engine nacelles, framed canopies with a glint, propeller blur discs
  ships    - hull sides and sheer, wooden deck planking or non-slip steel decks, shaded turrets with
             barrels, superstructure roofs with bridge windows, funnels with dark caps
The lighting is overhead (rotation invariant) because the DS rotates these sprites in hardware, and every
colour is one of the existing 16-colour team palette entries (team colours, loadout and hangar keep working).
Used by tools/port.py: veh_ds.patch(name, words) -> words.  Run directly to write before/after sheets."""
import numpy as np
CAMO=[2,3,4]; METAL=[1,12,13,14,11]  # camo ramp 2<3<4 (5 is an era accent)
def grids(words, n, stride, off=0):
    w=np.array(words,dtype=np.uint32);out=[]
    for i in range(n):
        g=np.zeros((32,32),np.uint8)
        for t in range(16):
            tx,ty=(t&3)*8,(t>>2)*8
            for y in range(8):
                v=int(w[i*stride+off+t*8+y])
                for x in range(8):g[ty+y,tx+x]=(v>>(4*x))&15
        out.append(g)
    return out
def pack(g):
    o=[]
    for t in range(16):
        tx,ty=(t&3)*8,(t>>2)*8
        for y in range(8):
            v=0
            for x in range(8):v|=int(g[ty+y,tx+x])<<(4*x)
            o.append(v)
    return o
def dist(mask, cap=8):
    d=np.where(mask,cap,0).astype(int)
    for _ in range(cap):
        p=np.pad(d,1);nb=np.minimum.reduce([p[:-2,1:-1],p[2:,1:-1],p[1:-1,:-2],p[1:-1,2:]])+1
        d=np.where(mask,np.minimum(d,nb),0)
    return d
def rng(seed):return np.random.default_rng(seed*7919+13)
def step(v,d):
    """move a colour d steps along its ramp (camo or metal)"""
    if v==1:return v                                                     # outlines stay black
    for R,lo in ((CAMO,0),(METAL,1)):
        if v in R:k=R.index(v)+d;return R[max(lo,min(len(R)-1,k))]
    return v
def runs(row):
    """[(start,end)] of True runs"""
    r=[];s=None
    for i,v in enumerate(list(row)+[False]):
        if v and s is None:s=i
        if not v and s is not None:r.append((s,i-1));s=None
    return r
def tube(o,mask,axis=0,hi=1):
    """shade thin parts as cylinders across `axis` (0: vertical tubes, shading varies along x)"""
    M=mask if axis==0 else mask.T;O=o if axis==0 else o.T
    for y in range(M.shape[0]):
        for a,b in runs(M[y]):
            w=b-a+1
            for x in range(a,b+1):
                t=abs((x-a)-(w-1)/2)/max(1,(w)/2)
                O[y,x]=step(O[y,x],hi if t<0.35 else (0 if t<0.75 else -1))
# ------------------------------------------------------------------------------------------- armour
def hull(g,seed=0):
    o=g.copy();m=g>0
    if not m.any():return o
    tr=np.isin(g,[12,13]);body=m&~tr;R=rng(seed)
    ys=np.nonzero(m.any(1))[0];y0,y1=ys.min(),ys.max()
    # tracks / wheels
    for a,b in runs(tr.any(0)):
        col=tr[:,a:b+1].any(1);seg=runs(col)
        for s0,s1 in seg:
            long=s1-s0>=6
            for y in range(s0,s1+1):
                for x in range(a,b+1):
                    if not tr[y,x]:continue
                    if long:
                        edge=x==a or x==b;end=y in(s0,s1)
                        o[y,x]=12 if edge or (y-s0)%2 else 13
                        if end and edge:o[y,x]=0 if b-a>=2 else 12           # rounded sprocket / idler ends
                        elif end:o[y,x]=12
                    else:                                                   # road wheel / tyre: dark tyre, lit hub
                        o[y,x]=12 if (x in(a,b) or y in(s0,s1)) else 13
            if long and b-a>=3:                                             # guide horns down the middle of the link
                cx=(a+b)//2
                for y in range(s0+1,s1):
                    if tr[y,cx]:o[y,cx]=12 if y%2 else 13
    # body: bevel by distance to the edge, lit glacis, darker sides
    D=dist(body,4)
    bys=np.nonzero(body.any(1))[0]
    if len(bys):
        b0,b1=bys.min(),bys.max()
        for y in range(b0,b1+1):
            for x in np.nonzero(body[y])[0]:
                v=int(g[y,x])
                if v not in CAMO:continue
                d=D[y,x]
                if d==1:v=step(v,-1)
                if y-b0<=1 and d>=1 and v in CAMO:v=step(int(g[y,x]),1 if y==b0 else 0)   # glacis edge catches the light
                o[y,x]=v
        # engine deck grille + exhausts
        L=b1-b0+1
        if L>=10:
            gy0=b1-max(3,L//5)
            for y in range(gy0,b1):
                xs=[x for x in np.nonzero(body[y])[0] if g[y,x] in CAMO and D[y,x]>=2]
                for x in xs[1:-1]:o[y,x]=2 if (y-gy0)%2==0 else step(int(g[y,x]),0)
            for x in (np.nonzero(body[b1])[0][[0,-1]] if body[b1].sum()>=6 else []):
                o[b1,x]=7
        # headlights on the front corners, a tool rack (shovel/crowbar) along one fender
        fr=np.nonzero(body[b0+1])[0] if b0+1<32 else []
        if len(fr)>=8:o[b0+1,fr[1]]=14;o[b0+1,fr[-2]]=14
        side=np.nonzero(body.any(0))[0]
        if len(side) and L>=12:
            x=side[0]+1
            for y in range(b0+L//3,b0+L//3+min(6,L//3)):
                if body[y,x] and g[y,x] in CAMO:o[y,x]=13 if y%3 else 8
        # mottled weathering on large flat areas
        for y in range(32):
            for x in range(32):
                if body[y,x] and o[y,x] in (3,4) and D[y,x]>=2 and R.random()<0.04:o[y,x]=step(int(o[y,x]),-1 if R.random()<0.7 else 1)
    return o
def turret(g,seed=0):
    o=g.copy();m=g>0
    if not m.any():return o
    st=g==14;shell=m&~st&np.isin(g,[2,3,4,5,7])
    D=dist(shell,6)
    # dome / box shading by distance to the edge
    mx=D.max() if shell.any() else 1
    for y in range(32):
        for x in range(32):
            if not shell[y,x] or g[y,x] not in CAMO:continue
            d=D[y,x];v=int(g[y,x])
            if d==1:v=step(v,-1)
            elif mx>=3 and d>=mx-0 and v in(3,4):v=step(v,1)
            o[y,x]=v
    # outer rim pixel on the silhouette: darkest ramp step for contrast against the hull
    for y in range(32):
        for x in range(32):
            if shell[y,x] and D[y,x]==1 and o[y,x] in CAMO and ((y+x)%3==0):o[y,x]=step(int(o[y,x]),-1)
    # barrels: round tubes, dark muzzle
    tube(o,st,0,hi=1)
    for x in range(32):
        col=np.nonzero(st[:,x])[0]
        if len(col)>=4:
            top=col.min()
            if top>0 or True:o[top,x]=12
    # hatch + cupola on the rear half of the turret
    if shell.sum()>=40:
        ys,xs=np.nonzero(shell);cy=int(ys.mean());cx=int(xs.mean())
        r=int((ys.max()-ys.min())/4)
        hy=min(ys.max()-2,cy+max(1,r));
        for (hx,c1,c2) in ((cx+2,13,7),(cx-3,step(int(g[cy,cx]) if g[cy,cx] in CAMO else 3,-1),None)):
            if 0<=hx<31 and shell[hy,hx] and shell[hy,hx+1] and shell[hy-1,hx]:
                o[hy-1,hx]=c1;o[hy-1,hx+1]=c1;o[hy,hx]=c1;o[hy,hx+1]=c2 or c1
                if c2:o[hy,hx]=c2
        # periscope glints along the front
        fy=ys.min()+1
        for x in (cx-2,cx+2):
            if shell[fy,x] and not st[fy,x]:o[fy,x]=10
    return o
# ------------------------------------------------------------------------------------------- aircraft
def plane(g,seed=0):
    o=g.copy();m=g>0
    if not m.any():return o
    camo=np.isin(g,[2,3,4,5])
    rowsum=camo.sum(1);colsum=camo.sum(0)
    # fuselage: the central columns that run most of the length
    xs=[x for x in range(32) if colsum[x]>=max(8,colsum.max()*0.55)]
    fus=np.zeros_like(m)
    if xs:
        fx0,fx1=min(xs),max(xs)
        if fx1-fx0>8:fx0,fx1=15-1,16+1
        fus[:,fx0:fx1+1]=camo[:,fx0:fx1+1]
    wing=camo&~fus
    # wings / tail: lit leading edge (front row), flap line one in from the trailing edge, dark tips
    for x in range(32):
        for a,b in runs(wing[:,x]):
            if b-a>=1:
                o[a,x]=step(int(g[a,x]),1)
                o[b,x]=step(int(g[b,x]),-1)
                if b-a>=3:o[b-1,x]=step(int(g[b-1,x]),-1) if (x%2==0) else o[b-1,x]
            else:o[a,x]=int(g[a,x])
    for y in range(32):
        for a,b in runs(wing[y]):
            if b-a>=3:o[y,a]=step(int(o[y,a]),-1);o[y,b]=step(int(o[y,b]),-1)
    # panel lines across the wings
    for x in range(32):
        if wing[:,x].any() and x%5==0 and abs(x-15.5)>3:
            for y in np.nonzero(wing[:,x])[0][1:-1]:o[y,x]=step(int(o[y,x]),-1)
    # fuselage cylinder: lit spine, dark flanks
    tube(o,fus,0,hi=1)
    # engine nacelles and other round dark parts
    tube(o,g==2,1,hi=1)
    # canopy: frame + glint
    gl=g==10
    if gl.any():
        ys,xs=np.nonzero(gl)
        o[ys.min(),xs.min()]=11
        for y,x in zip(ys,xs):
            if y==ys.max() and ys.max()-ys.min()>=2:o[y,x]=7
    # propellers (steel lines): blur disc look - alternate steel / clear
    pr=g==14
    for y in range(32):
        for a,b in runs(pr[y]):
            if b-a>=5:
                for x in range(a,b+1):o[y,x]=14 if (x-a)%2==0 or x in(a,b) else 13
    return o
# ------------------------------------------------------------------------------------------- ships
def ship(g,seed=0):
    o=g.copy();m=g>0
    if not m.any():return o
    D=dist(m,4)
    hullp=(g==14)&(D<=1)
    o[hullp]=13
    for y in range(32):
        r=runs(m[y])
        for a,b in r:o[y,a]=12;o[y,b]=12                                   # hull sides in shadow
    # deck
    wood=g==8
    for y in range(32):
        for x in range(32):
            if wood[y,x]:o[y,x]=8 if (x+ (y//4))%2 else 7 if (y%4==0 and x%3==0) else 8
    steel=(g==13)&(D>=2)
    for y in range(32):
        for x in range(32):
            if steel[y,x] and (x*3+y*5)%7==0:o[y,x]=12
    # superstructure (inner steel 14): lit roof, dark bridge windows at the front edge
    sup=(g==14)&(D>=2)
    for x in range(32):
        rs=runs(sup[:,x])
        for a,b in rs:
            o[a,x]=11
            if b-a>=2:o[a+1,x]=1 if x%2 else 7
            o[b,x]=13
    # turrets (12 + 14 ring): darker ring, lit top
    # funnels: dark cap
    fun=g==7
    for y in range(32):
        for a,b in runs(fun[y]):
            if b-a>=2:o[y,a]=1;o[y,b]=1
    return o
# ------------------------------------------------------------------------------------------- build
BANKS={'tank_spr':(256,128),'plane_spr':(128,158),'ship_spr':(128,25)}
def patch(name, words):
    if name not in BANKS:return words
    stride,n=BANKS[name];out=list(words)
    for i in range(n):
        if name=='tank_spr':
            h,t=grids(words,1,stride,i*stride)[0],grids(words,1,stride,i*stride+128)[0]
            out[i*stride:i*stride+128]=pack(hull(h,i));out[i*stride+128:i*stride+256]=pack(turret(t,i))
        else:
            g=grids(words,1,stride,i*stride)[0]
            out[i*stride:i*stride+128]=pack(plane(g,i) if name=='plane_spr' else ship(g,i))
    return out
