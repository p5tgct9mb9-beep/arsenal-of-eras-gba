// ===================================================================== v1.3.1 SPEC OPS: BRADLEY (overhaul)
// Side-scrolling armoured platformer: M2/M3/M7 Bradley family. 4 worlds x 7 set-piece courses, multi-phase bosses.
// D-pad L/R drive, UP/DOWN turret elevation (8 steps, independent of hull)  A: 25mm Bushmaster (hold A = turret locked, reverse & fire)
// SELECT tap: AP <-> HE (airburst)  SELECT hold: dismount squad  R: TOW (hold R = wire-guided, UP/DOWN steer)  L: coax 7.62 MG
// L+R: smoke grenades  B: jump  SELECT+DOWN: thermal sight (night courses)
// SRAM 0x980 'AOEB' v1 (layout unchanged from v1.3): cs bit0 clear, bits1-3 intel, bits4-6 stars (no damage / par time / 80% kills).
#include "braddb.inc"
#define SCR_BRAD 58
#define SCR_BRSEL 59
#define BR_OFF 0x980
enum{UP_ERA=1,UP_BUSK=2,UP_TOW=4,UP_TURRET=8,UP_APS=16};
static const u8 BRV_HP[5]={24,26,30,24,26},BRV_TOW[5]={4,4,5,6,3},BRV_RATE[5]={8,7,7,8,8},BRV_SQ[5]={2,2,2,1,2};
static const u16 BRV_SPD[5]={0x1C0,0x1C0,0x1B0,0x200,0x1C0};
static const char*const BRV_D[5]={"DESERT STORM ERA. 4 TOW, SQUAD","CIV SIGHT SPOTS IEDS + LASERS","HEAVIEST ARMOUR. 5 TOW","CAVALRY SCOUT: FAST, 6 TOW","FIRE SUPPORT: R CALLS ARTILLERY"};
#define NUPG 5
static const u8 UPG_COST[NUPG]={9,18,30,45,60};static const char*const UPG_N[NUPG]={"ERA ARMOUR","BUSK KIT","EXTRA TOW","FAST TURRET","IRON FIST APS"};
static const char*const UPG_D[NUPG]={"-40% FROM RPG / ATGM / SHELLS","+4 HULL, HALF MINE/IED DAMAGE","+2 TOW MISSILES","25MM FIRES 30% FASTER","2 CHARGES KILL INCOMING RPG/ATGM"};
typedef struct{u8 cs[28];u16 best[28];u8 var,camo,upg,lastW,lastC,allun;}BrSave;
static BrSave BS;
static void bs_write(void){const u8*b=(const u8*)&BS;u8 s=0x3C;SRAM[BR_OFF]='A';SRAM[BR_OFF+1]='O';SRAM[BR_OFF+2]='E';SRAM[BR_OFF+3]='B';SRAM[BR_OFF+4]=1;
 for(int i=0;i<(int)sizeof BS;i++){SRAM[BR_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[BR_OFF+5+sizeof BS]=s;}
static void bs_read(void){memset(&BS,0,sizeof BS);BS.camo=255;if(SRAM[BR_OFF]!='A'||SRAM[BR_OFF+1]!='O'||SRAM[BR_OFF+2]!='E'||SRAM[BR_OFF+3]!='B'||SRAM[BR_OFF+4]!=1)return;
 u8 b[sizeof BS];u8 s=0x3C;for(int i=0;i<(int)sizeof BS;i++){b[i]=SRAM[BR_OFF+5+i];s+=b[i]*(i+1);}if(s!=SRAM[BR_OFF+5+sizeof BS])return;memcpy(&BS,b,sizeof BS);}
static int br_medals(void){int n=0;for(int i=0;i<28;i++)n+=__builtin_popcount(BS.cs[i]&0x7E);return n;}
static int br_intel(void){int n=0;for(int i=0;i<28;i++)n+=__builtin_popcount(BS.cs[i]&0x0E);return n;}
static int br_stars(void){int n=0;for(int i=0;i<28;i++)n+=__builtin_popcount(BS.cs[i]&0x70);return n;}
static int br_wclear(int w){return BS.cs[w*7+6]&1;}
static int br_var_open(int v){return BS.allun||v==0||(v<=3&&br_wclear(v-1))||(v==4&&(br_wclear(3)||br_medals()>=60));}
static int br_course_open(int i){return BS.allun||i==0||(BS.cs[i-1]&1)||(BS.cs[i]&1);}
static int br_upg_open(int k){return BS.allun||br_medals()>=UPG_COST[k];}
static int br_isauto(void){return *(volatile u32*)0x0203FFD0==0x4F545541u;}
// ---------------------------------------------------------------- state
static const s8 BR_ELEV[8]={-6,0,7,16,25,36,46,57};   // turret elevation (256-unit angles): -8,0,10,22,35,50,65,80 deg
typedef struct{s32 x,y,vx,vy;s16 hp,maxhp,tow,smoke,sq,inv,face,on,lives,dead,deadT,anim,towId,water,aim,aimCd,ammo,rel,heat,ovh,cd,mgcd,towcd,recoil,aps,apsT,land,selT,tface,hitT,flash,dmg,thermal,skid;s16 mag[2];}BrV;
static BrV V;
typedef struct{s32 x,y,vx,vy;s16 hp,mhp,t,cd,c,st,st2,a;u8 k,alive,face,fall,wave,pad;}BrE;
#define NBE 48
static BrE BE[NBE] EWRAM;static int nBE,brEi;
typedef struct{s32 x,y,vx,vy;s16 life,dmg;u8 k,team,guided,trc;}BrB;
#define NBB 48
static BrB BB[NBB] EWRAM;
enum{BK_AP,BK_HE,BK_MG,BK_TOW,BK_ALLY,BK_EBUL,BK_RPG,BK_ATGM,BK_SHELL,BK_ARTY,BK_ROCKET,BK_EHE,BK_FLAK};
typedef struct{s32 x,y;s16 vx,vy;s16 t,life;u8 k,big;}BrP;
#define NBP 64
static BrP BP[NBP] EWRAM;
enum{PK_CASE,PK_DEB,PK_SPARK,PK_DUST,PK_SMOKE,PK_EMBER,PK_TOSS,PK_FB,PK_FLASH,PK_CORPSE,PK_FLARE,PK_APS,PK_GLINT};
typedef struct{s32 x,y,vx;s16 hp,t,cd;u8 alive,inHouse;}BrAlly;
static BrAlly BA[3];
typedef struct{s16 x,t;u8 team;}BrWarn;static BrWarn BW[8];
typedef struct{s16 cx,cy,t;}BrCrumb;static BrCrumb BCR[24];
static s16 WXx[18],WXy[18];
static struct{s32 x,y;s16 hp,max,t,cd,cd2,cd3,flash,dead,on,kind,face,x0,ph,era,aps,apsT,flare,hide,glint,dir;s16 car[3];}BO;
static int brW,brC,brI,brT,brDone,brKills,brHostile,brMedal,brChkX,brSmokeT,brToastT,brArenaX,brArenaW,brScore,brDeaths,brFire,brTier;static const char*brToast;
static int brCombo,brComboT,brObj,brObjN,brObjOk,brGoalX,brLock,brLockL,brLockR,brWave,brWaveT,brWaveLeft,brHouse,brHouseP,brHouseX,brConvoy,brFail,brLaser,brLaserT,brLightT,brNoDmg,brAimBlink;
static const BrLv*BL;
#define OT_V 64
#define OT_BAR 192
#define OT_BO 208
#define OT_C 304
#define BRP_CAM 0
#define BRP_EN 1
#define BRP_MISC 2
#define BRP_BURNT 4
#define BRP_HOT 6
static const u8 TIER_LOCK[3]={120,90,60},TIER_TANK[3]={70,50,35},TIER_DMG[3]={3,4,5},TIER_RPG[3]={44,32,22};
static void br_toast(const char*s){brToast=s;brToastT=100;}
static void bspr(int x,int y,int tile,int shape,int size,int pal,int hf){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14)|(hf?1<<12:0);o[2]=tile|(1<<10)|(pal<<12);noam++;}
static void bsprx(int x,int y,int tile,int shape,int size,int pal,int hf,int prio,int semi){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14)|(semi?1<<10:0);o[1]=(x&511)|(size<<14)|(hf?1<<12:0);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static void br_decode(void){pW=BL->w;const u8*t=BL->t;
 for(int y=0;y<PH;y++){int x=0;while(x<pW){int v=*t++,n=*t++;for(int i=0;i<n;i++)pm[PMI(x+i,y)]=v;x+=n;}}
 for(int y=1;y<PH;y++)for(int x=0;x<pW;x++)if(pm[PMI(x,y)]==T_TOP){int a=pm[PMI(x,y-1)];if(a==T_TOP||a==T_FILL||a==T_WTOP||a==T_WATER||a==T_BRICK)pm[PMI(x,y)]=a==T_WTOP||a==T_WATER||a==T_BRICK?T_TOP:T_FILL;}}
// ---------------------------------------------------------------- palettes: night / thermal / sandstorm / lightning built from sources
static u16 br_tc(u16 c,int mode){int r=c&31,g=(c>>5)&31,b=(c>>10)&31,l=(r*3+g*5+b*2)/10;
 switch(mode){case 1:r=r*2/5;g=g*2/5+1;b=b/2+5;break;                         // night
  case 2:l=l*3/5;r=g=b=l;break;                                               // thermal cold background
  case 3:l=l/2+16;if(l>31)l=31;r=g=b=l;break;                                  // thermal white-hot
  case 4:l=l*3/4+3;r=g=b=l;break;                                              // thermal own vehicle (warm grey)
  case 5:r=(r+26)/2;g=(g+21)/2;b=(b+13)/2;break;                               // sandstorm haze
  case 6:r=(r*3+28)/4;g=(g*3+24)/4;b=(b*3+18)/4;break;                         // light sand tint (terrain)
  case 7:r=(r+31)/2;g=(g+31)/2;b=(b+31)/2;break;}                               // lightning
 if(r>31)r=31;if(g>31)g=31;if(b>31)b=31;return r|(g<<5)|(b<<10);}
static void br_palettes(void){int th=BL->th,fl=BL->fl,cm=BS.camo<3?BS.camo:BR_WCAMO[brW];int thr=V.thermal,lit=brLightT>0;
 for(int i=0;i<80;i++){u16 c=i<48?pf_pal[th*48+i]:i<64?BRBF_P[brW][i-48]:BRBM_P[brW][i-64];
  int bank=i>>4;if(thr)c=br_tc(c,2);else{if(fl&FL_SAND)c=br_tc(c,bank>=3?5:6);if(fl&FL_NIGHT)c=br_tc(c,1);if(lit)c=br_tc(c,7);}
  PAL_BG[i]=c;}
 PAL_BG[0]=PAL_BG[53];
 for(int i=0;i<16;i++){u16 a=BR_CAMO[cm][i],e=brp_enemy[i],m=brp_misc[i],f=fx_pal[i],b=brp_burnt[i],h=brp_hot[i];
  if(thr){a=br_tc(a,4);e=br_tc(e,3);m=br_tc(m,4);b=br_tc(b,3);}
  else if(fl&FL_NIGHT){a=br_tc(a,1);e=br_tc(e,1);m=br_tc(m,1);b=br_tc(b,1);if(lit){a=br_tc(a,7);e=br_tc(e,7);}}
  if(i==0){a=e=m=b=h=0;}
  PAL_OBJ[i]=a;PAL_OBJ[16+i]=e;PAL_OBJ[32+i]=m;PAL_OBJ[48+i]=f;PAL_OBJ[64+i]=b;PAL_OBJ[96+i]=h;}}
static void br_load_gfx(void){int th=BL->th;
 vsync();REG_DISPCNT=0x80;
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(PF_TILES[th],CHARBLOCK(1),34*32);
 dma32(BRBM_T[brW],(u8*)CHARBLOCK(1)+64*32,BRBM_N[brW]*8);
 dma32(BRBF_T[brW],CHARBLOCK(2),BRBF_N[brW]*8);
 dma32(BRBF_M[brW],SCREENBLOCK(31),512);
 for(int i=0;i<1024;i++)bgc[i]=BRBM_M[brW][i];
 dma32(fx_tiles,OBJ_TILES,sizeof(fx_tiles)/4);
 dma32(br_veh+BS.var*128*8,(u8*)OBJ_TILES+OT_V*32,128*8);
 dma32(br_barrel,(u8*)OBJ_TILES+OT_BAR*32,16*8);
 if(BL->boss<5)dma32(br_boss+BL->boss*96*8,(u8*)OBJ_TILES+OT_BO*32,96*8);
 dma32(br_com,(u8*)OBJ_TILES+OT_C*32,sizeof(br_com)/4);
 br_palettes();
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=1|(1<<2)|(30<<8);REG_BGCNT(2)=2|(1<<2)|(29<<8);REG_BGCNT(3)=3|(2<<2)|(31<<8);
 REG_BLDCNT=(1<<6)|(1<<9)|(1<<10)|(1<<11)|(1<<13);REG_BLDALPHA=10|(8<<8);
 hud_clear();oam_begin();memset(bgw,0,2048);
 REG_DISPCNT=0|(1<<6)|(1<<8)|(1<<9)|(1<<10)|(1<<11)|(1<<12);}
static int br_solid(int px,int py){return TFL[ptile(px>>4,py>>4)]&F_SOL;}
static int br_floor(int px,int py,int oldpy){int f=TFL[ptile(px>>4,py>>4)];if(f&F_SOL)return 1;if((f&F_ONE)&&((oldpy-1)>>4)<(py>>4))return 1;return 0;}
static int br_ground(int x,int y){int gy=y;while(gy<PH*16&&!br_floor(x,gy,gy-1))gy++;while(gy>0&&br_solid(x,gy-1))gy--;return gy;}
static int br_hostile(int k){return k<=BE_SAM;}
static int br_armour(int k){return k==BE_TANK||k==BE_BTR||k==BE_BMP2||k==BE_TECH||k==BE_FLEE||k==BE_CAR;}
// ---------------------------------------------------------------- particles
static BrP*bp_add(int k,int x,int y,int vx,int vy,int life){int best=-1;for(int i=0;i<NBP;i++)if(BP[i].life<=0){best=i;break;}
 if(best<0){if(k!=PK_FB&&k!=PK_TOSS)return 0;best=(brT*7)%NBP;}BrP*p=&BP[best];p->x=x<<8;p->y=y<<8;p->vx=vx;p->vy=vy;p->k=k;p->t=0;p->life=life;p->big=0;return p;}
static void br_fireball(int x,int y,int big){BrP*p=bp_add(PK_FB,x,y,0,big?-0x10:-0x20,big?36:24);if(p)p->big=big;
 for(int i=0;i<(big?7:3);i++)bp_add(PK_DEB,x,y,(int)(rnd()%0x300)-0x180,-0x100-(int)(rnd()%0x200),40+i*4);
 for(int i=0;i<(big?4:2);i++)bp_add(PK_EMBER,x+(int)(rnd()%16)-8,y,(int)(rnd()%0x100)-0x80,-0x80-(int)(rnd()%0x100),30+i*6);}
static void br_spark(int x,int y,int n){for(int i=0;i<n;i++)bp_add(PK_SPARK,x,y,(int)(rnd()%0x300)-0x180,(int)(rnd()%0x200)-0x140,8+i*2);}
static void br_parts(void){
 for(int i=0;i<NBP;i++){BrP*p=&BP[i];if(p->life<=0)continue;p->t++;if(p->t>=p->life){p->life=0;continue;}
  switch(p->k){case PK_CASE:case PK_DEB:case PK_TOSS:case PK_CORPSE:p->vy+=0x28;if(p->vy>0x400)p->vy=0x400;break;
   case PK_SMOKE:p->vx+=((brT+i)&15)==0?((int)(rnd()%3)-1)*8:0;if(p->vy>-0x60)p->vy-=2;break;
   case PK_EMBER:p->vy-=4;break;case PK_SPARK:p->vy+=0x10;break;case PK_FLARE:p->vy+=0x08;p->vx=p->vx*15/16;break;}
  s32 nx=p->x+p->vx,ny=p->y+p->vy;
  if((p->k==PK_CASE||p->k==PK_DEB||p->k==PK_TOSS||p->k==PK_CORPSE)&&p->vy>0&&br_solid(nx>>8,ny>>8)){ny=p->y;p->vy=-p->vy/3;p->vx=p->vx/2;if(p->k==PK_TOSS&&iabs(p->vy)<0x40){p->vy=0;p->vx=0;}}
  p->x=nx;p->y=ny;}}
static void br_parts_draw(int pass){
 for(int i=0;i<NBP;i++){BrP*p=&BP[i];if(p->life<=0)continue;int x=(p->x>>8)-camX,y=(p->y>>8)-camY;if(x<-40||x>280||y<-40||y>200)continue;
  int k=p->k;
  if(pass==0){ // big things behind/at vehicle level
   if(k==PK_FB){int fr=p->t*4/p->life;if(p->big)spr_aff(x,y,OT_C+BT_FB0+fr*16,2,BRP_HOT,(p->t*3)&255,0x80,1,fr>=3);else bsprx(x-16,y-16,OT_C+BT_FB0+fr*16,0,2,BRP_HOT,0,1,fr>=3);}
   else if(k==PK_TOSS){spr_aff(x,y,OT_C+BT_TOSS,1,p->big?BRP_BURNT:BRP_EN,(p->t*9)&255,0x100,1,0);} // 32x16 not square: drawn as 32x32 affine from 32x16 tiles below
   else if(k==PK_CORPSE)bsprx(x-8,y-14,OT_C+BT_DEAD,0,1,BRP_EN,p->vx<0,1,0);
   continue;}
  switch(k){
   case PK_CASE:bspr(x-4,y-4,OT_C+BT_P_CASING,0,0,BRP_HOT,(p->t>>2)&1);break;
   case PK_DEB:bspr(x-4,y-4,OT_C+((i&1)?BT_P_DEBRIS:BT_P_DEBRIS2),0,0,BRP_HOT,(p->t>>3)&1);break;
   case PK_SPARK:bspr(x-4,y-4,OT_C+BT_P_SPARK,0,0,BRP_HOT,0);break;
   case PK_DUST:bsprx(x-4,y-4-p->t/6,OT_C+BT_P_DUST,0,0,BRP_HOT,0,1,1);break;
   case PK_SMOKE:bsprx(x-8,y-8,OT_C+BT_PUFF0+((i+p->t/12)&1)*4,0,1,BRP_HOT,i&1,1,1);break;
   case PK_EMBER:if(p->t&1)bspr(x-4,y-4,OT_C+BT_P_EMBER,0,0,BRP_HOT,0);break;
   case PK_FLASH:bspr(x-4,y-4,OT_FX+FX_FLASH,0,0,3,0);break;
   case PK_FLARE:bspr(x-4,y-4,OT_C+BT_P_FLARE,0,0,BRP_HOT,0);if((p->t&3)==0)bp_add(PK_SMOKE,(p->x>>8),(p->y>>8),0,-0x20,30);break;
   case PK_APS:bspr(x-4,y-4,OT_C+BT_P_APS,0,0,BRP_HOT,0);break;
   case PK_GLINT:if((p->t>>2)&1)bspr(x-4,y-4,OT_C+BT_P_GLINT,0,0,BRP_HOT,0);break;}}}
// ---------------------------------------------------------------- spawning / life
static void br_spawn_ent(const BrEnt*d){int tier=d->y>>6,row=d->y&31;
 if(d->k==BE_BOSS){brArenaX=d->x;return;}
 if(tier>brTier)return;
 if(d->k==BE_GOAL)brGoalX=d->x;
 for(int i=0;i<NBE;i++)if(!BE[i].alive){BrE*e=&BE[i];memset(e,0,sizeof*e);e->alive=1;e->k=d->k;e->x=d->x<<8;e->y=(row*16+16)<<8;e->face=-1;
  static const u8 HP[BE_N]={3,3,5,10,24,28,60,30,3,1,1,1,12,20,24,16,20, 1,1,1,1,1,1,40,1,1,1,1,1,1};
  int h=HP[d->k];e->hp=h>2&&SET.bot>=3?BDhp(h):h;if(d->k==BE_CONVOY)e->hp=BRV_HP[0]*4*(3-brTier)/2+40;e->mhp=e->hp;e->cd=60+(i*37)%90;if(i>=nBE)nBE=i+1;
  if(d->k==BE_ORLAN||d->k==BE_FPV||d->k==BE_HELI||d->k==BE_SWARM)e->y=(row*16)<<8;
  if(br_hostile(d->k)&&d->k!=BE_CAR&&d->k!=BE_MINE&&d->k!=BE_IED)brHostile++;
  if(d->k==BE_HOUSE){brHouseX=d->x;}
  return;}}
static BrE*br_spawn_k(int k,int x,int y){BrEnt d={(u8)k,(u16)x,(u8)(y&31)};int n0=nBE;(void)n0;
 for(int i=0;i<NBE;i++)if(!BE[i].alive){br_spawn_ent(&d);return &BE[i];}return 0;}
static void br_reset_life(void){int v=BS.var;s16 lv=V.lives,th=V.thermal;memset(&V,0,sizeof V);V.lives=lv;V.thermal=th;
 V.maxhp=(BRV_HP[v]+((BS.upg&UP_BUSK)?4:0))*4;V.hp=V.maxhp;
 V.tow=BRV_TOW[v]+((BS.upg&UP_TOW)&&v!=4?2:0);V.smoke=3;V.sq=BRV_SQ[v];if(BL->obj==BOB_DISMOUNT&&V.sq<2)V.sq=2;V.face=1;V.inv=90;V.tface=1;
 V.mag[0]=30;V.mag[1]=50;V.aim=1;V.aps=(BS.upg&UP_APS)?2:0;V.towId=-1;
 V.x=((brChkX?brChkX:48))<<8;V.y=0;for(int y=0;y<PH*16;y+=4)if(br_solid(V.x>>8,y)||br_floor(V.x>>8,y,y-1)){V.y=y<<8;break;}
 memset(BB,0,sizeof BB);memset(BA,0,sizeof BA);memset(BW,0,sizeof BW);brSmokeT=0;brLaser=0;}
static BrB*bb_new(int k,int team,s32 x,s32 y,s32 vx,s32 vy,int life,int dmg){for(int i=0;i<NBB;i++)if(BB[i].life<=0){BrB*b=&BB[i];b->x=x;b->y=y;b->vx=vx;b->vy=vy;b->k=k;b->team=team;b->life=life;b->dmg=dmg;b->guided=0;b->trc=0;return b;}return 0;}
// aim at the player (or the convoy truck when it is closer)
static int br_tgt_x,br_tgt_y;
static void br_pick_tgt(int x0){br_tgt_x=V.x>>8;br_tgt_y=(V.y>>8)-12;
 if(brConvoy>=0&&BE[brConvoy].alive&&BE[brConvoy].k==BE_CONVOY){int cx=BE[brConvoy].x>>8;if(iabs(cx-x0)+30<iabs(br_tgt_x-x0)){br_tgt_x=cx;br_tgt_y=(BE[brConvoy].y>>8)-12;}}}
static void br_aim(int x0,int y0,int k,int spd,int dmg,int life){br_pick_tgt(x0);int dx=br_tgt_x-x0,dy=br_tgt_y-y0;
 if(brSmokeT>0&&iabs(dx)<200){dy-=30+(int)(rnd()%40);}  // smoke spoils aim
 int d=idist(dx,dy);if(d<1)d=1;bb_new(k,1,x0<<8,y0<<8,dx*spd/d,dy*spd/d,life,dmg);}
// damage in quarter-HP units with armour zones: front 3/4, rear 6/4, top/belly 6/4; ERA -40% vs HEAT/KE; tier scaling
static void br_hurt(int n,int kind,int sx,int top){if(V.inv>0||V.dead||brDone)return;
 int px=V.x>>8;int front=((sx-px)*V.face)>0;
 if(top)n=n*6/4;else if(front)n=n*3/4;else n=n*6/4;
 if((BS.upg&UP_ERA)&&(kind==BK_RPG||kind==BK_ATGM||kind==BK_SHELL||kind==BK_ROCKET))n=n*3/5;
 n=n*TIER_DMG[brTier]/4;if(n<1)n=1;
 V.hp-=n;V.inv=kind==BK_EBUL||kind==BK_FLAK?6:24;V.hitT=8;brNoDmg=0;V.dmg++;
 if(n>=8){sfx_hurt();shake(n>=16?8:5);br_spark(px,(V.y>>8)-14,4);}else{sfx1(0x08,0xA,1,1,900,6);}
 if(V.hp<=0){V.hp=0;V.dead=1;V.deadT=0;br_fireball(px,(V.y>>8)-14,1);bp_add(PK_TOSS,px,(V.y>>8)-24,-V.face*0x100,-0x500,90);sfx_boom();shake(14);}}
static int br_blastsnd;
static void br_crumb(int cx,int cy,int t){if(cy<0||cy>=PH)return;for(int i=0;i<24;i++)if(BCR[i].t>0&&BCR[i].cx==cx&&BCR[i].cy==cy)return;for(int i=0;i<24;i++)if(BCR[i].t<=0){BCR[i].cx=cx;BCR[i].cy=cy;BCR[i].t=t;return;}}
static void br_break(int cx,int cy){int t=ptile(cx,cy);if(t!=T_BRICK&&t!=T_DOOR)return;pm[PMI(cx,cy)]=T_AIR;brScore+=10;
 for(int i=0;i<3;i++)bp_add(PK_DEB,cx*16+4+i*4,cy*16+6,(int)(rnd()%0x200)-0x100,-0x100-(int)(rnd()%0x180),40);
 bp_add(PK_SMOKE,cx*16+8,cy*16+8,0,-0x30,40);
 if(ptile(cx,cy-1)==T_BRICK)br_crumb(cx,cy-1,8);   // unsupported masonry above collapses
}
static void br_blast(int x,int y,int r,int dmg,int team){br_fireball(x,y,r>=22);if(!br_blastsnd){sfx_boom();br_blastsnd=1;}shake(r>=22?9:5);
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||team!=0)continue;if(!br_hostile(e->k))continue;int ex=e->x>>8,ey=(e->y>>8)-8;
  if(iabs(ex-x)<r+(br_armour(e->k)?20:4)&&iabs(ey-y)<r+10){int d=dmg;if(br_armour(e->k)&&e->k!=BE_CAR&&e->k!=BE_TECH)d=d/2;e->hp-=d<1?1:d;e->c=6;}}
 if(team==1){if(iabs((V.x>>8)-x)<r+22&&iabs((V.y>>8)-12-y)<r+12)br_hurt(dmg*4,BK_ARTY,x,y<(V.y>>8)-20);
  for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(a->alive&&iabs((a->x>>8)-x)<r&&iabs((a->y>>8)-8-y)<r){a->alive=0;bp_add(PK_CORPSE,a->x>>8,a->y>>8,0x80,-0x200,40);}}
  if(brConvoy>=0&&BE[brConvoy].alive&&BE[brConvoy].k==BE_CONVOY&&iabs((BE[brConvoy].x>>8)-x)<r+28&&iabs((BE[brConvoy].y>>8)-12-y)<r+12){BE[brConvoy].hp-=dmg*2;BE[brConvoy].c=6;}}
 for(int cx=(x-r)>>4;cx<=(x+r)>>4;cx++)for(int cy=(y-r)>>4;cy<=(y+r)>>4;cy++)if(ptile(cx,cy)==T_BRICK)br_break(cx,cy);}
static void br_wreck(int x,int y,int k){BrE*w=br_spawn_k(BE_WRECK,x>>8,0);if(w){w->x=x;w->y=y;w->st=k;w->t=0;w->face=1;}}
static void br_kill(BrE*e){e->alive=0;int k=e->k;int ex=e->x>>8,ey=e->y>>8;
 if(k!=BE_CAR&&k!=BE_MINE&&k!=BE_IED)brKills++;
 if(brComboT>0){if(brCombo<8)brCombo++;}else brCombo=1;brComboT=150;
 static const u16 SC[BE_N]={100,150,250,300,500,500,1000,1200,300,80,50,50,100,600,800,600,700};
 brScore+=(k<=BE_SAM?SC[k]:50)*brCombo;
 if(brCombo>=3&&(brCombo!=e->wave)){static const char*const CT[9]={"","","","COMBO x3!","COMBO x4!","COMBO x5!","COMBO x6!","COMBO x7!","COMBO x8 - RAMPAGE!"};br_toast(CT[brCombo]);}
 if(k==BE_AIRC||k==BE_RADAR||k==BE_SAM){brObj++;br_toast(BL->obj==BOB_TARGETS&&brObj>=brObjN?"ALL TARGETS DESTROYED":"TARGET DESTROYED");}
 if(k==BE_FLEE){brObj++;br_toast(brObj>=brObjN?"COLUMN DESTROYED":"TRUCK DESTROYED");}
 int big=br_armour(k)||k==BE_HELI||k==BE_AIRC||k==BE_RADAR||k==BE_SAM;
 if(big){br_fireball(ex,ey-14,k==BE_TANK||k==BE_HELI||k==BE_AIRC);sfx_boom();shake(k==BE_TANK?10:6);
  if(k==BE_TANK||k==BE_BMP2||k==BE_BTR){BrP*p=bp_add(PK_TOSS,ex,ey-22,(int)(rnd()%0x200)-0x100,-0x600,120);if(p)p->big=1;}
  if(k!=BE_TECH&&k!=BE_HELI)br_wreck(e->x,e->y,k);
  if(k==BE_CAR)br_blast(ex,ey-8,26,6,0);
  if(k==BE_HELI){BrE*w=br_spawn_k(BE_WRECK,ex,0);if(w){w->x=e->x;w->y=br_ground(ex,ey)<<8;w->st=k;}}}
 else if(k==BE_FPV||k==BE_ORLAN){br_fireball(ex,ey,0);}
 else if(k<=BE_ATGM){bp_add(PK_CORPSE,ex,ey,e->face*-0x80,-0x200,50);br_spark(ex,ey-8,2);sfx_shot(1);}}
static int ent_w(int k){switch(k){case BE_BTR:case BE_BMP2:case BE_TANK:case BE_AIRC:case BE_FLEE:case BE_CONVOY:return 28;case BE_HELI:return 26;case BE_TECH:case BE_CAR:return 14;case BE_RADAR:case BE_SAM:return 14;case BE_ATGM:return 12;}return 7;}
static int ent_h(int k){switch(k){case BE_BTR:case BE_BMP2:case BE_TANK:case BE_FLEE:case BE_CONVOY:return 24;case BE_AIRC:return 18;case BE_HELI:return 14;case BE_TECH:case BE_CAR:return 13;case BE_RADAR:case BE_SAM:return 26;case BE_MINE:case BE_IED:return 6;case BE_ATGM:return 12;case BE_ORLAN:case BE_FPV:return 8;}return 15;}
static int ent_air(int k){return k==BE_HELI||k==BE_ORLAN||k==BE_FPV;}
// ---------------------------------------------------------------- player vehicle
static int br_pivx(void){return (V.x>>8)+V.tface*13;}
static int br_pivy(void){return (V.y>>8)-24+(V.water?3:0);}
static void br_vehicle(void){int v=BS.var;if(V.inv>0)V.inv--;if(V.cd>0)V.cd--;if(V.mgcd>0)V.mgcd--;if(V.towcd>0)V.towcd--;if(V.hitT>0)V.hitT--;if(V.recoil>0)V.recoil--;if(V.flash>0)V.flash--;
 if(V.heat>0)V.heat--;if(V.ovh>0){V.ovh--;if(V.ovh==0)V.heat=40;}
 if(V.rel>0){V.rel--;if(V.rel==0){V.mag[V.ammo]=V.ammo?50:30;sfx_menu();}}
 if(V.aps<2&&(BS.upg&UP_APS)){if(++V.apsT>=600){V.apsT=0;V.aps++;}}
 if(V.dead){V.deadT++;return;}
 int px=V.x>>8,py=V.y>>8;
 int wat=(TFL[ptile(px>>4,(py-6)>>4)]&F_WAT)!=0;V.water=wat;
 int max=BRV_SPD[v];if(wat)max=max*11/20;
 int dir=(keys&K_RIGHT)?1:(keys&K_LEFT)?-1:0;
 if(dir){V.vx+=dir*0x18;if(V.vx>max)V.vx=max;if(V.vx<-max)V.vx=-max;V.face=dir;}
 else{if(V.vx>0){V.vx-=0x14;if(V.vx<0)V.vx=0;}else if(V.vx<0){V.vx+=0x14;if(V.vx>0)V.vx=0;}}
 if(!(keys&K_A)||dir==V.tface)V.tface=V.face;      // hold A: turret stays on target while hull reverses
 V.anim+=iabs(V.vx)>>5;
 // turret elevation: UP/DOWN (not while guiding a TOW)
 int guiding=(keys&K_R)&&V.towId>=0&&BB[V.towId].life>0&&BB[V.towId].k==BK_TOW&&BB[V.towId].guided;
 if(V.aimCd>0)V.aimCd--;
 if(!guiding&&!(keys&K_SEL)&&V.aimCd<=0){int sp=(BS.upg&UP_TURRET)?4:6;if((keys&K_UP)&&V.aim<7){V.aim++;V.aimCd=sp;}else if((keys&K_DOWN)&&V.aim>0){V.aim--;V.aimCd=sp;}}
 if((pressed&K_B)&&V.on){V.vy=wat?-0x400:-0x4C0;V.on=0;sfx_whoosh();}
 // horizontal move with 1-tile auto-climb
 s32 nx=V.x+V.vx;int fx=(nx>>8)+(V.vx>0?26:-26);
 if(V.vx){int hitLow=br_solid(fx,py-4),hitMid=br_solid(fx,py-14)||br_solid(fx,py-22);
  if(hitLow&&!hitMid&&V.on){int top=((py-4)>>4)*16;if(!br_solid(fx,top-8)&&!br_solid(fx,top-20)){V.y=top<<8;nx=V.x+V.vx/2;}else nx=V.x;}
  else if(hitLow||hitMid){nx=V.x;V.vx=0;}}
 if((nx>>8)<20)nx=20<<8;if((nx>>8)>pW*16-20)nx=(pW*16-20)<<8;
 if(brArenaX>=0&&BO.on&&!BO.dead){int lo=brArenaX+20,hi=brArenaX+brArenaW-20;if((nx>>8)<lo)nx=lo<<8;if((nx>>8)>hi)nx=hi<<8;}
 if(brLock){if((nx>>8)<brLockL)nx=brLockL<<8;if((nx>>8)>brLockR)nx=brLockR<<8;}
 if(BO.on&&!BO.dead&&BO.kind!=1){int bx=BO.x>>8,half=BO.kind==4?100:44;if(iabs((nx>>8)-bx)<half+12){nx=(bx+((nx>>8)<bx?-(half+12):(half+12)))<<8;V.vx=0;}}
 if(brGoalX>0&&!brObjOk&&(nx>>8)>brGoalX-30){nx=(brGoalX-30)<<8;V.vx=0;if((brT&63)==0)br_toast(BL->obj==BOB_ESCORT?"WAIT FOR THE CONVOY":BL->obj==BOB_TARGETS?"TARGETS REMAIN - LOOK BACK":"OBJECTIVE INCOMPLETE");}
 V.x=nx;px=V.x>>8;
 // vertical
 V.vy+=wat?0x28:0x40;if(V.vy>0x600)V.vy=0x600;s32 oy=V.y;V.y+=V.vy;py=V.y>>8;int opy=oy>>8;int wasOn=V.on;s32 fvy=V.vy;
 V.on=0;if(V.vy>=0){for(int k=-20;k<=20;k+=20){if(br_floor(px+k,py,opy)){int top=(py>>4)*16;V.y=top<<8;V.vy=0;V.on=1;
    int t=ptile((px+k)>>4,py>>4);if(t==T_CRUMB)br_crumb((px+k)>>4,py>>4,22);}}}
 else if(br_solid(px,py-26)){V.vy=0;V.y=((((py-26)>>4)+1)*16+26)<<8;}
 if(V.on&&!wasOn&&fvy>0x300){V.land=6;shake(3);for(int i=0;i<4;i++)bp_add(PK_DUST,px-20+i*13,py-2,(i-2)*0x30,-0x30,24);}
 if(V.land>0)V.land--;
 if(V.on&&iabs(V.vx)>0x100&&(brT&3)==0&&!wat&&!(BL->fl&FL_SNOW&&0))bp_add(PK_DUST,px-V.face*28,py-3,-V.face*0x40,-0x18,22);
 if(py>PH*16+24){V.hp=0;V.dead=1;V.deadT=40;sfx_hurt();}
 // damage states
 if(V.hp*2<V.maxhp&&(brT&7)==0)bp_add(PK_SMOKE,px-V.face*6,py-26,-V.face*0x20,-0x50,50);
 if(V.hp*4<V.maxhp&&(brT&3)==0)bp_add(PK_EMBER,px-V.face*10+(int)(rnd()%10),py-18,0,-0x80,24);
 // ---- weapons
 int a=BR_ELEV[V.aim];int dx=COS(a),dy=-SIN(a);int f=V.tface;
 int mx=br_pivx()+f*dx*15/256,my=br_pivy()+dy*15/256;
 // SELECT: tap = AP/HE, hold = dismount, +DOWN = thermal
 if(keys&K_SEL){V.selT++;if((pressed&K_DOWN)&&(BL->fl&FL_NIGHT)){V.thermal^=1;br_palettes();sfx_menu();br_toast(V.thermal?"THERMAL SIGHT ON":"THERMAL OFF");V.selT=99;}
  if(V.selT==30){if(V.sq>0){V.sq--;for(int i=0;i<3;i++){BrAlly*al=&BA[i];al->alive=1;al->x=V.x-(i*10<<8);al->y=V.y;al->hp=3;al->t=600;al->cd=20+i*7;al->inHouse=0;}br_toast("DISMOUNT! SQUAD OUT");sfx_ok();}else{br_toast("NO SQUAD LEFT");}}}
 else{if(V.selT>0&&V.selT<16){V.ammo^=1;if(V.mag[V.ammo]<=0&&V.rel<=0)V.rel=90;br_toast(V.ammo?"HE AIRBURST - INFANTRY/DRONES":"AP - ARMOUR PIERCING");sfx_menu();}V.selT=0;}
 if((keys&K_A)&&V.cd<=0&&V.ovh<=0&&V.rel<=0){
  if(V.mag[V.ammo]<=0){V.rel=90;br_toast("RELOADING");}
  else{int rate=BRV_RATE[v];if(BS.upg&UP_TURRET)rate=rate*7/10;V.cd=V.ammo?rate+2:rate;V.mag[V.ammo]--;
   int spd=V.ammo?0x480:0x700;int jit=((int)(rnd()%5)-2)*10;
   BrB*b=bb_new(V.ammo?BK_HE:BK_AP,0,mx<<8,my<<8,f*dx*spd/256,dy*spd/256+jit,V.ammo?70:60,V.ammo?2:4);
   if(b){b->trc=(V.mag[V.ammo]%3)==0;}
   V.heat+=7;if(V.heat>=100){V.ovh=120;br_toast("GUN OVERHEAT!");sfx_hurt();}
   V.recoil=3;V.flash=3;sfx_shot(0);brFire=3;
   bp_add(PK_CASE,br_pivx()-f*4,br_pivy()+2,-f*(0x80+(int)(rnd()%0x80)),-0x180-(int)(rnd()%0x100),50);
   if(V.mag[V.ammo]<=0)V.rel=90;}}
 if((keys&K_L)&&!(keys&K_R)&&V.mgcd<=0){V.mgcd=4;int jit=((int)(rnd()%7)-3)*12;bb_new(BK_MG,0,(br_pivx()+f*6)<<8,(br_pivy()+1)<<8,f*dx*0x600/256,dy*0x600/256+jit,50,1);if((brT&7)==0)sfx_shot(1);}
 if(((pressed&K_L)&&(keys&K_R))||((pressed&K_R)&&(keys&K_L))){
  if(V.smoke>0&&brSmokeT<=0){V.smoke--;brSmokeT=300;sfx_whoosh();br_toast("SMOKE! LASERS AND ATGMS LOSE LOCK");brLaser=0;
   for(int i=0;i<8;i++)bp_add(PK_SMOKE,px+V.face*(10+i*8),py-20-(i&3)*6,V.face*(0x40+i*0x10),-0x30,200+i*8);}
  return;}
 if(pressed&K_R){if(v==4){if(V.tow>0&&V.towcd<=0){int best=-1,bd=999;for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||!br_hostile(e->k)||e->k==BE_MINE||e->k==BE_IED)continue;int ddx=((e->x>>8)-px)*V.face;if(ddx>0&&ddx<220&&ddx<bd){bd=ddx;best=i;}}
    int tx=best>=0?(BE[best].x>>8):(BO.on&&!BO.dead?(BO.x>>8):px+V.face*120);for(int i=0;i<8;i++)if(BW[i].t<=0){BW[i].x=tx;BW[i].t=60;BW[i].team=0;break;}V.tow--;V.towcd=60;br_toast("FIRE MISSION - SHOT, OVER");sfx_ok();}}
  else if(V.tow>0&&V.towcd<=0&&(V.towId<0||BB[V.towId].life<=0||BB[V.towId].k!=BK_TOW)){BrB*b=bb_new(BK_TOW,0,(px-V.tface*6)<<8,(py-30)<<8,V.tface*0x200,-dy*0/256-(V.aim>1?0x100:0),170,40);
   if(b){b->guided=1;V.towId=b-BB;V.tow--;V.towcd=60;sfx_whoosh();for(int i=0;i<3;i++)bp_add(PK_SMOKE,px-V.tface*14,py-30,-V.tface*0x60,-0x20,30);}}}
 if(!(keys&K_R)&&V.towId>=0&&BB[V.towId].k==BK_TOW&&BB[V.towId].guided){BB[V.towId].guided=0;}  // wire cut
}
// ---------------------------------------------------------------- projectiles
static int brb_hit(int x,int y,BrB*b);
static void br_hit_ent(BrE*e,BrB*b,int x,int y){int k=e->k,d=b->dmg;
 if(k==BE_MINE||k==BE_IED){e->alive=0;br_blast(e->x>>8,(e->y>>8)-4,18,2,0);brScore+=50;return;}
 if(b->k==BK_MG&&br_armour(k)&&k!=BE_TECH&&k!=BE_CAR){br_spark(x,y,1);return;}
 if(b->k==BK_AP&&br_armour(k)){int front=(b->vx>0)==(e->face<0);if(k==BE_TANK&&front)d=1;else if(front&&(k==BE_BMP2||k==BE_BTR))d=3;br_spark(x,y,front&&k==BE_TANK?3:2);}
 if(b->k==BK_HE&&br_armour(k)&&k!=BE_TECH&&k!=BE_CAR)d=(brT&1);
 if(b->k==BK_HE&&ent_air(k))d*=2;
 if(b->k==BK_AP&&!br_armour(k)&&k!=BE_HELI&&k!=BE_AIRC&&k!=BE_RADAR&&k!=BE_SAM)d=k<=BE_ATGM?(e->t&1)+1:2;   // AP over-penetrates soft targets: use HE
 if(b->k==BK_AP&&k==BE_HELI)d=3;
 e->hp-=d;e->c=6;if(e->hp<=0&&e->alive)br_kill(e);}
static void br_airburst(int x,int y){br_spark(x,y,3);bp_add(PK_FB,x,y,0,0,12);
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||!br_hostile(e->k))continue;if(br_armour(e->k)&&e->k!=BE_TECH)continue;int ex=e->x>>8,ey=(e->y>>8)-(ent_air(e->k)?0:8);
  if(iabs(ex-x)<20&&iabs(ey-y)<20){e->hp-=e->k==BE_HELI?2:3;e->c=6;if(e->hp<=0)br_kill(e);}}}
static void br_bullets(void){br_blastsnd=0;int vx0=V.x>>8,vy0=V.y>>8;
 for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life<=0)continue;b->life--;
  if(b->k==BK_TOW){if(b->guided){int sp=b->vx>0?b->vx:-b->vx;if(sp<0x480){sp+=0x10;b->vx=b->vx>0?sp:-sp;}if(keys&K_UP)b->vy-=0x1C;if(keys&K_DOWN)b->vy+=0x1C;}
   else b->vy=b->vy*15/16;
   if(b->vy>0x280)b->vy=0x280;if(b->vy<-0x300)b->vy=-0x300;
   if((brT&1)==0)bp_add(PK_SMOKE,(b->x>>8)-(b->vx>0?8:-8),b->y>>8,0,-0x10,26);}
  if(b->k==BK_ATGM){if(brSmokeT<=0){int ty=((vy0)-12)<<8;b->vy+=(ty>b->y)?0x0C:-0x0C;if(b->vy>0x120)b->vy=0x120;if(b->vy<-0x120)b->vy=-0x120;}if((brT&3)==0)bp_add(PK_SMOKE,b->x>>8,b->y>>8,0,-0x10,20);}
  if(b->k==BK_RPG&&(brT&3)==0)bp_add(PK_SMOKE,b->x>>8,b->y>>8,0,-0x10,16);
  if(b->k==BK_ARTY||b->k==BK_EHE)b->vy+=0x20;
  if(b->k==BK_HE)b->vy+=0x06;
  b->x+=b->vx;b->y+=b->vy;int x=b->x>>8,y=b->y>>8;
  if(br_solid(x,y)){int t=ptile(x>>4,y>>4);
   if(t==T_BRICK&&b->team==0&&b->k!=BK_MG)br_break(x>>4,y>>4);
   if(b->k==BK_TOW)br_blast(x,y,24,8,0);
   else if(b->k==BK_ARTY||b->k==BK_EHE||b->k==BK_RPG||b->k==BK_SHELL||b->k==BK_ATGM||b->k==BK_ROCKET)br_blast(x,y,16,b->k==BK_ARTY&&b->team==0?6:3,b->team);
   else if(b->k==BK_HE)br_airburst(x,y-4);
   else{br_spark(x,y,b->k==BK_AP?2:1);if(b->k==BK_AP&&(i&1))bp_add(PK_DEB,x,y-2,-(b->vx>>3),-0x100,24);}
   b->life=0;continue;}
  if(x<camX-60||x>camX+300||y<-80||y>PH*16+20){b->life=0;continue;}
  if(b->team==0){
   if(brb_hit(x,y,b)){b->life=0;continue;}
   for(int j=0;j<nBE;j++){BrE*e=&BE[j];if(!e->alive||!br_hostile(e->k))continue;int ex=e->x>>8,ey=e->y>>8;int hw=ent_w(e->k),hh=ent_h(e->k);
    int air=ent_air(e->k);int top=air?ey-hh:ey-hh,bot=air?ey+hh:ey;
    if(e->k==BE_MINE&&b->k!=BK_HE)continue;
    if(b->k==BK_HE&&(air||e->k<=BE_ATGM)&&iabs(x-ex)<hw+16&&y>top-18&&y<bot+10){br_airburst(x,y);b->life=0;break;}
    if(iabs(x-ex)<hw+2&&y<bot+2&&y>top-2){
     if(b->k==BK_TOW){e->hp-=b->dmg;e->c=8;if(e->hp<=0&&e->alive)br_kill(e);br_blast(x,y,24,8,0);}
     else br_hit_ent(e,b,x,y);
     b->life=0;break;}}}
  else{
   // Iron Fist APS: hard-kill incoming RPG / ATGM
   if((b->k==BK_RPG||b->k==BK_ATGM||b->k==BK_ROCKET)&&V.aps>0&&!V.dead&&iabs(x-vx0)<44&&iabs(y-(vy0-14))<36){V.aps--;V.apsT=0;b->life=0;bp_add(PK_APS,x,y,0,0,8);br_spark(x,y,4);bp_add(PK_FB,x,y,0,0,14);sfx_shot(1);br_toast("IRON FIST INTERCEPT!");continue;}
   if(b->k==BK_FLAK&&iabs(x-vx0)<24&&iabs(y-(vy0-34))<18){br_blast(x,y,14,2,1);b->life=0;continue;}
   for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(a->alive&&!a->inHouse&&iabs(x-(a->x>>8))<5&&y>(a->y>>8)-14&&y<(a->y>>8)){a->hp-=b->dmg>4?3:1;b->life=0;if(a->hp<=0){a->alive=0;bp_add(PK_CORPSE,a->x>>8,a->y>>8,0x80,-0x200,40);}}}
   if(b->life>0&&brConvoy>=0&&BE[brConvoy].alive&&BE[brConvoy].k==BE_CONVOY){BrE*c=&BE[brConvoy];int cx=c->x>>8,cy=c->y>>8;if(iabs(x-cx)<28&&y>cy-26&&y<cy){c->hp-=b->dmg;c->c=6;b->life=0;br_spark(x,y,2);if(b->dmg>=8)br_fireball(x,y,0);}}
   if(b->life>0&&!V.dead&&iabs(x-vx0)<27&&y>vy0-26&&y<vy0+2){
    if(b->k==BK_ARTY||b->k==BK_EHE)br_blast(x,y,16,3,1);
    else{static const u8 DQ[13]={0,0,0,0,0,3,14,18,20,14,14,12,8};int top=b->vy>0x100;br_hurt(DQ[b->k]?DQ[b->k]:2,b->k,x,top);if(b->k>=BK_RPG)br_fireball(x,y,0);else br_spark(x,y,2);}
    b->life=0;}}}
 // artillery warnings (Orlan spotting / M7 fire missions / train rockets)
 for(int i=0;i<8;i++)if(BW[i].t>0){BW[i].t--;if(BW[i].t==0){int own=BW[i].team==0&&BS.var==4;for(int k=0;k<3;k++)bb_new(BK_ARTY,own?0:1,(BW[i].x+(k-1)*18)<<8,(camY-20-k*30)<<8,0,0x200,120,own?8:3);}}
 for(int i=0;i<24;i++)if(BCR[i].t>0&&--BCR[i].t==0){int cx=BCR[i].cx,cy=BCR[i].cy;int t=ptile(cx,cy);if(t==T_BRICK)br_break(cx,cy);else if(t==T_CRUMB){pm[PMI(cx,cy)]=T_AIR;bp_add(PK_SMOKE,cx*16+8,cy*16+8,0,-0x30,30);}}}
// ---------------------------------------------------------------- enemies / objectives
static void br_ent_ground(BrE*e){int ex=e->x>>8,ey=e->y>>8;
 if(br_floor(ex,ey,ey-1)||br_solid(ex,ey-1)){while(ey>0&&br_solid(ex,ey-1))ey--;e->y=ey<<8;e->vy=0;e->fall=0;}
 else{e->vy+=0x30;if(e->vy>0x500)e->vy=0x500;s32 ny=e->y+e->vy;int nyy=ny>>8;if(br_floor(ex,nyy,ey)){e->y=((nyy>>4)*16)<<8;if(e->fall&&e->vy>0x300&&e->k<=BE_ATGM){e->hp=0;}e->vy=0;e->fall=0;}else{e->y=ny;e->fall=1;}
  if((e->y>>8)>PH*16+20)e->alive=0;}}
static void br_laser_fire(BrE*e,int ex,int ey,int dmg){bb_new(BK_ATGM,1,(ex+e->face*10)<<8,(ey)<<8,e->face*0x1A0,0,220,dmg);sfx_whoosh();}
static void br_wave_spawn(void){int k=brWave;int bx=camX+200;
 for(int i=0;i<2+k+brTier;i++){BrE*e=br_spawn_k(i%3==2?BE_RPG:BE_INF,bx+i*8,0);if(e){e->y=br_ground(bx+i*8,0)<<8;e->wave=1;}}
 if(k>=1){BrE*e=br_spawn_k(k==2?BE_TANK:BE_BMP2,bx+30,0);if(e){e->y=br_ground(bx+30,0)<<8;e->wave=1;}}
 if(k>=1){BrE*e=br_spawn_k(BE_INF,camX+12,0);if(e){e->y=br_ground(camX+12,0)<<8;e->wave=1;}}
 if(k>=2)for(int i=0;i<3;i++){BrE*e=br_spawn_k(BE_FPV,bx+i*20,2);if(e)e->wave=1;}
 static const char*const WT[3]={"WAVE 1 - INFANTRY ASSAULT","WAVE 2 - MECHANISED","WAVE 3 - ARMOUR + DRONES"};br_toast(WT[k<3?k:2]);}
static void br_ents(void){int px=V.x>>8,py=V.y>>8;
 while(brEi<BL->ne&&BL->e[brEi].x<camX+300){br_spawn_ent(&BL->e[brEi]);brEi++;}
 brLaser=0;int waveAlive=0;
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive)continue;int ex=e->x>>8,ey=e->y>>8;int dx=px-ex,adx=iabs(dx);
  if(e->c>0)e->c--;
  if(ex<camX-160&&e->k!=BE_CONVOY&&e->k!=BE_FLEE&&!e->wave&&e->k!=BE_HOUSE&&!(BL->obj==BOB_TARGETS&&(e->k==BE_AIRC||e->k==BE_RADAR||e->k==BE_SAM))){e->alive=0;continue;}
  e->t++;if(e->cd>0)e->cd--;
  int vis=ex<camX+250&&ex>camX-10;if(e->k<=BE_SAM&&e->k!=BE_FLEE)e->face=dx<0?-1:1;
  if(e->wave){waveAlive++;if(brLock){int lo=(brLockL-10)<<8,hi=(camX+230)<<8;if(e->x<lo)e->x=lo;if(e->x>hi)e->x-=0x100;if(brWaveT<-1500)e->alive=0;}}
  switch(e->k){
  case BE_INF:case BE_RPG:
   br_ent_ground(e);if(e->hp<=0){br_kill(e);break;}
   if(adx<30&&iabs(py-ey)<20&&iabs(V.vx)>0x60&&!V.dead){br_kill(e);br_toast("OVERRUN!");break;}
   if(e->k<=BE_RPG&&(vis||e->wave)&&adx>(e->wave?70:120)&&!e->fall&&((e->t>>5)&1)&&e->st==0)e->x+=dx<0?-0x40:0x40;
   if(vis&&!V.dead&&adx<230){
    if(e->k==BE_INF){if(e->cd<=0){br_aim(ex,ey-10,BK_EBUL,0x220,1,120);e->st++;if(e->st>=3){e->st=0;e->cd=BDcd(80+(i*13)%40);}else e->cd=8;}}
    else{ if(e->st==0&&e->cd<=0&&brSmokeT<=0){e->st=1;e->st2=TIER_RPG[brTier];}
     if(e->st==1&&--e->st2<=0){br_aim(ex+e->face*8,ey-12,BK_RPG,0x260,12,140);e->st=0;e->cd=BDcd(150);sfx_whoosh();bp_add(PK_SMOKE,ex-e->face*8,ey-12,-e->face*0x60,-0x20,24);}}}
   break;
  case BE_ATGM:
   if(e->hp<=0){br_kill(e);break;}
   if(vis&&!V.dead&&adx<240&&adx>40&&e->cd<=0){
    if(brSmokeT>0){e->st=0;e->a=0;}
    else{e->st=1;e->a++;brLaser=1;if(brLaserT<e->a)brLaserT=e->a;if((e->a&15)==1)sfx1(0x08,0xC,1,1,1950,6);
     if(e->a>=TIER_LOCK[brTier]){br_laser_fire(e,ex,ey-12,16);e->a=0;e->st=0;e->cd=BDcd(200);}}}
   else{e->st=0;e->a=0;}
   break;
  case BE_TECH:case BE_BTR:case BE_BMP2:{int stop=e->k==BE_TECH?90:140;
   if(e->hp<=0){br_kill(e);break;}
   if(vis&&adx>stop){int sp=e->k==BE_TECH?0x90:0x50;e->x+=dx<0?-sp:sp;}
   if(e->k==BE_TECH&&adx<40&&iabs(py-ey)<20&&iabs(V.vx)>0x80&&!V.dead){br_kill(e);br_toast("RAMMED!");break;}
   if(e->k==BE_BTR&&e->hp*2<e->mhp&&!e->st2){e->st2=1;for(int k=0;k<2;k++){BrE*s=br_spawn_k(BE_INF,ex-e->face*20-k*10,0);if(s)s->y=e->y;}br_toast("BTR DISMOUNTS TROOPS");}
   if(vis&&e->cd<=0&&!V.dead&&adx<230){
    if(e->k==BE_BMP2&&(e->t/240)&1&&brSmokeT<=0){e->a++;brLaser=1;if(brLaserT<e->a)brLaserT=e->a;if(e->a>=TIER_LOCK[brTier]){br_laser_fire(e,ex,ey-24,16);e->a=0;e->cd=BDcd(200);}}
    else{br_aim(ex+e->face*(e->k==BE_TECH?6:28),ey-(e->k==BE_TECH?13:22),BK_EBUL,0x2A0,e->k==BE_TECH?1:2,100);e->st++;if(e->st>=(e->k==BE_TECH?4:3)){e->st=0;e->cd=BDcd(e->k==BE_TECH?70:100);}else e->cd=6;if((e->st&1))sfx_shot(1);}}
   e->y=br_ground(ex,ey-8)<<8;break;}
  case BE_TANK:{
   if(e->hp<=0){br_kill(e);break;}
   int want=150+(i&1)*30;
   if(vis&&e->st==0){if(adx>want+10)e->x+=dx<0?-0x60:0x60;else if(adx<want-30)e->x+=dx<0?0x40:-0x40;}
   if(vis&&!V.dead&&adx<260){
    if(e->st==0&&e->cd<=0){e->st=1;e->st2=TIER_TANK[brTier];bp_add(PK_GLINT,ex+e->face*30,ey-20,0,0,e->st2);}
    else if(e->st==1&&--e->st2<=0){int sy=ey-19;int yoff=brSmokeT>0?-(26+(int)(rnd()%20)):0;
     bb_new(BK_SHELL,1,(ex+e->face*32)<<8,(sy)<<8,e->face*0x600,yoff*8,120,16);bp_add(PK_FLASH,ex+e->face*34,sy,0,0,4);br_fireball(ex+e->face*36,sy,0);
     sfx_boom();shake(4);e->st=0;e->cd=BDcd(170);e->x-=e->face*0x200;}}
   e->y=br_ground(e->x>>8,ey-8)<<8;break;}
  case BE_HELI:{ // strafing runs
   if(e->hp<=0){br_kill(e);break;}
   if(e->st==0){e->st=1;e->a=0;e->vx=-0x1C0;e->x=(camX+280)<<8;}
   int alt=(40+((e->t>>2)&7))<<8;if(e->st2&1)alt=(62)<<8;
   e->y+=(alt-e->y)/16;e->x+=e->vx;e->face=e->vx<0?-1:1;
   if(!V.dead&&adx<90&&e->cd<=0&&((dx<0)==(e->vx<0))){bb_new(BK_EBUL,1,(ex+e->face*20)<<8,(ey+6)<<8,e->face*0x200,0x200,90,3);e->cd=5;if(++e->a%6==0){bb_new(BK_ROCKET,1,(ex)<<8,(ey+8)<<8,e->face*0x300,0x180,100,10);e->cd=20;}}
   if((e->vx<0&&ex<camX-50)||(e->vx>0&&ex>camX+290)){e->vx=-e->vx;e->st2++;if(e->st2>=6){e->st2=0;}}
   break;}
  case BE_ORLAN:{if(e->hp<=0){br_kill(e);break;}int ty=(18+((e->t>>5)&7))<<8;e->y+=(ty-e->y)/16;if(vis){int sp=0x80;if(dx>20)e->x+=sp;else if(dx<-20)e->x-=sp;}
   if(vis&&adx<40&&e->cd<=0&&!V.dead){for(int k=0;k<8;k++)if(BW[k].t<=0){BW[k].x=px+(V.vx>>4);BW[k].t=BDcd(90);BW[k].team=1;break;}e->cd=BDcd(260);br_toast("ORLAN SPOTTED YOU - MOVE!");}
   break;}
  case BE_FPV:if(e->hp<=0){br_kill(e);break;}
   if(vis&&adx<190&&!V.dead){int ddy=(py-14)-ey;int d=idist(dx,ddy);if(d<1)d=1;int sp=0x150+brTier*0x30;e->x+=dx*sp/d;e->y+=ddy*sp/d+(((e->t>>3)&1)?0x30:-0x30);
    if(adx<24&&iabs(ddy)<16){e->alive=0;br_fireball(ex,ey,0);sfx_boom();br_hurt(12,BK_ROCKET,ex,1);}}
   else if(!vis)e->x-=0x80;
   break;
  case BE_SWARM:e->alive=0;for(int k=0;k<5-(brTier==0?2:0);k++){BrE*f=br_spawn_k(BE_FPV,camX+250+k*22,1+(k&1));if(f){f->y=(20+k*9)<<8;}}br_toast("FPV SWARM! SWITCH TO HE AIRBURST");break;
  case BE_MINE:case BE_IED:{int r=e->k==BE_IED?22:24;if(!V.dead&&adx<r&&iabs(py-ey)<10){e->alive=0;int d=e->k==BE_IED?24:16;if(BS.upg&UP_BUSK)d/=2;br_blast(ex,ey-4,10,0,0);br_hurt(d,BK_ARTY,ex,1);br_toast(e->k==BE_IED?"IED! BELLY HIT":"MINE STRIKE!");}}break;
  case BE_CAR:if(e->hp<=0){br_kill(e);}break;
  case BE_FLEE:{if(e->hp<=0){br_kill(e);break;}
   int sp=adx<200||ex<camX+240?0x150:0x60;if(ex<px+60)sp=0x60;e->x+=sp;e->face=1;e->y=br_ground(e->x>>8,ey-8)<<8;
   if((e->t&63)==0&&adx<200)br_aim(ex-20,ey-20,BK_EBUL,0x240,2,100);
   if(brGoalX>0&&ex>brGoalX-20){brFail=2;e->alive=0;}
   break;}
  case BE_AIRC:case BE_RADAR:if(e->hp<=0)br_kill(e);break;
  case BE_SAM:if(e->hp<=0){br_kill(e);break;}
   if(vis&&(e->t&127)==0)bp_add(PK_SMOKE,ex+8,ey-28,0,-0x30,40);  // SAM sites are anti-air: no ground fire
   break;
  case BE_CONVOY:{
   if(e->hp<=0){e->alive=0;br_fireball(ex,ey-14,1);sfx_boom();shake(10);brFail=1;break;}
   int blocked=0;for(int j=0;j<nBE;j++){BrE*o=&BE[j];if(o->alive&&br_hostile(o->k)&&o->k!=BE_CAR&&o->k!=BE_MINE&&o->k!=BE_IED&&o->k!=BE_AIRC&&o->k!=BE_RADAR&&!ent_air(o->k)){int od=(o->x>>8)-ex;if(od>0&&od<110){blocked=1;break;}}}
   int go=!blocked&&(px>ex+50||(brGoalX>0&&px>brGoalX-60));
   if(go){e->x+=0xC0;e->st++;}
   if(brGoalX>0&&ex>=brGoalX-40){brObjOk=1;}
   e->y=br_ground(e->x>>8,ey-8)<<8;e->face=1;break;}
  case BE_HOUSE:break;
  case BE_AMBUSH:if(px>ex-20){e->alive=0;for(int k=0;k<2+brTier;k++){BrE*s=br_spawn_k(k==1?BE_RPG:BE_INF,camX-10-k*12,0);if(s){s->y=br_ground(camX-10-k*12,40)<<8;}}
    {BrE*s=br_spawn_k(BE_RPG,camX+232,0);if(s)s->y=br_ground(camX+232,20)<<8;}br_toast("AMBUSH! CHECK YOUR SIX");sfx_hurt();}break;
  case BE_WAVE:if(px>ex&&!brLock&&brWave==0){brLock=1;brLockL=ex-60;brLockR=ex+150;brWaveT=60;e->st=1;br_toast("HOLD THE CROSSROADS!");}
   e->alive=brWave<brObjN||brLock;break;
  case BE_FIRE:if((e->t&7)==0)bp_add(PK_SMOKE,ex+(int)(rnd()%8)-4,ey-20,0x10,-0x60,90);if((e->t&3)==0)bp_add(PK_EMBER,ex+(int)(rnd()%12)-6,ey-8,0,-0xC0,22);
   if(!V.dead&&adx<16&&iabs(py-ey)<20&&(brT&15)==0)br_hurt(2,BK_EBUL,ex,1);break;
  case BE_WRECK:if(e->t<900&&(e->t&15)==0)bp_add(PK_SMOKE,ex+(int)(rnd()%10)-5,ey-24,0x18,-0x50,80);if(e->t<300&&(e->t&5)==0)bp_add(PK_EMBER,ex+(int)(rnd()%20)-10,ey-14,0,-0x90,20);
   if(ex<camX-120)e->alive=0;break;
  default:{int hit=!V.dead&&adx<28&&py>ey-24&&py<ey+20;
   if(e->k==BE_GOAL){if(!V.dead&&dx>-8&&iabs(py-ey)<48&&!(BO.on&&!BO.dead)&&brObjOk&&!(brArenaX>=0&&!BO.dead)){brDone=1;e->alive=0;sfx_ok();}}
   else if(hit){e->alive=0;sfx_ok();
    if(e->k==BE_TOWC){V.tow+=2;br_toast("+2 TOW");}
    else if(e->k==BE_REP){static const u8 RP[3]={10,6,3};int r=V.maxhp*RP[brTier]/20;V.hp=V.hp+r>V.maxhp?V.maxhp:V.hp+r;br_toast("FIELD REPAIR");}
    else if(e->k==BE_INTEL){brMedal|=1<<(__builtin_popcount(brMedal));brScore+=1000;br_toast("INTEL RECOVERED!");}
    else if(e->k==BE_CHECK){brChkX=ex;static const u8 CR[3]={4,2,0};int r=V.maxhp*CR[brTier]/16;if(r){V.hp=V.hp+r>V.maxhp?V.maxhp:V.hp+r;}V.smoke=V.smoke<3?V.smoke+1:3;br_toast(r?"CHECKPOINT - PATCHED UP":"CHECKPOINT");}}}}}
 // objectives
 if(BL->obj==BOB_REACH)brObjOk=1;
 else if(BL->obj==BOB_TARGETS||BL->obj==BOB_CHASE){if(brObj>=brObjN)brObjOk=1;}
 if(brLock){if(brWaveT>0){if(--brWaveT==0){br_wave_spawn();V.tow++;brWaveT=-1;}}
  else if(waveAlive==0){brWave++;if(brWave>=brObjN){brLock=0;brObjOk=1;br_toast("POSITION HELD - MOVE OUT");sfx_ok();}else brWaveT=120;}else brWaveT--;}
 brWaveLeft=waveAlive;
 if(brComboT>0&&--brComboT==0)brCombo=0;
 // dismounted squad
 for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(!a->alive)continue;int ax=a->x>>8;
  if(BL->obj==BOB_DISMOUNT&&!brObjOk&&brHouseX>0&&iabs(ax-brHouseX)<60){
   if(!a->inHouse){a->x+=ax<brHouseX?0xA0:-0xA0;if(iabs(ax-brHouseX)<6){a->inHouse=1;br_toast("SQUAD ENTERING BUILDING - COVER THEM");}}
   else{brHouseP++;if((brT&31)==0)sfx_shot(1);}continue;}
  if(--a->t<=0){a->alive=0;continue;}
  int tgt=-1,bd=999;for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||!br_hostile(e->k)||e->k>=BE_MINE)continue;int d=iabs((e->x>>8)-ax);if(d<bd){bd=d;tgt=i;}}
  if(tgt>=0&&bd<140){if(--a->cd<=0){BrE*e=&BE[tgt];int dx=(e->x>>8)-ax,dy=((e->y>>8)-8)-((a->y>>8)-9);int d=idist(dx,dy);if(d<1)d=1;bb_new(BK_ALLY,0,a->x,a->y-(9<<8),dx*0x300/d,dy*0x300/d,60,1);a->cd=16;sfx_shot(1);}}
  else if(BL->obj==BOB_DISMOUNT&&brHouseX>0&&!brObjOk)a->x+=ax<brHouseX?0xA0:-0xA0;else a->x+=0xA0;
  {int gy=br_ground(a->x>>8,(a->y>>8)-8);a->y=gy<<8;}}
 if(BL->obj==BOB_DISMOUNT&&!brObjOk){int need=brTier==0?420:brTier==1?600:720;
  if(brHouseP>0&&(brT%200)==0){BrE*s=br_spawn_k(BE_INF,camX+236,0);if(s)s->y=br_ground(camX+236,40)<<8;}
  if(brHouseP>=need){brObjOk=1;br_toast("BUILDING CLEARED! OBJECTIVE COMPLETE");sfx_ok();for(int j=0;j<3;j++)if(BA[j].inHouse){BA[j].inHouse=0;BA[j].t=200;}brScore+=3000;}
  int any=0;for(int j=0;j<3;j++)if(BA[j].alive&&BA[j].inHouse)any=1;if(!any&&brHouseP>0&&(brT&127)==0)br_toast("SQUAD DOWN - DISMOUNT AGAIN");}}
// ---------------------------------------------------------------- bosses (multi-phase by HP thirds)
static int bo_car(void){for(int k=0;k<3;k++)if(BO.car[k]>0)return k;return -1;}   // train: leftmost alive car (0 flak,1 rockets,2 loco)
static int bo_carx(int k){return (BO.x>>8)+(k-1)*64;}
static void brb_dmg(int d,int x,int y){if(d<=0){br_spark(x,y,2);return;}
 if(BO.kind==4){int k=bo_car();if(k<0)return;BO.car[k]-=d;if(BO.car[k]<=0){BO.car[k]=0;int cx=bo_carx(k);br_fireball(cx,(BO.y>>8)-16,1);br_fireball(cx-20,(BO.y>>8)-10,0);sfx_boom();shake(12);
   br_toast(k==0?"FLAK WAGON DESTROYED":k==1?"ROCKET WAGON DESTROYED":"LOCOMOTIVE DESTROYED");brScore+=1500;}
  BO.hp=BO.car[0]+BO.car[1]+BO.car[2];}
 else BO.hp-=d;
 BO.flash=4;br_spark(x,y,2);
 if(BO.hp<=0){BO.hp=0;BO.dead=1;BO.t=0;brScore+=8000*(brCombo?brCombo:1);sfx_boom();shake(16);brKills++;
  if(BO.kind!=1&&BO.kind!=4){BrP*p=bp_add(PK_TOSS,BO.x>>8,(BO.y>>8)-24,0x100,-0x700,150);if(p)p->big=1;}}}
static int brb_hit(int x,int y,BrB*b){if(!BO.on||BO.dead)return 0;int bx=BO.x>>8,by=BO.y>>8,k=BO.kind;
 int d=b->k==BK_TOW?12:b->k==BK_AP?2:b->k==BK_HE?1:b->k==BK_ARTY?6:b->k==BK_ALLY?1:0;
 if(k==1){if(iabs(x-bx)>28||iabs(y-(by-16))>11)return 0;
  if(BO.flare>0&&(b->k==BK_TOW||b->k==BK_HE)){br_spark(x,y,3);if(b->k==BK_TOW)br_fireball(x,y,0);br_toast("FLARES! USE AP CANNON");return 1;}
  if(BO.ph>=1&&b->k==BK_AP)d=4;if(b->k==BK_HE)d=2;}
 else if(k==4){int c=bo_car();if(c<0)return 0;int hit=-1;for(int q=0;q<3;q++)if(BO.car[q]>0&&iabs(x-bo_carx(q))<30&&y>by-28&&y<by)hit=q;if(hit<0)return 0;
  if(hit!=c){br_spark(x,y,2);if(b->k==BK_TOW)br_blast(x,y,20,0,0);if((brT&63)==0)br_toast("ARMOURED - HIT THE NEAREST WAGON");return 1;}}
 else{int top=by-26+(BO.hide?8:0);if(iabs(x-bx)>30||y<top||y>by-2)return 0;
  if(k==0&&BO.ph==0&&y>by-15){br_spark(x,y,1);return 1;}               // berm soaks hull hits
  if(k==0&&BO.hide&&b->k!=BK_TOW){br_spark(x,y,1);return 1;}
  if(k==2&&b->k==BK_TOW&&BO.era<3){BO.era++;br_blast(x,y,18,0,0);br_toast(BO.era<3?"RELIKT ERA BLOCKED THE TOW":"ERA STRIPPED - NOW IT HURTS");d=2;}
  if(k==3&&b->k==BK_AP)d=2;}
 if(b->k==BK_TOW)br_blast(x,y,24,0,0);
 brb_dmg(d,x,y);return 1;}
static void br_bossup(void){
 if(!BO.on){if(brArenaX>=0&&(V.x>>8)>brArenaX+40){BO.on=1;BO.kind=BL->boss;static const u8 HP[5]={70,48,75,85,120};BO.max=BO.hp=SET.bot>=3?BDhp(HP[BO.kind]):HP[BO.kind];
   brArenaW=22*16;BO.x=(brArenaX+(BO.kind==0?264:19*16))<<8;BO.y=(9*16)<<8;if(BO.kind==1)BO.y=70<<8;
   if(BO.kind==4){BO.x=(brArenaX+240)<<8;for(int k=0;k<3;k++)BO.car[k]=BO.max/3;BO.hp=BO.car[0]*3;BO.max=BO.hp;}
   BO.x0=BO.x>>8;BO.cd=120;BO.cd2=60;BO.cd3=200;BO.aps=4;BO.ph=0;BO.dir=-1;bgm_t(TR_BOSS);br_toast(BR_BOSSN[BO.kind]);}return;}
 int bx=BO.x>>8,by=BO.y>>8,px=V.x>>8;BO.t++;if(BO.flash>0)BO.flash--;if(BO.cd>0)BO.cd--;if(BO.cd2>0)BO.cd2--;if(BO.cd3>0)BO.cd3--;if(BO.flare>0)BO.flare--;BO.face=px<bx?-1:1;
 if(BO.dead){if(BO.t<90&&(BO.t&7)==0){br_fireball(bx-30+(BO.t*7)%60,by-12-(BO.t*3)%16,BO.t&8);sfx_boom();}if(BO.t==90){bgm_t(TR_BRAD);br_wreck(BO.x,BO.y,BE_TANK);}
  if(BO.kind==1&&BO.y<(140<<8))BO.y+=0x200;return;}
 int ph=BO.hp*3>BO.max*2?0:BO.hp*3>BO.max?1:2;
 if(ph!=BO.ph){BO.ph=ph;static const char*const PT[5][3]={{"","T-72 BREAKS COVER - CHARGING!","T-72 BURNING - RAPID HE!"},{"","HIND GUN RUNS - AP CANNON!","HIND DROPS TROOPS!"},
  {"","T-90M CALLS ATGM TEAMS","T-90M RUSHES YOU!"},{"","T-14 LAUNCHES DRONE SWARM","T-14 SMOKES AND RAPID-FIRES"},{"","FLAK WAGON DOWN - ROCKETS INBOUND","LOCOMOTIVE MAIN GUN!"}};
  br_toast(PT[BO.kind][ph]);shake(6);sfx_ok();BO.cd=40;}
 int k=BO.kind;
 if(ph==2&&(brT&3)==0)bp_add(PK_EMBER,bx+(int)(rnd()%40)-20,by-20,0,-0xA0,24);
 if(ph>=1&&(brT&15)==0)bp_add(PK_SMOKE,bx+(int)(rnd()%30)-15,by-26,0x10,-0x50,60);
 if(k==1){ // ---- Hind
  int side=((BO.t/300)&1)?-1:1;int alt=ph==0?62:ph==1?92:70+((BO.t>>3)&15);
  int tx=clampi(px+side*(ph==1?60:110),brArenaX+40,brArenaX+brArenaW-40);if(ph==2)tx+=((BO.t>>4)&1)?40:-40;
  BO.x+=((tx<<8)-BO.x)/(ph==1?24:40);BO.y+=((alt<<8)-BO.y)/20;
  // flares decoy HE / TOW
  if(BO.flare<=0&&BO.cd3<=0)for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life>0&&b->team==0&&(b->k==BK_TOW||b->k==BK_HE)&&iabs((b->x>>8)-bx)<70){BO.flare=70;BO.cd3=ph==0?240:420;for(int f=0;f<6;f++)bp_add(PK_FLARE,bx,by-10,(f-3)*0x60,-0x80+f*0x30,60);sfx_whoosh();if(b->k==BK_TOW)b->guided=0;break;}}
  if(BO.cd<=0){for(int r=0;r<4;r++)bb_new(BK_ROCKET,1,(bx+BO.face*20)<<8,(by-6)<<8,BO.face*(0x200+r*0x40),0x1C0+r*0x30,140,10);BO.cd=BDcd(ph==0?130:170);sfx_whoosh();}
  if(BO.cd2<=0){br_aim(bx+BO.face*26,by-8,BK_EBUL,0x300,3,90);BO.cd2=(BO.t&7)?(ph==1?4:7):BDcd(60);}
  if(ph==2&&(BO.t%BDcd(300))==0){for(int i=0;i<2;i++){BrE*s=br_spawn_k(BE_INF,bx-10+i*20,0);if(s)s->y=br_ground(bx-10+i*20,by)<<8;}}
  return;}
 if(k==4){ // ---- armoured train: shuttles; flak, rocket salvoes with warning markers, loco main gun
  int lo=brArenaX+200,hi=brArenaX+252;BO.x+=BO.dir*0x30;if(bx<lo)BO.dir=1;if(bx>hi)BO.dir=-1;
  if((brT&7)==0)bp_add(PK_SMOKE,bo_carx(2)+20,by-34,0x30,-0x40,50);
  if(BO.car[0]>0&&BO.cd<=0){bb_new(BK_FLAK,1,(bo_carx(0)+10)<<8,(by-30)<<8,(px-bo_carx(0))*0x180/140,-0x100,90,2);BO.cd=BDcd(ph==0?60:80);sfx_shot(1);}
  if(BO.car[1]>0&&BO.cd3<=0){for(int i=0;i<2;i++)for(int w=0;w<8;w++)if(BW[w].t<=0){BW[w].x=px-30+i*60;BW[w].t=70+i*10;BW[w].team=1;break;}BO.cd3=BDcd(ph==0?340:260);br_toast("GRAD SALVO - MOVE!");sfx_whoosh();}
  if(BO.car[2]>0&&BO.cd2<=0){int gy=by-18;if(ph==2){bb_new(BK_SHELL,1,(bo_carx(2)-34)<<8,gy<<8,-0x600,0,140,16);bp_add(PK_FLASH,bo_carx(2)-36,gy,0,0,4);sfx_boom();BO.cd2=BDcd(110);}
   else{br_aim(bo_carx(2)-30,gy,BK_EBUL,0x2C0,2,100);BO.cd2=(BO.t&7)?8:BDcd(70);}}
  return;}
 // ---- ground armour
 if(k==0){
  if(ph==0){int cyc=BO.t%200;BO.hide=cyc<110;if(cyc==110-TIER_TANK[brTier]/2)bp_add(PK_GLINT,bx+BO.face*30,by-20,0,0,40);
   if(cyc==150&&BO.cd<=0){int gy=by-19;bb_new(BK_SHELL,1,(bx+BO.face*34)<<8,gy<<8,BO.face*0x600,0,140,16);bp_add(PK_FLASH,bx+BO.face*36,gy,0,0,4);br_fireball(bx+BO.face*36,gy,0);sfx_boom();BO.cd=30;}}
  else{BO.hide=0;int lo=brArenaX+120,hi=brArenaX+300;int tx=ph==1?(px+120<lo?lo:px+120):lo+tri(BO.t/2,hi-lo);if(tx>hi)tx=hi;BO.x+=((tx<<8)-BO.x)/40;BO.y=br_ground(bx,by-20)<<8;
   if(BO.cd<=0){int gy=by-19;if(ph==2){int dx=px-bx;bb_new(BK_EHE,1,(bx+BO.face*30)<<8,gy<<8,dx*0x100/60,-0x380,160,4);BO.cd=BDcd(60);}
    else{bp_add(PK_GLINT,bx+BO.face*30,gy,0,0,TIER_TANK[brTier]);BO.cd2=0;BO.cd=BDcd(140);BO.glint=TIER_TANK[brTier];}sfx_boom();}
   if(BO.glint>0&&--BO.glint==0){int gy=by-19;bb_new(BK_SHELL,1,(bx+BO.face*34)<<8,gy<<8,BO.face*0x600,0,140,16);bp_add(PK_FLASH,bx+BO.face*36,gy,0,0,4);sfx_boom();shake(4);}}
  if(BO.cd2<=0&&!BO.hide){br_aim(bx+BO.face*20,by-22,BK_EBUL,0x2C0,2,100);BO.cd2=(BO.t&7)?8:BDcd(90);}
  return;}
 {int lo=brArenaX+150,hi=brArenaX+brArenaW-36;int tx=lo+tri(BO.t/2,hi-lo);
  if(k==2&&ph==2)tx=clampi(px+70,brArenaX+80,hi);
  BO.x+=((tx<<8)-BO.x)/(k==2&&ph==2?20:60);BO.y=br_ground(BO.x>>8,by-20)<<8;}
 if(k==3){ // Afganit APS intercepts TOW + HE; recharges
  if(BO.aps<4&&++BO.apsT>=240){BO.apsT=0;BO.aps++;}
  for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life>0&&b->team==0&&(b->k==BK_TOW||b->k==BK_HE)&&BO.aps>0&&iabs((b->x>>8)-bx)<62&&iabs((b->y>>8)-(by-16))<40){b->life=0;BO.aps--;bp_add(PK_APS,b->x>>8,b->y>>8,0,0,8);bp_add(PK_FB,b->x>>8,b->y>>8,0,0,14);sfx_shot(1);
    br_toast(BO.aps?"AFGANIT INTERCEPT - OVERLOAD IT":"AFGANIT EMPTY - TOW NOW!");}}
  if(ph>=1&&BO.cd3<=0){for(int i=0;i<3;i++){BrE*f=br_spawn_k(BE_FPV,bx-10+i*12,2);if(f)f->y=(by-40-i*8)<<8;}BO.cd3=BDcd(400);br_toast("T-14 LAUNCHES FPV DRONES");}
  if(ph==2&&(BO.t%360)==0){for(int i=0;i<6;i++)bp_add(PK_SMOKE,bx-30+i*12,by-20,0,-0x20,160);}}
 if(k==2&&ph>=1&&BO.cd3<=0){int n=0;for(int i=0;i<nBE;i++)if(BE[i].alive&&BE[i].k==BE_ATGM)n++;if(n<2){BrE*e=br_spawn_k(BE_ATGM,brArenaX+(ph==1?150:200),0);if(e)e->y=br_ground(e->x>>8,40)<<8;br_toast("ATGM TEAM DEPLOYED");}BO.cd3=BDcd(600);}
 int gy=by-19;
 if(BO.cd<=0){int fast=ph==2;
  bp_add(PK_GLINT,bx+BO.face*30,gy,0,0,TIER_TANK[brTier]);BO.glint=TIER_TANK[brTier]*(fast?2:3)/3;BO.cd=BDcd(fast?90:150);}
 if(BO.glint>0&&--BO.glint==0){bb_new(BK_SHELL,1,(bx+BO.face*34)<<8,gy<<8,BO.face*0x600,(brSmokeT>0?-0x100:0),140,16);bp_add(PK_FLASH,bx+BO.face*36,gy,0,0,4);br_fireball(bx+BO.face*36,gy,0);sfx_boom();shake(4);
  if(k==2&&brSmokeT<=0&&ph==0)bb_new(BK_ATGM,1,(bx+BO.face*34)<<8,(gy-4)<<8,BO.face*0x180,0,220,16);}
 if(BO.cd2<=0){br_aim(bx+BO.face*20,gy-3,BK_EBUL,0x2C0,2,100);BO.cd2=(BO.t&7)?(ph==2?5:8):BDcd(90);}}
// ---------------------------------------------------------------- draw
static void br_tiles(void){int tx0=camX>>4,ty0=camY>>4;
 for(int cy=ty0;cy<ty0+11;cy++)for(int cx=tx0;cx<tx0+16;cx++){int t=ptile(cx,cy);if(cy>=PH)t=T_AIR;
  u16 b=(t*4)|(PF_MTB[t]<<12);int X=(cx*2)&31,Y=(cy*2)&31;bgw[Y*32+X]=b;bgw[Y*32+X+1]=b+1;bgw[(Y+1)*32+X]=b+2;bgw[(Y+1)*32+X+1]=b+3;}}
static void bspr_vf(int x,int y,int tile,int shape,int size,int pal,int hf,int vf){bspr(x,y,tile,shape,size,pal,hf);if(vf&&noam>0)oam[(noam-1)*4+1]|=1<<13;}
static void br_weather(void){int fl=BL->fl;if(!(fl&(FL_SAND|FL_SNOW|FL_RAIN)))return;
 for(int i=0;i<18;i++){int x=WXx[i],y=WXy[i];
  if(fl&FL_SAND){x-=6+(i&3);y+=1;}else if(fl&FL_RAIN){x-=2;y+=6+(i&1);}else{x+=((brT+i*7)>>4)&1?1:-1;if(((brT+i)&1)==0)y+=1+(i&1);}
  x-=(V.vx>>8);if(x<-8)x+=248;if(x>244)x-=248;if(y>164){y=-8-(i*13)%40;x=(i*53+brT*7)%240;}
  WXx[i]=x;WXy[i]=y;
  bsprx(x,y,OT_C+((fl&FL_SAND)?BT_P_SAND:(fl&FL_RAIN)?BT_P_RAIN:BT_P_FLAKE),0,0,BRP_HOT,0,1,fl&FL_SAND?1:0);}
 if((fl&FL_RAIN)&&(fl&FL_NIGHT)&&!V.thermal){if(brLightT>0){if(--brLightT==0)br_palettes();}else if((rnd()&511)==0){brLightT=5;br_palettes();sfx_boom();}}}
static void br_draw_ent(BrE*e){int x=(e->x>>8)-camX,y=(e->y>>8)-camY;if(x<-50||x>290)return;
 int hf=e->face<0,f=(e->t>>3)&1,pal=BRP_EN;if(e->c&1)return;
 switch(e->k){
 case BE_INF:bspr(x-8,y-16,OT_C+BT_RIFLE0+f*4*(!e->fall),0,1,pal,hf);break;
 case BE_RPG:bspr(x-8,y-16,OT_C+(e->st==1?BT_RPGAIM:BT_RPG0+f*4),0,1,pal,hf);if(e->st==1&&(brT&4))bspr(x-4,y-28,OT_C+BT_P_WARN,0,0,BRP_HOT,0);break;
 case BE_ATGM:bspr(x-16,y-16,OT_C+BT_NEST,1,2,pal,hf);break;
 case BE_TECH:bspr(x-16,y-16,OT_C+BT_TECH0+f*8,1,2,pal,hf);break;
 case BE_BTR:bspr(x-32,y-31,OT_C+BT_BTR0+((e->t>>2)&1)*32,1,3,pal,hf);break;
 case BE_BMP2:bspr(x-32,y-31,OT_C+BT_BMP0+((e->t>>2)&1)*32,1,3,pal,hf);break;
 case BE_TANK:bspr(x-32,y-31+((e->st==1&&(e->st2&2))?0:0),OT_C+BT_TANK0+((e->t>>2)&1)*32,1,3,pal,hf);if(e->st==1&&(brT&8))bspr(x-4,y-44,OT_C+BT_P_WARN,0,0,BRP_HOT,0);break;
 case BE_HELI:bspr(x-32,y-24,OT_C+BT_HELI0+((brT>>1)&1)*32,1,3,pal,hf);break;
 case BE_ORLAN:bspr(x-8,y-8,OT_C+BT_ORLAN0+((brT>>1)&1)*4,0,1,pal,hf);break;
 case BE_FPV:bspr(x-8,y-8,OT_C+BT_FPV0+((brT>>1)&1)*4,0,1,pal,0);break;
 case BE_MINE:{int near=iabs(x+camX-(V.x>>8))<100||BS.var==1||BS.var==2;if(near)bspr(x-8,y-6,OT_C+BT_MINE0+((brT>>4)&1)*2,1,0,pal,0);break;}
 case BE_IED:bspr(x-8,y-7,OT_C+BT_IED0+((brT>>3)&1)*2,1,0,pal,0);if(BS.var>=1&&BS.var!=3&&iabs(x-(V.x>>8)+camX)<140&&(brT&16))bspr(x-4,y-20,OT_C+BT_P_WARN,0,0,BRP_HOT,0);break;
 case BE_CAR:bspr(x-16,y-16,OT_C+BT_CAR,1,2,(e->hp*2<e->mhp)?BRP_BURNT:BRP_MISC,hf);break;
 case BE_FLEE:bspr(x-32,y-31,OT_C+BT_URAL,1,3,pal,0);break;
 case BE_AIRC:bspr(x-32,y-31,OT_C+BT_AIRC,1,3,pal,0);break;
 case BE_RADAR:bspr(x-16,y-31,OT_C+BT_RADAR,0,2,pal,((brT>>5)&1));break;
 case BE_SAM:bspr(x-16,y-31,OT_C+BT_SAM,0,2,pal,0);break;
 case BE_CONVOY:bspr(x-32,y-31,OT_C+BT_TRUCK0,1,3,BRP_MISC,0);break;
 case BE_TOWC:bspr(x-8,y-15,OT_C+BT_CRATE0,0,1,BRP_MISC,0);break;
 case BE_REP:bspr(x-8,y-15,OT_C+BT_CRATE1,0,1,BRP_MISC,0);break;
 case BE_INTEL:bspr(x-8,y-15+((brT>>4)&1),OT_C+BT_INTEL0+((brT>>3)&1)*4,0,1,BRP_MISC,0);break;
 case BE_CHECK:bspr(x-8,y-31,OT_C+BT_FLAG0,2,2,BRP_MISC,0);break;
 case BE_GOAL:bspr(x-8,y-31,OT_C+(brObjOk?BT_FLAG1:BT_FLAG2),2,2,BRP_MISC,0);break;
 case BE_HOUSE:if(!brObjOk)bspr(x-8,y-80+((brT>>4)&1)*2,OT_C+BT_FLAG2,2,2,BRP_MISC,0);break;
 case BE_WRECK:{int k=e->st;if(k==BE_TANK)bspr(x-32,y-31,OT_C+BT_TANK0,1,3,BRP_BURNT,0);else if(k==BE_BMP2)bspr(x-32,y-31,OT_C+BT_BMP0,1,3,BRP_BURNT,0);else if(k==BE_BTR)bspr(x-32,y-31,OT_C+BT_BTR0,1,3,BRP_BURNT,0);
  else if(k==BE_AIRC)bspr(x-32,y-31,OT_C+BT_AIRC,1,3,BRP_BURNT,0);else if(k==BE_FLEE)bspr(x-32,y-31,OT_C+BT_URAL,1,3,BRP_BURNT,0);else if(k==BE_HELI)bspr(x-32,y-26,OT_C+BT_HELI0,1,3,BRP_BURNT,0);
  else if(k==BE_CAR)bspr(x-16,y-16,OT_C+BT_CAR,1,2,BRP_BURNT,0);else if(k==BE_RADAR)bspr(x-16,y-31,OT_C+BT_RADAR,0,2,BRP_BURNT,0);else if(k==BE_SAM)bspr(x-16,y-31,OT_C+BT_SAM,0,2,BRP_BURNT,0);break;}}}
static void br_draw(void){oam_begin();
 for(int i=0;i<8;i++)if(BW[i].t>0&&(BW[i].t&8))bspr(BW[i].x-4-camX,24,OT_C+BT_P_WARN,0,0,BRP_HOT,0);
 // ATGM laser beams from lasing nests / BMPs
 if(!V.dead)for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||e->a<=0||(e->k!=BE_ATGM&&e->k!=BE_BMP2))continue;int x0=(e->x>>8)-camX,y0=(e->y>>8)-camY-(e->k==BE_ATGM?12:24),x1=(V.x>>8)-camX,y1=(V.y>>8)-camY-14;
  for(int s=1;s<7;s++){int q=(s*16+((brT*2)&15));int x=x0+(x1-x0)*q/112,y=y0+(y1-y0)*q/112;bspr(x-4,y-4,OT_C+BT_P_LASER,0,0,BRP_HOT,0);}
  bspr(x1-4+((brT>>1)&1),y1-4,OT_C+BT_P_LASERDOT,0,0,BRP_HOT,0);}
 // projectiles
 for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life<=0)continue;int x=(b->x>>8)-camX,y=(b->y>>8)-camY;int hf=b->vx<0;
  switch(b->k){
   case BK_AP:bspr(x-4,y-4,OT_C+(b->trc?BT_P_APTR:BT_P_AP),0,0,BRP_HOT,hf);if(b->trc)bspr(x-4-(b->vx>>7),y-4-(b->vy>>7),OT_C+BT_P_AP,0,0,BRP_HOT,hf);break;
   case BK_HE:bspr(x-4,y-4,OT_C+BT_P_HE,0,0,BRP_HOT,hf);break;
   case BK_MG:case BK_ALLY:bspr(x-4,y-4,OT_C+BT_P_MG,0,0,BRP_HOT,hf);break;
   case BK_EBUL:bspr(x-4,y-4,OT_C+BT_P_EBUL,0,0,BRP_HOT,hf);break;
   case BK_SHELL:case BK_EHE:case BK_FLAK:bspr(x-4,y-4,OT_C+BT_P_SHELL,0,0,BRP_HOT,hf);break;
   case BK_ARTY:bspr(x-4,y-4,OT_C+BT_P_ARTY,0,0,BRP_HOT,0);break;
   case BK_TOW:bspr(x-8,y-4,OT_C+BT_M_TOW,1,0,BRP_EN,hf);if(b->guided&&(brT&1)){int sx=(V.x>>8)-camX-V.tface*6,sy=(V.y>>8)-camY-30;for(int s=1;s<4;s++)bspr(sx+(x-sx)*s/4-4,sy+(y-sy)*s/4-4,OT_C+BT_P_EMBER,0,0,BRP_HOT,0);}break;
   case BK_RPG:bspr(x-8,y-4,OT_C+BT_M_RPG,1,0,BRP_HOT,hf);break;
   case BK_ATGM:bspr(x-8,y-4,OT_C+BT_M_ATGM,1,0,BRP_HOT,hf);break;
   case BK_ROCKET:bspr(x-8,y-4,OT_C+BT_M_ROCKET,1,0,BRP_HOT,hf);break;}}
 br_parts_draw(1);br_parts_draw(0);
 for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(a->alive&&!a->inHouse)bspr((a->x>>8)-8-camX,(a->y>>8)-16-camY,OT_C+BT_ALLY0+((brT>>3)&1)*4,0,1,BRP_MISC,0);}
 // the Bradley: hull (4-frame tracks, suspension bob, landing squash), turret barrel (affine, recoil), add-on kits
 if(!V.dead&&!(V.inv>40&&(brT&2))){int sx=(V.x>>8)-32-camX,sy=(V.y>>8)-31-camY+(V.water?3:0);int hf=V.face<0;
  int bob=(iabs(V.vx)>0x80&&V.on)?((V.anim>>4)&1):0;if(V.land>3)bob=1;
  int fr=(V.anim>>3)&3;
  if(V.flash>0){int a=BR_ELEV[V.aim];int mx=br_pivx()+V.tface*COS(a)*19/256-camX,my=br_pivy()-SIN(a)*19/256-camY;bspr(mx-4,my-4+bob,OT_FX+FX_FLASH,0,0,3,0);}
  if(BS.upg&UP_ERA)bspr(sx,sy+bob,OT_C+BT_ERA,1,3,BRP_CAM,hf);
  if(BS.upg&UP_APS)bspr(hf?sx+30:sx+18,sy-6+bob,OT_C+BT_APS,0,1,BRP_CAM,hf);
  bspr(sx,sy+bob,OT_V+fr*32,1,3,BRP_CAM,hf);
  {int a=BR_ELEV[V.aim];int ang=V.tface>0?(256-a)&255:(a+128)&255;int rc=V.recoil;
   int cx=br_pivx()-camX-V.tface*COS(a)*rc/256,cy=br_pivy()-camY+SIN(a)*rc/256+bob;
   if(V.tface!=V.face){cx=(V.x>>8)+V.tface*13-camX;} spr_aff(cx,cy,OT_BAR,2,BRP_CAM,ang,0x100,1,0);}}
 if(BO.on&&!(BO.dead&&BO.t>80)&&!(BO.flash&1)){int x=(BO.x>>8)-camX,y=(BO.y>>8)-31-camY;
  if(BO.kind==4){for(int k=0;k<3;k++){static const u8 TK[3]={32,64,0};bspr(bo_carx(k)-32-camX,y,OT_BO+TK[k],1,3,BO.car[k]>0?BRP_EN:BRP_BURNT,k==2);}}
  else if(BO.kind==1)bspr(x-32,y-8+16,OT_BO+((brT>>1)&1)*32,1,3,BO.dead?BRP_BURNT:BRP_EN,BO.face<0);
  else bsprx(x-32,y+(BO.hide?8:0),OT_BO+((BO.t>>2)&1)*32,1,3,BO.dead?BRP_BURNT:BRP_EN,BO.face<0,(BO.kind==0&&BO.ph==0)?2:1,0);
  if(BO.kind==3)for(int q=0;q<BO.aps;q++)bspr(x-14+q*8,y-10,OT_C+BT_P_APS,0,0,BRP_HOT,0);}
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(e->alive&&e->k!=BE_WRECK)br_draw_ent(e);}
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(e->alive&&e->k==BE_WRECK)br_draw_ent(e);}
 if(brSmokeT>0){int sx=(V.x>>8)-camX,sy=(V.y>>8)-camY;for(int k=0;k<3;k++)bsprx(sx-48+k*32-((brT>>3)&3)+V.face*20,sy-38-(k&1)*6,OT_C+BT_SMK0+((brT>>4)&1)*16,0,2,BRP_HOT,0,1,1);}
 // foreground grass tufts swaying at ground level
 if(!(BL->fl&FL_SNOW)&&BL->th!=10)for(int gx=((camX/48)*48);gx<camX+250;gx+=48){int cx=(gx>>4)+((gx/48)&1);int top=-1;for(int y=4;y<PH;y++)if(TFL[ptile(cx,y)]&F_SOL){top=y;break;}
  if(top>0&&ptile(cx,top)==T_TOP)bspr(gx+((gx/48*7)&15)-camX,top*16-6-camY,OT_C+BT_GRASS0+(((brT>>4)+(gx/48))&1)*2,1,0,BRP_MISC,0);}
 br_weather();
 fx_update_draw();}
static const char*br_objtxt(void){static char b[24];const char*s="";
 switch(BL->obj){case BOB_TARGETS:s=cat3("TARGETS ",itoa(brObj),cat("/",itoa(brObjN)));break;
  case BOB_CHASE:s=cat3("TRUCKS ",itoa(brObj),cat("/",itoa(brObjN)));break;
  case BOB_ESCORT:{int hp=0;if(brConvoy>=0&&BE[brConvoy].alive)hp=BE[brConvoy].hp*100/BE[brConvoy].mhp;s=brObjOk?"CONVOY SAFE":cat3("CONVOY ",itoa(hp),"%");break;}
  case BOB_DEFEND:s=brObjOk?"HELD":brLock?cat3("WAVE ",itoa(brWave+1),cat("/",itoa(brObjN))):"DEFEND THE X-ROADS";break;
  case BOB_DISMOUNT:{int need=brTier==0?420:brTier==1?600:720;s=brObjOk?"BUILDING CLEAR":brHouseP?cat3("CLEARING ",itoa(brHouseP*100/need),"%"):"DISMOUNT AT FLAG";break;}
  default:return "";}
 int k=0;while(s[k]&&k<22){b[k]=s[k];k++;}b[k]=0;return b;}
static void br_hud(void){hud_clear();
 htxt(0,0,"HULL",V.hp*4<V.maxhp&&(brT&8)?HB_RED:HB_WHITE);hbar(4,0,5,V.hp,V.maxhp);
 if(V.rel>0)htxt(10,0,"RLD",(brT&8)?HB_YEL:HB_GRAY);else htxt(10,0,cat(V.ammo?"HE":"AP",itoa(V.mag[V.ammo])),V.ammo?HB_GREEN:HB_YEL);
 if(V.ovh>0)htxt(15,0,"HOT",(brT&4)?HB_RED:HB_YEL);else hbar(15,0,3,V.heat,100);
 htxt(19,0,cat(BS.var==4?"ART":"TOW",itoa(V.tow)),V.tow?(V.towcd?HB_GRAY:HB_YEL):HB_GRAY);
 htxt(24,0,cat("S",itoa(V.smoke)),brSmokeT>0?HB_GREEN:HB_WHITE);
 if(BS.upg&UP_APS)htxt(27,0,cat("A",itoa(V.aps)),V.aps?HB_GREEN:HB_GRAY);
 htxt(0,1,cat("x",itoa(V.lives)),HB_WHITE);
 {char b[4];for(int i=0;i<3;i++)b[i]=(brMedal>>i&1)?'#':'-';b[3]=0;htxt(3,1,b,HB_YEL);}
 htxt(7,1,br_objtxt(),brObjOk?HB_GREEN:HB_BLUE);
 if(brCombo>=2)htxt(19,1,cat("x",itoa(brCombo)),HB_YEL);
 {const char*s=itoa(brScore);htxt(30-slen(s),1,s,HB_WHITE);}
 if(V.sq&&BL->obj==BOB_DISMOUNT)htxt(0,2,cat("SQUAD ",itoa(V.sq)),HB_GREEN);
 if(V.thermal)htxt(24,2,"THERMAL",HB_GREEN);
 if(brLaser&&(brT&8))hctxt(3,"!! LASER WARNING !!",HB_RED,0);
 if(BO.on&&!BO.dead){htxt(1,18,BR_BOSSN[BO.kind],HB_RED);hbar(1,19,14,BO.hp,BO.max);
  if(BO.kind==3)htxt(17,18,cat("APS ",itoa(BO.aps)),HB_YEL);if(BO.kind==2&&BO.era<3)htxt(17,18,cat("ERA ",itoa(3-BO.era)),HB_YEL);}
 if(brToastT>0){brToastT--;hctxt(4,brToast,HB_YEL,0);}}
// ---------------------------------------------------------------- results
static int br_result(void){int i=brI;int s1=brNoDmg,s2=brT<=BL->par,s3=brHostile==0||brKills*10>=brHostile*8;
 u8 v=BS.cs[i]|1|((brMedal&7)<<1)|(s1<<4)|(s2<<5)|(s3<<6);BS.cs[i]=v;
 int sc=brScore+V.hp*20+(s2?3000:0)+(s1?5000:0);if(sc<0)sc=0;brScore=sc;
 int nb=0;if(sc/10>BS.best[i]){BS.best[i]=sc/10>65535?65535:sc/10;nb=1;}BS.lastW=brW;BS.lastC=brC;bs_write();music_jingle(TR_CLEAR);
 int n=__builtin_popcount(brMedal&7);int st=s1+s2+s3;
 for(int t=0;;t++){BEAT->screen=SCR_BRAD;BEAT->phase=4;BEAT->scoreA=sc;BEAT->scoreB=n|(st<<4);BEAT->kills=brKills;hud_clear();hpanel(3,3,24,14);hctxt(4,"COURSE CLEAR",HB_YEL,1);
  hptxt(5,6,BL->n,HB_WHITE);
  hptxt(5,8,cat3("INTEL ",itoa(n),"/3"),HB_YEL);
  hptxt(5,9,s1?"* NO DAMAGE":"- NO DAMAGE",s1?HB_YEL:HB_GRAY);
  hptxt(5,10,cat3(s2?"* UNDER PAR ":"- UNDER PAR ",mmss(BL->par),""),s2?HB_YEL:HB_GRAY);
  hptxt(5,11,cat3(s3?"* KILLS ":"- KILLS ",itoa(brKills),cat("/",itoa(brHostile))),s3?HB_YEL:HB_GRAY);
  hptxt(5,12,cat("TIME ",mmss(brT)),HB_WHITE);hptxt(16,12,cat("DEATHS ",itoa(brDeaths)),HB_WHITE);
  hptxt(5,14,cat("SCORE ",itoa(sc)),nb?HB_GREEN:HB_WHITE);if(nb)hptxt(19,14,"BEST!",HB_GREEN);
  if(t>40)hptxt(9,15,"A: CONTINUE",(t>>4)&1?HB_YEL:HB_WHITE);
  oam_begin();present(1,camX/3,camY/2+10);REG_BGHOFS(3)=camX/8;REG_BGVOFS(3)=camY/8+6;
  if(t>40&&((pressed&(K_A|K_START))||(br_isauto()&&t>70)))break;}
 return 1;}
// ---------------------------------------------------------------- test autopilot (harness pokes 'AUTO' at 0x0203FFD0)
static int apPrev,apStuck,apLastX,apSelT,apRev,apIgn,apProgX,apProgT;
static void br_auto(void){int k=0,px=V.x>>8,py=V.y>>8;
 if(V.dead){keys=0;pressed=0;apPrev=0;return;}
 int best=-1,bd=9999,tx=0,ty=0,air=0,armour=0,boss=0;
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||!br_hostile(e->k)||e->k==BE_MINE||e->k==BE_IED)continue;int ex=e->x>>8;int dx=ex-px;if(dx<-130||dx>235)continue;
  int sc=iabs(dx)+(e->k==BE_CAR?150:0)-(e->k==BE_ATGM&&e->a>0?120:0)-(e->k==BE_FPV?80:0);if(sc<bd){bd=sc;best=i;}}
 if(best>=0){BrE*e=&BE[best];tx=e->x>>8;air=ent_air(e->k);ty=(e->y>>8)-(air?0:ent_h(e->k)/2+2);armour=e->k==BE_TANK||e->k==BE_BMP2||e->k==BE_BTR||e->k==BE_FLEE||e->k==BE_AIRC||e->k==BE_SAM||e->k==BE_RADAR;}
 if(BO.on&&!BO.dead){boss=1;tx=BO.x>>8;ty=(BO.y>>8)-(BO.kind==1?16:20);if(BO.kind==4){int c=bo_car();if(c>=0)tx=bo_carx(c);}armour=BO.kind!=1;air=BO.kind==1;
  if(best>=0&&ent_air(BE[best].k)&&iabs((BE[best].x>>8)-px)<120){tx=BE[best].x>>8;ty=BE[best].y>>8;air=1;armour=0;boss=0;}}
 int have=best>=0||boss;
 // turret elevation
 int want=1;if(have){int dxa=iabs(tx-br_pivx());if(dxa<4)dxa=4;int up=br_pivy()-ty;int ang=64-atan2i(dxa,-up);if(ang>100)ang-=256;
  int bi=0,bdd=999;for(int q=0;q<8;q++){int d=iabs(BR_ELEV[q]-ang);if(d<bdd){bdd=d;bi=q;}}want=bi;
  }
 if(V.aim<want)k|=K_UP;else if(V.aim>want)k|=K_DOWN;
 if(have&&iabs(V.aim-want)<=1)k|=K_A;
 // ammo selection: HE for air/infantry, AP for armour
 int wantHE=have&&(air||(!armour&&!boss&&best>=0&&BE[best].k<=BE_ATGM));
 if(apSelT>0){apSelT--;if(apSelT>3)k|=K_SEL;k&=~K_A;}else if(have&&wantHE!=V.ammo&&V.rel<=0){apSelT=8;}
 // TOW at armour / boss
 int tow=V.towId>=0&&BB[V.towId].life>0&&BB[V.towId].k==BK_TOW;
 if(tow&&BB[V.towId].guided){k|=K_R;k&=~(K_UP|K_DOWN);int my=BB[V.towId].y>>8;if(my>ty+3)k|=K_UP;else if(my<ty-3)k|=K_DOWN;}
 else if(have&&(armour||boss)&&V.tow>0&&V.towcd<=0&&iabs(tx-px)<230&&!(boss&&BO.kind==3&&BO.aps>0&&V.tow<3)&&!(boss&&BO.kind==1&&BO.flare>0))k|=K_R;
 // smoke on laser lock
 if(brLaser&&brLaserT>TIER_LOCK[brTier]/2&&V.smoke>0&&brSmokeT<=0&&!(apPrev&K_L))k|=K_L|K_R;
 brLaserT=0;
 // movement
 int go=1;
 if(have&&armour&&!boss&&tx>px&&tx-px<120)go=0;
 if(boss){int bx=BO.x>>8;go=(bx-px>(BO.kind==1?40:170));if(BO.kind==0&&BO.ph==0)go=bx-px>200;}
 if(BL->obj==BOB_ESCORT&&brConvoy>=0&&BE[brConvoy].alive&&(BE[brConvoy].x>>8)<px-80)go=0;
 if(BL->obj==BOB_DISMOUNT&&!brObjOk&&brHouseX>0&&iabs(px-brHouseX)<70){go=px<brHouseX-20;int any=0;for(int j=0;j<3;j++)if(BA[j].alive)any=1;if(!any&&V.sq>0&&!(k&K_SEL))k|=K_SEL;}
 if(brLock)go=px<(brLockL+brLockR)/2-30;
 if(++apProgT>240){apProgT=0;if(px<apProgX+24&&!brLock&&!boss)apIgn=360;apProgX=px;}
 if(apIgn>0){apIgn--;}
 else if(have&&!boss&&best>=0&&BE[best].k<=BE_ATGM&&iabs(tx-px)<90&&iabs(ty-py)<24){go=0;if(tx>px)k|=K_RIGHT;else k=(k&~K_RIGHT)|K_LEFT;}
 else if(have&&!boss&&tx<px-16){go=0;if(V.tface>0)k=(k&~K_A)|K_LEFT;else if(V.face>0)k|=K_LEFT;}
 if(BL->obj==BOB_TARGETS&&!brObjOk&&brGoalX>0&&px>brGoalX-80){int nx=-1;for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(e->alive&&(e->k==BE_AIRC||e->k==BE_RADAR||e->k==BE_SAM))nx=e->x>>8;}if(nx>=0&&nx<px-120){go=0;k=(k&~K_RIGHT)|K_LEFT;apIgn=200;}}
 if(go)k|=K_RIGHT;
 if(apRev>0){apRev--;k&=~K_RIGHT;k|=K_LEFT;}
 // jumps: walls, ditches, incoming tank shells
 if(V.on){int ahead=px+34;int wall=br_solid(ahead,py-20)||br_solid(ahead,py-6)&&br_solid(ahead,py-20);int floor=0;for(int d=0;d<40;d+=8)if(br_floor(ahead,py+d,py+d-1)||br_solid(ahead,py+d))floor=1;
  if((k&K_RIGHT)&&(!floor||(wall&&ptile(ahead>>4,(py-20)>>4)!=T_BRICK)))k|=K_B;
  for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life>0&&b->team==1&&b->k==BK_SHELL&&iabs((b->x>>8)-px)<90&&((b->x>>8)>px)==(b->vx<0))k|=K_B;}}
 if(V.on&&ptile((px+34)>>4,(py-20)>>4)==T_BRICK){k|=K_A;k&=~(K_UP);if(V.aim>1)k|=K_DOWN;}
 if(iabs(px-apLastX)<1&&(k&K_RIGHT)){if(++apStuck>70){k|=K_B;if(apStuck>200){apRev=30;apStuck=0;}}}else apStuck=0;apLastX=px;
 keys=k;pressed=k&~apPrev;apPrev=k;}
// ---------------------------------------------------------------- course loop
static void br_start(int i){brI=i;brW=i/7;brC=i%7;BL=&BR_LV[i];br_decode();memset(BE,0,sizeof BE);memset(BP,0,sizeof BP);nBE=0;brEi=0;brChkX=0;brT=0;brDone=0;brKills=0;brHostile=0;brMedal=0;brArenaX=-1;brArenaW=22*16;brDeaths=0;
 brTier=SET.bot==0?0:SET.bot==1?1:2;{u32 m=*(volatile u32*)0x0203FFDC;if(br_isauto()&&(m>>8)==0x544952u)brTier=m&3;}/* test-only tier override */brCombo=brComboT=0;brObj=0;brObjN=BL->objn;brObjOk=BL->obj==BOB_REACH;brGoalX=-1;brLock=0;brWave=0;brWaveT=0;brHouseP=0;brHouseX=-1;brConvoy=-1;brFail=0;brLaser=0;brLaserT=0;brLightT=0;brNoDmg=1;
 mapWrap=0;memset(&BO,0,sizeof BO);memset(BCR,0,sizeof BCR);for(int k=0;k<18;k++){WXx[k]=(k*53)%240;WXy[k]=(k*37)%160;}
 memset(&V,0,sizeof V);V.lives=brTier==0?5:3;br_reset_life();br_load_gfx();camX=0;camY=32;
 for(int k=0;k<BL->ne&&BL->e[k].x<64;k++){}
 apPrev=apStuck=apLastX=apSelT=apRev=apIgn=apProgX=apProgT=0;}
static int br_run(int i){brScore=0;br_start(i);bgm_t(TR_BRAD);br_toast(BL->n);int doneT=0,au=br_isauto();
 for(;;){BEAT->screen=SCR_BRAD;BEAT->mode=brW;BEAT->era=brC;BEAT->hp=V.hp;BEAT->x=V.x>>12;BEAT->y=V.y>>12;BEAT->phase=brDone?1:V.dead?2:(BO.on&&!BO.dead)?3:0;
  BEAT->kills=brKills;BEAT->deaths=brDeaths;BEAT->scoreA=brScore;BEAT->scoreB=brMedal|(BO.on?BO.hp<<4:0);BEAT->extra=pW|(BS.var<<10);BEAT->timer=brT;
  brT++;if(brSmokeT>0)brSmokeT--;
  if(brConvoy<0||!BE[brConvoy].alive||BE[brConvoy].k!=BE_CONVOY){brConvoy=-1;for(int q=0;q<nBE;q++)if(BE[q].alive&&BE[q].k==BE_CONVOY)brConvoy=q;}
  if(au&&!brDone)br_auto();
  if(!brDone){br_vehicle();br_ents();br_bossup();br_bullets();br_parts();}else{doneT++;br_parts();}
  {int tx=(V.x>>8)-100+V.face*24;int lo=0,hi=pW*16-240;if(BO.on&&!BO.dead){lo=brArenaX;hi=brArenaX+brArenaW-240;}if(brLock){lo=brLockL-20;hi=brLockR+20-240+40;if(hi<lo)hi=lo;}tx=clampi(tx,lo,hi);
   if(iabs(tx-camX)>200)camX=tx;else camX+=(tx-camX)/6+((tx>camX)-(tx<camX));camX=clampi(camX,lo,hi);
   int ty=clampi((V.y>>8)-112,0,PH*16-160);camY+=(ty-camY)/4;}
  br_tiles();br_draw();br_hud();present(1,camX/3,camY/2+10);REG_BGHOFS(3)=camX/8;REG_BGVOFS(3)=camY/8+6;
  if(brFail&&!V.dead){V.dead=1;V.deadT=0;br_toast(brFail==1?"CONVOY LOST - MISSION RESTART":"TARGET ESCAPED - RESTART");sfx_hurt();}
  if(V.dead&&V.deadT>90){brDeaths++;V.lives--;
   if(V.lives<=0){music_defeat();for(int t=0;t<120;t++){hud_clear();hpanel(8,7,14,4);hctxt(8,"GAME OVER",HB_RED,1);oam_begin();BEAT->phase=5;present(1,camX/3,camY/2+10);}return 0;}
   br_reset_life();camX=clampi((V.x>>8)-100,0,pW*16-240);
   if(brFail==1||(BL->obj==BOB_ESCORT&&brConvoy<0&&!brObjOk)){BrE*c=br_spawn_k(BE_CONVOY,(V.x>>8)-50,0);if(c){c->y=br_ground((V.x>>8)-50,40)<<8;}brConvoy=-1;}
   if(brFail==2){BrE*f=br_spawn_k(BE_FLEE,(V.x>>8)+260,0);if(f)f->y=br_ground((V.x>>8)+260,40)<<8;}
   for(int q=0;q<nBE;q++)if(BE[q].alive&&BE[q].k==BE_FLEE&&(BE[q].x>>8)>(V.x>>8)+400){BE[q].x=((V.x>>8)+260+q*30)<<8;BE[q].y=br_ground(BE[q].x>>8,40)<<8;}
   brFail=0;
   if(BO.on&&!BO.dead){brChkX=brArenaX+40;br_reset_life();BO.hp+=(BO.max-BO.hp)/4;if(BO.kind==4){int c=bo_car();if(c>=0){BO.car[c]+=(BO.max/3-BO.car[c])/4;}BO.hp=BO.car[0]+BO.car[1]+BO.car[2];}V.tow+=2;br_toast("RETRY - BOSS KEEPS ITS DAMAGE");}
   if(brLock){for(int q=0;q<nBE;q++)if(BE[q].wave)BE[q].alive=0;brWaveT=90;}}
  if(brDone&&doneT>60)return br_result();
  if(pressed&K_START){if(pause_menu())return 0;br_load_gfx();}}}
// ---------------------------------------------------------------- menus (mode 3)
static void m3_tiles(const u32*t,int w8,int h8,const u16*pal,int x0,int y0,int sc,int hf){
 for(int ty=0;ty<h8;ty++)for(int tx=0;tx<w8;tx++){const u32*q=t+(ty*w8+tx)*8;for(int y=0;y<8;y++){u32 r=q[y];for(int x=0;x<8;x++){int c=(r>>(4*x))&15;if(!c)continue;
  int X=hf?(w8*8-1-(tx*8+x)):(tx*8+x);for(int a=0;a<sc;a++)for(int b=0;b<sc;b++){int px=x0+X*sc+a,py=y0+(ty*8+y)*sc+b;if(px>=0&&px<240&&py>=0&&py<160)fb[py*240+px]=pal[c];}}}}}
static void br_vpreview(int x,int y,int sc){int cm=BS.camo<3?BS.camo:0;m3_tiles(br_veh+BS.var*128*8,8,4,BR_CAMO[cm],x,y,sc,0);
 // barrel drawn flat at the pivot
 for(int i=0;i<15;i++){for(int a=0;a<sc;a++)for(int b=0;b<2*sc;b++){int px=x+(45+i)*sc+a,py=y+6*sc+b;if(px<240&&py<160)fb[py*240+px]=BR_CAMO[cm][i>12?13:8];}}}
#define NGROW (3+NUPG)
static int br_garage(void){int sel=0,redraw=1;
 for(;;){BEAT->screen=SCR_BRSEL;BEAT->mode=1;BEAT->x=sel;BEAT->extra=BS.var|(BS.camo<<4)|(BS.upg<<8);
  if(redraw){cat_bgc(CAT_SPEC);banner("BRADLEY GARAGE","VARIANT / CAMO / UPGRADES",C_YEL);m3_box(4,38,232,118,1);
   br_vpreview(150,44,1);m3_text(150,78,BR_VARN[BS.var],C_YEL,1);
   m3_text(12,44,sel==0?">":" ",C_YEL,1);m3_text(20,44,"VARIANT",C_WHITE,1);m3_text(70,44,br_var_open(BS.var)?BR_VARN[BS.var]:"LOCKED",sel==0?C_YEL:C_WHITE,1);
   m3_text(12,55,sel==1?">":" ",C_YEL,1);m3_text(20,55,"CAMO",C_WHITE,1);m3_text(70,55,BS.camo<3?BR_CAMON[BS.camo]:"AUTO (WORLD)",sel==1?C_YEL:C_WHITE,1);
   for(int k=0;k<NUPG;k++){int y=67+k*10,op=br_upg_open(k);m3_text(12,y,sel==2+k?">":" ",C_YEL,1);m3_text(20,y,UPG_N[k],op?C_WHITE:C_GRAY,1);
    m3_text(98,y,op?((BS.upg>>k&1)?"ON":"OFF"):cat3("* ",itoa(UPG_COST[k]),""),op?((BS.upg>>k&1)?C_GREEN:C_GRAY):C_RED,1);}
   m3_text(12,120,sel==NGROW-1?">":" ",C_YEL,1);m3_text(20,120,"DONE",sel==NGROW-1?C_YEL:C_WHITE,1);
   m3_text(130,98,cat3("INTEL ",itoa(br_intel()),"/84"),C_YEL,1);m3_text(130,108,cat3("STARS ",itoa(br_stars()),"/84"),C_YEL,1);
   const char*d=sel==0?BRV_D[BS.var]:sel>=2&&sel<2+NUPG?UPG_D[sel-2]:sel==1?"DESERT / WOODLAND / WINTER":"UPGRADES UNLOCK WITH INTEL+STARS";m3_ctext(136,d,C_GRAY,1);
   if(sel==0&&!br_var_open(BS.var))m3_ctext(146,BS.var==4?"CLEAR WORLD 4 OR 60 INTEL+STARS":cat3("CLEAR WORLD ",itoa(BS.var)," BOSS"),C_RED,1);
   m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+NGROW-1)%NGROW;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%NGROW;redraw=1;sfx_menu();}
  int d=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(d||(pressed&K_A)){if(!d)d=1;
   if(sel==0){BS.var=(BS.var+5+d)%5;redraw=1;sfx_menu();}
   else if(sel==1){BS.camo=BS.camo>=3?(d>0?0:2):(BS.camo+d<0||BS.camo+d>2?255:BS.camo+d);redraw=1;sfx_menu();}
   else if(sel<NGROW-1){if(br_upg_open(sel-2)){BS.upg^=1<<(sel-2);redraw=1;sfx_ok();}}
   else if(pressed&K_A){if(!br_var_open(BS.var))BS.var=0;bs_write();pressed=0;return 0;}}
  if(pressed&K_B){if(!br_var_open(BS.var))BS.var=0;bs_write();pressed=0;return 0;}}}
static void bradley_ops(void){bs_read();int w=BS.lastW<4?BS.lastW:0,c=BS.lastC<7?BS.lastC:0,redraw=1;
 if(!br_var_open(BS.var))BS.var=0;bgm_t(TR_BRAD);
 if(keys&K_R)BS.allun=1; // demo/test unlock (hold R entering the mode)
 for(;;){int i=w*7+c;BEAT->screen=SCR_BRSEL;BEAT->mode=0;BEAT->x=i;BEAT->extra=BS.var;
  {u32 m=*(volatile u32*)0x0203FFD8;if((m>>8)==0x425243u&&br_isauto()){*(volatile u32*)0x0203FFD8=0;i=m&31;w=i/7;c=i%7;sfx_ok();br_run(i);pressed=0;bgm_t(TR_BRAD);bgCached=-1;redraw=1;continue;}}
  if(redraw){cat_bgc(CAT_SPEC);banner("SPEC OPS: BRADLEY",BR_WN[w],C_YEL);m3_box(4,38,232,118,1);
   m3_text(12,44,cat3("WORLD ",itoa(w+1)," <L R>"),C_YEL,1);
   for(int k=0;k<7;k++){int j=w*7+k,y=56+k*11,op=br_course_open(j);const BrLv*L=&BR_LV[j];
    m3_text(12,y,k==c?">":" ",C_YEL,1);m3_text(20,y,cat3(itoa(w+1),"-",itoa(k+1)),op?C_WHITE:C_GRAY,1);m3_text(44,y,op?L->n:"LOCKED",k==c?C_YEL:op?C_WHITE:C_GRAY,1);
    if(BS.cs[j]&1){char b[8];for(int q=0;q<3;q++)b[q]=(BS.cs[j]>>(q+1)&1)?'#':'-';b[3]=' ';for(int q=0;q<3;q++)b[4+q]=(BS.cs[j]>>(q+4)&1)?'*':'-';b[7]=0;m3_text(166,y,b,C_YEL,1);}
    else if(L->boss<5)m3_text(186,y,"BOSS",C_RED,1);}
   {const BrLv*L=&BR_LV[i];static const char*const ON[6]={"REACH THE FLAG","ESCORT THE CONVOY","DEFEND THE POSITION","CHASE THE COLUMN","DESTROY TARGETS","DISMOUNT + CLEAR"};
    const char*fx=(L->fl&FL_NIGHT)?((L->fl&FL_RAIN)?"  NIGHT RAIN":(L->fl&FL_SAND)?"  NIGHT SAND":"  NIGHT"):(L->fl&FL_SAND)?"  SANDSTORM":(L->fl&FL_SNOW)?"  SNOW":(L->fl&FL_RAIN)?"  RAIN":"";
    m3_text(12,136,cat(L->boss<5?"BOSS FIGHT":ON[L->obj],fx),C_GREEN,1);}
   m3_text(150,136,cat3("BEST ",itoa(BS.best[i]*10),""),C_WHITE,1);
   m3_ctext(147,"A: DRIVE  SELECT: GARAGE  B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){c=(c+6)%7;redraw=1;sfx_menu();}if(pressed&K_DOWN){c=(c+1)%7;redraw=1;sfx_menu();}
  if(pressed&(K_L|K_LEFT)){w=(w+3)%4;redraw=1;sfx_menu();}if(pressed&(K_R|K_RIGHT)){w=(w+1)%4;redraw=1;sfx_menu();}
  if(pressed&K_SEL){br_garage();redraw=1;}
  if(pressed&K_A){if(br_course_open(i)){sfx_ok();int ok=br_run(i);pressed=0;bgm_t(TR_BRAD);if(ok&&c<6)c++;bgCached=-1;redraw=1;}else sfx_hurt();}
  if(pressed&K_B){pressed=0;bgCached=-1;return;}}}
static void rec_brad(const char**L){static char b[5][40];bs_read();int n=0,best=0,bi=0;for(int i=0;i<28;i++){n+=BS.cs[i]&1;if(BS.best[i]>best){best=BS.best[i];bi=i;}}
 const char*a;int k;
 a=cat3("COURSES CLEARED: ",itoa(n),"/28");k=0;while(a[k]&&k<38){b[0][k]=a[k];k++;}b[0][k]=0;L[0]=b[0];
 a=cat3("INTEL ",itoa(br_intel()),cat3("/84  STARS ",itoa(br_stars()),"/84"));k=0;while(a[k]&&k<38){b[1][k]=a[k];k++;}b[1][k]=0;L[1]=b[1];
 a=cat3("BEST SCORE: ",itoa(best*10),best?cat("  ",BR_LV[bi].n):"");k=0;while(a[k]&&k<38){b[2][k]=a[k];k++;}b[2][k]=0;L[2]=b[2];
 int nv=0;for(int v=0;v<5;v++)nv+=br_var_open(v);a=cat3("VARIANTS UNLOCKED: ",itoa(nv),"/5");k=0;while(a[k]&&k<38){b[3][k]=a[k];k++;}b[3][k]=0;L[3]=b[3];
 int bw=0;for(int i=0;i<28;i++)if(BR_LV[i].boss<5&&(BS.cs[i]&1))bw++;a=cat3("BOSSES DEFEATED: ",itoa(bw),"/5");k=0;while(a[k]&&k<38){b[4][k]=a[k];k++;}b[4][k]=0;L[4]=b[4];}
