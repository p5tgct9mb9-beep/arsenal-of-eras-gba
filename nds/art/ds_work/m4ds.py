from dslib import *
POLY=[(12,12,13),(22,22,24),(34,34,37),(48,48,52),(64,64,68),(84,84,88),(108,108,112)]
ALU=[(14,15,18),(26,28,32),(40,42,48),(56,58,66),(74,76,86),(96,98,108),(122,124,134)]
STL=[(16,16,16),(30,30,30),(46,46,46),(64,64,64),(84,84,84),(106,106,106),(132,132,132)]
ODG=R((96,98,62))
def m4a1(att=()):
    att=set(att); P=[]; L=[]
    vs=lambda a,b: (lambda lx,ly,u,v:a+(b-a)*v)
    # buffer tube + stock
    P.append(W(rect(17,-6.6,31,-2.4),ALU,cyl(0.3,0.1,0.9),col=True))
    P.append(W([(0,-9),(4,-10),(19,-10),(21,-8),(21,-1),(17,0),(5,9),(0,9.5)],POLY,lambda lx,ly,u,v:0.9-0.55*v-0.1*u))
    P.append(W(rect(30,-7.2,32,-1.8),ALU,flat(0.55)))
    # lower receiver + magwell
    P.append(W([(31,-4.5),(57,-4.5),(57,0.5),(54,4),(45,4),(42,2.5),(31,1)],ALU,vs(0.85,0.35)))
    # pistol grip (angled back)
    P.append(W([(35,1),(41.5,1),(38,14.5),(31.5,14.5),(32.5,9)],POLY,lambda lx,ly,u,v:0.85-0.6*u))
    # trigger guard + trigger
    P.append(W(seg((41,6),(48,6),0.55),ALU,flat(0.5))); P.append(W(seg((48,3.5),(48,6),0.55),ALU,flat(0.5)))
    P.append(W(seg((44.5,3.5),(45.2,5.6),0.45),STL,flat(0.6)))
    # magazine (curved STANAG / extended)
    if 'EXT' in att: P.append(W([(45.5,3),(54,3),(57.5,20),(58,28),(50,29),(48.5,21)],ALU,lambda lx,ly,u,v:0.85-0.5*u))
    else: P.append(W([(45.5,3),(54,3),(56.5,15),(56.5,20),(48.5,21),(47.5,15)],ALU,lambda lx,ly,u,v:0.85-0.5*u))
    # upper receiver
    P.append(W(rect(30,-11,58,-4.5),ALU,vs(0.95,0.45)))
    P.append(W(rect(27.5,-11,31,-9),ALU,flat(0.5)))                       # charging handle
    # top rail (receiver + handguard)
    P.append(W(rect(30,-13,84,-11),ALU,flat(0.62)))
    # delta ring + rail handguard
    P.append(W(rect(57,-12,60,1),ALU,flat(0.4)))
    P.append(W([(60,-11),(84,-11),(84,0),(60,0)],POLY,vs(0.9,0.3)))
    P.append(W(rect(60,-1.5,84,0.5),ALU,flat(0.35)))                      # bottom rail
    # barrel + front sight base + gas block
    P.append(W(rect(84,-5.2,101,-2.2),STL,cyl(0.3,0.1,0.9),col=True))
    P.append(W([(86,-5.2),(87.4,-13.5),(89.6,-13.5),(91,-5.2)],STL,flat(0.45)))
    P.append(W(rect(86,-2.2,91,0.3),STL,flat(0.35)))
    if 'SUP' in att:
        P.append(W(rect(99,-7.6,128,0.4),R((64,66,60)),cyl(0.3,0.1,0.92),col=True))
        L+=[([(103,-7),(103,0)],(30,30,28)),([(124,-7),(124,0)],(30,30,28))]
    else: P.append(W(rect(100,-5.8,106,-1.6),STL,flat(0.5)))
    if 'GRIP' in att:
        P.append(W([(67,0.5),(74,0.5),(73.4,14),(67.6,14)],POLY,lambda lx,ly,u,v:0.85-0.6*u))
        L+=[([(68,5),(73,5)],POLY[1]),([(68,8),(73,8)],POLY[1]),([(68,11),(73,11)],POLY[1])]
    if 'LASER' in att:
        P.append(W(rect(70,-18,82,-13),ODG,vs(0.95,0.35))); P.append(W(rect(82,-17.5,83.5,-13.5),[(150,20,20),(230,50,40),(255,150,120)],flat(0.6)))
        L+=[([(73,-17),(73,-14)],ODG[1]),([(76,-17),(76,-14)],ODG[1])]
    if 'RDS' in att:
        P.append(W([(35,-13),(36,-15.5),(44,-15.5),(45,-13)],ALU,flat(0.45)))
        P.append(W(rect(33,-22.5,47,-15.5),ALU,cyl(0.3,0.1,0.9),col=True))
        P.append(W(rect(46,-23.5,48,-14.5),ALU,flat(0.6)))
        P.append(W(rect(31.5,-23.5,33.5,-14.5),ALU,flat(0.55)))
        P.append(W(rect(38,-25,41,-22.5),ALU,flat(0.5)))                  # turret
        L+=[([(34,-20),(46,-20)],(60,110,150))]
    elif '4X' in att:
        P.append(W(rect(32,-20,58,-15),ALU,cyl(0.3,0.1,0.9),col=True))
    else:
        P.append(W([(32,-13),(34,-17),(38,-17),(39,-13)],ALU,flat(0.5)))   # folding rear sight
    # detail lines
    for x in range(31,84,3): L.append(([(x,-13),(x,-12)],ALU[0]))          # rail slots
    for x in range(62,84,3): L.append(([(x,-10),(x,-1)],POLY[1]))          # handguard rail covers
    L+=[([(31,-8),(56,-8)],ALU[5]),([(38,-7),(48,-7)],ALU[1]),([(39,-6),(47,-6)],ALU[1]),      # ejection port
        ([(5,-6),(18,-6)],POLY[5]),([(3,-2),(18,-2)],POLY[1]),([(6,2),(14,2)],POLY[1]),        # stock ribs
        ([(1,-8),(1,8)],POLY[4]),([(19,-5.5),(30,-5.5)],ALU[5]),
        ([(36,-2),(36,-1)],ALU[5]),([(34,-1),(34,-1)],ALU[6]),([(54,-3),(55,-3)],ALU[6]),     # selector + pins
        ([(47,8),(54,8)],ALU[1]),([(48,12),(55,12)],ALU[1]),([(46,4),(53,4)],ALU[5]),
        ([(60,-10),(84,-10)],POLY[5]),([(86,-4.2),(100,-4.2)],STL[5])]
    return P,L
