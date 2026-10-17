from gbalib import *
def nat(p): im=Image.open(p).convert('RGB'); return im.resize((im.width//3,im.height//3),Image.NEAREST)
items=[('/workspace/realism_mockup_infantry.png','INFANTRY: ARCADE FULL HUD VS REALISTIC (DROP, MAG FEEL)'),
       ('/workspace/realism_mockup_settings.png','SETTINGS: REALISM LEVEL'),
       ('/workspace/realism_mockup_tank.png','TANK: PENETRATION + WEAK SPOT'),
       ('/workspace/realism_mockup_air.png','AIR: G, STALL, FUEL, KNOTS'),
       ('/workspace/realism_mockup_hangar.png','HANGAR: REAL-WORLD STATS')]
ims=[nat(p) for p,_ in items]
CW,CAP,PAD=484,12,4
H=16+3*(CAP+160+PAD)+2
S=Image.new('RGB',(CW+2*PAD,H),(16,16,12))
t='ARSENAL OF ERAS - REALISM SETTING MOCKUPS'; text(S,(S.width-tw(t))//2,4,t,GOLD)
def put(im,cap,x,y):
    text(S,x,y+2,cap,(206,200,170)); S.paste(im,(x,y+CAP))
y=16
put(ims[0],items[0][1],PAD,y); y+=CAP+160+PAD
put(ims[1],items[1][1],PAD,y); put(ims[2],items[2][1],PAD+244,y); y+=CAP+160+PAD
put(ims[3],items[3][1],PAD,y); put(ims[4],items[4][1],PAD+244,y)
up(S).save('/workspace/realism_mockup_sheet.png'); up(S).save('v_sheet.png'); print(S.size)
