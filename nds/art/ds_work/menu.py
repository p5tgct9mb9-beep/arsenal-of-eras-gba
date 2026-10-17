from dslib import *
OCEAN=[(6,14,40),(10,26,70),(16,44,104),(26,66,136),(44,96,166),(76,134,196)]
LAND=[(18,30,14),(34,52,22),(56,76,32),(84,98,44),(118,116,64),(150,138,88)]
DESERT=[(52,36,20),(96,70,40),(140,108,64),(176,146,96),(204,178,128)]
CLOUD=[(70,80,100),(120,130,150),(170,180,196),(214,220,232),(244,246,252)]
ICE=[(120,130,150),(180,190,206),(226,232,240)]
def earth(im,cx,cy,r):
    P=im.load(); W,H=im.size
    L=(-0.55,-0.62,0.56); n=math.sqrt(sum(a*a for a in L)); L=tuple(a/n for a in L)
    rnd=random.Random(3)
    for y in range(max(0,int(cy-r-4)),H):
        for x in range(W):
            dx,dy=(x+0.5-cx)/r,(y+0.5-cy)/r; d2=dx*dx+dy*dy
            if d2>1.0:
                # atmosphere glow
                dd=math.sqrt(d2)
                if dd<1.035:
                    k=(dd-1)/0.035
                    lit=max(0,-(dx*L[0]+dy*L[1]))
                    c=dith(0.95-k*0.9,x,y,[(10,20,50),(30,70,140),(70,150,220),(140,210,250),(200,240,255)])
                    if lit>0.1 or k<0.4: P[x,y]=c if (k<0.5 or (x+y)&1) else P[x,y]
                continue
            z=math.sqrt(1-d2)
            ta=math.radians(52); upv=-dy
            up2=upv*math.cos(ta)-z*math.sin(ta); z2=upv*math.sin(ta)+z*math.cos(ta)
            lon=math.atan2(dx,z2)+0.9; lat=math.asin(max(-1,min(1,up2)))
            u=lon*38; v=lat*38
            h=noise(u,v,7)*0.62+noise(u*2.1+50,v*2.1,5)*0.38
            lit=dx*L[0]+dy*L[1]+z*L[2]
            sh=max(0.0,min(1.0,0.15+0.95*lit))
            if abs(lat)>1.25: c=dith(0.3+0.7*sh,x,y,ICE)
            elif h>0.56:
                dry=noise(u+90,v+20,6)
                c=dith(0.15+0.85*sh*(0.6+0.6*(h-0.56)),x,y,DESERT if dry>0.6 and abs(lat)<0.6 else LAND)
            else:
                c=dith(0.05+0.95*sh*(0.75+0.5*h),x,y,OCEAN)
                if 0.53<h<0.56 and sh>0.3: c=OCEAN[4]
            cl=noise(u*1.4+200,v*2.4,4)*0.7+noise(u*3+11,v*3,3)*0.3
            if cl>0.6:
                a=min(1,(cl-0.6)*6)
                if a>0.6 or (x+y)&1: c=dith((0.25+0.75*sh)*min(1,0.6+a*0.5),x,y,CLOUD)
            if sh<0.12:                        # night side + city lights
                c=dith(0.25+sh*3,x,y,[(2,4,10),(6,10,24),(10,18,40)])
                if h>0.56 and rnd.random()<0.06: c=(255,214,120) if rnd.random()<0.5 else (230,160,70)
            P[x,y]=c
SOL=[(40,24,10),(84,52,20),(130,84,30),(176,120,44),(214,160,70),(240,200,110)]
WHT=[(40,42,48),(80,84,92),(130,134,142),(176,180,186),(214,216,220),(240,240,242)]
def iss(im,ox,oy,sc=1.0):
    P=[]; L=[]
    # radiators (white, perpendicular)
    for x in (-10,10):
        P.append(dict(poly=[(x-2,-2),(x+2,-2),(x+3,-11),(x-3,-11)],ramp=WHT,shade=lin_u(0.95,0.5)))
    # main truss
    P.append(dict(poly=[(-44,-1.4),(44,-1.4),(44,1.4),(-44,1.4)],ramp=WHT,shade=lin_v(0.95,0.3)))
    # solar wings: 4 per side (2 up 2 down)
    for sx in (-1,1):
        for k,x in enumerate((30,40)):
            for sy in (-1,1):
                y0,y1=(-3,-22) if sy<0 else (3,22)
                P.append(dict(poly=[(sx*x-3.5,y0),(sx*x+3.5,y0),(sx*x+3.5,y1),(sx*x-3.5,y1)],ramp=SOL,shade=lambda lx,ly,u,v:0.85-0.5*u-0.15*abs(v-0.5)))
                for yy in range(4,22,3): L.append(([(sx*x-3,sy*yy),(sx*x+3,sy*yy)],SOL[1]))
                L.append(([(sx*x,y0),(sx*x,y1)],SOL[1]))
    # pressurised modules (vertical stack + lab crosswise)
    P.append(dict(poly=[(-3,-16),(3,-16),(3,16),(-3,16)],ramp=WHT,shade=lin_u(1.0,0.35)))
    P.append(dict(poly=[(-12,3),(12,3),(12,7),(-12,7)],ramp=WHT,shade=lin_v(1.0,0.4)))
    P.append(dict(poly=[(-4.5,-6),(4.5,-6),(4.5,0),(-4.5,0)],ramp=WHT,shade=lin_u(0.95,0.4)))
    # small service-module wings at bottom
    P.append(dict(poly=[(-14,14),(-4,14),(-4,17),(-14,17)],ramp=[(16,24,60),(30,50,110),(60,90,160)],shade=lin_u(0.9,0.2)))
    P.append(dict(poly=[(4,14),(14,14),(14,17),(4,17)],ramp=[(16,24,60),(30,50,110),(60,90,160)],shade=lin_u(0.9,0.2)))
    for y in (-12,-8,8,12): L.append(([(-3,y),(3,y)],WHT[1]))
    for x in range(-42,44,4): L.append(([(x,-1),(x+2,1)],WHT[2]))
    at(im,P,L,ox,oy,sc)
def sat(im,ox,oy,sc=1.0,ang=0):
    BLU=[(10,16,44),(20,34,90),(36,60,140),(70,100,180),(120,150,210)]
    FOIL=[(60,40,6),(120,86,20),(184,140,40),(230,190,80),(255,230,140)]
    P=[dict(poly=[(-22,-3),(-6,-3),(-6,3),(-22,3)],ramp=BLU,shade=lin_u(0.9,0.4)),
       dict(poly=[(6,-3),(22,-3),(22,3),(6,3)],ramp=BLU,shade=lin_u(0.8,0.3)),
       dict(poly=[(-7,-0.6),(7,-0.6),(7,0.6),(-7,0.6)],ramp=WHT,shade=flat(0.6)),
       dict(poly=[(-4.5,-5),(4.5,-5),(4.5,5),(-4.5,5)],ramp=FOIL,shade=lambda lx,ly,u,v:0.95-0.6*u-0.2*v),
       dict(poly=[(-2,-5),(2,-5),(4,-10),(-4,-10)],ramp=WHT,shade=lin_u(0.95,0.4))]
    L=[([(-14,-3),(-14,3)],BLU[0]),([(14,-3),(14,3)],BLU[0]),([(-4,0),(4,0)],FOIL[1])]
    at(im,P,L,ox,oy,sc,ang)
STEEL=[(40,44,52),(70,76,86),(104,110,120),(140,146,156),(178,184,192),(214,218,224),(240,242,246)]
TILE=[(34,36,46),(52,54,66),(72,74,88),(94,96,110)]
def starship(im,ox,oy,sc=1.0,ang=-28):
    """stainless two-stage stack. local: nose y=-120, engines y=0, width 9 (~ 121m x 9m)"""
    sh=lambda lx,ly,u,v: 0.95-1.1*abs(u-0.32)
    P=[]; L=[]
    P.append(dict(poly=[(-4.5,-2),(4.5,-2),(4.5,-70),(-4.5,-70)],ramp=STEEL,shade=sh))           # booster
    for y in (-66,):                                                                            # grid fins
        P.append(dict(poly=[(-8,y),(-4.5,y),(-4.5,y+4),(-8,y+4)],ramp=STEEL[:5],shade=flat(0.4)))
        P.append(dict(poly=[(4.5,y),(8,y),(8,y+4),(4.5,y+4)],ramp=STEEL[:5],shade=flat(0.5)))
    P.append(dict(poly=[(-4.5,-70),(4.5,-70),(4.5,-73),(-4.5,-73)],ramp=TILE,shade=flat(0.5)))   # hot-stage ring
    P.append(dict(poly=[(-4.5,-73),(4.5,-73),(4.5,-108),(3.6,-114),(1.5,-119),(-1.5,-119),(-3.6,-114),(-4.5,-108)],ramp=STEEL,shade=sh))  # ship
    P.append(dict(poly=[(-4.5,-73),(-1.2,-73),(-1.2,-111),(-2.6,-116),(-3.6,-114),(-4.5,-108)],ramp=TILE,shade=lin_u(0.9,0.3),edge=False))  # heat shield
    P.append(dict(poly=[(-4.5,-76),(-8,-74),(-8,-84),(-4.5,-86)],ramp=TILE,shade=flat(0.5)))   # aft flaps
    P.append(dict(poly=[(4.5,-76),(8,-74),(8,-84),(4.5,-86)],ramp=STEEL[:6],shade=flat(0.45)))
    P.append(dict(poly=[(-3.8,-106),(-6.5,-104),(-6,-112),(-3.4,-112)],ramp=TILE,shade=flat(0.5)))  # fwd flaps
    P.append(dict(poly=[(3.8,-106),(6.5,-104),(6,-112),(3.4,-112)],ramp=STEEL[:6],shade=flat(0.45)))
    P.append(dict(poly=[(-4,0),(4,0),(4,-2),(-4,-2)],ramp=TILE,shade=flat(0.4)))
    for y in range(-60,-4,8): L.append(([(-1,y),(4,y)],STEEL[2]))
    for y in range(-100,-76,8): L.append(([(-1,y),(4,y)],STEEL[2]))
    L.append(([(1,-68),(1,-4)],STEEL[6]))
    return at(im,P,L,ox,oy,sc,ang)
def exhaust(im,ox,oy,sc,ang):
    """flame + dithered trail behind engines (local +y is aft)"""
    a=math.radians(ang); c,s=math.cos(a),math.sin(a)
    def f(x,y): return (ox+(x*c+y*s)*sc, oy+(-x*s+y*c)*sc)
    P=im.load(); W,H=im.size
    FL=[(120,40,10),(220,110,30),(255,180,60),(255,236,150),(255,255,230)]
    TR=[(60,64,90),(100,106,128),(150,154,170),(196,198,206),(236,236,240)]
    rnd=random.Random(9)
    for k in range(0,1600):
        t=k/1600
        L=t*220; w=3.0+t*16+2*math.sin(t*20)
        for j in range(3):
            o=(rnd.random()*2-1)*w
            bend=-(L**2)*0.0042
            X,Y=f(o+bend,L+2)
            X,Y=int(X),int(Y)
            if 0<=X<W and 0<=Y<H:
                if L<16: col=dith(1-L/16,X,Y,FL) if abs(o)<4.5-L*0.15 else FL[1]
                else:
                    if rnd.random()>0.25+t*0.6: continue
                    col=dith(0.95-t*0.9-abs(o)/max(w,1)*0.3,X,Y,TR)
                P[X,Y]=col
def top():
    im=Image.new('RGB',(SW,SH)); P=im.load()
    for y in range(SH):
        for x in range(SW):
            n=noise(x,y,18)
            P[x,y]=dith(0.08+0.22*n*max(0,1-abs(y-70-(x-128)*0.4)/80),x,y,[(2,4,12),(6,10,26),(14,16,44),(28,22,62)])
    stars(im,170,seed=11)
    earth(im,128,432,320)
    sat(im,30,64,0.7,20); sat(im,232,104,0.5,-15)
    iss(im,196,74,0.9)
    exhaust(im,88,136,1.0,-40)
    starship(im,88,136,0.78,-40)
    title(im,8,'ARSENAL OF ERAS')
    ctext(im,128,34,'DS EDITION',CYAN)
    tag(im,SW-41,182)
    return im
if __name__=='__main__':
    t=top(); up(t,3).save('menu_top.png')
from icons import *
MODES=[('CAMPAIGN','CAMPAIGN',None,'BY ERA'),('INFANTRY','INFANTRY','6V6',None),('DOGFIGHT','DOGFIGHT',None,'JETS'),('ARMOR','ARMOR',None,'TANKS'),
       ('NAVAL','NAVAL',None,'FLEETS'),('SPACE','SPACE',None,'ROCKETS'),('WORLD','WORLD','FLIGHT',None),('ARCADE','ARCADE',None,'QUICK'),
       ('STORE','STORE',None,'ROCKS'),('GUNSMITH','GUNSMITH',None,'BUILD'),('SPECOPS','SPEC OPS',None,'SOF'),('SETTINGS','SETTINGS',None,'REALISM')]
def bottom():
    im=Image.new('RGB',(SW,SH)); P=im.load()
    for y in range(SH):
        for x in range(SW): P[x,y]=dith(0.2+0.5*y/SH,x,y,[(4,6,14),(8,12,28),(12,20,40),(16,26,52)])
    d=ImageDraw.Draw(im)
    # header: profile chip + rocks balance
    nbevel(im,1,1,118,18,fill=NAVYF)
    s=spec(skin=4,hair='BLACK',face='STUBBLE',head='M1',unif='OLIVE DRAB')
    portrait(im,3,3,17,17,s,sc=1.5,top=-50)
    text(im,21,4,'HALE',GOLD); text(im,49,4,'SGT',CYAN); text(im,73,4,'LV 12',WHITE)
    nbevel(im,182,1,254,18,fill=NAVYF)
    rock(im,186,6); text(im,198,6,'2,400',PR); 
    text(im,124,6,'MARS',(220,120,80)); text(im,150,6,'ROCKS',(220,120,80))
    cw,ch=63,51; y0=21
    for i,(key,a,b,sub) in enumerate(MODES):
        cx=1+(i%4)*cw+(i%4>1); cy=y0+(i//4)*ch
        x1=cx+cw-2; y1=cy+ch-2
        nbevel(im,cx,cy,x1,y1,fill=(30,48,86) if i==0 else PANELN,sel=(i==0))
        bx=(cx+4,cy+4,x1-3,cy+29)
        if key=='INFANTRY':
            sp=spec(skin=3,hair='BROWN',head='HELMET',unif='DESERT TAN',camo='DIGITAL',gear='PLATE',rifle='M4',helmcol=(150,132,96))
            img,(ax,ay),m=sprite(sp,0.5); im.paste(img,((bx[0]+bx[2])//2-ax,bx[3]+2-ay),img)
        else:
            Pp,Ll=ICONS[key](); xf=fit(im,Pp,Ll,bx,maxsc={'CAMPAIGN':1.1,'SPACE':0.85,'ARCADE':1.0,'STORE':1.0,'SETTINGS':1.0,'WORLD':1.0,'DOGFIGHT':0.62}.get(key))
            if key=='SPECOPS':
                for a_,b_ in (((-17,-3),(-2,9)),((17,-3),(2,9)),((-6,-2),(-1,9)),((6,-2),(1,9))):
                    d.line((*xf.f(*a_),*xf.f(*b_)),fill=(200,200,180))
                Pp,Ll=ICONS[key](); at(im,Pp[1:],[],xf.ox,xf.oy,xf.sc)
        ccx=(cx+x1)//2
        ctext(im,ccx,cy+32,a,GOLD if i==0 else WHITE)
        if b: ctext(im,ccx,cy+41,b,GOLD if i==0 else WHITE)
        elif sub: ctext(im,ccx,cy+41,sub,(110,170,210))
    # footer
    d.rectangle((0,175,255,191),fill=(6,8,18)); d.line((0,175,255,175),fill=(58,120,170))
    text(im,4,180,'TAP A MODE',CYAN); text(im,72,180,'CAMPAIGN: BY ERA',(170,180,200))
    tag(im,SW-41,179)
    return im
if __name__=='__main__':
    b=bottom(); up(b,3).save('v_mb1.png')
