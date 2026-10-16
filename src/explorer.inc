// v1.3 ASTRONAUT EXPLORER - solar system + fictional warp, real surface gravity, experiments journal
#define SCR_XMAP 78
#define SCR_XGAL 79
#define SCR_XFACT 80
#define SCR_XLAND 81
#define SCR_XSURF 82
#define SCR_XJRNL 83
#define XP_OFF 0xC80
enum{XK_LAND,XK_GAS,XK_STAR};
enum{XS_ROCK,XS_BAND,XS_ICE,XS_IO,XS_EARTH,XS_MARS,XS_STAR,XS_GAS,XS_HAZE,XS_HOLE};
enum{XH_NONE,XH_HEAT,XH_ICE,XH_VOLC,XH_GEYS,XH_RAD};
typedef struct{const char*n;u16 g100;u8 kind,sty,haz,par;u32 rho;u16 c1,c2,sky,gnd;u32 rkm;u16 vesc10;const char*sub;const char*f[7];}XW;
// rho = atmosphere density near surface (g/m^3); exoplanet atmospheres are ASSUMED Earth-like
static const XW XWD[]={
{"THE SUN",27400,XK_STAR,XS_STAR,0,255,0,RGB(255,230,120),RGB(255,170,40),0,0,696000,0,"G-TYPE STAR - 28X EARTH GRAVITY",{"SURFACE 5,500 C - CORE 15 MILLION C","99.86% OF THE SOLAR SYSTEM'S MASS","LIGHT TAKES 8 MIN 20 S TO REACH EARTH","PARKER SOLAR PROBE PASSED 6.1 MILLION","KM FROM IT ON DEC 24 2024.","NO LANDING: OBSERVE FROM A SAFE ORBIT",0}},
{"MERCURY",370,XK_LAND,XS_ROCK,XH_HEAT,255,0,RGB(150,140,130),RGB(110,100,95),0,RGB(120,112,104),2440,0,"SMALLEST PLANET - 0.38 G",{"YEAR 88 DAYS, SOLAR DAY 176 DAYS","-180 C AT NIGHT, 430 C AT NOON","NO REAL ATMOSPHERE","MESSENGER FOUND WATER ICE IN DARK","POLAR CRATERS. BEPICOLOMBO ARRIVES","NOV 2026.",0}},
{"VENUS",887,XK_LAND,XS_HAZE,XH_HEAT,255,65000,RGB(230,200,130),RGB(200,170,100),RGB(150,120,60),RGB(120,90,50),6052,0,"HOTTEST PLANET - 0.90 G",{"465 C, 92 BAR CO2, ACID CLOUDS","A DAY (243 D) OUTLASTS ITS YEAR (225)","SPINS BACKWARD (RETROGRADE)","VENERA 13 SURVIVED 127 MIN ON THE","SURFACE IN 1982. YOUR COOLING LASTS","2 MINUTES - WORK FAST.",0}},
{"EARTH",981,XK_LAND,XS_EARTH,0,255,1200,RGB(40,90,200),RGB(60,150,70),RGB(110,170,240),RGB(80,130,60),6371,0,"HOME - 1.00 G = 9.81 M/S2",{"THE BASELINE FOR EVERY EXPERIMENT","1 BAR NITROGEN/OXYGEN AIR","AIR DRAG SLOWS FEATHERS AND BALLS","THE ONLY WORLD KNOWN TO HAVE LIFE","RUN THE SET HERE FIRST, THEN COMPARE",0,0}},
{"THE MOON",162,XK_LAND,XS_ROCK,0,3,0,RGB(190,190,186),RGB(140,140,138),0,RGB(150,150,146),1737,24,"EARTH'S MOON - 0.17 G",{"12 PEOPLE WALKED HERE, 1969-1972","NO AIR: FOOTPRINTS LAST MILLIONS OF YR","APOLLO 15: DAVE SCOTT DROPPED A HAMMER","AND A FALCON FEATHER - THEY LANDED","TOGETHER. ARTEMIS WILL RETURN.",0,0}},
{"MARS",371,XK_LAND,XS_MARS,0,255,20,RGB(200,100,60),RGB(160,70,40),RGB(200,150,110),RGB(170,90,50),3390,0,"THE RED PLANET - 0.38 G",{"AIR IS 0.6% OF EARTH'S (CO2)","OLYMPUS MONS: 22 KM HIGH","A SOL IS 24 H 37 MIN","PERSEVERANCE IS CACHING SAMPLES;","INGENUITY FLEW 72 TIMES.","USE THE CHUTE AND THE ENGINE.",0}},
{"PHOBOS",1,XK_LAND,XS_ROCK,0,5,0,RGB(110,100,90),RGB(80,72,64),0,RGB(96,88,78),11,114,"MARS MOON - 0.0006 G",{"ONLY 22 KM ACROSS, ESCAPE SPEED 11 M/S","ORBITS 3 TIMES PER MARTIAN DAY","SPIRALING IN: BREAKS UP IN ~50 MYR","STICKNEY CRATER IS 9 KM WIDE","A JUMP LASTS ABOUT 18 MINUTES!",0,0}},
{"DEIMOS",0,XK_LAND,XS_ROCK,0,5,0,RGB(150,135,115),RGB(120,108,92),0,RGB(132,120,104),6,56,"MARS MOON - 0.0003 G",{"12 KM ACROSS, ESCAPE SPEED 5.6 M/S","A HARD THROW LEAVES DEIMOS FOREVER","ORBITS MARS EVERY 30 HOURS","SMOOTH, DUST-BLANKETED SURFACE",0,0,0}},
{"CERES",28,XK_LAND,XS_ROCK,0,255,0,RGB(120,120,124),RGB(230,230,230),0,RGB(100,100,104),470,510,"DWARF PLANET - 0.03 G",{"LARGEST BODY IN THE ASTEROID BELT","940 KM ACROSS","DAWN ORBITED IT 2015-2018","OCCATOR'S BRIGHT SPOTS ARE SALT","(SODIUM CARBONATE) FROM BRINE.",0,0}},
{"JUPITER",2479,XK_GAS,XS_BAND,XH_RAD,255,160,RGB(210,170,120),RGB(170,110,70),RGB(200,170,130),RGB(110,90,80),69911,0,"GAS GIANT - 2.53 G AT 1 BAR",{"NO SURFACE: YOU FLOAT IN A HAB","GREAT RED SPOT IS WIDER THAN EARTH","A DAY IS 9 H 56 MIN","95+ KNOWN MOONS","JUNO HAS ORBITED SINCE 2016",0,0}},
{"IO",180,XK_LAND,XS_IO,XH_VOLC,9,0,RGB(230,210,90),RGB(200,120,40),0,RGB(200,180,70),1822,0,"JUPITER MOON - 0.18 G",{"MOST VOLCANIC BODY KNOWN","~400 ACTIVE VOLCANOES","TIDAL HEATING FROM JUPITER","SULFUR PLUMES UP TO 400 KM HIGH","RADIATION IS INTENSE: WORK FAST",0,0}},
{"EUROPA",131,XK_LAND,XS_ICE,XH_ICE,9,0,RGB(225,215,195),RGB(150,100,70),0,RGB(205,200,190),1561,0,"JUPITER MOON - 0.13 G",{"ICE SHELL OVER A SALTY OCEAN","~2X THE WATER OF EARTH'S OCEANS","SURFACE -160 C, SLICK ICE","EUROPA CLIPPER LAUNCHED OCT 2024,","ARRIVES APRIL 2030.",0,0}},
{"GANYMEDE",143,XK_LAND,XS_ROCK,0,9,0,RGB(150,140,130),RGB(200,195,185),0,RGB(140,132,124),2634,0,"JUPITER MOON - 0.15 G",{"LARGEST MOON - BIGGER THAN MERCURY","ONLY MOON WITH ITS OWN MAGNETIC FIELD","LIKELY HAS A BURIED OCEAN","ESA'S JUICE ARRIVES IN 2031",0,0,0}},
{"CALLISTO",124,XK_LAND,XS_ROCK,0,9,0,RGB(100,92,84),RGB(170,165,160),0,RGB(96,88,80),2410,0,"JUPITER MOON - 0.13 G",{"MOST HEAVILY CRATERED SURFACE","~4 BILLION YEARS OLD","LOW RADIATION: A FUTURE BASE SITE","VALHALLA BASIN IS 3,800 KM WIDE",0,0,0}},
{"SATURN",1044,XK_GAS,XS_BAND,0,255,190,RGB(230,210,150),RGB(200,170,110),RGB(220,200,150),RGB(120,110,90),58232,0,"RINGED GIANT - 1.06 G AT 1 BAR",{"LESS DENSE THAN WATER (0.69 G/CM3)","RINGS: WATER ICE, OFTEN ~10 M THICK","A DAY IS ABOUT 10 H 33 MIN","CASSINI ORBITED 2004-2017",0,0,0}},
{"TITAN",135,XK_LAND,XS_HAZE,0,14,5400,RGB(220,150,60),RGB(180,110,40),RGB(190,130,60),RGB(110,80,50),2575,0,"SATURN MOON - 0.14 G",{"AIR 1.5 BAR - THICKER THAN EARTH'S","NITROGEN SKY, METHANE RAIN AND LAKES","HUYGENS LANDED JAN 14 2005","DRAGONFLY ROTORCRAFT LAUNCHES 2028","THICK AIR + LOW G: FEATHERS DRIFT",0,0}},
{"ENCELADUS",11,XK_LAND,XS_ICE,XH_GEYS,14,0,RGB(240,245,250),RGB(150,190,220),0,RGB(230,236,240),252,239,"SATURN MOON - 0.012 G",{"BRIGHTEST SURFACE IN THE SYSTEM","GEYSERS AT THE SOUTH POLE 'TIGER","STRIPES' FEED SATURN'S E RING","CASSINI FLEW THROUGH THE PLUMES","AN OCEAN LIES UNDER THE ICE",0,0}},
{"URANUS",869,XK_GAS,XS_GAS,0,255,420,RGB(150,210,220),RGB(130,190,200),RGB(150,210,220),RGB(70,110,120),25362,0,"ICE GIANT - 0.89 G AT 1 BAR",{"TILTED 98 DEGREES - ROLLS ON ITS SIDE","COLDEST PLANETARY AIR: -224 C","13 FAINT RINGS","VOYAGER 2 FLEW BY IN 1986",0,0,0}},
{"NEPTUNE",1115,XK_GAS,XS_GAS,0,255,450,RGB(60,100,220),RGB(40,70,180),RGB(70,110,220),RGB(30,50,110),24622,0,"ICE GIANT - 1.14 G AT 1 BAR",{"FASTEST WINDS: UP TO 2,100 KM/H","FOUND BY MATH IN 1846","A YEAR IS 165 EARTH YEARS","VOYAGER 2 FLEW BY IN 1989",0,0,0}},
{"TRITON",78,XK_LAND,XS_ICE,XH_GEYS,18,0,RGB(220,200,190),RGB(180,140,130),0,RGB(200,184,176),1353,0,"NEPTUNE MOON - 0.08 G",{"ORBITS BACKWARD: A CAPTURED KUIPER","BELT OBJECT","NITROGEN GEYSERS SEEN BY VOYAGER 2","-235 C",0,0,0}},
{"PLUTO",62,XK_LAND,XS_ICE,0,255,0,RGB(220,190,160),RGB(150,100,80),0,RGB(210,186,160),1188,0,"DWARF PLANET - 0.06 G",{"NEW HORIZONS FLEW BY JULY 14 2015","THE 'HEART' IS NITROGEN ICE","(SPUTNIK PLANITIA)","CHARON IS HALF PLUTO'S SIZE","A YEAR IS 248 EARTH YEARS",0,0}},
// ---- beyond (FICTIONAL WARP) - exoplanet g / air are estimates
{"PROXIMA B",1050,XK_LAND,XS_MARS,0,255,1200,RGB(170,100,80),RGB(120,70,60),RGB(120,70,60),RGB(110,70,56),7200,0,"4.24 LY - G ~1.07 (EST)",{"ORBITS RED DWARF PROXIMA CENTAURI","IN ITS HABITABLE ZONE","YEAR: 11.2 DAYS, LIKELY TIDE-LOCKED","ALPHA CEN A + B SHINE IN THE SKY","MASS >=1.07 EARTH; AIR IS UNKNOWN","(GAME ASSUMES EARTH-LIKE AIR)",0}},
{"BARNARD'S STAR",0,XK_STAR,XS_STAR,0,255,0,RGB(230,100,60),RGB(170,50,30),0,0,140000,0,"5.96 LY - RED DWARF",{"FASTEST-MOVING STAR IN OUR SKY","(10.3 ARCSEC PER YEAR)","~14% THE SUN'S MASS","SMALL HOT PLANETS FOUND 2024-25","NO SAFE LANDING: OBSERVE",0,0}},
{"TRAPPIST-1E",802,XK_LAND,XS_ICE,0,255,1200,RGB(120,120,170),RGB(80,90,140),RGB(90,60,80),RGB(80,80,100),5900,0,"40 LY - 0.82 G",{"ONE OF 7 EARTH-SIZED PLANETS","IN THE HABITABLE ZONE","YEAR: 6.1 DAYS","JWST IS PROBING ITS ATMOSPHERE","(GAME ASSUMES EARTH-LIKE AIR)",0,0}},
{"KEPLER-452B",1840,XK_LAND,XS_EARTH,0,255,1200,RGB(60,110,160),RGB(90,120,70),RGB(140,170,200),RGB(90,110,70),10400,0,"~1,800 LY - G ~1.9 (EST)",{"1.6X EARTH'S SIZE","ORBITS A SUN-LIKE STAR, YEAR 385 D","ANNOUNCED 2015 AS 'EARTH'S COUSIN'","MASS UNKNOWN; STATUS DEBATED SINCE","A 2018 REANALYSIS",0,0}},
{"SIRIUS",0,XK_STAR,XS_STAR,0,255,0,RGB(220,235,255),RGB(150,190,255),0,0,1190000,0,"8.6 LY - BRIGHTEST NIGHT STAR",{"SIRIUS A: 2X SUN MASS, 25X BRIGHTER","SIRIUS B: A WHITE DWARF THE SIZE OF","EARTH WITH ~1 SUN MASS","THEY ORBIT EVERY 50 YEARS",0,0,0}},
{"BETELGEUSE",0,XK_STAR,XS_STAR,0,255,0,RGB(255,120,60),RGB(200,60,30),0,0,600000000,0,"~550 LY - RED SUPERGIANT",{"~750X THE SUN'S RADIUS: IT WOULD","SWALLOW MARS' ORBIT","GREAT DIMMING 2019-20: A DUST CLOUD","WILL GO SUPERNOVA, LIKELY WITHIN","~100,000 YEARS",0,0}},
{"SGR A*",0,XK_STAR,XS_HOLE,0,255,0,RGB(255,190,90),RGB(200,90,30),0,0,12000000,0,"~26,700 LY - GALAXY'S BLACK HOLE",{"4.3 MILLION SUN MASSES","EVENT HORIZON ~24 MILLION KM ACROSS","EHT RELEASED ITS IMAGE MAY 2022","STAR S2 LOOPS IT EVERY 16 YEARS","VIEWED FROM A SAFE 1 LIGHT-YEAR",0,0}},
};
#define XW_N 28
#define XW_SOL 21
static const u16 XLY10[7]={42,60,407,18000,86,5500,26700}; // light-years x10 except index 3,5,6 = whole ly
static const u8 XLYW[7]={0,0,0,1,0,1,1};
typedef struct{u32 vis,samp;u8 exp[XW_N];u16 jump[XW_N];u16 fall[XW_N];u8 last;}XPRec;
static XPRec XP EWRAM;
static void xp_write(void){const u8*b=(const u8*)&XP;u8 s=0x58;SRAM[XP_OFF]='A';SRAM[XP_OFF+1]='O';SRAM[XP_OFF+2]='E';SRAM[XP_OFF+3]='X';SRAM[XP_OFF+4]=1;for(int i=0;i<(int)sizeof XP;i++){SRAM[XP_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[XP_OFF+5+sizeof XP]=s;}
static void xp_read(void){memset(&XP,0,sizeof XP);if(SRAM[XP_OFF]!='A'||SRAM[XP_OFF+1]!='O'||SRAM[XP_OFF+2]!='E'||SRAM[XP_OFF+3]!='X'||SRAM[XP_OFF+4]!=1)return;u8*b=(u8*)&XP;u8 s=0x58;for(int i=0;i<(int)sizeof XP;i++){b[i]=SRAM[XP_OFF+5+i];s+=b[i]*(i+1);}if(s!=SRAM[XP_OFF+5+sizeof XP])memset(&XP,0,sizeof XP);}
static int xp_nsamp(void){int n=0;for(int i=0;i<XW_N;i++)n+=(XP.samp>>i)&1;return n;}
static float xg(int w){static const float sm[2]={0.0057f,0.003f};return w==6?sm[0]:w==7?sm[1]:w==16?0.113f:XWD[w].g100/100.0f;}
static char*xg_put(char*o,float g){if(g<0.1f){int v=(int)(g*10000+0.5f);o=sp_put(o,"0.");*o++='0'+v/1000;*o++='0'+v/100%10;*o++='0'+v/10%10;*o++='0'+v%10;*o=0;return o;}int v=(int)(g*100+0.5f);o=sp_int(o,v/100);*o++='.';*o++='0'+v/10%10;*o++='0'+v%10;*o=0;return o;}
static char*xm_put(char*o,float m){if(m>=1000){o=sp_intc(o,(int)(m+0.5f));return sp_put(o," M");}if(m>=10){o=sp_dec1(o,(int)(m*10+0.5f));return sp_put(o," M");}int v=(int)(m*100+0.5f);o=sp_int(o,v/100);*o++='.';*o++='0'+v/10%10;*o++='0'+v%10;*o=0;return sp_put(o," M");}
static char*xs_put(char*o,float s){if(s>=600){o=sp_int(o,(int)(s/60));o=sp_put(o," MIN ");o=sp_int(o,(int)s%60);return sp_put(o," S");}if(s>=10){o=sp_dec1(o,(int)(s*10+0.5f));return sp_put(o," S");}int v=(int)(s*100+0.5f);o=sp_int(o,v/100);*o++='.';*o++='0'+v/10%10;*o++='0'+v%10;*o=0;return sp_put(o," S");}
static u32 xh(u32 a){a^=a>>15;a*=0x2C1B3C6Du;a^=a>>12;a*=0x297A2D39u;a^=a>>15;return a;}
// ---------------------------------------------------------------- world disc renderer
static void xw_disc(int w,int cx,int cy,int r){const XW*W=&XWD[w];if(r<1){sp_px(cx,cy,W->c1);return;}int sty=W->sty;
 if(sty==XS_STAR){int gl=r/3+2;for(int k=gl;k>0;k--){sp_disc(cx,cy,r+k,dimc(dimc(W->c2)));}}
 if(sty==XS_HOLE){for(int a=0;a<64;a++){int rr2=r*2;int x=cx+(COS(a*4)*rr2>>8),y=cy+(SIN(a*4)*rr2>>8)/3;u16 c=(a<32)?W->c1:W->c2;lf(x-1,y,3,2,c);}sp_disc(cx,cy,r,0);sp_circ(cx,cy,r+1,RGB(255,220,150));for(int a=32;a<64;a++){int rr2=r*2;int x=cx+(COS(a*4)*rr2>>8),y=cy+(SIN(a*4)*rr2>>8)/3;if(y>cy)lf(x-1,y,3,2,W->c1);}lf(cx-r,cy-r-2,r*2,2,RGB(255,200,120));return;}
 for(int j=-r;j<=r;j++){int wd=isq(r*r-j*j);u16 c=W->c1;
  if(sty==XS_BAND){int b=((j+r)*9/(2*r+1));c=(b&1)?W->c2:W->c1;if(w==9&&b==6&&r>5)c=W->c1;}
  else if(sty==XS_EARTH){c=(fabsi(j)>r*3/4)?RGB(240,240,250):W->c1;}
  else if(sty==XS_GAS){c=((j+r)*5/(2*r+1))&1?W->c2:W->c1;}
  lf(cx-wd,cy+j,wd*2+1,1,c);int sh=wd/3;if(sh>0)lf(cx+wd-sh+1,cy+j,sh,1,dimc(c));}
 if(sty==XS_ROCK||sty==XS_MARS||sty==XS_IO||sty==XS_ICE||sty==XS_EARTH){int n=r*r/5+1;if(n>60)n=60;for(int k=0;k<n;k++){u32 h=xh(w*977+k);int dx=(int)(h%(2*r+1))-r,dy=(int)((h>>8)%(2*r+1))-r;if(dx*dx+dy*dy>=(r-1)*(r-1))continue;
   if(sty==XS_ICE){if(k&1){sp_px(cx+dx,cy+dy,W->c2);sp_px(cx+dx+1,cy+dy,W->c2);}}else if(sty==XS_IO){lf(cx+dx,cy+dy,1+(k&1),1+(k&1),(k%3)?W->c2:RGB(60,40,20));}else if(sty==XS_EARTH){lf(cx+dx,cy+dy,2,1,(k&1)?W->c2:RGB(240,240,250));}else sp_px(cx+dx,cy+dy,W->c2);}}
 if(w==9&&r>5){sp_disc(cx+r/3,cy+r/4,r/6+1,RGB(200,90,60));}
 if(w==14){int a=r*21/10,b=r*4/10+1;for(int k=0;k<96;k++){int an=k*256/96;int x=cx+(COS(an)*a>>8),y=cy+(SIN(an)*b>>8);int x2=cx+(COS(an)*a*3/4>>8),y2=cy+(SIN(an)*b*3/4>>8);int dx=x-cx,dy=y-cy;if(SIN(an)<0&&dx*dx+dy*dy<r*r)continue;sp_px(x,y,RGB(230,210,160));if(!(SIN(an)<0&&(x2-cx)*(x2-cx)+(y2-cy)*(y2-cy)<r*r))sp_px(x2,y2,RGB(200,180,130));}}}
static void x_stars(u32 seed,int n,int ymax){for(int k=0;k<n;k++){u32 h=xh(seed+k);int x=h%240,y=(h>>9)%ymax;u16 c=(h>>20)&3?RGB(150,150,170):RGB(255,255,240);sp_px(x,y,c);}}
// ---------------------------------------------------------------- solar system map
static const u8 XMX[XW_SOL]={8,30,44,60,60,78,74,84,96,120,108,116,124,132,152,146,158,180,204,204,226};
static const u8 XMY[XW_SOL]={72,72,72,72,92,72,90,90,72,72,98,102,98,102,72,98,98,72,72,94,72};
static const u8 XMR[XW_SOL]={0,2,3,3,1,2,1,1,1,9,1,1,2,1,7,2,1,5,5,1,1};
static int xw_moon(int w){return w<XW_SOL&&XWD[w].par!=255;}
static void xw_status(int w,char*b){char*o=b;o=sp_put(o,"G ");o=xg_put(o,xg(w));o=sp_put(o," M/S2 = ");o=xg_put(o,xg(w)/9.81f);o=sp_put(o," G");if((XP.vis>>w)&1)o=sp_put(o,"  VISITED");if((XP.samp>>w)&1)sp_put(o," +SAMPLE");}
static int x_auto(void){u32 v=*(volatile u32*)0x0203FFD0;return v==0x4F545541u;}
static int x_warp_ok(void){return xp_nsamp()>=3||x_auto();}
static int x_solmap(int sel){int redraw=1;
 for(int t=0;;t++){next_frame();BEAT->screen=SCR_XMAP;BEAT->mode=sel;BEAT->scoreA=xp_nsamp();BEAT->phase=0;u16 pr=pressed;pressed=0;
  if(pr&K_RIGHT){sel=(sel+1)%XW_SOL;redraw=1;sfx_menu();}if(pr&K_LEFT){sel=(sel+XW_SOL-1)%XW_SOL;redraw=1;sfx_menu();}
  if(pr&K_DOWN){int s=sel;do s=(s+1)%XW_SOL;while(xw_moon(s));sel=s;redraw=1;sfx_menu();}if(pr&K_UP){int s=sel;do s=(s+XW_SOL-1)%XW_SOL;while(xw_moon(s));sel=s;redraw=1;sfx_menu();}
  if(pr&K_A){sfx_menu();return sel;}if(pr&K_B)return -1;if(pr&K_L)return -3;if(pr&K_R){if(x_warp_ok())return -2;redraw=1;F.msgT=90;}
  if(!redraw&&(t&15))continue;redraw=0;
  lfb=0;lf(0,0,240,160,RGB(2,3,10));x_stars(11,90,124);
  for(int k=1;k<XW_SOL;k++){if(xw_moon(k))continue;int R=XMX[k]+20;for(int a=-10;a<=10;a++){int an=a*2;int x=-20+(COS(an&255)*R>>8),y=72+(SIN(an&255)*R>>8);sp_px(x,y,RGB(26,30,50));}}
  for(int j=-36;j<=36;j++){int wd=isq(36*36-j*j);int x1=-20+wd;if(x1>0)lf(0,72+j,x1,1,j*j<200?RGB(255,240,160):RGB(255,200,80));}
  for(int k=1;k<XW_SOL;k++)xw_disc(k,XMX[k],XMY[k],XMR[k]);
  {int x=XMX[sel],y=XMY[sel],r=XMR[sel]+3;if(sel==0){x=6;r=8;}u16 c=(t>>3)&1?RGB(255,255,120):RGB(255,180,60);lf(x-r,y-r,3,1,c);lf(x-r,y-r,1,3,c);lf(x+r-2,y-r,3,1,c);lf(x+r,y-r,1,3,c);lf(x-r,y+r,3,1,c);lf(x-r,y+r-2,1,3,c);lf(x+r-2,y+r,3,1,c);lf(x+r,y+r-2,1,3,c);}
  for(int k=1;k<XW_SOL;k++)if((XP.samp>>k)&1)lf(XMX[k]-1,XMY[k]-XMR[k]-5,3,2,C_GREEN);
  lf(0,0,240,11,RGB(8,10,24));ltx(2,2,"SOLAR SYSTEM",RGB(255,220,120));{char b[32],*o=sp_put(b,"SAMPLES ");o=sp_int(o,xp_nsamp());sp_put(o,"/23");ltx(150,2,b,C_GREEN);}
  lf(0,124,240,36,RGB(8,10,24));lf(0,124,240,1,RGB(60,70,110));{const XW*W=&XWD[sel];ltx(4,127,W->n,RGB(255,240,160));ltx(4+slen(W->n)*6+6,127,W->sub,RGB(150,170,210));char b[64];xw_status(sel,b);ltx(4,137,b,C_WHITE);}
  if(F.msgT>0){F.msgT-=16;ltc(147,"WARP DRIVE LOCKED: COLLECT 3 SAMPLES",RGB(255,140,100));}else ltx(4,148,"A:GO  </>:BODY  ^v:PLANET  R:WARP  L:LOG",RGB(120,140,180));
  m3_present();}}
// ---------------------------------------------------------------- galaxy map + warp (fictional)
static const u8 XGX[7]={190,176,208,226,166,146,62},XGY[7]={78,60,52,30,86,40,72};
static void x_galaxy_draw(int sel,int t){lfb=0;lf(0,0,240,160,RGB(2,2,8));x_stars(77,60,124);
 int cx=62,cy=72;u32 s=12345;for(int k=0;k<520;k++){s=s*1103515245u+12345u;int arm=(s>>16)&3;s=s*1103515245u+12345u;int tt=(s>>16)&255;s=s*1103515245u+12345u;int nz=((s>>16)&15)-8;int r=6+tt*50/256;int an=(arm*64+tt*3/2+nz)&255;int x=cx+(COS(an)*r>>8)+((nz*3)>>3),y=cy+((SIN(an)*r>>8)*3>>2);sp_px(x,y,tt<60?RGB(250,230,170):tt<150?RGB(170,170,210):RGB(110,120,170));}
 sp_disc(cx,cy,5,RGB(255,230,170));sp_disc(cx,cy,2,RGB(255,255,230));
 {int an=200,r=31;int x=cx+(COS(an)*r>>8),y=cy+((SIN(an)*r>>8)*3>>2);lf(x-1,y-1,3,3,RGB(255,255,0));ltx(x+3,y+2,"SUN",RGB(255,255,120));sp_line(x+2,y-2,130,20,RGB(50,60,90));sp_line(x+2,y+2,130,120,RGB(50,60,90));}
 m3_box(130,18,106,104,0);lf(131,19,104,102,RGB(4,6,16));ltx(134,21,"LOCAL STARS",RGB(120,140,180));lf(181,68,3,3,RGB(255,255,120));ltx(176,72,"SUN",RGB(160,160,100));
 for(int k=0;k<7;k++){int x=XGX[k],y=XGY[k];u16 c=XWD[21+k].c1;if(k==6){sp_disc(x,y,3,0);sp_circ(x,y,4,RGB(255,200,100));}else lf(x-1,y-1,3,3,c);if(k==3)ltx(x-24,y-10,"1800LY",RGB(90,100,130));if(k==5)ltx(x-6,y-10,"550LY",RGB(90,100,130));
  if(k==sel){int r=5;u16 cc=(t>>3)&1?RGB(255,255,120):RGB(255,180,60);sp_circ(x,y,r+1,cc);}if((XP.samp>>(21+k))&1)lf(x-1,y-6,3,2,C_GREEN);}
 lf(0,0,240,11,RGB(8,10,24));ltx(2,2,"MILKY WAY - NOT TO SCALE",RGB(255,220,120));
 lf(0,124,240,36,RGB(8,10,24));lf(0,124,240,1,RGB(60,70,110));const XW*W=&XWD[21+sel];ltx(4,127,W->n,RGB(255,240,160));ltx(4+slen(W->n)*6+6,127,W->sub,RGB(150,170,210));
 char b[64],*o=sp_put(b,"VOYAGER 1 WOULD TAKE ~");int ly10=XLY10[sel];int yr=XLYW[sel]?ly10*17600:ly10*1760;o=sp_intc(o,yr);sp_put(o," YR");ltx(4,137,b,C_WHITE);
 ltx(4,148,"A:WARP (FICTIONAL)  </>:TARGET  B:BACK",RGB(120,140,180));m3_present();}
static int x_galmap(int sel){int redraw=1;for(int t=0;;t++){next_frame();BEAT->screen=SCR_XGAL;BEAT->mode=21+sel;u16 pr=pressed;pressed=0;
  if(pr&K_RIGHT){sel=(sel+1)%7;redraw=1;sfx_menu();}if(pr&K_LEFT){sel=(sel+6)%7;redraw=1;sfx_menu();}if(pr&K_A)return sel;if(pr&K_B)return -1;
  if(redraw||(t&15)==0){x_galaxy_draw(sel,t);redraw=0;}}}
static void x_warp(int w,int in){static s16 SX[48]EWRAM,SY[48]EWRAM;for(int k=0;k<48;k++){u32 h=xh(k*31+w);SX[k]=(h%240)-120;SY[k]=((h>>10)%160)-80;}
 for(int t=0;t<110;t++){next_frame();BEAT->screen=SCR_XGAL;BEAT->phase=9;lfb=0;lf(0,0,240,160,RGB(2,2,10));int sp=t<50?t:t>80?110-t:50;
  for(int k=0;k<48;k++){int x=SX[k],y=SY[k];int f=256+sp*6+t*4;int x1=120+x*f/256,y1=80+y*f/256,x2=120+x*(f+sp*5)/256,y2=80+y*(f+sp*5)/256;if((unsigned)x1<240&&(unsigned)y1<160)sp_line(x1,y1,x2,y2,sp>30?RGB(170,200,255):C_WHITE);else{u32 h=xh(k+t*77);SX[k]=(h%120)-60;SY[k]=((h>>8)%80)-40;}}
  ltc(10,in?"FICTIONAL WARP DRIVE ENGAGED":"DROPPING OUT OF WARP",RGB(255,220,120));ltc(140,"(REAL PHYSICS SAYS NO - THIS IS A GAME)",RGB(120,130,170));if(t>60)ltc(76,XWD[w].n,C_WHITE);m3_present();}}
// ---------------------------------------------------------------- facts card
static int x_facts(int w){const XW*W=&XWD[w];char l1[48];char*o=sp_put(l1,"SURFACE G ");o=xg_put(o,xg(w));o=sp_put(o," M/S2 (EARTH 9.81)");(void)o;
 char hb[220];o=hb;for(int i=3;i<7&&W->f[i];i++){o=sp_put(o,W->f[i]);*o++='|';}
 o=sp_put(o,W->kind==XK_STAR?"A: OBSERVE   ":W->kind==XK_GAS?"A: DESCEND TO THE 1-BAR FLOAT HAB":"A: DESCEND AND LAND");*o=0;
 sp_brief(W->n,W->sub,W->f[0]?W->f[0]:"",W->f[1]?W->f[1]:"",W->f[2]?W->f[2]:"",hb);
 m3_fill(12,135,216,10,RGB(16,20,40));m3_text(14,136,l1,C_GREEN,1);m3_present();
 for(int t=0;;t++){next_frame();BEAT->screen=SCR_XFACT;BEAT->mode=w;if(x_auto()&&t>40)return 1;if(pressed&K_A){pressed=0;return 1;}if(pressed&K_B){pressed=0;return 0;}}}
// ---------------------------------------------------------------- shared surface scene
typedef struct{float x,y,vx,vy;int face,wf,ground;float camx,ppm;int w,t,heat,sample,exsel,msgT;char msg[48];float res[5];int done;s16 TX[64],TY[64];int tn;}XSS;
static XSS XS EWRAM;
static void xs_msg(const char*m){char*o=XS.msg;int k=0;while(m[k]&&k<46){o[k]=m[k];k++;}o[k]=0;XS.msgT=150;}
static int x_airless(int w){return XWD[w].rho==0;}
static void x_sky(int w,int gy){const XW*W=&XWD[w];if(x_airless(w)){lf(0,0,240,gy,RGB(0,0,4));x_stars(w*13,50,gy);}else{u16 c=W->sky;for(int y=0;y<gy;y+=4){int k=y*8/(gy+1);u16 cc=k<3?dimc(c):c;if(k>5)cc=lit(c);lf(0,y,240,4<gy-y?4:gy-y,cc);}}
 int p=W->par;if(w==21){sp_disc(200,28,12,RGB(200,70,40));lf(150,18,2,2,RGB(255,250,220));lf(154,17,1,1,RGB(255,220,170));}
 else if(p!=255){int r=p==3?10:p==5?(w==6?22:8):p==9?(w==10?26:w==11?18:12):p==14?(w==15?10:16):9;if(w==15)sp_blend(150,18,60,30,RGB(200,160,90),128);else xw_disc(p,190,30,r);}
 else if(w==20){lf(60,26,4,4,RGB(200,190,180));}else if(x_airless(w)&&w<XW_SOL){int sr=w<=3?4:w<=8?3:1;sp_disc(40,22,sr,RGB(255,250,210));}
 if(w==2||w==15){sp_blend(0,0,240,gy,RGB(200,150,80),90);}}
static void x_lander(int x,int y,int sc){u16 G=RGB(210,170,60),S=RGB(180,180,186),D=RGB(90,90,96);int s=sc;
 lf(x-2*s,y-6*s,4*s,3*s,S);lf(x-s,y-7*s,2*s,s,S);lf(x-3*s,y-3*s,6*s,2*s,G);lf(x-4*s,y-1*s,s,s,D);lf(x+3*s,y-1*s,s,s,D);lf(x-4*s,y,2*s,1,D);lf(x+3*s,y,2*s,1,D);lf(x-s/2,y-s,s,s,D);}
static void x_ground(int w,int gy,float camx,float ppm){const XW*W=&XWD[w];lf(0,gy,240,160-gy,W->gnd);
 for(int k=0;k<30;k++){float wx=(float)((int)(camx/4)*4+k*4-40);u32 h=xh((u32)((int)wx+5000)*7+w);int sx=120+(int)((wx-camx)*ppm);int rw=1+(int)(ppm*((h&3)+1)/8);if(sx<-8||sx>248)continue;lf(sx,gy-rw/2,rw*2,rw/2+1,dimc(W->gnd));lf(sx,gy+3+(h>>4)%12,rw,1,W->c2);}
 if(W->haz==XH_ICE)for(int k=0;k<6;k++){int sx=((k*53-(int)(camx*ppm))%300+300)%300-30;sp_line(sx,gy+2,sx+40,gy+18,RGB(150,100,70));}
 if(W->haz==XH_VOLC){int vx=60;for(int j=0;j<14;j++)lf(vx-j,gy-14+j,2*j+1,1,RGB(120,90,40));for(int k=0;k<20;k++){u32 h=xh(k+XS.t/3*20);int py=gy-14-(int)((XS.t*2+k*9)%60);int px=vx+((int)(h%9)-4)*(gy-14-py)/30;sp_px(px,py,RGB(240,220,120));}}
 if(W->haz==XH_GEYS){for(int g=0;g<2;g++){int gx=40+g*120;for(int k=0;k<16;k++){int py=gy-((XS.t*3+k*7)%(gy-10));int px=gx+((int)(xh(k+g*99)%7)-3)*(gy-py)/40;sp_px(px,py,RGB(230,240,255));}}}
 if(w==15){lf(0,gy+2,90,5,RGB(50,40,30));ltx(4,gy+8,"",0);}}
static void x_ruler(int gy,float ppm){float st=ppm>=8?1:ppm>=1.5f?5:ppm>=0.3f?25:ppm>=0.06f?100:500;lf(2,8,1,gy-8,RGB(200,200,120));
 for(int k=0;k<30;k++){int y=gy-(int)(k*st*ppm);if(y<10)break;lf(2,y,k%2?3:6,1,RGB(200,200,120));if(k&&(k%2==0||st>=25)&&y>14){char b[12];sp_int(b,(int)(k*st));ltx(9,y-3,b,RGB(200,200,140));}}}
static void x_hud(int w){const XW*W=&XWD[w];lf(0,0,240,10,RGB(6,8,18));char b[64],*o=sp_put(b,W->n);o=sp_put(o,"  G ");o=xg_put(o,xg(w));sp_put(o," M/S2");ltx(2,1,b,RGB(255,230,150));
 if(XS.heat>0){o=sp_put(b,W->haz==XH_HEAT?"COOLING ":"RAD LIMIT ");sp_int(o,XS.heat/60);ltx(170,1,b,XS.heat<1200?C_RED:RGB(255,200,120));}}
// ---------------------------------------------------------------- landing (real g, drag, chute, engine)
static int x_land(int w){const XW*W=&XWD[w];float g=xg(w);int au=x_auto();
 if(W->kind==XK_GAS){for(int t=0;t<200;t++){next_frame();BEAT->screen=SCR_XLAND;BEAT->mode=w;BEAT->phase=1;lfb=0;lf(0,0,240,160,W->sky);for(int k=0;k<8;k++){int y=(k*24+t*(t<120?3:1))%170-10;lf(0,y,240,6,lit(W->sky));lf(0,y+6,240,3,W->c2);}
   if(t<70){sp_disc(120,60,8,RGB(160,140,120));sp_blend(110,40,20,14,RGB(255,200,120),160-t);ltc(120,"ENTRY: HEAT SHIELD AT PEAK HEATING",C_WHITE);}
   else if(t<130){lf(116,56,8,10,RGB(180,180,186));sp_line(116,56,108,34,C_WHITE);sp_line(124,56,132,34,C_WHITE);lf(104,28,32,6,RGB(240,120,60));ltc(120,"PARACHUTE OUT",C_WHITE);}
   else{sp_disc(120,40,22,RGB(230,230,236));lf(108,62,24,10,RGB(180,180,186));lf(110,64,4,4,RGB(120,190,255));lf(118,64,4,4,RGB(120,190,255));ltc(120,"BALLOON INFLATED - FLOATING AT 1 BAR",C_WHITE);}
   ltc(140,W->n,RGB(255,230,150));m3_present();}return 1;}
 float alt=400,vy=-(g>4?30:g>1?16:g>0.2f?6:1.5f),x=-30,vx=2.2f,fuel=45;float rho=W->rho;float kd=rho/1200.0f*0.0006f;int chute=0,heat=0;
 float ts=fsqr(3.7f/(g>0.001f?g:0.001f));if(ts<1)ts=1;if(ts>40)ts=40;float tmax=2.6f*g+1.2f;if(tmax<0.05f)tmax=0.05f;int t=0,res=0;
 for(;;t++){next_frame();u16 ky=keys,pr=pressed;pressed=0;
  if(au){ky=0;pr=0;float vt=-(1.2f+alt*0.12f);if(vt<-25)vt=-25;if(g<0.05f)vt=-(0.4f+alt*0.02f);if(rho>1000&&alt>150&&!chute)pr|=K_SEL;if(vy<vt)ky|=K_UP;float hv=-x*0.04f;if(hv>3)hv=3;if(hv<-3)hv=-3;if(vx<hv-0.15f)ky|=K_RIGHT;if(vx>hv+0.15f)ky|=K_LEFT;}
  if((pr&K_SEL)&&rho>0&&!chute&&alt>30){chute=1;sfx_menu();}if(pr&K_START)ts=ts*2>80?ts/8:ts*2;
  float dt=ts/60/4;int th=0,sj=0;for(int i=0;i<4;i++){float a=-g;if((ky&(K_UP|K_A))&&fuel>0){a+=tmax;fuel-=dt;th=1;}float k=kd*(chute?25:1);a+=-k*vy*(vy<0?-vy:vy);float ax=-k*vx*ffabs(vx);if(fuel>0){if(ky&K_RIGHT){ax+=0.8f;sj=1;fuel-=dt*0.2f;}if(ky&K_LEFT){ax-=0.8f;sj=-1;fuel-=dt*0.2f;}}
   vy+=a*dt;vx+=ax*dt;alt+=vy*dt;x+=vx*dt;if(chute&&alt<60)chute=2;if(alt<=0)break;}
  if(chute==2)kd=kd;BEAT->screen=SCR_XLAND;BEAT->mode=w;BEAT->phase=1;BEAT->x=(int)(alt*10);BEAT->y=(int)(-vy*100);BEAT->timer=(int)fuel;BEAT->hp=(int)ts;
  // draw
  float ppm=alt>60?1.2f:alt>15?3:6;int ly=alt*ppm>80?70:140-(int)(alt*ppm);int gy=ly+(int)(alt*ppm);if(gy>160)gy=160;lfb=0;x_sky(w,gy<160?gy:160);if(gy<160){XS.t=t;x_ground(w,gy,x,ppm);int px=120-(int)(x*ppm);lf(px-8,gy,16,2,RGB(240,240,120));}
  int sc=ppm>=6?3:ppm>=3?2:1;if(chute==1){sp_line(120-3*sc,ly-7*sc,112,ly-28,C_WHITE);sp_line(120+3*sc,ly-7*sc,128,ly-28,C_WHITE);lf(106,ly-34,28,6,RGB(240,120,60));}
  x_lander(120,ly,sc);if(th){int L=4+(t&3)+sc*2;for(int j=0;j<L;j++)lf(120-(L-j)/3,ly+j+1,(L-j)/3*2+1,1,j<L/3?RGB(255,250,220):RGB(160,200,255));}if(sj)lf(sj>0?120-5*sc:120+4*sc,ly-2*sc,sc*2,sc,C_WHITE);
  lf(0,0,240,10,RGB(6,8,18));char b[64],*o=sp_put(b,W->n);o=sp_put(o," DESCENT  G ");o=xg_put(o,g);ltx(2,1,b,RGB(255,230,150));o=sp_put(b,"TIME X");sp_int(o,(int)ts);ltx(190,1,b,RGB(150,150,170));
  lf(0,150,240,10,RGB(6,8,18));o=sp_put(b,"ALT ");o=sp_int(o,(int)alt);o=sp_put(o,"  V ");o=sp_dec1(o,(int)(vy*10));o=sp_put(o,"  H ");o=sp_dec1(o,(int)(vx*10));o=sp_put(o,"  FUEL ");sp_int(o,(int)fuel);ltx(2,151,b,fuel<8?C_RED:C_WHITE);
  ltc(140,rho>0&&!chute&&alt>30?"UP/A: ENGINE  </>: RCS  SELECT: CHUTE":"UP/A: ENGINE  </>: RCS  START: TIME",RGB(150,170,210));m3_present();if(alt<=0)break;(void)heat;if(t>60*240)break;}
 float sp=-vy;if(sp<0)sp=0;res=sp<2.6f&&ffabs(vx)<1.6f?2:sp<5.5f?1:0;
 for(int k=0;k<120;k++){next_frame();BEAT->screen=SCR_XLAND;BEAT->phase=res?2:5;lfb=0;x_sky(w,120);XS.t=t+k;x_ground(w,120,x,6);x_lander(120,res?120:92,3);lf(0,0,240,10,RGB(6,8,18));
  char b[48],*o=sp_put(b,"TOUCHDOWN ");o=sp_dec1(o,(int)(sp*10));sp_put(o," M/S");ltc(30,res==2?"TOUCHDOWN - SOFT LANDING":res==1?"HARD LANDING - GEAR HELD":"TOO FAST: AUTO-ABORT TO ORBIT",res?C_YEL:C_RED);ltc(42,b,C_WHITE);
  if(!res)ltc(54,"NO HARM DONE - TRY AGAIN",C_WHITE);m3_present();if(pressed&K_A){pressed=0;break;}}
 return res>0;}
// ---------------------------------------------------------------- surface + experiments
static const char*const XEN[5]={"JUMP","HAMMER+FEATHER","BALL THROW","FALL TIMER 10 M","WEIGHT SCALE"};
static void xs_astro(float wx,float wy,float ppm,int gy,int fr){int sx=120+(int)((wx-XS.camx)*ppm);int yp=(int)(wy*ppm);
 if(ppm>=7){eva_astro(sx,gy-6-yp,1,0,fr,0);}else{int h=(int)(1.9f*ppm);if(h<3)h=3;lf(sx-1,gy-yp-h,3,h,C_WH);lf(sx-1,gy-yp-h,3,1,RGB(250,210,90));}}
static void xs_scene(float ppm,int gy){int w=XS.w;lfb=0;x_sky(w,gy);x_ground(w,gy,XS.camx,ppm);
 if(XWD[w].kind==XK_GAS){lf(0,gy,240,3,RGB(160,160,170));for(int x=0;x<240;x+=20)lf(x,gy-8,2,8,RGB(160,160,170));lf(0,gy-8,240,1,RGB(160,160,170));}
 else{int lx=120+(int)((0-XS.camx)*ppm);int sc=ppm>=8?3:ppm>=3?2:1;if(lx>-30&&lx<270)x_lander(lx,gy,sc);}
 if(!XS.sample&&XWD[w].kind!=XK_GAS){int sx=120+(int)((16-XS.camx)*ppm);if(sx>-4&&sx<244){lf(sx-3,gy-3,6,3,XWD[w].c2);if((XS.t>>4)&1)lf(sx-1,gy-9,2,4,C_GREEN);}}
 x_ruler(gy,ppm);x_hud(w);}
static void xs_bottom(const char*l1,const char*l2){lf(0,140,240,20,RGB(6,8,18));if(l1)ltx(2,141,l1,RGB(255,240,160));if(l2)ltx(2,151,l2,RGB(140,160,200));}
static void xs_store(int k,float v){int w=XS.w;XP.exp[w]|=1<<k;if(k==0){int cm=(int)(v*100+0.5f);XP.jump[w]=cm<32768?cm:0x8000|((int)v>32767?32767:(int)v);}if(k==3){int cs=(int)(v*100+0.5f);XP.fall[w]=cs>65535?65535:cs;}XS.res[k]=v;}
static int xs_wait(int fr){for(int t=0;t<fr;t++){next_frame();BEAT->screen=SCR_XSURF;if(pressed&K_A){pressed=0;return 1;}if(x_auto()&&t>90)return 1;}return 0;}
static void xe_jump(void){float g=xg(XS.w),v0=3.1f,T=2*v0/g,h=v0*v0/(2*g);float ts=T/3;if(ts<1)ts=1;float ppm=90/(h+2.2f);if(ppm>10)ppm=10;int gy=130;float t=0;
 for(int f=0;;f++){next_frame();BEAT->screen=SCR_XSURF;BEAT->phase=10;t+=ts/60;if(t>T)t=T;float y=v0*t-g*t*t/2;if(y<0)y=0;xs_scene(ppm,gy);xs_astro(XS.x,y,ppm,gy,0);
  char b[64],*o=sp_put(b,"JUMP 3.1 M/S  HEIGHT ");o=xm_put(o,y);xs_bottom(b,0);o=sp_put(b,"T+");o=xs_put(o,t);if(ts>1.5f){o=sp_put(o,"  TIME X");sp_int(o,(int)ts);}ltx(120,151,b,RGB(140,160,200));m3_present();if(t>=T)break;}
 xs_store(0,h);char b[64],*o=sp_put(b,"PEAK ");o=xm_put(o,h);o=sp_put(o,"  AIR ");o=xs_put(o,T);char c[64];o=sp_put(c,"EARTH: 0.49 M, 0.63 S  (X");o=sp_dec1(o,(int)(h/0.49f*10+0.5f));sp_put(o,")");xs_bottom(b,c);m3_present();xs_wait(240);}
static void xe_feather(void){int w=XS.w;float g=xg(w),h0=1.6f,rho=XWD[w].rho/1000.0f;float vtf=rho>0?0.175f*fsqr(g/rho):0;float th=fsqr(2*h0/g);float ts=th/2.5f;if(ts<1)ts=1;
 float t=0,yh=h0,yf=h0,vf=0,tf=-1,thit=-1;int gy=130;float ppm=30;
 for(int f=0;f<60*40;f++){next_frame();BEAT->screen=SCR_XSURF;BEAT->phase=11;float d=ts/60/8;for(int i=0;i<8;i++){t+=d;if(thit<0){yh=h0-g*t*t/2;if(yh<=0){yh=0;thit=th;}}if(tf<0){float a=g;if(vtf>0)a=g*(1-(vf*vf)/(vtf*vtf));vf+=a*d;yf-=vf*d;if(yf<=0){yf=0;tf=t;}}}
  if(thit>=0&&tf<0&&t>thit*1.5f+2&&ts<200)ts*=1.02f;
  lfb=0;x_sky(w,gy);x_ground(w,gy,XS.camx,4);lf(0,0,240,10,RGB(6,8,18));x_hud(w);eva_astro(96,gy-6*3,3,0,0,0);int hx=118,fx=128;
  {int y=gy-(int)(yh*ppm);lf(hx-1,y-6,2,6,RGB(140,100,60));lf(hx-3,y-7,6,2,RGB(120,120,130));}{int y=gy-(int)(yf*ppm);int wob=vtf>0&&yf>0?((f>>3)&1)*2-1:0;lf(fx-1+wob,y-6,1,7,RGB(240,240,230));lf(fx+wob,y-5,2,4,RGB(200,190,170));}
  char b[64],*o=sp_put(b,"HAMMER ");o=thit>=0?xs_put(o,thit):sp_put(o,"...");o=sp_put(o,"  FEATHER ");tf>=0?xs_put(o,tf):sp_put(o,"...");xs_bottom(b,vtf>0?"AIR DRAG SLOWS THE FEATHER":"VACUUM: NO DRAG AT ALL");if(ts>1.5f){o=sp_put(b,"TIME X");sp_int(o,(int)ts);ltx(180,151,b,RGB(140,160,200));}
  m3_present();if(thit>=0&&tf>=0)break;}
 if(tf<0)tf=t;xs_store(1,tf);char b[64],*o=sp_put(b,"HAMMER ");o=xs_put(o,thit);o=sp_put(o,"  FEATHER ");xs_put(o,tf);xs_bottom(b,tf-thit<0.05f?"THEY LAND TOGETHER - GALILEO WAS RIGHT":"AIR HOLDS THE FEATHER BACK");m3_present();xs_wait(240);}
static float x_ball_k(int w){float rho=XWD[w].rho/1000.0f;if(rho<=0)return 0;float vt=11.55f*fsqr(xg(w)/rho);return xg(w)/(vt*vt);}
static void xe_throw(void){int w=XS.w;float g=xg(w),k=x_ball_k(w),v=10;float Te=2*v*0.7071f/g+1;float dt=Te/800;if(dt>0.02f)dt=0.02f;
 int esc=XWD[w].vesc10&&XWD[w].vesc10<100;float apex=1.6f,R=0,T=0;int n=0;int gy=128;float hs=1,vs=1;
 for(int pass=0;pass<2&&!esc;pass++){float vx=v*0.7071f,vy=v*0.7071f,x=0,y=1.6f,t=0;int every=n/63+1;XS.tn=0;
  for(n=0;n<4000;n++){float sp=fsqr(vx*vx+vy*vy);vx+=(-k*sp*vx)*dt;vy+=(-g-k*sp*vy)*dt;x+=vx*dt;y+=vy*dt;t+=dt;if(y>apex)apex=y;if(pass&&(n%every)==0&&XS.tn<64){XS.TX[XS.tn]=20+(int)(x*hs);XS.TY[XS.tn]=gy-(int)((y>0?y:0)*vs);XS.tn++;}if(y<0)break;}
  R=x;T=t;hs=200/(R>4?R:4);vs=hs;if(apex*vs>100)vs=100/apex;}
 for(int f=0;f<200;f++){next_frame();BEAT->screen=SCR_XSURF;BEAT->phase=12;lfb=0;x_sky(w,gy);lf(0,gy,240,32,XWD[w].gnd);lf(0,0,240,10,RGB(6,8,18));x_hud(w);
  if(esc){float fr=f/120.0f;int bx=20+(int)(fr*220),by=gy-10-(int)(fr*130);lf(16,gy-12,3,12,C_WH);if(by>10)lf(bx,by,3,3,RGB(255,150,60));xs_bottom("THROWN AT 10 M/S: ESCAPE SPEED IS 5.6","IT LEAVES DEIMOS FOREVER!");m3_present();continue;}
  int m=XS.tn*(f>150?150:f)/150;if(m>=XS.tn)m=XS.tn-1;for(int i=0;i<m;i++)sp_px(XS.TX[i],XS.TY[i],RGB(200,200,120));lf(XS.TX[m]-1,XS.TY[m]-1,3,3,RGB(255,150,60));
  lf(18,gy-8,3,8,C_WH);for(int q=0;q<=4;q++){int sx=20+(int)(R*q/4*hs);lf(sx,gy,1,4,C_WHITE);if(q==4||q==2){char b[16];xm_put(b,R*q/4);ltx(sx-14,gy+6,b,C_WHITE);}}
  char b[64],*o=sp_put(b,"10 M/S AT 45 DEG  RANGE ");o=xm_put(o,f>=150?R:R*m/(XS.tn>1?XS.tn-1:1));xs_bottom(b,0);o=sp_put(b,"FLIGHT ");o=xs_put(o,T);sp_put(o,R>XWD[w].rkm*1000.0f?"  (FLAT MATH BREAKS!)":"  EARTH VACUUM 10.2 M");ltx(2,151,b,RGB(140,160,200));m3_present();}
 xs_store(2,esc?99999:R);xs_wait(150);}
static float x_fall_time(int w,float h){float g=xg(w),k=x_ball_k(w);if(k<=0)return fsqr(2*h/g);float y=h,vy=0,t=0,dt=fsqr(2*h/g)/600;for(int i=0;i<4000&&y>0;i++){vy+=(g-k*vy*vy)*dt;y-=vy*dt;t+=dt;}return t;}
static void xe_fall(void){int w=XS.w;float g=xg(w);float T=x_fall_time(w,10);float ts=T/3;if(ts<1)ts=1;int gy=134;float ppm=10;float t=0;float k=x_ball_k(w),y=10,vy=0;
 for(int f=0;;f++){next_frame();BEAT->screen=SCR_XSURF;BEAT->phase=13;float d=ts/60/8;for(int i=0;i<8&&y>0;i++){vy+=(g-k*vy*vy)*d;y-=vy*d;t+=d;}if(y<0)y=0;
  lfb=0;x_sky(w,gy);x_ground(w,gy,XS.camx,ppm);x_ruler(gy,ppm);x_hud(w);lf(140,gy-100,3,100,RGB(170,170,176));lf(136,gy-101,12,2,RGB(170,170,176));lf(146,gy-(int)(y*ppm)-3,4,4,RGB(255,150,60));eva_astro(110,gy-6,1,0,0,0);
  char b[64],*o=sp_put(b,"DROP FROM 10 M  T+ ");o=xs_put(o,t);xs_bottom(b,0);o=sp_put(b,"V ");o=sp_dec1(o,(int)(vy*10));o=sp_put(o," M/S");if(ts>1.5f){o=sp_put(o,"  TIME X");sp_int(o,(int)ts);}ltx(2,151,b,RGB(140,160,200));m3_present();if(y<=0||f>60*30)break;}
 xs_store(3,t);char b[64],*o=sp_put(b,"10 M FALL: ");o=xs_put(o,t);o=sp_put(o,"  EARTH 1.43 S");xs_bottom(b,k>0?"(INCLUDES AIR DRAG)":"(NO AIR: T = SQRT(2H/G))");m3_present();xs_wait(240);}
static void xe_weight(void){int w=XS.w;float g=xg(w);float N=100*g,kg=N/9.81f;int gy=130;
 for(int f=0;f<200;f++){next_frame();BEAT->screen=SCR_XSURF;BEAT->phase=14;xs_scene(10,gy);int cx=120;lf(cx-14,gy-4,28,4,RGB(90,90,100));eva_astro(cx,gy-10,1,0,0,0);
  m3_box(150,30,80,70,1);sp_disc(190,62,24,RGB(240,240,230));float fr=f>90?1:f/90.0f;float v=kg*fr;int ang=(int)(v*256/300);if(ang>230)ang=230;int an=(192+ang)&255;sp_line(190,62,190+(COS(an)*20>>8),62+(SIN(an)*20>>8),C_RED);for(int m=0;m<=6;m++){int a=(192+m*256*50/300)&255;sp_px(190+(COS(a)*22>>8),62+(SIN(a)*22>>8),RGB(40,40,40));}
  char b[48],*o=sp_int(b,(int)(v+0.5f));sp_put(o," KG");ltx(176,88,b,RGB(30,30,30));
  o=sp_put(b,"YOU+SUIT 100 KG: ");o=sp_int(o,(int)(N+0.5f));o=sp_put(o," N");char c[64];char*q=sp_put(c,"EARTH SCALE READS ");q=sp_dec1(q,(int)(kg*10+0.5f));sp_put(q," KG (EARTH 100)");xs_bottom(b,c);m3_present();}
 xs_store(4,N);xs_wait(60);}
static void xs_runexp(int k){if(k==0)xe_jump();else if(k==1)xe_feather();else if(k==2)xe_throw();else if(k==3)xe_fall();else xe_weight();XP.vis|=1u<<XS.w;xp_write();}
static void x_surface(int w){memset(&XS,0,sizeof XS);XS.w=w;XS.x=4;XS.ppm=10;int au=x_auto();int gas=XWD[w].kind==XK_GAS;float g=xg(w);
 if(w==2)XS.heat=60*120;if(w==10)XS.heat=60*150;XP.vis|=1u<<w;xp_write();int ast=0;int leave=0;
 for(XS.t=0;;XS.t++){next_frame();u16 ky=keys,pr=pressed;pressed=0;
  if(au){ky=0;pr=0;if(ast<5){if(XS.exsel!=ast)pr|=K_SEL;else pr|=K_A;}else if(ast==5){if(gas)pr|=K_DOWN;else if(XS.x<15.6f)ky|=K_RIGHT;else pr|=K_DOWN;}else if(ast==6){if(XS.x>2)ky|=K_LEFT;else pr|=K_B;}}
  if(pr&K_SEL){XS.exsel=(XS.exsel+1)%5;sfx_menu();}
  if(pr&K_A){xs_runexp(XS.exsel);if(au)ast++;continue;}
  if(pr&K_DOWN){if(!XS.sample&&(gas||ffabs(XS.x-16)<1.5f)){XS.sample=1;XP.samp|=1u<<w;xp_write();music_victory();xs_msg(gas?"ATMOSPHERE SAMPLE BOTTLED":"SAMPLE BAGGED AND TAGGED");if(au)ast=6;}else xs_msg(gas?"SAMPLE ALREADY TAKEN":"WALK TO THE GREEN MARKER");}
  if(pr&K_B){if(ffabs(XS.x)<3||gas)leave=1;else xs_msg("WALK BACK TO THE LANDER TO LEAVE");}
  if(XS.heat>0){XS.heat--;if(XS.heat==0){xs_msg(w==2?"COOLING LIMIT - BACK TO THE LANDER":"RADIATION LIMIT - BACK TO THE LANDER");leave=2;}}
  float spd=1.4f;int ice=XWD[w].haz==XH_ICE;if(XS.ground||XS.y<=0){if(ice){float a=0;if(ky&K_RIGHT)a=0.8f;if(ky&K_LEFT)a=-0.8f;XS.vx+=a/60-XS.vx*0.004f;if(XS.vx>spd)XS.vx=spd;if(XS.vx<-spd)XS.vx=-spd;}else XS.vx=(ky&K_RIGHT)?spd:(ky&K_LEFT)?-spd:0;}
  if(ky&K_RIGHT)XS.face=0;if(ky&K_LEFT)XS.face=1;
  if(XS.y<=0&&(pr&K_UP)){float v0=3.1f;float cap=fsqr(2*g*3);if(v0>cap)v0=cap;XS.vy=v0;}
  if(XS.y<=0&&XS.vx!=0&&g<2&&!(pr&K_UP))XS.vy=fsqr(2*g*0.08f);
  XS.vy-=g/60;XS.y+=XS.vy/60;if(XS.y<=0){XS.y=0;XS.vy=0;}XS.x+=XS.vx/60;if(XS.x<-20)XS.x=-20;if(XS.x>40)XS.x=40;if(XS.vx!=0)XS.wf++;
  XS.camx+=(XS.x-XS.camx)*0.1f;
  BEAT->screen=SCR_XSURF;BEAT->mode=w;BEAT->phase=2+XS.sample;BEAT->x=(int)(XS.x*10);BEAT->y=(int)(XS.y*100);BEAT->scoreA=xp_nsamp();BEAT->scoreB=XP.exp[w];BEAT->timer=XS.heat/60;
  xs_scene(10,130);xs_astro(XS.x,XS.y,10,130,(XS.wf>>3)&1);
  char b[64],*o=sp_put(b,"SELECT: ");o=sp_put(o,XEN[XS.exsel]);o=sp_put(o,"  [");for(int k=0;k<5;k++)*o++=(XP.exp[w]>>k)&1?'*':'-';*o++=']';*o=0;
  xs_bottom(XS.msgT>0?XS.msg:b,"A:RUN UP:JUMP DOWN:SAMPLE B:LEAVE");if(XS.msgT>0)XS.msgT--;m3_present();
  if(leave){for(int k=0;k<60;k++)next_frame();return;}}}
// ---------------------------------------------------------------- observation views (stars, black hole)
static void x_view(int w){const XW*W=&XWD[w];XP.vis|=1u<<w;xp_write();
 for(int t=0;;t++){next_frame();BEAT->screen=SCR_XGAL;BEAT->mode=w;BEAT->phase=7;lfb=0;lf(0,0,240,160,RGB(1,1,6));x_stars(w*7+3,70,160);
  if(w==0){for(int j=0;j<160;j++){int dy=j-200;int wd=isq(190*190-dy*dy);if(wd>0)lf(0,j,wd-60>0?(wd-60>240?240:wd-60):0,1,j>120?RGB(255,230,140):RGB(255,200,90));}for(int k=0;k<120;k++){u32 h=xh(k*3+t/8);int x=h%200,y=50+(h>>9)%110;if(isq((x+60)*(x+60)+(y-200)*(y-200))<185)sp_px(x,y,RGB(255,170,60));}
   for(int k=0;k<20;k++){int an=150+k*2;int R=128+(int)(xh(k)%6)+((t+k*5)%40)/8;int x=-60+(COS(an&255)*R>>8)+60,y=200+(SIN(an&255)*R>>8);sp_px(x+20,y,RGB(255,120,60));}ltc(14,"VIEW FROM 0.3 AU - SOLAR ORBITER RANGE",C_WHITE);}
  else if(w==26){for(int j=-90;j<=90;j++){int wd=isq(95*95-j*j);int y=80+j;if(y<0||y>=160)continue;lf(120-wd,y,wd*2,1,RGB(200,70,30));}for(int k=0;k<160;k++){u32 h=xh(k*5+(t/30));int x=40+h%160,y=(h>>9)%160;int dx=x-120,dy=y-80;if(dx*dx+dy*dy<88*88)lf(x,y,4,3,(h>>20)&1?RGB(240,110,50):RGB(160,50,24));}sp_blend(0,0,240,160,RGB(255,120,60),20);ltc(14,"RED SUPERGIANT - FILLS THE VIEW AT 10 AU",C_WHITE);}
  else if(w==27){int cx=120,cy=76;for(int a=0;a<128;a++){int an=a*2;int r=40+(a*7%5);int x=cx+(COS(an)*r>>8)*1,y=cy+(SIN(an)*r>>8)/4;u16 c=COS(an)>0?RGB(255,230,150):RGB(200,100,40);if(SIN(an)<0)lf(x-1,y,3,2,c);}
   sp_disc(cx,cy,18,0);sp_circ(cx,cy,19,RGB(255,220,150));sp_circ(cx,cy,20,RGB(200,140,70));for(int a=0;a<128;a++){int an=a*2;int r=40+(a*7%5);int x=cx+(COS(an)*r>>8),y=cy+(SIN(an)*r>>8)/4;u16 c=COS(an)>0?RGB(255,230,150):RGB(200,100,40);if(SIN(an)>=0)lf(x-1,y,3,2,c);}
   for(int a=0;a<64;a++)sp_px(cx+20+(COS(a*4)*70>>8),cy-10+(SIN(a*4)*24>>8),RGB(60,80,120));{int an=(t/2)&255;lf(cx+20+(COS(an)*70>>8)-1,cy-10+(SIN(an)*24>>8)-1,3,3,RGB(180,200,255));ltx(cx+40,cy+20,"S2",RGB(140,160,220));}
   ltc(14,"SGR A* - THE BRIGHT SIDE MOVES TOWARD YOU",C_WHITE);}
  else if(w==25){sp_disc(110,78,14,RGB(230,240,255));sp_disc(110,78,9,C_WHITE);lf(60,78,100,1,RGB(200,220,255));lf(110,30,1,96,RGB(200,220,255));lf(170,70,2,2,RGB(240,240,255));ltx(176,66,"SIRIUS B",RGB(170,190,230));ltc(14,"SIRIUS A + B (WHITE DWARF)",C_WHITE);}
  else{sp_disc(120,76,26,W->c2);sp_disc(120,76,22,W->c1);if((t/40)%5==0)sp_blend(126,58,14,10,RGB(255,240,200),160);ltc(14,"RED DWARF - FLARES ARE COMMON",C_WHITE);}
  lf(0,140,240,20,RGB(6,8,18));ltx(2,141,W->n,RGB(255,230,150));ltx(2,151,"OBSERVATION LOGGED - A/B: BACK",RGB(140,160,200));m3_present();
  if(t>20&&(pressed&(K_A|K_B))){pressed=0;return;}if(x_auto()&&t>150)return;}}
// ---------------------------------------------------------------- journal
static void x_journal(void){int pg=0;int L[XW_N];int n=0;for(int i=0;i<XW_N;i++)if((XP.vis>>i)&1)L[n++]=i;
 for(int redraw=1,t=0;;t++){next_frame();BEAT->screen=SCR_XJRNL;BEAT->mode=n;u16 pr=pressed;pressed=0;if(pr&(K_B|K_A))return;if(x_auto()&&t>120)return;if(pr&(K_RIGHT|K_R)&&pg*9+9<n){pg++;redraw=1;}if(pr&(K_LEFT|K_L)&&pg>0){pg--;redraw=1;}
  if(!redraw)continue;redraw=0;chrome(CAT_EXPLORE,0);banner("EXPEDITION JOURNAL","MEASURED vs EARTH (9.81 M/S2)",C_YEL);m3_box(4,38,232,118,1);
  ltx(10,44,"WORLD",C_GRAY);ltx(76,44,"G",C_GRAY);ltx(118,44,"JUMP",C_GRAY);ltx(166,44,"10M FALL",C_GRAY);
  if(!n)ltx(10,60,"NO WORLDS VISITED YET",C_WHITE);
  for(int r=0;r<9&&pg*9+r<n;r++){int w=L[pg*9+r];int y=56+r*10;char b[32];ltx(10,y,XWD[w].n,(XP.samp>>w)&1?C_GREEN:C_WHITE);xg_put(b,xg(w));ltx(76,y,b,C_WHITE);
   if(XP.jump[w]){u16 j=XP.jump[w];if(j&0x8000){char*o=sp_intc(b,j&0x7FFF);sp_put(o,"M");}else{char*o=sp_int(b,j/100);*o++='.';*o++='0'+j/10%10;*o++='0'+j%10;sp_put(o,"M");}ltx(118,y,b,C_WHITE);}else ltx(118,y,"-",C_GRAY);
   if(XP.fall[w]){u16 f=XP.fall[w];char*o=sp_int(b,f/100);*o++='.';*o++='0'+f/10%10;*o++='0'+f%10;sp_put(o,"S");ltx(166,y,b,C_WHITE);}else ltx(166,y,"-",C_GRAY);
   char*o=b;for(int k=0;k<5;k++)*o++=(XP.exp[w]>>k)&1?'*':'-';*o=0;ltx(204,y,b,RGB(255,220,120));}
  m3_ctext(146,n>9?"</>: PAGE   B: BACK":"B: BACK",C_GRAY,1);m3_present();}}
// ---------------------------------------------------------------- main
static void x_visit(int w){XP.last=w<XW_SOL?w:XP.last;if(!x_facts(w))return;const XW*W=&XWD[w];
 if(W->kind==XK_STAR){x_view(w);return;}bgm_t(TR_FUTURE);int ok=x_land(w);if(ok)x_surface(w);xp_write();bgm_t(TR_SPACE);}
static void astro_explorer(void){xp_read();bgm_t(TR_SPACE);int sel=XP.last<XW_SOL?XP.last:3,gs=0;
 for(;;){int r=x_solmap(sel);if(r==-1){bgCached=-1;return;}if(r==-3){x_journal();continue;}
  if(r==-2){for(;;){int g=x_galmap(gs);if(g<0)break;gs=g;x_warp(21+g,1);x_visit(21+g);x_warp(21+g,0);}continue;}
  sel=r;x_visit(r);}}
static void rec_astro(const char**L){xp_read();static char b[3][40]EWRAM;int nv=0,ne=0;for(int i=0;i<XW_N;i++){nv+=(XP.vis>>i)&1;for(int k=0;k<5;k++)ne+=(XP.exp[i]>>k)&1;}
 char*o=sp_put(b[0],"WORLDS VISITED: ");o=sp_int(o,nv);sp_put(o,"/28");o=sp_put(b[1],"SAMPLES: ");o=sp_int(o,xp_nsamp());sp_put(o,"/23");o=sp_put(b[2],"EXPERIMENTS: ");o=sp_int(o,ne);sp_put(o,"/115");
 L[0]=b[0];L[1]=b[1];L[2]=b[2];L[3]=xp_nsamp()>=3?"WARP DRIVE: UNLOCKED":"WARP DRIVE: 3 SAMPLES TO UNLOCK";L[4]="REAL SURFACE GRAVITY + EXPERIMENTS";}
