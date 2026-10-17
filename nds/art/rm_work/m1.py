from gbalib import *
base=native(SHOTS+'/v12_settings_default.png')
im=base.copy()
d=ImageDraw.Draw(im)
d.rectangle((12,26,227,147),fill=NAVY)   # clear old rows, keep title + frame
# rows (label gray, value white) - selected row gold
def bar(x,y,n,col):
    d.rectangle((x,y,x+91,y+5),fill=(41,41,66)); d.rectangle((x,y,x+int(91*n/10),y+5),fill=col)
rows=[('MUSIC VOLUME',None),('SFX VOLUME',None),('BOT DIFFICULTY','REGULAR'),('REALISM','<REALISTIC>')]
y=27
for i,(l,v) in enumerate(rows):
    sel = l=='REALISM'
    text(im,14,y,l,GOLD if sel else GRAY)
    if i==0: bar(110,y+1,8,BLUE); text(im,208,y,'8',WHITE)
    elif i==1: bar(110,y+1,10,GREEN); text(im,208,y,'10',WHITE)
    else: text(im,110 if not sel else 104,y,v,WHITE)
    y+=10
# sub panel with three levels
P0,P1=66,136
panel(im,10,P0,229,P1,fill=(16,24,66))
opts=[('ARCADE','FULL HUD, BIG HP, NO BULLET DROP',GREEN),
      ('STANDARD','DEFAULT: LIGHT DROP, NORMAL HUD',BLUE),
      ('REALISTIC','LOW HUD, BULLET DROP, G-LIMITS',RED)]
yy=P0+7
for i,(n,desc,c) in enumerate(opts):
    sel=i==2
    if sel:
        d.rectangle((15,yy-2,224,yy+18),fill=(33,66,140))
        d.rectangle((15,yy-2,16,yy+18),fill=(90,214,247))
        text(im,19,yy,'>',GOLD)
    # level pips (1-3) on right like a difficulty meter
    for k in range(3):
        px=196+k*9
        d.rectangle((px,yy+1,px+6,yy+5),fill=SHADOW)
        d.rectangle((px,yy+1,px+5,yy+4),fill=c if k<=i else (41,41,66))
    text(im,26,yy,n,GOLD if sel else WHITE)
    text(im,26,yy+9,desc,(206,214,231) if sel else GRAY)
    yy+=21
text(im,(240-tw('+DAMAGE, LIMITED AMMO, ARMOR ANGLES'))//2,140,'+DAMAGE, LIMITED AMMO, ARMOR ANGLES',GRAY)
mockup_tag(im,x=196,y=8)
remap={(24,41,99):(40,39,31),(16,24,66):(23,23,18),(33,66,140):(78,74,50),(41,41,66):(30,30,24),(24,33,90):(40,39,31),(41,90,206):(120,112,70),(123,222,255):(220,210,150),(206,255,255):(255,245,200),(90,214,247):(255,214,66)}
px=im.load()
for y in range(160):
    for x in range(240):
        c=px[x,y]
        if c in remap: px[x,y]=remap[c]
        elif y<4 or y>155 or x<4 or x>235: px[x,y]=(10,10,8) if (x+y)&1 else (23,23,18)
up(im).save('/workspace/realism_mockup_settings.png'); up(im).save('v_set3.png')
