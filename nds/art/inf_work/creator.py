from ui import *
from loadout import portrait
HALE=spec(skin=5,hair='BLACK',face='STUBBLE',head='BOONIE',unif='DESERT TAN',camo='TIGER STRIPE',gear='RADIO',build='SLIM',rifle='M4')
VARS=[('HELMET','PLATE','WOODLAND',spec(skin=2,hair='BROWN',head='HELMET',unif='OLIVE DRAB',camo='WOODLAND',gear='PLATE',rifle='M4',gloves=R((60,58,44)))),
      ('BERET','WEBBING','OLIVE',spec(skin=3,hair='BLACK',head='BERET',unif='OLIVE DRAB',camo='SOLID',gear='WEBBING',rifle='M16')),
      ('CAP','DIGITAL','URBAN',spec(skin=6,hair='BLACK',face='LONG',head='CAP',unif='URBAN GREY',camo='DIGITAL',gear='WEBBING',rifle='M4'))]
def frame_fig(im,box,s,sc,view='F',feet_pad=3,bgc=((40,44,28),(54,60,38),(70,76,50),(88,94,62),(104,110,74))):
    x0,y0,x1,y1=box; slot(im,x0,y0,x1,y1,fill=(54,58,40))
    w,h=x1-x0-3,y1-y0-3
    bg=Image.new('RGB',(w,h)); B=bg.load()
    for y in range(h):
        for x in range(w):
            t=y/h; B[x,y]=dith(0.85-0.75*t if t<0.78 else 0.35,x,y,list(bgc))
    # floor line
    d=ImageDraw.Draw(bg); fy=h-feet_pad-1
    d.line((0,int(h*0.78),w,int(h*0.78)),fill=bgc[1])
    spr=sprite(s,sc,view)
    place(bg,spr,w//2,fy,shadow_col=bgc[0],sw=int(18*sc))
    im.paste(bg,(x0+2,y0+2))
def screen():
    im=Image.new('RGB',(240,160),(22,24,14))
    bevel(im,0,0,136,159); bevel(im,137,0,239,159)
    text(im,4,3,'CHARACTER CREATOR',GOLD)
    rows=[('NAME','HALE'),('SKIN','TONE 5'),('HAIR','BLACK'),('FACE','STUBBLE'),('HEADGEAR','BOONIE HAT'),('UNIFORM','DESERT TAN'),
          ('CAMO','TIGER STRIPE'),('GEAR','RADIO PACK'),('BUILD','SLIM'),('IN GAME','ON')]
    y=15
    for k,v in rows:
        sel = k=='CAMO'
        if sel:
            bevel(im,2,y-2,134,y+8,(96,106,58),raised=False)
            text(im,52,y,'<',GOLD); text(im,128,y,'>',GOLD)
        text(im,5,y,k,GOLD if sel else YG); text(im,58,y,v,WHITE if sel else TX)
        if k=='SKIN':
            for i,b in enumerate(SKIN_BASE):
                xx=100+i*5; d=ImageDraw.Draw(im); d.rectangle((xx,y,xx+4,y+6),fill=(8,8,6)); d.rectangle((xx+1,y+1,xx+3,y+5),fill=b)
            d.rectangle((100+4*5,y-1,100+4*5+4,y+7),outline=GOLD)
        y+=11
    bevel(im,4,126,82,138); text(im,10,129,'SAVE & BACK',TX)
    text(im,4,143,'A:EDIT  B:BACK',DIM); text(im,4,151,'L/R:ROTATE VIEW',DIM)
    # right panel
    text(im,141,3,'HALE',(120,200,255)); text(im,170,3,'PREVIEW',DIM)
    frame_fig(im,(140,11,236,99),HALE,1.56,feet_pad=2)
    d=ImageDraw.Draw(im)
    for x,ch in ((143,'<'),(229,'>')):
        d.rectangle((x-1,50,x+5,58),fill=(16,18,10)); text(im,x,51,ch,GOLD,shadow=None)
    for i,t in enumerate(['FRONT','SIDE','BACK']):
        x=140+i*32; bevel(im,x,101,x+31,111,(96,106,58) if i==0 else PF,raised=i!=0)
        text(im,x+16-tw(t)//2,103,t,GOLD if i==0 else YG)
    for i,(a,b,c,s) in enumerate(VARS):
        x=140+i*32; frame_fig(im,(x,113,x+31,157),s,0.72,feet_pad=2)
    mockup_tag(im,x=90,y=127)
    return im
def sheet():
    A=screen()
    W=240*2+12; S=Image.new('RGB',(W,16+12+160+6+12+70+4),(16,16,12))
    t='ARSENAL OF ERAS - CHARACTER CREATOR REDESIGN'; text(S,(W-tw(t))//2,4,t,GOLD); mockup_tag(S)
    text(S,4,18,'CREATOR SCREEN (240X160)',(206,200,170)); S.paste(A,(4,28))
    # right: rotate views
    R_=Image.new('RGB',(240,160),(22,24,14)); bevel(R_,0,0,239,159)
    text(R_,4,3,'ROTATE: HALE',YG)
    for i,(v,nm) in enumerate((('F','FRONT'),('S','SIDE'),('B','BACK'))):
        x=3+i*78; frame_fig(R_,(x,12,x+77,104),HALE,1.42,view=v); text(R_,x+39-tw(nm)//2,106,nm,GOLD)
    text(R_,4,117,'VARIANT THUMBNAILS (RIGHT PANEL):',YG)
    for i,(a,b,c,s) in enumerate(VARS):
        x=4+i*78; text(R_,x,127,a,TX); text(R_,x,136,b,DIM); text(R_,x,145,c,DIM)
    text(S,248,18,'L/R ROTATE + VARIANT KEY',(206,200,170)); S.paste(R_,(248,28))
    # skin tones
    y0=28+160+6; text(S,4,y0,'SKIN TONE 1-6: DISTINCT RAMPS, SAME FACE DETAIL',(206,200,170))
    T=Image.new('RGB',(W-8,70),(22,24,14)); bevel(T,0,0,W-9,69)
    for i in range(6):
        x=4+i*80; s=spec(skin=i+1,hair=['BLOND','RED','BROWN','BLACK','BLACK','GREY'][i],face=['CLEAN','CLEAN','STUBBLE','CLEAN','STUBBLE','CLEAN'][i],head='NONE',unif='OLIVE DRAB',gear='NONE')
        portrait(T,x,3,x+50,66,s,sc=2.6,top=-52)
        text(T,x+54,8,'TONE',YG); text(T,x+54,17,str(i+1),GOLD)
        d=ImageDraw.Draw(T); d.rectangle((x+54,28,x+70,40),fill=(8,8,6))
        for k,c in enumerate(SKINS[i][1:]): d.rectangle((x+55+k*3,29,x+57+k*3,39),fill=c)
    S.paste(T,(4,y0+10))
    return S,A
if __name__=='__main__':
    S,A=sheet(); up(S,3).save('/workspace/character_creator_mockup.png'); up(A,3).save('ca.png')
