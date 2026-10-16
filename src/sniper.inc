// ===================================================================== v1.3 SNIPER
// Scoped long-range shooting: 5-layer panorama, real-time bullet flight (drag model), drop vs 100 m zero, wind drift (lag
// time), sway + breath hold, laser range finder, spotter callouts, counter-sniper glints, HVT/civilians, exfil vs QRF.
// Controls: D-pad aim/pan  R scope  L zoom (L+D-pad dial elevation/windage)  B hold breath  A fire  SELECT lase+spotter
#define SCR_SNSEL 60
#define SCR_SNB 61
#define SCR_SN 62
#define SN_OFF 0xA00
#define SN_NM 14
enum{SO_HVT,SO_KILL,SO_DUEL,SO_OVER,SO_VEH,SO_SILENT,SO_HOST};
enum{SK_SOLD,SK_HVT,SK_CIV,SK_CS,SK_VEH,SK_QRF,SK_OFF};
typedef struct{const char*n,*b1,*b2;u8 era,th,obj,nt,ncs,nciv,need,wind;const char*rif;}SnM;
static const SnM SM[SN_NM]={
 {"RED OCTOBER","STALINGRAD RUINS, 1942.","HVT: OFFICER IN RED-BANDED CAP.",0,0,SO_HVT,6,0,0,1,2,"MOSIN PU"},
 {"HEDGEROW HUNT","NORMANDY BOCAGE, 1944.","KILL THE CHURCH-TOWER SNIPER + 3.",0,1,SO_DUEL,4,1,0,4,3,"NO.4 MK I(T)"},
 {"VIMY RIDGE","WESTERN FRONT, 1917.","SILENCE 4 MACHINE-GUN CREW.",0,2,SO_KILL,7,0,0,4,4,"SMLE MK III* SNIPER"},
 {"PACIFIC RIDGE","JUNGLE RIDGE, 1944.","OVERWATCH: PROTECT THE PATROL.",0,3,SO_OVER,0,0,0,8,3,"M1C/M1D GARAND"},
 {"HILL 881","HIGHLANDS, 1967.","HVT: CAPTAIN WITH RED BERET.",1,4,SO_HVT,7,1,0,1,5,"M40"},
 {"BERLIN OVERWATCH","DIVIDED CITY, 1984.","3 GUNMEN HOLD A HOSTAGE. NO CIVS.",1,5,SO_HOST,3,0,3,3,2,"HK PSG-1"},
 {"MOUNTAIN PASS","HIGH VALLEY, MODERN.","HVT CONVOY LEADER AT 900-1600M.",2,6,SO_HVT,7,1,0,1,6,"L115A3"},
 {"ROOFTOPS","CITY BLOCK, MODERN.","2 ENEMY SNIPERS ON THE ROOFS.",2,5,SO_DUEL,4,2,2,6,3,"M110 SASS"},
 {"ENGINE BLOCK","DESERT ROAD, MODERN.","STOP 3 TRUCKS: HIT THE ENGINE.",2,7,SO_VEH,4,0,0,3,4,"M107"},
 {"THE LONG WALK","HIGH DESERT, MODERN.","HVT AT 1800-2300M. GUSTY WIND.",2,6,SO_HVT,5,0,0,1,9,"CHEYTAC M200"},
 {"ARCTIC STATION","POLAR OUTPOST, MODERN.","SNIPER DUEL: 2 COUNTER-SNIPERS.",2,8,SO_DUEL,3,2,0,5,5,"SAKO TRG-42"},
 {"SILENT NIGHT","FOREST CAMP, NIGHT.","5 SENTRIES. STAY UNDETECTED.",2,9,SO_SILENT,5,0,0,5,1,"VSS VINTOREZ"},
 {"ORBITAL RAIL","LUNAR MINING BASE.","DROIDS AT 2-3 KM. RAIL FLIES FLAT.",3,10,SO_KILL,7,0,0,7,0,"RAILGUN SNIPER"},
 {"SMART ROUND","NEON MEGACITY.","SELECT TAGS - ROUNDS STEER.",3,11,SO_HVT,7,1,2,1,4,"SMART-ROUND DMR"}};
static const char*const SON[7]={"ELIMINATE THE HVT","KILL HOSTILES","COUNTER-SNIPER","OVERWATCH","DISABLE VEHICLES","SILENT KILLS","HOSTAGE RESCUE"};
// themes: sky top, sky horizon, 5 layer colours (far->near), distance scale %, ridge style
typedef struct{u16 s0,s1,L[5];u8 dsc,sty;}SnTh;
static const SnTh STH[12]={
 {RGB(120,110,100),RGB(210,170,120),{RGB(130,120,110),RGB(110,100,92),RGB(120,104,88),RGB(104,90,76),RGB(92,80,66)},50,1},   // ruins
 {RGB(110,150,210),RGB(200,220,230),{RGB(110,140,120),RGB(90,140,70),RGB(78,128,56),RGB(66,112,46),RGB(58,98,40)},50,2},     // bocage
 {RGB(120,124,128),RGB(170,170,160),{RGB(120,118,110),RGB(110,96,78),RGB(100,84,62),RGB(92,74,52),RGB(84,66,46)},45,3},     // trench
 {RGB(90,150,200),RGB(200,230,220),{RGB(70,120,110),RGB(46,110,60),RGB(36,98,44),RGB(30,84,36),RGB(26,72,30)},50,4},        // jungle
 {RGB(120,170,220),RGB(220,230,220),{RGB(96,140,120),RGB(80,140,70),RGB(110,140,60),RGB(150,110,64),RGB(84,120,50)},70,2},  // hills
 {RGB(130,150,180),RGB(210,210,210),{RGB(140,146,160),RGB(120,124,134),RGB(106,110,118),RGB(96,98,104),RGB(84,84,88)},45,1},// city
 {RGB(80,130,210),RGB(200,214,230),{RGB(200,206,220),RGB(150,130,110),RGB(140,116,90),RGB(124,102,78),RGB(110,92,70)},100,0},// mountain
 {RGB(110,160,220),RGB(240,220,180),{RGB(210,180,140),RGB(214,186,130),RGB(206,174,118),RGB(196,162,104),RGB(186,150,94)},100,5},// desert
 {RGB(150,180,220),RGB(230,236,244),{RGB(200,210,226),RGB(220,226,236),RGB(232,236,242),RGB(236,240,246),RGB(244,246,250)},90,0},// snow
 {RGB(8,12,30),RGB(30,40,70),{RGB(26,36,56),RGB(22,40,40),RGB(18,36,30),RGB(16,32,26),RGB(14,28,22)},40,4},                   // night
 {RGB(0,0,6),RGB(20,20,30),{RGB(110,110,116),RGB(130,130,136),RGB(146,146,150),RGB(160,160,164),RGB(176,176,180)},130,5},     // lunar
 {RGB(30,10,50),RGB(150,60,120),{RGB(70,40,100),RGB(60,40,80),RGB(50,40,70),RGB(44,36,60),RGB(36,30,50)},70,1}};             // neon city
static const u16 SN_DF[5]={2600,1700,1000,550,250},SN_DN[5]={1700,1000,550,250,70};
// ---------------------------------------------------------------- state
typedef struct{s32 wx;s16 x0,x1,D,tm,glint,aim,hp;u8 L,t8,k,st,alive,pose,face,hostile,tag,cover;s16 vx;}SnT;
#define NST 24
static SnT SNT[NST];static int nST;
typedef struct{s32 wx;s16 w,h;u8 L,t8,k,pad;u16 col;}SnO;
#define NSO 48
static SnO SOB[NSO];static int nSO;
static s32 RH[6][122] EWRAM; // ridge heights (8.8 world units) every 8 wu
static s16 colY[5][240] EWRAM;static u16 colTx[240] EWRAM;static s16 snSpan[160];
static u16 snPal[5][5],snSky[17];
static int snM,snR,snZi,snNZ,snScoped,snEra;static s32 snCX,snCY;int snZoom;
static int snT,snHP,snAmmo,snCyc,snRel,snBreath,snBreathOut,snKick,snElev,snWdg,snWind,snWindT,snAlert,snKills,snShots,snHits,snHeads,snLong,snScore,snPhase,snPhaseT,snFail,snDone,snHitT,snNeedK,snSquad,snCivHit;
static int snRange,dprevL,snHudF;static const char*snMsg;static int snMsgT;static char snMsgB[48];
typedef struct{s32 ax,ay;s16 t,tof;s32 sx,sy;u8 on,Ls,splash,guided;s16 tgt;}SnShot;static SnShot SS[4];
typedef struct{s32 wx,wy;s16 t;u8 L,k;}SnPuff;static SnPuff SP[8];
#define SNZ(e,i) (SN_ZOOMS[e][i]*256)
static const u8 SN_ZOOMS[4][3]={{3,4,4},{4,8,10},{5,10,15},{6,12,20}},SN_NZ[4]={2,3,3,3};
static void sn_msg(const char*s){int k=0;while(s[k]&&k<46){snMsgB[k]=s[k];k++;}snMsgB[k]=0;snMsg=snMsgB;snMsgT=150;snHudF=1;}
static u16 sn_mix(u16 a,u16 b,int f){int r=(a&31)+(((b&31)-(a&31))*f>>8),g=((a>>5)&31)+((((b>>5)&31)-((a>>5)&31))*f>>8),bl=((a>>10)&31)+((((b>>10)&31)-((a>>10)&31))*f>>8);return r|(g<<5)|(bl<<10);}
static u16 sn_scale(u16 a,int f){int r=(a&31)*f>>8,g=((a>>5)&31)*f>>8,b=((a>>10)&31)*f>>8;if(r>31)r=31;if(g>31)g=31;if(b>31)b=31;return r|(g<<5)|(b<<10);}
static s32 sn_h(int L,s32 wx){if(L>=5)return 172<<8;if(wx<0)wx=0;if(wx>(959<<8))wx=959<<8;int i=wx>>11,f=(wx>>3)&255;return RH[L][i]+(((RH[L][i+1]-RH[L][i])*f)>>8);}
static int sn_D(int L,int t8){const SnTh*T=&STH[SM[snM].th];int df=SN_DF[L]*T->dsc/100,dn=SN_DN[L]*T->dsc/100;return df+((dn-df)*t8>>8);}
static s32 sn_feet(const SnT*t){s32 a=sn_h(t->L,t->wx),b=sn_h(t->L+1,t->wx);return a+(((b-a)*t->t8)>>8);}
static s32 sn_m(int m10,int D){return (s32)m10*19200/(D<1?1:D);} // metres*10 at distance D -> world units (8.8)
// ---------------------------------------------------------------- ballistics (float, only per shot / callout)
static float sn_exp(float x){float r=1,t=1;for(int i=1;i<9;i++){t*=x/i;r+=t;}return r;}
static float sn_tof(const SRif*r,float D){float k=r->k*0.000085f;if(k<0.00001f)return D/r->v0;return (sn_exp(k*D)-1)/(k*r->v0);}
static float sn_dropm(const SRif*r,float D){float t=sn_tof(r,D);return 4.905f*t*t;}
static float sn_dropmil(const SRif*r,float D){if(D<5)D=5;float rel=sn_dropm(r,D)-sn_dropm(r,100)*D/100.0f;return rel/D*1000.0f;}
static float sn_driftmil(const SRif*r,float D,float W){if(D<5)D=5;return W*(sn_tof(r,D)-D/r->v0)/D*1000.0f;}
static int f10(float v){return (int)(v*10+(v>=0?0.5f:-0.5f));}
static const char*m10s(int v){static char b[2][12];static int q;char*o=b[q++&1];int k=0;if(v<0){o[k++]='-';v=-v;}const char*a=itoa(v/10);while(*a)o[k++]=*a++;o[k++]='.';o[k++]='0'+v%10;o[k]=0;return o;}
// ---------------------------------------------------------------- scene generation
static u32 snSeed;static int sr(int n){snSeed=snSeed*1103515245u+12345u;return (int)((snSeed>>16)%(u32)n);}
static void sn_gen(void){const SnM*M=&SM[snM];const SnTh*T=&STH[M->th];snSeed=0x5EED+snM*977;
 static const u8 HB[5]={60,78,94,110,130},AMP[6][5]={{30,14,9,6,4},{6,5,4,3,2},{12,8,5,4,3},{6,4,3,3,2},{16,10,7,5,3},{8,5,3,2,2}};
 for(int L=0;L<5;L++){int p1=sr(256),p2=sr(256),p3=sr(256),a=AMP[T->sty][L];int walk=0;
  for(int i=0;i<122;i++){walk+=sr(5)-2;if(walk>6)walk=6;if(walk<-6)walk=-6;
   int h=HB[L]*256+(SIN(i*3+p1)*a)+(SIN(i*7+p2)*a/3)+(SIN(i*13+p3)*a/6)+walk*(a*12);RH[L][i]=h;}}
 for(int i=0;i<122;i++){RH[5][i]=172<<8;for(int L=1;L<5;L++)if(RH[L][i]<RH[L-1][i]+(6<<8))RH[L][i]=RH[L-1][i]+(6<<8);}
 // palettes with atmospheric fog
 static const u8 FOG[5]={150,100,60,26,0};static const u16 SH[5]={208,236,256,282,330};
 for(int L=0;L<5;L++)for(int s=0;s<5;s++)snPal[L][s]=sn_mix(sn_scale(T->L[L],SH[s]),T->s1,FOG[L]);
 for(int i=0;i<17;i++)snSky[i]=sn_mix(T->s0,T->s1,i*16>255?255:i*16);
 // objects
 nSO=0;int sty=M->th;
 #define OBJ(L_,x_,t_,w_,h_,k_,c_) if(nSO<NSO){SnO*o=&SOB[nSO++];o->L=L_;o->wx=(x_)<<8;o->t8=t_;o->w=w_;o->h=h_;o->k=k_;o->col=c_;}
 for(int n=0;n<40&&nSO<NSO;n++){int L=1+sr(3),x=20+sr(920),t=40+sr(140);
  switch(sty){
  case 0:case 5:case 11:if(n<26){int w=60+sr(160),h=80+sr(220);OBJ(L,x,t,w,h,sty==0?(sr(2)?2:1):1,sty==11?sn_mix(T->L[L],RGB(60,30,90),80):sn_scale(T->L[L],230+sr(40)));}break; // buildings (m*10)
  case 1:if(n<10){OBJ(2+sr(2),20,60+sr(120),9400,18,3,sn_scale(T->L[3],170));}else if(n<28){OBJ(L,x,t,40+sr(40),60+sr(60),4,sn_scale(T->L[L],180));}break; // hedgerows + trees
  case 2:if(n<3){OBJ(3+(n>0),0,90+n*40,9600,14,5,RGB(60,46,34));}else if(n<20){OBJ(L,x,t,6,50+sr(50),6,RGB(70,60,50));}else if(n<30){OBJ(3+sr(2),x,t,40+sr(40),10,7,RGB(70,58,44));}break; // trench lines, dead trees, craters
  case 3:case 9:if(n<34){OBJ(L,x,t,50+sr(70),90+sr(120),4,sn_scale(T->L[L],160+sr(60)));}break; // canopy trees
  case 4:if(n<14){OBJ(L,x,t,40+sr(50),70+sr(60),4,sn_scale(T->L[L],170));}else if(n<20){OBJ(2+sr(3),x,t,60,18,8,RGB(130,120,80));}break; // trees + sandbag bunkers
  case 6:if(n<10){OBJ(L,x,t,30+sr(50),20+sr(30),7,sn_scale(T->L[L],200));}else if(n<16){OBJ(2+sr(2),300+sr(360),t,80+sr(60),40+sr(30),1,RGB(170,150,120));}break; // rocks + compound
  case 7:if(n<2){OBJ(3,0,120+n*30,9600,8,5,RGB(110,104,96));}else if(n<8){OBJ(2+sr(2),200+sr(560),t,90+sr(80),35+sr(20),1,RGB(200,184,150));}else if(n<16){OBJ(L,x,t,30+sr(30),12+sr(14),7,sn_scale(T->L[L],180));}break; // road, compounds, rocks
  case 8:if(n<20){OBJ(L,x,t,30+sr(30),80+sr(80),9,RGB(40,70,60));}else if(n<26){OBJ(2+sr(2),250+sr(450),t,100+sr(80),40+sr(30),1,RGB(180,70,50));}break; // pines + station huts
  case 10:if(n<10){OBJ(L,x,t,80+sr(100),50+sr(40),10,RGB(180,180,190));}else if(n<16){OBJ(L,x,t,14,200+sr(160),1,RGB(150,150,170));}break; // domes + towers
  }}
 if(sty==1){OBJ(2,480,110,90,420,11,RGB(150,140,130));} // church tower (counter-sniper nest)
 #undef OBJ
 // targets
 nST=0;memset(SNT,0,sizeof SNT);int dmin=M->era==3?0:1;
 for(int i=0;i<M->nt+M->ncs+M->nciv+(M->obj==SO_HVT||M->obj==SO_HOST?0:0);i++){if(nST>=NST)break;SnT*t=&SNT[nST++];t->alive=1;t->hp=1;t->face=sr(2)?1:-1;
  int k=SK_SOLD;if(i<M->nt){k=SK_SOLD;if(M->obj==SO_HVT&&i==0)k=SK_HVT;if(M->obj==SO_VEH&&i<M->need)k=SK_VEH;if(M->obj==SO_KILL&&snM==2&&i<4)k=SK_SOLD;}
  else if(i<M->nt+M->ncs)k=SK_CS;else k=SK_CIV;
  t->k=k;t->hostile=k!=SK_CIV;
  int L=1+sr(3);if(M->obj==SO_HVT&&snM==9&&k==SK_HVT)L=0;if(snM==6&&k==SK_HVT)L=1;if(k==SK_VEH)L=3;if(k==SK_CS)L=1+sr(2);if(M->obj==SO_HOST)L=3;
  if(L<dmin)L=dmin;t->L=L;t->t8=k==SK_VEH?140:40+sr(160);
  int x=60+sr(840);if(M->obj==SO_HOST)x=440+i*14;if(snM==1&&k==SK_CS){t->L=2;x=480;t->t8=110;}
  t->wx=x<<8;t->x0=x-30-sr(40);t->x1=x+30+sr(40);t->D=sn_D(t->L,t->t8);
  t->st=sr(3)==0?0:1;t->pose=k==SK_CS?2:0;if(k==SK_VEH){t->hp=2;t->st=1;t->x0=0;t->x1=959;}
  t->tm=sr(200);}
 if(M->obj==SO_HVT){SnT*t=&SNT[0];if(snM==9){t->L=0;t->t8=120;}t->D=sn_D(t->L,t->t8);}}
// ---------------------------------------------------------------- rendering
static int snCirc,snOX,snOY;static u16*snDst;static s32 kCX=-1,kCY,kZ;
#define snBg ((u16*)dl_bg) // static background cache (aliases menu bg cache; bgCached=-1 on exit)
static int snRX0=0,snRX1=240,snRY0=0,snRY1=160; // dirty rect for cache renders
IWCODE static void sn_terrain(s32 cx,s32 cy,int zoom){
 s32 step=(1<<16)/zoom;u16*f=snBg;int rim=zoom>>9;if(rim<1)rim=1;
 u32 rowH[160];u16 skyR[160];{s32 wy=cy-80*step;for(int y=0;y<160;y++,wy+=step){rowH[y]=(u32)(wy>>6)*0x85EBCA77u;int si=((wy>>8)*205)>>11;if(si<0)si=0;if(si>16)si=16;skyR[y]=snSky[si];}}
 const int R0=snRY0,R1=snRY1;for(int sx=snRX0;sx<snRX1;sx++){s32 wx=cx+(sx-120)*step;s32 w=wx;if(w<0)w=0;if(w>(959<<8))w=959<<8;int i=w>>11,fr=(w>>3)&255;
  int yy[5];
  for(int L=0;L<5;L++){s32 h=RH[L][i]+(((RH[L][i+1]-RH[L][i])*fr)>>8);s32 y=80+(((h-cy)>>4)*zoom>>12);if(y<-1000)y=-1000;if(y>1000)y=1000;yy[L]=y;colY[L][sx]=(s16)y;}
  u32 hx=(u32)(wx>>6)*0x9E3779B1u;
  int end=160;
  for(int L=4;L>=0;L--){int a=yy[L],e=end;if(a<end)end=a;if(e>160)e=160;int st=a<R0?R0:a;if(e>R1)e=R1;if(st>=e)continue;
   const u16*pal=snPal[L];u16*p=f+st*240+sx;int y=st;int re=a+rim;if(re>e)re=e;if(y>re)re=y;
   for(;y<re;y++,p+=240)*p=pal[4];
   const int hs=zoom>=2048?31:30;const u16*pq=pal+(zoom>=2048);for(;y<e;y++,p+=240){u32 hh=hx^rowH[y];hh^=hh>>15;*p=pq[hh>>hs];}}
  if(end>R1)end=R1;{u16*p=f+R0*240+sx;for(int y=R0;y<end;y++,p+=240)*p=skyR[y];}}}
IWCODE static void sn_fill(int x0,int y0,int x1,int y1,u16 c,int L){if(snDst==snBg){if(x0<snRX0)x0=snRX0;if(x1>snRX1-1)x1=snRX1-1;if(y0<snRY0)y0=snRY0;if(y1>snRY1-1)y1=snRY1-1;}if(x1<x0||y1<y0)return;if(y0<0)y0=0;if(y1>159)y1=159;if(x0<0)x0=0;if(x1>239)x1=239;u16*D=snDst;
 for(int x=x0;x<=x1;x++){int a=y0,b=y1;if(snCirc){int dx=x-120;if(dx<-70||dx>69)continue;int h=snSpan[80+dx];if(a<80-h)a=80-h;if(b>80+h-1)b=80+h-1;}
  if(L<4){int cx=x-snOX;if(cx<0)cx=0;if(cx>239)cx=239;int oc=colY[L+1][cx]+snOY-1;if(b>oc)b=oc;}u16*p=D+a*240+x;for(int y=a;y<=b;y++,p+=240)*p=c;}}
static int sn_sx(s32 wx){return 120+snOX+(((wx-snCX)>>4)*snZoom>>12);}
static int sn_sy(s32 wy){return 80+snOY+(((wy-snCY)>>4)*snZoom>>12);}
static void sn_movr(const void*src,void*dst,int n){if(n<=0)return;*(volatile u32*)0x040000D4=(u32)((const u16*)src+n-1);*(volatile u32*)0x040000D8=(u32)((u16*)dst+n-1);*(volatile u32*)0x040000DC=(u32)n|0x80000000u|(1u<<21)|(1u<<23);}
static void dma16c(const void*src,void*dst,int n){if(n<=0)return;*(volatile u32*)0x040000D4=(u32)src;*(volatile u32*)0x040000D8=(u32)dst;*(volatile u32*)0x040000DC=(u32)n|0x80000000u;}
static int sn_px8(s32 w){return (w>>4)*snZoom>>12;} // world 8.8 length -> pixels
static void sn_obj(const SnO*o){int D=sn_D(o->L,o->t8);s32 fy=sn_h(o->L,o->wx);fy+=((sn_h(o->L+1,o->wx)-fy)*o->t8)>>8;
 int w=sn_px8(sn_m(o->w,D)),h=sn_px8(sn_m(o->h,D));if(o->w>=9000)w=sn_px8(9600<<8);
 int x=o->w>=9000?sn_sx(0):sn_sx(o->wx)-w/2,y=sn_sy(fy);if(x>240||x+w<0||y-h>160||y<0)return;if(w<1)w=1;if(h<1)h=1;
 switch(o->k){
 case 1:case 2:sn_fill(x,y-h,x+w,y,o->col,o->L);if(w>8&&h>8){u16 wc=sn_scale(o->col,150);int ws=w/6>2?w/6:2,hs=h/8>2?h/8:2;
   for(int yy=y-h+hs;yy<y-hs;yy+=hs*2)for(int xx=x+ws;xx<x+w-ws;xx+=ws*2)sn_fill(xx,yy,xx+ws/2,yy+hs/2,wc,o->L);}
  if(o->k==2&&w>4){for(int i=0;i<4;i++){int a=x+i*w/4;sn_fill(a,y-h-(i&1?h/6:0),a+w/8,y-h,STH[SM[snM].th].s1,o->L);}}break; // ruined top: bites out of the roofline
 case 3:case 5:sn_fill(x,y-h,x+w,y,o->col,o->L);break;
 case 4:case 9:{int cx=x+w/2;for(int yy=0;yy<h;yy++){int hw=o->k==9?(w*yy/(2*(h>0?h:1))):(w/2)*(yy<h/2?yy*2:h*2-yy*2)/(h>0?h:1)+1;sn_fill(cx-hw,y-h+yy,cx+hw,y-h+yy,o->col,o->L);}sn_fill(cx,y-h/5,cx+(w>20?2:0),y,RGB(70,50,30),o->L);break;}
 case 6:sn_fill(x,y-h,x+(w>2?w:1),y,o->col,o->L);break;
 case 7:case 8:case 10:{int cx=x+w/2,hh=h;for(int yy=0;yy<hh;yy++){int r=yy*yy*4/((hh*hh)>0?hh*hh:1);int hw=(w/2)*(4-r)/4;if(o->k==10)hw=w/2;sn_fill(cx-hw,y-hh+yy,cx+hw,y-hh+yy,o->col,o->L);}break;}
 case 11:sn_fill(x,y-h,x+w,y,o->col,o->L);sn_fill(x-w/4,y-h-h/8,x+w+w/4,y-h,sn_scale(o->col,150),o->L);sn_fill(x+w/3,y-h+h/14,x+2*w/3,y-h+h/7,RGB(20,20,24),o->L);break;}}
static void sn_man(const SnT*t){s32 fy=sn_feet(t);int D=t->D;int x=sn_sx(t->wx),y=sn_sy(fy);
 int Hm=t->pose==2?5:t->pose==1?11:18;int H=sn_px8(sn_m(Hm,D)),W=sn_px8(sn_m(t->pose==2?17:5,D));if(x<-40||x>280||y<-20||y>200)return;
 if(t->k==SK_VEH){int vw=sn_px8(sn_m(60,D)),vh=sn_px8(sn_m(26,D));if(vw<2)vw=2;if(vh<1)vh=1;int dir=t->face;u16 c=t->hp>0?RGB(150,140,110):RGB(60,56,50);
  sn_fill(x-vw/2,y-vh,x+vw/2,y-vh/4,c,t->L);int cab=dir>0?x+vw/2-vw/4:x-vw/2;sn_fill(cab,y-vh-vh/3,cab+vw/4,y-vh,sn_scale(c,200),t->L);
  sn_fill(x-vw/2+vw/8,y-vh/4,x-vw/2+vw/4,y,RGB(20,20,20),t->L);sn_fill(x+vw/2-vw/4,y-vh/4,x+vw/2-vw/8,y,RGB(20,20,20),t->L);if(t->hp<=0&&(snT&8))sn_fill(cab,y-vh*2,cab+vw/5,y-vh-vh/3,RGB(80,80,80),t->L);return;}
 if(H<1)H=1;if(W<1)W=1;
 u16 uni=SM[snM].era==0?RGB(110,100,70):SM[snM].era==1?RGB(80,96,60):SM[snM].era==2?RGB(150,136,100):RGB(90,90,110);
 if(t->k==SK_CIV)uni=RGB(210,210,220);if(t->k==SK_HVT)uni=RGB(70,74,80);if(t->k==SK_CS)uni=sn_scale(STH[SM[snM].th].L[t->L],140);if(SM[snM].th==10||SM[snM].th==11)uni=t->k==SK_CIV?RGB(200,200,220):RGB(180,60,60);
 if(!t->alive){sn_fill(x-W*2,y-W/2,x+W*2,y,sn_scale(uni,160),t->L);return;}
 if(t->pose==2){sn_fill(x-W/2,y-H,x+W/2,y,uni,t->L);sn_fill(t->face>0?x+W/2:x-W/2-H,y-H,t->face>0?x+W/2+H:x-W/2,y-1,RGB(214,170,130),t->L);
  if(t->glint>0&&(snT&2)){sn_fill(x+t->face*W/2-1,y-H-1,x+t->face*W/2+1,y-H+1,RGB(255,255,255),t->L);}return;}
 int legs=H*45/100,torso=H*36/100,head=H-legs-torso;int st=(t->st==1||t->st==3)&&((snT>>3)&1);
 sn_fill(x-W/2,y-legs,x-W/2+W/3,y-(st?1:0),sn_scale(uni,180),t->L);sn_fill(x+W/2-W/3,y-legs,x+W/2,y-(st?0:1),sn_scale(uni,180),t->L);
 sn_fill(x-W/2,y-legs-torso,x+W/2,y-legs,uni,t->L);
 int hw=W/3;sn_fill(x-hw,y-H,x+hw,y-legs-torso,RGB(214,170,130),t->L);
 if(t->k==SK_HVT){sn_fill(x-hw-1,y-H-(head>3?1:0),x+hw+1,y-H+head/3,RGB(200,40,40),t->L);}
 else if(t->k!=SK_CIV){sn_fill(x-hw,y-H,x+hw,y-H+head/3,sn_scale(uni,140),t->L);}
 if(t->hostile&&t->k!=SK_CIV){int gy=y-legs-torso/2;sn_fill(t->face>0?x:x-W,gy,t->face>0?x+W:x,gy+(H>30?1:0),RGB(30,30,30),t->L);}
 if(t->tag&&(snT&8))sn_fill(x-1,y-H-4,x+1,y-H-2,RGB(255,60,60),t->L);}
static void sn_reticle(void){int mil=(192*snZoom)>>16;if(mil<2)mil=2;u16 c=RGB(0,0,0);
 for(int i=-70;i<=70;i++){if(iabs(i)<3)continue;int w=iabs(i)>mil*5?1:0;fb[80*240+120+i]=c;fb[(80+i)*240+120]=c;if(w){fb[81*240+120+i]=c;fb[(80+i)*240+121]=c;}}
 for(int k=1;k<=10;k++){int d=k*mil;if(d>68)break;int big=(k%5)==0;for(int a=-big;a<=big;a++){fb[(80+d)*240+120+a-1]=c;fb[(80+d)*240+121+a]=c;if(k<=5){fb[(80-d)*240+120+a]=c;fb[(80+a)*240+120+d]=c;fb[(80+a)*240+120-d]=c;}}}}
static void sn_hud_text(int x,int y,const char*s,u16 c){m3_text(x,y,s,c,1);}
static void sn_draw(void){const SnM*M=&SM[snM];const SRif*R=&SR[snR];
 int swx=0,swy=0;{int A=R->sway*12;if(snBreath>0)A=A/5;else if(snBreathOut>0)A=A*5/2;A=A*192/100;swx=SIN(snT*3)*A>>8;swy=(SIN(snT*5+40)*A>>8)*2/3;}
 if(snCX!=kCX||snCY!=kCY||snZoom!=kZ){snOX=snOY=0;snDst=snBg;snCirc=0;s32 st=(1<<16)/snZoom;snRX0=0;snRX1=240;snRY0=0;snRY1=160;
  if(kCX>=0&&snZoom==kZ){s32 dx=snCX-kCX,dy=snCY-kCY;int px=dx/st,py=dy/st;
   if(px*st==dx&&py*st==dy&&(px==0||py==0)&&iabs(px)<100&&iabs(py)<60){
    if(px>0){for(int y=0;y<160;y++)dma16c(snBg+y*240+px,snBg+y*240,240-px);for(int L=0;L<5;L++)dma16c(colY[L]+px,colY[L],240-px);snRX0=240-px;}
    else if(px<0){int q=-px;for(int y=0;y<160;y++)sn_movr(snBg+y*240,snBg+y*240+q,240-q);for(int L=0;L<5;L++)sn_movr(colY[L],colY[L]+q,240-q);snRX1=q;}
    else if(py>0){dma16c(snBg+py*240,snBg,240*(160-py));snRY0=160-py;}
    else if(py<0){int q=-py;sn_movr(snBg,snBg+q*240,240*(160-q));snRY1=q;}}}
  sn_terrain(snCX,snCY,snZoom);for(int L=0;L<5;L++)for(int i=0;i<nSO;i++)if(SOB[i].L==L)sn_obj(&SOB[i]);snRX0=0;snRX1=240;snRY0=0;snRY1=160;kCX=snCX;kCY=snCY;kZ=snZoom;}
 {int ox=-(((swx)>>4)*snZoom>>12),oy=-(((swy-snKick*48)>>4)*snZoom>>12);if(ox>45)ox=45;if(ox<-45)ox=-45;if(oy>10)oy=10;if(oy<-10)oy=-10;
  if(snScoped){for(int y=10;y<150;y++){int h=snSpan[y];if(!h)continue;dma16c(snBg+(y-oy)*240+120-h-ox,fb+y*240+120-h,2*h);}}
  else{for(int y=0;y<160;y++){int sy=y-oy;if(sy<0)sy=0;if(sy>159)sy=159;if(ox>=0)dma16c(snBg+sy*240,fb+y*240+ox,240-ox);else dma16c(snBg+sy*240-ox,fb+y*240,240+ox);}}
  snOX=ox;snOY=oy;snDst=fb;snCirc=snScoped;}
 for(int L=0;L<5;L++){for(int i=0;i<nST;i++)if(SNT[i].L==L)sn_man(&SNT[i]);
  for(int i=0;i<8;i++)if(SP[i].t>0&&SP[i].L==L){int r=1+(30-SP[i].t)/6;r=r*snZoom>>10;if(r<1)r=1;int x=sn_sx(SP[i].wx),y=sn_sy(SP[i].wy);sn_fill(x-r,y-r,x+r,y,SP[i].k?RGB(255,230,120):RGB(190,170,140),L);}}
 if(snScoped)sn_reticle();else{for(int i=-6;i<=6;i++){if(iabs(i)<2)continue;fb[80*240+120+i]=RGB(255,255,255);fb[(80+i)*240+120]=RGB(255,255,255);}}

 // HUD (scoped side panels are outside the circle: refresh every 6th frame)
 if(snScoped&&(snT%6)&&!snHudF)goto hudmsg;snHudF=0;
 if(snScoped){m3_fill(0,0,240,10,0);m3_fill(0,10,48,140,0);m3_fill(192,10,48,140,0);}
 else{m3_fill(0,0,240,9,RGB(10,12,20));m3_fill(0,151,240,9,RGB(10,12,20));}
 sn_hud_text(2,1,R->n,C_YEL);{const char*z=cat(itoa(snZoom>>8),"X");sn_hud_text(snScoped?204:214,1,snScoped?z:"NAKED",C_WHITE);}
 for(int i=0;i<3;i++)m3_fill(160+i*8,2,6,6,i<snHP?RGB(220,40,40):RGB(60,60,60));
 if(snScoped){sn_hud_text(2,14,"ELEV",C_GRAY);sn_hud_text(2,23,m10s(snElev*5),C_WHITE);sn_hud_text(2,34,"WNDG",C_GRAY);sn_hud_text(2,43,m10s(snWdg*5),C_WHITE);
  sn_hud_text(2,58,"WIND",C_GRAY);{int w=snWind;const char*a=w<0?"<":w>0?">":"-";sn_hud_text(2,67,cat3(a,itoa(iabs(w)),"M/S"),C_WHITE);}
  sn_hud_text(2,82,"BREATH",C_GRAY);m3_fill(2,91,40,4,RGB(40,40,40));m3_fill(2,91,snBreathOut>0?0:(snBreath>0?snBreath*40/240:40),4,snBreath>0?C_GREEN:RGB(120,180,220));
  sn_hud_text(196,14,"RANGE",C_GRAY);sn_hud_text(196,23,snRange?cat(itoa(snRange),"M"):"----",C_GREEN);
  sn_hud_text(196,38,"AMMO",C_GRAY);sn_hud_text(196,47,cat3(itoa(snAmmo),"/",itoa(R->mag)),snAmmo?C_WHITE:C_RED);
  if(snCyc>0)sn_hud_text(196,58,R->act?"BOLT":"CYCLE",C_YEL);if(snRel>0)sn_hud_text(196,58,"RELOAD",C_RED);
  sn_hud_text(196,72,"KILLS",C_GRAY);sn_hud_text(196,81,itoa(snKills),C_WHITE);
  if(M->obj==SO_OVER){sn_hud_text(196,96,"SQUAD",C_GRAY);sn_hud_text(196,105,itoa(snSquad),snSquad>2?C_GREEN:C_RED);}}
 hudmsg:
 if(snScoped)m3_fill(0,150,240,10,0);
 const char*ob=snPhase==1?cat3("EXFIL: HOLD ",itoa((snPhaseT+59)/60),"S - QRF INBOUND"):SON[M->obj];
 if(snMsgT>0){snMsgT--;sn_hud_text(snScoped?4:4,152,snMsg,C_YEL);}else sn_hud_text(4,152,ob,snPhase==1?C_RED:C_WHITE);
 if(!snScoped)sn_hud_text(4,1+0,"",0);
 if(snHitT>0){snHitT--;for(int x=0;x<240;x+=2){fb[x+10*240]=RGB(255,0,0);fb[x+149*240]=RGB(255,0,0);}}}
// ---------------------------------------------------------------- simulation
static SnT*sn_at_reticle(int tolMil){int best=-1,bd=1<<30;for(int i=0;i<nST;i++){SnT*t=&SNT[i];if(!t->alive&&t->k!=SK_VEH)continue;s32 fy=sn_feet(t);s32 cyT=fy-sn_m(t->pose==2?3:9,t->D);
  s32 dx=t->wx-snCX,dy=cyT-snCY;s32 d=iabs(dx)+iabs(dy);if(d<tolMil*192&&d<bd){bd=d;best=i;}}return best>=0?&SNT[best]:0;}
static int sn_terrainD(s32 wx,s32 wy,int*L){for(int l=4;l>=0;l--){s32 a=sn_h(l,wx);if(wy>=a){s32 b=sn_h(l+1,wx);int t8=b>a?(int)(((wy-a)<<8)/(b-a)):0;if(t8>255)t8=255;*L=l;return sn_D(l,t8);}}*L=-1;return 0;}
static void sn_lase(void){const SRif*R=&SR[snR];SnT*t=sn_at_reticle(3);int L;int D=t?t->D:sn_terrainD(snCX,snCY,&L);snRange=D;sfx_menu();
 if(!t){sn_msg(D?cat3("RANGE ",itoa(D),"M - NO TARGET"):"NO RETURN - SKY");return;}
 float dm=sn_dropmil(R,D),wm=sn_driftmil(R,D,snWind);int e=f10(dm),w=f10(-wm);
 static char b[48];const char*s=cat3(t->k==SK_CIV?"CIV ":t->k==SK_HVT?"HVT ":t->k==SK_VEH?"TRUCK ":"TANGO ",itoa(D),"M ");int k=0;while(s[k]&&k<40){b[k]=s[k];k++;}b[k]=0;
 const char*h=cat3("UP ",m10s(e),w>=0?cat(" R",m10s(w)):cat(" L",m10s(-w)));int j=0;while(h[j]&&k<46)b[k++]=h[j++];b[k]=0;sn_msg(b);}
static void sn_alert(int global,s32 wx,int D){for(int i=0;i<nST;i++){SnT*t=&SNT[i];if(!t->alive||t->k==SK_VEH)continue;
  if(!global){s32 r=sn_m(600,D);if(iabs(t->wx-wx)>r||iabs(t->D-D)>200)continue;}
  if(t->st<2){t->st=3;t->tm=90+sr(120);t->cover=sr(2);t->face=t->cover?1:-1;}}
 if(global){if(!snAlert)sn_msg("SPOTTER: THEY'RE ALERTED - MOVING!");snAlert=1;}}
static void sn_kill(SnT*t,int head,int D){t->alive=0;if(!t->hostile){snCivHit=1;sn_msg("CIVILIAN HIT - MISSION FAILED");snFail=1;return;}
 snKills++;int sc=100+D/5;if(head){sc+=sc/2;snHeads++;}if(t->k==SK_HVT)sc+=1000;if(t->k==SK_CS)sc+=400;snScore+=sc;if(D>snLong)snLong=D;
 sn_msg(t->k==SK_HVT?(head?"HVT DOWN - HEADSHOT":"HVT DOWN"):t->k==SK_CS?"COUNTER-SNIPER DOWN":head?cat3("HEADSHOT ",itoa(D),"M"):cat3("TANGO DOWN ",itoa(D),"M"));sfx_ok();}
static void sn_fire(void){const SRif*R=&SR[snR];if(snAmmo<=0||snCyc>0||snRel>0)return;
 int swx=0,swy=0;{int A=R->sway*12;if(snBreath>0)A=A/5;else if(snBreathOut>0)A=A*5/2;A=A*192/100;swx=SIN(snT*3)*A>>8;swy=(SIN(snT*5+40)*A>>8)*2/3;}
 if(!snScoped){swx*=3;swy*=3;}
 for(int i=0;i<4;i++)if(!SS[i].on){SnShot*s=&SS[i];s->on=1;s->ax=snCX+swx+snWdg*96;s->ay=snCY+swy-snElev*96;s->guided=0;s->tgt=-1;
   if(R->fl&SRF_GUIDE){for(int k=0;k<nST;k++)if(SNT[k].tag&&SNT[k].alive){s->guided=1;s->tgt=k;break;}}
   int L;int D=sn_terrainD(s->ax,s->ay,&L);if(L<0)D=2000;float tof=sn_tof(R,(float)(D?D:1000));s->tof=s->t=(s16)(tof*60);if(s->t<2)s->t=2;
   if(L>=0){float dm=sn_dropmil(R,D),wm=sn_driftmil(R,D,snWind);s->sx=s->ax+(s32)(wm*192);s->sy=s->ay+(s32)(dm*192);s->Ls=L;s->splash=1;}else s->splash=0;
   break;}
 snAmmo--;snShots++;snCyc=R->act?R->cyc:R->cyc;snKick=R->dmg>110?10:6;sfx_shot(1);shake(2);
 if(!(R->fl&SRF_SUP)){sn_alert(1,0,0);}else{int L;int D=sn_terrainD(snCX,snCY,&L);sn_alert(0,snCX,D);}}
static void sn_arrive(SnShot*s){const SRif*R=&SR[snR];int hit=0;
 for(int i=0;i<nST&&!hit;i++){SnT*t=&SNT[i];if(!t->alive&&!(t->k==SK_VEH&&t->hp>0))continue;if(t->k==SK_VEH&&t->hp<=0)continue;
  float dm=sn_dropmil(R,t->D),wm=sn_driftmil(R,t->D,snWind);s32 ix=s->ax+(s32)(wm*192),iy=s->ay+(s32)(dm*192);
  s32 fy=sn_feet(t);
  if(s->guided&&s->tgt==i){s32 tx=t->wx,ty=fy-sn_m(10,t->D);if(iabs(ix-tx)<6*192&&iabs(iy-ty)<6*192){ix=tx;iy=ty;}}
  int Hm=t->k==SK_VEH?26:t->pose==2?5:t->pose==1?11:18,Wm=t->k==SK_VEH?60:t->pose==2?17:5;
  s32 H=sn_m(Hm,t->D),W=sn_m(Wm,t->D)/2+sn_m(1,t->D);
  if(iabs(ix-t->wx)<=W&&iy<=fy&&iy>=fy-H-sn_m(1,t->D)){hit=1;snHits++;
   if(t->k==SK_VEH){int eng=t->face>0?ix>t->wx+W/3:ix<t->wx-W/3;if(eng&&R->dmg>=115){t->hp=0;t->st=0;snKills++;snScore+=300+t->D/5;sn_msg("ENGINE BLOCK HIT - TRUCK STOPPED");sfx_boom();}
    else sn_msg(eng?"NEED .50 CAL FOR THE ENGINE":"HIT THE FRONT - ENGINE BLOCK");}
   else sn_kill(t,iy<fy-H+sn_m(3,t->D),t->D);
   if(!(R->fl&SRF_SUP))sn_alert(1,0,0);
   for(int k=0;k<8;k++)if(SP[k].t<=0){SP[k].wx=ix;SP[k].wy=iy;SP[k].t=20;SP[k].L=t->L;SP[k].k=1;break;}}}
 if(!hit){if(s->splash){for(int k=0;k<8;k++)if(SP[k].t<=0){SP[k].wx=s->sx;SP[k].wy=s->sy;SP[k].t=30;SP[k].L=s->Ls;SP[k].k=0;break;}}
  // spotter correction vs nearest hostile
  int best=-1;s32 bd=1<<30;for(int i=0;i<nST;i++){SnT*t=&SNT[i];if(!t->alive||!t->hostile)continue;s32 d=iabs(t->wx-s->ax)+iabs(sn_feet(t)-s->ay);if(d<bd){bd=d;best=i;}}
  if(best>=0&&bd<12*192){SnT*t=&SNT[best];float dm=sn_dropmil(R,t->D),wm=sn_driftmil(R,t->D,snWind);s32 ix=s->ax+(s32)(wm*192),iy=s->ay+(s32)(dm*192);s32 ty=sn_feet(t)-sn_m(9,t->D);
   int ex=(int)((ix-t->wx)*10/192),ey=(int)((iy-ty)*10/192);static char b[40];const char*a=cat3("MISS: ",ey>0?cat("LOW ",m10s(ey)):cat("HIGH ",m10s(-ey)),"");int k=0;while(a[k]&&k<30){b[k]=a[k];k++;}
   const char*c=ex>0?cat(" RIGHT ",m10s(ex)):cat(" LEFT ",m10s(-ex));int j=0;while(c[j]&&k<38)b[k++]=c[j++];b[k]=0;sn_msg(b);}
  else sn_msg(s->splash?"MISS":"MISS - HIGH");}}
static void sn_spawn_qrf(int n){for(int k=0;k<n&&nST<NST;k++){SnT*t=&SNT[nST++];memset(t,0,sizeof*t);t->k=SK_QRF;t->hostile=1;t->alive=1;t->hp=1;t->L=3+sr(2);t->t8=60+sr(150);
  int x=(snCX>>8)+(sr(2)?1:-1)*(100+sr(200));if(x<20)x=20;if(x>940)x=940;t->wx=x<<8;t->x0=0;t->x1=959;t->D=sn_D(t->L,t->t8);t->st=4;t->tm=60*14+sr(300);t->face=(t->wx<snCX)?1:-1;}}
// test-only autopilot (harness pokes 'AUTO' at 0x0203FFD0): aims holdover + lead at the best hostile and fires
static void sn_auto(void){if(*(volatile u32*)0x0203FFD0!=0x4F545541u)return;const SRif*R=&SR[snR];int b=-1;
 for(int i=0;i<nST;i++){SnT*t=&SNT[i];if(!t->hostile||t->k==SK_CIV)continue;if(t->k==SK_VEH?t->hp<=0:!t->alive)continue;if(b<0||t->k==SK_HVT||t->k==SK_VEH)b=i;if(SNT[b].k==SK_HVT)break;}
 if(b<0)return;SnT*t=&SNT[b];int D=t->D;float tof=sn_tof(R,(float)D),dm=sn_dropmil(R,D),wm=sn_driftmil(R,D,snWind);
 int spd=t->k==SK_VEH?(8*4160)/t->D:t->st==3?(4*4160)/t->D:(13*4160/10)/t->D;int mv=(t->st==1||t->st==3||t->k==SK_VEH)?t->face*spd:0;
 s32 aimY=sn_feet(t)-sn_m(t->k==SK_VEH?13:t->pose==2?3:t->pose==1?6:11,D);s32 aimX=t->wx+(s32)(mv*tof*60);if(t->k==SK_VEH)aimX+=t->face*sn_m(22,D);
 snCX=aimX-(s32)(wm*192);snCY=aimY-(s32)(dm*192);snElev=snWdg=0;if(!snScoped){snScoped=1;snZoom=SNZ(snEra,snZi);m3_fill(0,0,240,160,0);snHudF=1;}
 if(snBreath<=0&&snBreathOut<=0)snBreath=240;if((snT%40)==0&&snBreath>0)sn_fire();}
static void sn_update(void){const SnM*M=&SM[snM];const SRif*R=&SR[snR];snT++;sn_auto();
 if(snCyc>0)snCyc--;if(snRel>0){if(--snRel==0)snAmmo=R->mag;}if(snKick>0)snKick--;
 if(snAmmo==0&&snRel==0&&snCyc==0)snRel=R->rel;
 if((snT%(60*7))==0&&M->wind){int g=(int)sr(3)-1;snWind+=g;if(snWind>M->wind)snWind=M->wind;if(snWind<-M->wind)snWind=-M->wind;}
 // aim
 int lmod=(keys&K_L)!=0;static int lUsed;
 if(lmod){if(pressed&K_UP){snElev++;lUsed=1;sfx_menu();}if(pressed&K_DOWN){snElev--;lUsed=1;sfx_menu();}if(pressed&K_RIGHT){snWdg++;lUsed=1;sfx_menu();}if(pressed&K_LEFT){snWdg--;lUsed=1;sfx_menu();}}
 else{int sp=snScoped?((keys&K_B)&&0?1:2):4;static int hold;if(keys&(K_UP|K_DOWN|K_LEFT|K_RIGHT))hold++;else hold=0;if(hold>40)sp*=2;
  s32 d=sp*((1<<16)/snZoom);if(keys&K_LEFT)snCX-=d;if(keys&K_RIGHT)snCX+=d;if(keys&K_UP)snCY-=d;if(keys&K_DOWN)snCY+=d;}
 if(!(keys&K_L)&&lUsed==0&&(dprevL&K_L)){if(snScoped){snZi=(snZi+1)%snNZ;snZoom=SNZ(snEra,snZi);sfx_menu();}}
 if(!(keys&K_L))lUsed=0;dprevL=keys;
 s32 half=(120<<16)/snZoom;if(snCX<half)snCX=half;if(snCX>(960<<8)-half)snCX=(960<<8)-half;
 s32 vh=(80<<16)/snZoom;if(snCY<vh-(20<<8))snCY=vh-(20<<8);if(snCY>(172<<8)-vh)snCY=(172<<8)-vh;
 if(pressed&K_R){snScoped^=1;snZoom=snScoped?SNZ(snEra,snZi):256;m3_fill(0,0,240,160,0);snHudF=1;sfx_menu();}
 if(keys&K_B){if(snBreathOut==0){if(snBreath==0&&(pressed&K_B))snBreath=240;else if(snBreath>0){snBreath--;if(snBreath==0)snBreathOut=150;}}}else{if(snBreath>0){snBreath=0;}}
 if(snBreathOut>0)snBreathOut--;
 if(pressed&K_A)sn_fire();
 if(pressed&K_SEL){if(R->fl&SRF_GUIDE){SnT*t=sn_at_reticle(4);if(t){for(int i=0;i<nST;i++)SNT[i].tag=0;t->tag=1;sn_msg("TARGET TAGGED - ROUNDS WILL STEER");}else sn_lase();}else sn_lase();}
 // shots in flight
 for(int i=0;i<4;i++)if(SS[i].on&&--SS[i].t<=0){SS[i].on=0;sn_arrive(&SS[i]);}
 for(int i=0;i<8;i++)if(SP[i].t>0)SP[i].t--;
 // targets
 for(int i=0;i<nST;i++){SnT*t=&SNT[i];if(!t->alive&&!(t->k==SK_VEH&&t->hp>0))continue;if(t->k==SK_VEH&&t->hp<=0)continue;t->tm--;
  int spd=t->k==SK_VEH?(8*4160)/t->D:t->st==3?(4*4160)/t->D:(13*4160/10)/t->D;
  if(t->st==1||t->st==3||t->k==SK_VEH){t->wx+=t->face*spd;if(t->k==SK_VEH){if(t->wx>(940<<8)){t->face=-1;}if(t->wx<(20<<8))t->face=1;}
   else if(t->st==1){if((t->wx>>8)>t->x1)t->face=-1;if((t->wx>>8)<t->x0)t->face=1;}}
  if(t->st==3&&t->tm<=0){t->st=2;t->pose=t->k==SK_HVT?1:2;}
  if(t->st==0&&t->tm<=0){t->st=1;t->tm=200+sr(300);}else if(t->st==1&&t->tm<=0&&t->k!=SK_VEH){t->st=0;t->tm=120+sr(200);}
  if(t->k==SK_CS){if(snAlert||M->obj==SO_DUEL){if(t->glint>0)t->glint--;else if(sr(BDcd(220))==0){t->glint=30;t->aim++;if(t->aim==2)sn_msg("SPOTTER: GLINT! COUNTER-SNIPER!");}
    if(t->aim>=4&&t->glint==1){t->aim=1;snHP--;snHitT=30;sfx_hurt();shake(8);sn_msg("YOU'RE HIT - FIND THAT SNIPER");}}}
  if(t->k==SK_QRF){if(t->tm<=0){t->tm=60*10;snHP--;snHitT=30;sfx_hurt();sn_msg("QRF IS ON YOU - TAKING FIRE");}
   if(t->tm<60*5&&t->L<4&&(snT&63)==0){t->t8+=16;if(t->t8>230){t->t8=40;t->L++;}t->D=sn_D(t->L,t->t8);}}
  if(M->obj==SO_OVER&&t->hostile&&t->alive){if(t->tm<=0){t->tm=60*12;snSquad--;sn_msg("SQUAD MEMBER HIT - CLEAR THE RIDGE!");sfx_hurt();}}}
 // overwatch: waves
 if(M->obj==SO_OVER&&snPhase==0){int alive=0;for(int i=0;i<nST;i++)alive+=SNT[i].alive&&SNT[i].hostile;
  if(alive<3&&snKills+alive<M->need){for(int k=0;k<2&&nST<NST;k++){SnT*t=&SNT[nST++];memset(t,0,sizeof*t);t->k=SK_SOLD;t->hostile=1;t->alive=1;t->L=1+sr(3);t->t8=60+sr(150);int x=80+sr(800);t->wx=x<<8;t->x0=x-40;t->x1=x+40;t->D=sn_D(t->L,t->t8);t->st=1;t->face=1;t->tm=60*12;}}}
 // objectives
 if(snPhase==0){int done=0;
  switch(M->obj){case SO_HVT:done=!SNT[0].alive;break;
  case SO_KILL:case SO_SILENT:case SO_OVER:done=snKills>=M->need;break;
  case SO_DUEL:{int cs=0;for(int i=0;i<nST;i++)cs+=SNT[i].k==SK_CS&&SNT[i].alive;done=cs==0&&snKills>=M->need;break;}
  case SO_VEH:{int v=0;for(int i=0;i<nST;i++)v+=SNT[i].k==SK_VEH&&SNT[i].hp>0;done=v==0;break;}
  case SO_HOST:{int g=0;for(int i=0;i<nST;i++)g+=SNT[i].k==SK_SOLD&&SNT[i].alive;done=g==0;break;}}
  if(M->obj==SO_SILENT&&snAlert){snFail=1;sn_msg("COMPROMISED - MISSION FAILED");}
  if(M->obj==SO_HVT&&snM!=13&&snT>60*60*4&&SNT[0].alive){snFail=1;sn_msg("HVT ESCAPED");}
  if(M->obj==SO_OVER&&snSquad<=0){snFail=1;sn_msg("PATROL OVERRUN");}
  if(done&&!snFail){snPhase=1;snPhaseT=60*40;sn_spawn_qrf(4);sn_msg("OBJECTIVE COMPLETE - EXFIL, HOLD FOR HELO");bgm_t(TR_FORT);}}
 else if(snPhase==1){if(--snPhaseT<=0){snDone=1;sn_msg("HELO ON STATION - EXTRACTED");}if(snPhaseT==60*20)sn_spawn_qrf(3);}
 if(snHP<=0){snFail=1;}}
// ---------------------------------------------------------------- save / records
typedef struct{u16 best[SN_NM];u8 stars[SN_NM];u16 lng,kills;u8 last,pad;}SnSave;static SnSave SNS;
static void sns_write(void){const u8*b=(const u8*)&SNS;u8 s=0x4E;SRAM[SN_OFF]='A';SRAM[SN_OFF+1]='O';SRAM[SN_OFF+2]='E';SRAM[SN_OFF+3]='N';SRAM[SN_OFF+4]=1;for(int i=0;i<(int)sizeof SNS;i++){SRAM[SN_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[SN_OFF+5+sizeof SNS]=s;}
static void sns_read(void){memset(&SNS,0,sizeof SNS);if(SRAM[SN_OFF]!='A'||SRAM[SN_OFF+1]!='O'||SRAM[SN_OFF+2]!='E'||SRAM[SN_OFF+3]!='N'||SRAM[SN_OFF+4]!=1)return;u8 b[sizeof SNS];u8 s=0x4E;for(int i=0;i<(int)sizeof SNS;i++){b[i]=SRAM[SN_OFF+5+i];s+=b[i]*(i+1);}if(s==SRAM[SN_OFF+5+sizeof SNS])memcpy(&SNS,b,sizeof SNS);}
static void rec_snipe(const char**L){static char b[5][40];sns_read();int n=0,st=0;for(int i=0;i<SN_NM;i++){n+=SNS.stars[i]>0;st+=SNS.stars[i];}
 const char*s[5]={cat3("MISSIONS COMPLETE: ",itoa(n),"/14"),0,0,0,0};
 for(int k=0;k<5;k++){const char*a=k==0?cat3("MISSIONS COMPLETE: ",itoa(n),"/14"):k==1?cat3("STARS: ",itoa(st),"/42"):k==2?cat3("LONGEST SHOT: ",itoa(SNS.lng),"M"):k==3?cat3("CONFIRMED KILLS: ",itoa(SNS.kills),""):"48 RIFLES ACROSS 5 ERAS";int j=0;while(a[j]&&j<38){b[k][j]=a[j];j++;}b[k][j]=0;L[k]=b[k];}(void)s;}
// ---------------------------------------------------------------- run / menus
static void sn_present(void){vsync();dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);frame++;BEAT->frame=frame;poll();}
static int sn_find(const char*n){for(int i=0;i<SNP_N;i++){const char*a=SR[i].n;int k=0;while(a[k]&&a[k]==n[k])k++;if(!a[k]&&!n[k])return i;}return 0;}
static int sn_run(void){const SnM*M=&SM[snM];const SRif*R=&SR[snR];sn_gen();snEra=M->era;snNZ=SN_NZ[snEra];snZi=snNZ-1;snScoped=0;snZoom=256;snCX=480<<8;snCY=80<<8;
 kCX=-1;snT=0;snHP=3;snAmmo=R->mag;snCyc=snRel=snBreath=snBreathOut=snKick=0;snElev=snWdg=0;snWind=M->wind?((sr(2)?1:-1)*(1+sr(M->wind))):0;snAlert=0;snKills=snShots=snHits=snHeads=snLong=snScore=0;snPhase=0;snFail=0;snDone=0;snRange=0;snSquad=6;snCivHit=0;snMsgT=0;
 memset(SS,0,sizeof SS);memset(SP,0,sizeof SP);for(int y=0;y<160;y++){int dy=y-80;int h=0;if(iabs(dy)<70){int r2=70*70-dy*dy;h=0;while((h+1)*(h+1)<=r2)h++;}snSpan[y]=h;}
 if(M->obj==SO_OVER){snSquad=6;}
 bgm_t(TR_SNIPE);sn_msg(cat("SPOTTER: ",M->b2));int failT=0,doneT=0;
 for(;;){BEAT->screen=SCR_SN;BEAT->mode=snM;BEAT->era=snR;BEAT->hp=snHP;BEAT->kills=snKills;BEAT->scoreA=snScore;BEAT->scoreB=snShots|(snHits<<8);BEAT->phase=snFail?5:snDone?4:snPhase;BEAT->x=snCX>>8;BEAT->y=snCY>>8;BEAT->timer=snT;BEAT->extra=snZoom>>8|(snScoped<<8)|(snRange<<9);
  {static u32 lv;int n=(int)(vblN-lv);lv=vblN;if(n<1||n>3)n=n<1?1:3;u16 pk=pressed; // fixed 60Hz sim: catch up when rendering drops frames
   for(int k=0;k<n;k++){if(!snFail&&!snDone)sn_update();else if(snFail)failT++;else doneT++;pressed=0;}pressed=pk&K_START;}
  sn_draw();sn_present();
  if(failT>120){music_defeat();return 0;}if(doneT>90)break;
  if(pressed&K_START){int q=0;for(;;){m3_box(60,60,120,40,1);m3_ctext(68,"PAUSED",C_YEL,1);m3_ctext(82,"START: RESUME  B: ABORT",C_WHITE,1);sn_present();if(pressed&K_START)break;if(pressed&K_B){q=1;break;}}if(q)return 0;m3_fill(0,0,240,160,0);}}
 // results
 int acc=snShots?snHits*100/snShots:0;int stars=1+(acc>=70)+(snHP==3);int sc=snScore+acc*10+snHP*200;
 int nb=sc>SNS.best[snM];if(nb)SNS.best[snM]=sc>65535?65535:sc;if(stars>SNS.stars[snM])SNS.stars[snM]=stars;if(snLong>SNS.lng)SNS.lng=snLong;SNS.kills+=snKills;SNS.last=snM;sns_write();music_jingle(TR_CLEAR);
 for(int t=0;;t++){BEAT->screen=SCR_SN;BEAT->phase=4;BEAT->scoreA=sc;BEAT->scoreB=stars;
  if(t==0){m3_fill(0,0,240,160,RGB(8,10,18));m3_box(16,16,208,128,1);m3_ctext(24,"MISSION COMPLETE",C_YEL,2);m3_ctext(44,M->n,C_WHITE,1);
   m3_text(36,60,cat3("KILLS ",itoa(snKills),cat("  HEADSHOTS ",itoa(snHeads))),C_WHITE,1);m3_text(36,72,cat3("ACCURACY ",itoa(acc),"%"),C_WHITE,1);
   m3_text(36,84,cat3("LONGEST SHOT ",itoa(snLong),"M"),C_WHITE,1);m3_text(36,96,cat3("SCORE ",itoa(sc),nb?"  BEST!":""),nb?C_GREEN:C_WHITE,1);
   for(int i=0;i<3;i++)m3_fill(96+i*18,110,12,12,i<stars?C_YEL:RGB(50,50,60));m3_ctext(128,"A: CONTINUE",C_GRAY,1);m3_present();}
  else next_frame();if(t>30&&(pressed&(K_A|K_START)))break;}
 return 1;}
static void sn_brief(int m){snM=m;const SnM*M=&SM[m];int list[SNP_N],n=0;for(int i=0;i<SNP_N;i++)if(SR[i].era==M->era)list[n++]=i;
 int def=sn_find(M->rif),sel=0;for(int i=0;i<n;i++)if(list[i]==def)sel=i;int redraw=1;
 for(;;){BEAT->screen=SCR_SNB;BEAT->mode=m;BEAT->x=sel;BEAT->extra=list[sel];
  if(redraw){const SRif*R=&SR[list[sel]];cat_bgc(CAT_SPECIAL);banner(M->n,M->b1,C_YEL);m3_box(4,38,232,118,1);
   m3_text(12,44,"OBJECTIVE",C_GRAY,1);m3_text(72,44,SON[M->obj],C_YEL,1);m3_text(12,55,M->b2,C_WHITE,1);
   m3_text(12,70,"RIFLE <L R>",C_GRAY,1);m3_text(84,70,R->n,C_YEL,1);m3_text(200,70,R->tag,C_GRAY,1);
   m3_text(12,82,cat3("MUZZLE ",itoa(R->v0),"M/S"),C_WHITE,1);m3_text(120,82,cat3("MAG ",itoa(R->mag),R->act?" BOLT":" SEMI"),C_WHITE,1);
   {float d5=sn_dropmil(R,500),d10=sn_dropmil(R,1000);m3_text(12,93,cat3("DROP 500M ",m10s(f10(d5))," MIL"),C_WHITE,1);m3_text(12,104,cat3("DROP 1KM  ",m10s(f10(d10))," MIL"),C_WHITE,1);}
   m3_text(150,93,cat("SWAY ",itoa(R->sway)),C_WHITE,1);m3_text(150,104,cat3("CYCLE ",itoa(R->cyc),"F"),C_WHITE,1);
   if(R->fl&SRF_SUP)m3_text(12,115,"SUPPRESSED: QUIET SHOTS",C_GREEN,1);if(R->fl&SRF_RAIL)m3_text(12,115,"RAIL: NEAR-FLAT TRAJECTORY",C_GREEN,1);if(R->fl&SRF_GUIDE)m3_text(12,115,"SMART ROUNDS: SELECT TAGS",C_GREEN,1);
   m3_text(12,126,cat3("BEST ",itoa(SNS.best[m]),cat("  STARS ",itoa(SNS.stars[m]))),C_GRAY,1);
   m3_ctext(140,"A: DEPLOY   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&(K_L|K_LEFT)){sel=(sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&(K_R|K_RIGHT)){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&K_A){snR=list[sel];sfx_ok();pressed=0;sn_run();pressed=0;bgCached=-1;bgm_t(TR_SNIPE);return;}
  if(pressed&K_B){pressed=0;return;}}}
static void sniper_ops(void){sns_read();int sel=SNS.last<SN_NM?SNS.last:0,redraw=1;bgm_t(TR_SNIPE);
 for(;;){BEAT->screen=SCR_SNSEL;BEAT->x=sel;
  if(redraw){cat_bgc(CAT_SPECIAL);banner("SNIPER","SELECT A MISSION",C_YEL);m3_box(4,38,232,118,1);int top=sel>8?sel-8:0;
   for(int i=top;i<top+9&&i<SN_NM;i++){int y=44+(i-top)*11;static const char*const EN[4]={"WW2","CW","MOD","FUT"};m3_text(10,y,i==sel?">":" ",C_YEL,1);
    m3_text(18,y,itoa(i+1),C_GRAY,1);m3_text(34,y,SM[i].n,i==sel?C_YEL:C_WHITE,1);m3_text(140,y,i==2?"WW1":EN[SM[i].era],C_GRAY,1);
    for(int s=0;s<3;s++)m3_fill(170+s*8,y+1,6,6,s<SNS.stars[i]?C_YEL:RGB(50,50,60));}
   m3_ctext(146,"A: BRIEFING   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+SN_NM-1)%SN_NM;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%SN_NM;redraw=1;sfx_menu();}
  if(pressed&K_A){sfx_ok();pressed=0;sn_brief(sel);redraw=1;}
  if(pressed&K_B){pressed=0;bgCached=-1;return;}}}
