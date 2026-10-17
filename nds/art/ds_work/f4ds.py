from f4show import *
LBL=(150,170,200)
def tabs(im,y,names,sel=0):
    n=len(names); w=(256-4-(n-1)*3)//n
    for i,s in enumerate(names):
        x=2+i*(w+3); nbevel(im,x,y,x+w,y+15,sel=(i==sel),fill=(34,52,92) if i==sel else PANELN)
        ctext(im,x+w//2+1,y+5,s,GOLD if i==sel else (170,184,206))
def scalebar(im,x,y,xf,m=5):
    d=ImageDraw.Draw(im); L=round(m*xf.sc)
    d.rectangle((x,y,x+L,y+2),fill=(10,10,10)); d.rectangle((x+1,y+1,x+L//2,y+1),fill=(236,236,230)); d.rectangle((x+L//2+1,y+1,x+L-1,y+1),fill=(120,120,120))
    text(im,x+L+4,y-2,'%d M'%m,(210,214,222))
def top():
    im=Image.new('RGB',(256,192),NAVYD); d=ImageDraw.Draw(im)
    d.rectangle((0,0,255,11),fill=(6,8,18)); d.line((0,11,255,11),fill=NAVYL)
    text(im,4,3,'HANGAR > COLD WAR > INSPECT',(150,170,200))
    text(im,4,14,'F-4E PHANTOM II',GOLD,scale=2,adv=6)
    rtext(im,252,16,'USA',BLUE); rtext(im,252,25,'COLD WAR',GREEN)
    nbevel(im,2,32,253,163)
    sk=sky(248,127); xf=draw(sk,side,side_marks,(4,6,244,124)); im.paste(sk,(4,34))
    xf2=Xf(xf.ox+4,xf.oy+34,xf.sc)
    d.rectangle((0,166,255,191),fill=(6,8,18)); d.line((0,166,255,166),fill=NAVYL)
    text(im,4,170,'SIDE PROFILE  -  USAF SOUTHEAST ASIA CAMO',(206,210,220))
    scalebar(im,6,182,xf2,5)
    rtext(im,252,181,'VIEW 1/3',(150,170,200))
    return im,xf2
def bottom(tab=0):
    im=Image.new('RGB',(256,192),NAVYD); d=ImageDraw.Draw(im)
    tabs(im,1,['OVERVIEW','ARMAMENT','HISTORY'],tab)
    nbevel(im,2,19,150,98); nbevel(im,153,19,253,98)
    bgp=Image.new('RGB',(144,72),(20,30,58)); P=bgp.load()
    for y in range(72):
        for x in range(144): P[x,y]=dith(0.3+0.4*y/72,x,y,[(14,22,44),(20,30,58),(26,40,72)])
    for x in range(0,144,12):
        for y in range(72): P[x,y]=(30,46,80) if y%2 else P[x,y]
    bgh=bgp.crop((0,0,96,72))
    draw(bgp,plan,plan_marks,(3,2,141,70)); im.paste(bgp,(5,23)); text(im,8,88,'PLAN',LBL)
    draw(bgh,head,None,(2,14,94,62)); im.paste(bgh,(155,23)); text(im,158,88,'HEAD-ON',LBL)
    nbevel(im,2,101,253,178)
    rows=[('ROLE','FIGHTER-BOMBER/INTERCEPTOR'),('FIRST FLIGHT','1958'),('BUILT','~5,195 (ALL F-4)'),('ENGINES','2X GE J79-GE-17'),
          ('MAX SPEED','MACH 2.2+'),('CREW','2 (PILOT + WSO)'),('GUN','M61A1 20MM (INTERNAL)')]
    for i,(a,b) in enumerate(rows):
        y=106+i*10
        if i%2: d.rectangle((5,y-2,250,y+7),fill=(26,38,70))
        text(im,8,y,a,LBL); rtext(im,249,y,b,WHITE)
    text(im,4,182,'B:BACK  L/R:VIEW  TOUCH:TAB',(150,170,200))
    return im
if __name__=='__main__':
    t,xf=top(); b=bottom()
    im=save_pair(t,b,'/workspace/ds_f4_showcase.png',label='ARSENAL OF ERAS')
    t.save('show/n_f4_top.png'); b.save('show/n_f4_bot.png')
    # standalone large side profile on dithered sky
    sk=sky(272,124,5); draw(sk,side,side_marks,(10,8,262,116)); tag(sk); up(sk,4).save('/workspace/ds_f4_art_large.png')
    up(t,4).save('show/f4_top_v3.png'); up(b,4).save('show/f4_bot_v3.png')
