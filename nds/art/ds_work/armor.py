from dslib import *
from tanks_ds import *
SKYA=[(70,84,104),(96,110,126),(128,138,146),(162,164,158),(196,186,160)]
DIRT2=[(70,58,38),(92,78,52),(116,98,66),(140,120,82),(164,142,100)]
GRS=[(56,62,36),(74,80,46),(94,98,58)]
def scene(W,H,hz=92):
    im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            if y<hz:
                c=dith(0.1+0.85*(y/hz)**1.2,x,y,SKYA)
                cl=noise(x*0.5,y*1.8,12)
                if cl>0.64 and y<70: c=dith(0.6+(cl-0.64)*2,x,y,SKYA)
            else:
                k=(y-hz)/(H-hz)
                c=dith(0.3+0.45*k+0.2*noise(x*0.5,y*2.5,7),x,y,DIRT2)
                if noise(x*0.35+50,y*2.2,6)>0.68: c=dith(0.35+0.45*k,x,y,GRS)
                if int(y+3*math.sin(x/30))%11==0 and (x//3)%3: c=DIRT2[1]
            P[x,y]=c
    # distant ridges
    for x in range(W):
        h1=hz-6-8*noise(x,0,22)-4*noise(x,9,7); h2=hz-2-5*noise(x+90,0,14)
        for y in range(int(h1),hz):
            P[x,y]=dith(0.35+0.2*(y-h1)/max(1,hz-h1),x,y,[(70,74,72),(86,88,82),(104,104,94),(122,120,104)])
        for y in range(int(h2),hz+1):
            P[x,y]=dith(0.3,x,y,[(76,72,56),(92,86,64),(108,100,74)])
    return im
def smoke(im,x,y,h,w0=3,w1=12,seed=1):
    P=im.load(); W,H=im.size
    SM=[(34,34,34),(52,52,50),(74,72,68),(98,96,90),(124,120,112)]
    for k in range(int(h)):
        yy=y-k; w=w0+(w1-w0)*(k/h)**0.8; cx=x+k*0.45+2*math.sin(k/6)
        for xx in range(int(cx-w),int(cx+w)+1):
            e=abs(xx-cx)/max(w,1)
            if e<1 and noise(xx*0.8,yy*0.8,4)*0.6+noise(xx*2,yy*2,2)*0.4>0.12+0.55*e*e and 0<=xx<W and 0<=yy<H:
                P[xx,yy]=dith(0.75-0.45*e-0.25*k/h+0.15*noise(xx,yy,2),xx,yy,SM)
def dust(im,x,y,w,h,seed=2): 
    P=im.load(); W,H=im.size
    for yy in range(y-h,y):
        for xx in range(x-w,x+w):
            dd=((xx-x)/w)**2+((yy-y)/h)**2
            if dd<1 and noise(xx,yy,3)>0.35+0.5*dd and 0<=xx<W and 0<=yy<H:
                P[xx,yy]=dith(0.8-0.5*dd,xx,yy,[(120,108,84),(150,136,108),(180,166,136),(206,194,164)])
def top():
    im=scene(SW,SH); d=ImageDraw.Draw(im)
    smoke(im,40,92,56,3,14,seed=4); smoke(im,222,90,34,2,9,seed=6)
    for (x,c) in ((40,(30,28,24)),(222,(30,28,24))):
        d.rectangle((x-5,90,x+5,93),fill=c); d.point((x,90),fill=(255,160,60)); d.point((x+2,91),fill=(255,200,90))
    # T-90 (midground, facing left)
    P,L=t90(); at(im,P,L,168,130,0.6)
    # M1A2 (foreground, facing right)
    P,L=m1a2(); mx,my,sc=122,172,0.82
    at(im,P,L,mx,my,sc,flip=True)
    muz=(int(mx+38*sc)+1,int(my-35*sc))
    # muzzle blast + APFSDS tracer + impact
    dust(im,muz[0]-6,my-1,26,9)
    d.polygon([(muz[0],muz[1]-1),(muz[0]+10,muz[1]-5),(muz[0]+6,muz[1]),(muz[0]+12,muz[1]+4),(muz[0],muz[1]+2)],fill=(255,196,70))
    d.polygon([(muz[0],muz[1]),(muz[0]+6,muz[1]-2),(muz[0]+5,muz[1]+2)],fill=(255,250,210))
    tgt=(181,112)
    d.line((muz[0]+12,muz[1]-2,tgt[0]-2,tgt[1]+1),fill=(255,236,150))
    for a in range(0,360,45):
        d.line((tgt[0],tgt[1],tgt[0]+5*math.cos(math.radians(a)),tgt[1]+5*math.sin(math.radians(a))),fill=(255,220,120))
    d.point(tgt,fill=WHITE)
    # target bracket
    bx0,by0,bx1,by1=136,94,246,134; R_=(255,80,60)
    for (x0,y0,sx,sy) in ((bx0,by0,1,1),(bx1,by0,-1,1),(bx0,by1,1,-1),(bx1,by1,-1,-1)):
        d.line((x0,y0,x0+sx*7,y0),fill=R_); d.line((x0,y0,x0,y0+sy*7),fill=R_)
    d.rectangle((bx0,by0-11,bx0+tw('T-90')+3,by0-2),fill=(140,10,10)); text(im,bx0+2,by0-10,'T-90',WHITE,shadow=None)
    text(im,bx0+tw('T-90')+8,by0-10,'HIT: TURRET',(255,220,120))
    # HUD bars
    d.rectangle((0,0,SW-1,10),fill=(12,14,10)); d.line((0,11,SW-1,11),fill=(36,40,20))
    text(im,3,2,'M1A2 ABRAMS',GOLD); rtext(im,212,2,'HULL',DIM); rtext(im,252,2,'82%',GREEN)
    d.rectangle((0,181,SW-1,191),fill=(12,14,10)); d.line((0,180,SW-1,180),fill=(36,40,20))
    text(im,3,183,'APFSDS',GOLD); text(im,46,183,'LOADED',GREEN); text(im,92,183,'RNG 1,850M',WHITE); text(im,160,183,'ENEMY 3',(255,140,120))
    tag(im,SW-41,14)
    return im
# ---------------- bottom
TH=[(14,14,14),(40,40,40),(70,70,70),(104,104,104),(140,140,140),(184,184,184),(228,228,228),(255,255,255)]
def thermal(im,x0,y0,x1,y1):
    slot(im,x0,y0,x1,y1,fill=(10,10,10))
    w,h=x1-x0-3,y1-y0-3; t=Image.new('RGB',(w,h)); P=t.load()
    hz=int(h*0.52)
    for y in range(h):
        for x in range(w):
            k=0.18+0.12*noise(x,y,8) if y<hz else 0.26+0.14*noise(x*1.5,y*3,5)+0.08*(y-hz)/h
            P[x,y]=dith(k,x,y,TH)
    for x in range(w):
        hh=hz-3-5*noise(x,3,14)
        for y in range(int(hh),hz): P[x,y]=dith(0.3,x,y,TH)
    parts,lines=t90()
    heat={}
    for i,p in enumerate(parts):
        q=dict(p); q['ramp']=TH[1:7]
        bb=[c for c in (p['poly'] if 'poly' in p else [(p['circle'][0],p['circle'][1])])]
        mx=sum(a for a,b in bb)/len(bb)
        base=0.28+0.5*max(0,(mx-40)/80) if 'circle' not in p else 0.55
        q['shade']=(lambda b: (lambda lx,ly,u,v: b+0.2*(1-v)))(base)
        q.pop('rampf',None); heat[i]=q
    tp=[heat[i] for i in range(len(parts))]
    at(t,tp,[],w//2-24,hz+30,0.66)
    # hot engine deck + exhaust
    d=ImageDraw.Draw(t)
    for yy in range(hz+12,hz+16):
        for xx in range(w//2+38,w//2+52):
            if (xx+yy)&1 or yy>hz+13: P[xx,yy]=TH[7]
    # GPS-style reticle
    cx,cy=w//2-24+int(-6*0.66)+30,hz+30-int(30*0.66)
    cx,cy=w//2+8,hz+10
    col=(250,250,250); dk=(0,0,0)
    for (a,b,c_,e) in ((cx-40,cy,cx-5,cy),(cx+5,cy,cx+40,cy),(cx,cy-30,cx,cy-5),(cx,cy+5,cx,cy+22)):
        d.line((a,b+1,c_,e+1),fill=dk); d.line((a,b,c_,e),fill=col)
    for k in range(1,4): d.line((cx-k*9,cy+2,cx-k*9,cy+4),fill=col); d.line((cx+k*9,cy+2,cx+k*9,cy+4),fill=col)
    d.rectangle((cx-6,cy-6,cx+6,cy+6),outline=col)
    im.paste(t,(x0+2,y0+2))
def dart(im,box_,heat=False):
    if not heat:
        P=[W(rect(0,-1,40,1),[(60,60,56),(110,110,104),(170,170,160),(210,210,200)],V()),W([(40,-1),(46,0),(40,1)],[(120,90,40),(180,140,60),(230,190,100)],flat(0.6)),
           W([(0,-1),(6,-5),(9,-5),(8,-1)],[(60,60,56),(110,110,104),(170,170,160)],flat(0.6)),W([(0,1),(6,5),(9,5),(8,1)],[(60,60,56),(110,110,104),(170,170,160)],flat(0.4)),
           W([(16,-3.5),(30,-3.5),(30,3.5),(16,3.5)],[(70,60,30),(110,94,50),(150,130,70)],V(),)]
    else:
        P=[W([(0,-4),(30,-4),(30,4),(0,4)],[(40,46,30),(70,80,46),(100,110,64),(130,140,90)],cyl(0.3,0.1,0.9),col=True),W([(30,-4),(40,-2),(46,-0.5),(46,0.5),(40,2),(30,4)],[(80,70,30),(140,120,50),(200,180,90)],V()),
           W(rect(-4,-3,0,3),[(60,50,30),(110,90,50),(150,130,70)],flat(0.5))]
    fit(im,P,[],box_)
def ammo_btn(im,x0,y0,x1,y1,name,cnt,sel,heat=False):
    bevel(im,x0,y0,x1,y1,(92,102,56) if sel else PF,raised=not sel)
    if sel:
        d=ImageDraw.Draw(im); d.rectangle((x0,y0,x1,y1),outline=GOLD)
    dart(im,(x0+4,y0+3,x0+52,y0+20),heat)
    text(im,x0+5,y0+21,name,GOLD if sel else TX)
    text(im,x1-tw(cnt,2)-3,y0+6,cnt,WHITE,scale=2)
def damage(im,x0,y0,x1,y1):
    slot(im,x0,y0,x1,y1,fill=(28,32,18))
    d=ImageDraw.Draw(im)
    cx=x0+34; cy=y0+37
    G_=(90,200,80); Y_=(230,200,60); R_=(230,70,50); O=(10,10,8)
    def zone(b,c): d.rectangle(b,fill=O); d.rectangle((b[0]+1,b[1]+1,b[2]-1,b[3]-1),fill=c)
    zone((cx-19,cy-23,cx-12,cy+24),R_); zone((cx+12,cy-23,cx+19,cy+24),G_)
    zone((cx-12,cy-25,cx+12,cy-13),G_); zone((cx-12,cy-13,cx+12,cy+15),Y_); zone((cx-12,cy+15,cx+12,cy+24),G_)
    d.polygon([(cx-9,cy-10),(cx+9,cy-10),(cx+11,cy+12),(cx-11,cy+12)],fill=O)
    d.polygon([(cx-8,cy-9),(cx+8,cy-9),(cx+10,cy+11),(cx-10,cy+11)],fill=(110,190,90))
    d.rectangle((cx-1,cy-32,cx+1,cy-9),fill=O); d.line((cx,cy-31,cx,cy-10),fill=(160,170,120))
    for yy in range(cy-21,cy+23,4): d.line((cx-18,yy,cx-13,yy),fill=mul(R_,0.6)); d.line((cx+13,yy,cx+18,yy),fill=mul(G_,0.6))
    tx=x0+62
    text(im,tx,y0+3,'ARMOR',YG)
    rows=[('FRONT','100%',G_),('TURRET','92%',G_),('HULL','64%',Y_),('L TRACK','HIT',R_),('ENGINE','OK',G_)]
    for i,(k,v,c) in enumerate(rows):
        y=y0+13+i*10; text(im,tx,y,k,TX); rtext(im,x1-4,y,v,c)
def tmap(im,x0,y0,x1,y1):
    slot(im,x0,y0,x1,y1,fill=(40,52,26))
    w,h=x1-x0-3,y1-y0-3; m=Image.new('RGB',(w,h)); P=m.load()
    for y in range(h):
        for x in range(w): P[x,y]=dith(0.3+0.45*noise(x*2,y*2,9),x,y,[(70,62,40),(90,80,52),(110,98,64),(84,92,52)])
    d=ImageDraw.Draw(m)
    for y in range(h): 
        X=int(w*0.6+6*math.sin(y/8)); P[X,y]=(60,50,30)
    for gx in range(0,w,24):
        for y in range(0,h,2): P[gx,y]=mix(P[gx,y],(170,170,120),0.3)
    def tank_ic(x,y,col,ang):
        d.rectangle((x-3,y-2,x+3,y+2),fill=SHADOW); d.rectangle((x-2,y-1,x+2,y+1),fill=col)
        d.line((x,y,x+int(5*math.cos(ang)),y+int(5*math.sin(ang))),fill=col)
    for (x,y) in ((20,40),(30,48),(16,54)): tank_ic(x,y,(110,190,255),-0.3)
    for (x,y) in ((64,26),(76,36),(84,18)): tank_ic(x,y,(255,90,70),2.8)
    d.ellipse((50,6,62,18),outline=(240,200,60)); text(m,54,8,'A',(240,200,60))
    d.polygon([(30,44),(34,48),(30,52),(26,48)],outline=WHITE)
    im.paste(m,(x0+2,y0+2))
    text(im,x0+4,y0+3,'MAP',WHITE)
def bottom():
    im=Image.new('RGB',(SW,SH),(22,24,14)); d=ImageDraw.Draw(im)
    bevel(im,0,0,255,15); text(im,4,4,'GUNNER',GOLD); text(im,46,4,'TIS 10X',TX); text(im,94,4,'WHT',DIM); ctext(im,160,4,'RNG 1,850',WHITE); rtext(im,252,4,'SCORE 900',GOLD)
    bevel(im,0,17,153,103); thermal(im,2,19,151,101)
    bevel(im,155,17,255,103); text(im,159,20,'AMMO',YG); rtext(im,251,20,'TAP',DIM)
    ammo_btn(im,158,29,252,63,'APFSDS','18',True)
    ammo_btn(im,158,66,252,100,'HEAT','12',False,heat=True)
    bevel(im,0,105,153,174); damage(im,2,107,151,172)
    bevel(im,155,105,255,174); tmap(im,157,107,253,172)
    d.rectangle((0,177,255,191),fill=(12,14,8))
    text(im,4,181,'L: ZOOM  R: FIRE',DIM); text(im,110,181,'RELOAD 6S',(240,200,60))
    tag(im,SW-41,180)
    return im
if __name__=='__main__':
    t=top(); b=bottom(); up(t,3).save('v_at1.png'); up(b,3).save('v_ab1.png')
