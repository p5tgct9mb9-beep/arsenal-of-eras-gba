"""Museum/hangar card helpers for DS vehicle sheets."""
from dslib import *
from hd import shadow
GLYPH['~']=['.....','.....','.##.#','#..#.','.....','.....','.....']
O=OLIVE
CW,CH=156,112          # card size (native)
SPW,SPH=CW-4,66        # sprite area
def fit_mask(T,parts,lines,dots,box_,align='c',maxsc=None):
    xs=[];ys=[]
    for p in parts:
        if 'circle' in p: cx,cy,r=p['circle']; xs+=[cx-r,cx+r]; ys+=[cy-r,cy+r]
        else: xs+=[a for a,b in p['poly']]; ys+=[b for a,b in p['poly']]
    x0,y0,x1,y1=box_; bw=max(xs)-min(xs); bh=max(ys)-min(ys)
    sc=min((x1-x0)/bw,(y1-y0)/bh)
    if maxsc: sc=min(sc,maxsc)
    ox=x0+(x1-x0-bw*sc)/2-min(xs)*sc; oy=y0+(y1-y0-bh*sc)/2-min(ys)*sc
    if align=='b': oy=y1-max(ys)*sc
    xf=Xf(ox,oy,sc); mask,ids=render2(T,parts,xf,lines=lines,dots=dots)
    return xf,mask
# ---------------- markings (pixel level)
def _star_pts(cx,cy,R,r=None,rot=-90):
    r=r or R*0.42; pts=[]
    for k in range(10):
        a=math.radians(rot+36*k); rr=R if k%2==0 else r
        pts.append((cx+rr*math.cos(a),cy+rr*math.sin(a)))
    return pts
def us_star(im,X,Y,R,style='ww2'):
    d=ImageDraw.Draw(im)
    blue=(30,40,96) if style!='lowvis' else (60,62,66); white=(236,236,232) if style!='lowvis' else (120,124,130)
    bw=R*1.1; bh=max(1,R*0.42)
    d.rectangle((X-R-bw-1,Y-bh-1,X+R+bw+1,Y+bh+1),fill=blue)
    d.rectangle((X-R-bw,Y-bh,X+R+bw,Y+bh),fill=white if style!='lowvis' else blue)
    if style=='post': d.line((X-R-bw+1,Y,X+R+bw-1,Y),fill=(190,30,30))
    d.ellipse((X-R-1,Y-R-1,X+R+1,Y+R+1),fill=blue)
    d.polygon(_star_pts(X,Y+0.4,R*0.95),fill=white)
def vstar(im,X,Y,R):
    d=ImageDraw.Draw(im); d.ellipse((X-R-1,Y-R-1,X+R+1,Y+R+1),outline=(226,226,214))
    d.polygon(_star_pts(X,Y+0.4,R*0.95),fill=(226,226,214))
def roundel(im,X,Y,R,yellow=True):
    d=ImageDraw.Draw(im)
    if yellow: d.ellipse((X-R-1,Y-R-1,X+R+1,Y+R+1),fill=(226,190,40))
    d.ellipse((X-R,Y-R,X+R,Y+R),fill=(30,40,110)); r2=R*0.66
    d.ellipse((X-r2,Y-r2,X+r2,Y+r2),fill=(236,236,230)); r3=R*0.36
    d.ellipse((X-r3,Y-r3,X+r3,Y+r3),fill=(190,34,34))
def finflash(im,X,Y,w,h):
    d=ImageDraw.Draw(im); s=max(1,w//3)
    d.rectangle((X,Y,X+s-1,Y+h),fill=(190,34,34)); d.rectangle((X+s,Y,X+2*s-1,Y+h),fill=(236,236,230)); d.rectangle((X+2*s,Y,X+3*s-1,Y+h),fill=(30,40,110))
def hinomaru(im,X,Y,R,border=True):
    d=ImageDraw.Draw(im)
    if border: d.ellipse((X-R-1,Y-R-1,X+R+1,Y+R+1),fill=(236,236,230))
    d.ellipse((X-R,Y-R,X+R,Y+R),fill=(196,30,30)); d.point((X-int(R*0.4),Y-int(R*0.4)),fill=(230,80,70))
def redstar(im,X,Y,R,border=(236,236,230)):
    d=ImageDraw.Draw(im)
    if border: d.polygon(_star_pts(X,Y,R+1.4,(R+1.4)*0.45),fill=border)
    d.polygon(_star_pts(X,Y,R,R*0.42),fill=(200,30,30))
def bcross(im,X,Y,a):
    """Balkenkreuz: black cross with white outline, arm half-length a"""
    d=ImageDraw.Draw(im); t=max(1,a//3)
    d.rectangle((X-a-1,Y-t-1,X+a+1,Y+t+1),fill=(236,236,230)); d.rectangle((X-t-1,Y-a-1,X+t+1,Y+a+1),fill=(236,236,230))
    d.rectangle((X-a,Y-t,X+a,Y+t),fill=(14,14,14)); d.rectangle((X-t,Y-a,X+t,Y+a),fill=(14,14,14))
def prop(im,X,Y,R,blades=None):
    P=im.load(); W,H=im.size
    for y in range(Y-R,Y+R+1):
        t=abs(y-Y)/max(1,R); w=1 if t>0.7 else 2
        for x in range(X-w,X+1):
            if 0<=x<W and 0<=y<H and (x+y)&1: P[x,y]=(186,186,178) if t<0.85 else (124,124,116)
    for y in (Y-R,Y+R):
        if 0<=y<H: P[X-1,y]=(60,60,56)
def rotor(im,X,Y,R,ry=3):
    """main-rotor blur: dithered translucent disc seen edge-on, motion-streaked blades"""
    P=im.load(); W,H=im.size
    for y in range(int(Y-ry),int(Y+ry)+1):
        for x in range(int(X-R),int(X+R)+1):
            e=((x-X)/R)**2+((y-Y)/max(ry,0.5))**2
            if e<=1 and 0<=x<W and 0<=y<H:
                core=abs(y-Y)<1
                if not core and (x+y)&1: continue
                P[x,y]=mix(P[x,y],(176,180,176) if core else (138,142,138),0.7 if core else 0.5)
    d=ImageDraw.Draw(im)
    for dx,ln in ((-0.95,9),(-0.4,7),(0.3,8),(0.85,9)):          # blade streaks (2 blades caught mid-sweep)
        x=int(X+dx*R)
        for k in range(ln):
            if 0<=x-k<W: P[x-k,Y]=mix((34,36,34),P[x-k,Y],k/ln)
    d.line((X-1,Y-1,X+1,Y-1),fill=(60,60,56))
def disc(im,X,Y,R):
    """small rotor (tail) blur disc face-on"""
    P=im.load(); W,H=im.size
    for y in range(Y-R,Y+R+1):
        for x in range(X-R,X+R+1):
            dd=math.hypot(x-X,y-Y)
            if dd<=R and 0<=x<W and 0<=y<H and (x+y)%3==0: P[x,y]=(170,172,168)
    d=ImageDraw.Draw(im); d.line((X-R+1,Y-R+1,X+R-1,Y+R-1),fill=(40,40,40)); d.line((X-R+1,Y+R-1,X+R-1,Y-R+1),fill=(60,60,60))
# ---------------- card
def bg_hangar(W,H):
    return bands(W,H,[(0,H,O[3],O[3],'solid'),(0,3,O[1],O[2],'checker'),(3,10,O[2],O[2],'solid'),(10,13,O[2],O[3],'checker'),
        (H-24,H-20,O[3],O[4],'checker'),(H-20,H-12,O[4],O[4],'solid'),(H-12,H-8,O[4],O[2],'checker'),(H-8,H,O[2],O[1],'checker')])
def bg_ground(W,H):
    im=bands(W,H,[(0,H,O[3],O[3],'solid'),(0,3,O[1],O[2],'checker'),(3,10,O[2],O[2],'solid'),(10,13,O[2],O[3],'checker')])
    P=im.load(); gy=H-9
    for y in range(gy,H):
        for x in range(W): P[x,y]=dith(0.65-0.5*(y-gy)/9+0.15*noise(x,y,3),x,y,[(30,28,20),(46,42,30),(62,58,40),(80,74,52),(96,90,64)])
    return im
def wrap(s,n):
    out=[]; cur=''
    for w in s.split(' '):
        if len(cur)+len(w)+(1 if cur else 0)>n: out.append(cur); cur=w
        else: cur=(cur+' '+w) if cur else w
    if cur: out.append(cur)
    return out
def card(S,x0,y0,v,land=False,sel=False):
    d=ImageDraw.Draw(S)
    T=(bg_ground if land else bg_hangar)(SPW,SPH)
    a=v['fn']()
    parts,lines,dots=a['parts'],a.get('lines',[]),a.get('dots',[])
    pad=v.get('pad',(6,7,6,6 if not land else 9))
    box_=(pad[0],pad[1],SPW-pad[2],SPH-pad[3])
    xf,mask=fit_mask(T,parts,lines,dots,box_,align='b' if land else 'c',maxsc=v.get('maxsc'))
    if land: shadow(T,mask,1,0.08,col=(24,22,16))
    else: shadow(T,mask,5,0.08,col=O[1])
    for m in a.get('marks',[]):
        k=m[0]; X,Y=xf.f(m[1],m[2]); X,Y=int(round(X)),int(round(Y)); s=m[3] if len(m)>3 else 4
        S_=max(2,int(round(s*xf.sc)))
        if k=='cross': bcross(T,X,Y,max(2,S_))
        elif k=='star' and len(m)>4 and m[4]=='ww2c': vstar(T,X,Y,max(2,S_))
        elif k=='star': us_star(T,X,Y,max(2,S_),m[4] if len(m)>4 else 'ww2')
        elif k=='roundel': roundel(T,X,Y,max(2,S_),m[4] if len(m)>4 else True)
        elif k=='flash': finflash(T,X,Y,max(3,S_),max(3,int(m[4]*xf.sc)))
        elif k=='hino': hinomaru(T,X,Y,max(2,S_))
        elif k=='redstar': redstar(T,X,Y,max(2,S_),m[4] if len(m)>4 else (236,236,230))
        elif k=='prop': prop(T,X,Y,max(3,S_))
        elif k=='rotor': rotor(T,X,Y,max(6,S_),m[4] if len(m)>4 else 2)
        elif k=='disc': disc(T,X,Y,max(3,S_))
    edge=GOLD if sel else O[5]
    d.rectangle((x0,y0,x0+CW-1,y0+CH-1),fill=O[0]); d.rectangle((x0+1,y0+1,x0+CW-2,y0+CH-2),fill=edge)
    d.rectangle((x0+2,y0+2,x0+CW-3,y0+CH-3),fill=(20,20,16))
    d.rectangle((x0+2,y0+2,x0+CW-3,y0+12),fill=O[0])
    text(S,x0+5,y0+4,v['name'],GOLD if sel else (222,220,206))
    S.paste(T,(x0+2,y0+13))
    ly=y0+13+SPH
    d.line((x0+2,ly,x0+CW-3,ly),fill=GOLD if sel else O[5])
    text(S,x0+5,ly+3,v['era'],(124,236,96)); rtext(S,x0+CW-6,ly+3,v['country'],(160,200,240))
    for i,l in enumerate(wrap(v['fact'],24)[:2]):
        text(S,x0+5,ly+13+i*9,l,(206,200,170))
def sheet(title,rows,path,land=False,cols=5,footer=''):
    PAD=5
    W=cols*CW+(cols+1)*PAD
    H=16+sum(12+CH+4 for r in rows)+14
    S=Image.new('RGB',(W,H),(16,16,12)); d=ImageDraw.Draw(S)
    d.rectangle((0,0,W-1,13),fill=O[0]); d.line((0,13,W-1,13),fill=O[5])
    text(S,6,3,title,GOLD); mockup_tag(S,x=W-44,y=1)
    y=16
    for ri,(label,sub,items) in enumerate(rows):
        text(S,PAD,y+2,label,(239,90,70) if False else (206,200,170)); text(S,PAD+tw(label)+8,y+2,sub,(150,148,118))
        y+=12
        for i,v in enumerate(items): card(S,PAD+i*(CW+PAD),y,v,land=land,sel=(ri==0 and i==0))
        y+=CH+4
    text(S,PAD,y+2,footer,(150,148,118))
    up(S,3).save(path); return S
