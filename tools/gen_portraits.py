#!/usr/bin/env python3
# v1.3 anime-style briefing portraits (original procedural pixel art, DBZ-GBA-inspired cel shading + bold outlines).
# 7 headgear kinds x 2 genders x 3 skin tones, 48x48, 15-bit with bit15 = transparent. -> src/portraits.inc
import os
from PIL import Image,ImageDraw
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
W=48;OUT=(14,10,18)
SKIN=[((246,206,170),(214,160,122)),((214,160,112),(170,116,76)),((150,100,66),(108,68,44))]
HAIR=[(46,32,24),(120,80,40),(30,30,36),(196,150,70),(90,40,28)]
KINDS=['ww2','tanker','pilot','naval','cold','modern','future']
HC={'ww2':((96,108,62),(66,76,42)),'tanker':((70,62,50),(46,40,32)),'pilot':((120,90,50),(86,62,34)),'naval':((236,236,240),(180,184,196)),
    'cold':((104,112,72),(72,78,48)),'modern':((150,136,100),(110,98,70)),'future':((70,90,120),(44,58,84))}
UNI={'ww2':((110,116,70),(78,84,48)),'tanker':((92,96,64),(62,66,42)),'pilot':((126,92,56),(90,64,38)),'naval':((40,52,96),(26,34,66)),
     'cold':((96,108,70),(66,76,48)),'modern':((168,150,110),(126,110,78)),'future':((56,64,80),(36,42,56))}
def portrait(kind,fem,sk,hair):
    im=Image.new('RGBA',(W,W),(0,0,0,0));d=ImageDraw.Draw(im)
    s0,s1=SKIN[sk];h0=HAIR[hair];u0,u1=UNI[kind];c0,c1=HC[kind]
    # shoulders / torso
    d.polygon([(4,47),(8,36),(17,32),(31,32),(40,36),(44,47)],fill=u0,outline=OUT)
    d.polygon([(24,33),(31,32),(40,36),(44,47),(26,47)],fill=u1)
    d.line([(4,47),(8,36),(17,32),(31,32),(40,36),(44,47)],fill=OUT)
    d.polygon([(19,32),(24,38),(29,32)],fill=s1,outline=OUT)                  # collar V
    if kind=='naval':d.polygon([(20,33),(24,40),(28,33)],fill=(250,250,250),outline=OUT)
    if kind in ('modern','cold','future'):d.rectangle([10,38,15,42],fill=u1,outline=OUT);d.rectangle([33,38,38,42],fill=u1,outline=OUT)  # pouches/straps
    # neck + face (angular anime jaw)
    d.rectangle([20,27,28,33],fill=s1,outline=OUT)
    face=[(14,13),(34,13),(35,22),(31,29),(24,32),(17,29),(13,22)]
    d.polygon(face,fill=s0,outline=OUT)
    d.polygon([(26,13),(34,13),(35,22),(31,29),(24,32),(27,24)],fill=s1)    # cel shadow (light from the left)
    d.line(face+[face[0]],fill=OUT)
    # hair (spiky fringe; ponytail/long strands for female)
    if fem:
        d.polygon([(12,15),(10,28),(14,30),(15,18)],fill=h0,outline=OUT);d.polygon([(36,15),(38,28),(34,30),(33,18)],fill=h0,outline=OUT)
    for i,(x,y) in enumerate([(14,18),(18,20),(22,19),(26,20),(30,19),(34,18)]):
        d.polygon([(x-3,12),(x+3,12),(x+(1 if i%2 else -1),y)],fill=h0,outline=OUT)
    # eyes: thick upper lash, coloured iris, highlight; angular brows
    for ex,fl in ((19,-1),(29,1)):
        d.polygon([(ex-3,21),(ex+3,21),(ex+3,24),(ex-3,24)],fill=(250,250,250))
        d.rectangle([ex-1,21,ex+1,24],fill=(60,90,150) if hair%2 else (90,60,40));d.point((ex,22),fill=(255,255,255))
        d.line([(ex-4,20),(ex+3,20)],fill=OUT,width=1);d.line([(ex-3,21),(ex+3,21)],fill=OUT)
        bw=[(ex-4,17+(0 if fl<0 else 1)),(ex+3,18+(1 if fl<0 else 0))] if not fem else [(ex-3,17),(ex+3,17)]
        d.line(bw,fill=OUT,width=2)
    d.line([(24,23),(25,26),(24,26)],fill=(s1[0]-30,s1[1]-30,s1[2]-30))              # nose
    d.line([(21,29),(27,29)],fill=OUT) if not fem else d.line([(22,29),(26,29)],fill=(170,70,70))
    # headgear
    if kind in ('ww2','cold','modern'):
        d.chord([10,3,38,23],180,360,fill=c0,outline=OUT);d.rectangle([9,12,39,14],fill=c1,outline=OUT)
        d.chord([24,4,37,22],250,360,fill=c1)
        d.line([(15,6),(20,4)],fill=(min(255,c0[0]+60),min(255,c0[1]+60),min(255,c0[2]+60)),width=1)   # highlight
        if kind=='cold':
            for x in range(13,36,4):d.point((x,8),fill=c1)
        if kind=='modern':d.rectangle([21,1,27,5],fill=(40,40,44),outline=OUT);d.rectangle([22,-1,26,1],fill=(70,70,76))  # NVG mount
    elif kind=='tanker':
        d.ellipse([11,4,37,18],fill=c0,outline=OUT);d.rectangle([12,11,36,14],fill=c1,outline=OUT)
        d.ellipse([15,6,22,11],fill=(60,60,64),outline=OUT);d.ellipse([26,6,33,11],fill=(60,60,64),outline=OUT)  # goggles up
    elif kind=='pilot':
        d.chord([10,2,38,26],180,360,fill=c0,outline=OUT);d.rectangle([9,13,39,15],fill=c1,outline=OUT)
        d.ellipse([14,7,22,13],fill=(150,200,230),outline=OUT);d.ellipse([26,7,34,13],fill=(150,200,230),outline=OUT)
        d.rectangle([9,15,12,26],fill=c1,outline=OUT);d.rectangle([36,15,39,26],fill=c1,outline=OUT)
    elif kind=='naval':
        d.ellipse([10,4,38,13],fill=c0,outline=OUT);d.rectangle([12,10,36,14],fill=(30,30,40),outline=OUT);d.polygon([(12,14),(36,14),(31,17),(17,17)],fill=(24,24,30))
        d.ellipse([21,9,27,13],fill=(250,206,70),outline=OUT)
    elif kind=='future':
        d.chord([9,2,39,26],180,360,fill=c0,outline=OUT);d.rectangle([12,17,36,22],fill=(90,230,255),outline=OUT);d.line([(14,18),(22,18)],fill=(220,255,255))
    return im
def to15(im):
    out=[]
    for y in range(W):
        for x in range(W):
            r,g,b,a=im.getpixel((x,y))
            out.append(0x8000 if a<128 else (r>>3)|((g>>3)<<5)|((b>>3)<<10))
    return out
data=[];sheet=Image.new('RGBA',(W*len(KINDS),W*6),(30,40,80,255))
for ki,k in enumerate(KINDS):
    for fem in (0,1):
        for sk in range(3):
            im=portrait(k,fem,sk,(ki+sk+fem*2)%len(HAIR));data+=to15(im);sheet.alpha_composite(im,(ki*W,(fem*3+sk)*W))
sheet.resize((sheet.width*3,sheet.height*3),Image.NEAREST).save(os.path.join(ROOT,'shots','v13','v13_style_portraits_sheet.png'))
o=["// generated by tools/gen_portraits.py: %d anime-style briefing portraits 48x48 (kind*6+fem*3+skin), bit15=transparent"%(len(data)//(W*W)),
   "enum{PK_WW2,PK_TANKER,PK_PILOT,PK_NAVAL,PK_COLD,PK_MODERN,PK_FUTURE};",
   "static const u16 PORTR[%d]={%s};"%(len(data),",".join(map(str,data)))]
import math
bm=[]
for y in range(48):
    for x in range(48):
        dx,dy=x-24,y-28;r2=dx*dx+dy*dy;a=int((math.atan2(dx,-dy)/(2*math.pi))*256)&255
        bm.append(1 if r2<90 else ((((a+(r2>>7))>>3)&1)))
o.append("static const u8 BURST[2304]={%s};"%",".join(map(str,bm)))
open(os.path.join(ROOT,'src','portraits.inc'),'w').write("\n".join(o)+"\n");print("portraits",len(data)//(W*W))
