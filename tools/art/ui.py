from weap import *
from gbalib import text,tw,up,mockup_tag,GOLD,GRAY,WHITE,SHADOW,GLYPH
PF=(70,76,42); PL=(118,126,74); PD=(36,40,20); SLOT=(98,110,64); SLOTD=(60,68,36)
YG=(222,228,124); PR=(124,236,96); TX=(236,234,206); DIM=(160,166,110)
def bevel(im,x0,y0,x1,y1,fill=PF,raised=True,lt=PL,dk=PD):
    d=ImageDraw.Draw(im); d.rectangle((x0,y0,x1,y1),fill=(10,12,6))
    a,b=(lt,dk) if raised else (dk,lt)
    d.rectangle((x0+1,y0+1,x1-1,y1-1),fill=b); d.rectangle((x0+1,y0+1,x1-2,y1-2),fill=a); d.rectangle((x0+2,y0+2,x1-2,y1-2),fill=fill)
def slot(im,x0,y0,x1,y1,fill=SLOT,sel=False):
    bevel(im,x0,y0,x1,y1,fill,raised=False)
    if sel:
        d=ImageDraw.Draw(im); d.rectangle((x0,y0,x1,y1),outline=GOLD)
def tab(im,x0,y0,x1,y1,s,sel=False):
    bevel(im,x0,y0,x1,y1,(92,102,56) if sel else PF,raised=not sel)
    text(im,(x0+x1)//2-tw(s)//2+1,y0+(y1-y0)//2-3,s,GOLD if sel else YG)
def rock(im,x,y):
    pts=[(x+1,y+2),(x+3,y),(x+6,y),(x+8,y+2),(x+8,y+5),(x+6,y+7),(x+2,y+7),(x,y+5)]
    d=ImageDraw.Draw(im); d.polygon(pts,fill=(10,8,6)); d.polygon([(x+2,y+2),(x+3,y+1),(x+6,y+1),(x+7,y+2),(x+7,y+5),(x+6,y+6),(x+2,y+6),(x+1,y+5)],fill=(176,74,40))
    d.point((x+3,y+2),fill=(236,140,96)); d.point((x+4,y+2),fill=(236,140,96)); d.line((x+5,y+5,x+6,y+5),fill=(110,40,22))
def tiny(im,x,y,s,col=TX):
    """3x5 font for counts"""
    G={'0':'###|#.#|#.#|#.#|###','1':'.#.|##.|.#.|.#.|###','2':'##.|..#|.#.|#..|###','3':'##.|..#|.#.|..#|##.','4':'#.#|#.#|###|..#|..#',
       '5':'###|#..|##.|..#|##.','6':'.##|#..|###|#.#|###','7':'###|..#|.#.|.#.|.#.','8':'###|#.#|###|#.#|###','9':'###|#.#|###|..#|##.',
       'X':'#.#|#.#|.#.|#.#|#.#','.':'...|...|...|...|.#.','M':'#.#|###|###|#.#|#.#','K':'#.#|##.|#..|##.|#.#','G':'.##|#..|#.#|#.#|.##'}
    P=im.load()
    for ch in s:
        g=G.get(ch.upper())
        if g:
            for r,row in enumerate(g.split('|')):
                for c,b in enumerate(row):
                    if b=='#': P[x+c+1,y+r+1]=(8,8,6); P[x+c,y+r]=col
        x+=4
