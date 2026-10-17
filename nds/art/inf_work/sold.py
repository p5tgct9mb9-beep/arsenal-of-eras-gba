import sys; sys.path.insert(0,__import__('os').path.join(__import__('os').path.dirname(__import__('os').path.abspath(__file__)),'..','rm_work')); sys.path.insert(0,__import__('os').path.join(__import__('os').path.dirname(__import__('os').path.abspath(__file__)),'..','ww2_work'))
from r2 import *
import math
from PIL import Image, ImageDraw
def mix(a,b,t): return tuple(int(a[i]*(1-t)+b[i]*t) for i in range(3))
def mul(a,k): return tuple(max(0,min(255,int(c*k))) for c in a)
# ---------------- palettes
SKIN_BASE=[(248,222,196),(234,192,156),(210,160,116),(176,122,82),(134,88,58),(96,62,42)]
def skin_ramp(b):
    sh=mix(mul(b,0.62),(120,40,40),0.12); hl=mix(b,(255,240,220),0.22)
    return [mul(sh,0.8),sh,mix(sh,b,0.5),b,mix(b,hl,0.5),hl]
SKINS=[skin_ramp(b) for b in SKIN_BASE]
HAIR={'BROWN':(96,62,36),'BLACK':(34,30,30),'BLOND':(214,176,96),'RED':(170,74,36),'GREY':(158,156,150)}
UNIF={'OLIVE DRAB':(92,96,58),'KHAKI':(160,140,96),'FIELD GREY':(104,110,98),'NAVY':(46,54,86),
      'BLACK':(46,46,50),'DESERT TAN':(184,158,112),'SNOW WHITE':(214,216,218),'URBAN GREY':(122,124,128)}
def R(b,lo=0.38,hi=1.3): return ramp(b,7,lo,hi)
def hr(b): return ramp(b,6,0.4,1.35)
LEATHER=R((70,50,34)); BLKB=R((40,38,36)); TANB=R((150,124,84))
GUNM=[(18,18,20),(34,34,38),(52,52,56),(72,72,76),(96,96,100),(124,124,126),(156,156,154)]
WOOD=[(40,24,14),(70,42,22),(100,62,32),(128,82,44),(156,104,60),(184,130,80)]
WEB=R((112,108,72)); WEBT=R((150,132,94)); WEBB=R((52,52,50))
# ---------------- camo: field -> layer 0..3
def camo_layer(kind,x,y):
    if kind=='WOODLAND':
        n=noise(x,y,3.0); m=noise(x+40,y+17,2.2)
        if m>0.72: return 3
        if n>0.62: return 1
        if n<0.30: return 2
        return 0
    if kind=='SPLINTER':
        k=math.floor((x*0.9+y*1.8+noise(x,y,4)*6+100)/5.0)%4
        return [0,1,0,2][k]
    if kind=='DIGITAL':
        c=noise(math.floor(x/1.2)*7.1,math.floor(y/1.2)*7.1,1.0)
        return 3 if c>0.78 else 1 if c>0.55 else 2 if c<0.18 else 0
    if kind=='TIGER STRIPE':
        s=math.sin(y*1.5+noise(x,y,5)*5+x*0.12)
        return 3 if s>0.88 else 2 if s>0.55 else 0
    return 0
def camo_ramps(base):
    l1=mix(mul(base,0.74),(70,84,46),0.35); l2=mix(mul(base,0.6),(92,66,42),0.45); l3=mul(base,0.32)
    return [R(base),R(l1),R(l2),R(l3,0.5,1.4)]
def cloth(base,kind):
    rs=camo_ramps(base)
    if kind=='SOLID': return dict(ramp=rs[0])
    return dict(ramp=rs[0],rampf=lambda x,y,rs=rs,k=kind: rs[camo_layer(k,x,y)])
# ---------------- geometry helpers
def seg(a,b,w0,w1=None):
    w1=w0 if w1 is None else w1
    dx,dy=b[0]-a[0],b[1]-a[1]; L=math.hypot(dx,dy) or 1; nx,ny=-dy/L,dx/L
    return [(a[0]+nx*w0,a[1]+ny*w0),(b[0]+nx*w1,b[1]+ny*w1),(b[0]-nx*w1,b[1]-ny*w1),(a[0]-nx*w0,a[1]-ny*w0)]
def along(M,B,s0,s1,w0,w1=None,off=0.0):
    dx,dy=B[0]-M[0],B[1]-M[1]; L=math.hypot(dx,dy); ux,uy=dx/L,dy/L; nx,ny=-uy,ux
    w1=w0 if w1 is None else w1
    p=lambda s,o:(M[0]+ux*s+nx*o,M[1]+uy*s+ny*o)
    return [p(s0,off-w0),p(s1,off-w1),p(s1,off+w1),p(s0,off+w0)]
def ell(cx,cy,rx,ry,n=14):
    return [(cx+rx*math.cos(2*math.pi*k/n),cy+ry*math.sin(2*math.pi*k/n)) for k in range(n)]
TL=lambda lx,ly,u,v:0.95-0.45*u-0.3*v
TR=lambda lx,ly,u,v:0.55+0.3*(1-u)-0.25*v
CYLU=lambda lx,ly,u,v:0.9-0.75*abs(u-0.35)*1.4
class Warp:
    """top-down 3/4: piecewise vertical squash (legs short, head big)"""
    def __init__(s,ox,oy,sc=1.0,knots=((0,0),(-23,-8),(-37,-17),(-62,-40)),flip=False):
        s.ox,s.oy,s.sc,s.flip=ox,oy,sc,flip; s.k=knots
    def g(s,y):
        k=s.k
        if y>=k[0][0]: return k[0][1]+(y-k[0][0])*(k[1][1]-k[0][1])/(k[1][0]-k[0][0])
        for (a,A),(b,B) in zip(k,k[1:]):
            if b<=y<=a: return A+(y-a)*(B-A)/(b-a)
        (a,A),(b,B)=k[-2],k[-1]; return A+(y-a)*(B-A)/(b-a)
    def gi(s,Y):
        k=s.k
        if Y>=k[0][1]: return k[0][0]+(Y-k[0][1])*(k[1][0]-k[0][0])/(k[1][1]-k[0][1])
        for (a,A),(b,B) in zip(k,k[1:]):
            if B<=Y<=A: return a+(Y-A)*(b-a)/(B-A)
        (a,A),(b,B)=k[-2],k[-1]; return a+(Y-A)*(b-a)/(B-A)
    def f(s,x,y):
        if s.flip: x=-x
        return (s.ox+x*s.sc, s.oy+s.g(y)*s.sc)
    def inv(s,X,Y):
        x=(X-s.ox)/s.sc; y=s.gi((Y-s.oy)/s.sc)
        return (-x if s.flip else x, y)
# ---------------- soldier spec
DEF=dict(skin=3,hair='BROWN',face='CLEAN',head='HELMET',unif='OLIVE DRAB',camo='SOLID',gear='WEBBING',build='STANDARD',
         rifle='M4',boots=None,gloves=None,team=None,helmcol=None,cover=None)
def spec(**k): d=dict(DEF); d.update(k); return d
def helm_ramp(s):
    if s['helmcol']: return R(s['helmcol'])
    return R(mix(UNIF.get(s['unif'],(92,96,58)),(70,76,50),0.5))
def rifle_parts(kind,M,B,side=False):
    """rifle along axis muzzle M -> butt B"""
    L=math.hypot(B[0]-M[0],B[1]-M[1]); P=[]; ln=[]
    met=GUNM; wood = WOOD if kind in ('GARAND','BOLT','THOMPSON','AK') else GUNM
    def a(s0,s1,w0,w1=None,off=0,ramp=met,sh=None):
        P.append(dict(poly=along(M,B,s0*L,s1*L,w0,w1,off),ramp=ramp,shade=sh or (lambda lx,ly,u,v:0.75-0.4*v)))
    if kind in ('GARAND','BOLT'):
        a(0,0.30,0.45); a(0.22,0.62,0.95,ramp=wood); a(0.42,0.62,0.85,0.95,-0.2,met)
        a(0.62,1.0,0.95,1.4,0.25,wood)
    elif kind=='THOMPSON':
        a(0,0.22,0.55); a(0.10,0.22,0.9,ramp=met); a(0.22,0.55,0.9,ramp=met); a(0.3,0.38,0.9,0.9,-1.6,wood)
        a(0.35,0.48,0.7,0.7,1.6,met); a(0.58,1.0,0.9,1.4,0.3,wood)
    elif kind=='AK':
        a(0,0.25,0.42); a(0.2,0.45,0.9,ramp=wood); a(0.45,0.66,1.0,ramp=met); a(0.5,0.62,0.7,0.6,2.0,met)
        a(0.66,1.0,0.9,1.3,0.3,wood)
    else: # M16 / M4 / SOF
        sup = kind=='SOF'
        a(0,0.22 if sup else 0.18,0.75 if sup else 0.4); a(0.18,0.46,1.0); a(0.46,0.68,1.05)
        a(0.5,0.62,0.75,0.65,2.2)                       # magazine
        if kind in ('M4','SOF'):
            a(0.44,0.58,0.65,0.65,-1.5,met)             # optic
        a(0.68,1.0,0.75,1.15,0.3)                       # stock
    return P
def front(s, view='F'):
    """returns parts, lines(after), feats(list of (lx,ly,w,h,col)) in 48-unit space. feet at y=0"""
    bx=0.86 if s['build']=='SLIM' else 1.0
    sk=SKINS[s['skin']-1]; hc=hr(HAIR[s['hair']]); cl=cloth(UNIF[s['unif']],s['camo'])
    base=UNIF[s['unif']]; dk=R(base)[1]
    boots=s['boots'] or (TANB if s['unif'] in ('DESERT TAN','KHAKI') else BLKB if s['unif'] in ('BLACK','NAVY','URBAN GREY','SNOW WHITE') else LEATHER)
    webr={'DESERT TAN':WEBT,'KHAKI':WEBT,'BLACK':WEBB,'NAVY':WEBB}.get(s['unif'],WEB)
    if s['gear']=='PLATE': webr=R(mix(base,(80,82,62),0.45))
    X=lambda pts:[(x*bx,y) for x,y in pts]
    P=[]; L=[]; F=[]
    def add(poly,body=True,**k): P.append(dict(poly=X(poly) if body else poly,**k))
    def C(**k): d=dict(cl); d.update(k); return d
    back = view=='B'
    # behind-body items
    if s['gear']=='RADIO':
        add([(8.4,-37),(9.2,-37),(10.8,-60),(10.2,-60)],ramp=GUNM,shade=flat(0.35),edge=False)  # antenna
        if not back:
            add([(-10,-38),(10,-38),(10.5,-22),(-10.5,-22)],ramp=webr,shade=TL)
    if s['face']=='LONG' and not back and s['head']!='BOONIE':
        add([(2.6,-43),(5.8,-42),(6.2,-36),(3.4,-36.5)],body=False,ramp=hc,shade=lin_u(0.8,0.3))
    # legs + boots
    for sgn in (-1,1):
        add([(sgn*0.6,-23),(sgn*7,-23),(sgn*6.6,-5),(sgn*1.2,-5)],**C(shade=(lin_u(0.95,0.35) if sgn<0 else lin_u(0.65,0.2))))
        add([(sgn*0.9,-6.5),(sgn*7,-6.5),(sgn*7.6,0),(sgn*0.8,0)],ramp=boots,shade=lambda lx,ly,u,v:0.85-0.7*v)
    L+= [([( -5*bx,-15),(-2*bx,-13)],dk),([(5*bx,-15),(2.5*bx,-13)],dk),([(-4*bx,-10),(-2*bx,-9)],dk),([(4*bx,-10),(2*bx,-9)],dk),
         ([(-0.6*bx,-23),(-1.2*bx,-6)],dk),([(0.6*bx,-23),(1.2*bx,-6)],dk)]
    # torso
    add([(-8.6,-37.5),(8.6,-37.5),(8.2,-30),(7.2,-23),(-7.2,-23),(-8.2,-30)],**C(shade=TL))
    if not back:
        L+= [([(0,-36),(0,-24)],dk),([(-6*bx,-33),(-2.5*bx,-33)],dk),([(2.5*bx,-33),(6*bx,-33)],dk)]
    # belt
    add([(-7.4,-24.5),(7.4,-24.5),(7.4,-22.5),(-7.4,-22.5)],ramp=webr,shade=flat(0.45))
    # gear
    g=s['gear']
    if g=='WEBBING':
        if back: add([(-5,-37.5),(-4,-37.5),(1,-30),(4,-37.5),(5,-37.5),(1.2,-28.5),(1.2,-24.5),(-1.2,-24.5),(-1.2,-28.5)],ramp=webr,shade=flat(0.6))
        else:
            add([(-5.6,-37.5),(-4.2,-37.5),(-3.6,-24.5),(-5,-24.5)],ramp=webr,shade=flat(0.62))
            add([(4.2,-37.5),(5.6,-37.5),(5,-24.5),(3.6,-24.5)],ramp=webr,shade=flat(0.5))
        for x0 in ((-6.6,-3.4),(3.4,6.6)):
            add([(x0[0],-26),(x0[1],-26),(x0[1],-21.5),(x0[0],-21.5)],ramp=webr,shade=lin_v(0.85,0.3))
    if g=='PLATE':
        add([(-7.6,-37),(-3.6,-37.5),(-2.4,-35.5),(2.4,-35.5),(3.6,-37.5),(7.6,-37),(7.6,-25),(-7.6,-25)],ramp=webr,shade=TL)
        if not back:
            for i in range(3):
                x=-5.4+i*3.8
                add([(x-1.5,-30.5),(x+1.5,-30.5),(x+1.5,-25.5),(x-1.5,-25.5)],ramp=webr,shade=lin_v(0.95,0.35))
            L+= [([(-7*bx,-33),(7*bx,-33)],webr[1])]
        else:
            add([(-4,-35),(4,-35),(4,-29),(-4,-29)],ramp=webr,shade=lin_v(0.9,0.3))
    if g=='RADIO':
        if back:
            add([(-7.6,-38),(7.6,-38),(8,-23),(-8,-23)],ramp=GUNM[1:],shade=TL)
            add([(-6,-35),(6,-35),(6,-30),(-6,-30)],ramp=GUNM,shade=flat(0.2))
            for i in range(4): F.append(((-5+i*2.6)*bx,-33.5,1,1,(96,180,96) if i==0 else (40,40,40)))
            add([(-7,-27),(7,-27),(7,-24),(-7,-24)],ramp=webr,shade=flat(0.5))
        else:
            add([(-5.6,-37.5),(-4.2,-37.5),(-3.6,-24.5),(-5,-24.5)],ramp=webr,shade=flat(0.62))
            add([(4.2,-37.5),(5.6,-37.5),(5,-24.5),(3.6,-24.5)],ramp=webr,shade=flat(0.5))
            add([(-6.4,-33.5),(-3.6,-33.5),(-3.6,-29.5),(-6.4,-29.5)],ramp=GUNM[1:],shade=lin_v(0.9,0.3))   # handset
            add([(3.4,-26),(6.6,-26),(6.6,-21.5),(3.4,-21.5)],ramp=webr,shade=lin_v(0.85,0.3))
    # neck + head
    add([(-2.2,-40),(2.2,-40),(2.4,-36.5),(-2.4,-36.5)],body=False,ramp=sk,shade=flat(0.15 if not back else 0.35))
    head=[(-4.2,-47),(4.2,-47),(4.8,-43),(4.2,-40.6),(2.4,-39),(-2.4,-39),(-4.2,-40.6),(-4.8,-43)]
    for sgn in (-1,1): add([(sgn*4.4,-44),(sgn*5.6,-43.6),(sgn*5.4,-41.4),(sgn*4.4,-41.2)],body=False,ramp=sk,shade=flat(0.35))
    add(head,body=False,ramp=sk,shade=lambda lx,ly,u,v:0.9-0.5*u-0.25*v if not back else 0.5-0.3*u)
    hd_=s['head']
    hair_top = hd_ in ('NONE','BERET','CAP') or (hd_=='BOONIE')
    if back:
        add([(-4.8,-47.6),(4.8,-47.6),(5,-43),(4.2,-40.6),(-4.2,-40.6),(-5,-43)],body=False,ramp=hc,shade=lin_u(0.85,0.3))
        if s['face']=='LONG': add(ell(0,-42.5,2.4,2.2),body=False,ramp=hc,shade=lin_v(0.95,0.35))
    else:
        if hd_=='NONE':
            add([(-4.8,-47.8),(-2,-48.8),(2.5,-48.8),(4.8,-47.6),(5,-44),(4.3,-43.6),(3.5,-45.6),(-1,-45.8),(-3.5,-45.4),(-4.3,-43.4),(-5,-44)],body=False,ramp=hc,shade=lin_v(0.95,0.4))
        elif hd_ in ('BERET','CAP','BOONIE'):
            for sgn in (-1,1): add([(sgn*4.9,-46.4),(sgn*3.8,-46.4),(sgn*4.1,-43.6),(sgn*4.9,-43.2)],body=False,ramp=hc,shade=flat(0.45))
        if s['face']=='LONG':
            for sgn in (-1,1): add([(sgn*5.0,-46),(sgn*4.0,-46),(sgn*4.1,-41.4),(sgn*5.0,-41.2)],body=False,ramp=hc,shade=flat(0.55))
    # headgear
    hr_=helm_ramp(s)
    if hd_=='HELMET':
        add([(-5.4,-43.2),(-5.6,-46.5),(-4,-49.5),(-1.4,-50.6),(1.4,-50.6),(4,-49.5),(5.6,-46.5),(5.4,-43.2),(4.2,-44.6),(-4.2,-44.6)],body=False,ramp=hr_,shade=lambda lx,ly,u,v:0.95-0.55*u-0.35*v)
        if not back and s.get('nvg'): add([(-1.3,-49.6),(1.3,-49.6),(1.3,-47.4),(-1.3,-47.4)],body=False,ramp=GUNM,shade=flat(0.4))
        if s.get('ears'):
            for sgn in (-1,1): add([(sgn*4.3,-44.4),(sgn*6.2,-44.4),(sgn*6.2,-41),(sgn*4.3,-41)],body=False,ramp=GUNM,shade=flat(0.45))
    elif hd_=='M1':
        add([(-6.2,-44),(-5.4,-47.4),(-3.6,-49.8),(0,-50.6),(3.6,-49.8),(5.4,-47.4),(6.2,-44),(5,-44.6),(-5,-44.6)],body=False,ramp=hr_,shade=lambda lx,ly,u,v:0.95-0.55*u-0.35*v)
        if not back: L.append(([(-2,-42),(-1,-40),(1,-40),(2,-42)],hr_[1]))
    elif hd_=='STAHL':
        add([(-6.4,-41.4),(-5.6,-44.6),(-5,-48),(-3,-50.2),(0,-50.8),(3,-50.2),(5,-48),(5.6,-44.6),(6.4,-41.4),(4.6,-42.2),(4.2,-44.4),(-4.2,-44.4),(-4.6,-42.2)],body=False,ramp=hr_,shade=lambda lx,ly,u,v:0.95-0.55*u-0.3*v)
    elif hd_=='BRODIE':
        add([(-8.4,-45),(-4,-46.4),(-3,-49.6),(0,-50.4),(3,-49.6),(4,-46.4),(8.4,-45),(7.6,-44),(-7.6,-44)],body=False,ramp=hr_,shade=lambda lx,ly,u,v:0.95-0.55*u-0.35*v)
    elif hd_=='BOONIE':
        bc=cloth(base,s['camo'])
        add([(-4.4,-46),(-4,-49.4),(-1.4,-50.2),(1.4,-50.2),(4,-49.4),(4.4,-46)],body=False,**dict(bc,shade=TL))
        add([(-7.8,-45),(-5,-46.4),(5,-46.4),(7.8,-45),(7.4,-43.8),(4.4,-44.6),(-4.4,-44.6),(-7.4,-43.8)],body=False,**dict(bc,shade=lin_u(0.9,0.35)))
        L.append(([(-4.2,-46.6),(4.2,-46.6)],R(base)[1]))
    elif hd_=='BERET':
        brr=R(s.get('beretcol') or (110,30,36))
        add([(-5,-45.4),(-4.6,-48.4),(-1,-50),(3,-50),(6.4,-48.4),(6.8,-46.4),(5,-45.4)],body=False,ramp=brr,shade=lambda lx,ly,u,v:0.95-0.55*u-0.3*v)
        add([(-5,-46),(5,-46),(5,-44.8),(-5,-44.8)],body=False,ramp=LEATHER,shade=flat(0.3))
        F.append((-3,-47.6,1.4,1.4,(214,184,80)))
    elif hd_=='CAP':
        cc=cloth(base,s['camo'])
        add([(-4.8,-45),(-4.6,-49.4),(4.6,-49.4),(4.8,-45)],body=False,**dict(cc,shade=TL))
        add([(-4.8,-45.4),(4.8,-45.4),(4.2,-43.8),(-4.2,-43.8)],body=False,ramp=R(base)[:4],shade=flat(0.3))
    # rifle + arms
    if back:
        M,B=(7.5,-49),(-6,-21)
        P.extend([dict(poly=X(p['poly']),ramp=p['ramp'],shade=p['shade']) for p in rifle_parts(s['rifle'],M,B)])
        add(seg((7,-38),(-7,-24),0.5),ramp=webr,shade=flat(0.4))
        for sgn in (-1,1):
            add(seg((sgn*8.4,-36.5),(sgn*9.8,-28),1.9,1.7),**C(shade=lin_u(0.8,0.4)))
            add(seg((sgn*9.8,-28.5),(sgn*9.6,-22.5),1.7,1.5),**C(shade=lin_u(0.7,0.3)))
            gl=s['gloves']
            add(ell(sgn*9.6*1.0,-21.4,1.7,1.7,10),ramp=gl or sk,shade=flat(0.45))
    else:
        M,B=(-10.5,-46),(8,-19.5)
        for sgn in (-1,1): add(seg((sgn*8.2,-36.6),(sgn*9.8,-29),2.1,1.8),**C(shade=(lin_u(0.95,0.45) if sgn<0 else lin_u(0.6,0.25))))
        P.extend([dict(poly=X(p['poly']),ramp=p['ramp'],shade=p['shade']) for p in rifle_parts(s['rifle'],M,B)])
        gl=s['gloves']
        add(seg((-9.8,-29.4),(-4.6,-34.6),1.8,1.5),**C(shade=lin_v(0.85,0.45)))
        add(seg((9.8,-29.4),(3.6,-27.8),1.8,1.5),**C(shade=lin_v(0.65,0.3)))
        add(ell(-3.6,-35.2,1.9,1.8,10),ramp=gl or sk,shade=lambda lx,ly,u,v:0.95-0.6*v)
        add(ell(2.6,-27.6,1.9,1.8,10),ramp=gl or sk,shade=lambda lx,ly,u,v:0.8-0.6*v)
    # face features
    if not back:
        eye=(30,22,20); wh=mix(sk[5],(240,236,228),0.6); brow=mul(HAIR[s['hair']],0.7) if s['hair']!='GREY' else (110,108,104)
        if sum(HAIR[s['hair']])<130: brow=(22,18,18)
        shd=sk[1]
        F+= [(-2.6,-43.8,1,1,wh,'hi'),(-1.6,-43.8,1,1,eye),(1.6,-43.8,1,1,wh,'hi'),(0.6,-43.8,1,1,eye),
             (-2.8,-45.5,2.2,0.6,brow,'br'),(0.6,-45.5,2.2,0.6,brow,'br'),(-0.4,-42.4,1,1.2,shd),(-1.2,-40.6,2.4,0.7,mix(sk[1],(120,40,40),0.25))]
        if s['face']=='STUBBLE':
            sc=mix(sk[2],HAIR[s['hair']],0.55) if s['skin']<5 else mix(sk[2],(30,24,22),0.4)
            mouth=F.pop(); F.append((-3.4,-41.4,6.8,2.4,sc,'sd')); F.append(mouth)
    return P,L,F
def side(s):
    """profile facing +x, rifle at low ready"""
    bx=0.86 if s['build']=='SLIM' else 1.0
    sk=SKINS[s['skin']-1]; hc=hr(HAIR[s['hair']]); cl=cloth(UNIF[s['unif']],s['camo']); base=UNIF[s['unif']]; dk=R(base)[1]
    boots=s['boots'] or (TANB if s['unif'] in ('DESERT TAN','KHAKI') else BLKB if s['unif'] in ('BLACK','NAVY','URBAN GREY','SNOW WHITE') else LEATHER)
    webr={'DESERT TAN':WEBT,'KHAKI':WEBT,'BLACK':WEBB,'NAVY':WEBB}.get(s['unif'],WEB)
    if s['gear']=='PLATE': webr=R(mix(base,(80,82,62),0.45))
    X=lambda pts:[(x*bx,y) for x,y in pts]
    P=[];L=[];F=[]
    def add(poly,body=True,**k): P.append(dict(poly=X(poly) if body else poly,**k))
    def C(**k): d=dict(cl); d.update(k); return d
    if s['gear']=='RADIO':
        add([(-4.6,-37),(-4.0,-37),(-6.6,-60),(-7.2,-60)],ramp=GUNM,shade=flat(0.35),edge=False)
    add(seg((0,-23),(-3.5,-5),2.8,2.4),**C(shade=lin_u(0.5,0.2)))          # far leg back
    add([(-6.4,-6.5),(-1.4,-6.5),(1.0,-1.8),(0.6,0),(-6.4,0)],ramp=boots,shade=lambda lx,ly,u,v:0.6-0.5*v)
    add(seg((0,-23),(3.6,-5),2.8,2.4),**C(shade=lin_u(0.9,0.4)))           # near leg forward
    add([(0.4,-6.5),(5.4,-6.5),(8.4,-2),(8.2,0),(0.6,0)],ramp=boots,shade=lambda lx,ly,u,v:0.9-0.7*v)
    if s['gear']=='RADIO': add([(-9.6,-38),(-3.6,-38),(-3.6,-23.5),(-9,-23.5)],ramp=GUNM[1:],shade=TL)
    add([(-4.6,-37.6),(3.4,-37.6),(4.6,-32),(4.2,-23),(-3.8,-23),(-4.6,-30)],**C(shade=TL))
    add([(-4.2,-24.5),(4.6,-24.5),(4.6,-22.5),(-4.2,-22.5)],ramp=webr,shade=flat(0.45))
    if s['gear']=='PLATE': add([(-5.6,-37),(4.8,-37),(5.6,-25),(-5.4,-25)],ramp=webr,shade=TL); add([(4.4,-31),(6.4,-31),(6.4,-25.5),(4.4,-25.5)],ramp=webr,shade=lin_v(0.9,0.3))
    if s['gear'] in ('WEBBING','RADIO'): add([(1.2,-37.6),(2.6,-37.6),(2.4,-24.5),(1.0,-24.5)],ramp=webr,shade=flat(0.6)); add([(2,-26),(5.4,-26),(5.4,-21.5),(2,-21.5)],ramp=webr,shade=lin_v(0.85,0.3))
    add([(-1.4,-40),(2.2,-40),(2.4,-36.5),(-1.6,-36.5)],body=False,ramp=sk,shade=flat(0.2))
    add([(-4.2,-47),(3.6,-47),(4.4,-44.4),(5.4,-42.8),(4.4,-42.2),(4.2,-40.2),(2.2,-39),(-1.6,-39.2),(-3.6,-41),(-4.4,-44)],body=False,ramp=sk,shade=lin_u(0.4,0.95))
    add(ell(-0.6,-42.8,1.2,1.5,8),body=False,ramp=sk,shade=flat(0.4))
    hd_=s['head']; hr_=helm_ramp(s)
    stahl=hd_=='STAHL'; brodie=hd_=='BRODIE'
    if hd_ in ('M1','STAHL','BRODIE'): hd_='HELMET'
    if hd_ in ('NONE','BERET','CAP','BOONIE'):
        add([(-4.8,-47.8),(0,-49),(3.8,-48),(3.6,-46),(0.6,-45.6),(-1.6,-44.4),(-1.8,-42.4),(-3.6,-41.6),(-4.8,-44)],body=False,ramp=hc,shade=lin_v(0.95,0.4))
        if s['face']=='LONG': add(ell(-5.2,-44.6,1.9,1.9,10),body=False,ramp=hc,shade=lin_v(0.9,0.3))
    if hd_=='HELMET':
        add([(-5.6,-42.6),(-5.6,-46.8),(-3.6,-49.8),(0,-50.6),(3,-49.6),(4.8,-46.8),(4.8,-45.4),(-1,-45),(-2.4,-43.4)],body=False,ramp=hr_,shade=lambda lx,ly,u,v:0.6+0.35*u-0.3*v)
        if stahl: add([(-5.8,-46),(-1.2,-45.4),(-1.6,-42.4),(-6.2,-40.8)],body=False,ramp=hr_,shade=flat(0.45))
        if brodie: add([(-7.6,-45.2),(7,-46),(6.6,-44.8),(-7.4,-44)],body=False,ramp=hr_,shade=flat(0.6))
        if s.get('ears'): add(ell(-0.6,-42.8,1.8,2.0,8),body=False,ramp=GUNM,shade=flat(0.45))
        if s.get('nvg'): add([(3.6,-49),(5.6,-49),(5.6,-47.2),(3.6,-47.2)],body=False,ramp=GUNM,shade=flat(0.4))
    elif hd_=='BOONIE':
        bc=cloth(base,s['camo'])
        add([(-4.2,-46),(-3.8,-49.4),(0,-50.4),(3.6,-49.4),(4,-46)],body=False,**dict(bc,shade=TL))
        add([(-8,-44.6),(-4,-46.4),(4,-46.4),(7.6,-45.2),(7.4,-44),(-7.6,-43.6)],body=False,**dict(bc,shade=lin_u(0.5,0.95)))
    elif hd_=='BERET':
        add([(-4.8,-45.4),(-4.8,-48.4),(-1,-50),(3.4,-49.4),(4.4,-46.6),(2,-45.4)],body=False,ramp=R(s.get('beretcol') or (110,30,36)),shade=TL)
    elif hd_=='CAP':
        cc=cloth(base,s['camo'])
        add([(-4.6,-45),(-4.4,-49.4),(3.6,-49.4),(3.8,-45.4)],body=False,**dict(cc,shade=TL))
        add([(2,-45.8),(6.8,-45.2),(6.4,-44.2),(2,-44.6)],body=False,ramp=R(base)[:4],shade=flat(0.3))
    M,B=(19,-27),(2.2,-33.5)
    P.extend([dict(poly=X(p['poly']),ramp=p['ramp'],shade=p['shade']) for p in rifle_parts(s['rifle'],M,B)])
    gl=s['gloves']
    add(seg((0.8,-36.6),(1.6,-29),2.2,1.9),**C(shade=lin_u(0.6,0.95)))
    add(seg((1.6,-29.4),(9,-29.6),1.8,1.5),**C(shade=lin_v(0.95,0.45)))
    add(ell(9.8,-29.4,1.8,1.7,10),ramp=gl or sk,shade=lin_v(0.95,0.4))
    add(ell(4.4,-30.6,1.8,1.7,10),ramp=gl or sk,shade=lin_v(0.95,0.4))
    eye=(30,22,20); brow=(22,18,18) if sum(HAIR[s['hair']])<130 else mul(HAIR[s['hair']],0.7)
    F+=[(2.6,-43.8,1,1,eye),(2,-45.1,2.2,0.8,brow),(3.0,-40.6,1.4,0.7,mix(sk[1],(120,40,40),0.25))]
    if s['face']=='STUBBLE':
        sc=mix(sk[2],HAIR[s['hair']],0.55) if s['skin']<5 else mix(sk[1],(20,16,16),0.4)
        F.insert(len(F)-1,(-0.4,-42.2,4.6,3.2,sc,'sd'))
    return P,L,F
def draw(im,s,xf,view='F',shadow_=True,feats=True,shcol=INK):
    P,L,F=(side(s) if view=='S' else front(s,view))
    if view=='S' and getattr(xf,'flip',False): pass
    mask,ids=render2(im,P,xf,lines=L)
    if shadow_: shadow(im,mask,1,0.12,col=shcol)  # placeholder small
    if feats:
        Pp=im.load(); I=ids.load(); W,H=im.size
        for f in F:
            x,y,w,h,col=f[:5]; tag=f[5] if len(f)>5 else ''
            if tag in ('hi','br') and xf.sc<1.4: continue
            if tag=='st' and xf.sc<1.0: continue
            X0,Y0=xf.f(x,y); X1,Y1=xf.f(x+w,y+h)
            xa,xb=sorted((X0,X1)); ya,yb=sorted((Y0,Y1))
            xa,ya=int(math.floor(xa+0.25)),int(math.floor(ya+0.25)); xb=max(xa+1,int(round(xb))); yb=max(ya+1,int(round(yb)))
            if tag=='st':
                xb=xa+1; yb=ya+1
            for Y in range(ya,yb):
                for X in range(xa,xb):
                    if 0<=X<W and 0<=Y<H and I[X,Y]: Pp[X,Y]=col
    return mask
def sprite(s,sc=1.0,view='F',bg=None,pad=4,warp=False,flip=False,feats=True):
    """render on its own canvas; returns RGBA image with feet anchor (ax,ay)"""
    Wd=int(48*sc)+2*pad; Ht=int(64*sc)+2*pad
    key=(1,2,3)
    im=Image.new('RGB',(Wd,Ht),key)
    xf=(Warp(Wd/2,Ht-pad,sc,flip=flip) if warp else Xf(Wd/2,Ht-pad,sc,0,flip))
    P,L,F=(side(s) if view=='S' else front(s,view))
    mask,ids=render2(im,P,xf,lines=L)
    Pp=im.load(); I=ids.load()
    if feats:
        for f in F:
            x,y,w,h,col=f[:5]; tag=f[5] if len(f)>5 else ''
            if tag in ('hi','br') and sc<1.4: continue
            if tag=='st' and sc<0.95: continue
            X0,Y0=xf.f(x,y); X1,Y1=xf.f(x+w,y+h)
            xa,xb=sorted((X0,X1)); ya,yb=sorted((Y0,Y1))
            xa,ya=int(math.floor(xa+0.25)),int(math.floor(ya+0.25)); xb=max(xa+1,int(round(xb))); yb=max(ya+1,int(round(yb)))
            if tag=='st' and sc<2: xb=xa+1; yb=ya+1
            for Y in range(ya,yb):
                for X in range(xa,xb):
                    if 0<=X<Wd and 0<=Y<Ht and I[X,Y] and (tag!='sd' or (((X+2*Y)%3==0) if sc<2.0 else ((X+Y)&1 and Y&1))): Pp[X,Y]=col
    rgba=im.convert('RGBA'); A=rgba.load(); M=mask.load()
    for y in range(Ht):
        for x in range(Wd):
            if not M[x,y]: A[x,y]=(0,0,0,0)
    return rgba,(Wd//2,Ht-pad),mask
def place(dst,spr,x,y,shadow_col=None,sw=None):
    """paste sprite so feet anchor at x,y; optional dithered ground shadow"""
    im,(ax,ay),mask=spr
    if shadow_col is not None:
        bb=mask.getbbox(); w=sw or (bb[2]-bb[0])
        P=dst.load(); W,H=dst.size
        for yy in range(-1,2):
            for xx in range(-w//2,w//2+1):
                e=(xx/(w/2))**2+(yy/1.6)**2
                X,Y=x+xx,y+yy
                if e<1 and 0<=X<W and 0<=Y<H and (e<0.5 or (X+Y)&1): P[X,Y]=shadow_col
    dst.paste(im,(x-ax,y-ay),im)
