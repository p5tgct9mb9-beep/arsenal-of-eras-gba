from ac import *
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,SHADOW,GLYPH
GLYPH['~']=['.....','.....','.##.#','#..#.','.....','.....','.....']
O=OLIVE
TW,TH,PAD=120,106,4
W=4*TW+5*PAD
H=16+10+TH+12+TH+16
S=Image.new('RGB',(W,H),(16,16,12)); d=ImageDraw.Draw(S)
def cross(im,X,Y,mask):
    P=im.load()
    blk=set()
    for i in range(-4,4):
        for j in (-1,0): blk.add((X+i,Y+j)); blk.add((X+j,Y+i))
    for (x,y) in blk:
        for dx in (-1,0,1):
            for dy in (-1,0,1):
                q=(x+dx,y+dy)
                if q not in blk and abs(dx)+abs(dy)==1: P[q]=(232,232,226)
    for q in blk: P[q]=(14,14,14)
def prop(im,X,Y,R):
    P=im.load()
    for y in range(Y-R,Y+R+1):
        t=abs(y-Y)/R; w=1 if t>0.7 else 2
        for x in range(X-w,X+1):
            if (x+y)&1: P[x,y]=(176,176,168) if t<0.85 else (120,120,112)
    for y in (Y-R,Y+R): P[X-1,y]=(60,60,56)
def tile(x0,y0,name,fn,facts,sc,ox,oy,sel=False):
    T=bands(TW,58,[(0,58,O[3],O[3],'solid'),(0,3,O[1],O[2],'checker'),(3,10,O[2],O[2],'solid'),(10,13,O[2],O[3],'checker'),
        (13,40,O[3],O[3],'solid'),(40,44,O[3],O[4],'checker'),(44,50,O[4],O[4],'solid'),(50,54,O[4],O[2],'checker'),(54,58,O[2],O[1],'checker')])
    a=fn(); xf=Xf(ox,oy,sc)
    mask,ids=render2(T,a['parts'],xf,lines=a['lines'],dots=a['dots'])
    shadow(T,mask,4,0.10,col=O[1])
    for m in a['marks']:
        X,Y=xf.f(m[1],m[2]); X,Y=int(round(X)),int(round(Y))
        if m[0]=='cross': cross(T,X,Y,mask)
        else: prop(T,X,Y,int(m[3]*sc))
    edge=GOLD if sel else O[5]
    d.rectangle((x0,y0,x0+TW-1,y0+TH-1),fill=O[0]); d.rectangle((x0+1,y0+1,x0+TW-2,y0+TH-2),fill=edge)
    d.rectangle((x0+2,y0+2,x0+TW-3,y0+TH-3),fill=(20,20,16))
    d.rectangle((x0+2,y0+2,x0+TW-3,y0+12),fill=O[0])
    text(S,x0+5,y0+4,name,GOLD if sel else (222,220,206))
    S.paste(T.crop((2,0,TW-2,58)),(x0+2,y0+13))
    d.line((x0+2,y0+71,x0+TW-3,y0+71),fill=O[5] if not sel else GOLD)
    for i,f in enumerate(facts):
        text(S,x0+5,y0+75+i*9,f,(196,190,150) if i else (222,220,206))
    if sel:
        text(S,x0+TW-12,y0+4,'<',GOLD)
t='WORLD FLIGHT - WW2 GERMANY'
d.rectangle((0,0,W-1,13),fill=O[0]); d.line((0,13,W-1,13),fill=O[5])
text(S,6,3,t,GOLD)
mockup_tag(S,x=W-44,y=1)
row1=[('BF 109 G',bf109,['MOST-BUILT FIGHTER','IN HISTORY,','~34,000 BF 109S'],1.08,4,26),
      ('FW 190 A',fw190,['BMW 801 RADIAL','ENGINE. FIRST','FLEW IN 1939'],1.08,4,27),
      ('JU 87 STUKA',ju87,['DIVE BOMBER. SIREN',"'JERICHO TRUMPET'",'ON EARLY MODELS'],1.06,4,24),
      ('JU 88',ju88,['BOMBER, NIGHT','FIGHTER AND RECON.','~15,000 BUILT'],1.06,2,28)]
row2=[('ME 262',me262,['FIRST OPERATIONAL','JET FIGHTER.','TOP SPEED ~870KM/H'],1.05,3,27),
      ('ME 163 KOMET',me163,['ROCKET-POWERED','INTERCEPTOR.','TOP SPEED ~960KM/H'],0.92,12,33),
      ('HO 229',ho229,['JET FLYING WING','PROTOTYPE BY THE','HORTEN BROTHERS'],1.45,3,29),
      ('AR 234 BLITZ',ar234,['FIRST OPERATIONAL','JET BOMBER.','TOP SPEED ~740KM/H'],1.06,3,27)]
y=16
text(S,PAD,y,'PROPELLER ERA',(206,200,170)); y+=10
for i,r in enumerate(row1): tile(PAD+i*(TW+PAD),y,r[0],r[1],r[2],r[3],r[4],r[5],sel=(i==0))
y+=TH+3
d.rectangle((PAD,y,PAD+tw('WUNDERWAFFEN')+3,y+8),fill=(60,16,16))
text(S,PAD+2,y+1,'WUNDERWAFFEN',(239,90,70)); text(S,PAD+tw('WUNDERWAFFEN')+10,y+1,'LATE-WAR JETS, ROCKETS + PROTOTYPES',(150,148,118)); y+=12
for i,r in enumerate(row2): tile(PAD+i*(TW+PAD),y,r[0],r[1],r[2],r[3],r[4],r[5])
y+=TH+2
text(S,PAD,y+2,'A:SELECT  B:BACK  L/R:NATION  +:BROWSE',(150,148,118))
print(S.size,y)
up(S,3).save('/workspace/ww2_german_preview.png'); up(S,3).save('v4.png')
