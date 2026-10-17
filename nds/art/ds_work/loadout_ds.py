from dslib import *
from m4ds import m4a1
ON=('RDS','SUP','GRIP','LASER','EXT')
def bench(W,H):
    im=Image.new('RGB',(W,H)); P=im.load()
    for y in range(H):
        for x in range(W):
            t=0.55-0.35*abs(y-H*0.45)/H+0.12*noise(x,y,6)
            c=dith(t,x,y,[(24,26,16),(34,38,22),(46,50,30),(58,64,38),(72,78,48)])
            if x%8==3 and y%8==3: c=(18,20,12)
            P[x,y]=c
    return im
def statbar(im,x,y,k,v,delta=0,w=70):
    d=ImageDraw.Draw(im); text(im,x,y,k,TX)
    bx=x+52; d.rectangle((bx,y+1,bx+w,y+5),fill=SHADOW); d.rectangle((bx+1,y+2,bx+w-1,y+4),fill=(40,44,26))
    base=int((w-2)*v/100); d.rectangle((bx+1,y+2,bx+base,y+4),fill=(150,160,96))
    if delta>0: d.rectangle((bx+base+1,y+2,bx+base+int((w-2)*delta/100),y+4),fill=(124,236,96))
    if delta<0: d.rectangle((bx+base+int((w-2)*delta/100)+1,y+2,bx+base,y+4),fill=(240,80,60))
    s=('+%d'%delta) if delta>0 else ('%d'%delta) if delta<0 else ''
    if s: text(im,bx+w+4,y,s,(124,236,96) if delta>0 else (240,80,60))
def top():
    im=Image.new('RGB',(SW,SH),(22,24,14)); bevel(im,0,0,255,191)
    text(im,4,4,'GUNSMITH',GOLD); text(im,58,4,'M4A1 CARBINE',TX)
    slot(im,3,14,252,122,fill=(52,58,34))
    b=bench(246,105); im.paste(b,(5,16))
    xf,(ox,oy)=draw_item(im,lambda:m4a1(ON),(8,24,250,116))
    d=ImageDraw.Draw(im)
    def call(lx,ly,tx,ty,s,col=GOLD):
        X,Y=xf.f(lx,ly); X+=ox; Y+=oy
        ex=tx+tw(s)//2; ey=ty+8 if ty<Y else ty-1
        d.line((X,Y,ex,ey),fill=col); d.rectangle((X-1,Y-1,X+1,Y+1),fill=col)
        d.rectangle((tx-2,ty-1,tx+tw(s)+1,ty+7),fill=(12,14,8)); d.rectangle((tx-2,ty-1,tx+tw(s)+1,ty+7),outline=mul(col,0.6))
        text(im,tx,ty,s,col,shadow=None)
    call(40,-21,14,19,'RED DOT'); call(76,-17,96,19,'LASER'); call(118,-4,186,19,'SUPPRESSOR')
    call(70,12,126,110,'FOREGRIP'); call(53,26,10,110,'EXT MAG 40RD')
    # stats
    text(im,6,126,'5.56X45MM NATO',YG); rtext(im,250,126,'14.5IN BARREL',DIM)
    rows=[('DAMAGE',58,0),('RANGE',62,6),('ACCURACY',60,12),('HANDLING',66,-8),('STEALTH',30,40)]
    for i,(k,v,dl) in enumerate(rows):
        x=6 if i<3 else 132; y=137+(i%3)*10
        statbar(im,x,y,k,v,dl,w=44)
    text(im,132,157,'RECOIL',TX); text(im,184,157,'LOW',(124,236,96))
    d.rectangle((3,170,252,188),fill=(16,18,10)); d.rectangle((3,170,252,188),outline=PD)
    rock(im,8,175); text(im,20,175,'BUILD COST',DIM); text(im,84,175,'640',PR); text(im,120,175,'5 OF 6 SLOTS',TX)
    tag(im,SW-43,3)
    return im
SLOTS=[('RDS','RED DOT',1,'150'),('X4','4X SCOPE',0,'200'),('SNIPE','SNIPER',0,'400'),('SUP','SUPPRESS',1,'180'),
       ('GRIP','FOREGRIP',1,'90'),('LASER','LASER',1,'120'),('BIPOD','BIPOD',0,'140'),('EXT','EXT MAG',1,'100'),('M4','RESET',2,None)]
def bottom():
    im=Image.new('RGB',(SW,SH),(22,24,14)); d=ImageDraw.Draw(im)
    bevel(im,0,0,255,15); text(im,4,4,'ATTACHMENTS',YG); text(im,76,4,'TOUCH TO EQUIP',DIM)
    rock(im,206,4); text(im,218,4,'2,400',PR)
    bevel(im,0,17,173,159)
    cw,ch=56,46
    for i,(it,nm,eq,pr) in enumerate(SLOTS):
        x=3+(i%3)*cw; y=20+(i//3)*ch
        sel=(i==1)
        slot(im,x,y,x+cw-2,y+ch-2,fill=(98,110,64) if eq==1 else (72,80,46),sel=sel)
        if it=='M4':
            ctext(im,x+cw//2-1,y+12,'RESET',TX); ctext(im,x+cw//2-1,y+22,'BUILD',DIM)
            continue
        draw_item(im,ITEMS[it],(x+4,y+11,x+cw-6,y+33))
        text(im,x+3,y+35,nm,TX if eq else DIM)
        if eq: d.rectangle((x+cw-21,y+2,x+cw-5,y+9),fill=(16,18,10)); text(im,x+cw-19,y+3,'ON',PR,shadow=None)
        else: rock(im,x+3,y+2); text(im,x+13,y+3,pr,PR)
    # selected-item info
    bevel(im,0,161,173,191)
    text(im,4,165,'4X SCOPE',GOLD); rock(im,60,165); text(im,72,165,'200',PR); text(im,98,165,'NOT OWNED',(240,140,90))
    text(im,4,176,'+RANGE  -HANDLING',DIM)
    # portrait + stats
    bevel(im,175,17,255,191)
    text(im,179,20,'HALE',GOLD); rtext(im,251,20,'SGT',YG)
    s=spec(skin=4,hair='BLACK',face='STUBBLE',head='HELMET',unif='OLIVE DRAB',camo='WOODLAND',gear='PLATE',rifle='M4',helmcol=(80,90,56))
    portrait(im,178,29,252,79,s,sc=3.6)
    for i,(k,v) in enumerate([('SHT','45'),('SPD','20'),('STR','62'),('HP','76'),('THR','39'),('MOR','70')]):
        y=83+i*9; text(im,180,y,k,TX); rtext(im,250,y,v,PR)
    bevel(im,178,140,252,159,(92,102,56),raised=True); ctext(im,216,146,'BUY 200',GOLD)
    bevel(im,178,162,214,188); ctext(im,197,171,'BACK',TX)
    bevel(im,216,162,252,188); ctext(im,235,171,'SAVE',TX)
    tag(im,124,4) if False else None
    tag(im,130,165+12-12+0) if False else None
    d.rectangle((130,175,170,186),fill=(0,0,0)); tag(im,130,175)
    return im
if __name__=='__main__':
    t=top(); b=bottom(); up(t,3).save('v_lt1.png'); up(b,3).save('v_lb1.png')
