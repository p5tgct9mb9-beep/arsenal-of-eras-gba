from dslib import *
import tank as T
G9=GREY
TAN=[(60,44,24),(100,76,44),(146,116,72),(186,156,104),(220,196,146)]
def ic_campaign():
    P=[dict(poly=[(0,2),(8,0),(16,2),(24,0),(24,18),(16,20),(8,18),(0,20)],ramp=TAN,shade=lambda lx,ly,u,v:0.9-0.3*v-(0.25 if 8<lx<16 else 0))]
    L=[([(8,0),(8,18)],TAN[1]),([(16,2),(16,20)],TAN[1]),([(2,12),(5,10)],(200,40,30)),([(7,9),(10,10)],(200,40,30)),([(12,11),(15,8)],(200,40,30)),
       ([(3,5),(6,4),(4,7)],(70,110,50)),([(17,13),(21,12),(20,16)],(70,110,50))]
    P.append(dict(circle=(19,6,3),ramp=[(90,10,10),(180,30,30),(240,80,60)],shade=lin_v(1,0.3)))
    P.append(dict(poly=[(18.5,8),(19.5,8),(19,12)],ramp=[(90,10,10),(180,30,30)],shade=flat(0.5)))
    return P,L
def ic_jet():
    # F-16-like planform, top view, nose up
    P=[dict(poly=[(0,-22),(2,-14),(3,-4),(16,4),(16,7),(3,6),(3,12),(8,16),(8,18),(1,17),(0,19),(-1,17),(-8,18),(-8,16),(-3,12),(-3,6),(-16,7),(-16,4),(-3,-4),(-2,-14)],ramp=G9,shade=lambda lx,ly,u,v:0.95-0.5*u-0.2*v),
       dict(poly=[(-1.2,-14),(1.2,-14),(1.2,-8),(-1.2,-8)],ramp=GLS,shade=lin_v(0.9,0.3)),
       dict(poly=[(16,2),(17,2),(17,8),(16,8)],ramp=G9,shade=flat(0.7))]
    return P,[([(0,-6),(0,16)],G9[3])]
def ic_tank():
    return T.tank_parts(),T.tank_lines()[0]
def ic_ship():
    H=[(30,34,40),(50,56,64),(76,82,92),(104,110,120),(136,142,150)]
    P=[dict(poly=[(0,0),(92,0),(100,-4),(98,6),(86,12),(10,12),(4,6)],ramp=H,shade=lin_v(0.95,0.25)),
       dict(poly=[(30,0),(32,-8),(46,-8),(48,-14),(60,-14),(62,-8),(70,-8),(72,0)],ramp=H,shade=lin_v(0.95,0.4)),
       dict(poly=[(52,-14),(54,-24),(56,-24),(58,-14)],ramp=H,shade=flat(0.6)),
       dict(poly=[(78,-2),(84,-5),(86,-2)],ramp=H,shade=flat(0.7)),dict(poly=[(84,-4),(96,-6),(96,-5),(84,-3)],ramp=H,shade=flat(0.6)),
       dict(poly=[(4,6),(98,6),(97,8),(6,8)],ramp=[(80,20,16),(130,40,30),(160,60,40)],shade=flat(0.5),edge=False)]
    return P,[([(36,-5),(68,-5)],(20,24,30)),([(50,-11),(60,-11)],(150,190,220))]
def ic_rocket():
    W_=[(60,64,72),(110,114,122),(170,174,180),(220,222,226),(250,250,252)]
    P=[dict(poly=[(-4,0),(4,0),(4,-24),(0,-32),(-4,-24)],ramp=W_,shade=lambda lx,ly,u,v:0.95-1.0*abs(u-0.35)),
       dict(poly=[(-4,-6),(-9,2),(-4,0)],ramp=[(90,16,16),(170,36,30),(220,70,50)],shade=flat(0.6)),
       dict(poly=[(4,-6),(9,2),(4,0)],ramp=[(90,16,16),(170,36,30),(220,70,50)],shade=flat(0.4)),
       dict(circle=(0,-17,2),ramp=GLS,shade=flat(0.6)),
       dict(poly=[(-3,0),(3,0),(0,10)],ramp=[(200,80,20),(255,170,40),(255,240,160)],shade=lin_v(0.95,0.1),edge=False)]
    return P,[([(-4,-24),(4,-24)],W_[1])]
def ic_globe():
    OC=[(10,30,80),(20,60,130),(40,100,180),(90,150,220)]
    P=[dict(circle=(0,0,12),ramp=OC,shade=lambda lx,ly,u,v:0.95-0.6*u-0.3*v)]
    P.append(dict(poly=[(-8,-6),(-2,-9),(1,-4),(-3,0),(-6,4),(-10,0)],ramp=LAND5,shade=lin_u(0.9,0.4),edge=False))
    P.append(dict(poly=[(3,1),(8,-2),(10,4),(5,9),(2,6)],ramp=LAND5,shade=lin_u(0.8,0.3),edge=False))
    P.append(dict(poly=[(6,-14),(8,-14),(9,-11),(13,-10),(13,-9),(9,-9),(8,-6),(7,-6),(7,-9),(4,-9),(4,-10),(7,-11)],ramp=G9[3:],shade=flat(0.7)))
    L=[([(-12,6),(-6,10),(2,11),(8,8)],(240,240,240))]
    return P,L
LAND5=[(30,50,20),(56,84,32),(90,118,52),(130,150,80)]
def ic_stick():
    BL=[(16,16,18),(34,34,38),(56,56,62),(84,84,92),(120,120,128)]
    P=[dict(poly=[(-14,6),(14,6),(16,12),(-16,12)],ramp=BL,shade=lin_v(0.9,0.3)),
       dict(poly=[(-1.5,6),(1.5,6),(1.5,-8),(-1.5,-8)],ramp=G9,shade=lin_u(0.95,0.3)),
       dict(circle=(0,-11,5),ramp=[(90,10,10),(170,26,26),(220,60,50),(255,140,120)],shade=lambda lx,ly,u,v:0.95-0.5*u-0.5*v),
       dict(circle=(9,3,2.4),ramp=[(20,40,100),(40,90,200),(120,170,255)],shade=lin_v(1,0.3)),
       dict(circle=(-9,3,2.4),ramp=[(100,80,10),(200,170,30),(255,230,110)],shade=lin_v(1,0.3))]
    return P,[]
def ic_rocks():
    RK=[(60,20,10),(110,40,22),(160,66,36),(200,100,60),(236,150,104)]
    P=[dict(poly=[(-12,6),(-10,-2),(-4,-6),(2,-4),(4,2),(0,8),(-8,8)],ramp=RK,shade=lambda lx,ly,u,v:0.95-0.6*u-0.4*v),
       dict(poly=[(2,4),(5,-3),(11,-4),(14,2),(12,8),(4,8)],ramp=RK,shade=lambda lx,ly,u,v:0.9-0.6*u-0.4*v),
       dict(poly=[(-5,8),(-3,2),(3,1),(6,5),(4,9),(-4,10)],ramp=RK,shade=lambda lx,ly,u,v:0.95-0.6*u-0.4*v)]
    return P,[([(-7,-2),(-5,-3)],RK[4]),([(7,-2),(9,-2)],RK[4])]
def ic_gunsmith():
    P,L=m4(('RDS',))
    WR=[(40,42,46),(80,84,90),(130,134,140),(180,184,190),(220,224,228)]
    hx,hy=76,-11
    P=list(P)+[dict(poly=seg((36,17),(hx-3,hy+4),2.6,3.0),ramp=WR,shade=lambda lx,ly,u,v:0.95-0.6*u),
               dict(circle=(hx,hy,6.5),ramp=WR,shade=lambda lx,ly,u,v:0.95-0.5*u-0.3*v),
               dict(poly=[(hx-1,hy-1),(hx+4,hy-10),(hx+9,hy-6),(hx+2,hy+1)],ramp=[(14,20,40),(24,34,60)],shade=flat(0.5),edge=False)]
    return P,L+[([(38,14),(70,-6)],WR[4])]
def ic_chute():
    C=[(40,54,30),(66,82,48),(96,114,68),(130,146,96),(170,182,130)]
    P=[dict(poly=[(-18,-2),(-15,-9),(-6,-13),(6,-13),(15,-9),(18,-2),(12,-4),(6,-2),(0,-4),(-6,-2),(-12,-4)],ramp=C,shade=lambda lx,ly,u,v:0.95-0.5*u-0.3*v),
       dict(circle=(0,7,3),ramp=SKINS[2],shade=flat(0.6)),
       dict(poly=[(-3.5,9),(3.5,9),(3,17),(-3,17)],ramp=R((120,110,70)),shade=lin_u(0.9,0.3))]
    L=[([(-18,-2),(-2,9)],(200,200,180)),([(18,-2),(2,9)],(200,200,180)),([(-6,-2),(-1,9)],(200,200,180)),([(6,-2),(1,9)],(200,200,180)),
       ([(-6,-13),(-6,-2)],C[1]),([(6,-13),(6,-2)],C[1]),([(0,-13),(0,-4)],C[1])]
    return P,L
def ic_gear():
    P=[]; pts=[]
    for k in range(48):
        a=2*math.pi*k/48; r=12 if (k//3)%2==0 else 9
        pts.append((r*math.cos(a),r*math.sin(a)))
    P.append(dict(poly=pts,ramp=G9,shade=lambda lx,ly,u,v:0.95-0.45*u-0.35*v))
    P.append(dict(circle=(0,0,4),ramp=[(14,20,40),(24,34,60)],shade=flat(0.5)))
    return P,[]
def ic_infantry():
    return None
ICONS=dict(CAMPAIGN=ic_campaign,DOGFIGHT=ic_jet,ARMOR=ic_tank,NAVAL=ic_ship,SPACE=ic_rocket,WORLD=ic_globe,ARCADE=ic_stick,
           STORE=ic_rocks,GUNSMITH=ic_gunsmith,SPECOPS=ic_chute,SETTINGS=ic_gear)
