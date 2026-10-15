// ===================================================================== LOADOUT / CUSTOMIZE (+ SRAM save)
typedef struct{const char*n;u8 dmg,rate,spd16,spread,pellets,life,mag,reload,move;}Wpn;
typedef struct{int dmg,rate,spd16,spread,pellets,life,mag,reload,move;const char*n;}Wstat;
static const u8 NPRIM[5]={5,5,5,4,4};
static const Wpn PRIM[5][5]={
 {{"M1 GARAND",50,20,84,2,1,42,8,90,100},{"THOMPSON",20,6,72,7,1,30,30,110,104},{"BAR M1918",32,8,84,5,1,42,20,130,92},{"KAR98K",95,40,100,1,1,50,5,120,98},{"STG 44",26,7,84,5,1,38,30,110,100}},
 {{"AK-47",30,7,88,6,1,40,30,110,98},{"M16A1",26,6,96,4,1,40,20,100,102},{"M60",26,5,84,8,1,40,100,240,86},{"DRAGUNOV SVD",75,24,112,1,1,52,10,120,96},{"UZI",18,4,76,8,1,28,32,90,108}},
 {{"M4A1",26,5,96,4,1,40,30,95,102},{"HK416",28,5,96,3,1,42,30,100,100},{"AK-12",30,6,96,4,1,42,30,100,99},{"SCAR-H",40,8,100,4,1,46,20,110,96},{"BARRETT M82",120,45,140,1,1,56,10,160,88}},
 {{"SMART RIFLE",30,6,104,1,1,44,30,100,100},{"RAILGUN",110,42,170,0,1,46,5,150,92},{"LASER CARBINE",22,3,140,2,1,36,60,140,100},{"PULSE SMG",18,3,96,6,1,30,45,90,108},{0}},
 {{"PLASMA RIFLE",32,7,90,4,1,40,30,110,100},{"COILGUN",85,30,150,1,1,52,8,130,94},{"GAUSS SMG",18,3,100,6,1,30,50,95,108},{"ION REPEATER",22,4,96,7,1,40,80,200,90},{0}}};
static const Wpn SEC[5][2]={
 {{"M1911",34,12,72,4,1,26,7,70,106},{"LUGER P08",30,10,72,3,1,26,8,70,106}},
 {{"MAKAROV PM",28,10,72,4,1,26,8,65,106},{"M1911A1",34,12,72,4,1,26,7,70,106}},
 {{"GLOCK 17",25,8,80,4,1,26,17,60,106},{"M870 SHOTGUN",18,30,80,12,5,18,8,150,100}},
 {{"SMART PISTOL",26,9,90,1,1,28,12,60,106},{"ARC PISTOL",40,18,100,3,1,24,6,70,106}},
 {{"NEEDLER",16,4,90,5,1,26,24,70,106},{"ARC SCATTER",22,30,90,10,4,22,6,100,100}}};
static const char*const OPTN[3]={"IRON SIGHTS","RED DOT","4X SCOPE"};
static const char*const MUZN[3]={"STANDARD","SUPPRESSOR","COMPENSATOR"};
static const char*const GRIPN[3]={"NONE","VERTICAL GRIP","ANGLED GRIP"};
static const char*const MAGN[3]={"STANDARD MAG","EXTENDED MAG","FAST MAG"};
static const char*const LETN[3]={"FRAG","SEMTEX","CLUSTER"};
static const char*const TACN[3]={"FLASHBANG","SMOKE","STIM"};
static const char*const PERKN[5]={"SPRINTER","FLAK JACKET","SCAVENGER","SLEIGHT OF HAND","JUGGERNAUT"};
static const char*const OPTD[3]={"NO CHANGE","RECOIL -1","RECOIL -2 RANGE +15% MOB -3"};
static const char*const MUZD[3]={"NO CHANGE","BOTS SPOT YOU LATER, RANGE -15%","RECOIL -2, FIRE RATE -10%"};
static const char*const GRIPD[3]={"NO CHANGE","RECOIL -1","RELOAD -15%"};
static const char*const MAGD[3]={"NO CHANGE","MAG +50%, RELOAD +15%, MOB -3","RELOAD -35%"};
static const char*const LETD[3]={"4 SEC FUSE, BIG BLAST","SHORT FUSE, STICKS TO WALLS","3 SMALLER BLASTS"};
static const char*const TACD[3]={"STUNS ENEMIES 2.5 SEC","BLOCKS ENEMY LINE OF SIGHT","INSTANT +60 HP"};
static const char*const PERKD[5]={"MOVE SPEED +12%","EXPLOSIVE DAMAGE -50%","KILLS REFILL AMMO + LETHAL","RELOAD -40%","130 HP, MOVE -6%"};
typedef struct{const char*n;s16 spd,turn,reload,dmg,hp,sspd;}TankS;
static const TankS TANKV[5][2]={
 {{"M4 SHERMAN",240,320,85,32,100,1100},{"TIGER I",170,230,110,48,150,1300}},
 {{"M48 PATTON",250,300,80,36,115,1200},{"T-55",260,310,85,36,105,1200}},
 {{"M1A2 ABRAMS",290,320,70,42,130,1450},{"T-90M",280,340,65,40,120,1400}},
 {{"HOVER MBT",350,420,55,34,105,1500},{"RAIL WALKER",220,300,80,60,140,2200}},
 {{"GRAV TANK",320,420,50,30,100,1600},{"LUNAR CRAWLER",240,320,65,42,140,1600}}};
typedef struct{const char*n;const char*mn;s16 spd,turn;u8 rate,dmg;s16 bspd;u8 ammo;s16 mturn,hp;}AirS;
static const u8 NAIR[5]={3,2,2,2,2};
static const AirS AIRV[5][3]={
 {{"P-51D MUSTANG","ROCKETS",390,560,4,9,1100,4,0,100},{"SPITFIRE IX","ROCKETS",370,620,4,8,1100,4,0,90},{"F6F HELLCAT","HVAR ROCKETS",360,540,4,11,1100,6,0,140}},
 {{"MIG-21","K-13 AAM",480,520,5,11,1300,4,380,95},{"F-4 PHANTOM","AIM-7",460,470,5,12,1300,6,360,120}},
 {{"F-35A","AIM-120",520,540,4,10,1400,6,560,100},{"SU-57","R-77",540,580,4,10,1400,6,540,105}},
 {{"NGAD 6TH-GEN","SEEKERS",600,640,4,9,1500,6,700,100},{"X-90 ARROW","SWARM",640,600,4,8,1500,8,650,90}},
 {{"SF-1 STARFIGHTER","TORPEDOES",520,700,5,12,1300,8,700,100},{"VOID INTERCEPTOR","PLASMA TORP",580,660,4,10,1400,6,720,95}}};
typedef struct{u8 prim,sec,opt,muz,grip,mag,leth,tact,perk;}Loadout;
static Loadout LO[5];static u8 tankSel[5],airSel[5];
static const char save_tag[] __attribute__((used,aligned(4)))="SRAM_V113";
#define SRAM ((volatile u8*)0x0E000000)
#define SAVE_N (5*9+10)
static void lo_pack(u8*b){for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<9;k++)b[e*9+k]=p[k];}for(int e=0;e<5;e++){b[45+e]=tankSel[e];b[50+e]=airSel[e];}}
static void lo_defaults(void){memset(LO,0,sizeof LO);memset(tankSel,0,5);memset(airSel,0,5);}
static int lo_valid(void){for(int e=0;e<5;e++){Loadout*l=&LO[e];if(l->prim>=NPRIM[e]||l->sec>1||l->opt>2||l->muz>2||l->grip>2||l->mag>2||l->leth>2||l->tact>2||l->perk>4||tankSel[e]>1||airSel[e]>=NAIR[e])return 0;}return 1;}
static void save_write(void){u8 b[SAVE_N];lo_pack(b);u8 sum=0x5A;SRAM[0]='A';SRAM[1]='O';SRAM[2]='E';SRAM[3]='L';SRAM[4]=1;
 for(int i=0;i<SAVE_N;i++){SRAM[5+i]=b[i];sum+=b[i]*(i+1);}SRAM[5+SAVE_N]=sum;}
static int save_load(void){lo_defaults();if(SRAM[0]!='A'||SRAM[1]!='O'||SRAM[2]!='E'||SRAM[3]!='L'||SRAM[4]!=1)return 0;
 u8 b[SAVE_N];u8 sum=0x5A;for(int i=0;i<SAVE_N;i++){b[i]=SRAM[5+i];sum+=b[i]*(i+1);}if(sum!=SRAM[5+SAVE_N])return 0;
 for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<9;k++)p[k]=b[e*9+k];tankSel[e]=b[45+e];airSel[e]=b[50+e];}
 if(!lo_valid()){lo_defaults();return 0;}return 1;}
static Wstat wcalc(int era,const Loadout*l,int secondary){const Wpn*w=secondary?&SEC[era][l->sec]:&PRIM[era][l->prim];Wstat s;
 s.n=w->n;s.dmg=w->dmg;s.rate=w->rate;s.spd16=w->spd16;s.spread=w->spread;s.pellets=w->pellets;s.life=w->life;s.mag=w->mag;s.reload=w->reload;s.move=w->move;
 if(!secondary){
  if(l->opt==1)s.spread-=1;if(l->opt==2){s.spread-=2;s.life=s.life*115/100;s.move-=3;}
  if(l->muz==1)s.life=s.life*85/100;if(l->muz==2){s.spread-=2;s.rate=s.rate*110/100+1;}
  if(l->grip==1)s.spread-=1;if(l->grip==2)s.reload=s.reload*85/100;
  if(l->mag==1){s.mag=s.mag*3/2;s.reload=s.reload*115/100;s.move-=3;}if(l->mag==2)s.reload=s.reload*65/100;}
 if(l->perk==3)s.reload=s.reload*60/100;if(l->perk==0)s.move=s.move*112/100;if(l->perk==4)s.move=s.move*94/100;
 if(s.spread<0)s.spread=0;return s;}
// ---------------------------------------------------------------- menu
enum{R_ERA,R_PRIM,R_SEC,R_OPT,R_MUZ,R_GRIP,R_MAG,R_LETH,R_TACT,R_PERK,R_TANK,R_AIR,R_DONE};
static const char*const RLAB[13]={"ERA","PRIMARY","SIDEARM","OPTIC","MUZZLE","GRIP","MAGAZINE","LETHAL","TACTICAL","PERK","TANK","AIRCRAFT",""};
static const char*const ERA_NAME[5]={"WORLD WAR II","COLD WAR","MODERN","FUTURISTIC","SPACE"};
static void m3_fill(int x0,int y0,int w,int h,u16 c){for(int y=y0;y<y0+h;y++){u16*p=&fb[y*240+x0];if(!(x0&1)&&!(w&1)&&w>=8)fill32(p,c|(c<<16),w/2);else for(int x=0;x<w;x++)p[x]=c;}}
static void m3_bar(int x,int y,const char*lab,int v,int max,u16 col){m3_text(x,y,lab,C_GRAY,1);int w=74;int f=max>0?v*w/max:0;f=clampi(f,1,w);
 m3_fill(x,y+8,w,4,RGB(40,46,70));m3_fill(x,y+8,f,4,col);}
static int lo_optcount(int r,int e){switch(r){case R_ERA:return 5;case R_PRIM:return NPRIM[e];case R_SEC:case R_TANK:return 2;case R_AIR:return NAIR[e];case R_PERK:return 5;case R_DONE:return 1;}return 3;}
static u8*lo_field(int r,int e){Loadout*l=&LO[e];switch(r){case R_PRIM:return &l->prim;case R_SEC:return &l->sec;case R_OPT:return &l->opt;case R_MUZ:return &l->muz;case R_GRIP:return &l->grip;
 case R_MAG:return &l->mag;case R_LETH:return &l->leth;case R_TACT:return &l->tact;case R_PERK:return &l->perk;case R_TANK:return &tankSel[e];case R_AIR:return &airSel[e];}return 0;}
static const char*lo_value(int r,int e){Loadout*l=&LO[e];switch(r){case R_ERA:return ERA_NAME[e];case R_PRIM:return PRIM[e][l->prim].n;case R_SEC:return SEC[e][l->sec].n;case R_OPT:return OPTN[l->opt];
 case R_MUZ:return MUZN[l->muz];case R_GRIP:return GRIPN[l->grip];case R_MAG:return MAGN[l->mag];case R_LETH:return LETN[l->leth];case R_TACT:return TACN[l->tact];case R_PERK:return PERKN[l->perk];
 case R_TANK:return TANKV[e][tankSel[e]].n;case R_AIR:return AIRV[e][airSel[e]].n;}return "";}
static const char*lo_desc(int r,int e){Loadout*l=&LO[e];switch(r){case R_OPT:return OPTD[l->opt];case R_MUZ:return MUZD[l->muz];case R_GRIP:return GRIPD[l->grip];case R_MAG:return MAGD[l->mag];
 case R_LETH:return LETD[l->leth];case R_TACT:return TACD[l->tact];case R_PERK:return PERKD[l->perk];case R_ERA:return "L/R OR LEFT/RIGHT: CHANGE ERA";
 case R_TANK:return tankSel[e]?"ENEMY FIELDS THE OTHER TANK":"ENEMY FIELDS THE OTHER TANK";case R_AIR:return (e==0&&airSel[e]==2)?"US NAVY - RUGGED, HEAVY GUNS":"ENEMY FLIES A RIVAL AIRCRAFT";case R_DONE:return "SAVED TO CARTRIDGE SRAM";}return "LEFT/RIGHT: CHANGE   A: NEXT";}
// ctx: 0 full (main menu), 1 infantry, 2 campaign, 3 tank, 4 air.  returns 1 = deploy/done, 0 = back
static int loadout_menu(int ctx,int era){
 u8 rows[13];int nr=0;
 if(ctx==0)for(int r=0;r<=R_DONE;r++)rows[nr++]=r;
 else if(ctx<=2){for(int r=R_PRIM;r<=R_PERK;r++)rows[nr++]=r;rows[nr++]=R_DONE;}
 else{rows[nr++]=ctx==3?R_TANK:R_AIR;rows[nr++]=R_DONE;}
 int sel=0,e=ctx==2?0:era,redraw=2;
 for(;;){int r=rows[sel];BEAT->screen=SCR_LOADOUT;BEAT->era=e;BEAT->menu=r;BEAT->extra=LO[e].prim|(LO[e].opt<<4)|(LO[e].perk<<8)|(tankSel[e]<<12)|(airSel[e]<<13);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;
   m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);
   m3_ctext(8,ctx==2?"LOADOUT - DOG GREEN 1944":ctx>=3?(ctx==3?"SELECT TANK":"SELECT AIRCRAFT"):cat("LOADOUT - ",ERA_NAME[e]),C_YEL,1);
   int y0=ctx>=3?40:22;
   for(int k=0;k<nr;k++){int rr_=rows[k];int y=y0+k*(ctx>=3?14:9)+(rr_==R_DONE?3:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(rr_==R_DONE){m3_text(10,y,k==sel?"> ":"  ",C_YEL,1);m3_text(22,y,ctx==0?"SAVE & BACK":"DEPLOY",c,1);continue;}
    m3_text(10,y,RLAB[rr_],k==sel?C_YEL:C_GRAY,1);
    const char*v=lo_value(rr_,e);m3_text(62,y,v,c,1);
    if(k==sel&&lo_optcount(rr_,e)>1){m3_text(56,y,"<",C_YEL,1);m3_text(62+slen(v)*6+1,y,">",C_YEL,1);}}
   int sx=156,sy=22;
   if(r==R_TANK||ctx==3){const TankS*t=&TANKV[e][tankSel[e]];m3_text(sx,sy-1,t->n,C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"ARMOR",t->hp,150,C_GREEN);m3_bar(sx,sy+16,"FIREPOWER",t->dmg,60,C_RED);m3_bar(sx,sy+32,"SPEED",t->spd,360,C_BLUE);
    m3_bar(sx,sy+48,"RELOAD",140-t->reload,100,C_YEL);m3_bar(sx,sy+64,"TURN",t->turn,440,C_WHITE);}
   else if(r==R_AIR||ctx==4){const AirS*a=&AIRV[e][airSel[e]];m3_text(sx,sy-1,a->n,C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"SPEED",a->spd,650,C_BLUE);m3_bar(sx,sy+16,"TURN",a->turn,720,C_WHITE);m3_bar(sx,sy+32,"GUNS",a->dmg*60/a->rate,180,C_RED);
    m3_bar(sx,sy+48,"ARMOR",a->hp,150,C_GREEN);m3_bar(sx,sy+64,a->mturn?cat("MISSILES ",itoa(a->ammo)):cat("ROCKETS ",itoa(a->ammo)),a->ammo,8,C_YEL);
    if(a->mturn)m3_text(sx,sy+82,cat("TRACKING ",itoa(a->mturn/72)),C_GRAY,1);else m3_text(sx,sy+82,"UNGUIDED",C_GRAY,1);}
   else{int secd=r==R_SEC;Wstat w=wcalc(e,&LO[e],secd);m3_text(sx,sy-1,w.n,C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"DAMAGE",w.dmg*w.pellets,120,C_RED);m3_bar(sx,sy+16,"FIRE RATE",600/w.rate,200,C_YEL);
    m3_bar(sx,sy+32,"RANGE",w.spd16*w.life/16,420,C_BLUE);m3_bar(sx,sy+48,"RECOIL",w.spread+1,13,RGB(255,140,60));
    m3_bar(sx,sy+64,"MOBILITY",w.move-70,45,C_GREEN);
    m3_text(sx,sy+82,cat("MAG ",itoa(w.mag)),C_GRAY,1);{char rb[12]="RLD 0.0S";int t10=w.reload/6;rb[4]='0'+t10/10%10;rb[6]='0'+t10%10;m3_text(sx,sy+92,rb,C_GRAY,1);}}
   m3_ctext(146,lo_desc(r,e),C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+nr-1)%nr;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%nr;redraw=1;sfx_menu();}
  int dir=0;if(pressed&K_LEFT)dir=-1;if(pressed&K_RIGHT)dir=1;if((pressed&K_A)&&r!=R_DONE)dir=1;
  if(ctx==0&&(pressed&(K_L|K_R))){e=(e+((pressed&K_R)?1:4))%5;redraw=1;sfx_menu();}
  if(dir&&r!=R_DONE){int n=lo_optcount(r,e);if(r==R_ERA)e=(e+n+dir)%n;else{u8*f=lo_field(r,e);*f=(*f+n+dir)%n;}redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(r==R_DONE||(pressed&K_START))){save_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();sfx_menu();return 0;}}}
