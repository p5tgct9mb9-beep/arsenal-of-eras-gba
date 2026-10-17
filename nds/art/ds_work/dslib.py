"""DS mockup helpers: reuses the inf_work / rm_work / ww2_work pixel-art toolkit."""
import sys
for p in (__import__('os').path.join(__import__('os').path.dirname(__import__('os').path.abspath(__file__)),'..','inf_work'),__import__('os').path.join(__import__('os').path.dirname(__import__('os').path.abspath(__file__)),'..','rm_work'),__import__('os').path.join(__import__('os').path.dirname(__import__('os').path.abspath(__file__)),'..','ww2_work')):
    if p not in sys.path: sys.path.insert(0,p)
from ui import *          # bevel, slot, tab, rock, tiny, olive palette, weap (m4 etc), sold (sprite, spec...)
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,SHADOW,GLYPH,RED,GREEN,BLUE,box
import random, math
from loadout import portrait, card
SW,SH=256,192; GAP=12
CYAN=(90,214,247); NAVYF=(14,22,44); NAVYD=(8,12,26); NAVYL=(40,70,120); PANELN=(20,30,58)
HUDG=(120,255,140); HUDGD=(30,90,40)
def nbevel(im,x0,y0,x1,y1,fill=PANELN,edge=(58,120,170),sel=False):
    d=ImageDraw.Draw(im); d.rectangle((x0,y0,x1,y1),fill=(4,6,12))
    e=GOLD if sel else edge
    d.rectangle((x0+1,y0+1,x1-1,y1-1),outline=e)
    d.line((x0+2,y0+2,x1-2,y0+2),fill=mix(e,(255,255,255),0.25) if sel else (34,54,96))
    d.rectangle((x0+2,y0+3,x1-2,y1-2),fill=fill)
def fit(im,parts,lines,box_,maxsc=None,ang=0.0,flip=False,align='c',outline=INK,dots=()):
    """render shaded parts (polys or circles) scaled to fit box; returns xf in image coords"""
    xs=[];ys=[]
    for p in parts:
        if 'circle' in p: cx,cy,r=p['circle']; xs+=[cx-r,cx+r]; ys+=[cy-r,cy+r]
        else: xs+=[a for a,b in p['poly']]; ys+=[b for a,b in p['poly']]
    x0,y0,x1,y1=box_; bw=max(xs)-min(xs); bh=max(ys)-min(ys)
    sc=min((x1-x0-2)/bw,(y1-y0-2)/bh)
    if maxsc: sc=min(sc,maxsc)
    if flip:
        ox=x0+(x1-x0-bw*sc)/2+max(xs)*sc
    else: ox=x0+(x1-x0-bw*sc)/2-min(xs)*sc
    oy=y0+(y1-y0-bh*sc)/2-min(ys)*sc
    if align=='b': oy=y1-1-max(ys)*sc
    crop=im.crop((x0,y0,x1,y1)); xf=Xf(ox-x0,oy-y0,sc,ang,flip)
    render2(crop,parts,xf,lines=lines,outline=outline,dots=dots); im.paste(crop,(x0,y0))
    return Xf(ox,oy,sc,ang,flip)
def at(im,parts,lines,ox,oy,sc=1.0,ang=0.0,flip=False,outline=INK,dots=(),pad=None):
    """render parts with origin at ox,oy into a local crop (fast)"""
    W,H=im.size
    pts=[]
    tmp=Xf(ox,oy,sc,ang,flip)
    for p in parts:
        if 'circle' in p:
            cx,cy,r=p['circle']; X,Y=tmp.f(cx,cy); pts+=[(X-r*sc,Y-r*sc),(X+r*sc,Y+r*sc)]
        else: pts+=[tmp.f(x,y) for x,y in p['poly']]
    x0=max(0,int(min(a for a,b in pts))-2); y0=max(0,int(min(b for a,b in pts))-2)
    x1=min(W,int(max(a for a,b in pts))+3); y1=min(H,int(max(b for a,b in pts))+3)
    if x1<=x0 or y1<=y0: return None
    crop=im.crop((x0,y0,x1,y1)); xf=Xf(ox-x0,oy-y0,sc,ang,flip)
    m,ids=render2(crop,parts,xf,lines=lines,outline=outline,dots=dots); im.paste(crop,(x0,y0))
    full=Image.new('L',(W,H),0); full.paste(m,(x0,y0)); return full
def tag(im,x=None,y=None): mockup_tag(im,x,y)
def chip(im,x0,y0,x1,y1,fill=(10,12,8),edge=(70,76,50)):
    d=ImageDraw.Draw(im); d.rectangle((x0,y0,x1,y1),fill=SHADOW); d.rectangle((x0+1,y0+1,x1-1,y1-1),fill=edge); d.rectangle((x0+2,y0+2,x1-2,y1-2),fill=fill)
def ctext(im,cx,y,s,col=WHITE,**k): text(im,cx-tw(s)//2,y,s,col,**k)
def rtext(im,rx,y,s,col=WHITE,**k): text(im,rx-tw(s)+1,y,s,col,**k)
# ---------------- big title (glyphs at 3x with gradient + bevel + outline)
TITLE_RAMP=[(255,252,230),(255,240,170),(255,214,90),(250,182,52),(232,140,36),(196,96,28),(150,64,22)]
def title(im,y,s,scale=3,adv=16,space=9,ramp=TITLE_RAMP,shadow=(4,6,16),outline=(20,10,6)):
    W=sum(space if c==' ' else adv for c in s)-1; x=(im.width-W)//2
    m=Image.new('L',im.size,0); M=m.load(); col={}
    cx=x
    for c in s:
        if c==' ': cx+=space; continue
        g=GLYPH[c]
        for r,row in enumerate(g):
            for k,b in enumerate(row):
                if b=='#':
                    for dy in range(scale):
                        for dx in range(scale):
                            X,Y=cx+k*scale+dx,y+r*scale+dy; M[X,Y]=255; col[(X,Y)]=(r*scale+dy,dy)
        cx+=adv
    P=im.load(); H=7*scale
    # drop shadow + outline
    for (X,Y) in col:
        for dx,dy in ((2,2),(3,3),(2,3)):
            if (X+dx,Y+dy) not in col: P[X+dx,Y+dy]=shadow
    for (X,Y) in col:
        for dx in (-1,0,1):
            for dy in (-1,0,1):
                q=(X+dx,Y+dy)
                if q not in col and 0<=q[0]<im.width and 0<=q[1]<im.height: P[q]=outline
    for (X,Y),(r,dy) in col.items():
        t=r/(H-1); c=dith(t,X,Y,ramp)
        if dy==0 and (X,Y-1) not in col: c=ramp[0]
        P[X,Y]=c
def stars(im,n=140,seed=5,region=None):
    rnd=random.Random(seed); P=im.load(); W,H=im.size
    x0,y0,x1,y1=region or (0,0,W,H)
    for i in range(n):
        x=rnd.randrange(x0,x1); y=rnd.randrange(y0,y1); b=rnd.random()
        c=(255,255,255) if b>0.9 else (190,200,230) if b>0.6 else (110,120,160)
        P[x,y]=c
        if b>0.97 and 1<x<W-2 and 1<y<H-2:
            for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)): P[x+dx,y+dy]=(120,140,200)
# ---------------- DS pair + generic handheld frame
SHELL=[(16,17,20),(30,32,36),(44,46,52),(60,62,70),(78,80,90),(98,100,110)]
def rrect(d,box_,r,fill):
    d.rounded_rectangle(box_,radius=r,fill=fill)
def pair(top,bot,framed=True,label=None):
    """top/bot: 256x192 native screens -> native composite (optionally framed in a generic handheld)."""
    if not framed:
        im=Image.new('RGB',(SW,SH*2+GAP),(10,10,12)); im.paste(top,(0,0)); im.paste(bot,(0,SH+GAP))
        d=ImageDraw.Draw(im); d.rectangle((0,SH,SW-1,SH+GAP-1),fill=(18,18,22))
        return im
    side=40; mt=14; mb=16; bz=4
    Wd=SW+2*side; Ht=mt+SH+bz+GAP-2*bz+bz+SH+mb
    im=Image.new('RGB',(Wd+8,Ht+8),(0,0,0,)); 
    im=Image.new('RGB',(Wd+8,Ht+8),(22,22,18)); d=ImageDraw.Draw(im)
    ox,oy=4,4
    ty=oy+mt; by=ty+SH+GAP
    hinge_y0=ty+SH+1; hinge_y1=by-2
    # drop shadow
    rrect(d,(ox+3,oy+3,ox+Wd+3,oy+Ht+3),14,(8,8,6))
    # lower shell / upper shell
    for (y0,y1) in ((oy,hinge_y0+2),(hinge_y1-2,oy+Ht)):
        rrect(d,(ox,y0,ox+Wd-1,y1),14,SHELL[0])
        rrect(d,(ox+1,y0+1,ox+Wd-2,y1-1),13,SHELL[2])
        rrect(d,(ox+2,y0+2,ox+Wd-3,y1-3),12,SHELL[3])
    # hinge barrel (dark gap between screens)
    d.rectangle((ox+side-10,hinge_y0+1,ox+Wd-side+9,hinge_y1-1),fill=(10,10,12))
    d.rectangle((ox+side-10,hinge_y0+1,ox+side+30,hinge_y1-1),fill=SHELL[1]); d.rectangle((ox+Wd-side-31,hinge_y0+1,ox+Wd-side+9,hinge_y1-1),fill=SHELL[1])
    d.line((ox+side-10,hinge_y0+2,ox+side+30,hinge_y0+2),fill=SHELL[4]); d.line((ox+Wd-side-31,hinge_y0+2,ox+Wd-side+9,hinge_y0+2),fill=SHELL[4])
    # screen bezels
    for y in (ty,by):
        d.rectangle((ox+side-bz,y-bz,ox+side+SW-1+bz,y+SH-1+bz),fill=(6,6,8))
        d.rectangle((ox+side-bz+1,y-bz+1,ox+side+SW-1+bz-1,y+SH-1+bz-1),fill=(16,16,20))
    im.paste(top,(ox+side,ty)); im.paste(bot,(ox+side,by))
    # speaker grilles on top half
    for sx in (ox+side//2, ox+Wd-side//2):
        for k in range(5):
            for j in range(2):
                x=sx-3+j*5; y=ty+70+k*6; d.rectangle((x,y,x+1,y+1),fill=SHELL[0])
    # D-pad (left) and four face buttons (right) on lower half
    cx,cy=ox+side//2,by+46
    d.rectangle((cx-3,cy-11,cx+3,cy+11),fill=(8,8,10)); d.rectangle((cx-11,cy-3,cx+11,cy+3),fill=(8,8,10))
    d.rectangle((cx-2,cy-10,cx+2,cy+10),fill=SHELL[1]); d.rectangle((cx-10,cy-2,cx+10,cy+2),fill=SHELL[1])
    d.line((cx-2,cy-10,cx+2,cy-10),fill=SHELL[4]); d.line((cx-10,cy-2,cx-10,cy+2),fill=SHELL[4])
    bx,byy=ox+Wd-side//2,by+46
    for dx,dy in ((0,-9),(-9,0),(9,0),(0,9)):
        d.ellipse((bx+dx-5,byy+dy-5,bx+dx+4,byy+dy+4),fill=(8,8,10)); d.ellipse((bx+dx-4,byy+dy-4,bx+dx+3,byy+dy+3),fill=SHELL[1])
        d.point((bx+dx-2,byy+dy-2),fill=SHELL[5])
    # small start/select pills + power led
    for k in range(2):
        d.rounded_rectangle((bx-6,byy+26+k*8,bx+5,byy+29+k*8),radius=1,fill=(8,8,10))
    d.rectangle((cx-2,by+SH-30,cx+1,by+SH-28),fill=(80,220,110))
    # stylus mic dot
    d.rectangle((ox+Wd//2-1,hinge_y1+1,ox+Wd//2,hinge_y1+2),fill=SHELL[0])
    tag(im,ox+Wd-46,oy+3)
    if label: text(im,ox+12,oy+4,label,(196,198,206))
    return im
def save_pair(top,bot,path,label=None):
    im=pair(top,bot,label=label); up(im,3).save(path); return im
