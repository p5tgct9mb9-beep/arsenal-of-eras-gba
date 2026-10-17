from dslib import *
G=GREY
GL=[(10,14,20),(24,36,50),(44,70,92),(90,130,150),(170,200,210)]
NOZ=[(20,18,16),(44,40,36),(70,64,56),(98,90,80),(124,116,104)]
def f16_rear():
    P=[]; L=[]
    wsh=lambda lx,ly,u,v:0.95-0.45*v-0.2*abs(u-0.5)
    # far parts first: canopy region / dorsal deck / LERX
    P.append(dict(poly=[(-8,-12),(8,-12),(5,-23),(-5,-23)],ramp=G[1:],shade=lambda lx,ly,u,v:0.95-0.5*v))       # dorsal spine going forward
    P.append(dict(poly=[(-4.5,-22),(-3.5,-26),(0,-27.5),(3.5,-26),(4.5,-22)],ramp=GL,shade=lambda lx,ly,u,v:0.95-0.7*v-0.2*u))  # canopy
    for s in (-1,1):
        # main wing (top surface visible)
        P.append(dict(poly=[(s*10,0),(s*47,0.5),(s*48,-2.5),(s*16,-11)],ramp=G,shade=wsh))
        # wingtip AIM-9 + rail
        P.append(dict(poly=[(s*46.5,-1),(s*50,-1),(s*50,1.5),(s*46.5,1.5)],ramp=G[:6],shade=flat(0.5)))
        P.append(dict(circle=(s*50.5,0.8,1.9),ramp=[(40,40,40),(90,90,88),(170,170,166),(220,220,214)],shade=lin_v(0.95,0.2)))
        # AIM-120 under wing pylons
        for x in (28,38):
            P.append(dict(poly=[(s*x-0.6,0.5),(s*x+0.6,0.5),(s*x+0.6,3),(s*x-0.6,3)],ramp=G[:5],shade=flat(0.4)))
            P.append(dict(circle=(s*x,4.4,1.7),ramp=[(50,50,48),(110,110,106),(190,190,184),(230,230,224)],shade=lin_v(0.95,0.2)))
            P.append(dict(poly=[(s*x-3,4.2),(s*x+3,4.2),(s*x+3,4.7),(s*x-3,4.7)],ramp=G[2:7],shade=flat(0.6),edge=False))
    # fuselage rear body
    P.append(dict(poly=[(-12,-4),(-10,-10),(-5,-13),(5,-13),(10,-10),(12,-4),(11,4),(7,8),(-7,8),(-11,4)],ramp=G,shade=lambda lx,ly,u,v:0.98-0.75*v-0.12*abs(u-0.5)))
    # intake under belly (farther forward)
    P.append(dict(poly=[(-6,8),(6,8),(6.5,12),(-6.5,12)],ramp=G[:6],shade=lin_v(0.6,0.1)))
    for s in (-1,1):
        # horizontal stabilisers (slight anhedral)
        P.append(dict(poly=[(s*9,1),(s*29,5.5),(s*29.5,7.5),(s*9,4.5)],ramp=G[1:],shade=lambda lx,ly,u,v:0.9-0.6*v))
        # ventral fins
        P.append(dict(poly=[(s*5,7),(s*6.5,7),(s*10,14),(s*8.6,14.4)],ramp=G[:7],shade=flat(0.45)))
        # speed brake petals beside nozzle
        P.append(dict(poly=[(s*7,-4),(s*10.5,-4),(s*10.5,3),(s*7,3)],ramp=G[:7],shade=lin_v(0.75,0.25)))
    # vertical fin + rudder (thin, seen from behind)
    P.append(dict(poly=[(-2.2,-12),(2.2,-12),(1.4,-40),(-1.4,-40)],ramp=G,shade=lin_u(0.98,0.35)))
    P.append(dict(poly=[(-1.6,-40),(1.6,-40),(1.6,-42),(-1.6,-42)],ramp=G[:6],shade=flat(0.5)))
    # nozzle with petals + afterburner glow
    P.append(dict(circle=(0,0,7),ramp=NOZ,shade=lambda lx,ly,u,v:0.95-0.7*v))
    P.append(dict(circle=(0,0,5),ramp=[(70,20,8),(150,60,14),(230,130,30),(255,200,80),(255,244,190)],shade=lambda lx,ly,u,v:max(0,0.98-1.6*math.hypot(lx,ly)/5),edge=False))
    for k in range(12):
        a=2*math.pi*k/12; L.append(([(5.2*math.cos(a),5.2*math.sin(a)),(6.8*math.cos(a),6.8*math.sin(a))],NOZ[0]))
    for s in (-1,1):
        L.append(([(s*18,-8),(s*44,-1.5)],G[3]))          # flap hinge line
        L.append(([(s*14,-0.5),(s*30,0)],G[7]))           # trailing-edge highlight
    L.append(([(0,-14),(0,-38)],G[2])); L.append(([(-0.6,-26),(0.6,-26)],(220,220,220)))
    L.append(([(-3,-25.5),(1,-26.8)],GL[4]))
    dots=[(-47,-1,(220,40,40)),(47,-1,(60,220,90))]     # nav lights
    return P,L,dots
MG=[(26,30,36),(46,52,60),(66,74,84),(90,98,108),(116,124,132),(146,152,158),(176,182,186)]
MB=[(30,40,52),(48,62,78),(68,86,104),(92,112,130),(120,140,156),(150,168,180)]
def mig29_rear():
    P=[]; L=[]
    cam=lambda lx,ly: MB if noise(lx*3,ly*3,4)>0.55 else MG
    for s in (-1,1):
        P.append(dict(poly=[(s*6,-0.5),(s*22,1),(s*22.5,-1.5),(s*9,-6)],ramp=MG,rampf=cam,shade=lambda lx,ly,u,v:0.98-0.5*v))
    P.append(dict(poly=[(-3,-5),(3,-5),(2,-10),(-2,-10)],ramp=GL,shade=lin_v(0.95,0.3)))
    P.append(dict(poly=[(-10,-1),(-5,-5.5),(5,-5.5),(10,-1),(10,3.5),(-10,3.5)],ramp=MG,rampf=cam,shade=lin_v(0.98,0.35)))
    for s in (-1,1):
        P.append(dict(poly=[(s*6,2),(s*15,4.5),(s*15,6),(s*6,4.5)],ramp=MG,shade=lin_v(0.9,0.3)))
        P.append(dict(poly=[(s*3.4,-4),(s*6.4,-4),(s*9.6,-15),(s*7.6,-15.5)],ramp=MG,rampf=cam,shade=flat(0.85)))
        P.append(dict(circle=(s*5,1,3),ramp=NOZ,shade=lin_v(0.9,0.2)))
        P.append(dict(circle=(s*5,1,1.8),ramp=[(40,16,8),(110,50,16),(190,110,40)],shade=flat(0.5),edge=False))
    L.append(([(-20,-0.5),(-8,-4)],MG[6])); L.append(([(8,-4),(20,-0.5)],MG[6]))
    return P,L
SKY=[(18,40,96),(30,62,128),(48,90,160),(76,122,186),(112,156,206),(156,190,224),(200,218,236)]
SEA=[(14,30,52),(22,46,74),(34,64,96),(50,86,116),(70,108,136)]
CL=[(120,134,160),(160,172,194),(200,208,222),(230,234,242),(250,250,252)]
def sky(W,H,horizon=128,bank=-6):
    im=Image.new('RGB',(W,H)); P=im.load(); t=math.tan(math.radians(bank))
    for y in range(H):
        for x in range(W):
            hy=horizon+(x-W/2)*t
            if y<hy:
                k=max(0,min(1,(y)/(hy)))
                c=dith(0.05+0.9*k**1.3,x,y,SKY)
                cl=noise(x*0.6,y*2.2,9)*0.7+noise(x*1.5,y*4,4)*0.3
                if cl>0.62 and y>30: c=dith(0.25+0.9*(cl-0.62)*3*(0.5+0.5*k),x,y,CL)
            else:
                k=(y-hy)/max(1,H-hy)
                c=dith(0.85-0.6*k+0.3*noise(x,y*3,6),x,y,SEA)
                cl=noise(x*0.8+100,(y-hy)*1.5,8)
                if cl>0.6: c=dith(0.3+0.9*(cl-0.6)*2,x,y,CL)
                if abs(y-hy)<1.2: c=(170,190,210)
            P[x,y]=c
    return im
def hud_text(im,x,y,s,col=HUDG): text(im,x,y,s,col,shadow=(4,30,10))
def top():
    im=sky(SW,SH,104,-8); d=ImageDraw.Draw(im)
    mx,my=94,56
    mp,ml=mig29_rear(); at(im,mp,ml,mx,my,1.15,ang=6)
    P,L,dots=f16_rear(); at(im,P,L,130,148,1.95,ang=-8,dots=dots)
    # heading tape
    for i,hd in enumerate(range(340,400,10)):
        x=48+i*32; h=hd%360
        d.line((x,12,x,15),fill=HUDG); hud_text(im,x-8,3,'%03d'%h)
        d.line((x+16,13,x+16,15),fill=HUDG)
    d.polygon([(128,17),(125,21),(131,21)],fill=HUDG)
    # horizon ladder stubs (sides only)
    for s in (-1,1):
        x0=128+s*96; d.line((x0,106,x0+s*-22,106-s*3),fill=HUDG)
    hud_text(im,236,96,'0')
    # flight path marker
    cx,cy=180,82
    d.ellipse((cx-3,cy-3,cx+3,cy+3),outline=HUDG); d.line((cx-9,cy,cx-4,cy),fill=HUDG); d.line((cx+4,cy,cx+9,cy),fill=HUDG); d.line((cx,cy-4,cx,cy-8),fill=HUDG)
    # seeker circle + lock box on the MiG
    d.ellipse((mx-26,my-26,mx+26,my+26),outline=HUDGD)
    bx,by=mx,my-3; R_=(255,80,60)
    for (x0,y0,sx,sy) in ((bx-17,by-14,1,1),(bx+17,by-14,-1,1),(bx-17,by+13,1,-1),(bx+17,by+13,-1,-1)):
        d.line((x0,y0,x0+sx*6,y0),fill=R_); d.line((x0,y0,x0,y0+sy*6),fill=R_)
    lx=bx-66
    d.rectangle((lx,by-14,lx+tw('LOCK')+3,by-5),fill=(140,10,10)); text(im,lx+2,by-13,'LOCK',WHITE,shadow=None)
    hud_text(im,lx,by-2,'MIG-29',(255,210,200)); hud_text(im,lx,by+7,'2.4NM',(255,210,200))
    d.rectangle((4,24,46,34),fill=(10,40,16)); d.rectangle((4,24,46,34),outline=HUDG); ctext(im,26,26,'SHOOT',HUDG,shadow=None)
    d.rectangle((1,179,68,190),fill=(6,24,12)); d.rectangle((1,179,68,190),outline=HUDGD)
    hud_text(im,4,182,'AIM-120',HUDG); hud_text(im,52,182,'X4',WHITE)
    tag(im,SW-41,181)
    return im
# -------------- bottom: cockpit instruments
RG=[(4,18,8),(8,34,14),(14,60,24),(30,110,46),(80,200,100),(170,255,180)]
def radar(im,cx,cy,r):
    P=im.load()
    sweep=math.radians(-58)
    for y in range(cy-r,cy+r+1):
        for x in range(cx-r,cx+r+1):
            dx,dy=x-cx,y-cy; dd=math.hypot(dx,dy)
            if dd>r: continue
            a=math.atan2(dy,dx); diff=(sweep-a)%(2*math.pi)
            t=0.12+0.08*noise(x,y,3)
            if diff<1.1: t+=0.42*(1-diff/1.1)
            c=dith(t,x,y,RG)
            if dd>r-1.2: c=(40,140,60)
            P[x,y]=c
    d=ImageDraw.Draw(im)
    for k in (1,2,3):
        rr=r*k/4
        for a in range(0,360,4):
            X=int(cx+rr*math.cos(math.radians(a))); Y=int(cy+rr*math.sin(math.radians(a)))
            if (a//4)%2==0: P[X,Y]=(50,150,70)
    for a in range(0,360,30):
        ca,sa=math.cos(math.radians(a)),math.sin(math.radians(a))
        d.line((cx+ca*(r-4),cy+sa*(r-4),cx+ca*r,cy+sa*r),fill=(90,210,110))
    d.line((cx,cy,cx+r*math.cos(sweep),cy+r*math.sin(sweep)),fill=(170,255,180))
    # own ship
    d.polygon([(cx,cy-4),(cx+3,cy+3),(cx,cy+1),(cx-3,cy+3)],fill=WHITE)
    # contacts
    for (dx,dy,kind,lab) in ((18,-30,'H','MIG'),(-26,-38,'H','MIG'),(34,-6,'U','?'),(-14,22,'F','F-16')):
        x,y=cx+dx,cy+dy
        col={'H':(255,80,60),'U':(255,220,80),'F':BLUE_T if False else (110,190,255)}[kind]
        d.polygon([(x,y-3),(x+3,y),(x,y+3),(x-3,y)],fill=SHADOW); d.polygon([(x,y-2),(x+2,y),(x,y+2),(x-2,y)],fill=col)
        text(im,x+5,y-3,lab,col)
    # locked target bracket
    x,y=cx+18,cy-30; d.rectangle((x-5,y-5,x+5,y+5),outline=(255,80,60))
def gmeter(im,x0,y0,x1,y1,g=4.2,gmax=9.0,gmin=-3.0):
    slot(im,x0,y0,x1,y1,fill=(20,22,14))
    d=ImageDraw.Draw(im); h=y1-y0-22; bx=x0+5
    def gy(v): return int(y1-6-(v-gmin)/(gmax-gmin)*h)
    for v in range(-3,10):
        d.line((bx+8,gy(v),bx+10 if v%3 else bx+12,gy(v)),fill=(160,166,110))
    d.rectangle((bx,gy(gmax),bx+6,gy(gmin)),fill=(30,32,20))
    for yy in range(gy(g),gy(0)):
        v=gmin+(y1-6-yy)/h*(gmax-gmin)
        c=(120,236,96) if v<6 else (240,200,60) if v<8 else (240,70,50)
        d.line((bx+1,yy,bx+5,yy),fill=c)
    d.line((bx-1,gy(gmax),bx+7,gy(gmax)),fill=(240,70,50))
    tiny(im,bx+13,gy(9)-2,'9'); tiny(im,bx+13,gy(6)-2,'6'); tiny(im,bx+13,gy(3)-2,'3'); tiny(im,bx+13,gy(0)-2,'0')
    text(im,x0+3,y0+3,'G',YG)
    text(im,x0+3,y1-14+0,'',TX)
def bottom():
    im=Image.new('RGB',(SW,SH),(18,20,14)); d=ImageDraw.Draw(im)
    bevel(im,0,0,255,15); text(im,4,4,'F-16C',GOLD); text(im,40,4,'BLOCK 50',DIM); ctext(im,150,4,'KILLS 3',WHITE); rtext(im,252,4,'WAVE 2/5',TX)
    bevel(im,0,17,135,160)
    text(im,4,20,'RADAR',YG); rtext(im,132,20,'40NM',TX)
    radar(im,68,92,62)
    text(im,4,151,'RWS',DIM); rtext(im,132,151,'TWS',DIM)
    bevel(im,137,17,255,160)
    gmeter(im,140,20,170,157)
    rows=[('LOAD','4.2G'),('SPD','450KT'),('ALT','12,500FT'),('HDG','012')]
    for i,(k,v) in enumerate(rows):
        y=22+i*12; text(im,173,y,k,DIM); rtext(im,251,y,v,WHITE if i else (120,236,96))
    y=72; text(im,173,y,'FUEL',YG); rtext(im,251,y,'5,200LB',TX)
    d.rectangle((173,82,251,86),fill=SHADOW); d.rectangle((174,83,174+int(76*5200/7000),85),fill=(240,200,60))
    for i,(k,v) in enumerate([('AIM-120','4'),('AIM-9','2'),('GUN','511'),('FLARE','24'),('CHAFF','30')]):
        y=92+i*13; text(im,173,y,k,TX); rtext(im,251,y,v,GOLD if k in ('FLARE','CHAFF') else WHITE)
    for i,(a,b) in enumerate([('FLARE',None),('LOCK',None),('WEAPON','SWAP'),('CHAFF',None)]):
        x=1+i*64; tbutton(im,x,163,x+61,190,a,sel=(i==1),sub=b)
    tag(im,47,150)
    return im
def tbutton(im,x0,y0,x1,y1,s,col=TX,sel=False,sub=None):
    bevel(im,x0,y0,x1,y1,(92,102,56) if sel else PF,raised=not sel)
    cy=(y0+y1)//2-(7 if sub else 3)
    ctext(im,(x0+x1)//2+1,cy,s,GOLD if sel else col)
    if sub: ctext(im,(x0+x1)//2+1,cy+9,sub,DIM)
if __name__=='__main__':
    t=top(); b=bottom(); up(t,3).save('v_dt1.png'); up(b,3).save('v_db1.png')
