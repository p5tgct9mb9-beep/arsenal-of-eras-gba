from hd import *
AR=[(22,24,16),(36,40,26),(52,58,38),(68,74,50),(86,92,62),(106,112,78),(128,132,96),(156,158,120)]
TR=[(12,12,10),(26,26,22),(40,40,34),(56,56,48),(74,72,62)]
RB=[(18,18,16),(30,30,26),(46,46,40)]
def tank_parts():
    P=[]
    # track run (behind wheels)
    P.append(dict(poly=[(-6,-14),(2,-20),(130,-20),(140,-14),(138,-4),(128,0),(4,0),(-4,-6)],ramp=TR,shade=cyl(0.2,0.0,0.8),col=True))
    # lower hull side (behind wheels, mostly hidden)
    P.append(dict(poly=[(0,-26),(132,-26),(134,-14),(2,-14)],ramp=AR[:5],shade=lin_v(0.6,0.1)))
    # road wheels
    for i in range(6):
        P.append(dict(circle=(14+i*20.5,-9,8.5),ramp=AR[:7],shade=lambda lx,ly,u,v:1.0-0.9*v-0.2*u))
        P.append(dict(circle=(14+i*20.5,-9,3.5),ramp=AR[1:6],shade=lambda lx,ly,u,v:0.2+0.8*v))
    # drive sprocket (rear) and idler (front)
    P.append(dict(circle=(133,-14,5.5),ramp=TR,shade=lin_v(0.9,0.2)))
    P.append(dict(circle=(-1,-13,5),ramp=TR,shade=lin_v(0.9,0.2)))
    # upper hull + glacis (front at left, sloped)
    P.append(dict(poly=[(-10,-24),(10,-34),(128,-34),(140,-30),(140,-24)],ramp=AR,shade=lin_v(0.95,0.35)))
    # side skirt with rubber flap
    P.append(dict(poly=[(-8,-24),(140,-24),(140,-17),(96,-17),(94,-15),(4,-15),(-6,-18)],ramp=AR,shade=lambda lx,ly,u,v:0.85-0.55*v-0.1*u))
    # turret ring shadow gap (weak spot)
    P.append(dict(poly=[(40,-34),(92,-34),(90,-37),(42,-37)],ramp=[INK,(20,20,16),(30,30,24)],shade=flat(0.4),edge=False))
    # turret (low dome, front-sloped)
    P.append(dict(poly=[(28,-40),(38,-48),(56,-53),(84,-53),(100,-49),(106,-42),(100,-37),(36,-37)],ramp=AR,shade=cyl(0.22,0.1,1.0),col=True))
    # ERA bricks on turret cheek
    for k in range(3):
        x=34+k*8
        P.append(dict(poly=[(x,-46+k*1),(x+7,-47+k*1),(x+7,-41),(x,-40)],ramp=AR[1:8],shade=lin_v(0.95,0.3)))
    # stowage bin on turret rear
    P.append(dict(poly=[(96,-46),(112,-46),(114,-39),(98,-38)],ramp=AR[:7],shade=lin_v(0.85,0.2)))
    # gun: mantlet, barrel, fume extractor, muzzle
    P.append(dict(poly=[(22,-46),(32,-46),(32,-38),(22,-38)],ramp=AR[:7],shade=cyl(0.3,0.1,0.9),col=True))
    P.append(dict(poly=[(-62,-44),(24,-45),(24,-41),(-62,-42)],ramp=AR,shade=cyl(0.3,0.05,0.95),col=True))
    P.append(dict(poly=[(-20,-46),(-6,-46),(-6,-40),(-20,-40)],ramp=AR,shade=cyl(0.3,0.05,0.95),col=True))
    P.append(dict(poly=[(-66,-45),(-60,-45),(-60,-41),(-66,-41)],ramp=AR[:6],shade=cyl(0.3,0.05,0.9),col=True))
    # commander cupola + MG
    P.append(dict(poly=[(70,-53),(72,-58),(84,-58),(86,-53)],ramp=AR[1:8],shade=lin_v(0.95,0.3)))
    P.append(dict(poly=[(58,-60),(74,-60),(74,-58),(58,-58)],ramp=TR,shade=flat(0.6)))
    # exhaust grille on rear deck
    P.append(dict(poly=[(112,-34),(132,-34),(130,-31),(114,-31)],ramp=TR,shade=flat(0.4)))
    return P
def tank_lines():
    D=AR[1]; H=AR[6]
    ln=[([(-6,-22),(138,-22)],H),([(20,-24),(20,-15)],D),([(48,-24),(48,-15)],D),([(76,-24),(76,-15)],D),([(104,-24),(104,-17)],D),
        ([(12,-33),(126,-33)],H),([(40,-50),(96,-50)],AR[6]),([(60,-44),(90,-44)],D),
        ([(-58,-43),(20,-44)],AR[7])]
    # track teeth
    for i in range(0,34):
        x=-2+i*4.1
        ln.append(([(x,0),(x,-1)],TR[3]))
    return ln,[]
def draw_tank(im,ox,oy,sc=1.0,ang=0.0):
    xf=Xf(ox,oy,sc,ang); ln,dots=tank_lines()
    return render(im,tank_parts(),xf,lines=ln,dots=dots),xf
