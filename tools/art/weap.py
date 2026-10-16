from sold import *
BLUED=[(14,16,20),(28,30,36),(44,48,56),(64,68,78),(88,92,104),(116,120,132),(150,154,164)]
PARK=[(16,16,14),(30,30,26),(46,46,40),(64,64,56),(86,86,76),(110,110,98),(140,140,126)]
WD=[(36,20,10),(64,36,18),(94,56,28),(124,78,40),(152,100,56),(180,126,76),(206,156,104)]
OD=R((88,92,56))
BLK=[(12,12,12),(24,24,26),(38,38,40),(54,54,58),(74,74,78),(98,98,102),(126,126,130)]
GLS=[(20,30,40),(40,70,96),(70,120,150),(140,190,210)]
def V(a=0.95,b=0.25): return lambda lx,ly,u,v:a+(b-a)*v
def W(poly,ramp,sh=None,**k): return dict(poly=poly,ramp=ramp,shade=sh or V(),**k)
def rect(x0,y0,x1,y1): return [(x0,y0),(x1,y0),(x1,y1),(x0,y1)]
def thompson():
    P=[W([(0,-1),(2,-4),(30,-6.5),(30,0.5),(4,8),(0,8.5)],WD,lambda lx,ly,u,v:0.9-0.55*v-0.1*u),
       W([(36,0),(43,0),(42,13),(34,13),(33.5,9)],WD,lambda lx,ly,u,v:0.85-0.6*u),
       W(rect(46,0,52,22),BLUED,lambda lx,ly,u,v:0.8-0.5*u),
       W([(28,-8),(62,-8),(64,-6),(64,1),(28,1)],PARK),
       W(rect(62,-6,99,-3),PARK,V(0.9,0.3)),
       W([(64,-3),(84,-3),(84,1.5),(66,1.5)],WD,V(0.9,0.3)),
       W([(36,1),(44,1),(44,5),(40,6),(37,4)],PARK,flat(0.3),edge=False),
       W(rect(96,-9,98,-6),PARK,flat(0.5)),W(rect(30,-10,34,-8),PARK,flat(0.5)),W(rect(52,-9,56,-8),PARK,flat(0.6))]
    L=[([(4,2),(28,-2)],WD[2]),([(28,-4),(62,-4)],PARK[5]),([(40,-6),(48,-6)],PARK[1]),([(47,4),(51,4)],BLUED[4])]
    return P,L
def m1911():
    P=[W([(3,2),(16,2),(13,23),(12,25),(1,25),(-1,22)],BLUED,lambda lx,ly,u,v:0.8-0.6*u),
       W([(4,5),(14,5),(11.5,22),(1,22)],WD,lambda lx,ly,u,v:0.9-0.6*u-0.1*v),
       W([(3,0),(36,0),(36,4),(26,4),(25,9),(17,9),(16,4),(3,4)],BLUED,lambda lx,ly,u,v:0.7-0.2*v),
       W([(17.5,4),(24,4),(23.5,7.5),(18,7.5)],BLUED,flat(0.15),edge=False),
       W([(0,-6),(38,-6),(38,0),(0,0)],BLUED,V(0.95,0.4)),
       W(rect(35,-8,37,-6),BLUED,flat(0.6)),W([(-2,-7),(2,-7),(2,-3),(-1,-4)],BLUED,flat(0.6))]
    L=[([(2,-3),(30,-3)],BLUED[1]),([(4,-5),(4,-1)],BLUED[1]),([(6,-5),(6,-1)],BLUED[1]),([(8,-5),(8,-1)],BLUED[1]),
       ([(4,9),(13,9)],WD[5]),([(3,14),(12,14)],WD[1]),([(2,18),(11,18)],WD[5])]
    return P,L
def garand():
    P=[W([(0,-1),(3,-4.5),(26,-5),(34,-3),(72,-3),(74,-0.5),(74,2),(36,2),(28,1.5),(24,4),(4,9),(0,9)],WD,lambda lx,ly,u,v:0.92-0.6*v-0.1*u),
       W(rect(70,-5.5,102,-3.4),PARK,V(0.9,0.3)),
       W([(30,-8),(46,-8),(48,-6),(48,-2.5),(30,-2.5)],PARK),
       W([(48,-6.5),(72,-6.5),(73,-3),(48,-3)],WD,V(0.95,0.4)),
       W(rect(84,-6,86,-2),PARK,flat(0.4)),W(rect(99.5,-8,101,-5.5),PARK,flat(0.5)),W(rect(28,-10,32,-8),PARK,flat(0.5)),
       W([(36,2),(44,2),(43,5),(38,5)],PARK,flat(0.3),edge=False)]
    L=[([(4,2),(24,-2)],WD[3]),([(34,-6),(46,-6)],PARK[5]),([(50,-4.5),(70,-4.5)],WD[5]),([(76,-1),(84,-1)],PARK[2])]
    return P,L
def bar():
    P=[W([(0,-2),(3,-5),(24,-5.5),(24,2),(4,10),(0,10)],WD,lambda lx,ly,u,v:0.9-0.6*v-0.1*u),
       W([(24,-7),(60,-7),(62,-5),(62,3),(24,3)],PARK),
       W(rect(42,3,52,18),PARK,lambda lx,ly,u,v:0.8-0.5*u),
       W([(28,3),(35,3),(34,12),(28,12)],WD,lambda lx,ly,u,v:0.85-0.6*u),
       W([(62,-5),(86,-5),(86,1.5),(64,1.5)],WD,V(0.9,0.3)),
       W(rect(60,-5.5,104,-2.8),PARK,V(0.9,0.3)),
       W(rect(100,-6.5,106,-2),PARK,flat(0.35)),
       W(seg((92,-2),(84,14),0.7),PARK,flat(0.5)),W(seg((92,-2),(97,14),0.7),PARK,flat(0.6)),W(rect(26,-9,30,-7),PARK,flat(0.5))]
    L=[([(4,2),(24,-2)],WD[3]),([(26,-4),(60,-4)],PARK[5]),([(44,7),(50,7)],PARK[5]),([(44,11),(50,11)],PARK[5])]
    return P,L
def bazooka():
    P=[W(rect(0,-6,112,4),OD,cyl(0.3,0.1,0.95),col=True),
       W(rect(0,-7,4,5),OD[:5],cyl(0.3,0.1,0.8),col=True),W(rect(108,-7.5,114,5.5),OD,cyl(0.3,0.1,0.95),col=True),
       W([(26,4),(48,4),(46,11),(28,11)],WD,V(0.9,0.3)),
       W([(56,4),(62,4),(62,16),(57,16)],WD,lambda lx,ly,u,v:0.85-0.6*u),
       W([(70,4),(76,4),(76,15),(71,15)],WD,lambda lx,ly,u,v:0.85-0.6*u),
       W(rect(52,-11,56,-6),PARK,flat(0.5)),W(rect(90,-10,92,-6),PARK,flat(0.5))]
    L=[([(14,-6),(14,4)],OD[1]),([(84,-6),(84,4)],OD[1]),([(6,-4),(106,-4)],OD[6])]
    return P,L
def m4(att=()):
    att=set(att)
    P=[W(rect(10,-5.5,24,-2),BLK,cyl(0.3,0.1,0.9),col=True),
       W([(0,-6),(3,-7),(14,-7),(15,-5),(15,0),(12,0),(2,6),(0,6)],BLK,lambda lx,ly,u,v:0.85-0.6*v),
       W([(36,1),(42,1),(40,13),(34,13)],BLK,lambda lx,ly,u,v:0.8-0.6*u),
       W([(22,-7),(56,-7),(56,2),(22,2)],BLK),
       W(rect(56,-6.5,82,1),BLK,V(0.9,0.35)),
       W(rect(82,-4.8,100,-2.2),BLK,V(0.9,0.3)),
       W([(44,2),(52,2),(56,17 if 'EXT' not in att else 26),(49,18.5 if 'EXT' not in att else 27.5)],BLK,lambda lx,ly,u,v:0.8-0.5*u),
       W([(36,2),(44,2),(43,5),(38,5)],BLK,flat(0.2),edge=False),
       W(rect(24,-9,80,-7),BLK,flat(0.55))]
    L=[([(58,-4),(80,-4)],BLK[1]),([(58,-1),(80,-1)],BLK[1]),([(24,-4),(54,-4)],BLK[5])]
    for x in range(26,80,3): L.append(([(x,-9),(x,-8)],BLK[1]))
    if 'SUP' in att: P.append(W(rect(98,-6.5,124,-0.5),PARK,cyl(0.3,0.1,0.9),col=True)); L.append(([(102,-6),(102,-1)],PARK[1]))
    else: P.append(W(rect(97,-5.5,102,-1.5),BLK,flat(0.5))); P.append(W(rect(78,-12,80,-9),BLK,flat(0.5)))
    if 'GRIP' in att: P.append(W([(64,1),(70,1),(69.5,13),(65,13)],BLK,lambda lx,ly,u,v:0.8-0.6*u))
    if 'BIPOD' in att: P.append(W(seg((76,1),(70,18),0.8),BLK,flat(0.5))); P.append(W(seg((76,1),(84,18),0.8),BLK,flat(0.6)))
    if 'LASER' in att: P.append(W(rect(70,-12,79,-9),OD,V(0.9,0.3))); P.append(W(rect(79,-11.5,80,-9.5),[(160,20,20),(240,60,40),(255,140,120)],flat(0.6)))
    if 'RDS' in att:
        P.append(W([(30,-9),(32,-15),(42,-15),(44,-9)],BLK,V(0.9,0.3)))
        P.append(W(rect(33,-14,41,-11),GLS,flat(0.4),edge=False))
    if '4X' in att:
        P.append(W(rect(30,-16,58,-11),BLK,cyl(0.3,0.1,0.9),col=True)); P.append(W(rect(26,-17,31,-10),BLK,flat(0.5))); P.append(W(rect(56,-17.5,62,-9.5),BLK,flat(0.6)))
        P.append(W(rect(36,-11,40,-9),BLK,flat(0.3))); P.append(W(rect(48,-11,52,-9),BLK,flat(0.3)))
    if 'SNIPE' in att:
        P.append(W(rect(24,-17,64,-11),BLK,cyl(0.3,0.1,0.9),col=True)); P.append(W(rect(18,-18.5,26,-9.5),BLK,flat(0.5))); P.append(W(rect(62,-19.5,70,-8.5),BLK,flat(0.6)))
        P.append(W(rect(40,-20,44,-17),BLK,flat(0.4)))
    return P,L
# attachment icons (standalone)
def a_rds(): return [W([(0,4),(3,-6),(17,-6),(20,4)],BLK,V()),W(rect(4,-4,16,1),GLS,flat(0.45),edge=False),W(rect(-1,4,21,7),BLK,flat(0.4))],[([(9,-2),(10,-2)],(255,60,40))]
def a_4x(): return [W(rect(6,-4,34,3),BLK,cyl(0.3,0.1,0.9),col=True),W(rect(0,-6,7,5),BLK,flat(0.5)),W(rect(33,-7,40,6),BLK,flat(0.6)),W(rect(14,3,18,6),BLK,flat(0.3)),W(rect(24,3,28,6),BLK,flat(0.3))],[([(34,-5),(34,4)],GLS[2])]
def a_snipe(): return [W(rect(8,-4,48,3),BLK,cyl(0.3,0.1,0.9),col=True),W(rect(0,-6,9,5),BLK,flat(0.5)),W(rect(47,-8,56,7),BLK,flat(0.6)),W(rect(26,-8,30,-4),BLK,flat(0.4)),W(rect(16,3,20,6),BLK,flat(0.3)),W(rect(36,3,40,6),BLK,flat(0.3))],[([(56,-7),(56,6)],GLS[2])]
def a_sup(): return [W(rect(0,-4,40,4),PARK,cyl(0.3,0.1,0.9),col=True)],[([(5,-4),(5,4)],PARK[1]),([(35,-4),(35,4)],PARK[1])]
def a_grip(): return [W(rect(0,-4,20,0),BLK,flat(0.5)),W([(5,0),(14,0),(13,22),(6,22)],BLK,lambda lx,ly,u,v:0.85-0.6*u)],[([(7,6),(12,6)],BLK[1]),([(7,11),(12,11)],BLK[1]),([(7,16),(12,16)],BLK[1])]
def a_laser(): return [W(rect(0,-4,26,4),OD,V(0.95,0.3)),W(rect(26,-3,29,3),[(150,20,20),(230,50,40),(255,150,120)],flat(0.6)),W(rect(4,4,22,6),BLK,flat(0.4))],[([(29,0),(44,0)],(255,60,40))]
def a_bipod(): return [W(rect(10,-3,26,2),BLK,V()),W(seg((14,2),(2,24),1.2),BLK,flat(0.55)),W(seg((22,2),(34,24),1.2),BLK,flat(0.65))],[]
def a_ext(): return [W([(0,0),(10,0),(14,34),(4,35)],BLK,lambda lx,ly,u,v:0.85-0.6*u),W(rect(-1,-3,11,0),BLK,flat(0.5))],[([(3,12),(11,12)],BLK[5])]
def binos(): 
    P=[]
    for x0 in (0,14):
        P.append(W(rect(x0,0,x0+11,22),BLK,lambda lx,ly,u,v:0.9-0.6*u))
        P.append(W(rect(x0+1,-6,x0+10,0),BLK,flat(0.6)))
    P.append(W(rect(10,4,15,10),BLK,flat(0.4))); P.append(W(rect(1,21,10,25),BLK,flat(0.3)))
    return P,[([(2,6),(8,6)],BLK[5]),([(16,6),(22,6)],BLK[5])]
ITEMS=dict(THOMPSON=thompson,M1911=m1911,GARAND=garand,BAR=bar,BAZOOKA=bazooka,BINO=binos,
           RDS=a_rds,X4=a_4x,SNIPE=a_snipe,SUP=a_sup,GRIP=a_grip,LASER=a_laser,BIPOD=a_bipod,EXT=a_ext)
def draw_item(im,fn,box,maxsc=None,align='c'):
    P,L=fn() if callable(fn) else fn
    xs=[x for p in P for x,y in p['poly']]; ys=[y for p in P for x,y in p['poly']]
    for pts,_ in L:
        xs+= [x for x,y in pts]; ys+=[y for x,y in pts]
    x0,y0,x1,y1=box; bw=max(xs)-min(xs); bh=max(ys)-min(ys)
    sc=min((x1-x0-2)/bw,(y1-y0-2)/bh); 
    if maxsc: sc=min(sc,maxsc)
    ox=x0+(x1-x0-bw*sc)/2-min(xs)*sc; oy=y0+(y1-y0-bh*sc)/2-min(ys)*sc
    if align=='b': oy=y1-1-max(ys)*sc
    crop=im.crop((x0,y0,x1,y1)); xf=Xf(ox-x0,oy-y0,sc)
    render2(crop,P,xf,lines=L); im.paste(crop,(x0,y0))
    return xf,(x0,y0)
