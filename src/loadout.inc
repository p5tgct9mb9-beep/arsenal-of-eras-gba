static const char*const TEAMN[5][2]={{"ALLIES","AXIS"},{"NATO","PACT"},{"BLUFOR","OPFOR"},{"UNION","SYNDIC"},{"TERRAN","VOID"}};
// ===================================================================== LOADOUT / CUSTOMIZE (+ SRAM save)
typedef struct{const char*n;u8 dmg,rate,spd16,spread,pellets,life,mag,reload,move;}Wpn;
typedef struct{int dmg,rate,spd16,spread,pellets,life,mag,reload,move,kind;const char*n;}Wstat;
#define MAXPRIM 15
#define MAXSEC 5
static const u8 NPRIM[5]={15,5,5,4,4};
static const u8 NSEC[5]={5,2,2,2,2};
static const Wpn PRIM[5][MAXPRIM]={
 {{"M1 GARAND",50,20,84,2,1,42,8,90,100},{"THOMPSON M1A1",20,6,72,7,1,30,30,110,104},{"BAR M1918",32,8,84,5,1,42,20,130,92},{"KAR98K",95,40,100,1,1,50,5,120,98},
  {"STG 44",26,7,84,5,1,38,30,110,100},{"M1 CARBINE",30,10,80,3,1,36,15,85,104},{"MP40",19,7,72,6,1,30,32,100,105},{"MG42",24,3,84,10,1,40,50,255,84},
  {"PPSH-41",16,3,72,9,1,28,71,150,103},{"MOSIN-NAGANT",92,42,100,1,1,50,5,125,98},{"LEE-ENFIELD NO4",70,24,96,2,1,46,10,110,99},{"STEN MK II",18,6,70,8,1,28,32,95,106},
  {"M3 GREASE GUN",22,8,70,6,1,28,30,105,104},{"M1903 SCOPED",105,46,110,0,1,58,5,130,95},{"M97 TRENCH GUN",16,34,76,14,6,18,6,170,100}},
 {{"AK-47",30,7,88,6,1,40,30,110,98},{"M16A1",26,6,96,4,1,40,20,100,102},{"M60",26,5,84,8,1,40,100,240,86},{"DRAGUNOV SVD",75,24,112,1,1,52,10,120,96},{"UZI",18,4,76,8,1,28,32,90,108}},
 {{"M4A1",26,5,96,4,1,40,30,95,102},{"HK416",28,5,96,3,1,42,30,100,100},{"AK-12",30,6,96,4,1,42,30,100,99},{"SCAR-H",40,8,100,4,1,46,20,110,96},{"BARRETT M82",120,45,140,1,1,56,10,160,88}},
 {{"SMART RIFLE",30,6,104,1,1,44,30,100,100},{"RAILGUN",110,42,170,0,1,46,5,150,92},{"LASER CARBINE",22,3,140,2,1,36,60,140,100},{"PULSE SMG",18,3,96,6,1,30,45,90,108}},
 {{"PLASMA RIFLE",32,7,90,4,1,40,30,110,100},{"COILGUN",85,30,150,1,1,52,8,130,94},{"GAUSS SMG",18,3,100,6,1,30,50,95,108},{"ION REPEATER",22,4,96,7,1,40,80,200,90}}};
static const u8 AXIS_PRIM[4]={3,4,6,7}; // WW2 bots on the Axis side carry these
static const u8 ALLY_PRIM[11]={0,1,2,5,8,9,10,11,12,13,14};
static const Wpn SEC[5][MAXSEC]={
 {{"M1911",34,12,72,4,1,26,7,70,106},{"LUGER P08",30,10,72,3,1,26,8,70,106},{"WALTHER P38",31,11,72,3,1,26,8,70,106},{"WEBLEY MK VI",42,16,70,4,1,24,6,95,106},{"TOKAREV TT-33",29,9,80,4,1,26,8,70,106}},
 {{"MAKAROV PM",28,10,72,4,1,26,8,65,106},{"M1911A1",34,12,72,4,1,26,7,70,106}},
 {{"GLOCK 17",25,8,80,4,1,26,17,60,106},{"M870 SHOTGUN",18,30,80,12,5,18,8,150,100}},
 {{"SMART PISTOL",26,9,90,1,1,28,12,60,106},{"ARC PISTOL",40,18,100,3,1,24,6,70,106}},
 {{"NEEDLER",16,4,90,5,1,26,24,70,106},{"ARC SCATTER",22,30,90,10,4,22,6,100,100}}};
// launchers: dmg, rate(frames between shots), spd16, spread, pellets, life, ammo, reload(unused), move
static const Wpn LAUN[5][3]={
 {{"NONE",0,0,0,0,0,0,0,0,100},{"BAZOOKA M1A1",90,90,56,2,1,70,3,0,96},{"PANZERSCHRECK",110,110,52,2,1,70,2,0,95}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"M72 LAW",90,60,60,2,1,64,2,0,98},{"RPG-7",100,100,58,3,1,70,3,0,95}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"AT4",110,80,64,1,1,64,2,0,97},{"CARL GUSTAF",100,90,62,1,1,72,3,0,94}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"SMART LAUNCHER",90,70,70,0,1,64,3,0,96},{"PLASMA LANCE",120,110,80,0,1,60,2,0,95}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"GRAV LANCE",100,80,72,1,1,64,3,0,96},{"ION MORTAR",90,70,60,3,1,70,4,0,95}}};
static const char*const LETH_N[5][3]={{"MK2 FRAG","STIELHANDGRANATE","GAMMON BOMB"},{"FRAG","SEMTEX","CLUSTER"},{"FRAG","SEMTEX","CLUSTER"},{"FRAG","SEMTEX","CLUSTER"},{"FRAG","SEMTEX","CLUSTER"}};
static const u8 LETH_SUB[5][3]={{0,3,4},{0,1,2},{0,1,2},{0,1,2},{0,1,2}};
static const char*const LETH_D[5]={"4 SEC FUSE, BIG BLAST","SHORT FUSE, STICKS TO WALLS","3 SMALLER BLASTS","LONG THROW (STICK), FRAG BLAST","IMPACT FUSE, HUGE BLAST"};
static const u8 NTACT[5]={2,3,3,3,3};
static const char*const TACT_N[5][3]={{"M15 SMOKE","MORPHINE",""},{"FLASHBANG","SMOKE","STIM"},{"FLASHBANG","SMOKE","STIM"},{"FLASHBANG","SMOKE","STIM"},{"FLASHBANG","SMOKE","STIM"}};
static const u8 TACT_FX[5][3]={{1,2,0},{0,1,2},{0,1,2},{0,1,2},{0,1,2}};  // 0 flash 1 smoke 2 stim
static const char*const OPTN[3]={"IRON SIGHTS","RED DOT","4X SCOPE"};
static const char*const MUZN[3]={"STANDARD","SUPPRESSOR","COMPENSATOR"};
static const char*const GRIPN[3]={"NONE","VERTICAL GRIP","ANGLED GRIP"};
static const char*const MAGN[3]={"STANDARD MAG","EXTENDED MAG","FAST MAG"};
static const char*const PERKN[5]={"SPRINTER","FLAK JACKET","SCAVENGER","SLEIGHT OF HAND","JUGGERNAUT"};
static const char*const OPTD[3]={"NO CHANGE","RECOIL -1","RECOIL -2 RANGE +15% MOB -3"};
static const char*const MUZD[3]={"NO CHANGE","BOTS SPOT YOU LATER, RANGE -15%","RECOIL -2, FIRE RATE -10%"};
static const char*const GRIPD[3]={"NO CHANGE","RECOIL -1","RELOAD -15%"};
static const char*const MAGD[3]={"NO CHANGE","MAG +50%, RELOAD +15%, MOB -3","RELOAD -35%"};
static const char*const TACD[3]={"STUNS ENEMIES 2.5 SEC","BLOCKS ENEMY LINE OF SIGHT","INSTANT +60 HP"};
static const char*const PERKD[5]={"MOVE SPEED +12%","EXPLOSIVE DAMAGE -50%","KILLS REFILL AMMO + LETHAL","RELOAD -40%","130 HP, MOVE -6%"};
typedef struct{const char*n;s16 spd,turn,reload,dmg,hp,sspd;}TankS;
static const TankS TANKV[5][2]={
 {{"M4 SHERMAN",240,320,85,32,100,1100},{"TIGER I",170,230,110,48,150,1300}},
 {{"M48 PATTON",250,300,80,36,115,1200},{"T-55",260,310,85,36,105,1200}},
 {{"M1A2 ABRAMS",290,320,70,42,130,1450},{"T-90M",280,340,65,40,120,1400}},
 {{"HOVER MBT",350,420,55,34,105,1500},{"RAIL WALKER",220,300,80,60,140,2200}},
 {{"GRAV TANK",320,420,50,30,100,1600},{"LUNAR CRAWLER",240,320,65,42,140,1600}}};
typedef struct{const char*n;const char*mn;s16 spd,turn;u8 rate,dmg;s16 bspd;u8 ammo;s16 mturn,hp;u8 side,sec,spr,gunner;}AirS; // sec 0 rockets 1 missiles 2 bombs
#define MAXAIR 14
static const u8 NAIR[5]={13,2,2,2,2};
static const AirS AIRV[5][MAXAIR]={
 {{"P-51D MUSTANG","HVAR ROCKETS",410,560,4,10,1150,6,0,100,0,0,0,0},{"SPITFIRE IX","RP-3 ROCKETS",380,640,4,9,1100,4,0,90,0,0,1,0},
  {"F6F HELLCAT","HVAR ROCKETS",360,540,4,11,1100,6,0,140,0,0,2,0},{"P-47 THUNDERBOLT","HVAR ROCKETS",380,470,3,11,1150,8,0,160,0,0,3,0},
  {"F4U CORSAIR","HVAR ROCKETS",400,520,4,11,1150,8,0,130,0,0,4,0},{"YAK-3","RS-82 ROCKETS",400,660,5,11,1100,2,0,85,0,0,5,0},
  {"HURRICANE IIC","RP-3 ROCKETS",340,600,4,10,1050,4,0,110,0,0,6,0},{"P-38 LIGHTNING","HVAR ROCKETS",420,450,3,12,1200,10,0,120,0,0,7,0},
  {"BF 109 G","WGR 21 ROCKETS",400,560,5,12,1150,2,0,90,1,0,8,0},{"FW 190 A","WGR 21 ROCKETS",400,540,4,13,1150,4,0,120,1,0,9,0},
  {"ME 262","R4M ROCKETS",580,360,5,16,1250,12,0,100,1,0,10,0},{"A6M ZERO","TYPE 3 ROCKETS",350,700,5,11,1050,2,0,70,1,0,11,0},
  {"JU 87 STUKA","SC250 BOMBS",300,420,6,8,1050,4,0,130,1,2,12,1},{"SBD DAUNTLESS","1000LB BOMBS",330,480,5,9,1050,6,0,150,0,2,13,1}},
 {{"MIG-21","K-13 AAM",480,520,5,11,1300,4,380,95,1,1,0,0},{"F-4 PHANTOM","AIM-7",460,470,5,12,1300,6,360,120,0,1,1,0}},
 {{"F-35A","AIM-120",520,540,4,10,1400,6,560,100,0,1,0,0},{"SU-57","R-77",540,580,4,10,1400,6,540,105,1,1,1,0}},
 {{"NGAD 6TH-GEN","SEEKERS",600,640,4,9,1500,6,700,100,0,1,0,0},{"X-90 ARROW","SWARM",640,600,4,8,1500,8,650,90,1,1,1,0}},
 {{"SF-1 STARFIGHTER","TORPEDOES",520,700,5,12,1300,8,700,100,0,1,0,0},{"VOID INTERCEPTOR","PLASMA TORP",580,660,4,10,1400,6,720,95,1,1,1,0}}};
typedef struct{u8 prim,sec,opt,muz,grip,mag,leth,tact,perk,laun;}Loadout;
static Loadout LO[5];static u8 tankSel[5],airSel[5];
static const char save_tag[] __attribute__((used,aligned(4)))="SRAM_V113";
#define SRAM ((volatile u8*)0x0E000000)
#define LO_F 10
// ---------------------------------------------------------------- 6v6 ARSENAL: one pool across all eras + factions
typedef struct{u8 pI,sI,lI,leth,tact,tkI,aI,fac0,fac1;}G6;
static G6 g6={0,0,0,0,0,0,0,0,6};
static const u8 N2[5]={2,2,2,2,2};
static const char*const ERA_TAG[5]={"WW2","COLD WAR","MODERN","FUTURE","SPACE"};
static int fsum(const u8*n){int s=0;for(int e=0;e<5;e++)s+=n[e];return s;}
static int fdec(const u8*n,int idx,int*o){for(int e=0;e<5;e++){if(idx<n[e]){*o=idx;return e;}idx-=n[e];}*o=0;return 0;}
static int fenc(const u8*n,int e,int i){int s=0;for(int k=0;k<e;k++)s+=n[k];return s+i;}
#define NLETH6 6
#define NTACT6 4
static const char*const LETH6_N[NLETH6]={"MK2 FRAG","STIELHANDGRANATE","GAMMON BOMB","FRAG","SEMTEX","CLUSTER"};
static const u8 LETH6_SUB[NLETH6]={0,3,4,0,1,2};
static const char*const TACT6_N[NTACT6]={"SMOKE","MORPHINE","FLASHBANG","STIM"};
static const u8 TACT6_FX[NTACT6]={1,2,0,2};
typedef struct{const char*n;u8 m[3],h[3],t[3];u8 axis;}Fac;
#define NFAC 11
static const Fac FAC[NFAC]={{"USA",{92,104,58},{80,92,52},{70,150,255},0},{"UK",{124,104,66},{100,90,60},{90,200,255},0},{"USSR",{132,128,76},{90,96,60},{230,60,50},0},
 {"GERMANY",{96,102,100},{80,86,88},{210,210,210},1},{"JAPAN",{150,132,80},{120,110,70},{240,140,60},1},{"CHINA",{80,110,70},{70,96,64},{255,90,90},0},
 {"RUSSIA",{104,110,84},{84,92,70},{220,70,70},0},{"NATO",{82,98,64},{70,80,56},{80,140,255},0},{"COALITION",{170,148,104},{150,134,96},{90,220,160},0},
 {"FEDERATION",{190,196,206},{170,176,190},{110,200,255},0},{"SYNDICATE",{80,64,92},{70,56,80},{255,80,200},0}};
static u16 fac_c(const u8*c,int num,int den){return RGB(clampi(c[0]*num/den,0,255),clampi(c[1]*num/den,0,255),clampi(c[2]*num/den,0,255));}
static void fac_pal(int bank,int f,int player){volatile u16*p=PAL_OBJ+bank*16;const Fac*F=&FAC[f];p[2]=fac_c(F->m,3,4);p[3]=fac_c(F->m,1,1);p[4]=fac_c(F->m,5,4);p[5]=fac_c(F->h,1,1);p[9]=player?RGB(255,220,40):fac_c(F->t,1,1);}
#define SAVE_N2 (5*LO_F+10+6+4+2)
#define SAVE_N (SAVE_N2+9+20)
#define SAVE_V 3
static u32 campUnlock=1;static u8 droneSel,droneCall;
static void lo_defaults(void){campUnlock=1;droneSel=0;droneCall=0;{G6 d={0,0,0,0,0,0,0,0,6};g6=d;}{Chr c={"SOLDIER",1,0,0,0,0,1,1,0,1};CH=c;}memset(LO,0,sizeof LO);memset(tankSel,0,5);memset(airSel,0,5);}
static int lo_valid(void){for(int e=0;e<5;e++){Loadout*l=&LO[e];if(l->prim>=NPRIM[e]||l->sec>=NSEC[e]||l->opt>2||l->muz>2||l->grip>2||l->mag>2||l->leth>2||l->tact>=NTACT[e]||l->perk>4||l->laun>2||tankSel[e]>1||airSel[e]>=NAIR[e])return 0;}return 1;}
static void save_write(void){u8 b[SAVE_N];for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<LO_F;k++)b[e*LO_F+k]=p[k];b[5*LO_F+e]=tankSel[e];b[5*LO_F+5+e]=airSel[e];}
 {u8*q=&SET.track;for(int k=0;k<6;k++)b[5*LO_F+10+k]=q[k];for(int k=0;k<4;k++)b[5*LO_F+16+k]=campUnlock>>(8*k);b[5*LO_F+20]=droneSel;b[5*LO_F+21]=droneCall;}
 {u8*q=&g6.pI;for(int k=0;k<9;k++)b[SAVE_N2+k]=q[k];u8*c=(u8*)&CH;for(int k=0;k<20;k++)b[SAVE_N2+9+k]=c[k];}
 u8 sum=0x5A;SRAM[0]='A';SRAM[1]='O';SRAM[2]='E';SRAM[3]='L';SRAM[4]=SAVE_V;
 for(int i=0;i<SAVE_N;i++){SRAM[5+i]=b[i];sum+=b[i]*(i+1);}SRAM[5+SAVE_N]=sum;}
static int saveLoaded; // 0 none/defaults, 1 v3, 2 migrated from v1, 3 migrated from v2
static int save_load(void){lo_defaults();saveLoaded=0;if(SRAM[0]!='A'||SRAM[1]!='O'||SRAM[2]!='E'||SRAM[3]!='L')return 0;
 int v=SRAM[4];int nf=v==1?9:LO_F;int n=v==1?5*nf+10:v==2?SAVE_N2:SAVE_N;if(v<1||v>3)return 0;
 u8 b[SAVE_N];u8 sum=0x5A;for(int i=0;i<n;i++){b[i]=SRAM[5+i];sum+=b[i]*(i+1);}if(sum!=SRAM[5+n])return 0;
 for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<nf;k++)p[k]=b[e*nf+k];tankSel[e]=b[5*nf+e];airSel[e]=b[5*nf+5+e];}
 if(v>=2){u8*q=&SET.track;for(int k=0;k<6;k++)q[k]=b[5*LO_F+10+k];campUnlock=0;for(int k=0;k<4;k++)campUnlock|=(u32)b[5*LO_F+16+k]<<(8*k);droneSel=b[5*LO_F+20]%9;droneCall=b[5*LO_F+21]%3;
  if(SET.track>NTRK||SET.musVol>10||SET.sfxVol>10||SET.shake>1||SET.diff>2||SET.hints>1){Settings d={0,8,10,1,1,1};SET=d;}}
 if(v==3){u8*q=&g6.pI;for(int k=0;k<9;k++)q[k]=b[SAVE_N2+k];u8*c=(u8*)&CH;for(int k=0;k<20;k++)c[k]=b[SAVE_N2+9+k];CH.name[10]=0;
  if(g6.pI>=fsum(NPRIM)||g6.sI>=fsum(NSEC)||g6.lI>10||g6.leth>=NLETH6||g6.tact>=NTACT6||g6.tkI>=10||g6.aI>=fsum(NAIR)||g6.fac0>=NFAC||g6.fac1>=NFAC||g6.fac0==g6.fac1){G6 d={0,0,0,0,0,0,0,0,6};g6=d;}
  if(CH.skin>=CH_NSKIN||CH.hair>=CH_NHAIR||CH.head>=CH_NHEAD||CH.hat>=CH_NHAT||CH.uni>=CH_NUNI||CH.camo>=CH_NCAMO||CH.vest>=CH_NVEST||CH.body>1||CH.use>1||!CH.name[0]){Chr c={"SOLDIER",1,0,0,0,0,1,1,0,1};CH=c;}}
 if(v==1){ // v1.0 -> v1.1 migration: WW2 tactical list changed (flash/smoke/stim -> smoke/morphine)
  for(int e=0;e<5;e++)LO[e].laun=0;LO[0].tact=LO[0].tact==2?1:0;}
 if(!lo_valid()){lo_defaults();return 0;}
 saveLoaded=v==3?1:v==2?3:2;if(v<3)save_write();return saveLoaded;}
static Wstat wcalc(int era,const Loadout*l,int slot){int secondary=slot!=0;const Wpn*w=slot==2?&LAUN[era][l->laun]:slot==1?&SEC[era][l->sec]:&PRIM[era][l->prim];Wstat s;s.kind=slot==2?K_ROCKET:K_BUL;
 s.n=w->n;s.dmg=w->dmg;s.rate=w->rate;s.spd16=w->spd16;s.spread=w->spread;s.pellets=w->pellets;s.life=w->life;s.mag=w->mag;s.reload=w->reload;s.move=w->move;
 if(!secondary){
  if(l->opt==1)s.spread-=1;if(l->opt==2){s.spread-=2;s.life=s.life*115/100;s.move-=3;}
  if(l->muz==1)s.life=s.life*85/100;if(l->muz==2){s.spread-=2;s.rate=s.rate*110/100+1;}
  if(l->grip==1)s.spread-=1;if(l->grip==2)s.reload=s.reload*85/100;
  if(l->mag==1){s.mag=s.mag*3/2;s.reload=s.reload*115/100;s.move-=3;}if(l->mag==2)s.reload=s.reload*65/100;}
 if(l->perk==3)s.reload=s.reload*60/100;if(l->perk==0)s.move=s.move*112/100;if(l->perk==4)s.move=s.move*94/100;
 if(l->laun)s.move-=3;if(slot==2){s.reload=s.rate;}if(s.spread<0)s.spread=0;return s;}
// ---------------------------------------------------------------- menu
enum{R_ERA,R_PRIM,R_SEC,R_LAUN,R_OPT,R_MUZ,R_GRIP,R_MAG,R_LETH,R_TACT,R_PERK,R_TANK,R_AIR,R_DONE};
static const char*const RLAB[14]={"ERA","PRIMARY","SIDEARM","LAUNCHER","OPTIC","MUZZLE","GRIP","MAGAZINE","LETHAL","TACTICAL","PERK","TANK","AIRCRAFT",""};
static char wnb[14];static const char*trunc12(const char*s){int k=0;while(s[k]&&k<12){wnb[k]=s[k];k++;}wnb[k]=0;return wnb;}
static const char*const ERA_NAME[5]={"WORLD WAR II","COLD WAR","MODERN","FUTURISTIC","SPACE"};
static void m3_fill(int x0,int y0,int w,int h,u16 c){for(int y=y0;y<y0+h;y++){u16*p=&fb[y*240+x0];if(!(x0&1)&&!(w&1)&&w>=8)fill32(p,c|(c<<16),w/2);else for(int x=0;x<w;x++)p[x]=c;}}
static void m3_bar(int x,int y,const char*lab,int v,int max,u16 col){m3_text(x,y,lab,C_GRAY,1);int w=70;int f=max>0?v*w/max:0;f=clampi(f,1,w);
 m3_fill(x,y+8,w,4,RGB(40,46,70));m3_fill(x,y+8,f,4,col);}
static int lo_optcount(int r,int e){switch(r){case R_ERA:return 5;case R_PRIM:return NPRIM[e];case R_SEC:return NSEC[e];case R_TANK:return 2;case R_AIR:return NAIR[e];case R_TACT:return NTACT[e];case R_PERK:return 5;case R_DONE:return 1;}return 3;}
static u8*lo_field(int r,int e){Loadout*l=&LO[e];switch(r){case R_PRIM:return &l->prim;case R_SEC:return &l->sec;case R_LAUN:return &l->laun;case R_OPT:return &l->opt;case R_MUZ:return &l->muz;case R_GRIP:return &l->grip;
 case R_MAG:return &l->mag;case R_LETH:return &l->leth;case R_TACT:return &l->tact;case R_PERK:return &l->perk;case R_TANK:return &tankSel[e];case R_AIR:return &airSel[e];}return 0;}
static const char*lo_value(int r,int e){Loadout*l=&LO[e];switch(r){case R_ERA:return ERA_NAME[e];case R_PRIM:return PRIM[e][l->prim].n;case R_SEC:return SEC[e][l->sec].n;case R_LAUN:return LAUN[e][l->laun].n;case R_OPT:return OPTN[l->opt];
 case R_MUZ:return MUZN[l->muz];case R_GRIP:return GRIPN[l->grip];case R_MAG:return MAGN[l->mag];case R_LETH:return LETH_N[e][l->leth];case R_TACT:return TACT_N[e][l->tact];case R_PERK:return PERKN[l->perk];
 case R_TANK:return TANKV[e][tankSel[e]].n;case R_AIR:return AIRV[e][airSel[e]].n;}return "";}
static const char*lo_desc(int r,int e){Loadout*l=&LO[e];switch(r){case R_OPT:return OPTD[l->opt];case R_MUZ:return MUZD[l->muz];case R_GRIP:return GRIPD[l->grip];case R_MAG:return MAGD[l->mag];
 case R_LETH:return LETH_D[LETH_SUB[e][l->leth]];case R_TACT:return TACD[TACT_FX[e][l->tact]];case R_LAUN:return l->laun?"L CYCLES TO IT. SPLASH DAMAGE":"NO LAUNCHER (NO MOBILITY PENALTY)";case R_PERK:return PERKD[l->perk];case R_ERA:return "L/R OR LEFT/RIGHT: CHANGE ERA";
 case R_TANK:return tankSel[e]?"ENEMY FIELDS THE OTHER TANK":"ENEMY FIELDS THE OTHER TANK";case R_AIR:return AIRV[e][airSel[e]].side?"AXIS SIDE - BOTS FLY AXIS TYPES":"ALLIED SIDE - BOTS FLY ALLIED TYPES";case R_DONE:return "SAVED TO CARTRIDGE SRAM";}return "LEFT/RIGHT: CHANGE   A: NEXT";}
// ctx: 0 full (main menu), 1 infantry, 2 campaign, 3 tank, 4 air.  returns 1 = deploy/done, 0 = back
static int loadout_menu(int ctx,int era){
 u8 rows[14];int nr=0;
 if(ctx==0)for(int r=0;r<=R_DONE;r++)rows[nr++]=r;
 else if(ctx<=2){for(int r=R_PRIM;r<=R_PERK;r++)rows[nr++]=r;rows[nr++]=R_DONE;}
 else{rows[nr++]=ctx==3?R_TANK:R_AIR;rows[nr++]=R_DONE;}
 int sel=0,e=ctx==2?0:era,redraw=2;
 for(;;){int r=rows[sel];BEAT->screen=SCR_LOADOUT;BEAT->era=e;BEAT->menu=r;BEAT->extra=LO[e].prim|(LO[e].opt<<4)|(LO[e].perk<<8)|(tankSel[e]<<12)|(airSel[e]<<13);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;
   m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);
   m3_ctext(8,ctx==2?"LOADOUT - DOG GREEN 1944":ctx>=3?(ctx==3?"SELECT TANK":"SELECT AIRCRAFT"):cat("LOADOUT - ",ERA_NAME[e]),C_YEL,1);
   int y0=ctx>=3?40:20;
   for(int k=0;k<nr;k++){int rr_=rows[k];int y=y0+k*(ctx>=3?14:(ctx==0?8:9))+(rr_==R_DONE?3:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(rr_==R_DONE){m3_text(10,y,k==sel?"> ":"  ",C_YEL,1);m3_text(22,y,ctx==0?"SAVE & BACK":"DEPLOY",c,1);continue;}
    m3_text(7,y,RLAB[rr_],k==sel?C_YEL:C_GRAY,1);
    const char*v=lo_value(rr_,e);m3_text(63,y,v,c,1);
    if(k==sel&&lo_optcount(rr_,e)>1){m3_text(57,y,"<",C_YEL,1);m3_text(63+slen(v)*6+1,y,">",C_YEL,1);}}
   int sx=162,sy=22;
   if(r==R_TANK||ctx==3){const TankS*t=&TANKV[e][tankSel[e]];m3_text(sx,sy-1,trunc12(t->n),C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"ARMOR",t->hp,150,C_GREEN);m3_bar(sx,sy+16,"FIREPOWER",t->dmg,60,C_RED);m3_bar(sx,sy+32,"SPEED",t->spd,360,C_BLUE);
    m3_bar(sx,sy+48,"RELOAD",140-t->reload,100,C_YEL);m3_bar(sx,sy+64,"TURN",t->turn,440,C_WHITE);}
   else if(r==R_AIR||ctx==4){const AirS*a=&AIRV[e][airSel[e]];m3_text(sx,sy-1,trunc12(a->n),C_BLUE,1);sy+=12;
    m3_text(sx,sy-3,a->side?TEAMN[e][1]:TEAMN[e][0],a->side?C_RED:C_BLUE,1);sy+=8;m3_bar(sx,sy,"SPEED",a->spd,650,C_BLUE);m3_bar(sx,sy+16,"TURN",a->turn,720,C_WHITE);m3_bar(sx,sy+32,"GUNS",a->dmg*60/a->rate,180,C_RED);
    m3_bar(sx,sy+48,"ARMOR",a->hp,150,C_GREEN);m3_bar(sx,sy+64,cat(a->sec==2?"BOMBS ":a->mturn?"MISSILES ":"ROCKETS ",itoa(a->ammo)),a->ammo,12,C_YEL);
    if(a->gunner)m3_text(sx,sy+82,"+REAR GUNNER",C_GRAY,1);else if(a->mturn)m3_text(sx,sy+82,cat("TRACKING ",itoa(a->mturn/72)),C_GRAY,1);else m3_text(sx,sy+82,"UNGUIDED",C_GRAY,1);}
   else{int secd=r==R_SEC?1:(r==R_LAUN&&LO[e].laun)?2:0;Wstat w=wcalc(e,&LO[e],secd);m3_text(sx,sy-1,trunc12(w.n),C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"DAMAGE",w.dmg*w.pellets,120,C_RED);m3_bar(sx,sy+16,"FIRE RATE",600/w.rate,200,C_YEL);
    m3_bar(sx,sy+32,"RANGE",w.spd16*w.life/16,420,C_BLUE);m3_bar(sx,sy+48,"RECOIL",w.spread+1,13,RGB(255,140,60));
    m3_bar(sx,sy+64,"MOBILITY",w.move-70,45,C_GREEN);
    if(secd==2){m3_text(sx,sy+82,cat("ROCKETS ",itoa(w.mag)),C_GRAY,1);m3_text(sx,sy+92,"SPLASH DMG",C_GRAY,1);}else{m3_text(sx,sy+82,cat("MAG ",itoa(w.mag)),C_GRAY,1);char rb[12]="RLD 0.0S";int t10=w.reload/6;rb[4]='0'+t10/10%10;rb[6]='0'+t10%10;m3_text(sx,sy+92,rb,C_GRAY,1);}}
   m3_ctext(146,lo_desc(r,e),C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+nr-1)%nr;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%nr;redraw=1;sfx_menu();}
  int dir=0;if(pressed&K_LEFT)dir=-1;if(pressed&K_RIGHT)dir=1;if((pressed&K_A)&&r!=R_DONE)dir=1;
  if(ctx==0&&(pressed&(K_L|K_R))){e=(e+((pressed&K_R)?1:4))%5;redraw=1;sfx_menu();}
  if(dir&&r!=R_DONE){int n=lo_optcount(r,e);if(r==R_ERA)e=(e+n+dir)%n;else{u8*f=lo_field(r,e);*f=(*f+n+dir)%n;}redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(r==R_DONE||(pressed&K_START))){save_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();sfx_menu();return 0;}}}
// ---------------------------------------------------------------- 6v6 ARSENAL menu (all eras in one pool)
static const u8 NLAUN6[5]={2,2,2,2,2};
static const char*a6_value(int r,int*era){int i,e;Loadout*l;
 switch(r){
  case R_PRIM:e=fdec(NPRIM,g6.pI,&i);*era=e;return PRIM[e][i].n;
  case R_SEC:e=fdec(NSEC,g6.sI,&i);*era=e;return SEC[e][i].n;
  case R_LAUN:if(!g6.lI){*era=-1;return "NONE";}e=(g6.lI-1)/2;*era=e;return LAUN[e][(g6.lI-1)%2+1].n;
  case R_LETH:*era=-1;return LETH6_N[g6.leth];case R_TACT:*era=-1;return TACT6_N[g6.tact];
  case R_TANK:e=fdec(N2,g6.tkI,&i);*era=e;return TANKV[e][i].n;
  case R_AIR:e=fdec(NAIR,g6.aI,&i);*era=e;return AIRV[e][i].n;}
 e=fdec(NPRIM,g6.pI,&i);l=&LO[e];*era=-1;
 switch(r){case R_OPT:return OPTN[l->opt];case R_MUZ:return MUZN[l->muz];case R_GRIP:return GRIPN[l->grip];case R_MAG:return MAGN[l->mag];case R_PERK:return PERKN[l->perk];}return "";}
static void a6_step(int r,int dir,int tab){int i,e;u8*f=0;int n=0;const u8*cnt=0;
 switch(r){case R_PRIM:f=&g6.pI;cnt=NPRIM;break;case R_SEC:f=&g6.sI;cnt=NSEC;break;case R_TANK:f=&g6.tkI;cnt=N2;break;case R_AIR:f=&g6.aI;cnt=NAIR;break;
  case R_LAUN:if(tab){int e2=g6.lI?(g6.lI-1)/2:-1;e2=(e2+(dir>0?1:5))%5;if(e2<0)e2+=5;g6.lI=1+e2*2;}else g6.lI=(g6.lI+11+dir)%11;return;
  case R_LETH:g6.leth=(g6.leth+NLETH6+dir)%NLETH6;return;case R_TACT:g6.tact=(g6.tact+NTACT6+dir)%NTACT6;return;}
 if(cnt){n=fsum(cnt);if(tab){e=fdec(cnt,*f,&i);e=(e+(dir>0?1:4))%5;*f=fenc(cnt,e,0);}else *f=(*f+n+dir)%n;return;}
 e=fdec(NPRIM,g6.pI,&i);Loadout*l=&LO[e];
 switch(r){case R_OPT:l->opt=(l->opt+3+dir)%3;break;case R_MUZ:l->muz=(l->muz+3+dir)%3;break;case R_GRIP:l->grip=(l->grip+3+dir)%3;break;case R_MAG:l->mag=(l->mag+3+dir)%3;break;case R_PERK:l->perk=(l->perk+5+dir)%5;break;}}
static void a6_build(Loadout*lp,Wstat*w,int slot){int pi,si;int pe=fdec(NPRIM,g6.pI,&pi),se=fdec(NSEC,g6.sI,&si);*lp=LO[pe];lp->prim=pi;
 int laun=g6.lI?(g6.lI-1)%2+1:0,le=g6.lI?(g6.lI-1)/2:pe;lp->laun=laun;
 if(slot==0)*w=wcalc(pe,lp,0);else if(slot==1){Loadout ls=LO[se];ls.sec=si;ls.perk=lp->perk;ls.laun=laun;*w=wcalc(se,&ls,1);}else{Loadout ll=LO[le];ll.laun=laun;ll.perk=lp->perk;*w=wcalc(le,&ll,2);}}
static int arsenal_menu(int mode){u8 rows[12];int nr=0;
 if(mode==1){static const u8 RI[11]={R_PRIM,R_SEC,R_LAUN,R_OPT,R_MUZ,R_GRIP,R_MAG,R_LETH,R_TACT,R_PERK,R_DONE};for(int k=0;k<11;k++)rows[nr++]=RI[k];}
 else{rows[nr++]=mode==2?R_TANK:R_AIR;rows[nr++]=R_DONE;}
 int sel=0,redraw=2;
 for(;;){int r=rows[sel];BEAT->screen=SCR_LOADOUT;BEAT->menu=r;BEAT->mode=mode;BEAT->extra=g6.pI|(g6.sI<<6)|(g6.lI<<10)|(g6.tkI<<14)|(g6.aI<<18);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);
   m3_ctext(8,mode==1?"6V6 ARSENAL - ALL ERAS":mode==2?"6V6 ARSENAL - ANY TANK":"6V6 ARSENAL - ANY AIRCRAFT",C_YEL,1);
   int y0=mode==1?20:40;
   for(int k=0;k<nr;k++){int q=rows[k];int y=y0+k*(mode==1?11:14)+(q==R_DONE?2:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(q==R_DONE){m3_text(10,y,k==sel?"> ":"  ",C_YEL,1);m3_text(22,y,"DEPLOY",c,1);continue;}
    int e;const char*v=a6_value(q,&e);m3_text(7,y,RLAB[q],k==sel?C_YEL:C_GRAY,1);m3_text(63,y,v,c,1);
    if(k==sel){m3_text(57,y,"<",C_YEL,1);m3_text(63+slen(v)*6+1,y,">",C_YEL,1);}}
   int sx=162,sy=22,e;const char*nm=a6_value(r==R_DONE?rows[0]:r,&e);
   if(r==R_TANK||mode==2){int i;int te=fdec(N2,g6.tkI,&i);const TankS*t=&TANKV[te][i];m3_text(sx,sy-1,trunc12(t->n),C_BLUE,1);m3_text(sx,sy+8,cat3("TAB: ",ERA_TAG[te],""),C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"ARMOR",t->hp,150,C_GREEN);m3_bar(sx,sy+16,"FIREPOWER",t->dmg,60,C_RED);m3_bar(sx,sy+32,"SPEED",t->spd,360,C_BLUE);m3_bar(sx,sy+48,"RELOAD",140-t->reload,100,C_YEL);m3_bar(sx,sy+64,"TURN",t->turn,440,C_WHITE);}
   else if(r==R_AIR||mode==3){int i;int ae=fdec(NAIR,g6.aI,&i);const AirS*a=&AIRV[ae][i];m3_text(sx,sy-1,trunc12(a->n),C_BLUE,1);m3_text(sx,sy+8,cat3("TAB: ",ERA_TAG[ae],""),C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"SPEED",a->spd,650,C_BLUE);m3_bar(sx,sy+16,"TURN",a->turn,720,C_WHITE);m3_bar(sx,sy+32,"GUNS",a->dmg*60/a->rate,180,C_RED);m3_bar(sx,sy+48,"ARMOR",a->hp,150,C_GREEN);
    m3_bar(sx,sy+64,cat(a->sec==2?"BOMBS ":a->mturn?"MISSILES ":"ROCKETS ",itoa(a->ammo)),a->ammo,12,C_YEL);}
   else{int slot=r==R_SEC?1:(r==R_LAUN&&g6.lI)?2:0;Loadout lp;Wstat w;a6_build(&lp,&w,slot);int i,we=slot==1?fdec(NSEC,g6.sI,&i):slot==2?(g6.lI-1)/2:fdec(NPRIM,g6.pI,&i);
    m3_text(sx,sy-1,trunc12(w.n),C_BLUE,1);m3_text(sx,sy+8,cat3("TAB: ",ERA_TAG[we],""),C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"DAMAGE",w.dmg*w.pellets,120,C_RED);m3_bar(sx,sy+16,"FIRE RATE",600/w.rate,200,C_YEL);m3_bar(sx,sy+32,"RANGE",w.spd16*w.life/16,420,C_BLUE);
    m3_bar(sx,sy+48,"RECOIL",w.spread+1,13,RGB(255,140,60));m3_bar(sx,sy+64,"MOBILITY",w.move-70,45,C_GREEN);(void)nm;(void)e;}
   const char*d=r==R_PRIM?cat3("L/R ERA TAB  <> ",itoa(fsum(NPRIM))," PRIMARIES"):r==R_SEC?cat3("L/R ERA TAB  <> ",itoa(fsum(NSEC))," SIDEARMS"):r==R_LAUN?"L/R ERA TAB  <> 10 LAUNCHERS + NONE":
    r==R_TANK?"L/R ERA TAB  <> 10 TANKS, ALL ERAS":r==R_AIR?cat3("L/R ERA TAB  <> ",itoa(fsum(NAIR))," AIRCRAFT"):r==R_DONE?"SAVED TO SRAM - BOTS GET MIRRORED KITS":"ATTACHMENTS APPLY TO THE PRIMARY";
   m3_ctext(146,d,C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+nr-1)%nr;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%nr;redraw=1;sfx_menu();}
  int dir=0;if(pressed&K_LEFT)dir=-1;if(pressed&K_RIGHT)dir=1;if((pressed&K_A)&&r!=R_DONE)dir=1;
  if(r!=R_DONE&&(pressed&(K_L|K_R))){a6_step(r,(pressed&K_R)?1:-1,1);redraw=1;sfx_menu();}
  else if(dir&&r!=R_DONE){a6_step(r,dir,0);redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(r==R_DONE||(pressed&K_START))){save_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();sfx_menu();return 0;}}}
