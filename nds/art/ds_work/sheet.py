from dslib import *
items=[('menu','1. MAIN MENU','TOP: TITLE, EARTH, ISS, STARSHIP','BOTTOM: 12 TOUCH MODES, ROCKS'),
       ('infantry','2. INFANTRY 6V6','TOP: 3/4 BATTLE, 30PX SOLDIERS','BOTTOM: MAP, M4A1, AMMO, BUTTONS'),
       ('dogfight','3. DOGFIGHT','TOP: CHASE VIEW F-16C VS MIG-29','BOTTOM: RADAR, G, SPD, ALT, STORES'),
       ('loadout','4. GUNSMITH','TOP: M4A1 WITH 5 ATTACHMENTS','BOTTOM: TOUCH GRID, PRICES, STATS'),
       ('armor','5. ARMOR','TOP: M1A2 VS T-90 ENGAGEMENT','BOTTOM: THERMAL, AMMO, DAMAGE, MAP')]
pairs=[Image.open(f'n_{n}_pair.png') for n,*_ in items]
pw,ph=pairs[0].size; g=8
W=5*pw+6*g; H=22+14+ph+30
S=Image.new('RGB',(W,H),(16,16,12))
t='ARSENAL OF ERAS - NINTENDO DS (DSI) PORT MOCKUPS: TOP SCREEN = GAMEPLAY, BOTTOM = TOUCH'
title_y=6; text(S,(W-tw(t))//2,title_y,t,GOLD); tag(S)
text(S,4,title_y,'256X192 X2, NATIVE PIXELS, 3X NEAREST',(150,150,130))
for i,((n,cap,a,b),p) in enumerate(zip(items,pairs)):
    x=g+i*(pw+g); y=22
    text(S,x+4,y+2,cap,GOLD)
    S.paste(p,(x,y+14))
    text(S,x+4,y+14+ph+3,a,(206,200,170)); text(S,x+4,y+14+ph+13,b,(206,200,170))
up(S,3).save('/workspace/ds_mockup_sheet.png'); print(S.size)
