from hd import *
from jet import draw_jet
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,RED,GREEN,SHADOW
SKY=[(10,10,8),(28,30,28),(44,48,44),(60,66,60),(78,84,74),(98,102,88)]
bg=bands(240,160,[(0,160,SKY[3],SKY[3],'solid'),
 (0,10,SKY[0],SKY[0],'solid'),(10,16,SKY[0],SKY[1],'checker'),(16,26,SKY[1],SKY[1],'solid'),(26,32,SKY[1],SKY[2],'checker'),
 (32,48,SKY[2],SKY[2],'solid'),(48,54,SKY[2],SKY[3],'checker'),(54,92,SKY[3],SKY[3],'solid'),(92,98,SKY[3],SKY[4],'checker'),
 (98,118,SKY[4],SKY[4],'solid'),(118,122,SKY[4],SKY[5],'checker'),(122,130,SKY[5],SKY[5],'solid'),
 # distant cloud deck / ground haze
 (130,134,SKY[5],OLIVE[4],'checker'),(134,142,OLIVE[4],OLIVE[4],'solid'),(142,148,OLIVE[4],OLIVE[3],'checker'),(148,160,OLIVE[3],OLIVE[2],'checker')])
d=ImageDraw.Draw(bg)
# speed streaks
for (x,y,l) in [(8,60,18),(30,104,26),(196,44,22),(204,120,16),(4,86,10),(214,88,14)]:
    d.line((x,y,x+l,y),fill=SKY[4])
XF=Xf(40,60,0.85,-16)
m=draw_jet(bg,40,60,0.85,-16)
# wingtip vapour (high AOA)
for tip in [(146,19),(190,12)]:
    X,Y=XF.f(*tip)
    for k in range(1,9):
        if k%2: d.point((round(X+k*3.2),round(Y+k*0.9)),fill=(200,204,196) if k<5 else (140,146,138))
P=bg.load()
def hudbox(x0,y0,x1,y1,edge=(122,128,110)):
    d.rectangle((x0,y0,x1,y1),fill=SHADOW); d.rectangle((x0+1,y0+1,x1-1,y1-1),fill=edge); d.rectangle((x0+2,y0+2,x1-2,y1-2),fill=(18,20,16))
# --- G meter (left)
hudbox(3,34,25,116)
text(bg,6,37,'G',GRAY)
gx0,gy0,gy1=10,48,110
for g in range(0,10):
    y=gy1-int((gy1-gy0)*g/9)
    d.line((gx0+7,y,gx0+9,y),fill=(150,150,140))
# bar fill to 7.5
lvl=gy1-int((gy1-gy0)*7.5/9)
for y in range(lvl,gy1+1):
    g=(gy1-y)/(gy1-gy0)*9
    c=(107,231,123) if g<6 else (255,214,66) if g<8 else RED
    d.line((gx0,y,gx0+5,y),fill=c)
d.rectangle((gx0,gy0,gx0+5,lvl-1),fill=(40,44,38))
d.line((gx0-1,gy1-int((gy1-gy0)*9/9),gx0+9,gy1-int((gy1-gy0)*9/9)),fill=RED)  # 9G limit
text(bg,3,119,'7.5G',(255,214,66))
# --- stall warning (top centre, flashing frame)
hudbox(64,4,176,28,edge=(214,33,57))
d.rectangle((66,6,174,26),fill=(120,16,24))
t='! STALL !'; text(bg,(240-tw(t))//2,8,t,WHITE)
t='AOA 26  NOSE DOWN'; text(bg,(240-tw(t))//2,17,t,(255,200,190))
# --- energy / speed (right)
hudbox(194,34,237,74)
text(bg,198,37,'SPD',GRAY)
text(bg,198,46,'142KT',WHITE)
d.polygon([(229,38),(233,38),(231,41)],fill=RED)   # falling
text(bg,198,56,'ENRGY',GRAY)
for i in range(4):
    d.rectangle((198+i*9,66,198+i*9+6,69),fill=(255,214,66) if i<2 else (48,52,44))
text(bg,190,78,'8400FT',GRAY)
# --- fuel gauge (bottom right)
hudbox(150,128,236,156)
text(bg,154,131,'FUEL',GRAY); text(bg,184,131,'2100LB',WHITE)
d.rectangle((154,141,231,146),fill=(40,44,38))
d.rectangle((154,141,154+int(77*0.31),146),fill=(255,214,66))
d.line((154+int(77*0.25),139,154+int(77*0.25),148),fill=RED)
text(bg,154,149,'BINGO 1.6T',(200,160,150)) if False else None
text(bg,201,149-0,'',GRAY)
mockup_tag(bg,x=196,y=2)
text(bg,4,148,'F-16C  REALISTIC',(150,156,140))
up(bg).save('/workspace/realism_mockup_air.png'); up(bg).save('v_air1.png')
