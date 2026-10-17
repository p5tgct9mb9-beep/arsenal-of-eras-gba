from gbalib import *
import math
base=native(SHOTS+'/v13/v13_fps_0_play.png')
CX,CY=120,68   # crosshair centre
def soldier(im,x,y):
    """tiny distant OPFOR rifleman, feet at (x,y)"""
    d=ImageDraw.Draw(im)
    O=(16,16,24); U=(140,57,49); U2=(99,41,33); S=(222,165,123); H=(74,82,49)
    pts=[]
    d.rectangle((x-2,y-14,x+2,y-12),fill=H)        # helmet
    d.point((x-3,y-12),fill=H); d.point((x+3,y-12),fill=H)
    d.rectangle((x-1,y-11,x+1,y-10),fill=S)        # face
    d.rectangle((x-3,y-9,x+3,y-5),fill=U)          # torso
    d.rectangle((x-3,y-9,x-3,y-5),fill=U2)
    d.rectangle((x+4,y-8,x+8,y-8),fill=O)          # rifle
    d.point((x+3,y-7),fill=S)
    d.rectangle((x-2,y-4,x-1,y),fill=U2); d.rectangle((x+1,y-4,x+2,y),fill=U2)  # legs
    d.point((x-2,y),fill=O); d.point((x+2,y),fill=O)
def label(im,x,y,s,col):
    w=tw(s)+5
    d=ImageDraw.Draw(im); d.rectangle((x,y,x+w,y+10),fill=SHADOW); d.rectangle((x+1,y+1,x+w-1,y+9),fill=col)
    text(im,x+3,y+2,s,WHITE,shadow=None)

# ---------- ARCADE ----------
A=base.copy(); d=ImageDraw.Draw(A)
soldier(A,121,83)
# hitmarker (white X around crosshair, the game's crosshair stays)
for k in (3,4,5):
    for sx,sy in ((1,1),(-1,1),(1,-1),(-1,-1)):
        d.point((CX+sx*k,CY+sy*k),fill=WHITE)
for k in (4,):
    for sx,sy in ((1,1),(-1,1),(1,-1),(-1,-1)):
        d.point((CX+sx*k+1,CY+sy*k),fill=WHITE)
# damage pop + enemy hp bar
text(A,128,54,'-34',(255,90,74))
d.rectangle((113,64-14-4,129,64-14-2),fill=SHADOW); d.rectangle((114,64-14-3,124,64-14-3),fill=(239,66,57))
text(A,96,40,'HIT +25',GOLD)
# minimap top-right
mx,my,ms=204,4,32
d.rectangle((mx,my,mx+ms,my+ms),fill=SHADOW); d.rectangle((mx+1,my+1,mx+ms-1,my+ms-1),fill=(239,198,90))
d.rectangle((mx+2,my+2,mx+ms-2,my+ms-2),fill=(148,99,41)); d.rectangle((mx+3,my+3,mx+ms-2,my+ms-2),fill=(24,33,90))
for (px,py,c) in [(16,22,(90,214,247)),(18,23,(90,214,247)),(14,25,(90,214,247)),(16,10,(239,66,57)),(22,8,(239,66,57)),(25,14,(239,66,57)),(8,6,(239,66,57)),(27,26,GOLD)]:
    d.rectangle((mx+px,my+py,mx+px+1,my+py+1),fill=c)
d.line((mx+16,my+22,mx+16,my+17),fill=WHITE)   # view dir
# kill-feed style objective line already says ENEMY 6; add ammo tick bar
label(A,3,3,'ARCADE',(57,148,74))

# ---------- REALISTIC ----------
R=base.copy(); px=R.load()
# remove bottom HUD bar: extend the scene downward (gun/arms continue, ground keeps lightening)
ground=[(165,140,107),(173,148,107),(173,148,115),(181,156,115),(189,156,115),(189,165,123)]
gref=px[20,135]
for y in range(136,160):
    for x in range(240):
        c=px[x,135]
        px[x,y]= ground[min(5,(y-136)//4)] if c==gref else c
soldier(R,121,83)
d=ImageDraw.Draw(R)
# erase the arcade crosshair (restore wall pixels from a clean column offset)
src=native(SHOTS+'/v13/v13_fps_0_play.png')
for y in range(CY-7,CY+8):
    for x in range(CX-7,CX+8):
        c=px[x,y]
        if c[1]>180 and c[0]<120:  # green crosshair pixel
            px[x,y]=src.getpixel((x+32,y))
soldier(R,121,83)
# bullet-drop arc: from the muzzle, lofted, falling onto the target (dotted tracer)
P0=(128,79); P1=(124,22); P2=(121,76)
pts=[]
for i in range(0,41):
    t=i/40; x=(1-t)**2*P0[0]+2*(1-t)*t*P1[0]+t*t*P2[0]; y=(1-t)**2*P0[1]+2*(1-t)*t*P1[1]+t*t*P2[1]
    pts.append((round(x),round(y)))
for i,(x,y) in enumerate(pts):
    if i%2==0 and i>2:
        d.point((x,y),fill=(255,239,165)); d.point((x+1,y),fill=(239,148,33))
# holdover post: aim point dot above the head + stadia ticks
hx,hy=121,50
d.rectangle((hx-1,hy-1,hx+1,hy+1),fill=SHADOW); d.point((hx,hy),fill=(255,90,74))
for i,(yy,lab) in enumerate([(56,'1'),(62,'2'),(50,'3')]):
    pass
for yy in (54,58,62):
    d.line((hx-2,yy,hx-1,yy),fill=(240,240,240)); d.line((hx+1,yy,hx+2,yy),fill=(240,240,240))
# range note box
box(R,138,30,226,52,fill=(16,16,24),edge=(148,99,41))
text(R,142,33,'300M:',GOLD); text(R,174,33,'AIM HIGH',WHITE)
text(R,142,42,'DROP -1.1M',GRAY)
d.line((137,44,126,49),fill=(255,239,165))
# minimal ammo feel, bottom right, no numbers
text(R,166,147,'MAG: HEAVY',(206,214,231))
for i in range(3):
    x=168+i*7
    d.rectangle((x,138,x+4,144),fill=SHADOW); d.rectangle((x,138,x+3,143),fill=(140,140,148) if i<2 else (66,74,90))
text(R,190,138,'+2',GRAY)
# red damage vignette (blended, 3 bands like GBA alpha blend)
V=R.copy(); vp=V.load()
for y in range(160):
    for x in range(240):
        e=min(x,239-x,y*1.5,(159-y)*1.5)
        a=0.62 if e<6 else 0.42 if e<12 else 0.24 if e<20 else 0.10 if e<30 else 0
        if a:
            r,g,b=vp[x,y]; vp[x,y]=(int(r*(1-a)+170*a),int(g*(1-a)+16*a),int(b*(1-a)+16*a))
R=V
label(R,3,3,'REALISTIC',(181,41,41))
mockup_tag(R)
out=Image.new('RGB',(240*2+4,160),(8,8,8))
out.paste(A,(0,0)); out.paste(R,(244,0))
up(out).save('/workspace/realism_mockup_infantry.png')
up(out).save('/workspace/rm_work/v_inf1.png')
