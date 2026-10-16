# v1.2 vehicle database + procedural sprites (exec'd from gen_assets.py; shares its globals).
# Emits tank_spr / plane_spr / ship_spr sprite banks and writes src/vehdb.inc (C tables).
VC=['LT','MT','HT','TD','TDC','SPG','IFV','APC','AC','JEEP','MINE','ARTY','MLRS','SPAA','MRAP']
VCN={'LT':'LIGHT TANK','MT':'MEDIUM TANK','HT':'HEAVY TANK','TD':'TANK DESTROYER','TDC':'CASEMATE TD','SPG':'ASSAULT GUN / SPG','IFV':'INFANTRY FIGHTING VEH','APC':'APC / HALFTRACK','AC':'ARMORED CAR','JEEP':'JEEP / UTILITY','MINE':'TRACKED MINE','ARTY':'SP HOWITZER (CALL-IN)','MLRS':'ROCKET ARTILLERY','SPAA':'SPAAG / AIR DEFENSE','MRAP':'MRAP / PATROL VEH'}
def tk(sp):
    L=sp.get('L',22);W=sp.get('W',18);oy=sp.get('oy',0);cx=16
    h=Cv(32,32);t=Cv(32,32)
    y0=16-L//2+oy;y1=y0+L-1;x0=cx-W//2;x1=cx+W//2-1;wh=sp.get('wh',0);tw=sp.get('tw',4 if W>=16 else 3)
    if wh==0:
        for xa in (x0,x1-tw+1):
            h.r(xa,y0,xa+tw-1,y1,12)
            for y in range(y0+1,y1,3):h.l(xa,y,xa+tw-1,y,13)
        bx0,bx1=x0+tw-1,x1-tw+1
    elif wh=='half':
        for xa in (x0,x1-2):
            h.r(xa,y0+L//2,xa+2,y1,12);[h.l(xa,y,xa+2,y,13) for y in range(y0+L//2+1,y1,3)]
            h.r(xa,y0+2,xa+2,y0+5,12)
        bx0,bx1=x0+1,x1-1
    else:
        n=wh//2
        for k in range(n):
            y=y0+2+k*(L-6)//max(1,n-1) if n>1 else y0+L//2
            h.r(x0,y,x0+2,y+3,12);h.r(x1-2,y,x1,y+3,12)
        bx0,bx1=x0+1,x1-1
    body=sp.get('body',3)
    if sp.get('jeep'):
        h.r(bx0,y0,bx1,y1,body);h.r(bx0+1,y0,bx1-1,y0+3,4);h.l(bx0,y0+5,bx1,y0+5,10);h.r(bx0+1,y0+7,bx1-1,y1-2,7);h.r(bx0+2,y0+8,cx-1,y0+10,2);h.r(cx+1,y0+8,bx1-2,y0+10,2)
    elif sp.get('mine'):
        h.r(bx0,y0,bx1,y1,body);h.r(bx0+1,y0+1,bx1-1,y0+3,4);h.px(cx,y1-1,9)
    else:
        h.p([(bx0,y0+3),(bx0+2,y0),(bx1-2,y0),(bx1,y0+3),(bx1,y1),(bx0,y1)],body)
        h.r(bx0+1,y0+1,bx1-1,y0+3,4)
        if not sp.get('open'):h.r(bx0+1,y1-4,bx1-1,y1-1,2);[h.l(bx0+2,y,bx1-2,y,7) for y in (y1-3,y1-1)]
        else:h.r(bx0+2,y0+L//3,bx1-2,y1-2,7);h.r(bx0+2,y1-2,bx1-2,y1-2,2)
        h.px(cx,y1,9);h.px(cx-1,y1,9)
    if sp.get('skirt'):h.r(x0,y0+3,x0,y1-2,2);h.r(x1,y0+3,x1,y1-2,2)
    gl=sp.get('gl',12);gt=sp.get('gt',2);tu=sp.get('tur','r');tsw,tsh=sp.get('ts',(10,10))
    def gun(cv,ytop,ybase):
        gx0=cx-gt//2;cv.r(gx0,ytop,gx0+gt-1,ybase,14);cv.l(gx0,ytop,gx0,ybase,15 if False else 14)
        if sp.get('mb'):cv.r(gx0-1,ytop,gx0+gt,ytop+1,14)
        if sp.get('fume'):cv.r(gx0-1,ytop+gl//2,gx0+gt,ytop+gl//2+1,2)
    if tu=='n':  # casemate: superstructure + hull-mounted gun (hull sprite carries the gun)
        cw,ch=sp.get('cas',(W-6,L//2));cy0=y0+sp.get('cy',4)
        h.r(cx-cw//2,cy0,cx+cw//2-1,cy0+ch,3);h.r(cx-cw//2+1,cy0,cx+cw//2-2,cy0+1,4);h.r(cx-cw//2+1,cy0+ch-1,cx+cw//2-2,cy0+ch,2)
        if sp.get('mort'):h.e(cx-3,cy0-3,cx+2,cy0+2,14);h.e(cx-2,cy0-2,cx+1,cy0+1,7)
        else:gun(h,max(0,cy0-gl),cy0+2)
        h.e(cx+cw//2-4,cy0+ch-4,cx+cw//2-2,cy0+ch-2,2)
    elif tu=='mg':
        t.r(cx-1,16-gl,cx,16,14);t.e(cx-2,14,cx+1,17,2);t.r(cx-3,15,cx+2,16,7)
    else:
        tx0,ty0=cx-tsw//2,16-tsh//2;tx1,ty1=tx0+tsw-1,ty0+tsh-1
        if tu=='r':t.e(tx0,ty0,tx1,ty1,3)
        elif tu=='b':t.r(tx0,ty0,tx1,ty1,3);t.r(tx0+1,ty0,tx1-1,ty0+1,4)
        elif tu=='h':t.p([(tx0+2,ty0),(tx1-2,ty0),(tx1,ty0+3),(tx1,ty1),(tx0,ty1),(tx0,ty0+3)],3)
        elif tu=='c':t.e(tx0,ty0,tx1,ty1,3);t.e(tx0+1,ty0+1,tx1-1,ty0+tsh//2,4)
        elif tu=='o':t.e(tx0,ty0,tx1,ty1,3);t.e(tx0+2,ty0+2,tx1-2,ty1-2,7)
        gun(t,max(0,ty0-gl),ty0+2);t.r(cx-gt//2-1,ty0,cx+gt//2,ty0+1,2)
        if tu!='o':t.e(tx1-4,ty1-4,tx1-2,ty1-2,4);t.px(tx1-3,ty1-3,2)
        t.px(tx0+2,ty1-1,9)
        if sp.get('rk'):t.r(cx-8,ty0-1,cx+7,ty0+3,14);[t.l(x,ty0-1,x,ty0+3,7) for x in range(cx-7,cx+8,2)]
        if sp.get('coax'):t.r(cx+gt//2+1,ty0-4,cx+gt//2+1,ty0,14)
        if sp.get('atgm'):t.r(tx0-2,ty0+1,tx0-1,ty0+6,13);t.r(tx0-2,ty0,tx0-1,ty0,7)
        if sp.get('pod'):
            pw,ph=sp['pod'];px0,py0=cx-pw//2,16-ph//2-2;t.r(px0,py0,px0+pw-1,py0+ph-1,3);t.r(px0,py0,px0+pw-1,py0,4)
            [t.px(x,y,9) for x in range(px0+1,px0+pw-1,2) for y in range(py0+1,py0+ph//2,2)]
        if sp.get('twin'):t.r(cx-4,ty0-gl+2,cx-4,ty0,14);t.r(cx+3,ty0-gl+2,cx+3,ty0,14);t.e(tx1-1,ty0-3,tx1+2,ty0,7)
    return h.grid(1),t.grid(1)
# ---------------------------------------------------------------- tanks: (name,cls,axis,exp,spd,turn,reload,dmg,hp,sspd,ammo2,spritespec)
TK0=[
 ('M4 SHERMAN','MT',0,0,240,320,85,32,100,1100,0,dict(L=22,W=18,tur='c',ts=(10,11),gl=10)),
 ('TIGER I','HT',1,0,170,230,110,48,150,1300,0,dict(L=24,W=20,tur='b',ts=(12,11),gl=13,mb=1)),
 ('SHERMAN FIREFLY','MT',0,0,235,315,100,44,100,1300,0,dict(L=22,W=18,tur='c',ts=(10,11),gl=15,mb=1)),
 ('M4A3E8 EASY 8','MT',0,0,255,330,80,36,105,1250,0,dict(L=23,W=18,tur='c',ts=(10,12),gl=12,mb=1)),
 ('M4A3E2 JUMBO','HT',0,0,190,270,90,34,145,1150,0,dict(L=22,W=19,tur='b',ts=(12,12),gl=10)),
 ('M26 PERSHING','HT',0,0,215,290,95,46,135,1350,0,dict(L=25,W=19,tur='c',ts=(11,12),gl=14,mb=1)),
 ('M24 CHAFFEE','LT',0,0,320,380,70,24,75,1100,0,dict(L=20,W=16,tur='r',ts=(9,9),gl=10)),
 ('M10 WOLVERINE','TD',0,0,250,300,95,42,85,1300,0,dict(L=23,W=18,tur='o',ts=(12,11),gl=13)),
 ('M18 HELLCAT','TD',0,0,360,360,85,40,70,1300,0,dict(L=22,W=16,tur='o',ts=(11,10),gl=13,mb=1)),
 ('M36 JACKSON','TD',0,0,245,300,100,50,90,1400,0,dict(L=23,W=18,tur='o',ts=(12,12),gl=15,mb=1)),
 ('M3 STUART','LT',0,0,330,400,60,20,70,1050,0,dict(L=18,W=15,tur='b',ts=(8,8),gl=8)),
 ('M3 LEE','MT',0,0,220,290,95,30,110,1100,0,dict(L=22,W=18,tur='r',ts=(8,8),gl=8,coax=1)),
 ('M8 GREYHOUND','AC',0,0,380,380,65,22,60,1050,0,dict(L=20,W=14,wh=6,tur='o',ts=(9,9),gl=8)),
 ('M3 HALFTRACK','APC',0,0,330,360,6,5,65,1300,2,dict(L=22,W=14,wh='half',open=1,tur='mg',gl=7)),
 ('WILLYS JEEP','JEEP',0,0,440,480,5,5,40,1300,0,dict(L=14,W=10,wh=4,jeep=1,tur='mg',gl=7)),
 ('CHURCHILL VII','HT',0,0,160,250,95,34,155,1150,0,dict(L=26,W=18,tur='b',ts=(11,10),gl=10,skirt=1)),
 ('CROMWELL','MT',0,0,330,350,80,30,95,1150,0,dict(L=22,W=18,tur='b',ts=(11,10),gl=10)),
 ('COMET','MT',0,0,300,340,85,40,105,1300,0,dict(L=23,W=18,tur='b',ts=(11,11),gl=13,mb=1)),
 ('MATILDA II','HT',0,0,140,260,90,24,150,1050,0,dict(L=21,W=18,tur='c',ts=(10,10),gl=7,skirt=1)),
 ('VALENTINE','LT',0,0,230,330,75,22,105,1050,0,dict(L=19,W=16,tur='b',ts=(9,8),gl=8)),
 ('T-34/76','MT',0,0,290,340,85,34,105,1150,0,dict(L=22,W=18,tur='h',ts=(10,10),gl=11,oy=-2)),
 ('T-34/85','MT',0,0,280,330,90,40,110,1250,0,dict(L=23,W=18,tur='c',ts=(11,11),gl=13,oy=-1)),
 ('KV-1','HT',0,0,190,260,95,36,150,1150,0,dict(L=23,W=19,tur='b',ts=(12,11),gl=10)),
 ('KV-2','HT',0,0,150,220,200,70,160,900,0,dict(L=23,W=19,tur='b',ts=(13,14),gl=6,gt=3)),
 ('IS-2','HT',0,0,200,260,150,62,160,1250,0,dict(L=25,W=20,tur='c',ts=(12,12),gl=14,gt=2,mb=1)),
 ('SU-100','TDC',0,0,260,300,100,52,110,1450,0,dict(L=23,W=18,tur='n',cas=(12,9),cy=2,gl=12,mb=0)),
 ('SU-152','SPG',0,0,210,260,190,70,140,850,0,dict(L=24,W=19,tur='n',cas=(14,11),cy=2,gl=7,gt=3,mb=1)),
 ('TYPE 97 CHI-HA','MT',1,0,260,360,75,24,80,1050,0,dict(L=20,W=16,tur='r',ts=(9,9),gl=7)),
 ('TYPE 95 HA-GO','LT',1,0,340,420,60,18,60,1000,0,dict(L=17,W=14,tur='r',ts=(8,8),gl=6,oy=-2)),
 ('PANZER III','MT',1,0,270,330,80,28,95,1150,0,dict(L=21,W=17,tur='b',ts=(10,10),gl=10)),
 ('PANZER IV H','MT',1,0,250,320,85,36,100,1250,0,dict(L=23,W=18,tur='b',ts=(11,10),gl=14,mb=1,skirt=1)),
 ('PANTHER','MT',1,0,260,300,85,44,120,1400,0,dict(L=25,W=20,tur='c',ts=(11,12),gl=15,mb=1,oy=-1)),
 ('TIGER II','HT',1,0,150,210,115,56,170,1450,0,dict(L=27,W=21,tur='b',ts=(12,13),gl=15,mb=1,oy=-2)),
 ('JAGDPANTHER','TDC',1,0,250,280,95,56,125,1450,0,dict(L=25,W=20,tur='n',cas=(14,12),cy=1,gl=12,mb=1)),
 ('STUG III','TDC',1,0,260,300,85,40,100,1300,0,dict(L=21,W=18,tur='n',cas=(12,8),cy=3,gl=10,mb=1)),
 ('HETZER','TDC',1,0,280,320,85,38,90,1300,0,dict(L=19,W=16,tur='n',cas=(10,8),cy=2,gl=9,mb=1)),
 ('ELEFANT','TDC',1,0,130,190,120,62,175,1500,0,dict(L=27,W=20,tur='n',cas=(14,10),cy=9,gl=15,mb=1)),
 ('SDKFZ 251','APC',1,0,320,350,6,5,65,1300,2,dict(L=22,W=14,wh='half',open=1,tur='mg',gl=7)),
 ('PUMA 8-RAD','AC',1,0,390,370,70,26,65,1150,0,dict(L=22,W=14,wh=8,tur='b',ts=(9,9),gl=10)),
 # ---- experimental / prototypes (generic markings)
 ('MAUS','HT',1,1,90,150,140,72,260,1450,0,dict(L=30,W=22,tur='b',ts=(14,14),gl=14,gt=3,mb=0,skirt=1)),
 ('E-100','HT',1,1,110,170,130,68,230,1450,0,dict(L=29,W=22,tur='c',ts=(14,13),gl=15,gt=3,skirt=1)),
 ('JAGDTIGER','TDC',1,1,120,170,150,76,210,1550,0,dict(L=28,W=21,tur='n',cas=(16,13),cy=8,gl=13,gt=3,mb=0)),
 ('STURMTIGER','SPG',1,1,120,180,260,95,190,700,0,dict(L=25,W=20,tur='n',cas=(16,11),cy=1,gl=4,gt=4,mort=1)),
 ('PANTHER II','MT',1,1,240,290,90,48,135,1450,0,dict(L=25,W=20,tur='b',ts=(11,11),gl=15,mb=1)),
 ('VK 45.01 (P)','HT',1,1,150,200,110,50,165,1350,0,dict(L=25,W=21,tur='c',ts=(12,12),gl=13,mb=1,oy=-3)),
 ('RAUMER S','APC',1,1,100,150,6,5,320,1300,0,dict(L=30,W=22,wh=4,tur='mg',gl=6)),
 ('GOLIATH','MINE',1,1,300,500,30,120,18,0,0,dict(L=10,W=8,tw=2,mine=1,tur='mg',gl=0)),
 ('KARL-GERAT','SPG',1,1,60,120,320,110,230,600,0,dict(L=30,W=20,tur='n',cas=(16,14),cy=2,gl=5,gt=5,mort=1)),
 ('T28 SUPER HEAVY','TDC',0,1,100,150,130,70,240,1500,0,dict(L=28,W=22,tw=5,tur='n',cas=(12,12),cy=2,gl=14,gt=2,skirt=1)),
 ('T29 HEAVY','HT',0,1,170,220,125,62,190,1500,0,dict(L=27,W=21,tur='b',ts=(13,13),gl=15,gt=2)),
 ('T30 HEAVY','HT',0,1,165,215,150,74,195,1400,0,dict(L=27,W=21,tur='b',ts=(13,13),gl=13,gt=3)),
 ('M4 CALLIOPE','MT',0,1,225,310,85,32,100,1100,4,dict(L=22,W=18,tur='c',ts=(10,11),gl=10,rk=1)),
 ('IS-3','HT',0,1,210,270,150,64,170,1300,0,dict(L=26,W=20,tur='r',ts=(13,12),gl=15,gt=2,mb=0)),
 ('TOG II','HT',0,1,100,170,100,40,220,1250,0,dict(L=31,W=18,tur='b',ts=(11,10),gl=12)),
 ('A39 TORTOISE','TDC',0,1,110,160,120,68,230,1500,0,dict(L=28,W=22,tur='n',cas=(16,15),cy=3,gl=12,gt=2,mb=0)),
]
TK1=[('M48 PATTON','MT',0,0,250,300,80,36,115,1200,0,dict(L=24,W=19,tur='c',ts=(12,12),gl=12)),
 ('T-55','MT',0,0,260,310,85,36,105,1200,0,dict(L=23,W=19,tur='r',ts=(12,12),gl=13,fume=1)),
 ('M60A3','MT',0,0,260,300,75,40,125,1350,0,dict(L=25,W=20,tur='c',ts=(12,12),gl=14,fume=1)),
 ('T-72A','MT',0,0,280,330,80,42,115,1350,0,dict(L=24,W=19,tur='r',ts=(12,11),gl=15)),
 ('BMP-2','IFV',0,0,330,380,14,10,80,1400,4,dict(L=23,W=17,tur='r',ts=(9,9),gl=12,gt=1,atgm=1,oy=-2)),
 ('M113 APC','APC',0,0,320,360,6,6,70,1300,2,dict(L=22,W=17,tur='mg',gl=7,open=0)),
 ('T-62','MT',0,0,265,315,85,38,110,1250,0,dict(L=24,W=19,tur='r',ts=(12,12),gl=14,fume=1)),
 ('BMP-1','IFV',0,0,330,380,16,12,75,1400,4,dict(L=22,W=17,tur='r',ts=(8,8),gl=11,gt=1,atgm=1,oy=-2)),
]
TK2=[('M1A2 ABRAMS','MT',0,0,290,320,70,42,130,1450,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=14,oy=-1)),
 ('T-90M','MT',0,0,280,340,65,40,120,1400,0,dict(L=25,W=20,tur='h',ts=(12,11),gl=15)),
 ('M2A3 BRADLEY','IFV',0,0,320,370,12,11,95,1450,4,dict(L=24,W=19,tur='h',ts=(11,9),gl=12,gt=1,atgm=1,oy=-3)),
 ('LEOPARD 2A7','MT',0,0,300,330,65,44,130,1500,0,dict(L=26,W=20,tur='b',ts=(13,12),gl=15)),
 ('CHALLENGER 2','HT',0,0,250,300,70,46,145,1450,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=15,fume=1)),
 ('STRYKER','APC',0,0,380,380,6,6,80,1350,2,dict(L=24,W=16,wh=8,tur='mg',gl=8)),
 ('M1A2 SEPV3','MT',0,0,300,320,66,44,140,1500,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=15,oy=-1,skirt=1)),
 ('M10 BOOKER','LT',0,0,330,370,70,34,95,1350,0,dict(L=23,W=18,tur='h',ts=(11,10),gl=12)),
 ('STRYKER DRAGOON','AC',0,0,390,380,14,14,90,1450,0,dict(L=24,W=16,wh=8,tur='r',ts=(9,8),gl=11,gt=1)),
 ('LAV-25','AC',0,0,410,400,12,12,80,1400,0,dict(L=23,W=16,wh=8,tur='r',ts=(9,8),gl=10,gt=1)),
 ('AAV/ACV AMPHIB','APC',0,0,360,370,6,6,95,1350,2,dict(L=24,W=17,wh=8,tur='mg',gl=8)),
 ('JLTV (TOW)','MRAP',0,0,440,460,8,6,85,1350,4,dict(L=19,W=15,wh=4,tur='mg',gl=9,atgm=1)),
 ('HUMVEE (M2)','JEEP',0,0,460,470,6,6,60,1300,0,dict(L=18,W=14,wh=4,jeep=1,tur='mg',gl=8)),
 ('MAXXPRO MRAP','MRAP',0,0,380,400,6,6,115,1300,0,dict(L=21,W=16,wh=4,tur='mg',gl=8)),
 ('M109A7 PALADIN','ARTY',0,0,230,280,120,60,110,1250,3,dict(L=26,W=19,tur='b',ts=(14,13),gl=16,mb=1)),
 ('M142 HIMARS','MLRS',0,0,400,400,60,40,80,1300,3,dict(L=25,W=16,wh=6,tur='n',cas=(10,6),cy=1,gl=0,pod=(10,10))),
 ('M270 MLRS','MLRS',0,0,270,300,50,40,95,1300,4,dict(L=25,W=19,tur='b',ts=(4,4),gl=1,pod=(14,12))),
 ('AMPV','APC',0,0,320,360,6,6,105,1300,2,dict(L=24,W=19,tur='mg',gl=8)),
 ('T-14 ARMATA','MT',1,0,300,330,64,46,140,1550,0,dict(L=27,W=20,tur='h',ts=(12,10),gl=16)),
 ('T-80BVM','MT',1,0,350,340,70,42,125,1450,0,dict(L=25,W=19,tur='r',ts=(12,11),gl=15,skirt=1)),
 ('T-72B3','MT',1,0,280,330,75,42,120,1400,0,dict(L=24,W=19,tur='r',ts=(12,11),gl=15)),
 ('BMP-3','IFV',1,0,320,370,14,12,95,1400,4,dict(L=23,W=18,tur='r',ts=(10,9),gl=12,gt=2,atgm=1,oy=1)),
 ('BTR-82A','AC',1,0,400,390,12,11,85,1400,0,dict(L=24,W=16,wh=8,tur='r',ts=(8,8),gl=11,gt=1)),
 ('2S19 MSTA-S','ARTY',1,0,220,270,125,64,115,1250,3,dict(L=26,W=20,tur='b',ts=(15,13),gl=17,mb=1)),
 ('TOS-1A','MLRS',1,0,230,280,70,46,120,1250,2,dict(L=24,W=19,tur='b',ts=(4,4),gl=1,pod=(14,14))),
 ('PANTSIR-S1','SPAA',1,0,370,380,6,10,90,1600,6,dict(L=24,W=16,wh=8,tur='b',ts=(12,9),gl=10,gt=1,twin=1)),
 ('LEOPARD 2A8','MT',0,0,300,330,62,46,140,1550,0,dict(L=26,W=20,tur='b',ts=(13,12),gl=16,skirt=1)),
 ('CHALLENGER 3','HT',0,0,260,300,66,48,150,1550,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=16)),
 ('LECLERC','MT',0,0,330,340,52,42,125,1500,0,dict(L=25,W=19,tur='b',ts=(13,11),gl=15)),
 ('MERKAVA MK4','HT',0,0,250,300,70,46,155,1500,0,dict(L=27,W=20,tur='h',ts=(14,10),gl=15,oy=2)),
 ('NAMER','APC',0,0,270,320,6,6,160,1300,2,dict(L=27,W=20,tur='mg',gl=8)),
 ('K2 BLACK PANTHER','MT',0,0,320,340,58,44,135,1550,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=15)),
 ('K21 KNIFV','IFV',0,0,330,380,12,11,95,1450,4,dict(L=24,W=18,tur='h',ts=(11,9),gl=12,gt=1,atgm=1)),
 ('TYPE 99A','MT',1,0,300,330,66,44,135,1500,0,dict(L=26,W=20,tur='h',ts=(13,12),gl=15)),
 ('ZTZ-15','LT',1,0,340,380,64,36,95,1400,0,dict(L=23,W=18,tur='h',ts=(11,10),gl=13)),
 ('ZBD-04','IFV',1,0,320,370,13,11,90,1400,4,dict(L=23,W=18,tur='r',ts=(10,9),gl=12,gt=2,atgm=1)),
 ('CV9035','IFV',0,0,340,380,16,13,95,1450,3,dict(L=24,W=18,tur='h',ts=(11,10),gl=13,gt=1,atgm=1)),
 ('PUMA IFV','IFV',0,0,320,370,11,11,110,1450,4,dict(L=24,W=19,tur='b',ts=(10,9),gl=12,gt=1,atgm=1)),
 ('BOXER','APC',0,0,400,380,6,6,110,1300,2,dict(L=25,W=17,wh=8,tur='mg',gl=8)),
 ('PATRIA AMV','APC',0,0,410,390,6,6,95,1300,2,dict(L=24,W=16,wh=8,tur='mg',gl=8)),
]
TK3=[('HOVER MBT','MT',0,0,350,420,55,34,105,1500,0,None),('RAIL WALKER','HT',0,0,220,300,80,60,140,2200,0,None)]
TK4=[('GRAV TANK','MT',0,0,320,420,50,30,100,1600,0,None),('LUNAR CRAWLER','HT',0,0,240,320,65,42,140,1600,0,None)]
# v1.3 eras 5-7: WW1 armour (generic tk styles) + 1776 artillery/wagons + medieval siege engines (custom top-down art)
TK5=[('MARK IV MALE','HT',0,0,110,180,110,40,160,1000,0,dict(L=28,W=22,tur='n',cas=(16,10),cy=9,gl=8,skirt=1)),
 ('MARK V','HT',0,0,130,200,100,40,165,1000,0,dict(L=28,W=22,tur='n',cas=(16,12),cy=8,gl=8,skirt=1)),
 ('A7V STURMPANZER','HT',1,0,120,170,110,44,170,1000,0,dict(L=27,W=20,tur='n',cas=(16,14),cy=6,gl=6)),
 ('RENAULT FT','LT',0,0,170,280,90,30,90,1050,0,dict(L=18,W=13,tur='c',ts=(8,8),gl=7)),
 ('WHIPPET MK A','MT',0,0,220,300,20,10,110,1300,0,dict(L=24,W=16,tur='n',cas=(12,8),cy=12,gl=4)),
 ('SCHNEIDER CA1','MT',0,0,140,220,100,36,120,1000,0,dict(L=22,W=17,tur='n',cas=(12,8),cy=4,gl=6)),
 ('ST CHAMOND','HT',0,0,130,200,100,44,140,1050,0,dict(L=28,W=18,tur='n',cas=(14,10),cy=3,gl=9)),
 ('LK II','LT',1,0,200,280,80,28,90,1050,0,dict(L=20,W=14,tur='n',cas=(10,8),cy=10,gl=6)),
 ('ROLLS-ROYCE AC','AC',0,0,300,340,14,10,80,1300,0,dict(L=22,W=14,wh=4,tur='c',ts=(9,9),gl=6)),
 ('EHRHARDT E-V/4','AC',1,0,280,320,14,10,90,1300,0,dict(L=23,W=15,wh=4,tur='c',ts=(9,8),gl=6))]
TK6=[('3-PDR GRASSHOPPER','TD',0,0,150,260,120,34,70,950,0,dict(custom='cannon',bar=8)),
 ('6-PDR FIELD GUN','TD',0,0,130,240,130,40,80,950,0,dict(custom='cannon',bar=10)),
 ('12-PDR FIELD GUN','TD',1,0,110,220,150,48,90,950,0,dict(custom='cannon',bar=12)),
 ('ROYAL HOWITZER','SPG',1,0,100,200,170,56,90,800,0,dict(custom='cannon',bar=7,fat=1)),
 ('SUPPLY WAGON','APC',0,0,200,260,40,8,100,1100,2,dict(custom='wagon'))]
TK7=[('BATTERING RAM','HT',0,0,110,200,90,60,170,700,0,dict(custom='ram')),
 ('RIBAULDEQUIN','TD',0,0,140,240,60,14,80,1000,0,dict(custom='cannon',bar=8,organ=1)),
 ('WAR WAGON','IFV',0,0,150,220,50,16,140,1100,4,dict(custom='wagon',war=1)),
 ('SIEGE TOWER','HT',1,0,90,160,70,20,200,900,0,dict(custom='tower')),
 ('TREBUCHET','SPG',1,0,80,160,200,70,90,700,0,dict(custom='treb')),
 ('BOMBARD CART','TD',1,0,110,220,160,58,90,850,0,dict(custom='cannon',bar=11,fat=1))]
def custom_tank(sp):
    k=sp['custom'];h=Cv(32,32);t=Cv(32,32);cx=16
    if k=='cannon':   # carriage seen from above: 2 big wheels, trail behind, barrel on the turret layer
        h.r(14,14,17,29,6 if False else 3);h.r(13,27,18,29,2)            # trail
        for xa in (7,22):h.r(xa,10,xa+2,20,12);[h.l(xa,y,xa+2,y,13) for y in range(11,20,3)]
        h.r(10,13,21,16,4);h.l(10,14,21,14,2)                             # axle
        bl=sp.get('bar',10);fat=sp.get('fat',0)
        if sp.get('organ'):
            for gx in (11,13,15,17,19):t.r(gx,14-bl,gx,15,14)
        else:t.r(15-fat,15-bl,16+fat,16,14);t.r(14-fat,15-bl,17+fat,16-bl,14);t.e(13-fat,13,18+fat,18,14)
    elif k=='wagon':
        h.r(8,6,23,27,3);h.r(9,7,22,26,4);[h.l(9,y,22,y,2) for y in range(9,26,4)]
        for xa,ya in ((5,8),(24,8),(5,21),(24,21)):h.r(xa,ya,xa+2,ya+4,12)
        h.l(15,0,15,5,2);h.l(16,0,16,5,2)                                 # tongue
        if sp.get('war'):[h.px(8,y,14) for y in range(10,26,5)];[h.px(23,y,14) for y in range(10,26,5)];t.r(15,8,16,16,14);t.e(13,14,18,19,2)
        else:t.e(10,9,21,24,15);t.l(12,12,19,12,4)
    elif k=='ram':
        h.p([(8,8),(23,8),(23,28),(8,28)],3);h.p([(16,6),(24,10),(24,27),(16,24),(7,27),(7,10)],4);h.l(16,6,16,24,2)
        for xa,ya in ((5,10),(24,10),(5,22),(24,22)):h.r(xa,ya,xa+2,ya+4,12)
        t.r(15,0,16,12,14);t.r(14,0,17,2,2)
    elif k=='tower':
        h.r(7,7,24,24,3);[h.l(7,y,24,y,2) for y in range(9,24,4)];[h.l(x,7,x,24,2) for x in (12,19)];h.r(9,9,22,22,4)
        for xa,ya in ((4,8),(25,8),(4,20),(25,20)):h.r(xa,ya,xa+2,ya+4,12)
        t.r(13,2,18,8,15);t.l(13,2,18,2,2)
    elif k=='treb':
        h.r(9,10,22,26,2);h.r(10,11,21,25,3);[h.r(xa,ya,xa+2,ya+3,12) for xa,ya in ((6,11),(23,11),(6,22),(23,22))]
        t.r(15,0,16,22,14);t.r(13,18,18,24,2);t.e(14,0,17,3,15)
    return h.grid(1),t.grid(1)
TKS=[TK0,TK1,TK2,TK3,TK4,TK5,TK6,TK7]
tank_t=[];gid=0;TKG={}
for e,lst in enumerate(TKS):
    for i,row in enumerate(lst):
        sp=row[11]
        if sp is None:hg,tg=tank(e,'hull'),tank(e,'tur')
        elif sp.get('custom'):hg,tg=custom_tank(sp)
        else:hg,tg=tk(sp)
        tank_t+=tiles4(hg,32,32)+tiles4(tg,32,32);TKG[(e,i)]=gid;gid+=1
emit("tank_spr",tank_t,"unsigned int")
# ---------------------------------------------------------------- aircraft sprite generator
def heli(sp):
    c=Cv(32,32);fw=sp.get('fw',4);
    c.r(15,12,16,29,3);c.r(12,28,19,29,3);c.r(13,26,14,30,2)            # tail boom + tail rotor
    c.e(16-fw//2-1,4,15+fw//2+1,17,3);c.e(16-fw//2,5,15+fw//2,9,10)     # cabin + canopy
    if fw>4:c.l(15,6,16,6,11)
    c.r(9,11,22,12,3);[c.px(x,13,14) for x in (9,10,21,22)]               # stub wings + pods
    if sp.get('twin'):  # coaxial: two discs
        c.l(3,3,28,20,2);c.l(28,3,3,20,2)
    else:c.l(2,4,29,19,2);c.l(29,4,2,19,2)
    c.e(14,10,17,13,7)
    return c.grid(1)
def pg(sp):
    if sp.get('rotor') and sp.get('nose')=='glass':return heli(sp)
    if sp.get('balloon') or sp.get('orni') or sp.get('screw'):
        c=Cv(32,32)
        if sp.get('balloon'):c.e(5,2,26,24,3);c.e(8,4,20,14,4);[c.l(x,3,x,23,2) for x in (11,16,21)];c.r(13,25,18,29,7);c.l(12,22,13,25,14);c.l(19,22,18,25,14);(c.r(14,26,17,28,9) if sp['balloon']==2 else None)
        elif sp.get('orni'):c.p([(16,8),(1,4),(3,12),(8,16),(16,14)],3);c.p([(16,8),(30,4),(28,12),(23,16),(16,14)],3);[c.l(16,9,x,y,2) for x,y in ((2,5),(5,13),(29,5),(26,13))];c.e(14,6,17,24,7);c.p([(16,22),(12,29),(20,29)],4)
        else:c.e(3,3,28,28,4);c.e(6,6,25,25,3);c.l(16,3,16,28,2);c.l(3,16,28,16,2);c.e(12,12,19,19,7);c.e(14,14,17,17,14)
        return c.grid(1)
    c=Cv(32,32);k=sp.get('k','sef');span=sp.get('span',26);ch=sp.get('ch',5);wy=sp.get('wy',11);sw=sp.get('sw',0);tp=sp.get('tp',0.6)
    fl=sp.get('fl',24);fw=sp.get('fw',4);fy0=sp.get('fy',3);fy1=fy0+fl-1
    hx=span//2
    def L(pts,col):c.p(pts,col);c.p([(31-x,y) for x,y in pts],col)
    if sp.get('disc'):c.e(4,4,27,25,3);c.e(8,7,23,13,4)
    elif sp.get('wing')=='fly':  # flying wing
        L([(16,fy0),(16-hx,fy0+sw),(16-hx,fy0+sw+3),(16-hx+4,fy0+sw+4),(16,fy0+ch+sw//2)],3)
    elif sp.get('bi'):
        for yy in (wy-3,wy+3):c.r(16-hx,yy,15+hx,yy+ch-2,3)
        for xs in (6,25):c.l(xs,wy-1,xs,wy+3,2)
    else:
        rx=16-fw//2;tx=16-hx
        if sp.get('fsw'):pts=[(rx,wy+ch),(tx,wy+ch-sw),(tx,wy+ch-sw-max(2,int(ch*tp))),(rx,wy)]
        elif sp.get('gull'):pts=[(rx,wy),(rx-5,wy+2),(tx,wy+sw),(tx,wy+sw+max(2,int(ch*tp))),(rx-5,wy+ch+1),(rx,wy+ch)]
        elif sp.get('wing')=='ell':
            c.e(tx,wy,31-tx,wy+ch,3);pts=None
        else:pts=[(rx,wy),(tx,wy+sw),(tx,wy+sw+max(2,int(ch*tp))),(rx,wy+ch)]
        if pts:L(pts,3)
    if sp.get('canard'):L([(16-fw//2,fy0+3),(9,fy0+5),(9,fy0+6),(16-fw//2,fy0+6)],3)
    # tail
    if not sp.get('notail') and not sp.get('disc'):
        ts=sp.get('ts',10);ty=sp.get('ty',fy1-3)
        if sp.get('boom'):
            bx=sp['boom'];c.r(16-bx-1,wy,16-bx,ty+2,3);c.r(15+bx,wy,16+bx,ty+2,3);c.r(16-bx-1,ty,16+bx,ty+2,3)
        else:L([(16,ty),(16-ts//2,ty+1),(16-ts//2,ty+3),(16,ty+3)],3)
        if sp.get('twin'):c.r(16-ts//2,ty-1,16-ts//2,ty+3,2);c.r(15+ts//2,ty-1,15+ts//2,ty+3,2)
        if sp.get('ttail'):c.r(15,ty-3,16,ty+2,2)
    # fuselage
    if not sp.get('disc'):
        if sp.get('hull'):c.e(16-fw//2-1,fy0,15+fw//2+1,fy1,3);c.l(15,fy0+2,15,fy1-2,2)
        else:c.e(16-fw//2,fy0,15+fw//2,fy0+6,3);c.r(16-fw//2,fy0+3,15+fw//2,fy1-2,3);c.e(15,fy1-4,16,fy1,3)
    nose=sp.get('nose','inl')
    if nose=='rad':c.e(16-fw//2-1,fy0-1,15+fw//2+1,fy0+4,3);c.e(16-fw//2,fy0,15+fw//2,fy0+3,7);c.e(15,fy0+1,16,fy0+2,14)
    elif nose=='glass':c.e(16-fw//2,fy0,15+fw//2,fy0+4,10);c.l(15,fy0+1,16,fy0+1,11)
    elif nose=='jet':c.p([(15,fy0-1),(16,fy0-1),(15+fw//2,fy0+4),(16-fw//2,fy0+4)],3);c.px(15,fy0,2);c.px(16,fy0,2)
    elif nose=='int':c.e(16-fw//2,fy0,15+fw//2,fy0+3,7)
    # engines
    for ex in sp.get('eng',[]):
        jet=sp.get('jeteng');x=16-ex
        for xx in (x,31-x):
            if jet:c.e(xx-2,wy-3,xx+1,wy+ch+2,2);c.px(xx-1,wy-3,7);c.px(xx,wy-3,7);c.px(xx-1,wy+ch+2,15);c.px(xx,wy+ch+2,15)
            else:c.e(xx-2,wy-3,xx+1,wy+ch,3);c.e(xx-1,wy-3,xx,wy-2,7);c.l(xx-4,wy-4,xx+3,wy-4,14)
    if sp.get('topjet'):c.e(14,fy1-11,17,fy1-4,2);c.px(15,fy1-4,15);c.px(16,fy1-4,15)
    if sp.get('pusher'):c.l(10,fy1+1,21,fy1+1,14);c.px(15,fy1,7);c.px(16,fy1,7)
    if sp.get('rotor'):
        c.l(16,4,16,28,3,2);c.l(4,16,28,16,3,2);c.e(13,13,18,18,2)
    # canopy
    cy=sp.get('cy',fy0+6)
    if sp.get('cy',1)!=-1 and not sp.get('rotor'):
        if sp.get('disc'):c.e(14,7,17,10,10)
        else:c.e(15-(fw>4),cy,16+(fw>4),cy+3,10)
    if nose not in ('jet','glass','int') and not sp.get('noprop') and not sp.get('pusher') and not sp.get('jeteng') and not sp.get('eng') and not sp.get('rotor'):c.l(16-6,fy0-2,15+6,fy0-2,14);c.px(15,fy0-2,7);c.px(16,fy0-2,7)
    if nose=='glass' and not sp.get('eng'):c.l(10,fy0-2,21,fy0-2,14)
    st=sp.get('st')
    if st=='torp':c.r(15,wy+1,16,wy+ch+5,14);c.px(15,wy+ch+5,7)
    if st=='bomb':c.px(10,wy+ch-1,7);c.px(21,wy+ch-1,7)
    if st=='rk':[c.px(x,wy+ch,14) for x in (7,9,22,24)]
    if sp.get('wing') not in ('fly',) and not sp.get('disc') and not sp.get('bi') and not sp.get('rotor'):
        mx=max(4,16-hx+3);c.px(mx,wy+2+sw//2,9);c.px(31-mx,wy+2+sw//2,9)
    if sp.get('asym'):  # BV 141: crew pod to the right of the main boom
        for y in range(32):
            for x in range(16,32):c.px(x,y,c.get(31-x,y))
        c.r(14,0,17,31,0);c.r(14,wy,17,wy+ch,3)
        c.e(19,4,23,17,3);c.e(20,5,22,9,10);c.e(9,2,13,26,3);c.l(6,1,16,1,14);c.r(7,24,14,26,3)
    return c.grid(1)
# class baselines: spd,turn,rate,dmg,bspd,ammo,hp,sec,gunner
CB={'sef':(380,580,4,11,1100,4,100,0,0),'tef':(390,440,4,14,1150,4,150,0,1),'jet':(560,380,5,15,1250,8,95,0,0),'db':(320,470,5,9,1050,4,140,2,1),
    'tb':(300,410,6,8,1050,2,160,3,1),
    'jf':(520,540,4,10,1400,6,105,1,0),'js':(470,430,5,12,1300,6,140,2,0),'jb':(430,300,5,10,1300,12,320,2,1),'cas':(330,420,8,16,1150,10,240,0,0),
    'heli':(240,760,6,11,1050,8,150,0,0),'gun':(300,300,9,13,1100,0,360,0,2),'mb':(320,340,5,9,1100,8,220,2,1),'hb':(280,270,5,8,1050,12,300,2,2),'fb':(250,300,6,7,1050,4,260,2,2)}
SECN={0:'ROCKETS',1:'MISSILES',2:'BOMBS',3:'TORPEDO'}
def A(name,cls,side,d=(0,0,0,0),mn=None,ammo=None,sec=None,exp=0,fuel=0,sp=None,gun=None,rate=None,mt=None):
    b=CB[cls];s=sec if sec is not None else b[7]
    return dict(n=name,mn=mn or SECN[s],spd=b[0]+d[0],turn=b[1]+d[1],rate=rate or b[2],dmg=b[3]+d[3],bspd=b[4],ammo=b[5] if ammo is None else ammo,mturn=(mt if mt is not None else (420 if s==1 else 0)),hp=b[6]+d[2],side=side,sec=s,
                gunner=b[8] if gun is None else gun,exp=exp,fuel=fuel,sp=sp,cls=cls)
# original 14 (sprites hand-drawn in gen_assets.ww2plane); order kept for the campaign
OLD=[('P-51D MUSTANG','HVAR ROCKETS',410,560,4,10,1150,6,0,100,0,0,0,0),('SPITFIRE IX','RP-3 ROCKETS',380,640,4,9,1100,4,0,90,0,0,1,0),
 ('F6F HELLCAT','HVAR ROCKETS',360,540,4,11,1100,6,0,140,0,0,2,0),('P-47 THUNDERBOLT','HVAR ROCKETS',380,470,3,11,1150,8,0,160,0,0,3,0),
 ('F4U CORSAIR','HVAR ROCKETS',400,520,4,11,1150,8,0,130,0,0,4,0),('YAK-3','RS-82 ROCKETS',400,660,5,11,1100,2,0,85,0,0,5,0),
 ('HURRICANE IIC','RP-3 ROCKETS',340,600,4,10,1050,4,0,110,0,0,6,0),('P-38 LIGHTNING','HVAR ROCKETS',420,450,3,12,1200,10,0,120,0,0,7,0),
 ('BF 109 G','WGR 21 ROCKETS',400,560,5,12,1150,2,0,90,1,0,8,0),('FW 190 A','WGR 21 ROCKETS',400,540,4,13,1150,4,0,120,1,0,9,0),
 ('ME 262','R4M ROCKETS',580,360,5,16,1250,12,0,100,1,0,10,0),('A6M ZERO','TYPE 3 ROCKETS',350,700,5,11,1050,2,0,70,1,0,11,0),
 ('JU 87 STUKA','SC250 BOMBS',300,420,6,8,1050,4,0,130,1,2,12,1),('SBD DAUNTLESS','1000LB BOMBS',330,480,5,9,1050,6,0,150,0,2,13,1)]
OLDCLS=['sef','sef','sef','sef','sef','sef','sef','tef','sef','sef','jet','sef','db','db']
W0=[]
for i,o in enumerate(OLD):
    W0.append(dict(n=o[0],mn=o[1],spd=o[2],turn=o[3],rate=o[4],dmg=o[5],bspd=o[6],ammo=o[7],mturn=o[8],hp=o[9],side=o[10],sec=o[11],gunner=o[13],exp=0,fuel=0,sp=('old',WW2_PLANES[i]),cls=OLDCLS[i]))
S=dict  # sprite spec shorthand
W0+=[
 A('B-17 FLYING FORT.','hb',0,(0,0,40,0),'500LB BOMBS',sp=S(k='hb',span=32,ch=6,wy=10,fl=28,fy=2,fw=4,eng=[5,10],nose='glass',ts=14,tp=.5)),
 A('B-25 MITCHELL','mb',0,(20,30,-10,2),'500LB BOMBS',6,sp=S(span=30,ch=5,wy=11,fl=24,fy=4,eng=[7],nose='glass',twin=1,ts=12)),
 A('P-40 WARHAWK','sef',0,(-10,-30,15,0),'500LB BOMB',1,2,sp=S(span=26,ch=5,wy=11,fl=24,nose='int',ts=10)),
 A('P-39 AIRACOBRA','sef',0,(0,-20,-5,4),'M4 37MM SHELLS',rate=6,sp=S(span=24,ch=4,wy=12,fl=24,fw=4,ts=9)),
 A('F4F WILDCAT','sef',0,(-50,20,20,-1),'100LB BOMBS',2,2,sp=S(span=26,ch=5,wy=11,fl=21,fy=4,fw=6,nose='rad',wing='sq',tp=1,ts=10)),
 A('TBF AVENGER','tb',0,(10,0,20,1),'MK13 TORPEDO',sp=S(span=30,ch=6,wy=11,fl=26,fw=6,nose='rad',st='torp',ts=12,tp=.8)),
 A('MOSQUITO','tef',0,(60,10,-40,-1),'500LB BOMBS',4,2,0,sp=S(span=28,ch=5,wy=11,fl=24,fy=4,eng=[6],nose='int',ts=10)),
 A('TYPHOON IB','sef',0,(40,-130,30,3),'RP-3 ROCKETS',8,sp=S(span=28,ch=6,wy=11,fl=24,fw=5,nose='int',ts=10,st='rk')),
 A('LANCASTER','hb',0,(0,10,-20,-1),'4000LB COOKIE',14,sp=S(k='hb',span=32,ch=6,wy=11,fl=26,fy=3,eng=[5,10],nose='glass',twin=1,ts=14)),
 A('FW 190 D-9','sef',1,(50,-40,15,2),'WGR 21 ROCKETS',2,sp=S(span=24,ch=5,wy=12,fl=27,fy=2,nose='int',ts=9)),
 A('BF 110','tef',1,(-10,-20,0,-1),'WGR 21 ROCKETS',4,sp=S(span=28,ch=5,wy=11,fl=24,fy=4,eng=[7],nose='glass',twin=1,ts=12)),
 A('JU 88','mb',1,(20,40,-20,0),'SC500 BOMBS',8,sp=S(span=30,ch=5,wy=11,fl=24,fy=4,eng=[7],nose='glass',ts=12)),
 A('HE 111','mb',1,(-20,-20,20,-1),'SC250 BOMBS',10,gun=2,sp=S(span=32,ch=6,wy=11,fl=24,fy=4,eng=[8],nose='glass',wing='ell',ts=12)),
 A('ME 163 KOMET','jet',1,(120,-50,-30,1),'NONE',0,exp=1,fuel=25,sp=S(k='jet',wing='fly',span=24,ch=8,sw=10,fl=14,fy=8,fw=4,nose='int',notail=1,noprop=1)),
 A('IL-2 STURMOVIK','sef',0,(-60,-160,120,2),'RS-82 ROCKETS',8,gun=1,sp=S(span=28,ch=5,wy=12,fl=24,nose='int',ts=10,st='rk')),
 A('LA-5FN','sef',0,(20,60,-5,1),'RS-82 ROCKETS',2,sp=S(span=24,ch=5,wy=11,fl=23,fw=5,nose='rad',ts=9)),
 A('LA-7','sef',0,(40,80,-5,1),'RS-82 ROCKETS',2,sp=S(span=24,ch=5,wy=11,fl=23,fw=5,nose='rad',ts=9,tp=.5)),
 A('KI-43 OSCAR','sef',1,(-40,160,-35,-3),'30KG BOMBS',2,2,sp=S(span=28,ch=4,wy=11,fl=22,fw=5,nose='rad',ts=9,tp=.5)),
 A('KI-84 HAYATE','sef',1,(40,40,-5,1),'250KG BOMB',1,2,sp=S(span=26,ch=5,wy=11,fl=24,fw=5,nose='rad',ts=10)),
 A('N1K2 GEORGE','sef',1,(20,60,5,2),'TYPE 3 ROCKETS',2,sp=S(span=26,ch=5,wy=12,fl=24,fw=5,nose='rad',ts=10)),
 A('B5N KATE','tb',1,(0,10,-50,-1),'TYPE 91 TORPEDO',sp=S(span=30,ch=5,wy=11,fl=25,fw=5,nose='rad',st='torp',ts=11)),
 A('D3A VAL','db',1,(-10,10,-30,-1),'250KG BOMBS',3,sp=S(span=28,ch=5,wy=11,fl=24,fw=5,nose='rad',wing='ell',ts=10)),
 # --- additional list
 A('P-61 BLACK WIDOW','tef',0,(-20,-40,40,1),'HVAR ROCKETS',4,gun=1,sp=S(span=32,ch=6,wy=11,fl=16,fy=3,fw=6,eng=[7],nose='int',boom=7,ty=26,ts=16)),
 A('P-63 KINGCOBRA','sef',0,(30,-10,0,4),'M4 37MM SHELLS',rate=6,sp=S(span=26,ch=4,wy=12,fl=24,ts=9)),
 A('F2A BUFFALO','sef',0,(-60,0,0,-2),'100LB BOMBS',2,2,sp=S(span=24,ch=5,wy=11,fl=20,fy=5,fw=7,nose='rad',ts=9)),
 A('F8F BEARCAT','sef',0,(60,90,0,1),'HVAR ROCKETS',4,sp=S(span=24,ch=5,wy=11,fl=21,fy=4,fw=6,nose='rad',ts=9)),
 A('P-80 SHOOTING STAR','jet',0,(-30,40,0,-3),'HVAR ROCKETS',6,exp=1,sp=S(span=28,ch=5,wy=13,fl=26,fy=2,nose='jet',ts=10,cy=6)),
 A('B-24 LIBERATOR','hb',0,(20,0,-20,0),'500LB BOMBS',12,sp=S(k='hb',span=32,ch=4,wy=11,fl=26,fy=3,eng=[5,10],nose='glass',twin=1,ts=12,tp=.7)),
 A('B-26 MARAUDER','mb',0,(40,0,-20,1),'500LB BOMBS',8,sp=S(span=26,ch=5,wy=11,fl=26,fy=3,eng=[7],nose='glass',ts=11)),
 A('B-29 SUPERFORT.','hb',0,(60,10,20,1),'500LB BOMBS',16,sp=S(k='hb',span=32,ch=4,wy=12,fl=29,fy=1,eng=[5,10],nose='glass',ts=12,tp=.5)),
 A('A-20 HAVOC','mb',0,(60,60,-60,1),'500LB BOMBS',4,sp=S(span=26,ch=5,wy=11,fl=24,fy=4,eng=[6],nose='glass',ts=11)),
 A('PBY CATALINA','fb',0,(-10,0,0,0),'DEPTH CHARGES',4,sp=S(span=32,ch=4,wy=9,fl=24,fy=4,fw=6,eng=[5],hull=1,nose='glass',ts=12)),
 A('SB2C HELLDIVER','db',0,(20,-10,10,1),'1000LB BOMBS',4,sp=S(span=28,ch=6,wy=11,fl=24,fw=5,nose='rad',ts=11)),
 A('BEAUFIGHTER','tef',0,(-40,-30,30,2),'RP-3 ROCKETS',8,gun=0,sp=S(span=28,ch=5,wy=10,fl=20,fy=6,fw=5,eng=[6],nose='int',ts=11,st='rk')),
 A('TEMPEST V','sef',0,(60,-60,20,2),'RP-3 ROCKETS',8,sp=S(span=28,ch=6,wy=11,fl=25,fw=5,nose='int',wing='ell',ts=10)),
 A('HALIFAX','hb',0,(0,0,-10,0),'1000LB BOMBS',12,sp=S(k='hb',span=32,ch=6,wy=11,fl=26,fy=3,eng=[5,10],nose='glass',twin=1,ts=13,tp=.8)),
 A('WELLINGTON','mb',0,(-40,-20,40,-1),'500LB BOMBS',8,gun=2,sp=S(span=32,ch=5,wy=11,fl=26,fy=3,eng=[8],nose='glass',ts=12,tp=.4)),
 A('SWORDFISH','tb',0,(-110,160,-30,-2),'MK XII TORPEDO',sp=S(bi=1,span=26,ch=5,wy=11,fl=22,fy=4,fw=4,nose='rad',st='torp',ts=10)),
 A('FAIREY FIREFLY','sef',0,(-30,-40,30,1),'RP-3 ROCKETS',6,gun=1,sp=S(span=28,ch=5,wy=12,fl=26,fy=3,nose='int',ts=10)),
 A('GLOSTER METEOR','jet',0,(-20,-20,20,0),'NONE',0,sp=S(span=30,ch=5,wy=12,fl=26,fy=2,eng=[7],jeteng=1,nose='jet',ts=10,ttail=1)),
 A('BF 109 K-4','sef',1,(40,-30,0,2),'WGR 21 ROCKETS',2,sp=S(span=24,ch=4,wy=12,fl=24,ts=8)),
 A('TA 152 H','sef',1,(70,-40,10,3),'WGR 21 ROCKETS',2,sp=S(span=30,ch=4,wy=12,fl=28,fy=1,nose='int',ts=9,tp=.5)),
 A('ME 410','tef',1,(10,-30,0,1),'SC500 BOMBS',4,2,sp=S(span=28,ch=5,wy=11,fl=22,fy=5,eng=[7],nose='glass',ts=12)),
 A('DO 335 PFEIL','tef',1,(110,-40,-20,2),'SC500 BOMB',2,2,gun=0,exp=1,sp=S(span=26,ch=5,wy=11,fl=27,fy=2,nose='int',pusher=1,ts=10,ttail=1)),
 A('AR 234 BLITZ','jet',1,(-20,-60,40,-6),'SC500 BOMBS',4,2,exp=1,sp=S(span=28,ch=5,wy=12,fl=26,fy=2,eng=[7],jeteng=1,nose='glass',ts=10,cy=-1)),
 A('HE 162 SALAMANDER','jet',1,(-30,20,-25,-2),'NONE',0,exp=1,fuel=60,sp=S(span=20,ch=5,wy=13,fl=22,fy=3,topjet=1,nose='jet',twin=1,ts=10)),
 A('JU 87 G KANONEN','db',1,(-30,-40,10,16),'BK 37 SHELLS',8,0,rate=8,sp=S(span=28,ch=5,wy=12,fl=24,gull=1,sw=0,nose='int',ts=10)),
 A('YAK-9','sef',0,(10,60,-10,0),'RS-82 ROCKETS',2,sp=S(span=24,ch=5,wy=12,fl=24,ts=9)),
 A('YAK-1','sef',0,(-30,40,-10,-1),'RS-82 ROCKETS',2,sp=S(span=24,ch=5,wy=12,fl=23,ts=9,tp=.5)),
 A('MIG-3','sef',0,(30,-60,-15,-1),'RS-82 ROCKETS',2,sp=S(span=24,ch=5,wy=13,fl=26,fy=2,ts=9)),
 A('LAGG-3','sef',0,(-30,-40,10,0),'RS-82 ROCKETS',4,sp=S(span=24,ch=5,wy=12,fl=24,ts=9,tp=.7)),
 A('PE-2','mb',0,(80,60,-50,1),'FAB-250 BOMBS',4,sp=S(span=28,ch=4,wy=11,fl=24,fy=4,eng=[7],nose='glass',twin=1,ts=11)),
 A('TU-2','mb',0,(50,20,-20,1),'FAB-500 BOMBS',6,sp=S(span=28,ch=5,wy=11,fl=25,fy=3,eng=[7],nose='glass',twin=1,ts=12)),
 A('IL-10','sef',0,(0,-120,110,2),'RS-82 ROCKETS',8,gun=1,sp=S(span=26,ch=5,wy=12,fl=24,nose='int',ts=10,st='rk')),
 A('P-39 (VVS)','sef',0,(-10,10,0,4),'RS-82 ROCKETS',2,rate=6,sp=S(span=24,ch=4,wy=12,fl=24,ts=9)),
 A('KI-61 HIEN','sef',1,(10,0,0,1),'250KG BOMB',1,2,sp=S(span=28,ch=4,wy=12,fl=25,ts=9,tp=.5)),
 A('KI-100','sef',1,(10,60,0,1),'250KG BOMB',1,2,sp=S(span=28,ch=4,wy=11,fl=24,fw=5,nose='rad',ts=9,tp=.5)),
 A('J2M RAIDEN','sef',1,(30,-60,10,2),'TYPE 3 ROCKETS',2,sp=S(span=24,ch=5,wy=12,fl=25,fy=2,fw=6,nose='rad',ts=9)),
 A('A6M5 ZERO','sef',1,(10,120,-25,0),'60KG BOMBS',2,2,sp=S(span=26,ch=5,wy=12,fl=24,fw=5,nose='rad',ts=10,tp=.7)),
 A('G4M BETTY','mb',1,(0,-20,-70,-1),'TYPE 91 TORPEDO',1,3,gun=2,sp=S(span=32,ch=5,wy=11,fl=26,fy=3,fw=6,eng=[8],nose='glass',ts=12,st='torp')),
 A('KI-21 SALLY','mb',1,(-20,-20,0,-1),'50KG BOMBS',10,sp=S(span=30,ch=5,wy=11,fl=24,fy=4,eng=[7],nose='glass',ts=12)),
 A('H8K EMILY','fb',1,(40,0,40,1),'250KG BOMBS',8,sp=S(k='hb',span=32,ch=4,wy=9,fl=28,fy=2,fw=6,eng=[5,10],hull=1,nose='glass',ts=12)),
 A('MC.202 FOLGORE','sef',1,(10,40,-15,-1),'50KG BOMBS',2,2,sp=S(span=24,ch=5,wy=12,fl=24,ts=9)),
 A('MC.205 VELTRO','sef',1,(30,30,-10,1),'50KG BOMBS',2,2,sp=S(span=24,ch=5,wy=12,fl=24,ts=9,tp=.5)),
 A('FIAT G.55','sef',1,(20,10,0,2),'50KG BOMBS',2,2,sp=S(span=26,ch=5,wy=12,fl=24,ts=10)),
 A('RE.2005','sef',1,(20,50,-10,1),'50KG BOMBS',2,2,sp=S(span=26,ch=5,wy=12,fl=23,ts=9,wing='ell')),
 # --- experimental (Luft '46 / prototypes)
 A('HO 229','jet',1,(60,-20,-20,2),'R4M ROCKETS',8,exp=1,fuel=0,sp=S(wing='fly',span=32,ch=7,sw=8,fl=10,fy=8,fw=6,nose='int',notail=1,noprop=1,eng=[3],jeteng=1)),
 A('ME 262 HG III','jet',1,(90,-30,-10,1),'R4M ROCKETS',12,exp=1,sp=S(span=28,ch=5,wy=11,sw=8,fl=26,fy=2,eng=[4],jeteng=1,nose='jet',ts=10)),
 A('TA 183 HUCKEBEIN','jet',1,(80,40,-20,0),'R4M ROCKETS',6,exp=1,sp=S(span=26,ch=6,wy=12,sw=9,tp=.5,fl=22,fy=3,nose='int',ttail=1,ts=10)),
 A('LIPPISCH P.13A','jet',1,(100,-80,-30,-1),'NONE',0,exp=1,fuel=40,sp=S(wing='fly',span=28,ch=18,sw=18,fl=20,fy=4,fw=4,nose='int',notail=1,noprop=1)),
 A('BA 349 NATTER','jet',1,(160,-180,-60,0),'HS 217 ROCKETS',24,exp=1,fuel=15,sp=S(span=14,ch=4,wy=14,fl=24,fy=4,fw=4,nose='int',noprop=1,ts=8,cy=-1)),
 A('AR 234 C','jet',1,(30,-50,30,-5),'SC500 BOMBS',6,2,exp=1,sp=S(span=28,ch=5,wy=12,fl=26,fy=2,eng=[6,9],jeteng=1,nose='glass',ts=10)),
 A('JU 287','jet',1,(-120,-180,150,-8),'SC1000 BOMBS',8,2,gun=1,exp=1,sp=S(fsw=1,span=32,ch=6,wy=13,sw=7,fl=28,fy=1,fw=4,eng=[5,10],jeteng=1,nose='glass',ts=12)),
 A('TRIEBFLUGEL','jet',1,(-80,140,-40,1),'NONE',0,exp=1,fuel=35,sp=S(rotor=1,fl=26,fy=3,fw=4,nose='jet',ts=10)),
 A('BV 141','sef',1,(-80,-60,20,-2),'50KG BOMBS',4,2,gun=1,exp=1,sp=S(asym=1,span=30,ch=5,wy=12,fl=24)),
 A('ME 264 AMERIKA','hb',1,(20,-10,10,0),'SC1000 BOMBS',14,exp=1,sp=S(k='hb',span=32,ch=4,wy=11,fl=28,fy=2,eng=[5,10],nose='glass',ts=12,tp=.5)),
 A('XP-55 ASCENDER','sef',0,(30,-60,-20,1),'HVAR ROCKETS',2,exp=1,sp=S(span=26,ch=7,wy=16,sw=6,fl=22,fy=4,fw=4,nose='int',canard=1,pusher=1,notail=1)),
 A('XF5U FLAPJACK','sef',0,(-20,180,40,0),'HVAR ROCKETS',4,exp=1,sp=S(disc=1,eng=[12])),
 A('NORTHROP XP-79','jet',0,(40,60,-40,-2),'NONE',0,exp=1,fuel=50,sp=S(wing='fly',span=30,ch=7,sw=9,fl=10,fy=8,fw=4,nose='int',notail=1,noprop=1,eng=[4],jeteng=1)),
]

# v1.2: Cold War expansion (side 0 = NATO/West, 1 = Warsaw Pact/East)
J=dict(nose='jet',noprop=1)
CW_ADD=[
 A('F-100 SUPER SABRE','jf',0,(-60,-60,0,1),'AIM-9B',2,mt=300,sp=S(span=24,ch=6,wy=13,sw=7,tp=.5,fl=28,fy=1,fw=4,nose='int',ts=10,noprop=1)),
 A('F-104 STARFIGHTER','jf',0,(80,-200,-25,1),'AIM-9B',2,mt=300,sp=S(span=14,ch=4,wy=14,sw=1,fl=30,fy=0,fw=4,ttail=1,ts=10,**J)),
 A('F-105 THUNDERCHIEF','js',0,(30,-40,20,2),'M117 BOMBS',8,sp=S(span=20,ch=6,wy=14,sw=7,tp=.5,fl=30,fy=0,fw=5,ts=10,**J)),
 A('F-111 AARDVARK','jb',0,(80,40,-120,0),'MK 82 BOMBS',10,gun=0,sp=S(span=28,ch=5,wy=12,sw=8,tp=.6,fl=30,fy=0,fw=6,ts=12,**J)),
 A('A-4 SKYHAWK','js',0,(-80,90,-30,0),'ZUNI ROCKETS',8,0,sp=S(span=20,ch=9,wy=13,sw=8,tp=.3,fl=24,fy=3,fw=4,ts=9,**J)),
 A('F-8 CRUSADER','jf',0,(-30,40,0,2),'AIM-9 SIDEWINDER',4,mt=330,sp=S(span=24,ch=6,wy=11,sw=8,tp=.5,fl=29,fy=0,fw=4,nose='int',ts=10,noprop=1)),
 A('MIRAGE IIIC','jf',0,(30,0,-15,1),'R.530 AAM',2,mt=320,sp=S(span=22,ch=14,wy=12,sw=12,tp=.12,fl=28,fy=1,fw=4,notail=1,**J)),
 A('AH-1G COBRA','heli',0,(0,0,0,0),'2.75IN ROCKETS',14,0,sp=S(rotor=1,fl=28,fy=2,fw=4,nose='glass',ts=8)),
 A('MIG-17 FRESCO','jf',1,(-120,120,-10,4),'S-5 ROCKETS',8,0,sp=S(span=22,ch=7,wy=11,sw=7,tp=.5,fl=24,fy=3,fw=5,nose='int',ts=9,ttail=1,noprop=1)),
 A('MIG-19 FARMER','jf',1,(-40,60,-5,4),'K-13 AAM',2,mt=300,sp=S(span=22,ch=7,wy=12,sw=9,tp=.4,fl=26,fy=2,fw=5,nose='int',ts=9,noprop=1)),
 A('MIG-23 FLOGGER','jf',1,(40,-60,10,1),'R-23 AAM',4,mt=380,sp=S(span=24,ch=6,wy=12,sw=9,tp=.6,fl=29,fy=0,fw=5,ts=11,**J)),
 A('MIG-25 FOXBAT','jf',1,(130,-240,40,0),'R-40 AAM',4,mt=300,sp=S(span=24,ch=8,wy=15,sw=6,tp=.5,fl=29,fy=1,fw=8,twin=1,ts=12,**J)),
 A('SU-7 FITTER','js',1,(10,-60,0,2),'FAB-250 BOMBS',6,sp=S(span=20,ch=7,wy=13,sw=10,tp=.4,fl=30,fy=0,fw=5,nose='int',ts=10,noprop=1)),
 A('MI-24 HIND','heli',1,(40,-160,90,2),'S-8 ROCKETS',16,0,sp=S(rotor=1,fl=29,fy=1,fw=6,nose='glass',ts=10)),
]
# v1.2: modern expansion (side 0 = West/partners, 1 = East)
MD_ADD=[
 A('F-16C VIPER','jf',0,(0,60,-10,0),'AIM-120 AMRAAM',6,mt=520,sp=S(span=20,ch=7,wy=14,sw=5,tp=.4,fl=28,fy=1,fw=4,ts=10,**J)),
 A('F-15E STRIKE EAGLE','js',0,(70,40,10,0),'GBU-31 JDAM',8,sp=S(span=26,ch=9,wy=13,sw=6,tp=.4,fl=29,fy=0,fw=7,twin=1,ts=12,**J)),
 A('F-15EX EAGLE II','jf',0,(40,-10,40,1),'AIM-120D',12,mt=540,sp=S(span=26,ch=9,wy=13,sw=6,tp=.4,fl=29,fy=0,fw=7,twin=1,ts=12,**J)),
 A('EA-18G GROWLER','jf',0,(-30,-20,10,-2),'AGM-88 HARM',4,mt=450,sp=S(span=24,ch=7,wy=13,sw=4,fl=28,fy=1,fw=6,twin=1,ts=12,**J)),
 A('AV-8B HARRIER II','js',0,(-120,140,-30,-1),'CRV7 ROCKETS',10,0,sp=S(span=18,ch=7,wy=13,sw=4,fl=26,fy=2,fw=6,ts=9,**J)),
 A('A-10C WARTHOG','cas',0,(0,0,60,6),'HYDRA ROCKETS',14,rate=10,sp=S(span=30,ch=5,wy=12,fl=24,fy=3,fw=4,nose='int',noprop=1,twin=1,ts=12,topjet=1)),
 A('B-1B LANCER','jb',0,(70,40,-20,0),'MK 84 BOMBS',16,gun=0,sp=S(k='hb',span=30,ch=6,wy=12,sw=9,tp=.6,fl=31,fy=0,fw=6,ts=12,eng=[4],jeteng=1,**J)),
 A('B-2 SPIRIT','jb',0,(-20,0,30,0),'GBU-57 BOMBS',8,gun=0,sp=S(wing='fly',span=32,ch=8,sw=8,fl=10,fy=8,fw=6,nose='int',notail=1,noprop=1)),
 A('AC-130J GHOSTRIDER','gun',0,(0,0,0,0),'NONE',0,sp=S(k='hb',span=32,ch=5,wy=11,fl=27,fy=2,fw=6,eng=[6,11],nose='int',ts=13)),
 A('GRIPEN E','jf',0,(10,90,-25,-1),'METEOR AAM',6,mt=560,sp=S(span=20,ch=10,wy=15,sw=8,tp=.3,fl=26,fy=2,fw=4,canard=1,notail=1,**J)),
 A('MIRAGE 2000-5','jf',0,(20,30,-15,1),'MICA AAM',4,mt=500,sp=S(span=22,ch=14,wy=12,sw=12,tp=.12,fl=27,fy=1,fw=5,notail=1,**J)),
 A('KF-21 BORAMAE','jf',0,(20,10,10,0),'METEOR AAM',6,mt=540,sp=S(span=24,ch=8,wy=13,sw=6,tp=.4,fl=28,fy=1,fw=7,twin=1,ts=12,**J)),
 A('TEJAS MK1A','jf',0,(-40,70,-30,-1),'ASTRA AAM',4,mt=500,sp=S(span=20,ch=13,wy=13,sw=11,tp=.15,fl=25,fy=2,fw=4,notail=1,**J)),
 A('AH-64E APACHE','heli',0,(10,-40,40,1),'AGM-114 HELLFIRE',8,0,sp=S(rotor=1,fl=28,fy=2,fw=6,nose='glass',ts=10)),
 A('SU-30SM','jf',1,(10,60,30,1),'R-77 AAM',8,mt=520,sp=S(span=26,ch=9,wy=13,sw=6,tp=.4,fl=30,fy=0,fw=7,twin=1,canard=1,ts=12,**J)),
 A('SU-34 FULLBACK','js',1,(20,-20,60,0),'KAB-500 BOMBS',10,sp=S(span=26,ch=9,wy=13,sw=6,tp=.4,fl=31,fy=0,fw=8,twin=1,canard=1,ts=12,**J)),
 A('SU-35 FLANKER-E','jf',1,(30,90,20,1),'R-77M AAM',8,mt=540,sp=S(span=26,ch=9,wy=13,sw=6,tp=.4,fl=30,fy=0,fw=7,twin=1,ts=12,**J)),
 A('MIG-31 FOXHOUND','jf',1,(120,-220,50,0),'R-37M AAM',4,mt=420,sp=S(span=24,ch=8,wy=15,sw=6,tp=.5,fl=30,fy=0,fw=8,twin=1,ts=12,**J)),
 A('TU-160 BLACKJACK','jb',1,(90,0,0,0),'KH-101 CRUISE',12,gun=0,sp=S(k='hb',span=32,ch=6,wy=12,sw=10,tp=.6,fl=31,fy=0,fw=6,ts=12,eng=[4],jeteng=1,**J)),
 A('J-10C','jf',1,(10,60,-15,0),'PL-15 AAM',6,mt=540,sp=S(span=22,ch=12,wy=14,sw=10,tp=.2,fl=28,fy=1,fw=5,canard=1,notail=1,**J)),
 A('J-35','jf',1,(20,20,-5,0),'PL-15 AAM',6,mt=540,sp=S(span=22,ch=8,wy=14,sw=6,tp=.4,fl=27,fy=2,fw=6,twin=1,ts=11,**J)),
 A('JF-17 THUNDER','jf',1,(-60,40,-25,0),'PL-12 AAM',4,mt=480,sp=S(span=20,ch=7,wy=14,sw=6,tp=.4,fl=26,fy=2,fw=5,ts=10,**J)),
 A('KA-52 ALLIGATOR','heli',1,(30,-20,30,1),'S-8 ROCKETS',12,0,sp=S(rotor=1,fl=26,fy=3,fw=6,nose='glass',twin=1,ts=10)),
 A('SU-25 FROGFOOT','cas',1,(40,40,0,4),'S-13 ROCKETS',10,sp=S(span=26,ch=5,wy=12,sw=2,fl=26,fy=2,fw=5,ts=10,eng=[3],jeteng=1,**J)),
]
ERA_AIR=[W0,
 [dict(n='MIG-21',mn='K-13 AAM',spd=480,turn=520,rate=5,dmg=11,bspd=1300,ammo=4,mturn=380,hp=95,side=1,sec=1,gunner=0,exp=0,fuel=0,sp=('era',1),cls='jet'),
  dict(n='F-4 PHANTOM',mn='AIM-7',spd=460,turn=470,rate=5,dmg=12,bspd=1300,ammo=6,mturn=360,hp=120,side=0,sec=1,gunner=0,exp=0,fuel=0,sp=('era',1),cls='jet'),
  dict(n='YAK-38',mn='KH-23 ROCKETS',spd=400,turn=440,rate=6,dmg=9,bspd=1250,ammo=4,mturn=0,hp=80,side=1,sec=0,gunner=0,exp=0,fuel=0,sp=S(span=16,ch=6,wy=14,sw=4,fl=26,fy=2,fw=6,nose='jet',ts=10,jeteng=0,noprop=1),cls='jet')]+CW_ADD,
 [dict(n='F-35A',mn='AIM-120',spd=520,turn=540,rate=4,dmg=10,bspd=1400,ammo=6,mturn=560,hp=100,side=0,sec=1,gunner=0,exp=0,fuel=0,sp=('era',2),cls='jet'),
  dict(n='SU-57',mn='R-77',spd=540,turn=580,rate=4,dmg=10,bspd=1400,ammo=6,mturn=540,hp=105,side=1,sec=1,gunner=0,exp=0,fuel=0,sp=('era',2),cls='jet'),
  dict(n='F/A-18E HORNET',mn='AIM-120',spd=500,turn=560,rate=4,dmg=11,bspd=1400,ammo=8,mturn=520,hp=110,side=0,sec=1,gunner=0,exp=0,fuel=0,sp=S(span=24,ch=7,wy=13,sw=4,fl=28,fy=1,fw=6,nose='jet',twin=1,ts=12,noprop=1),cls='jet'),
  dict(n='J-15',mn='PL-12',spd=510,turn=540,rate=4,dmg=11,bspd=1400,ammo=6,mturn=520,hp=115,side=1,sec=1,gunner=0,exp=0,fuel=0,sp=S(span=26,ch=8,wy=13,sw=6,fl=29,fy=1,fw=6,nose='jet',twin=1,ts=12,noprop=1),cls='jet')]+MD_ADD,
 [dict(n='NGAD 6TH-GEN',mn='SEEKERS',spd=600,turn=640,rate=4,dmg=9,bspd=1500,ammo=6,mturn=700,hp=100,side=0,sec=1,gunner=0,exp=0,fuel=0,sp=('era',3),cls='jet'),
  dict(n='X-90 ARROW',mn='SWARM',spd=640,turn=600,rate=4,dmg=8,bspd=1500,ammo=8,mturn=650,hp=90,side=1,sec=1,gunner=0,exp=0,fuel=0,sp=('era',3),cls='jet')],
 [dict(n='SF-1 STARFIGHTER',mn='TORPEDOES',spd=520,turn=700,rate=5,dmg=12,bspd=1300,ammo=8,mturn=700,hp=100,side=0,sec=1,gunner=0,exp=0,fuel=0,sp=('era',4),cls='jet'),
  dict(n='VOID INTERCEPTOR',mn='PLASMA TORP',spd=580,turn=660,rate=4,dmg=10,bspd=1400,ammo=6,mturn=720,hp=95,side=1,sec=1,gunner=0,exp=0,fuel=0,sp=('era',4),cls='jet')],
 # v1.3 WW1 biplanes / triplane / bombers
 [A('SOPWITH CAMEL','sef',0,(-140,80,-30,-2),'20LB COOPER BOMBS',4,2,sp=S(bi=1,span=26,ch=4,wy=10,fl=18,fy=4,fw=4,nose='rad',ts=9)),
  A('FOKKER DR.I','sef',1,(-160,120,-35,-2),'NONE',0,sp=S(bi=1,span=22,ch=4,wy=9,fl=17,fy=5,fw=4,nose='rad',ts=8)),
  A('SPAD XIII','sef',0,(-110,20,-25,-1),'LE PRIEUR ROCKETS',4,0,sp=S(bi=1,span=24,ch=4,wy=10,fl=19,fy=4,fw=5,nose='int',ts=9)),
  A('ALBATROS D.III','sef',1,(-130,40,-30,-2),'NONE',0,sp=S(bi=1,span=24,ch=4,wy=10,fl=19,fy=4,fw=4,nose='int',ts=9)),
  A('SE5A','sef',0,(-110,30,-25,-1),'25LB BOMBS',4,2,sp=S(bi=1,span=24,ch=4,wy=10,fl=19,fy=4,fw=5,nose='int',ts=9)),
  A('FOKKER D.VII','sef',1,(-110,50,-20,-1),'NONE',0,sp=S(bi=1,span=24,ch=4,wy=10,fl=19,fy=4,fw=5,nose='int',ts=9)),
  A('NIEUPORT 17','sef',0,(-140,90,-40,-2),'LE PRIEUR ROCKETS',4,0,sp=S(bi=1,span=22,ch=3,wy=10,fl=17,fy=5,fw=4,nose='rad',ts=8)),
  A('GOTHA G.IV','hb',1,(-150,-40,-120,-2),'50KG BOMBS',10,sp=S(bi=1,k='hb',span=32,ch=5,wy=10,fl=24,fy=3,fw=4,eng=[7],nose='glass',ts=12)),
  A('HANDLEY PAGE O/400','hb',0,(-160,-50,-110,-2),'112LB BOMBS',12,sp=S(bi=1,k='hb',span=32,ch=5,wy=10,fl=24,fy=3,fw=4,eng=[7],nose='glass',ts=12))],
 # 1776 / medieval: no real warplanes existed - fictional lighter-than-air & da Vinci concept craft (flagged EXPERIMENTAL)
 [A('MONTGOLFIER BALLOON','db',0,(-200,-260,40,-4),'GRENADOES',8,2,exp=1,sp=S(balloon=1)),
  A('CROWN OBS. BALLOON','db',1,(-210,-270,50,-4),'GRENADOES',8,2,exp=1,sp=S(balloon=2))],
 [A('DA VINCI ORNITHOPTER','sef',0,(-170,40,-40,-4),'FIRE POTS',4,2,exp=1,sp=S(orni=1)),
  A('DA VINCI AERIAL SCREW','db',1,(-220,120,30,-4),'FIRE POTS',6,2,exp=1,sp=S(screw=1))]]
plane_t=[];AG={};gid=0
for e,lst in enumerate(ERA_AIR):
    for i,a in enumerate(lst):
        sp=a['sp']
        if isinstance(sp,tuple):g=ww2plane(sp[1]) if sp[0]=='old' else plane(sp[1])
        else:g=pg(sp)
        plane_t+=tiles4(g,32,32);AG[(e,i)]=gid;gid+=1
emit("plane_spr",plane_t,"unsigned int")
# ---------------------------------------------------------------- ships
def ss(sp):
    c=Cv(32,32);L=sp['L'];B=sp['B'];y0=16-L//2;y1=y0+L-1;k=sp['k'];mod=sp.get('mod',0)
    hull=14;deck=13 if mod else 8
    bow=int(L*0.28)
    c.p([(16,y0),(15+B//2,y0+bow),(15+B//2,y1-2),(14+B//2,y1),(17-B//2,y1),(16-B//2,y1-2),(16-B//2,y0+bow),(15,y0)],hull)
    c.p([(16,y0+2),(14+B//2,y0+bow+1),(14+B//2,y1-2),(17-B//2,y1-2),(17-B//2,y0+bow+1),(15,y0+2)],deck)
    if k=='CV':
        x0=16-B//2-sp.get('ov',1);x1=15+B//2+sp.get('ov',1)
        c.p([(x0+2,y0+1),(x1-2,y0+1),(x1,y0+4),(x1,y1-1),(x0,y1-1),(x0,y0+4)],12)
        for y in range(y0+3,y1-2,4):c.px(15,y,11);c.px(16,y,11)
        if sp.get('ang'):c.l(x0+1,y0+L//2,x1-2,y1-2,11)
        if sp.get('ski'):c.r(x0+2,y0+1,x1-2,y0+3,13)
        isl=sp.get('isl',[(L//2-3,5)])
        for (iy,ih) in isl:c.r(x1-2,y0+iy,x1+1,y0+iy+ih,14);c.r(x1-1,y0+iy+1,x1,y0+iy+2,7)
        if sp.get('fwd'):c.p([(16,y0),(15+B//2,y0+bow),(16-B//2,y0+bow),(15,y0)],hull);c.r(14,y0+3,17,y0+6,7)
        c.px(16,y1-2,9);c.px(15,y1-2,9)
        return c.grid(1)
    for (ty,sz,ng) in sp.get('tur',[]):
        yy=y0+ty;c.e(16-sz,yy-sz+1,15+sz,yy+sz-1,12);c.e(16-sz+1,yy-sz+1,14+sz,yy,14)
        fwd=yy<16;ln=sz+3
        for g in range(ng):
            gx=16-ng//2+g
            if fwd:c.l(gx,yy-ln,gx,yy-sz+1,7)
            else:c.l(gx,yy+sz-1,gx,yy+ln,7)
    if sp.get('cas'):  # casemate secondary battery along the hull sides (Texas' 5in/51s)
        for yy in range(y0+6,y1-6,3):c.px(16-B//2+1,yy,7);c.px(14+B//2,yy,7)
    s0,s1=sp.get('sup',(L//2-4,L//2+2))
    c.r(16-B//2+2,y0+s0,15+B//2-2,y0+s1,14);c.r(16-B//2+3,y0+s0,15+B//2-3,y0+s0+1,11)
    for fy in sp.get('fun',[]):c.e(14,y0+fy,17,y0+fy+2,7);c.px(15,y0+fy,2)
    if sp.get('vls'):
        for vy in sp['vls']:c.r(14,y0+vy,17,y0+vy+2,7);c.l(14,y0+vy+1,17,y0+vy+1,13)
    if sp.get('tt'):c.r(15-B//2+2,y0+sp['tt'],15-B//2+3,y0+sp['tt']+3,7);c.r(16+B//2-3,y0+sp['tt'],16+B//2-2,y0+sp['tt']+3,7)
    for my in sp.get('tri',[]):  # tripod masts (1916+ refit) with fire-control tops
        c.e(14,y0+my-1,17,y0+my+1,2);c.px(15,y0+my,11);c.px(16,y0+my,11)
    c.px(16,y1-2,9);c.px(15,y1-2,9)
    return c.grid(1)
# (name,cls,axis,spd,turn,reload,dmg,hp,sspd,life,w2,ammo2,wing(era,idx) or None,sprite)
# w2: 0 secondary guns/AA, 1 torpedoes, 2 anti-ship missiles, 3 launch air wing
def AI_(e,n):return (e,[a['n'] for a in ERA_AIR[e]].index(n))
SH0=[('USS IOWA','BB',0,150,150,210,78,620,1100,150,0,0,None,S(k='BB',L=31,B=8,tur=[(5,3,3),(10,3,3),(24,3,3)],sup=(13,19),fun=[14,18])),
 ('YAMATO','BB',1,130,130,240,90,700,1050,160,0,0,None,S(k='BB',L=31,B=9,tur=[(5,3,3),(10,3,3),(25,3,3)],sup=(13,20),fun=[16])),
 ('BISMARCK','BB',1,150,150,200,72,600,1100,150,0,0,None,S(k='BB',L=30,B=8,tur=[(4,3,2),(8,3,2),(21,3,2),(25,3,2)],sup=(11,17),fun=[14])),
 ('HMS HOOD','BB',0,170,160,200,70,540,1100,150,0,0,None,S(k='BB',L=31,B=7,tur=[(4,2,2),(8,2,2),(22,2,2),(26,2,2)],sup=(11,18),fun=[13,16])),
 ('USS BALTIMORE','CA',0,200,190,140,48,420,1150,130,0,0,None,S(k='CA',L=27,B=6,tur=[(5,2,3),(9,2,3),(21,2,3)],sup=(11,16),fun=[12,15])),
 ('TAKAO','CA',1,200,190,140,46,400,1150,130,1,6,None,S(k='CA',L=27,B=6,tur=[(4,2,2),(7,2,2),(10,2,2),(19,2,2),(22,2,2)],sup=(12,16),fun=[14],tt=17)),
 ('USS FLETCHER','DD',0,260,260,60,22,250,1150,100,1,8,None,S(k='DD',L=22,B=5,tur=[(3,2,1),(6,2,1),(15,2,1),(18,2,1)],sup=(8,11),fun=[9,12],tt=13)),
 ('KAGERO','DD',1,260,260,60,22,240,1150,100,1,8,None,S(k='DD',L=22,B=5,tur=[(3,2,2),(15,2,2),(18,2,2)],sup=(6,10),fun=[8,11],tt=12)),
 ('USS ENTERPRISE CV-6','CV',0,180,150,40,8,520,1200,60,3,0,AI_(0,'SBD DAUNTLESS'),S(k='CV',L=31,B=8,isl=[(11,6)])),
 ('AKAGI','CV',1,180,150,40,8,500,1200,60,3,0,AI_(0,'B5N KATE'),S(k='CV',L=31,B=8,isl=[(12,4)])),
 # v1.3: USS TEXAS (BB-35) - New York-class dreadnought, 1914; 10x 14in/45 in 5 twin turrets, ~21 kn, WW1 Grand Fleet + D-Day Omaha/Pointe du Hoc, Cherbourg
 ('USS TEXAS BB-35','BB',0,125,125,215,66,560,1050,145,0,0,None,S(k='BB',L=29,B=7,tur=[(4,2,2),(8,2,2),(16,2,2),(21,2,2),(25,2,2)],sup=(10,14),fun=[12],tri=[9,18],cas=1)),
 ('S-BOOT (SCHNELLBOOT)','DD',1,300,300,45,10,150,1150,80,1,2,None,S(k='DD',L=15,B=4,tur=[(3,1,1),(12,1,1)],sup=(6,9),tt=4))]
SH1=[('IOWA (1984 REFIT)','BB',0,150,150,210,76,640,1100,150,2,8,None,S(k='BB',L=31,B=8,mod=1,tur=[(5,3,3),(10,3,3),(24,3,3)],sup=(13,19),fun=[15],vls=[20])),
 ('KIROV','CA',1,190,170,90,40,520,1250,130,2,10,None,S(k='CA',L=30,B=8,mod=1,tur=[(26,2,2)],sup=(13,20),vls=[4,8])),
 ('SVERDLOV','CA',1,200,190,130,44,430,1150,130,0,0,None,S(k='CA',L=28,B=6,mod=1,tur=[(4,2,3),(8,2,3),(20,2,3),(24,2,3)],sup=(11,17),fun=[12,15])),
 ('USS SPRUANCE','DD',0,250,250,55,22,280,1250,110,2,8,None,S(k='DD',L=25,B=5,mod=1,tur=[(3,2,1),(21,2,1)],sup=(8,16),vls=[5])),
 ('USS FORRESTAL','CV',0,170,140,40,8,580,1250,60,3,0,AI_(1,'F-4 PHANTOM'),S(k='CV',L=31,B=8,ov=3,ang=1,isl=[(13,5)],mod=1)),
 ('KIEV','CV',1,180,150,70,30,520,1250,110,3,0,AI_(1,'YAK-38'),S(k='CV',L=30,B=8,ang=1,fwd=1,isl=[(12,7)],mod=1))]
SH2=[('ARLEIGH BURKE','DD',0,250,250,55,24,300,1300,110,2,10,None,S(k='DD',L=25,B=6,mod=1,tur=[(3,2,1)],sup=(9,15),vls=[6,18])),
 ('TICONDEROGA','CA',0,230,220,60,26,380,1300,110,2,12,None,S(k='CA',L=27,B=6,mod=1,tur=[(3,2,1),(24,2,1)],sup=(9,17),vls=[6,20])),
 ('TYPE 055','CA',1,240,220,60,26,400,1300,110,2,12,None,S(k='CA',L=29,B=7,mod=1,tur=[(3,2,1)],sup=(10,18),vls=[6,21])),
 ('USS NIMITZ','CV',0,170,140,40,8,640,1300,60,3,0,AI_(2,'F/A-18E HORNET'),S(k='CV',L=31,B=9,ov=3,ang=1,isl=[(17,5)],mod=1)),
 ('GERALD R. FORD','CV',0,180,140,40,8,660,1300,60,3,0,AI_(2,'F-35A'),S(k='CV',L=31,B=9,ov=3,ang=1,isl=[(19,4)],mod=1)),
 ('HMS QUEEN ELIZABETH','CV',0,180,150,40,8,600,1300,60,3,0,AI_(2,'F-35A'),S(k='CV',L=30,B=8,ov=2,ski=1,isl=[(11,3),(18,3)],mod=1)),
 ('FUJIAN','CV',1,170,140,40,8,640,1300,60,3,0,AI_(2,'J-15'),S(k='CV',L=31,B=9,ov=3,ang=1,isl=[(15,5)],mod=1))]
SHS=[SH0,SH1,SH2,[],[]]
ship_t=[];SG={};gid=0
for e,lst in enumerate(SHS):
    for i,s in enumerate(lst):ship_t+=tiles4(ss(s[13]),32,32);SG[(e,i)]=gid;gid+=1
emit("ship_spr",ship_t,"unsigned int")
# ---------------------------------------------------------------- C tables
def q(s):return '"'+s+'"'
o=["// generated by tools/gen_vehicles.py - do not edit",
   "enum{"+",".join("VC_"+c for c in VC)+",VC_N};",
   "static const char*const VCLASS_N[VC_N]={"+",".join(q(VCN[c]) for c in VC)+"};"]
mt=max(len(l) for l in TKS);o.append(f"#define MAXTK {mt}")
o.append("static const u8 NTK[NERA]={"+",".join(str(len(l)) for l in TKS)+"};")
o.append("typedef struct{const char*n;s16 spd,turn,reload,dmg,hp,sspd;u8 cls,gs,axis,exp,ammo2;}TankS;")
o.append("static const TankS TANKV[NERA][MAXTK]={")
for e,l in enumerate(TKS):
    o.append(" {"+",".join("{%s,%d,%d,%d,%d,%d,%d,VC_%s,%d,%d,%d,%d}"%(q(r[0]),r[4],r[5],r[6],r[7],r[8],r[9],r[1],TKG[(e,i)],r[2],r[3],r[10]) for i,r in enumerate(l))+"},")
o.append("};")
ma=max(len(l) for l in ERA_AIR);o.append(f"#define MAXAIR {ma}")
o.append("static const u8 NAIR[NERA]={"+",".join(str(len(l)) for l in ERA_AIR)+"};")
ACL=['sef','tef','jet','db','tb','mb','hb','fb','jf','js','jb','cas','heli','gun']
o.append("static const char*const ACLASS_N[14]={\"FIGHTER\",\"HEAVY FIGHTER\",\"JET / ROCKET\",\"DIVE BOMBER\",\"TORPEDO BOMBER\",\"MEDIUM BOMBER\",\"HEAVY BOMBER\",\"FLYING BOAT\",\"JET FIGHTER\",\"STRIKE JET\",\"JET BOMBER\",\"CLOSE SUPPORT\",\"ATTACK HELO\",\"GUNSHIP\"};")
o.append("typedef struct{const char*n;const char*mn;s16 spd,turn;u8 rate,dmg;s16 bspd;u8 ammo;s16 mturn,hp;u8 side,sec,spr,gunner,gs,exp,fuel,cls;}AirS; // sec 0 rockets 1 missiles 2 bombs 3 torpedo")
o.append("static const AirS AIRV[NERA][MAXAIR]={")
for e,l in enumerate(ERA_AIR):
    rows=[]
    for i,a in enumerate(l):
        spr=i if (e==0 and i<14) else (i if 0<e<5 and i<2 else 0)
        rows.append("{%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}"%(q(a['n']),q(a['mn']),a['spd'],a['turn'],a['rate'],a['dmg'],a['bspd'],a['ammo'],a['mturn'],a['hp'],a['side'],a['sec'],spr,a['gunner'],AG[(e,i)],a['exp'],a['fuel'],ACL.index(a['cls'])))
    o.append(" {"+",".join(rows)+"},")
o.append("};")
ms=max(len(l) for l in SHS);o.append(f"#define MAXSH {ms}")
o.append("static const u8 NSH[NERA]={"+",".join(str(len(l)) for l in SHS)+"};")
o.append("enum{SC_BB,SC_CA,SC_DD,SC_CV};static const char*const SCLASS_N[4]={\"BATTLESHIP\",\"CRUISER\",\"DESTROYER\",\"CARRIER\"};")
o.append("typedef struct{const char*n;u8 cls,axis;s16 spd,turn,reload,dmg,hp,sspd,life;u8 w2,ammo2,we,wi,gs;}ShipS;")
o.append("static const ShipS SHIPV[3][MAXSH]={")
for e in range(3):
    o.append(" {"+",".join("{%s,SC_%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}"%(q(s[0]),s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10],s[11],(s[12] or (0,0))[0],(s[12] or (0,0))[1],SG[(e,i)]) for i,s in enumerate(SHS[e]))+"},")
o.append("};")
open(os.path.join(ROOT,'src','vehdb.inc'),'w').write("\n".join(o)+"\n")
# preview sheets
def sheet(grids,fn,cols=12,pal=None,scale=3,cell=34):
    pal=pal or [((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3) for v in obj_pal(0,'A')]
    rows=(len(grids)+cols-1)//cols;im=Image.new('RGB',(cols*cell,rows*cell),(70,120,170))
    for n,gs in enumerate(grids):
        for g in gs:
            for y,row in enumerate(g):
                for x,v in enumerate(row):
                    if v:im.putpixel(((n%cols)*cell+1+x,(n//cols)*cell+1+y),pal[v])
    im.resize((im.width*scale,im.height*scale),Image.NEAREST).save(os.path.join(ROOT,'build',fn))
sheet([tk(r[11]) for l in TKS[:3] for r in l if r[11]],'v12_tanks_sheet.png')
sheet([[pg(a['sp']) if not isinstance(a['sp'],tuple) else (ww2plane(a['sp'][1]) if a['sp'][0]=='old' else plane(a['sp'][1]))] for l in ERA_AIR for a in l],'v12_planes_sheet.png')
sheet([[ss(s[13])] for l in SHS for s in l],'v12_ships_sheet.png')
print("vehicles:",sum(len(l) for l in TKS),"land,",sum(len(l) for l in ERA_AIR),"air,",sum(len(l) for l in SHS),"ships")
