from hd import *
from tank import draw_tank, AR
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,RED,GREEN,SHADOW
D=[(10,10,8),(30,28,22),(52,46,34),(74,64,46),(98,84,58),(120,102,70),(140,120,84)]
im=bands(240,160,[(0,160,D[3],D[3],'solid'),
  (0,6,D[0],D[0],'solid'),(6,12,D[0],D[1],'checker'),(12,22,D[1],D[1],'solid'),(22,28,D[1],D[2],'checker'),
  (28,44,D[2],D[2],'solid'),(44,50,D[2],D[3],'checker'),(50,72,D[3],D[3],'solid'),
  # distant ridge band
  (72,76,D[3],D[2],'checker'),(76,86,D[2],D[2],'solid'),(86,90,D[2],D[4],'checker'),
  # ground
  (90,104,D[4],D[4],'solid'),(104,108,D[4],D[5],'checker'),(108,124,D[5],D[5],'solid'),(124,128,D[5],D[4],'checker'),
  (128,140,D[4],D[3],'checker'),(140,160,D[3],D[2],'checker')])
d=ImageDraw.Draw(im)
m,xf=draw_tank(im,76,120,1.1,0)
shadow(im,m,5,0.14)
def P(x,y): return tuple(round(v) for v in xf.f(x,y))
def callbox(x0,y0,lines,edge):
    w=max(tw(s) for s,_ in lines)+7; h=len(lines)*9+4
    d.rectangle((x0,y0,x0+w,y0+h),fill=SHADOW); d.rectangle((x0+1,y0+1,x0+w-1,y0+h-1),fill=edge); d.rectangle((x0+2,y0+2,x0+w-2,y0+h-2),fill=(16,16,12))
    for i,(s,c) in enumerate(lines): text(im,x0+4,y0+3+i*9,s,c)
    return (x0,y0,x0+w,y0+h)
# --- SIDE HIT (penetration) on hull side skirt
hx,hy=P(84,-20)
for (dx,dy) in [(0,0),(1,0),(0,1),(1,1),(-1,0),(0,-1),(1,-1),(-1,1)]: im.putpixel((hx+dx,hy+dy),INK)
for (dx,dy) in [(-3,-3),(4,-3),(-3,4),(4,4),(0,-4),(5,0),(-4,1),(1,5)]: im.putpixel((hx+dx,hy+dy),(255,214,66))
for (dx,dy) in [(-2,-2),(3,-2),(-2,3),(3,3)]: im.putpixel((hx+dx,hy+dy),(255,150,40))
for (dx,dy,c) in [(-1,-1,(255,214,66)),(2,-1,(255,150,40)),(-1,2,(255,150,40)),(2,2,(255,214,66)),(3,0,(255,239,165)),(-2,1,(255,239,165))]:
    im.putpixel((hx+dx,hy+dy),c)
# incoming tracer from the right/rear-quarter
b=callbox(240-tw('SIDE HIT 80MM VS 45MM')-10,128,[('SIDE HIT 80MM VS 45MM',WHITE),('PENETRATED',(107,231,123))],(214,33,57))
d.line((hx+1,hy+3,hx+1,b[1]),fill=(255,214,66))
# --- FRONT RICOCHET on upper glacis
fx,fy=P(0,-29)
for k in range(4,40,2): d.point((fx-k,fy+k//5),fill=(200,150,60))        # incoming from front-left
for k in range(2,24,2): d.point((fx-k//2,fy-k),fill=(255,239,165))        # deflected upward
for (dx,dy,c) in [(0,0,WHITE),(-1,0,(255,214,66)),(0,-1,(255,214,66)),(1,1,(255,150,40))]:
    im.putpixel((fx+dx,fy+dy),c)
b2=callbox(4,16,[('FRONT 30o:',WHITE),('RICOCHET',(255,214,66))],(148,99,41))
d.line((b2[0]+24,b2[3],fx-7,fy+2),fill=(255,214,66))
# --- WEAK SPOT: turret ring brackets
wx0,wy0=P(44,-39); wx1,wy1=P(90,-32)
C=(239,66,57)
for (x,y,sx,sy) in [(wx0-2,wy0-2,1,1),(wx1+2,wy0-2,-1,1),(wx0-2,wy1+2,1,-1),(wx1+2,wy1+2,-1,-1)]:
    d.line((x,y,x+4*sx,y),fill=C); d.line((x,y,x,y+3*sy),fill=C)
tx=(wx0+wx1)//2
b3=callbox(150,16,[('WEAK SPOT',C),('TURRET RING',(206,190,170))],(120,30,30))
d.line((b3[0]+12,b3[3],wx1+2,wy0-3),fill=C)
# --- minimal REALISTIC HUD: ammo by type, range, no minimap/score
d.rectangle((0,0,239,10),fill=(10,10,8))
text(im,3,2,'APFSDS 9',WHITE); text(im,57,2,'HEAT 3',GRAY); text(im,99,2,'RNG 1240M',(206,190,170))
mockup_tag(im,x=196,y=0)
text(im,4,150,'T-72 TYPE',(150,140,110))
up(im).save('/workspace/realism_mockup_tank.png'); up(im).save('v_tank1.png')
