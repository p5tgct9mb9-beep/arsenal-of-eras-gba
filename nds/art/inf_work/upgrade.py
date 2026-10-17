from ui import *
from loadout import portrait
from gbalib import native,SHOTS,RED,GREEN,BLUE
import random
random.seed(3)
GR=[(46,62,30),(60,80,38),(76,98,46),(94,116,56),(112,132,66)]
DIRT=[(86,66,42),(108,84,54),(130,102,66),(150,120,80)]
BRK=[(60,28,20),(92,44,30),(124,62,40),(150,82,54),(176,108,72)]
SB=[(58,52,34),(86,78,52),(114,104,70),(140,130,92),(166,156,116)]
def ground(W=240,H=160):
    im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            n=noise(x,y,9)*0.6+noise(x+300,y,3)*0.4
            road = abs(x-176-6*math.sin(y/22))<11
            if road:
                t=0.35+0.5*noise(x,y,4); P[x,y]=dith(t,x,y,DIRT)
                if abs(x-176-6*math.sin(y/22))>9.5: P[x,y]=dith(0.3,x,y,GR)
            else: P[x,y]=dith(0.25+0.6*n,x,y,GR)
    d=ImageDraw.Draw(im)
    for _ in range(90):
        x,y=random.randrange(W),random.randrange(H)
        if abs(x-176-6*math.sin(y/22))>12: d.point((x,y),fill=GR[4]); d.point((x,y+1),fill=GR[1])
    for x,y in [(30,40),(92,130),(130,30),(60,100),(210,140)]:   # small flowers/rocks
        d.point((x,y),fill=(214,190,120)); d.point((x+1,y),fill=(150,130,80))
    # wheel ruts
    for y in range(H):
        for o in (-5,5):
            X=int(176+6*math.sin(y/22)+o)
            if y%3: P[X,y]=DIRT[0]
    return im
def brick_wall(im,x0,y0,w,h,top=4):
    I=Xf(0,0,1)
    # top face (3/4 view) + front face
    render2(im,[dict(poly=[(x0,y0),(x0+w,y0),(x0+w,y0+top),(x0,y0+top)],ramp=BRK,shade=lin_v(1.0,0.7)),
                dict(poly=[(x0,y0+top),(x0+w,y0+top),(x0+w,y0+h),(x0,y0+h)],ramp=BRK,shade=lin_v(0.7,0.25))],I,
            lines=[([(x0,yy),(x0+w,yy)],BRK[0]) for yy in range(y0+top+3,y0+h,3)]+
                  [([(xx,yy),(xx,yy+2)],BRK[0]) for yy in range(y0+top,y0+h-1,3) for xx in range(x0+3+((yy-y0)//3%2)*3,x0+w-1,6)])
    d=ImageDraw.Draw(im); 
    for xx in range(x0+1,x0+w,2): d.point((xx,y0+h+1),fill=GR[0])
def sandbags(im,x0,y0,n):
    I=Xf(0,0,1); P=[]
    for r in range(2):
        for i in range(n-r):
            x=x0+i*9+r*4; y=y0-r*4
            P.append(dict(poly=ell(x+4.5,y+3,5,3.2,12),ramp=SB,shade=lambda lx,ly,u,v:0.95-0.6*v-0.2*u))
    m,_=render2(im,P,I); 
def crate(im,x,y):
    I=Xf(0,0,1); W_=[(52,34,18),(84,56,30),(116,80,44),(146,104,60),(172,128,80)]
    render2(im,[dict(poly=[(x,y),(x+14,y),(x+14,y+4),(x,y+4)],ramp=W_,shade=flat(0.9)),
                dict(poly=[(x,y+4),(x+14,y+4),(x+14,y+16),(x,y+16)],ramp=W_,shade=lin_v(0.6,0.3))],I,
            lines=[([(x+1,y+5),(x+13,y+15)],W_[1]),([(x+13,y+5),(x+1,y+15)],W_[1]),([(x,y+2),(x+14,y+2)],W_[1])])
BLUE_T=(90,170,255); RED_T=(240,70,60)
US=dict(head='M1',unif='OLIVE DRAB',gear='WEBBING',rifle='GARAND')
AX=dict(head='STAHL',unif='FIELD GREY',gear='WEBBING',rifle='BOLT',helmcol=(84,90,84))
def plate(im,x,y,s,col):
    w=tw(s)+3; d=ImageDraw.Draw(im); d.rectangle((x-w//2-1,y-1,x+w//2+1,y+8),fill=(8,8,8)); d.rectangle((x-w//2,y,x+w//2,y+7),fill=mul(col,0.45))
    text(im,x-w//2+2,y+1,s,(240,240,230),shadow=None)
def arrow(im,x,y,col):
    d=ImageDraw.Draw(im); d.polygon([(x-3,y),(x+3,y),(x,y+3)],fill=(8,8,8)); d.polygon([(x-2,y),(x+2,y),(x,y+2)],fill=col)
def ring(im,x,y,col):
    P=im.load()
    for a in range(0,360,12):
        X=int(x+9*math.cos(math.radians(a))); Y=int(y+3.2*math.sin(math.radians(a)))
        if 0<=X<240 and 0<=Y<160: P[X,Y]=col
def topdown_after():
    im=ground()
    brick_wall(im,20,22,76,14); brick_wall(im,20,22,10,56); brick_wall(im,108,92,46,14)
    sandbags(im,70,128,5); crate(im,200,128); crate(im,120,40)
    units=[ # x,y(feet),team,view,flip,name,skin
        (64,74,'B','N',False,'HALE',5),(40,104,'B','S',False,None,2),(88,96,'B','S',True,None,4),(50,140,'B','N',False,None,6),
        (150,60,'R','S',False,None,1),(206,66,'R','S',True,None,3),(176,128,'R','W',False,None,2),(222,108,'R','S',False,None,1),(132,140,'B','E',False,None,3)]
    for (x,y,t,v,fl,nm,sk) in sorted(units,key=lambda u:u[1]):
        base=US if t=='B' else AX
        s=spec(skin=sk,hair='BLACK' if sk>3 else 'BROWN',face='STUBBLE' if nm else 'CLEAN',**base)
        view={'N':'B','S':'F','E':'S','W':'S'}[v]
        flip = (v=='W') or fl and v=='S' and False
        spr=sprite(s,0.95,view,warp=True,flip=flip)
        col=BLUE_T if t=='B' else RED_T
        if nm: ring(im,x,y,(120,220,255))
        place(im,spr,x,y,shadow_col=(36,48,22),sw=14)
        top=y-31
        if nm: plate(im,x,top-10,nm,col); arrow(im,x,top-1,col)
        else: arrow(im,x,top,col)
    # HUD like the game
    d=ImageDraw.Draw(im); d.rectangle((0,0,239,9),fill=(12,14,10))
    text(im,2,1,'USA 2',BLUE_T); text(im,104,1,'4:53',WHITE); text(im,204,1,'3 AXIS',RED_T)
    d.rectangle((0,150,239,159),fill=(12,14,10))
    for i in range(6):
        x=3+i*8; d.polygon([(x,152),(x+2,151),(x+3,152),(x+4,151),(x+6,152),(x+6,154),(x+3,157),(x,154)],fill=(230,50,50))
    text(im,56,151,'M1 GARAND',WHITE); text(im,124,151,'8/40',GRAY); text(im,160,151,'NADE 2',GOLD)
    mockup_tag(im,x=196,y=12)
    return im
# ---------- FPS
SKY=[(56,70,86),(76,94,110),(100,120,132),(128,146,150),(160,172,166)]
def fps_after():
    W,H=240,160; im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            if y<64: P[x,y]=dith(0.15+0.8*y/64,x,y,SKY)
            elif y<74: P[x,y]=dith(0.3+0.4*noise(x,y,6),x,y,[(52,62,46),(66,76,54),(80,90,62)])
            else: P[x,y]=dith(0.2+0.6*(y-74)/86+0.15*noise(x,y,5),x,y,[(70,64,44),(92,84,58),(112,102,70),(130,118,82),(146,132,92)])
    I=Xf(0,0,1)
    render2(im,[dict(poly=[(0,66),(30,58),(70,62),(110,54),(150,60),(200,52),(240,58),(240,70),(0,70)],ramp=[(34,44,34),(46,58,42),(60,72,52),(74,86,62)],shade=lin_v(0.8,0.1))],I,outline=(34,44,34))
    # ruined wall left, sandbags center
    brick_wall(im,0,58,70,34,top=4)
    d=ImageDraw.Draw(im)
    im2=im.crop((0,0,W,H))
    for (x,y) in [(40,58)]:
        pass
    # broken top of wall: paint sky back above jagged line
    for x in range(0,71):
        jag=int(58+ (6 if 38<x<56 else 0) + 2*math.sin(x/3.0))
        for y in range(54,jag): P[x,y]=dith(0.15+0.8*y/64,x,y,SKY) if y<64 else P[x,y]
    sandbags(im,150,86,6)
    # enemies: far (behind sandbags), mid, near-left
    e1=spec(skin=2,**AX); e2=spec(skin=4,hair='BLACK',**AX); e3=spec(skin=1,**AX)
    place(im,sprite(e2,0.5,'S',flip=True),112,80,shadow_col=(60,56,38),sw=10)
    place(im,sprite(e3,0.62,'F'),176,92,None)   # behind bags (drawn before bags redraw)
    sandbags(im,154,90,5)
    place(im,sprite(e1,1.0,'F'),92,118,shadow_col=(60,56,38),sw=18)
    # hit + crosshair
    CX,CY=120,82
    for k in (3,4,5):
        for sx,sy in ((k,0),(-k,0),(0,k),(0,-k)): d.point((CX+sx,CY+sy),fill=(120,240,120))
    rifle_fp(im)
    # HUD
    d.rectangle((0,136,239,159),fill=(16,16,24)); d.line((0,135,239,135),fill=(66,74,90))
    s=spec(skin=5,hair='BLACK',face='STUBBLE',head='M1',unif='OLIVE DRAB')
    portrait(im,1,137,22,158,s,sc=1.9)
    text(im,26,139,'HP 100',GREEN); d.rectangle((26,149,74,152),fill=(41,41,66)); d.rectangle((26,149,74,152),fill=GREEN)
    text(im,82,139,'M1 GARAND',WHITE); text(im,82,149,'8/40',GRAY); text(im,128,149,'ENEMY 3',GOLD); text(im,196,139,'28 FPS',GRAY)
    mockup_tag(im,x=196,y=3)
    return im
def rifle_fp(im):
    I=Xf(0,0,1)
    M=PARK; T=WD
    SL=R((90,96,58)); SK=SKINS[2]
    parts=[
      dict(poly=[(78,136),(112,108),(134,114),(118,136)],ramp=SL,shade=lambda lx,ly,u,v:0.25+0.65*u-0.2*v),            # support sleeve
      dict(poly=[(188,136),(198,112),(240,116),(240,136)],ramp=SL,shade=lambda lx,ly,u,v:0.85-0.6*u),                   # firing sleeve
      dict(poly=[(132,136),(142,108),(160,107),(160,136)],ramp=T,shade=lambda lx,ly,u,v:0.35+0.25*u),                   # stock L
      dict(poly=[(160,136),(160,107),(176,106),(200,136)],ramp=T,shade=lambda lx,ly,u,v:0.95-0.55*u-0.1*v),             # stock R
      dict(poly=[(146,110),(149,96),(162,96),(166,110)],ramp=M,shade=lambda lx,ly,u,v:0.85-0.5*u),                      # receiver
      dict(poly=[(151,98),(152,90),(158,90),(159,98)],ramp=M,shade=lin_v(0.9,0.4)),                                     # rear sight
      dict(poly=[(153,94),(157,94),(157,96),(153,96)],ramp=[INK,INK],shade=flat(0),edge=False),                       # aperture
      dict(poly=[(148,97),(151,70),(161,70),(163,97)],ramp=T,shade=lambda lx,ly,u,v:0.9-0.6*u),                         # handguard wood
      dict(poly=[(152,72),(153,62),(158,62),(159,72)],ramp=M,shade=lin_v(0.95,0.4)),                                    # barrel + gas cyl
      dict(poly=[(154,62),(154.5,56),(156.5,56),(157,62)],ramp=M,shade=flat(0.6)),                                      # front sight
      dict(poly=[(116,104),(136,90),(150,92),(152,104),(138,112),(118,112)],ramp=SK,shade=lambda lx,ly,u,v:0.95-0.75*v), # support hand
      dict(poly=[(136,90),(152,86),(156,90),(150,94)],ramp=SK,shade=lin_v(1.0,0.4)),                                    # fingers
      dict(poly=[(176,126),(188,112),(208,114),(206,130),(186,134)],ramp=SK,shade=lambda lx,ly,u,v:0.95-0.7*v),         # trigger hand
    ]
    ln=[([(150,84),(158,84)],T[1]),([(150,78),(159,78)],T[5]),([(138,94),(148,92)],SK[1]),([(140,98),(150,96)],SK[1]),([(142,102),(151,101)],SK[1]),
        ([(120,108),(136,104)],SK[4]),([(190,120),(204,120)],SK[1]),([(188,124),(204,125)],SK[1]),([(92,130),(110,114)],SL[1]),([(206,124),(232,124)],SL[1]),
        ([(160,110),(160,134)],T[1]),([(166,112),(186,134)],T[5]),([(152,62),(158,62)],M[5])]
    render2(im,parts,I,lines=ln)
# ---------- era strip
ERAS=[('WW1','INFANTRY',spec(skin=2,hair='BROWN',head='BRODIE',unif='KHAKI',gear='WEBBING',rifle='BOLT',helmcol=(98,96,66))),
      ('WW2','US GI',spec(skin=4,hair='BLACK',head='M1',unif='OLIVE DRAB',gear='WEBBING',rifle='GARAND')),
      ('WW2','GER (GENERIC)',spec(skin=1,hair='BLOND',head='STAHL',unif='FIELD GREY',gear='WEBBING',rifle='BOLT',helmcol=(84,90,84))),
      ('COLD WAR','RIFLEMAN',spec(skin=6,hair='BLACK',head='M1',unif='OLIVE DRAB',camo='WOODLAND',gear='WEBBING',rifle='M16',helmcol=(80,90,56))),
      ('MODERN','US PLATE',spec(skin=3,hair='BROWN',head='HELMET',unif='DESERT TAN',camo='DIGITAL',gear='PLATE',rifle='M4',nvg=True,gloves=R((120,104,74)),helmcol=(150,132,96))),
      ('MODERN','SOF',spec(skin=5,hair='BLACK',face='STUBBLE',head='HELMET',unif='OLIVE DRAB',gear='PLATE',rifle='SOF',nvg=True,ears=True,gloves=BLK,helmcol=(70,72,60)))]
def era_strip(W):
    im=Image.new('RGB',(W,86),(16,16,12)); bevel(im,0,0,W-1,85)
    cw=(W-6)//6
    for i,(a,b,s) in enumerate(ERAS):
        x=3+i*cw
        from creator import frame_fig
        frame_fig(im,(x,3,x+cw-2,66),s,1.0,feet_pad=3)
        text(im,x+cw//2-tw(a)//2,68,a,GOLD); text(im,x+cw//2-tw(b)//2,77,b,TX)
    return im
def sheet():
    W=492; H=16+2*(12+160+4)+12+86+4
    S=Image.new('RGB',(W,H),(16,16,12))
    t='ARSENAL OF ERAS - INFANTRY UPGRADE: BEFORE VS AFTER'; text(S,(W-tw(t))//2,4,t,GOLD); mockup_tag(S)
    cap=(206,200,170)
    y=16
    text(S,4,y+2,'BEFORE: 16PX BLOB',cap); text(S,248,y+2,'AFTER: 3/4 TOP-DOWN, 28PX, TEAM ARROWS',cap)
    S.paste(native(SHOTS+'/v13/v13_era_ww1_inf_play2.png'),(4,y+12)); S.paste(topdown_after(),(248,y+12))
    lab(S,8,y+12+56,'BEFORE')
    y+=12+160+4
    text(S,4,y+2,'BEFORE: FLAT FPS',cap); text(S,248,y+2,'AFTER: FULL-BODY ENEMIES, NEW RIFLE',cap)
    S.paste(native(SHOTS+'/v13/v13_fps_2_play.png'),(4,y+12)); S.paste(fps_after(),(248,y+12))
    lab(S,8,y+12+56,'BEFORE')
    y+=12+160+4
    text(S,4,y+2,'ERA SOLDIERS - FRONT SPRITES, 48PX (NO INSIGNIA, GENERIC KIT)',cap)
    S.paste(era_strip(W-8),(4,y+12))
    return S
def lab(S,x,y,s):
    w=tw(s)+5; d=ImageDraw.Draw(S); d.rectangle((x,y,x+w,y+10),fill=SHADOW); d.rectangle((x+1,y+1,x+w-1,y+9),fill=(90,90,96))
    text(S,x+3,y+2,s,WHITE,shadow=None)
if __name__=='__main__':
    S=sheet(); up(S,3).save('/workspace/infantry_upgrade_mockup.png')
    up(topdown_after(),3).save('ua.png'); up(fps_after(),3).save('ub.png'); print(S.size)
