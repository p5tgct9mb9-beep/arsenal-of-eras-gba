from m1show import *
from f4ds import tabs
LBL=(150,170,200); OLB=(196,184,140)
def callouts(im,xf,box_):
    d=ImageDraw.Draw(im); x0,y0,x1,y1=box_; LC=(120,124,100)
    # (text, anchor, row y, label x offset from anchor in px, up?)
    spec=[('REAR SIGHT',(13.17,-2.2),y0+4,-42,True),('OP ROD HANDLE',(18.9,-0.6),y0+15,-33,True),
          ('GAS CYLINDER',(39.0,-0.6),y0+15,-60,True),('FRONT SIGHT',(42.45,-1.4),y0+4,-65,True),
          ('SAFETY',(15.9,3.05),y1-20,-21,False),('M1907 SLING',(24.0,6.1),y1-10,-33,False),('BAYONET LUG',(42.2,0.86),y1-20,-63,False)]
    for s_,(ax,ay),ty,dx,upw in spec:
        X,Y=xf.f(ax,ay); X=round(X); Y=round(Y); w=tw(s_); tx=max(x0+2,min(x1-w-1,X+dx))
        if upw:
            d.line((X,ty+9,X,Y-2),fill=LC)
            d.line((min(X,tx),ty+9,max(X,tx+w-1),ty+9),fill=LC)
        else:
            d.line((X,Y+2,X,ty-2),fill=LC); d.line((min(X,tx),ty-2,max(X,tx+w-1),ty-2),fill=LC)
        text(im,tx,ty,s_,OLB)
def top():
    im=Image.new('RGB',(256,192),NAVYD); d=ImageDraw.Draw(im)
    d.rectangle((0,0,255,11),fill=(6,8,18)); d.line((0,11,255,11),fill=NAVYL)
    text(im,4,3,'ARMORY > WW2 > RIFLES > INSPECT',LBL)
    text(im,4,14,'M1 GARAND',GOLD,scale=2,adv=6)
    rtext(im,252,16,'USA',BLUE); rtext(im,252,25,'WW2',GREEN)
    nbevel(im,2,32,253,163,edge=(110,100,60))
    bg=felt(248,127); im.paste(bg,(4,34))
    xf=draw(im,garand,(8,58,248,126))
    callouts(im,xf,(4,34,251,161))
    d.rectangle((0,166,255,191),fill=(6,8,18)); d.line((0,166,255,166),fill=NAVYL)
    text(im,4,170,'U.S. RIFLE, CALIBER .30, M1',(206,210,220))
    L=round(10*xf.sc); d.rectangle((6,182,6+L,184),fill=(10,10,10)); d.rectangle((7,183,6+L//2,183),fill=(236,236,230)); d.rectangle((7+L//2,183,5+L,183),fill=(120,120,120))
    text(im,6+L+4,180,'10 IN',(210,214,222)); rtext(im,252,181,'VIEW 1/2',LBL)
    return im
def bottom(tab=0):
    im=Image.new('RGB',(256,192),NAVYD); d=ImageDraw.Draw(im)
    tabs(im,1,['OVERVIEW','HISTORY','ATTACHMENTS'],tab)
    nbevel(im,2,19,253,80)
    rows=[('CALIBER','.30-06 SPRINGFIELD'),('FEED',"8-RD EN-BLOC CLIP ('PING!')"),('ACTION','GAS-OPERATED, SEMI-AUTO'),
          ('IN SERVICE','FROM 1936'),('PRODUCED','~5.4 MILLION'),('WEIGHT','~4.3 KG (9.5 LB) EMPTY')]
    for i,(a,b) in enumerate(rows):
        y=23+i*9
        if i%2: d.rectangle((5,y-1,250,y+7),fill=(26,38,70))
        text(im,8,y,a,LBL); rtext(im,249,y,b,WHITE)
    text(im,4,83,'WW2 KIT',GOLD)
    names=[('M1','HELMET'),('M1923','BELT'),('MK 2','GRENADE'),('M1910','CANTEEN'),('M3','KNIFE')]
    for i,f in enumerate([helmet,belt,grenade,canteen,knife_r]):
        x=3+i*50; nbevel(im,x,92,x+49,140,edge=(110,100,60),sel=(i==0),fill=(30,30,22))
        t=felt(44,27); im.paste(t,(x+3,95)); draw(im,f,(x+4,96,x+46,121))
        ctext(im,x+25,124,names[i][0],WHITE); ctext(im,x+25,132,names[i][1],LBL)
    text(im,4,143,'PERIOD ATTACHMENTS',GOLD)
    att=[('M1 BAYONET','10 IN BLADE',bayonet),('M7 GRENADE','LAUNCHER',m7gl),('M84 SCOPE','M1C / M1D',m84)]
    for i,(a,b,f) in enumerate(att):
        x=3+i*84; nbevel(im,x,151,x+82,188,edge=(110,100,60),fill=(30,30,22))
        t=felt(77,14); im.paste(t,(x+3,154)); draw(im,f,(x+4,154,x+79,168))
        ctext(im,x+42,171,a,WHITE); ctext(im,x+42,179,b,LBL)
    return im
if __name__=='__main__':
    t=top(); b=bottom()
    save_pair(t,b,'/workspace/ds_m1_showcase.png',label='ARSENAL OF ERAS')
    t.save('show/n_m1_top.png'); b.save('show/n_m1_bot.png')
    bg=felt(264,72); draw(bg,garand,(8,6,256,66)); tag(bg); up(bg,4).save('/workspace/ds_m1_art_large.png')
    up(t,4).save('show/m1_top_v3.png'); up(b,4).save('show/m1_bot_v3.png')
