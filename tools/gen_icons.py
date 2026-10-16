#!/usr/bin/env python3
# v1.3.3 menu icons: 12x12 pictograms, 16-colour shared palette -> src/mdicons.inc (+ build/icons.png preview)
import os
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAL={'.':(0,0,0),'K':(20,20,28),'W':(240,240,240),'G':(150,156,170),'Y':(236,196,92),'R':(220,50,40),'O':(255,140,30),'B':(80,150,240),
     'C':(90,210,240),'N':(90,200,90),'D':(110,120,70),'M':(150,100,56),'P':(232,186,140),'S':(80,84,96),'L':(250,230,120),'V':(170,110,220)}
KEYS=list(PAL.keys())
I={}
I['camp']=["............","GGGGGGGGGGG.","GMMMMMMMMMG.","GMNNMMMRRMG.","GMNNMMRMMMG.","GMMMMRMMNMG.","GMMMRMMNNMG.","GMRRMMMMMMG.","GMRMMMMYMMG.","GMMMMMMYYMG.","GGGGGGGGGGG.","............"]
I['rifle']=["............","............","...........K","KKKKKKKKKKK.","SSSSSSSSSSS.","MMMMSSSKK...","MMMM.SK.....","MMM..SK.....","MM....K.....","............","............","............"]
I['tank']=["............","............","....DDDD....","...DDDDDGGGG","...DDDDD....",".DDDDDDDDDD.","DDDDDDDDDDDD","SKSKSKSKSKSK","KSSSSSSSSSSK",".KKKKKKKKKK.","............","............"]
I['jet']=[".....GG.....",".....GG.....","....GGGG....","....GCCG....","...GGGGGG...",".GGGGGGGGGG.","GGGGGGGGGGGG","....GGGG....",".....GG.....","....GGGG....","...GG..GG...","....O..O...."]
I['ship']=["............","......G.....","......G.....",".....GGG....","...GGGGG.G..","..GGGGGGGGG.","SSSSSSSSSSSS",".SSSSSSSSSS.","..SSSSSSSS..","BBBBBBBBBBBB",".B..B..B..B.","............"]
I['sword']=["..........W.",".........WG.","........WG..",".......WG...","......WG....",".....WG.....","..Y.WG......","...YG.......","...MY.......","..M..Y......",".M..........","............"]
I['fps']=[".....RR.....",".....RR.....","...RR..RR...","..R......R..",".R...RR...R.","RRR.RRRR.RRR","RRR.RRRR.RRR",".R...RR...R.","..R......R..","...RR..RR...",".....RR.....",".....RR....."]
I['spec']=["...WWWWWW...","..WWGWWGWW..",".WWGWWWWGWW.","WW.G....G.WW","G...G..G...G",".G...GG...G.","..G..GG..G..","....KDDK....","....DDDD....","....DDDD....","....D..D....","...KK..KK..."]
I['brad']=["............","............","......SSSSS.","...DDDDDD...","..DDDDDDDD..",".DDDDDDDDDDD","DDDDDDDDDDDD","DKDKDKDKDKDD","KSSSSSSSSSSK",".KKKKKKKKKK.","............","............"]
I['snipe']=["....KKKK....","..KK.RR.KK..",".K...RR...K.",".K...RR...K.","K....RR....K","KRRRR..RRRRK","KRRRR..RRRRK","K....RR....K",".K...RR...K.",".K...RR...K.","..KK.RR.KK..","....KKKK...."]
I['eod']=["........Y.Y.",".......L.Y..","......KK..Y.",".....KK.....","...SSSSS....","..SSSSSSS...",".SSWSSSSSS..",".SWSSSSSSS..",".SSSSSSSSS..",".SSSSSSSSS..","..SSSSSSS...","...SSSSS...."]
I['lrrp']=["............","....DDDD....","...DDDDDD...","DDDDDDDDDDDD","..PPPPPPPP..","..PKPPPPKP..","..PPPPPPPP..","...PPMMPP...","..DDDDDDDD..",".DDDDDDDDDD.",".DDDDDDDDDD.","............"]
I['fire']=["....KKKK....","...KYYYYK...","..KKKKKKKK..","KKKKKKKKKKKK",".....RR.....","....ROOR....","...ROLLOR...","...OLLLLO...","..ROLWWLOR..","..OLLWWLLO..","...OLLLLO...","....OOOO...."]
I['aa']=["W.........W.",".W.......W..","..O.....O...","...O...O....","....S.S.....",".....SS.....","....SSSS....","...SDDDDS...","..DDDDDDDD..","..DDDDDDDD..",".KKKKKKKKKK.","............"]
I['drone']=["SSSS....SSSS","..S......S..","..SS....SS..","...SGGGGS...","....GGGG....","....GCCG....","....GGGG....","...SGGGGS...","..SS....SS..","..S......S..","SSSS....SSSS","............"]
I['flight']=["............","............","..........G.",".........GG.","GGGGGGGGGGGG","GCGGGGGGGGG.","..GGGGG.....","....GGG.....","......GG....","............","............","............"]
I['over']=["............","............","....KKKK....","..KKKKKKKK..",".KKKWWWWKKK.","KKKWWBBWWKKK","KKKWWBBWWKKK",".KKKWWWWKKK.","..KKKKKKKK..","....KKKK....","............","............"]
I['rocket']=[".....WW.....","....WWWW....","....WRRW....","....WWWW....","....WBBW....","....WBBW....","....WWWW....","...WWWWWW...","..WW.WW.WW..",".....OO.....","....OLLO....",".....OO....."]
I['space']=["............","..G.......G.","...G.....G..","GGGG.SS.GGGG","BBBBSSSSBBBB","BBBBSCCSBBBB","GGGGSSSSGGGG","...G.SS.G...","..G.......G.","............","..W.....W...","............"]
I['astro']=["....WWWW....","..WWWWWWWW..",".WWYYYYYYWW.",".WYYYCYYYYW.",".WYYYYYYYYW.",".WWYYYYYYWW.","..WWWWWWWW..","..GWWWWWWG..",".WWWGRRGWWW.",".WWWWWWWWWW.",".WW.WWWW.WW.","....WWWW...."]
I['moon']=["....GGGG....","..GGGWWGGG..",".GGWWWWWSGG.",".GWWSWWWWWG.","GGWSSWWWWWGG","GWWWWWWWSWWG","GWWWWWWSSWWG","GGWWWSWWWWGG",".GWWWWWWWWG.",".GGWWWWWWGG.","..GGGWWGGG..","....GGGG...."]
I['mars']=["....RRRR....","..RRRORRRR..",".RRORRRRMRR.",".RRRRRRMMRR.","RRRRRRRRRRRR","RMMRRRORRRRR","RMRRRRRRRRRR","RRRRRMMRRRRR",".RRRRMRRRRR.",".RRRRRRRORR.","..RRRRRRRR..","....RRRR...."]
I['odef']=["....CCCC....","..CC....CC..",".C..BBBB..C.","C..BBBBBB..C","C.BBBNNBBB.C","C.BBNNNNBB.C","C.BBBNNBBB.C","C..BBBBBB..C",".C..BBBB..C.","..CC....CC..","....CCCC....","............"]
I['aster']=["............","....MMMM....","..MMSMMMMM..",".MMMMMMSMMM.",".MSSMMMMMMM.","MMMMMMMMMSMM","MMMMMSMMMMMM",".MMMMMMMMMM.",".MMSMMMMSM..","..MMMMMMM...","....MMM.....","............"]
I['rbuild']=[".....WW.....","....WWWW....","....WRRW....","....WWWW....","....WWWW.GG.","...WWWWWGG..","..WW.WWGG...",".......GGG..","......GG.GG.",".....GG...G.","............","............"]
I['whatif']=["...YYYYYY...","..YY....YY..","..YY....YY..","........YY..",".......YY...","......YY....",".....YY.....",".....YY.....","............",".....YY.....",".....YY.....","............"]
I['hist']=["............",".MMMMMMMMMM.",".MWWWWMWWWW.",".MWGGWMWGGW.",".MWWWWMWWWW.",".MWGGWMWGGW.",".MWWWWMWWWW.",".MWGGWMWGGW.",".MWWWWMWWWW.",".MMMMMMMMMM.","......R.....","......R....."]
I['vbm']=["BBBBBBBBBBBB","BCBBBWWBBBCB","BBBBWWWWBBBB","BBBBWBBWBBBB","BCBBWBBWBBBB","BBBBWWWWBBCB","BBBWWBBWWBBB","BBWWBBBBWWBB","BCBBBOOBBBBB","BBBBOLLOBBBB","BBBBBOOBBBCB","BBBBBBBBBBBB"]
I['invade']=["..N......N..","...N....N...","..NNNNNNNN..",".NN.NNNN.NN.","NNNNNNNNNNNN","N.NNNNNNNN.N","N.N......N.N","...NN..NN...","............",".....WW.....","....WWWW....","...WWWWWW..."]
I['race']=["............","............","....RRRR....","...RCCCCR...","..RRRRRRRR..",".RRRRRRRRRR.",".RWRRRRRRWR.",".RRRRRRRRRR.",".KKK....KKK.",".KKK....KKK.","............","............"]
I['tt']=["....WWWW....","......W.....","...WWWWWW...","..WW.R..WW..",".WW..R...WW.",".W...R....W.",".W...RRR..W.",".W........W.",".WW......WW.","..WW....WW..","...WWWWWW...","............"]
I['free']=["....S..S....","....S..S....","...SS..SS...","...S.YY.S...","..SS....SS..","..S..YY..S..",".SS......SS.",".S...YY...S.","SS........SS","S....YY....S","S..........S","............"]
I['build']=["..GG........",".G..G.......",".G..GG......","..GGGGG.....","......GG....",".......GG...","....Y...GG..","...YYY...GG.","YYYYYYYYY.G.","...YYY......","....Y.......","...YYY......"]
I['sand']=["............","....R.......","....RR......","....R.......","...MMM......","..MMMMM.MMM.","..M.M.M.MMM.",".MMMMMMMMMMM",".MMMMMMMMMMM","YYYYYYYYYYYY","YYYYYYYYYYYY","............"]
I['boot']=["............","..MM....MM..",".MMMM..MMMM.",".MMMM..MMMM.","..MM....MM..","............","....MMMM....","...MMMMMM...","..MMMMMMMM..","..MMMMMMMM..","...MM..MM...","............"]
I['loadout']=["...DD..DD...","..DDD..DDD..",".DDDDDDDDDD.",".DDKDDDDKDD.",".DDKDDDDKDD.",".DDDDDDDDDD.",".DKKDDDDKKD.",".DKKDDDDKKD.",".DDDDDDDDDD.",".DDDDDDDDDD.","..DDDDDDDD..","............"]
I['settings']=[".....GG.....","..G.GGGG.G..",".GGGGGGGGGG.","..GGG..GGG..",".GGG....GGG.","GGG......GGG","GGG......GGG",".GGG....GGG.","..GGG..GGG..",".GGGGGGGGGG.","..G.GGGG.G..",".....GG....."]
I['credits']=["............","SSSSSSSSSSSS","SWSWSWSWSWSW","SSSSSSSSSSSS","SYYYYYYYYYYS","SYYYYLYYYYYS","SYYYLLLYYYYS","SYYYYLYYYYYS","SSSSSSSSSSSS","SWSWSWSWSWSW","SSSSSSSSSSSS","............"]
I['codes']=["............","....GGGG....","...G....G...","...G....G...","...G....G...","..YYYYYYYY..","..YYYYYYYY..","..YYYKKYYY..","..YYYKKYYY..","..YYYYKYYY..","..YYYYYYYY..","............"]
I['store']=["............","....M..M....","...M....M...","...M....M...","..OOOOOOOO..",".OOOOOOOOOO.",".ORROOOOOOO.",".OOOOOROOOO.",".OOOOOOOROO.",".OOROOOOOOO.","..OOOOOOOO..","............"]
I['trophy']=["............",".YYYYYYYYYY.","YYLYYYYYYYYY","Y.YLYYYYYY.Y","Y.YYYYYYYY.Y",".YYYYYYYYYY.","..YYYYYYYY..","....YYYY....",".....YY.....","....YYYY....","...MMMMMM...","...MMMMMM..."]
I['profile']=["....PPPP....","...PPPPPP...","...PKPPKP...","...PPPPPP...","....PPPP....","..BBBBBBBB..",".BBBBYBBBBB.",".BBBBBBBBBB.",".BBBBBBBBBB.",".BBBBBBBBBB.",".BBBBBBBBBB.","............"]
I['lb']=["............","....YYYY....","....YLYY....","....YYYY....","GGGGYYYY....","GWGGYYYYMMMM","GGGGYYYYMWMM","GGGGYYYYMMMM","GGGGYYYYMMMM","GGGGYYYYMMMM","GGGGYYYYMMMM","............"]
NAMES=list(I.keys())
for n in NAMES:
    assert len(I[n])==12 and all(len(r)==12 for r in I[n]),(n,[len(r) for r in I[n]])
o=["// generated by tools/gen_icons.py","enum{"+",".join("IC_"+n.upper() for n in NAMES)+",IC_N};",
   "static const u16 IC_PAL[16]={"+",".join("RGB(%d,%d,%d)"%(r>>3,g>>3,b>>3) if False else "0x%04X"%((r>>3)|((g>>3)<<5)|((b>>3)<<10)) for (r,g,b) in PAL.values())+"};",
   "static const u8 IC_DAT[IC_N][72]={"]
for n in NAMES:
    b=[]
    for r in I[n]:
        for x in range(0,12,2):b.append(KEYS.index(r[x])|(KEYS.index(r[x+1])<<4))
    o.append(" {"+",".join(map(str,b))+"},")
o.append("};")
open(os.path.join(ROOT,'src','mdicons.inc'),'w').write("\n".join(o)+"\n")
try:
    from PIL import Image
    im=Image.new('RGB',(len(NAMES)*14,14),(30,36,60))
    for i,n in enumerate(NAMES):
        for y,r in enumerate(I[n]):
            for x,c in enumerate(r):
                if c!='.':im.putpixel((i*14+1+x,1+y),PAL[c])
    im.resize((im.width*4,im.height*4),Image.NEAREST).save(os.path.join(ROOT,'build','icons.png'))
except Exception as e:print(e)
print(len(NAMES),"icons")
