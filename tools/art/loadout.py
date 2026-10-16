from ui import *
def card(im,x0,y0,x1,y1,item,name,price=None,sel=False,pad=(3,2,3,9)):
    slot(im,x0,y0,x1,y1,sel=sel)
    draw_item(im,ITEMS[item] if isinstance(item,str) else item,(x0+pad[0],y0+pad[1],x1-pad[2],y1-pad[3]))
    if price: rock(im,x0+3,y0+3); text(im,x0+13,y0+3,price,PR)
    text(im,x0+3,y1-8,name,TX)
def portrait(im,x0,y0,x1,y1,s,sc=3.0,cx_off=0,top=None):
    slot(im,x0,y0,x1,y1,fill=(54,58,40))
    spr=sprite(s,sc); img,(ax,ay),mask=spr
    w,h=x1-x0-3,y1-y0-3
    head_top=ay+int(-51*sc)-1 if top is None else ay+int(top*sc)
    c=img.crop((ax-w//2+cx_off,head_top,ax-w//2+cx_off+w,head_top+h))
    # backdrop
    bg=Image.new('RGB',(w,h),(66,72,48)); B=bg.load()
    for y in range(h):
        for x in range(w):
            t=y/h; B[x,y]=dith(0.75-0.6*t,x,y,[(40,44,28),(54,60,38),(70,76,50),(88,94,62),(104,110,74)])
    bg.paste(c,(0,0),c); im.paste(bg,(x0+2,y0+2))
def screen_loadout():
    im=Image.new('RGB',(240,160),(22,24,14)); d=ImageDraw.Draw(im)
    bevel(im,0,0,138,159); bevel(im,139,0,239,159)
    # LEFT ----------------------------------------------------
    text(im,4,3,'PRIMARY',YG); text(im,4+tw('PRIMARY')+6,3,'WEAPON',DIM)
    card(im,3,11,135,41,'THOMPSON','M1A1 THOMPSON','300',sel=True,pad=(26,2,2,2))
    text(im,4,44,'SECONDARY',YG)
    card(im,3,52,135,74,'M1911','M1911A1','200',pad=(70,2,24,1))
    for i,t in enumerate(['PISTOL','RIFLE','MG','ACCESS','OPTION','AT']):
        x=3+(i%3)*44; y=77+(i//3)*12; tab(im,x,y,x+43,y+11,t,sel=(t=='RIFLE'))
    # equipment pool
    text(im,4,102,'EQUIPMENT POOL',YG)
    bevel(im,3,111,10,139); text(im,4,121,'<',YG)
    bevel(im,128,111,135,139); text(im,129,121,'>',YG)
    for i,(it,nm,pr) in enumerate([('GARAND','M1 GARAND','250'),('BAR','M1918 BAR','450')]):
        x=12+i*58; slot(im,x,111,x+57,139)
        draw_item(im,ITEMS[it],(x+2,113,x+56,135)); rock(im,x+3,114); text(im,x+13,114,pr,PR); text(im,x+3,132,nm[:9],TX)
    rock(im,4,146); text(im,14,146,'2400',PR)
    bevel(im,42,143,135,156); text(im,48,147,'RETURN TO SQUAD',TX)
    # RIGHT ---------------------------------------------------
    text(im,143,3,'HALE',GOLD); text(im,206,3,'SGT',YG)
    text(im,143,12,'USA',TX)
    s=spec(skin=5,hair='BLACK',face='STUBBLE',head='M1',unif='OLIVE DRAB',gear='WEBBING',rifle='THOMPSON')
    portrait(im,142,21,177,58,s,sc=3.0)
    slot(im,180,21,236,58,fill=(56,62,34))
    for i,(k,v) in enumerate([('CMD','4'),('SPD','20'),('STR','62'),('HP','76')]):
        text(im,184,25+i*8,k,TX); text(im,232-tw(v),25+i*8,v,PR)
    slot(im,180,61,236,98,fill=(56,62,34))
    for i,(k,v) in enumerate([('SHT','45'),('THR','39'),('MEL','39'),('MOR','70')]):
        text(im,184,65+i*8,k,TX); text(im,232-tw(v),65+i*8,v,PR)
    # inventory slots 2x2: ammo + grenades
    inv=[('AMMO','45 ACP','X4'),('AMMO','45 ACP','X3'),('NADE','MK2','X2'),('SMOKE','M18','X1')]
    for i,(kind,nm,ct) in enumerate(inv):
        x=142+(i%2)*18; y=61+(i//2)*19; slot(im,x,y,x+17,y+18)
        P=im.load()
        if kind=='AMMO':
            draw_item(im,([W([(0,0),(8,0),(8,14),(0,14)],R((92,96,58)),lambda lx,ly,u,v:0.9-0.6*u),W(rect(1,-3,7,0),R((120,100,50)),flat(0.6))],[([(1,5),(7,5)],(60,64,36))]),(x+3,y+2,x+12,y+13))
        elif kind=='NADE':
            draw_item(im,([W(ell(5,8,5,6,12),R((84,96,52)),lambda lx,ly,u,v:0.95-0.5*u-0.3*v),W(rect(3,-1,7,3),PARK,flat(0.5)),W(rect(7,0,10,2),PARK,flat(0.6))],[([(1,6),(9,6)],(44,52,26)),([(1,10),(9,10)],(44,52,26)),([(5,3),(5,13)],(44,52,26))]),(x+3,y+2,x+13,y+13))
        else:
            draw_item(im,([W(rect(0,0,8,14),R((60,70,50)),lambda lx,ly,u,v:0.9-0.6*u),W(rect(1,-2,7,0),PARK,flat(0.5))],[([(1,3),(7,3)],(220,220,200))]),(x+3,y+2,x+12,y+13))
        tiny(im,x+10,y+12,ct[1])
    text(im,143,101,'WT',DIM); text(im,160,101,'11.0KG',TX); text(im,204,101,'/25',DIM)
    # equipped
    for i,(it,nm,pd) in enumerate([('THOMPSON','M1A1',(14,1,2,1)),('M1911','M1911',(36,1,3,1))]):
        y=110+i*24; slot(im,142,y,201,y+23); draw_item(im,ITEMS[it],(142+pd[0],y+pd[1]+1,201-pd[2],y+23-pd[3])); text(im,145,y+15,nm,TX)
    slot(im,204,110,236,157); draw_item(im,binos,(208,113,233,143)); text(im,206,148,'BINO',TX)
    mockup_tag(im,x=196,y=11)
    return im
def screen_attach():
    im=Image.new('RGB',(240,160),(22,24,14)); bevel(im,0,0,239,159)
    text(im,4,3,'ATTACHMENTS',YG); text(im,76,3,'M4A1 CARBINE',TX)
    rock(im,180,12); text(im,190,12,'2400',PR)
    slot(im,3,20,236,84,fill=(84,94,54))
    on=('RDS','SUP','GRIP','LASER','EXT')
    xf,(ox,oy)=draw_item(im,lambda:m4(on),(6,22,234,82))
    # callout labels
    def call(lx,ly,tx,ty,s):
        X,Y=xf.f(lx,ly); X+=ox; Y+=oy; d=ImageDraw.Draw(im); d.line((X,Y,tx,ty+3),fill=GOLD)
        bx=tx-2 if tx<X else tx-2
        d.rectangle((tx-2,ty-1,tx+tw(s)+1,ty+7),fill=(16,18,10)); text(im,tx,ty,s,GOLD,shadow=None)
    call(37,-15,20,23,'RED DOT'); call(75,-12,120,23,'LASER'); call(118,-6,190,30,'SUPP'); call(67,10,96,74,'GRIP'); call(50,22,20,74,'EXT MAG')
    slots=[('RDS','RED DOT',1),('X4','4X SCOPE',0),('SNIPE','SNIPER',0),('SUP','SUPPRESS',1),
           ('GRIP','FOREGRIP',1),('LASER','LASER',1),('BIPOD','BIPOD',0),('EXT','EXT MAG',1)]
    cost=['150','200','400','180','90','120','140','100']
    for i,(it,nm,eq) in enumerate(slots):
        x=3+(i%4)*59; y=87+(i//4)*32; slot(im,x,y,x+57,y+30,fill=(98,110,64) if eq else (72,80,46),sel=(i==1))
        draw_item(im,ITEMS[it],(x+4,y+10,x+54,y+22)); text(im,x+3,y+22,nm,TX if eq else DIM)
        if eq: 
            d=ImageDraw.Draw(im); d.rectangle((x+38,y+2,x+54,y+9),fill=(16,18,10)); text(im,x+40,y+3,'ON',PR,shadow=None)
        else: rock(im,x+3,y+2); text(im,x+13,y+3,cost[i],PR)
    text(im,4,151,'A:MOUNT  B:BACK  L/R:RIFLE',DIM)
    mockup_tag(im,x=196,y=2)
    return im
if __name__=='__main__':
    A=screen_loadout(); B=screen_attach()
    S=Image.new('RGB',(240*2+12,16+12+160+6),(16,16,12))
    t='ARSENAL OF ERAS - SQUAD LOADOUT (PRICES IN MARS ROCKS)'; text(S,(S.width-tw(t))//2,4,t,GOLD)
    text(S,4,18,'LOADOUT (A)',(206,200,170)); text(S,248,18,'ATTACHMENTS SUB-SCREEN',(206,200,170))
    S.paste(A,(4,28)); S.paste(B,(248,28))
    mockup_tag(S)
    up(S,3).save('/workspace/loadout_screen_mockup.png'); up(A,3).save('la.png'); up(B,3).save('lb.png')
