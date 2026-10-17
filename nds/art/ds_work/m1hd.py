"""High-fidelity M1 Garand + WW2 kit pixel art. Units: inches, right side view, butt at x=0, muzzle at x=43.6, bore line y=0 (down +)."""
from cardlib import *
import math
def RP(c,lo=0.34,hi=1.36,n=8): return ramp(c,n,lo,hi)
WAL=RP((108,60,32),0.3,1.3); WALD=RP((104,58,32))
PARK=[(14,15,13),(26,28,24),(40,43,37),(56,59,51),(72,76,66),(90,94,82),(112,116,102),(140,144,128)]
BLUED=RP((60,62,62),0.3,1.5)
LEA=RP((120,70,36),0.32,1.3); BRASS=RP((176,140,60),0.4,1.3)
def p(poly,ramp,shade=None,**k): return dict(poly=poly,ramp=ramp,shade=shade or cyl(0.3,0.1,0.95),**k)
def C(x,y,r,ramp,shade=None,**k): return dict(circle=(x,y,r),ramp=ramp,shade=shade or (lambda lx,ly,u,v:1.0-0.8*v-0.1*u),**k)
def grain(base=0.62,amp=0.16):
    def f(lx,ly,u,v):
        g=math.sin(ly*5.5+lx*0.18+noise(lx*0.5,ly*1.5,2.2)*6.0)
        g2=1 if math.sin(ly*9.0+lx*0.25+noise(lx*0.35,ly*2,1.6)*9.0)>0.86 else 0
        return base+0.26*(1-v)-0.24*v+amp*g*0.6-0.16*g2+0.05*noise(lx*3,ly*3,1)
    return f
def garand():
    P=[]; L=[]; D=[]
    # ---- M1907 leather sling (behind stock), sagging between rear swivel (stock) and front swivel (gas cylinder)
    def sag(t): 
        x=5.55+(37.7-5.55)*t; y=5.3+(1.5-5.3)*t+1.05*math.sin(math.pi*t)
        return x,y
    top=[sag(i/24) for i in range(25)]; bot=[(x+0.12,y+0.75) for x,y in reversed(top)]
    P.append(p(top+bot,LEA,lambda lx,ly,u,v:0.75-0.35*v+0.06*math.sin(lx*1.3)))
    for t in (0.62,0.72):
        x,y=sag(t); P.append(p([(x-0.35,y-0.15),(x+0.35,y-0.2),(x+0.4,y+0.95),(x-0.3,y+1.0)],LEA[1:],flat(0.4)))   # keepers
    x,y=sag(0.55); P.append(p([(x-0.25,y+0.05),(x+0.6,y),(x+0.6,y+0.8),(x-0.25,y+0.85)],BRASS,lin_v(0.95,0.35)))  # brass hook
    for t in (0.3,0.36,0.42,0.48): 
        x,y=sag(t); D.append((x+0.3,y+0.38,(40,22,12)))                                                 # adjustment holes
    # ---- stock (one-piece walnut, straight wrist, no pistol grip)
    stock=[(0.3,2.15),(4.0,1.95),(9.0,1.62),(10.6,1.3),(11.6,0.8),(12.1,0.55),(12.4,0.95),(18.8,0.95),(20.6,0.62),(35.7,0.6),(35.7,1.5),(30.0,1.68),(23.5,1.95),(21.2,2.35),(19.6,2.62),
           (16.5,2.62),(12.6,2.45),(11.4,2.7),(9.4,3.4),(5.0,5.0),(0.0,6.6)]
    P.append(p(stock,WAL,grain(),col=False))
    # buttplate (checkered steel) + butt trap line
    P.append(p([(-0.12,2.1),(0.34,2.12),(0.08,6.68),(-0.32,6.68)],PARK,lambda lx,ly,u,v:0.7-0.3*u))
    for k in range(10): D.append((-0.08+0.03*k*0,2.3+k*0.45,PARK[1]))
    # rear sling swivel on butt belly
    P.append(p([(5.2,4.95),(5.9,4.75),(6.0,5.55),(5.3,5.65)],PARK,flat(0.55)))
    # ---- trigger guard + trigger + safety
    P.append(p([(12.5,2.3),(16.7,2.45),(16.6,2.9),(16.0,3.55),(13.4,3.55),(12.7,3.0)],PARK,lambda lx,ly,u,v:0.75-0.35*v))
    P.append(p([(13.05,2.55),(15.85,2.65),(15.5,3.25),(13.55,3.25)],[(14,12,10),(22,20,16)],flat(0.3),edge=False))   # guard opening
    P.append(p([(13.9,2.45),(14.25,2.45),(14.05,3.15),(13.8,3.1)],PARK[2:],lin_v(0.9,0.4)))                               # trigger
    P.append(p([(15.6,2.6),(16.15,2.6),(16.15,3.05),(15.7,3.05)],PARK[1:],lin_v(0.95,0.4)))                            # safety (front of guard)
    # ---- receiver (parkerized grey-green)
    rec=[(11.55,0.25),(11.9,-0.55),(12.4,-0.78),(18.6,-0.78),(18.75,-0.7),(20.7,-0.7),(20.7,0.4),(18.8,0.95),(12.4,0.95),(11.7,0.7)]
    P.append(p(rec,PARK,cyl(0.25,0.15,0.95),col=True))
    # rear sight: base, aperture ear protectors, windage knob (right) & elevation knob (left, peeking)
    P.append(p([(12.2,-0.78),(12.4,-1.62),(13.7,-1.72),(14.1,-0.78)],PARK,lin_v(1.0,0.45)))
    P.append(p([(12.8,-1.7),(12.95,-2.2),(13.4,-2.2),(13.55,-1.7)],PARK[1:],flat(0.65)))
    P.append(C(13.17,-1.9,0.12,[(4,4,4),(10,10,10)],flat(0.3),edge=False))
    P.append(C(12.75,-1.0,0.36,PARK,lambda lx,ly,u,v:0.95-0.6*v))
    P.append(C(14.0,-1.08,0.3,PARK[1:],lambda lx,ly,u,v:0.75-0.5*v))
    # bolt (visible on top of receiver) + bolt roller
    P.append(p([(14.2,-0.78),(14.2,-1.05),(18.3,-1.05),(18.5,-0.78)],PARK,lin_v(0.95,0.4)))
    P.append(C(17.3,-0.95,0.2,PARK[1:],flat(0.7)))
    # ---- barrel + handguards + lower band
    P.append(p([(20.7,-0.48),(43.3,-0.3),(43.3,0.3),(20.7,0.42)],BLUED,cyl(0.3,0.1,0.95),col=True))
    P.append(p([(20.5,-0.72),(21.2,-0.95),(26.9,-0.92),(26.9,0.28),(20.5,0.28)],WAL,grain(0.66,0.12)))              # rear handguard
    P.append(p([(26.85,-0.98),(27.35,-0.98),(27.35,0.64),(26.85,0.64)],PARK,lin_u(0.9,0.5)))                         # rear handguard band
    P.append(p([(27.35,-0.86),(35.6,-0.76),(35.6,0.62),(27.35,0.62)],WAL,grain(0.64,0.12)))                         # front handguard
    P.append(p([(35.5,-0.82),(36.4,-0.82),(36.4,1.5),(35.5,1.5)],PARK,cyl(0.3,0.1,0.9),col=True))                  # lower band / ferrule
    # ---- operating rod + handle (right side)
    P.append(p([(18.4,0.26),(26.9,0.26),(26.9,0.62),(18.4,0.62)],PARK,lin_v(1.0,0.35)))
    P.append(p([(17.6,-0.12),(18.3,-0.55),(19.5,-0.55),(19.5,0.55),(18.4,0.55)],PARK,lambda lx,ly,u,v:1.0-0.6*v-0.15*u))   # op rod handle
    P.append(p([(36.4,0.15),(37.2,0.15),(37.2,0.48),(36.4,0.48)],PARK,lin_v(0.9,0.4)))
    # ---- gas cylinder, front sight w/ protective ears, gas cylinder lock + lock screw, bayonet lug, front sling swivel
    P.append(p([(36.4,-0.58),(42.9,-0.58),(42.9,0.5),(36.4,0.5)],PARK,cyl(0.28,0.1,0.95),col=True))
    P.append(p([(37.1,0.5),(38.4,0.5),(38.2,0.85),(37.3,0.85)],PARK,flat(0.5)))                                   # swivel lug
    P.append(p([(37.35,0.8),(38.15,0.8),(38.2,1.5),(37.3,1.5)],PARK,flat(0.55)))                                  # front sling swivel
    P.append(p([(41.6,0.5),(42.8,0.5),(42.8,0.86),(41.9,0.86)],PARK,lin_u(0.9,0.5)))                               # bayonet lug
    P.append(p([(41.7,-0.58),(41.95,-1.4),(42.25,-1.4),(42.35,-0.9),(42.55,-0.9),(42.65,-1.4),(42.95,-1.4),(42.95,-0.58)],PARK,lin_v(0.95,0.45)))  # sight ears
    P.append(p([(42.35,-0.9),(42.4,-1.3),(42.5,-1.3),(42.55,-0.9)],PARK[2:],flat(0.8)))                             # blade
    P.append(p([(42.9,-0.5),(43.35,-0.5),(43.35,0.42),(42.9,0.42)],PARK,cyl(0.3,0.1,0.85),col=True))               # gas cylinder lock
    P.append(p([(43.35,-0.32),(43.6,-0.32),(43.6,0.28),(43.35,0.28)],PARK[1:],flat(0.55)))                          # lock screw
    # ---- lines: wood/metal joints, grain accents, op-rod slot, rivets
    L+=[([(12.4,-0.1),(18.6,-0.1)],PARK[1]),([(14.1,-0.78),(14.1,0.9)],PARK[2]),([(11.8,0.4),(12.3,0.62)],PARK[2]),([(18.6,-0.78),(18.6,0.9)],PARK[1]),
        ([(36.6,-0.05),(42.8,-0.05)],PARK[2]),([(39.6,-0.58),(39.6,0.5)],PARK[2]),([(36.5,-0.45),(42.8,-0.45)],PARK[6]),([(12.5,-0.68),(18.5,-0.68)],PARK[6]),
        ([(21.0,-0.78),(26.8,-0.8)],WAL[6]),([(27.5,-0.7),(35.4,-0.64)],WAL[6]),([(1.0,2.25),(9.0,1.75)],WAL[6]),([(27.4,0.62),(35.5,0.6)],WAL[1]),([(18.5,0.3),(26.8,0.3)],PARK[6]),
        ([(19.6,-0.55),(19.6,0.55)],PARK[1]),([(0.4,6.3),(5.0,4.8)],WAL[1])]
    for k in range(5): L.append(([(2.0+k*1.4,2.8+k*0.1),(4.0+k*1.4,2.3+k*0.1)],WAL[2]))
    L.append(([(17.9,0.0),(18.25,-0.4)],PARK[7]))
    # cartouche / stamp marks on stock (tiny)
    D+=[(10.0,2.3,WAL[1]),(10.2,2.2,WAL[1]),(10.4,2.3,WAL[1])]
    return P,L,D
def garand_labels():
    """(text, anchor x,y, label side) callouts in rifle coords"""
    return [('REAR SIGHT',13.2,-2.0,'up'),('OP ROD HANDLE',18.6,-0.55,'up'),('GAS CYLINDER',39.4,-0.6,'up'),('FRONT SIGHT',42.5,-1.4,'up'),
            ('SAFETY',15.9,3.1,'dn'),('M1907 SLING',24.0,6.2,'dn'),('BAYONET LUG',42.2,0.86,'dn')]
# ================================================================= WW2 kit icons (local units ~cm, nose left)
OD7=RP((92,88,56)); KHAKI=RP((150,132,86)); ODG=RP((96,100,62)); STEEL=RP((110,112,104)); BLACK=RP((44,42,40),0.4,1.6)
def helmet():
    P=[]; L=[]; D=[]
    # chinstrap hanging (behind brim)
    strap=[(4.5,1.0),(5.0,6.0),(9.0,9.5),(14.0,9.6),(18.0,6.2),(19.0,1.0),(18.2,1.0),(17.2,5.6),(14.0,8.4),(9.4,8.3),(5.9,5.6),(5.4,1.0)]
    P.append(p(strap,KHAKI,flat(0.5)))
    P.append(p([(10.6,7.9),(12.6,7.9),(12.6,9.8),(10.6,9.8)],STEEL,flat(0.7)))                     # buckle
    dome=[(0.5,0.6),(1.8,-4.0),(4.6,-8.2),(8.6,-10.4),(12.4,-10.8),(16.4,-9.8),(19.8,-7.0),(22.2,-2.6),(23.0,0.6)]
    P.append(p(dome+[(22.6,1.0),(0.9,1.0)],ODG,lambda lx,ly,u,v:1.0-0.65*u-0.25*v+0.08*noise(lx*4,ly*4,1),col=False))
    P.append(p([(-0.8,0.4),(24.2,0.4),(24.8,1.4),(-1.2,1.6)],ODG,lin_v(0.8,0.3)))                 # rolled rim
    P.append(p([(4.2,-0.4),(5.8,-0.4),(5.8,1.2),(4.2,1.2)],STEEL,flat(0.55)))                     # chinstrap bale (fixed loop)
    for i in range(40): D.append(((i*7.3)%21+1.5,-((i*3.7)%9)-0.5,ODG[2]))                         # sand-texture paint
    L+=[([(3.0,-5.6),(7.4,-9.2)],ODG[7]),([(8.0,-9.8),(10.6,-10.4)],ODG[7])]
    return P,L,D
def belt():
    P=[]; L=[]; D=[]
    P.append(p([(0,0),(30,0),(30,4.6),(0,4.6)],KHAKI,lin_v(0.85,0.4)))
    for i in range(5):
        x=1.0+i*5.8
        P.append(p([(x,-1.2),(x+5.2,-1.2),(x+5.2,7.8),(x,7.8)],KHAKI,lambda lx,ly,u,v:0.92-0.45*v-0.1*abs(u-0.5)))
        P.append(p([(x,-1.6),(x+5.2,-1.6),(x+5.2,2.6),(x+2.6,3.2),(x,2.6)],KHAKI,lambda lx,ly,u,v:1.0-0.5*v))      # flap
        P.append(C(x+2.6,2.0,0.7,STEEL,flat(0.8)))                                                               # lift-the-dot snap
        L.append(([(x+0.2,2.7),(x+5.0,2.7)],KHAKI[2]))
    for i in range(16): D.append((0.8+i*1.9,9.2,STEEL[6]))                                                       # grommets
    P.append(p([(0,8.6),(30,8.6),(30,9.8),(0,9.8)],KHAKI,lin_v(0.8,0.4)))
    return P,L,D
def grenade():
    P=[]; L=[]; D=[]
    P.append(p([(6.8,-4.6),(8.0,-6.2),(9.6,-6.0),(9.4,-4.6)],STEEL,flat(0.6)))                     # fuze top
    P.append(p([(3.6,-4.4),(9.6,-4.4),(9.6,-2.8),(3.6,-2.8)],OD7,lin_v(0.95,0.4)))                  # fuze body
    body=[(6.6,-3.0)]
    for i in range(25):
        a=math.pi*2*i/24; body.append((6.6+3.6*math.sin(a),2.6-5.4*math.cos(a)*(1 if math.cos(a)>0 else 0.95)))
    P.append(dict(poly=body[1:],ramp=OD7,shade=lambda lx,ly,u,v:1.0-0.7*u-0.2*v,col=False))
    # segmented 'pineapple' grid
    for k in range(1,5): L.append(([(3.1+0.2*abs(k-2.5),-2.8+k*1.9-1.9+1.9),(10.1-0.2*abs(k-2.5),-2.8+k*1.9)],OD7[1]))
    for k in (4.6,6.6,8.6): L.append(([(k,-2.6),(k,7.8)],OD7[1]))
    for r in range(5):
        for c in range(4): D.append((3.9+c*2.0+(0.3 if r%2 else 0),-1.9+r*1.9,OD7[6]))
    # spoon (lever) + pin ring
    P.append(p([(9.2,-4.6),(10.4,-4.2),(10.8,-1.2),(10.3,1.8),(9.8,1.6),(10.1,-1.0),(9.6,-3.6)],STEEL,lin_u(0.95,0.5)))
    P.append(dict(circle=(1.6,-3.6,1.7),ramp=STEEL,shade=flat(0.75)))
    P.append(dict(circle=(1.6,-3.6,1.1),ramp=[(0,0,0)],shade=flat(0.0),edge=False,cut=True))
    P.append(p([(2.9,-3.9),(3.8,-3.9),(3.8,-3.3),(2.9,-3.3)],STEEL,flat(0.6)))
    return P,L,D
def canteen():
    P=[]; L=[]; D=[]
    P.append(p([(7.2,-2.4),(10.8,-2.4),(10.8,0.4),(7.2,0.4)],BLACK,lin_u(1.0,0.4)))                 # cap
    P.append(p([(0,0),(18,0),(18.6,2),(18.4,16),(16,19.6),(2,19.6),(-0.4,16),(-0.6,2)],ODG,lambda lx,ly,u,v:0.95-0.45*u-0.2*v))   # cover
    P.append(p([(0,0),(8.4,0),(8.4,6.4),(4.2,7.4),(0,6.4)],ODG,lambda lx,ly,u,v:1.0-0.4*v))          # left flap
    P.append(p([(9.6,0),(18,0),(18,6.4),(13.8,7.4),(9.6,6.4)],ODG,lambda lx,ly,u,v:0.92-0.4*v))      # right flap
    for x in (4.2,13.8): P.append(C(x,5.8,0.9,STEEL,flat(0.8)))
    L+=[([(1.0,18.2),(17.0,18.2)],ODG[2]),([(9.0,7.4),(9.0,19.4)],ODG[2]),([(0.4,1.0),(17.6,1.0)],ODG[7])]
    P.append(p([(16.4,-1.4),(17.4,-1.4),(17.4,0.4),(16.4,0.4)],STEEL,flat(0.6)))                     # wire hanger
    P.append(p([(0.6,-1.4),(1.6,-1.4),(1.6,0.4),(0.6,0.4)],STEEL,flat(0.6)))
    return P,L,D
def knife():
    P=[]; L=[]; D=[]
    P.append(p([(0,0),(3.0,-0.95),(17.1,-0.95),(17.1,0.95),(2.0,0.95)],PARK,lambda lx,ly,u,v:0.95-0.6*v+0.1*(1-u)))   # 6.75in blade (cm)
    L+=[([(1.0,-0.2),(16.6,-0.2)],PARK[6]),([(0.6,-0.4),(4.0,-0.95)],PARK[7])]
    P.append(p([(17.1,-1.9),(18.0,-1.9),(18.0,1.7),(17.1,1.9)],PARK,lin_v(0.9,0.4)))                     # guard
    P.append(p([(18.0,-1.1),(28.8,-1.25),(28.8,1.25),(18.0,1.1)],LEA,cyl(0.3,0.1,0.95),col=True))        # leather washer grip
    for k in range(14): L.append(([(18.7+k*0.72,-1.1),(18.7+k*0.72,1.1)],LEA[1]))
    P.append(p([(28.8,-1.5),(30.6,-1.3),(30.8,1.3),(28.8,1.5)],PARK,lin_v(0.9,0.4)))                     # pommel
    return P,L,D
def bayonet():
    P=[]; L=[]; D=[]
    P.append(p([(0,0),(3.0,-0.85),(25.4,-0.85),(25.4,0.85),(1.6,0.85)],PARK,lambda lx,ly,u,v:0.95-0.6*v))        # 10in blade (cm)
    L+=[([(4,-0.15),(23.5,-0.15)],PARK[1]),([(1,-0.4),(4,-0.85)],PARK[7])]
    P.append(p([(25.4,-1.7),(26.2,-1.7),(26.2,1.7),(25.4,1.7)],PARK,flat(0.6)))
    P.append(dict(circle=(25.8,-2.4,0.9),ramp=PARK,shade=flat(0.6)))                                            # muzzle ring
    P.append(p([(26.2,-1.0),(35.8,-1.1),(36.2,1.0),(26.2,1.0)],BLACK,cyl(0.3,0.1,0.9),col=True))                 # grip
    for k in range(6): L.append(([(27.6+k*1.4,-0.9),(27.6+k*1.4,0.9)],BLACK[2]))
    return P,L,D
def m7gl():
    P=[]; L=[]; D=[]
    P.append(p([(0,-1.4),(12,-1.4),(12,1.4),(0,1.4)],PARK,cyl(0.3,0.1,0.95),col=True))                 # launcher tube
    P.append(p([(12,-1.0),(19,-1.0),(19,1.0),(12,1.0)],PARK,cyl(0.3,0.1,0.85),col=True))              # sleeve over gas cylinder
    for x in (2.0,4.0,6.0,8.0,10.0): L.append(([(x,-1.4),(x,1.4)],PARK[2]))                           # range rings
    P.append(p([(15,1.0),(17.4,1.0),(17.4,2.6),(15,2.6)],PARK,flat(0.5)))                             # bayonet-lug clamp
    P.append(p([(0,-1.2),(0.6,-1.2),(0.6,1.2),(0,1.2)],[(6,6,6),(18,18,18)],flat(0.3),edge=False))
    return P,L,D
def m84():
    P=[]; L=[]; D=[]
    P.append(p([(0,-1.6),(3.2,-1.6),(4.2,-1.0),(4.2,1.0),(3.2,1.6),(0,1.6)],BLACK,cyl(0.3,0.1,0.95),col=True))    # objective
    P.append(p([(4.2,-0.95),(15.0,-0.95),(15.0,0.95),(4.2,0.95)],BLACK,cyl(0.3,0.1,0.95),col=True))
    P.append(p([(7.2,-1.9),(9.4,-1.9),(9.4,-0.95),(7.2,-0.95)],BLACK,flat(0.6)))                           # turret
    P.append(p([(15,-1.4),(17.6,-1.4),(19.6,-2.2),(19.6,2.2),(17.6,1.4),(15,1.4)],BLACK[:6],cyl(0.3,0.1,0.8),col=True))   # rubber eyecup
    P.append(p([(5,0.95),(13,0.95),(13,2.4),(5,2.4)],PARK,lin_v(0.9,0.4)))                                  # Griffin & Howe-style mount
    P.append(dict(circle=(0.4,0,1.0),ramp=[(40,60,70),(90,120,130),(160,190,200)],shade=flat(0.6),edge=False))
    return P,L,D

def rot(fn,ang):
    def g():
        P,L,D=fn(); a=math.radians(ang); c,si=math.cos(a),math.sin(a)
        R=lambda x,y:(x*c+y*si,-x*si+y*c)
        for q in P:
            if 'poly' in q: q['poly']=[R(*pt) for pt in q['poly']]
            else: x,y,r=q['circle']; q['circle']=(*R(x,y),r)
        return P,[([R(*pt) for pt in pts],col) for pts,col in L],[(*R(x,y),col) for x,y,col in D]
    return g
knife_r=rot(knife,30)
