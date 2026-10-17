from sold import *
from gbalib import up
bg=Image.new('RGB',(400,110),(54,52,40))
specs=[spec(skin=5,hair='BLACK',face='STUBBLE',head='BOONIE',unif='DESERT TAN',camo='TIGER STRIPE',gear='RADIO',build='SLIM'),
 spec(skin=1,head='HELMET',camo='WOODLAND',gear='PLATE'),spec(skin=6,head='BERET',gear='WEBBING',unif='OLIVE DRAB'),
 spec(skin=3,head='CAP',camo='DIGITAL',unif='URBAN GREY',hair='RED',face='LONG')]
x=24
for s in specs:
    place(bg,sprite(s,1.0),x,100,(30,30,22)); x+=44
for v in 'FSB':
    place(bg,sprite(specs[0],1.5,v),x+20,100,(30,30,22)); x+=60
up(bg,3).save('t1.png')
