from hd import *
# original F-16C-style side view, nose at x=0, centreline y=0, ~190 units long
G=GREY
GLASS=[(14,16,18),(30,34,36),(52,58,58),(84,90,86),(128,132,124),(196,200,190)]
METAL=[(20,20,20),(40,38,36),(64,60,56),(92,86,78),(120,112,100)]
DARKG=G[:6]
def jet_parts():
    P=[]
    # far-side horizontal stabiliser (behind body)
    P.append(dict(poly=[(150,-2),(172,-3),(190,-6),(186,-2),(168,1)],ramp=G[:5],shade=lin_u(0.9,0.3)))
    # vertical fin
    P.append(dict(poly=[(126,-11),(148,-44),(160,-45),(166,-44),(176,-7)],ramp=G[1:8],shade=lambda lx,ly,u,v:0.95-0.55*v-0.25*u))
    # fuselage
    P.append(dict(poly=[(0,2),(6,-1),(16,-5),(30,-8),(44,-10),(80,-12),(130,-11),(160,-8),(178,-5),(180,5),(166,8),(130,10),(80,10),(40,10),(22,8),(10,5)],
                  ramp=G,shade=cyl(0.28,0.05,0.88),col=True))
    # radome (darker grey cone)
    P.append(dict(poly=[(0,2),(6,-1),(16,-5),(19,-5),(19,7),(10,5)],ramp=G[:7],shade=cyl(0.3,0.05,0.9),col=True))
    # chin intake
    P.append(dict(poly=[(44,9),(54,8),(100,10),(112,10),(116,14),(104,17),(60,18),(48,18),(43,14)],ramp=G[:7],shade=lin_v(0.75,0.0)))
    P.append(dict(poly=[(43,10),(47,10),(48,18),(44,17)],ramp=[INK,(22,22,22)],shade=flat(0.3),edge=False))
    # dorsal spine / canopy fairing
    P.append(dict(poly=[(56,-11),(70,-15),(110,-14),(134,-11)],ramp=G[2:9],shade=lin_v(1.0,0.4)))
    # canopy bubble
    P.append(dict(poly=[(24,-8),(30,-13),(38,-18),(48,-21),(58,-20),(66,-17),(72,-12),(70,-10),(40,-9)],ramp=GLASS,
                  shade=lambda lx,ly,u,v: 0.15+0.55*v*(1-u)+0.2*(1-abs(u-0.5)*2)))
    # wing (near side, seen slightly from above)
    P.append(dict(poly=[(66,3),(112,2),(140,5),(152,16),(144,19),(120,17),(84,11)],ramp=G[1:9],shade=lambda lx,ly,u,v:0.95-0.6*u-0.2*v))
    # leading edge flap strip
    P.append(dict(poly=[(66,3),(84,11),(88,10),(72,3)],ramp=G[3:9],shade=flat(0.9)))
    # near horizontal stabiliser
    P.append(dict(poly=[(150,3),(170,3),(192,10),(186,13),(164,9)],ramp=G[1:8],shade=lin_u(0.9,0.2)))
    # ventral fin
    P.append(dict(poly=[(140,9),(152,9),(150,17),(146,17)],ramp=G[:6],shade=lin_v(0.8,0.1)))
    # exhaust nozzle
    P.append(dict(poly=[(176,-5),(190,-4),(191,6),(176,6)],ramp=METAL,shade=cyl(0.3,0.0,0.9),col=True))
    # wingtip missile rail + AIM-9
    P.append(dict(poly=[(120,17),(152,17),(154,19),(120,19)],ramp=G[2:9],shade=lin_v(1.0,0.2)))
    P.append(dict(poly=[(116,18),(120,17),(120,19)],ramp=G[2:6],shade=flat(0.5)))
    return P
def jet_lines():
    D=(36,37,39); L=(206,206,204)
    ln=[([(19,-5),(19,7)],D),([(76,-12),(76,-2)],D),([(104,-12),(104,-4)],D),([(132,-11),(132,-2)],D),
        ([(22,2),(64,2)],(70,71,74)),([(96,-1),(170,-1)],(70,71,74)),([(150,-42),(171,-7)],D),([(168,-7),(177,-7)],D),
        ([(180,-4),(180,5)],(30,30,30)),([(184,-4),(184,6)],(36,34,32)),
        ([(33,-14),(43,-19),(54,-19),(60,-18)],L),([(36,-14),(44,-18)],(240,240,232)),([(31,-12),(35,-15)],(150,156,150)),([(72,-12),(70,-10)],D),
        ([(96,6),(134,13)],D),([(60,12),(104,13)],(50,51,54)),([(154,-34),(162,-34)],(62,63,66)),
        ([(140,-24),(146,-24)],(150,150,152)),([(84,-6),(92,-6)],(82,83,86))]
    dots=[(88,-4,(150,40,40)),(89,-4,(220,220,220)),(90,-4,(40,60,150)),(160,-30,(150,40,40)),(161,-30,(220,220,220))]
    return ln,dots
def draw_jet(im,ox,oy,sc=1.0,ang=0.0):
    xf=Xf(ox,oy,sc,ang); ln,dots=jet_lines()
    return render(im,jet_parts(),xf,lines=ln,dots=dots)
if __name__=='__main__':
    from gbalib import up
    bg=bands(240,160,[(0,160,OLIVE[4],OLIVE[4],'solid'),(0,8,OLIVE[0],OLIVE[0],'solid'),(8,14,OLIVE[0],OLIVE[2],'checker'),(14,20,OLIVE[2],OLIVE[3],'checker'),
        (20,26,OLIVE[3],OLIVE[4],'checker'),(118,124,OLIVE[4],OLIVE[3],'checker'),(124,132,OLIVE[3],OLIVE[1],'checker'),(132,146,OLIVE[1],OLIVE[0],'checker'),(146,160,OLIVE[0],OLIVE[0],'solid')])
    m=draw_jet(bg,22,82,1.0,0)
    shadow(bg,m,26,0.15)
    up(bg).save('jet_test.png')
