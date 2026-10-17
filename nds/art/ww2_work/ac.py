from r2 import *
# RLM colour ramps (7 steps, index 4 ~ nominal)
R74=ramp((70,78,78)); R75=ramp((96,96,106)); R76=ramp((150,166,176),lo=0.4,hi=1.25)
R70=ramp((46,54,42)); R71=ramp((66,76,52)); R65=ramp((128,156,172),lo=0.4,hi=1.3)
R81=ramp((84,76,58)); R82=ramp((70,90,56)); R04=ramp((214,170,40),lo=0.45,hi=1.2)
GLASS=[(14,16,18),(30,34,36),(52,58,58),(84,90,86),(128,132,124),(170,176,168),(206,210,200)]
MET=[(16,16,16),(30,28,26),(46,44,40),(66,62,56),(90,84,76),(112,104,94),(134,126,114)]
BLK=ramp((40,40,38))
PL=(34,38,38)  # panel line
def camo_fighter(belly,spine,up=(R74,R75),side=R76,under=R76,s=3.0,mott=0.66):
    a,b=up
    def f(lx,ly):
        if ly>belly(lx): return under
        n=noise(lx,ly,s)
        if ly<spine(lx): return a if n>0.5 else b
        if n>mott+0.08: return a
        if n>mott: return b
        return side
    return f
def two_tone(a,b,s=5.0,k=0.5,under=None,belly=None):
    def f(lx,ly):
        if under is not None and ly>belly(lx): return under
        return a if noise(lx,ly,s)>k else b
    return f
def splinter(a,b,per=14,under=None,belly=None):
    def f(lx,ly):
        if under is not None and ly>belly(lx): return under
        return a if int(math.floor((lx*0.8+ly*1.6+200)/per))%2 else b
    return f
CYL=cyl(0.28,0.08,0.9)
def wingsh(lx,ly,u,v): return 0.92-0.5*u-0.2*v
# each returns dict(parts, lines, dots, marks)
def bf109():
    belly=lambda x: 3.0 if x<26 else 2.0+0.0*x
    spine=lambda x: -6 if x<30 else -5+ (x-30)*0.02
    cam=camo_fighter(lambda x:2.5,lambda x:-5.0)
    P=[]
    P.append(dict(poly=[(84,-3),(99,-4),(103,-2),(88,-1)],ramp=R75,rampf=cam,shade=lin_u(0.8,0.4)))  # far tailplane
    P.append(dict(poly=[(85,-3),(88,-11),(91,-16),(95,-17),(98,-15),(99,-12),(99,-2)],ramp=R76,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(24,-7),(34,-8),(48,-7),(62,-6),(80,-4),(94,-3),(99,-2),(99,2),(90,3),(70,4),(50,6),(36,8),(24,8)],ramp=R76,rampf=cam,shade=CYL,col=True))
    P.append(dict(poly=[(8,-5),(14,-6),(26,-7),(26,8),(14,7),(8,6)],ramp=R76,rampf=cam,shade=CYL,col=True))  # cowl
    P.append(dict(poly=[(16,-6),(18,-8),(23,-8),(25,-7)],ramp=R74,shade=lin_v(0.9,0.4)))  # gun bulge (Beule)
    P.append(dict(poly=[(14,7),(24,7),(22,10),(16,10)],ramp=R76,shade=lin_v(0.7,0.2)))  # oil cooler
    P.append(dict(poly=[(18,-1),(23,-1),(23,2),(19,2)],ramp=MET,shade=flat(0.3)))  # supercharger intake
    P.append(dict(poly=[(0,1),(2,-2),(5,-4),(9,-5),(9,6),(5,5),(2,3)],ramp=R70,shade=cyl(0.3,0.1,0.9),col=True))  # spinner
    P.append(dict(poly=[(28,-8),(31,-12),(44,-12),(47,-8)],ramp=GLASS,shade=lambda lx,ly,u,v:0.2+0.6*(1-v)*(1-u)+0.15))
    P.append(dict(poly=[(56,-6),(61,-4)],ramp=R76))  # dummy (ignored small)
    P.append(dict(poly=[(66,-4),(72,-4),(72,4),(66,4)],ramp=R04,shade=CYL,edge=False,col=True))  # yellow band
    P.append(dict(poly=[(22,6),(48,5),(62,18),(58,21),(52,21),(30,12)],ramp=R74,rampf=splinter(R74,R75,12),shade=wingsh))  # near wing
    P.append(dict(poly=[(85,0),(100,0),(104,3),(100,4),(88,2)],ramp=R74,rampf=splinter(R74,R75,10),shade=lin_u(0.9,0.3)))  # near tailplane
    ln=[([(26,-7),(26,7)],PL),([(9,-5),(9,6)],PL),([(36,-12),(36,-8)],(40,44,44)),([(40,-12),(40,-8)],(40,44,44)),
        ([(30,-11),(36,-11)],(200,204,196)),([(94,-15),(94,-2)],PL),([(50,-7),(50,6)],PL),([(84,-3),(84,3)],PL),
        ([(30,7),(46,6)],PL),([(40,9),(56,18)],PL),([(14,-1),(16,1)],(30,26,20)),([(14,1),(16,3)],(30,26,20))]
    dots=[(12,-1,(30,30,26)),(12,2,(30,30,26))]
    marks=[('cross',78,0),('cross',48,14),('prop',0,1,17)]
    return dict(parts=[p for p in P if len(p['poly'])>2],lines=ln,dots=dots,marks=marks)
def fw190():
    cam=camo_fighter(lambda x:3.0,lambda x:-6.0)
    P=[]
    P.append(dict(poly=[(84,-3),(99,-4),(103,-2),(88,-1)],ramp=R75,shade=lin_u(0.8,0.4)))
    P.append(dict(poly=[(84,-4),(88,-12),(91,-18),(96,-19),(99,-15),(99,-3)],ramp=R76,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(20,-8),(30,-9),(56,-9),(64,-7),(80,-5),(94,-4),(99,-3),(99,2),(90,3),(70,4),(50,7),(34,9),(20,9)],ramp=R76,rampf=cam,shade=CYL,col=True))
    P.append(dict(poly=[(4,-8),(8,-10),(22,-9),(24,-8),(24,9),(8,10),(4,8)],ramp=R04,
                  rampf=lambda lx,ly: R04 if ly>3 else R70,shade=CYL,col=True))  # BMW 801 cowl, yellow under
    P.append(dict(poly=[(0,-2),(1,-4),(4,-5),(5,-5),(5,5),(4,5),(1,4),(0,2)],ramp=R70,shade=cyl(0.3,0.1,0.9),col=True))
    P.append(dict(poly=[(30,-9),(33,-13),(38,-15),(48,-15),(54,-13),(58,-9)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.6*(1-v)*(1-u)+0.1))
    P.append(dict(poly=[(26,-1),(30,-1),(30,3),(26,3)],ramp=MET,shade=flat(0.25),edge=False))  # exhaust
    P.append(dict(poly=[(22,7),(48,6),(64,18),(60,21),(54,21),(30,13)],ramp=R74,rampf=splinter(R74,R75,12),shade=wingsh))
    P.append(dict(poly=[(85,0),(100,0),(104,3),(100,4),(88,2)],ramp=R74,rampf=splinter(R74,R75,10),shade=lin_u(0.9,0.3)))
    ln=[([(24,-8),(24,8)],PL),([(12,-9),(12,9)],(30,32,26)),([(36,-14),(36,-9)],(40,44,44)),([(50,-14),(52,-9)],(40,44,44)),
        ([(34,-12),(40,-14)],(206,210,200)),([(94,-17),(94,-3)],PL),([(46,-9),(46,7)],PL),([(84,-4),(84,3)],PL),
        ([(30,4),(34,6)],(60,56,48)),([(42,9),(58,18)],PL)]
    marks=[('cross',74,0),('cross',50,14),('prop',0,0,17)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
def ju87():
    up=splinter(R70,R71,14,under=R65,belly=lambda x:3.0)
    P=[]
    P.append(dict(poly=[(84,0),(99,-1),(103,1),(88,2)],ramp=R71,shade=lin_u(0.8,0.4)))
    P.append(dict(poly=[(84,-5),(86,-12),(90,-19),(95,-19),(98,-15),(98,-3)],ramp=R70,rampf=up,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(24,-6),(34,-7),(66,-7),(86,-5),(98,-4),(98,1),(86,2),(60,5),(40,8),(24,8)],ramp=R70,rampf=up,shade=CYL,col=True))
    P.append(dict(poly=[(5,-5),(26,-6),(26,7),(5,5)],ramp=R70,rampf=up,shade=CYL,col=True))
    P.append(dict(poly=[(8,5),(22,6),(21,12),(10,12)],ramp=R65,shade=lin_v(0.8,0.2)))  # radiator bath
    P.append(dict(poly=[(9,10),(21,10),(21,12),(10,12)],ramp=MET,shade=flat(0.2),edge=False))
    P.append(dict(poly=[(0,-1),(2,-3),(5,-3),(5,3),(2,3),(0,1)],ramp=R70,shade=cyl(0.3,0.1,0.9),col=True))
    P.append(dict(poly=[(28,-7),(31,-12),(64,-12),(68,-7)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.5*(1-v)+0.1*(1-u)))
    # 250 kg bomb
    P.append(dict(poly=[(34,9),(36,8),(52,8),(58,10),(52,12),(36,12),(34,11)],ramp=BLK,shade=cyl(0.3,0.1,0.85),col=True))
    P.append(dict(poly=[(56,8),(62,7),(62,13),(56,12)],ramp=BLK,shade=flat(0.4)))
    # gull wing + spatted gear
    P.append(dict(poly=[(28,7),(50,6),(52,10),(64,22),(58,24),(50,22),(34,13)],ramp=R70,rampf=splinter(R70,R71,12),shade=wingsh))
    P.append(dict(poly=[(36,13),(40,13),(40,20),(36,20)],ramp=R65,shade=lin_u(0.9,0.4)))  # gear leg
    P.append(dict(poly=[(32,20),(42,19),(44,23),(42,27),(34,27),(31,24)],ramp=R65,shade=cyl(0.3,0.1,0.9),col=True))  # spat
    P.append(dict(poly=[(85,1),(100,1),(103,4),(99,5),(88,3)],ramp=R70,rampf=splinter(R70,R71,10),shade=lin_u(0.9,0.3)))
    ln=[([(26,-6),(26,7)],PL),([(5,-5),(5,5)],PL),([(38,-12),(38,-7)],(30,34,34)),([(46,-12),(46,-7)],(30,34,34)),
        ([(54,-12),(54,-7)],(30,34,34)),([(60,-12),(60,-7)],(30,34,34)),([(28,-9),(66,-9)],(40,46,44)),([(32,-11),(36,-11)],(206,210,200)),
        ([(94,-17),(94,-4)],PL),([(84,-5),(84,2)],PL),([(37,10),(51,10)],(60,60,58)),([(88,2),(86,7)],(30,30,28)),
        ([(40,10),(58,20)],PL),([(14,-2),(22,-2)],(30,30,26))]
    dots=[(41,15,(200,200,190)),(42,15,(60,60,58))]
    marks=[('cross',76,-1),('cross',52,17),('prop',0,0,17)]
    return dict(parts=P,lines=ln,dots=dots,marks=marks)
def ju88():
    up=splinter(R70,R71,14,under=R65,belly=lambda x:4.0)
    P=[]
    P.append(dict(poly=[(84,-2),(99,-3),(103,-1),(88,0)],ramp=R71,shade=lin_u(0.8,0.4)))
    P.append(dict(poly=[(82,-5),(85,-15),(89,-21),(95,-22),(99,-17),(99,-3)],ramp=R70,rampf=up,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(18,-10),(30,-11),(60,-8),(90,-5),(99,-3),(99,1),(80,3),(50,6),(30,8),(18,8)],ramp=R70,rampf=up,shade=CYL,col=True))
    # 'beetle-eye' glazed nose + cockpit
    P.append(dict(poly=[(0,1),(1,-4),(4,-8),(10,-11),(18,-13),(26,-13),(30,-10),(22,-8),(20,8),(8,8),(2,5)],ramp=GLASS,
                  shade=lambda lx,ly,u,v:0.35+0.55*(1-v)*(1-u*0.6)))
    P.append(dict(poly=[(14,7),(40,7),(38,12),(22,12),(16,10)],ramp=R65,shade=lin_v(0.8,0.2)))  # Bola gondola
    P.append(dict(poly=[(16,8),(21,8),(22,11),(17,10)],ramp=GLASS,shade=flat(0.4)))
    P.append(dict(poly=[(36,7),(40,7),(38,12),(34,11)],ramp=GLASS,shade=flat(0.35)))
    P.append(dict(poly=[(30,2),(52,1),(70,16),(64,19),(56,19),(38,10)],ramp=R70,rampf=splinter(R70,R71,12),shade=wingsh))
    # near engine nacelle with annular radiator
    P.append(dict(poly=[(16,-2),(19,-5),(40,-5),(54,-1),(58,4),(40,8),(19,8),(16,5)],ramp=R71,rampf=lambda lx,ly: R65 if ly>4 else R71,shade=cyl(0.28,0.15,1.0),col=True))
    P.append(dict(poly=[(16,-3),(20,-5),(23,-5),(23,8),(20,8),(16,6)],ramp=BLK,shade=cyl(0.3,0.1,0.75),col=True))
    P.append(dict(poly=[(11,0),(13,-2),(17,-2),(17,4),(13,4),(11,2)],ramp=R70,shade=cyl(0.3,0.1,0.9),col=True))
    P.append(dict(poly=[(85,1),(100,1),(103,4),(99,5),(88,3)],ramp=R70,rampf=splinter(R70,R71,10),shade=lin_u(0.9,0.3)))
    G2=(36,42,40)
    ln=[([(4,-8),(6,4)],G2),([(10,-11),(12,6)],G2),([(1,-3),(20,-3)],G2),([(16,-13),(18,-8)],G2),([(4,-6),(10,-9)],(206,210,200)),
        ([(23,-5),(23,8)],PL),([(30,-1),(48,-1)],PL),([(94,-19),(94,-3)],PL),([(82,-5),(82,2)],PL),([(42,6),(62,16)],PL),
        ([(60,-8),(60,5)],PL)]
    marks=[('cross',72,-1),('cross',56,13),('prop',11,1,15)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
def me262():
    cam=camo_fighter(lambda x:2.0,lambda x:-5.0,up=(R81,R82),side=R76,mott=0.58)
    P=[]
    P.append(dict(poly=[(84,-5),(99,-6),(103,-4),(88,-3)],ramp=R82,shade=lin_u(0.8,0.4)))
    P.append(dict(poly=[(86,-3),(89,-11),(93,-16),(97,-16),(100,-12),(100,-2)],ramp=R76,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(0,0),(3,-3),(10,-5),(30,-7),(44,-8),(70,-6),(92,-3),(100,-2),(100,1),(90,2),(60,4),(30,5),(10,4),(3,2)],
                  ramp=R76,rampf=cam,shade=CYL,col=True))
    P.append(dict(poly=[(36,-8),(40,-12),(48,-13),(54,-12),(58,-8)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.6*(1-v)*(1-u)+0.1))
    P.append(dict(poly=[(36,3),(60,3),(80,18),(76,21),(70,21),(46,10)],ramp=R81,rampf=two_tone(R81,R82,5),shade=wingsh))
    # Jumo 004 nacelle
    P.append(dict(poly=[(24,6),(28,3),(50,3),(60,5),(62,8),(58,12),(30,12),(24,10)],ramp=R76,rampf=lambda lx,ly: R76 if ly>7 else R82,shade=CYL,col=True))
    P.append(dict(poly=[(22,6),(25,5),(25,11),(22,10)],ramp=MET,shade=flat(0.15),edge=False))
    P.append(dict(poly=[(60,6),(66,7),(66,9),(60,11)],ramp=MET,shade=cyl(0.3,0.1,0.8),col=True))
    P.append(dict(poly=[(85,-2),(100,-3),(104,0),(100,1),(88,-1)],ramp=R81,rampf=two_tone(R81,R82,4),shade=lin_u(0.9,0.3)))
    ln=[([(10,-5),(10,4)],PL),([(30,-7),(30,5)],PL),([(46,-12),(46,-8)],(40,44,44)),([(40,-11),(44,-12)],(206,210,200)),
        ([(95,-14),(95,-2)],PL),([(84,-4),(84,2)],PL),([(34,4),(34,11)],PL),([(50,6),(70,17)],PL),([(4,-1),(8,-1)],(30,30,28)),([(5,1),(8,1)],(30,30,28))]
    marks=[('cross',76,-2),('cross',66,14)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
def me163():
    cam=camo_fighter(lambda x:3.0,lambda x:-11.0,up=(R81,R82),side=R76,mott=0.55,s=5)
    P=[]
    P.append(dict(poly=[(0,0),(2,-6),(8,-11),(20,-14),(36,-15),(50,-13),(70,-8),(86,-4),(92,-2),(92,3),(80,5),(50,8),(20,8),(6,6),(1,3)],
                  ramp=R76,rampf=cam,shade=CYL,col=True))
    P.append(dict(poly=[(54,-12),(62,-24),(68,-31),(74,-34),(80,-34),(84,-30),(86,-6),(70,-8)],ramp=R76,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(18,-13),(22,-19),(30,-21),(38,-19),(42,-14)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.6*(1-v)*(1-u)+0.1))
    P.append(dict(poly=[(30,4),(58,2),(94,17),(92,21),(78,22),(44,12)],ramp=R81,rampf=two_tone(R81,R82,6),shade=wingsh))
    P.append(dict(poly=[(90,-2),(97,-1),(97,2),(90,3)],ramp=MET,shade=cyl(0.3,0.0,0.8),col=True))
    P.append(dict(poly=[(-2,-1),(0,-2),(2,-2),(2,2),(0,2),(-2,1)],ramp=BLK,shade=flat(0.5)))  # generator spinner
    P.append(dict(poly=[(20,8),(48,8),(46,10),(22,10)],ramp=MET,shade=flat(0.4)))  # landing skid
    ln=[([(10,-10),(10,6)],PL),([(46,-14),(46,8)],PL),([(24,-18),(28,-20)],(206,210,200)),([(30,-21),(30,-14)],(40,44,44)),
        ([(82,-30),(84,-6)],PL),([(84,18),(92,18)],PL),([(50,8),(80,18)],PL),([(-1,-6),(-1,6)],(60,60,58))]
    marks=[('cross',66,-4),('cross',70,15)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
def ar234():
    cam=camo_fighter(lambda x:2.0,lambda x:-6.0,up=(R81,R82),side=R76,mott=0.58)
    P=[]
    P.append(dict(poly=[(84,-3),(99,-4),(103,-2),(88,-1)],ramp=R82,shade=lin_u(0.8,0.4)))
    P.append(dict(poly=[(86,-3),(89,-11),(93,-16),(97,-17),(100,-13),(100,-2)],ramp=R76,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.5*v))
    P.append(dict(poly=[(0,2),(1,-4),(4,-8),(10,-10),(18,-10),(30,-8),(60,-6),(90,-3),(100,-2),(100,1),(90,2),(60,4),(30,5),(10,5),(3,4)],
                  ramp=R76,rampf=cam,shade=CYL,col=True))
    P.append(dict(poly=[(0,2),(1,-4),(4,-8),(10,-10),(17,-10),(18,-5),(15,1),(6,4)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.6*(1-v)*(1-u)+0.1))
    P.append(dict(poly=[(14,-11),(16,-13),(19,-13),(20,-10)],ramp=MET,shade=flat(0.5)))  # periscope
    P.append(dict(poly=[(30,-7),(56,-7),(84,12),(80,16),(72,16),(42,0)],ramp=R81,rampf=two_tone(R81,R82,5),shade=wingsh))
    P.append(dict(poly=[(26,3),(30,1),(52,1),(62,3),(64,7),(60,10),(32,10),(26,8)],ramp=R76,rampf=lambda lx,ly: R76 if ly>5 else R82,shade=CYL,col=True))
    P.append(dict(poly=[(24,4),(27,3),(27,9),(24,8)],ramp=MET,shade=flat(0.15),edge=False))
    P.append(dict(poly=[(62,4),(68,5),(68,7),(62,9)],ramp=MET,shade=cyl(0.3,0.1,0.8),col=True))
    P.append(dict(poly=[(85,0),(100,0),(104,3),(100,4),(88,2)],ramp=R81,rampf=two_tone(R81,R82,4),shade=lin_u(0.9,0.3)))
    G2=(36,42,40)
    ln=[([(5,-7),(8,3)],G2),([(10,-10),(12,4)],G2),([(1,-2),(16,-2)],G2),([(3,-5),(8,-8)],(206,210,200)),
        ([(30,-8),(30,1)],PL),([(95,-15),(95,-2)],PL),([(84,-3),(84,2)],PL),([(60,-2),(78,12)],PL),([(36,1),(36,10)],PL)]
    marks=[('cross',80,-1),('cross',72,11)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
def ho229():
    # oblique top-down view: nose left, span vertical (squashed); far wing up
    k=0.42
    def Q(x,y): return (x,y*k)
    cam=two_tone(R82,R81,6,0.5)
    wing=[Q(0,0),Q(14,-14),Q(56,-50),Q(62,-52),Q(68,-48),Q(60,-20),Q(76,-2),Q(76,2),Q(60,20),Q(68,48),Q(62,52),Q(56,50),Q(14,14)]
    P=[]
    P.append(dict(poly=wing,ramp=R82,rampf=cam,shade=lambda lx,ly,u,v:0.95-0.35*u-0.35*v))
    P.append(dict(poly=[Q(2,-4),Q(30,-10),Q(66,-6),Q(70,0),Q(66,6),Q(30,10),Q(2,4)],ramp=R82,rampf=cam,shade=lambda lx,ly,u,v:1.0-0.4*v-0.2*u))  # centre body
    P.append(dict(poly=[(10,-4),(14,-7),(22,-8),(28,-6),(30,-3),(14,-2)],ramp=GLASS,shade=lambda lx,ly,u,v:0.25+0.6*(1-v)*(1-u)+0.1))  # canopy
    P.append(dict(poly=[Q(8,-9),Q(12,-12),Q(14,-8),Q(10,-6)],ramp=MET,shade=flat(0.1),edge=False))  # intakes
    P.append(dict(poly=[Q(8,9),Q(12,12),Q(14,8),Q(10,6)],ramp=MET,shade=flat(0.1),edge=False))
    P.append(dict(poly=[Q(56,-10),Q(66,-9),Q(66,-5),Q(56,-5)],ramp=MET,shade=flat(0.35)))  # exhausts on upper surface
    P.append(dict(poly=[Q(56,5),Q(66,5),Q(66,9),Q(56,10)],ramp=MET,shade=flat(0.35)))
    ln=[([Q(20,-24),Q(50,-46)],PL),([Q(20,24),Q(50,46)],PL),([Q(58,-22),Q(64,-46)],PL),([Q(58,22),Q(64,46)],PL),
        ([(14,-6),(20,-7)],(206,210,200))]
    marks=[('cross',44,-28*k),('cross',44,30*k)]
    return dict(parts=P,lines=ln,dots=[],marks=marks)
