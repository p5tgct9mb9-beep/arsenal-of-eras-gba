from hd import *
from jet import draw_jet
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,RED,SHADOW
O=OLIVE
im=bands(240,160,[(0,160,O[1],O[1],'solid'),
  (14,18,O[0],O[2],'checker'),(18,26,O[2],O[2],'solid'),(26,30,O[2],O[3],'checker'),(30,38,O[3],O[3],'solid'),(38,42,O[3],O[4],'checker'),
  (42,80,O[4],O[4],'solid'),(80,84,O[4],O[3],'checker'),(84,92,O[3],O[2],'checker'),(92,96,O[2],O[0],'checker')])
d=ImageDraw.Draw(im)
# header
d.rectangle((0,0,239,13),fill=O[0]); d.line((0,13,239,13),fill=O[5])
text(im,4,3,'F-16C FIGHTING FALCON',GOLD)
m=draw_jet(im,30,56,0.9,0)
shadow(im,m,14,0.12)
# museum plaque tag on the display
d.rectangle((4,82,58,92),fill=O[0]); d.rectangle((5,83,57,91),fill=O[2])
text(im,7,84,'USA 1978',(196,190,150))
# stats panel
d.rectangle((0,96,239,159),fill=O[0])
d.rectangle((2,98,237,157),fill=O[5]); d.rectangle((3,99,236,156),fill=(20,20,16))
d.line((119,101,119,154),fill=O[3])
L=(150,148,118); V=(222,220,206)
left=[('SPEED','MACH 2.05'),('CEIL','50,000FT'),('THRUST','29,500LBF'),('ENGINE','F110-GE-129'),('LENGTH','15.06M'),('SPAN','9.96M')]
right=[('G-LIM','+9.0G'),('GUN','M61A1 20MM'),('AMMO','511 RDS'),('EMPTY','8,570KG'),('MAX TO','19,187KG'),('RADIUS','546KM')]
for i,((a,b),(c,e)) in enumerate(zip(left,right)):
    y=102+i*9
    text(im,6,y,a,L); text(im,48,y,b,V)
    text(im,123,y,c,L); text(im,165,y,e,V)
mockup_tag(im,x=196,y=1)
up(im).save('/workspace/realism_mockup_hangar.png'); up(im).save('v_hg1.png')
