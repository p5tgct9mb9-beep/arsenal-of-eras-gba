from hd import *
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,RED,GREEN,BLUE,SHADOW
S=[(10,10,8),(30,32,28),(48,52,44),(66,70,58),(86,88,70),(108,106,82),(130,124,94)]
G=[(30,26,18),(52,44,30),(74,62,42),(96,80,54),(118,98,66),(138,114,78)]
CON=[(26,26,22),(42,42,36),(60,60,52),(80,80,70),(102,100,88),(124,122,108),(148,144,128)]
MET=[(12,12,12),(24,24,24),(38,38,38),(54,54,54),(72,72,70),(94,94,90)]
CLO=[(22,26,14),(34,40,22),(48,56,30),(64,72,40),(82,90,52),(100,108,66)]
GLV=[(20,18,14),(36,32,24),(54,48,36),(74,66,50),(96,86,66)]
SKN=[(70,44,30),(112,74,52),(150,104,76),(184,136,100)]
OPF=[(30,18,14),(60,30,24),(92,46,36),(124,64,48)]
def scene():
    im=bands(240,160,[(0,160,S[3],S[3],'solid'),
      (0,8,S[1],S[1],'solid'),(8,12,S[1],S[2],'checker'),(12,24,S[2],S[2],'solid'),(24,28,S[2],S[3],'checker'),
      (28,40,S[3],S[3],'solid'),(40,44,S[3],S[4],'checker'),(44,58,S[4],S[4],'solid'),
      (58,62,S[4],S[5],'checker'),(62,70,S[5],S[5],'solid'),
      (70,76,G[1],G[1],'solid'),(76,80,G[1],G[2],'checker'),(80,90,G[2],G[2],'solid'),(90,94,G[2],G[3],'checker'),
      (94,108,G[3],G[3],'solid'),(108,112,G[3],G[4],'checker'),(112,130,G[4],G[4],'solid'),(130,134,G[4],G[5],'checker'),(134,160,G[5],G[5],'solid')])
    I=Xf(0,0,1,0)
    # distant ridge
    render(im,[dict(poly=[(0,70),(0,62),(30,58),(60,61),(96,55),(140,60),(180,56),(214,60),(240,58),(240,70)],ramp=S[1:5],shade=lin_v(0.8,0.0))],I,outline=S[1])
    # ruined wall, broken top edge, block courses
    wall=[(14,88),(14,58),(30,58),(34,54),(52,54),(56,60),(70,60),(74,52),(102,52),(104,57),(128,57),(132,50),(160,50),(164,56),(176,56),(180,62),(196,62),(200,55),(226,55),(226,88)]
    ln=[]
    for y in (64,72,80): ln.append(([(15,y),(225,y)],CON[1]))
    for row,y0 in enumerate((56,64,72,80)):
        for x in range(20+(row%2)*10,226,20): ln.append(([(x,y0),(x,y0+7)],CON[1]))
    for y in (65,73,81): ln.append(([(16,y),(224,y)],CON[5]))
    render(im,[dict(poly=wall,ramp=CON,shade=lambda lx,ly,u,v:0.95-0.6*v-0.15*u),
               dict(poly=[(14,88),(226,88),(226,91),(14,91)],ramp=CON[:3],shade=flat(0.4))],I,lines=ln)
    # rubble
    # damage chips on wall
    d=ImageDraw.Draw(im)
    for (x,y) in [(60,67),(61,67),(150,60),(151,61),(190,75),(44,79),(45,79)]: d.point((x,y),fill=CON[0])
    return im
def soldier(im,x,y):
    I=Xf(x,y,1,0)
    parts=[dict(poly=[(-3,-6),(-1,-6),(-1,0),(-4,0)],ramp=OPF[:3],shade=lin_u(0.3,0.8)),
           dict(poly=[(1,-6),(3,-6),(4,0),(1,0)],ramp=OPF[:3],shade=lin_u(0.8,0.3)),
           dict(poly=[(-4,-13),(4,-13),(4,-6),(-4,-6)],ramp=OPF,shade=lambda lx,ly,u,v:0.9-0.5*u-0.3*v),
           dict(poly=[(-2,-16),(2,-16),(2,-13),(-2,-13)],ramp=SKN,shade=lin_u(0.9,0.3)),
           dict(poly=[(-3,-19),(3,-19),(4,-16),(-4,-16)],ramp=CLO,shade=lin_v(0.9,0.2)),
           dict(poly=[(-6,-11),(7,-12),(7,-10),(-6,-9)],ramp=MET,shade=lin_v(0.9,0.2))]
    return render(im,parts,I)
def rifle(im):
    I=Xf(0,0,1,0)
    M=[(14,14,14),(28,28,28),(44,44,44),(62,62,60),(84,84,80),(110,110,104),(140,140,132)]
    T=[(44,36,24),(74,60,40),(104,86,58),(134,112,78),(162,138,100)]
    C=[(26,30,16),(40,46,26),(56,64,36),(74,82,48),(94,102,62),(116,124,80)]
    parts=[
      dict(poly=[(86,160),(118,114),(138,118),(124,160)],ramp=C,shade=lambda lx,ly,u,v:0.25+0.65*u-0.2*v),          # support sleeve
      dict(poly=[(190,160),(202,130),(236,136),(232,160)],ramp=C,shade=lambda lx,ly,u,v:0.85-0.6*u),                # firing sleeve
      dict(poly=[(128,160),(138,120),(152,119),(152,160)],ramp=M,shade=lambda lx,ly,u,v:0.25+0.25*u),             # receiver L face
      dict(poly=[(152,160),(152,119),(170,118),(196,160)],ramp=M,shade=lambda lx,ly,u,v:0.95-0.55*u-0.1*v),         # receiver R face
      dict(poly=[(148,121),(150,104),(156,104),(158,121)],ramp=M,shade=lin_v(0.5,0.9)),                             # carry handle / rail
      dict(poly=[(150,111),(154,111),(154,117),(150,117)],ramp=[INK,INK],shade=flat(0),edge=False),                  # rear aperture
      dict(poly=[(139,120),(145,94),(152,94),(152,120)],ramp=M,shade=flat(0.3)),                                    # handguard L
      dict(poly=[(152,120),(152,94),(160,94),(168,120)],ramp=M,shade=lambda lx,ly,u,v:0.85-0.5*u+0.15*v),          # handguard R
      dict(poly=[(146,94),(148,86),(158,86),(160,94)],ramp=M,shade=lin_v(0.95,0.35)),                               # front sight base
      dict(poly=[(151,86),(152,80),(154,80),(155,86)],ramp=M,shade=flat(0.55)),                                     # post
      dict(poly=[(150,80),(150,76),(156,76),(156,80)],ramp=M[:5],shade=flat(0.6)),                                  # muzzle device
      dict(poly=[(122,112),(140,100),(146,104),(146,118),(132,124),(120,122)],ramp=T,shade=lambda lx,ly,u,v:0.95-0.75*v), # support palm
      dict(poly=[(140,100),(156,98),(158,103),(146,106)],ramp=T,shade=lin_v(1.0,0.4)),                              # fingers over top
      dict(poly=[(178,146),(190,132),(210,136),(204,152),(186,156)],ramp=T,shade=lambda lx,ly,u,v:0.95-0.7*v),        # grip hand
    ]
    ln=[([(156,98),(160,112)],M[4]),([(158,100),(163,116)],M[4]),([(141,110),(150,110)],M[1]),
        ([(126,116),(140,110)],T[4]),([(124,121),(138,117)],T[1]),([(146,100),(152,99)],T[1]),([(188,140),(204,140)],T[4]),
        ([(160,124),(186,156)],M[4]),([(152,124),(152,158)],M[1])]
    render(im,parts,I,lines=ln)
CX,CY=120,81
def label(im,x,y,s,col):
    w=tw(s)+5; d=ImageDraw.Draw(im); d.rectangle((x,y,x+w,y+10),fill=SHADOW); d.rectangle((x+1,y+1,x+w-1,y+9),fill=col)
    text(im,x+3,y+2,s,WHITE,shadow=None)
base=scene(); soldier(base,120,91); rifle(base)
# ---------- ARCADE
A=base.copy(); d=ImageDraw.Draw(A)
for k in (2,3,4):  # crosshair
    for sx,sy in ((k,0),(-k,0),(0,k),(0,-k)): d.point((CX+sx,CY+sy),fill=(82,222,82))
for k in (4,5,6):  # hitmarker
    for sx,sy in ((1,1),(-1,1),(1,-1),(-1,-1)): d.point((CX+sx*k,CY+sy*k),fill=WHITE)
text(A,128,66,'-34',(255,90,74)); text(A,96,50,'HIT +25',GOLD)
d.rectangle((113,64,128,66),fill=SHADOW); d.rectangle((114,65,123,65),fill=RED)
mx,my,ms=204,4,32
d.rectangle((mx,my,mx+ms,my+ms),fill=SHADOW); d.rectangle((mx+1,my+1,mx+ms-1,my+ms-1),fill=(239,198,90))
d.rectangle((mx+2,my+2,mx+ms-2,my+ms-2),fill=(148,99,41)); d.rectangle((mx+3,my+3,mx+ms-2,my+ms-2),fill=(24,33,90))
for (px,py,c) in [(16,22,(90,214,247)),(18,23,(90,214,247)),(14,25,(90,214,247)),(16,10,RED),(22,8,RED),(25,14,RED),(8,6,RED),(27,26,GOLD)]:
    d.rectangle((mx+px,my+py,mx+px+1,my+py+1),fill=c)
d.line((mx+16,my+22,mx+16,my+17),fill=WHITE)
d.rectangle((0,137,239,159),fill=(16,16,24)); d.line((0,136,239,136),fill=(66,74,90))
text(A,4,140,'HP 53',GREEN); d.rectangle((4,150,56,153),fill=(41,41,66)); d.rectangle((4,150,31,153),fill=GREEN)
text(A,74,140,'M4 CARBINE',WHITE); text(A,74,150,'27/90',GRAY); text(A,130,150,'ENEMY 6',GOLD); text(A,196,140,'25 FPS',GRAY)
label(A,3,3,'ARCADE',(57,148,74))
# ---------- REALISTIC
R=base.copy(); d=ImageDraw.Draw(R)
P0=(152,79); P1=(132,24); P2=(CX,CY)
pts=[]
for i in range(41):
    t=i/40; pts.append((round((1-t)**2*P0[0]+2*(1-t)*t*P1[0]+t*t*P2[0]),round((1-t)**2*P0[1]+2*(1-t)*t*P1[1]+t*t*P2[1])))
for i,(x,y) in enumerate(pts):
    if i%2==0 and i>1: d.point((x,y),fill=(255,239,165)); d.point((x+1,y),fill=(200,140,50))
hx,hy=CX,64   # holdover aim point above head
d.rectangle((hx-1,hy-1,hx+1,hy+1),fill=SHADOW); d.point((hx,hy),fill=(255,90,74))
for yy in (68,72):
    d.line((hx-2,yy,hx-1,yy),fill=(220,220,210)); d.line((hx+1,yy,hx+2,yy),fill=(220,220,210))
d.rectangle((150,22,232,44),fill=SHADOW); d.rectangle((151,23,231,43),fill=(120,100,60)); d.rectangle((152,24,230,42),fill=(16,16,12))
text(R,155,26,'300M:',GOLD); text(R,186,26,'AIM HIGH',WHITE); text(R,155,35,'DROP -1.1M',GRAY)
d.line((150,40,hx+3,hy-1),fill=(255,239,165))
text(R,172,150,'MAG: HEAVY',(206,214,200))
for i in range(3):
    x=174+i*7; d.rectangle((x,140,x+4,147),fill=SHADOW); d.rectangle((x,140,x+3,146),fill=(150,150,140) if i<2 else (60,60,52))
text(R,196,140,'+2',GRAY)
V=R.load()
for y in range(160):
    for x in range(240):
        e=min(x,239-x,y*1.5,(159-y)*1.5)
        a=max(0.0,(34-e)/34)**1.4*0.7
        q=int(a*6*16+BAYER[y&3][x&3])//16/6
        a=q
        if a>0:
            r,g,b=V[x,y]; V[x,y]=(int(r*(1-a)+170*a),int(g*(1-a)+16*a),int(b*(1-a)+16*a))
label(R,3,3,'REALISTIC',(181,41,41))
mockup_tag(R,x=196,y=3)
out=Image.new('RGB',(484,160),(8,8,8)); out.paste(A,(0,0)); out.paste(R,(244,0))
up(out).save('/workspace/realism_mockup_infantry.png'); up(out).save('v_inf2.png')
