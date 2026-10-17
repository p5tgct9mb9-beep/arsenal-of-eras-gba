from dslib import *
TANC=R((170,150,106),0.32,1.3)     # desert tan (M1A2)
TRK=[(12,12,10),(26,26,22),(40,40,34),(56,56,48),(74,72,62)]
RU=[(40,44,26),(56,62,36),(72,80,46),(90,100,58),(110,120,72),(132,142,88),(156,164,108)]  # russian green
def wheelset(P,L,xs,cy,r,ramp,hub=None):
    for x in xs:
        P.append(dict(circle=(x,cy,r),ramp=ramp,shade=lambda lx,ly,u,v:1.0-0.85*v-0.15*u))
        P.append(dict(circle=(x,cy,r*0.42),ramp=ramp[1:6],shade=lambda lx,ly,u,v:0.25+0.7*v))
def m1a2():
    P=[]; L=[]; A=TANC
    # track run
    P.append(dict(poly=[(-2,-12),(4,-18),(130,-18),(136,-13),(134,-4),(126,0),(10,0),(0,-6)],ramp=TRK,shade=cyl(0.2,0.0,0.8),col=True))
    wheelset(P,L,[14+i*16.6 for i in range(7)],-5.4,5.4,A[:7])
    P.append(dict(circle=(2,-10,4.4),ramp=A[:6],shade=lin_v(0.9,0.2)))
    P.append(dict(circle=(129,-12,5.6),ramp=TRK,shade=lin_v(0.9,0.2)))
    # hull: glacis + deck + rear
    P.append(dict(poly=[(-8,-21),(22,-27.5),(132,-27.5),(136,-25),(136,-19),(-6,-18)],ramp=A,shade=lin_v(0.95,0.4)))
    # side skirts (heavier front sections)
    P.append(dict(poly=[(-7,-21),(136,-21),(136,-12),(52,-12),(50,-10.5),(-4,-10.5),(-7,-14)],ramp=A,shade=lambda lx,ly,u,v:0.88-0.5*v-0.08*u))
    # turret with long bustle
    P.append(dict(poly=[(18,-28),(30,-41),(118,-41),(126,-39),(127,-32),(104,-30),(100,-28)],ramp=A,shade=lambda lx,ly,u,v:0.98-0.55*v-0.1*u))
    P.append(dict(poly=[(30,-41),(118,-41),(118,-39),(29,-39)],ramp=A[2:],shade=flat(0.9),edge=False))     # turret roof edge highlight
    # bustle rack
    P.append(dict(poly=[(102,-45),(126,-45),(126,-41),(102,-41)],ramp=A[:5],shade=flat(0.35)))
    # gun: mantlet, barrel, bore evacuator, muzzle reference
    P.append(dict(poly=[(16,-38.5),(25,-38.5),(25,-31),(16,-31)],ramp=A[:7],shade=cyl(0.3,0.1,0.9),col=True))
    P.append(dict(poly=[(-36,-36.4),(18,-37),(18,-33),(-36,-33.6)],ramp=A,shade=cyl(0.3,0.05,0.95),col=True))
    P.append(dict(poly=[(-14,-37.4),(-2,-37.6),(-2,-32.4),(-14,-32.6)],ramp=A,shade=cyl(0.3,0.05,0.95),col=True))
    P.append(dict(poly=[(-38,-36.6),(-35,-36.6),(-35,-33.4),(-38,-33.4)],ramp=TRK,shade=flat(0.5)))
    # GPS doghouse, CITV, commander cupola + M2, loader hatch
    P.append(dict(poly=[(34,-41),(36,-45),(44,-45),(45,-41)],ramp=A[:7],shade=lin_v(0.95,0.35)))
    P.append(dict(poly=[(50,-41),(51,-47),(58,-47),(59,-41)],ramp=A[:7],shade=lin_v(0.95,0.35)))   # CITV
    P.append(dict(poly=[(52,-47),(57,-47),(57,-50),(52,-50)],ramp=TRK,shade=flat(0.5)))
    P.append(dict(poly=[(70,-41),(72,-45),(84,-45),(86,-41)],ramp=A[1:],shade=lin_v(0.95,0.3)))
    P.append(dict(poly=[(56,-47),(76,-47),(76,-45.4),(56,-45.4)],ramp=TRK,shade=flat(0.6)))   # M2
    P.append(dict(poly=[(90,-41),(92,-43),(98,-43),(99,-41)],ramp=A[1:],shade=flat(0.6)))
    # smoke dischargers
    P.append(dict(poly=[(27,-38),(32,-38),(32,-35),(27,-35)],ramp=A[:6],shade=flat(0.5)))
    D=A[1]; H=A[6]
    L+=[([(-6,-19.5),(136,-19.5)],H),([(14,-21),(14,-11)],D),([(32,-21),(32,-11)],D),([(52,-21),(52,-12)],D),([(76,-21),(76,-12)],D),([(100,-21),(100,-12)],D),([(122,-21),(122,-12)],D),
        ([(24,-27),(130,-27)],H),([(34,-36),(100,-36)],D),([(30,-40),(118,-40)],A[6]),([(60,-33),(96,-33)],D),([(104,-33),(126,-33)],D),
        ([(-34,-35.8),(16,-36.2)],A[6]),([(106,-44),(124,-44)],A[5]),([(108,-45),(108,-41)],A[1]),([(116,-45),(116,-41)],A[1]),
        ([(46,-34),(54,-34)],D),([(128,-26),(134,-26)],D)]
    for i in range(34): L.append(([(-1+i*4.1,0),(-1+i*4.1,-1)],TRK[3]))
    L.append(([(40,-45),(40,-58)],TRK[2]))   # antenna
    return P,L
def t90():
    P=[]; L=[]; A=RU
    P.append(dict(poly=[(-2,-11),(4,-17),(112,-17),(118,-12),(116,-4),(108,0),(10,0),(0,-6)],ramp=TRK,shade=cyl(0.2,0.0,0.8),col=True))
    wheelset(P,L,[14+i*17 for i in range(6)],-6.2,6.2,A[:7])
    for x in (30,56,82): P.append(dict(circle=(x,-15.5,1.8),ramp=A[:5],shade=flat(0.5)))
    P.append(dict(circle=(2,-10,4.4),ramp=A[:6],shade=lin_v(0.9,0.2)))
    P.append(dict(circle=(113,-11,5.2),ramp=TRK,shade=lin_v(0.9,0.2)))
    # hull + glacis ERA
    P.append(dict(poly=[(-6,-16),(16,-24),(116,-24),(118,-21),(118,-16)],ramp=A,shade=lin_v(0.95,0.35)))
    for k in range(4):
        x=-3+k*5; P.append(dict(poly=[(x,-17-k*1.9),(x+5,-19-k*1.9),(x+5,-17.6-k*1.9),(x,-15.6-k*1.9)],ramp=A[1:],shade=lin_v(0.95,0.4)))
    # rubber side skirt with front ERA panels
    P.append(dict(poly=[(-2,-17),(116,-17),(116,-14),(-2,-14)],ramp=TRK[1:],shade=lin_v(0.8,0.3)))
    for k in range(4): P.append(dict(poly=[(-2+k*9,-18),(6.5+k*9,-18),(6.5+k*9,-13),(-2+k*9,-13)],ramp=A,shade=lin_v(0.9,0.3)))
    # rear fuel drums + unditching log
    P.append(dict(circle=(118,-26.5,2.6),ramp=[(50,34,20),(80,56,34),(110,80,50)],shade=lin_v(0.9,0.2)))
    # turret dome
    P.append(dict(poly=[(20,-25),(22,-31),(32,-36),(52,-38),(72,-37),(84,-33),(88,-28),(86,-25)],ramp=A,shade=cyl(0.3,0.1,1.0),col=True))
    # Kontakt-5 clamshell ERA on turret front
    P.append(dict(poly=[(16,-27),(25,-36.5),(38,-37.5),(30,-27)],ramp=A,shade=lambda lx,ly,u,v:0.95-0.5*v-0.15*u))
    P.append(dict(poly=[(25,-36.5),(38,-37.5),(37,-39),(27,-38.5)],ramp=A[2:],shade=flat(0.8)))
    # gun + fume extractor
    P.append(dict(poly=[(-46,-31.2),(18,-32),(18,-28.8),(-46,-29.4)],ramp=A,shade=cyl(0.3,0.05,0.95),col=True))
    P.append(dict(poly=[(-16,-32.4),(-4,-32.8),(-4,-28.2),(-16,-28.4)],ramp=A,shade=cyl(0.3,0.05,0.95),col=True))
    # Shtora dazzler box with red lens
    P.append(dict(poly=[(19,-32),(26,-32),(26,-27.5),(19,-27.5)],ramp=A[:6],shade=flat(0.45)))
    P.append(dict(circle=(21.5,-29.8,1.5),ramp=[(90,10,10),(180,30,26),(240,90,70)],shade=flat(0.6),edge=False))
    # cupola + remote MG, snorkel on turret rear
    P.append(dict(poly=[(54,-37.6),(56,-41),(66,-41),(68,-37.4)],ramp=A[1:],shade=lin_v(0.95,0.3)))
    P.append(dict(poly=[(42,-42.6),(58,-42.6),(58,-41),(42,-41)],ramp=TRK,shade=flat(0.6)))
    P.append(dict(poly=[(72,-36.5),(88,-33),(88,-31.5),(72,-35)],ramp=A[:6],shade=flat(0.45)))
    D=A[1]
    L+=[([(16,-23),(116,-23)],A[6]),([(30,-33),(80,-33)],D),([(36,-35.5),(70,-36)],A[6]),([(-44,-30.8),(16,-31.4)],A[6]),
        ([(26,-34),(30,-28)],D),([(32,-36.5),(36,-28)],D),([(96,-22),(112,-22)],D)]
    for i in range(30): L.append(([(-1+i*4,0),(-1+i*4,-1)],TRK[3]))
    L.append(([(74,-37),(78,-52)],TRK[2]))
    return P,L
