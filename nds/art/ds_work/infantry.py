from dslib import *
from m4ds import m4a1
from upgrade import brick_wall, sandbags, crate, plate, arrow, ring, GR, DIRT, BLUE_T, RED_T
random.seed(4)
def ground(W,H,rx=206):
    im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            n=noise(x,y,9)*0.6+noise(x+300,y,3)*0.4
            dr=abs(x-rx-7*math.sin(y/26))
            if dr<12:
                P[x,y]=dith(0.35+0.5*noise(x,y,4),x,y,DIRT)
                if dr>10.5: P[x,y]=dith(0.3,x,y,GR)
            else: P[x,y]=dith(0.22+0.62*n,x,y,GR)
    d=ImageDraw.Draw(im); rnd=random.Random(2)
    for _ in range(130):
        x,y=rnd.randrange(W),rnd.randrange(H)
        if abs(x-rx-7*math.sin(y/26))>13: d.point((x,y),fill=GR[4]); d.point((x,y+1),fill=GR[1])
    for y in range(H):
        for o in (-5,5):
            X=int(rx+7*math.sin(y/26)+o)
            if y%3: P[X,y]=DIRT[0]
    return im
def bush(im,x,y,r=7):
    B=[(16,30,12),(28,48,18),(44,68,26),(64,90,36),(88,112,50)]
    parts=[dict(circle=(x+dx,y+dy,rr),ramp=B,shade=lambda lx,ly,u,v:0.95-0.55*v-0.25*u) for dx,dy,rr in ((-r*0.6,1,r*0.7),(r*0.5,1.5,r*0.65),(0,-r*0.4,r*0.75))]
    render2(im,parts,Xf(0,0,1))
def objective(im,x,y,letter,col):
    P=im.load()
    for a in range(0,360,6):
        X=int(x+20*math.cos(math.radians(a))); Y=int(y+9*math.sin(math.radians(a)))
        if (a//6)%2==0: P[X,Y]=col; P[X,Y+1]=mul(col,0.4)
    d=ImageDraw.Draw(im); d.line((x,y,x,y-16),fill=(30,30,30)); d.line((x+1,y,x+1,y-16),fill=(150,150,150))
    d.rectangle((x+2,y-16,x+11,y-9),fill=SHADOW); d.rectangle((x+2,y-16,x+10,y-10),fill=col); text(im,x+4,y-16,letter,WHITE,shadow=None)
BLU=dict(head='HELMET',unif='OLIVE DRAB',camo='WOODLAND',gear='PLATE',rifle='M4',helmcol=(80,90,56))
OPF=dict(head='HELMET',unif='KHAKI',camo='SOLID',gear='WEBBING',rifle='AK',helmcol=(96,92,66))
def tracer(im,a,b,col=(255,236,140)):
    d=ImageDraw.Draw(im); d.line((*a,*b),fill=col)
    d.point(a,fill=(255,255,220))
def flash(im,x,y):
    d=ImageDraw.Draw(im); d.polygon([(x-3,y),(x,y-2),(x+3,y),(x,y+2)],fill=(255,200,60)); d.point((x,y),fill=(255,255,230))
def top():
    W,H=SW,SH; im=ground(W,H)
    bush(im,14,150); bush(im,244,26,6); bush(im,150,176,6); bush(im,116,22,5)
    brick_wall(im,14,24,86,14); brick_wall(im,14,24,10,62); brick_wall(im,118,104,52,14)
    sandbags(im,70,146,5); crate(im,226,146); crate(im,134,46); crate(im,30,98)
    objective(im,104,92,'B',(220,180,60))
    units=[(68,84,'B','N','HALE',3),(44,116,'B','S',None,5),(94,108,'B','E',None,2),(56,160,'B','N',None,6),(140,150,'B','E',None,4),(30,76,'B','E',None,1),
           (168,72,'R','S',None,2),(214,76,'R','W',None,4),(190,134,'R','W',None,1),(236,116,'R','S',None,3),(196,52,'R','S',None,6)]
    sp={}
    for (x,y,t,v,nm,sk) in sorted(units,key=lambda u:u[1]):
        base=BLU if t=='B' else OPF
        s=spec(skin=sk,hair='BLACK' if sk>3 else 'BROWN',face='STUBBLE' if nm else 'CLEAN',**base)
        view={'N':'B','S':'F','E':'S','W':'S'}[v]
        spr=sprite(s,0.98,view,warp=True,flip=(v=='W'))
        col=BLUE_T if t=='B' else RED_T
        if nm: ring(im,x,y,(120,220,255))
        place(im,spr,x,y,shadow_col=(36,48,22),sw=14)
        top_=y-32
        if nm: plate(im,x,top_-10,nm,col); arrow(im,x,top_-1,col)
        else: arrow(im,x,top_,col)
    # firefight: blue rifleman east-facing shoots at red near road, red returns fire
    tracer(im,(108,96),(176,82)); flash(im,107,96)
    tracer(im,(180,118),(156,146),(255,170,90)); flash(im,181,118)
    # hit marker
    d=ImageDraw.Draw(im)
    # HUD
    d.rectangle((0,0,W-1,10),fill=(12,14,10)); d.line((0,11,W-1,11),fill=(36,40,20))
    text(im,3,2,'USA',BLUE_T); text(im,25,2,'14',WHITE); ctext(im,128,2,'3:42',WHITE); rtext(im,W-3,2,'OPFOR',RED_T); rtext(im,W-37,2,'11',WHITE)
    d.rectangle((0,181,W-1,191),fill=(12,14,10)); d.line((0,180,W-1,180),fill=(36,40,20))
    for i in range(5):
        x=3+i*8; d.polygon([(x,183),(x+2,182),(x+3,183),(x+4,182),(x+6,183),(x+6,185),(x+3,188),(x,185)],fill=(230,50,50) if i<4 else (70,30,30))
    text(im,48,183,'M4A1',WHITE); text(im,78,183,'27/90',GRAY); text(im,124,183,'NADE 2',GOLD); text(im,172,183,'B',(220,180,60)); text(im,180,183,'62%',(220,180,60)); d.rectangle((202,184,252,187),fill=(40,40,24)); d.rectangle((202,184,233,187),fill=(220,180,60))
    tag(im,2,14)
    return im
# ---------------- bottom
def minimap(im,x0,y0,x1,y1):
    slot(im,x0,y0,x1,y1,fill=(40,52,26))
    w,h=x1-x0-3,y1-y0-3; m=Image.new('RGB',(w,h)); P=m.load()
    for y in range(h):
        for x in range(w):
            rd=abs(x-88-5*math.sin(y/18))
            P[x,y]=dith(0.3+0.4*noise(x*2,y*2,10),x,y,[(28,40,20),(38,52,26),(50,66,32),(62,80,40)]) if rd>4 else dith(0.5,x,y,[(84,68,44),(110,90,58)])
    d=ImageDraw.Draw(m)
    for b in [(10,10,40,14),(10,10,14,34),(46,52,66,56),(66,80,84,92),(20,86,34,96),(92,20,104,30)]:
        d.rectangle(b,fill=(26,24,18)); d.rectangle((b[0]+1,b[1]+1,b[2]-1,b[3]-1),fill=(92,56,40))
    # grid
    for gx in range(0,w,29):
        for y in range(0,h,2): P[gx,y]=mix(P[gx,y],(140,150,100),0.35)
    for gy in range(0,h,29):
        for x in range(0,w,2): P[x,gy]=mix(P[x,gy],(140,150,100),0.35)
    # objectives
    for (ox,oy,L,col) in ((22,20,'A',BLUE_T),(52,62,'B',(220,180,60)),(94,98,'C',RED_T)):
        d.ellipse((ox-7,oy-7,ox+7,oy+7),outline=SHADOW); d.ellipse((ox-6,oy-6,ox+6,oy+6),outline=col)
        text(m,ox-2,oy-3,L,col)
    # camera view box (what the top screen shows)
    d.rectangle((24,28,72,64),outline=(220,220,200))
    # dots
    for (x,y) in ((30,44),(36,52),(44,48),(32,60),(56,58),(28,36)): d.rectangle((x-2,y-2,x+2,y+2),fill=SHADOW); d.rectangle((x-1,y-1,x+1,y+1),fill=BLUE_T)
    for (x,y) in ((62,40),(70,42),(66,56),(74,50),(70,32),(98,90)): d.rectangle((x-2,y-2,x+2,y+2),fill=SHADOW); d.rectangle((x-1,y-1,x+1,y+1),fill=RED_T)
    # player arrow
    px,py=38,44; d.polygon([(px,py-4),(px+3,py+3),(px,py+1),(px-3,py+3)],fill=WHITE,outline=SHADOW)
    # ping
    for a in range(0,360,30):
        X=int(70+6*math.cos(math.radians(a))); Y=int(42+6*math.sin(math.radians(a)))
        P[X,Y]=(255,220,80)
    im.paste(m,(x0+2,y0+2))
    text(im,x0+4,y0+3,'N',WHITE); 
def bullet_pips(im,x,y,n,tot,per=15):
    d=ImageDraw.Draw(im)
    for i in range(tot):
        X=x+(i%per)*4; Y=y+(i//per)*7
        c=(214,180,90) if i<n else (50,54,34)
        d.rectangle((X,Y,X+2,Y+5),fill=SHADOW); d.rectangle((X,Y+1,X+1,Y+5),fill=c)
        if i<n: d.point((X,Y),fill=(240,220,150))
def nade_icon(im,box_):
    fit(im,[W(ell(5,8,5,6,12),R((84,96,52)),lambda lx,ly,u,v:0.95-0.5*u-0.3*v),W(rect(3,-1,7,3),PARK,flat(0.5)),W(rect(7,0,10,2),PARK,flat(0.6))],[([(1,6),(9,6)],(44,52,26)),([(1,10),(9,10)],(44,52,26)),([(5,3),(5,13)],(44,52,26))],box_)
def tbutton(im,x0,y0,x1,y1,s,col=TX,sel=False,sub=None):
    bevel(im,x0,y0,x1,y1,(92,102,56) if sel else PF,raised=not sel)
    cy=(y0+y1)//2-(7 if sub else 3)
    ctext(im,(x0+x1)//2+1,cy,s,GOLD if sel else col)
    if sub: ctext(im,(x0+x1)//2+1,cy+9,sub,DIM)
def bottom():
    im=Image.new('RGB',(SW,SH),(22,24,14)); d=ImageDraw.Draw(im)
    bevel(im,0,0,255,15)
    text(im,4,4,'USA',BLUE_T); text(im,26,4,'14',WHITE); text(im,48,4,'K7 D2',TX); ctext(im,140,4,'SCORE 1,250',GOLD); rtext(im,218,4,'11',WHITE); rtext(im,252,4,'OPFOR',RED_T)
    bevel(im,0,17,123,139); text(im,4,20,'TACTICAL MAP',YG)
    minimap(im,3,29,120,136)
    bevel(im,125,17,255,139)
    text(im,129,20,'M4A1',GOLD); text(im,159,20,'CARBINE',TX)
    slot(im,128,29,252,70,fill=(84,94,54))
    draw_item(im,lambda:m4a1(('RDS','GRIP')),(130,30,250,69))
    text(im,129,74,'5.56MM',TX); text(im,129,82,'',DIM)
    text(im,192,72,'27',WHITE,scale=2); text(im,216,79,'/90',GRAY)
    bullet_pips(im,129,89,27,30)
    slot(im,128,105,252,136,fill=(60,68,36))
    nade_icon(im,(131,108,147,133)); text(im,151,110,'FRAG',TX); text(im,151,120,'X2',GOLD)
    fit(im,[W(rect(0,0,8,14),R((60,70,50)),lambda lx,ly,u,v:0.9-0.6*u),W(rect(1,-2,7,0),PARK,flat(0.5))],[([(1,3),(7,3)],(220,220,200))],(190,108,204,133))
    text(im,207,110,'SMOKE',TX); text(im,207,120,'X1',GOLD)
    for i,(a,b) in enumerate([('RELOAD',None),('THROW','NADE'),('CROUCH',None),('AIM','SCOPE')]):
        x=1+i*64; tbutton(im,x,142,x+61,174,a,sel=(i==0),sub=b)
    d.rectangle((0,177,255,191),fill=(12,14,8))
    text(im,4,181,'TAP MAP: PING',DIM); text(im,96,181,'L: AIM  R: FIRE',DIM)
    tag(im,SW-41,180)
    return im
if __name__=='__main__':
    t=top(); b=bottom(); up(t,3).save('v_it1.png'); up(b,3).save('v_ib1.png')
