// ===================================================================== v1.2 SPEC OPS PLATFORMER
// Side-scrolling run-and-gun. 10 worlds / 90 courses (tools/gen_courses.py + gen_levels.py), 17 selectable units.
#include "platdb.inc"
#include "pflevels.inc"
#define SCR_PLAT 40
#define SCR_PMAP 41
#define SCR_PGLOBE 42
#define SCR_PUNIT 43
#define PH 12
enum{T_AIR,T_TOP,T_FILL,T_BRICK,T_CRATE,T_USED,T_SOLID,T_PLAT,T_LADDER,T_ROPE,T_WTOP,T_WATER,T_SPIKE,T_HIDDEN,T_REVEAL,T_DOOR,T_GRAPPLE,T_DECO1,T_DECO2,T_GHOST,T_INVIS,
 T_CRUMB,T_CONVL,T_CONVR,T_CANNON,T_PIPETL,T_PIPETR,T_PIPEL,T_PIPER,T_LAVA,T_LAVA2,T_SPRING,T_SWOFF,T_SWON,T_N};
enum{F_SOL=1,F_ONE=2,F_HAZ=4,F_WAT=8,F_CLB=16,F_BRK=32,F_BUMP=64,F_KILL=128};
static const u8 TFL[T_N]={0,1,1,1|32|64,1|64,1,1,2,16,16,8,8,4,0,1,1,0,0,0,0,1, 2,1,1,1,1,1,1,1,4|128,4|128,2,0,1};
static u8 pm[PH*352] EWRAM;static int pW;
static u8 tideM[6][48];
#define PMI(x,y) ((y)*352+(x))
static inline int ptile(int cx,int cy){if(cx<0||cx>=pW)return T_SOLID;if(cy<0)return T_AIR;if(cy>=PH)return T_AIR;return pm[PMI(cx,cy)];}
static inline int pflag(int px,int py){return TFL[ptile(px>>4,py>>4)];}
// ---------------------------------------------------------------- save (SRAM 0x500, 'AOEP' v1)
typedef struct{u8 cs[PF_NW][PF_MS];u8 unit,lastW,lastS,demo;u16 tags;u8 allun,pad;}PSave;
static PSave PS;
#define PS_OFF 0x500
static void ps_write(void){const u8*b=(const u8*)&PS;u8 sum=0x5A;SRAM[PS_OFF]='A';SRAM[PS_OFF+1]='O';SRAM[PS_OFF+2]='E';SRAM[PS_OFF+3]='P';SRAM[PS_OFF+4]=1;
 for(unsigned i=0;i<sizeof PS;i++){SRAM[PS_OFF+5+i]=b[i];sum+=b[i]*(i+3);}SRAM[PS_OFF+5+sizeof PS]=sum;}
static void ps_read(void){memset(&PS,0,sizeof PS);if(SRAM[PS_OFF]!='A'||SRAM[PS_OFF+1]!='O'||SRAM[PS_OFF+2]!='E'||SRAM[PS_OFF+3]!='P'||SRAM[PS_OFF+4]!=1)return;
 u8 b[sizeof(PSave)];u8 sum=0x5A;for(unsigned i=0;i<sizeof PS;i++){b[i]=SRAM[PS_OFF+5+i];sum+=b[i]*(i+3);}if(sum!=SRAM[PS_OFF+5+sizeof PS])return;memcpy(&PS,b,sizeof PS);
 if(PS.unit>=PF_NU)PS.unit=0;if(PS.lastW>=PF_NW)PS.lastW=0;if(PS.lastS>=PF_NS[PS.lastW])PS.lastS=0;}
static int cs_clear(int w,int s){return PS.cs[w][s]&1;}
static int cs_intel(int w,int s){int v=PS.cs[w][s]>>1&7;return (v&1)+(v>>1&1)+(v>>2&1);}
static int is_bonus(int w,int s){return (PF_ST[w][s].fl&SF_K)!=0;}
static int world_intel(int w){int n=0;for(int s=0;s<PF_NS[w];s++)n+=cs_intel(w,s);return n;}
static int world_secret(int w){for(int s=0;s<PF_NS[w];s++)if(PS.cs[w][s]&16)return 1;return 0;}
static int course_open(int w,int s){if(PS.allun)return 1;
 if(w>0&&!cs_clear(w-1,PF_NS[w-1]-1))return 0;
 if(is_bonus(w,s))return world_secret(w)||world_intel(w)>=9||cs_clear(w,s);
 for(int p=s-1;p>=0;p--)if(!is_bonus(w,p))return cs_clear(w,p);
 return 1;}
static int world_open(int w){return PS.allun||w==0||cs_clear(w-1,PF_NS[w-1]-1);}
static int total_clear(void){int n=0;for(int w=0;w<PF_NW;w++)for(int s=0;s<PF_NS[w];s++)n+=cs_clear(w,s);return n;}
static int total_intel(void){int n=0;for(int w=0;w<PF_NW;w++)n+=world_intel(w);return n;}
// ---------------------------------------------------------------- units / abilities
enum{AB_GLIDE,AB_HIJUMP,AB_MEDIC,AB_AIR,AB_SPRINT,AB_STEALTH,AB_ROCKET,AB_NVG,AB_GRAPPLE,AB_SNIPER,AB_FLASH,AB_WALL,AB_DRONE,AB_MELEE,AB_FOCUS,AB_BARRAGE,AB_AMPHIB,AB_BREACH,AB_DIVER,AB_DEMO,AB_HEAVY,AB_RECON,AB_S13,AB_RESCUE,AB_SPY};
static const char*const AB_N[PF_NU]={"GLIDE CHUTE","HIGH JUMP + CHUTE","MEDIC / REVIVE","CALL AIRSTRIKE","SPRINT + FAST CLIMB","STEALTH + SWIM","ROCKET LAUNCHER","NIGHT VISION",
 "GRAPPLE + HACK","SNIPER SHOT","FLASHBANG BREACH","WALL JUMP + SWIM","FPV DRONE STRIKE","BLADE + DOUBLE JUMP","MARKSMAN FOCUS","GRENADE BARRAGE","AMPHIBIOUS DASH","FAST-ROPE + BREACH","COMBAT DIVER","DEMOLITIONS","HEAVY WEAPONS","STEALTH RECON","NAVAL COMMANDO","AIRBORNE RESCUE","DISGUISE + LOCKPICK"};
static const char*const AB_D[PF_NU]={"PARADROP START. HOLD A TO GLIDE","JUMPS HIGHER. R: PARACHUTE","MEDKITS +2. R: HEAL. 1 REVIVE","R: BOMBING RUN ON SCREEN",
 "R: SPRINT. CLIMBS 2X FASTER","R: STEALTH. FAST SWIM, NO AIR","STARTS WITH LMG. R: ROCKET","R: NVG REVEALS HIDDEN BLOCKS","R: GRAPPLE HOOK. HACKS DOORS",
 "MARKSMAN RIFLE. R: HEAVY SHOT","R: FLASHBANG STUNS + BREACHES","WALL JUMP. FAST SWIM, NO AIR","R: FPV DRONE HOMES ON TARGET","DOUBLE JUMP. R: BLADE STRIKE",
 "PRECISION RIFLE. R: SLOW TIME","+3 GRENADES. R: 3-GRENADE FAN","FAST SWIM, NO AIR. R: DASH","FAST-ROPE IN. R: BREACH CHARGE","FAST SWIM, NO AIR. R: LIMPET MINE","PARADROP. R: CHUTE / DEMO CHARGE","LMG. R: PLANT HEAVY MG (2X FIRE)","TAVOR X95. R: STEALTH, 2X DMG","SWIM/NO AIR. R: DPV DASH / LIMPET","PARADROP, +3 LIVES. R: CHUTE/HEAL","PISTOL. R: DISGUISE. PICKS LOCKS"};
static const char*const WPN_N[9]={"CARBINE","SMG","LMG","SUPPRESSED","MARKSMAN","CARBINE","CARBINE","CARBINE","SUPPR. PISTOL"};
static const u8 WPN_RATE[9]={10,6,5,9,24,10,10,10,13},WPN_DMG[9]={1,1,1,1,3,1,1,1,1};
static int is_swimmer(int u){return u==5||u==11||u==16||u==AB_DIVER||u==AB_S13;}
// ---------------------------------------------------------------- runtime state
typedef struct{s32 x,y,vx,vy;s16 hp,maxhp,armor,inv,cd,gcd,abcd,abT,gren,rapidT,jetT,swT,air,lives,anim,face,on,crouch,climb,swim,jumps,chute,dead,deadT,revived,dashT,hookT,hx,hy,nvg,wallT,spaceW,stand;}PPl;
static PPl P;
typedef struct{s32 x,y,vx,vy;s16 hp,t,cd,a,b,c,stun;u8 k,p,face,alive,on,st;s16 x0,y0;}PEnt;
#define NPE 96
static PEnt PE[NPE];static int nPE;
typedef struct{s32 x,y,vx,vy;s16 life,dmg;u8 k,team,pierce,pad;}PB;
#define NPB 40
static PB PBs[NPB];static int pMainW;
enum{PK_BUL,PK_EBUL,PK_ROCKET,PK_GREN,PK_SHELL,PK_BOMB,PK_PLASMA,PK_ARC,PK_FPV,PK_ESHELL};
static int pw,ps_,pTheme,pFl,pAuto,pAutoX,pDone,pT,pIntel,pTags,pSecret,pBoss,pBossOn,pArenaX,pWindT,pWindDir,pWindOn,pTide,pSlow,pDemo,pChkX,pChkY,pRaft,pCamY;
static s32 pCamX8;
static int pSwOn;
typedef struct{s32 x,y,vx,vy;s16 hp,max,t,ph,cd,cd2,fr,flash,inv,n;u8 ai,face;}PBoss;
static PBoss B;
typedef struct{s16 x,y,t;}Crumb;
#define NCR 16
static Crumb CR[NCR];
// OBJ tile layout
#define OT_PL 64
#define OT_EN 96
#define OT_MI 168
#define OT_PICK OT_MI
#define OT_FLAG (OT_MI+40)
#define OT_KC 404
#define OT_PROJ (OT_MI+56)
#define OT_CHUTE (OT_MI+64)
#define OT_RAFT (OT_MI+68)
#define OT_FPVD (OT_MI+76)
#define OT_FLAME (OT_MI+80)
#define OT_BA 256
#define OT_BB 320
#define OT_GEN 384   // C-generated: crusher 16x32 (8), platform 32x8 (4), shell 16x8 (2), p-switch 16x16 (4), firebar ball 8x8 (1)
#define OT_CRUSH OT_GEN
#define OT_MPLAT (OT_GEN+8)
#define OT_SHELLS (OT_GEN+12)
#define OT_PSW (OT_GEN+14)
#define OT_BALL (OT_GEN+18)
static void gen_tile(int tile,int w8,int h8,int(*f)(int,int)){volatile u32*t=(volatile u32*)(OBJ_TILES)+tile*8;
 for(int ty=0;ty<h8;ty++)for(int tx=0;tx<w8;tx++)for(int y=0;y<8;y++){u32 v=0;for(int x=0;x<8;x++)v|=(u32)(f(tx*8+x,ty*8+y)&15)<<(x*4);t[(ty*w8+tx)*8+y]=v;}}
static int g_crush(int x,int y){if(x==0||x==15||y==0)return 1;if(y>=28){return ((x+1)%4<2)?(y==31?1:10):(y>=30?0:9);}if(y<4)return 9;if((x==3||x==12)&&(y==8||y==20))return 1;if(y==14||y==15)return 12;return (x<3)?10:9;}
static int g_mplat(int x,int y){if(y==0||y==7||x==0||x==31)return 1;if(y==1)return 15;if(y>=5)return 6;if(x%8==4)return 1;return 7;}
static int g_shell(int x,int y){int cy=y-3;if(x<1||y<0||y>7)return 0;if(x>11){int r=(x-11)*(x-11)+cy*cy*3;return r<26?(r>18?1:9):0;}if(y==0||y==7)return 1;if(x<3)return (y>1&&y<6)?11:1;return y==2?10:8;}
static int g_psw(int x,int y){if(y<6)return 0;if(y>=14)return (x>1&&x<14)?13:0;if(x<2||x>13)return 0;if(y==6)return 1;if(x==2||x==13)return 1;return (y==7)?4:3;}
static int g_ball(int x,int y){int d=(x-3)*(x-3)+(y-3)*(y-3);return d<3?15:d<7?12:d<12?11:0;}
// ---------------------------------------------------------------- level load
static void pl_decode(void){const PfLv*L=&PF_LV[pw][ps_];pW=L->w;const u8*t=L->t;
 for(int y=0;y<PH;y++){int x=0;while(x<pW){int v=*t++,n=*t++;for(int i=0;i<n;i++)pm[PMI(x+i,y)]=v;x+=n;}}
 for(int y=1;y<PH;y++)for(int x=0;x<pW;x++)if(pm[PMI(x,y)]==T_TOP){int a=pm[PMI(x,y-1)];if(a==T_TOP||a==T_FILL)pm[PMI(x,y)]=T_FILL;}
 memset(tideM,0,sizeof tideM);}
static PEnt*pe_new(void){for(int i=0;i<NPE;i++)if(!PE[i].alive){PEnt*e=&PE[i];memset(e,0,sizeof*e);e->alive=1;if(i>=nPE)nPE=i+1;return e;}return 0;}
static int BDcd(int v){return v*(100-BOTD[SET.bot].cd*3)/100;}
static int BDhp(int v){int r=v*BOTD[SET.bot].hp/100;return r<1?1:r;}
static void pl_spawn_ents(int from_chk){const PfLv*L=&PF_LV[pw][ps_];memset(PE,0,sizeof PE);nPE=0;
 for(int i=0;i<L->ne;i++){int tp=L->e[i*3]&255,p=L->e[i*3]>>8,x=L->e[i*3+1],y=L->e[i*3+2];if(!tp)continue;
  if(tp==10&&(PS.cs[pw][ps_]>>(1+p)&1)&&0)continue;
  if(tp==10&&(pIntel>>p&1))continue;
  PEnt*e=pe_new();if(!e)break;e->k=tp;e->p=p;e->x=(x*16+8)<<8;e->y=(y*16+16)<<8;e->x0=x;e->y0=y;e->face=-1;
  switch(tp){case 1:e->hp=BDhp(2);break;case 2:e->hp=BDhp(4);break;case 3:e->hp=BDhp(3);break;case 4:e->hp=BDhp(2);break;case 5:e->hp=BDhp(2);e->y-=4<<8;break;
   case 6:e->hp=BDhp(10);break;case 7:e->hp=BDhp(18);break;
   case 20:if(from_chk&&(x*16+8)<=pChkX)e->st=1;break;
   case 24:e->a=(i*40)&255;break;case 27:e->st=0;e->y=(y*16)<<8;break;case 28:e->cd=60+i*13%90;break;}
  if(tp>=10&&tp<=18){int t=ptile(x,y);if(t==T_CRATE||t==T_HIDDEN||t==T_BRICK)e->st=2;}
  e->cd+=30+(i*17)%60;}
 pMainW=pW*16;for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].k==31&&PE[i].p==0)pMainW=(PE[i].x0-2)*16;}
static void pl_load_gfx(void){
 vsync();REG_DISPCNT=0x80;
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(PF_TILES[pTheme],CHARBLOCK(1),34*32);dma32(PF_BDT[pTheme],(u8*)CHARBLOCK(1)+160*32,PF_BDN[pTheme]*8);
 for(int i=0;i<48;i++)PAL_BG[i]=pf_pal[pTheme*48+i];
 if(pFl&SF_D){for(int i=1;i<32;i++)if(i!=16)PAL_BG[i]=tint_c(PAL_BG[i],3);}
 for(int i=0;i<1024;i++)bgc[i]=PF_BDM[pTheme][i];
 dma32(fx_tiles,OBJ_TILES,sizeof(fx_tiles)/4);
 dma32(pf_unit_spr+PS.unit*28*8,(u8*)OBJ_TILES+OT_PL*32,28*8);
 dma32(PF_ESET[PF_ST[pw][ps_].es],(u8*)OBJ_TILES+OT_EN*32,72*8);
 dma32(pf_misc,(u8*)OBJ_TILES+OT_MI*32,sizeof(pf_misc)/4);
 const PfStage*S=&PF_ST[pw][ps_];
 if(S->ba!=BK_NONE)dma32(PF_BK[S->ba],(u8*)OBJ_TILES+OT_BA*32,(S->ba==BK_CMDR?32:64)*8);
 if(S->bb!=BK_NONE)dma32(PF_BK[S->bb],(u8*)OBJ_TILES+OT_BB*32,(S->bb==BK_CMDR?32:64)*8);
 gen_tile(OT_CRUSH,2,4,g_crush);gen_tile(OT_MPLAT,4,1,g_mplat);gen_tile(OT_SHELLS,2,1,g_shell);gen_tile(OT_PSW,2,2,g_psw);gen_tile(OT_BALL,1,1,g_ball);dma32(kc_spr,(u8*)OBJ_TILES+OT_KC*32,sizeof(kc_spr)/4);for(int i=0;i<16;i++)PAL_OBJ[80+i]=kc_pal[i];
 for(int i=0;i<16;i++){PAL_OBJ[i]=pf_unit_pal[PS.unit*16+i];PAL_OBJ[16+i]=pf_en_pal[S->ep*16+i];PAL_OBJ[32+i]=pf_boss_pal[S->bp*16+i];PAL_OBJ[48+i]=fx_pal[i];PAL_OBJ[64+i]=pf_misc_pal[i];}
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=1|(1<<2)|(30<<8);REG_BGCNT(2)=3|(1<<2)|(29<<8);
 REG_BLDCNT=(1<<6)|(1<<9);REG_BLDALPHA=8|(8<<8);
 hud_clear();oam_begin();memset(bgw,0,2048);
 REG_DISPCNT=0|(1<<6)|(1<<8)|(1<<9)|(1<<10)|(1<<12);shimmer=0;}
static void pl_respawn(int fromChk){
 pl_decode();memset(PBs,0,sizeof PBs);memset(CR,0,sizeof CR);memset(FXs,0,sizeof FXs);pSwOn=0;
 pl_spawn_ents(fromChk);
 int sx=fromChk?pChkX:24,sy=-1;
 if(!fromChk&&(pFl&SF_B)){sx=56;}
 for(int y=0;y<PH;y++){if(!(TFL[ptile(sx>>4,y)]&(F_SOL))&&(TFL[ptile(sx>>4,y+1)]&(F_SOL|F_ONE))){sy=y*16+16;break;}}
 if(sy<0)sy=9*16;
 int keepHp=P.maxhp?P.maxhp:3;int lives=P.lives,gr=P.gren;
 memset(&P,0,sizeof P);P.lives=lives;P.maxhp=keepHp;P.hp=P.maxhp;P.gren=gr<3?3+(PS.unit==AB_BARRAGE?3:0):gr;
 P.x=sx<<8;P.y=sy<<8;P.face=1;P.air=600;P.inv=60;
 if(!fromChk&&((pFl&SF_C)||PS.unit==AB_GLIDE||PS.unit==AB_DEMO||PS.unit==AB_RESCUE)&&!(pFl&(SF_T|SF_B|SF_A))){P.y=-8<<8;P.chute=1;}
 memset(&B,0,sizeof B);pBossOn=0;
 pAuto=(pFl&(SF_A|SF_B))?1:0;pCamX8=fromChk?((pChkX-80)<<8):0;if(pCamX8<0)pCamX8=0;
 pTide=0;pWindOn=0;pWindT=0;pRaft=-1;
 for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].k==35){pRaft=i;PE[i].x=(sx-16)<<8;PE[i].y=(9*16+2)<<8;}}
static void pl_start(int w,int s){pw=w;ps_=s;const PfStage*S=&PF_ST[w][s];pTheme=S->th;pFl=S->fl;pIntel=0;pTags=0;pSecret=0;pDone=0;pT=0;pChkX=0;
 memset(&P,0,sizeof P);P.lives=PS.unit==AB_RESCUE?8:5;P.maxhp=3;P.gren=3+(PS.unit==AB_BARRAGE?3:0);
 pl_load_gfx();pl_respawn(0);}
// ---------------------------------------------------------------- collision helpers
static int solid_at(int px,int py){int f=pflag(px,py);return f&F_SOL;}
static int box_hit(s32 x,s32 y,int hw,int hh){int px=x>>8,py=y>>8;
 for(int yy=py-hh;yy<py;yy+=8){if(solid_at(px-hw,yy)||solid_at(px+hw,yy))return 1;}
 return solid_at(px-hw,py-1)||solid_at(px+hw,py-1);}
static void bump_tile(int cx,int cy);
static void break_tile(int cx,int cy){int t=ptile(cx,cy);if(t==T_BRICK||t==T_DOOR){pm[PMI(cx,cy)]=T_AIR;fx_add(cx*16+8,cy*16+8,FXK_SMALL);for(int i=0;i<3;i++)fx_add(cx*16+4+i*4,cy*16+4,FXK_SPARK);}}
static void blast(int x,int y,int r,int dmg,int team);
static int pKills,pToastT,pNextTag;static const char*pToast1;static char pToastB[32];static u16 dprev;
static void ptoast(const char*s){int k=0;while(*s&&k<30)pToastB[k++]=*s++;pToastB[k]=0;pToast1=pToastB;pToastT=120;}
static void sprf(int x,int y,int tile,int shape,int size,int pal,int prio,int hf){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14)|(hf?1<<12:0);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static void pl_die(int pit){if(P.dead)return;
 if(!pit&&PS.unit==AB_MEDIC&&!P.revived){P.revived=1;P.hp=2;P.inv=150;ptoast("PJ REVIVE!");sfx_ok();return;}
 P.dead=1;P.deadT=0;P.vy=-0x500;P.vx=0;sfx_boom();shake(10);}
static void pl_hurt(int n){if(P.inv>0||P.dead||P.dashT>0)return;
 if(pDemo){P.inv=30;return;}
 if(P.armor>0)P.armor--;else P.hp-=n;P.inv=90;sfx_hurt();if(P.hp<=0)pl_die(0);else{P.vy=-0x300;P.vx=-P.face*0x180;}}
static PB*pb_new(int k,int team,s32 x,s32 y,s32 vx,s32 vy,int life,int dmg){for(int i=0;i<NPB;i++)if(PBs[i].life<=0){PB*b=&PBs[i];b->x=x;b->y=y;b->vx=vx;b->vy=vy;b->k=k;b->team=team;b->life=life;b->dmg=dmg;b->pierce=0;return b;}return 0;}
static void bump_tile(int cx,int cy){int t=ptile(cx,cy);
 if(t==T_CRATE||t==T_HIDDEN||t==T_BRICK){int found=0;
  for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(e->alive&&e->st==2&&e->x0==cx&&e->y0==cy){e->st=0;e->y=(cy*16)<<8;found=1;sfx_ok();}}
  if(t==T_BRICK&&!found){if(PS.unit==AB_ROCKET||P.armor>0||P.spaceW){break_tile(cx,cy);}else sfx1(0x08,0x6,1,1,1200,6);return;}
  pm[PMI(cx,cy)]=T_USED;if(!found){pTags++;fx_add(cx*16+8,cy*16-6,FXK_SPARK);sfx_menu();}
  for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(e->alive&&e->k<10&&iabs((e->x>>8)-(cx*16+8))<12&&iabs((e->y>>8)-(cy*16))<6){e->hp=0;}}}}
static void ent_kill(PEnt*e){e->alive=0;pKills++;fx_add(e->x>>8,(e->y>>8)-8,e->k>=6?FXK_BIG:FXK_SMALL);if(e->k>=6){sfx_boom();shake(6);}else sfx_shot(1);
 if(e->k<=7&&((pKills*7)%5)==0){PEnt*t=pe_new();if(t){t->k=11;t->x=e->x;t->y=e->y-(4<<8);}}}
static void ent_dmg(PEnt*e,int d){e->hp-=d;e->c=6;if(e->hp<=0)ent_kill(e);}
static void boss_dmg(int d);static void boss_start(void);
static int boss_hit(int x,int y){if(!pBossOn||B.hp<=0)return 0;int hw=B.ai==BA_MI8&&B.ph==1?12:28,ht=B.ai==BA_MI8&&B.ph==1?30:28;return iabs(x-(B.x>>8))<hw&&y<(B.y>>8)-2&&y>(B.y>>8)-ht;}
static void blast(int x,int y,int r,int dmg,int team){fx_add(x,y,FXK_BIG);sfx_boom();shake(6);
 if(team==0){for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(!e->alive||e->k>9)continue;if(idist((e->x>>8)-x,(e->y>>8)-8-y)<r+6)ent_dmg(e,dmg);}
  if(pBossOn&&idist((B.x>>8)-x,(B.y>>8)-16-y)<r+28)boss_dmg(dmg);
  for(int cy=(y-r)>>4;cy<=(y+r)>>4;cy++)for(int cx=(x-r)>>4;cx<=(x+r)>>4;cx++)if(idist(cx*16+8-x,cy*16+8-y)<r+4)break_tile(cx,cy);}
 else if(idist((P.x>>8)-x,(P.y>>8)-8-y)<r)pl_hurt(1);}
// ---------------------------------------------------------------- player
static int stand_on(int px,int py,int oldpy){ // returns 1 if feet at py land on something
 int t=ptile(px>>4,py>>4),f=TFL[t];if(f&F_SOL)return 1;
 if((f&F_ONE)&&(oldpy>>4)<(py>>4)+(((oldpy&15)==0)?1:0)&&(oldpy<=((py>>4)<<4)))return 1;
 return 0;}
static int plat_under(int px,int fy,int oldfy){for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(!e->alive)continue;
 if(e->k==25||e->k==26){int lx=e->x>>8,ty=e->y>>8;if(px>=lx-2&&px<=lx+33&&oldfy<=ty+1&&fy>=ty)return i+1;}
 else if(e->k==35){int lx=e->x>>8,ty=e->y>>8;if(px>=lx&&px<=lx+31&&oldfy<=ty+1&&fy>=ty)return i+1;}}return 0;}
static void pl_ability(void){int u=PS.unit;if(P.abcd>0)return;
 switch(u){
  case AB_GLIDE:case AB_HIJUMP:if(!P.on){P.chute^=1;sfx_whoosh();}break;
  case AB_MEDIC:if(P.hp<P.maxhp){P.hp++;P.abcd=900;sfx_ok();ptoast("FIELD MEDICINE +1");}break;
  case AB_AIR:P.abT=50;P.abcd=900;ptoast("AIRSTRIKE INBOUND");sfx_whoosh();break;
  case AB_SPRINT:P.abT=200;P.abcd=420;break;
  case AB_STEALTH:P.abT=300;P.abcd=720;ptoast("STEALTH");break;
  case AB_ROCKET:{PB*b=pb_new(PK_ROCKET,0,P.x+P.face*(8<<8),P.y-(9<<8),P.face*0x500,0,120,6);(void)b;P.abcd=80;sfx_shot(1);}break;
  case AB_NVG:P.nvg^=1;P.abcd=20;if(pFl&SF_D){for(int i=1;i<32;i++)if(i!=16)PAL_BG[i]=P.nvg?tint_c(pf_pal[pTheme*48+i],1):tint_c(pf_pal[pTheme*48+i],3);}sfx_menu();break;
  case AB_GRAPPLE:{int bx=-1,by=-1,bd=1<<20;int cx0=(P.x>>12)-7,cy0=(P.y>>12)-8;
    for(int cy=cy0;cy<cy0+9;cy++)for(int cx=cx0;cx<cx0+15;cx++)if(ptile(cx,cy)==T_GRAPPLE){int d=idist(cx*16+8-(P.x>>8),cy*16+8-(P.y>>8));if(d<bd&&d<130){bd=d;bx=cx;by=cy;}}
    if(bx<0){int cx=P.x>>12;for(int cy=(P.y>>12)-1;cy>=(P.y>>12)-6;cy--)if(TFL[ptile(cx,cy)]&F_SOL){bx=cx;by=cy;break;}}
    if(bx>=0){P.hookT=40;P.hx=bx*16+8;P.hy=by*16+16+14;P.abcd=40;sfx_whoosh();}}break;
  case AB_SNIPER:{PB*b=pb_new(PK_BUL,0,P.x+P.face*(8<<8),P.y-(9<<8),P.face*0x900,0,60,6);if(b)b->pierce=1;P.abcd=60;sfx_shot(1);}break;
  case AB_FLASH:{int x=(P.x>>8)+P.face*40,y=(P.y>>8)-10;fx_add(x,y,FXK_FLASH);fx_add(x,y,FXK_BIG);sfx_boom();
    for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(e->alive&&e->k<10&&idist((e->x>>8)-x,(e->y>>8)-y)<110)e->stun=200;}
    for(int cy=(y>>4)-2;cy<=(y>>4)+2;cy++)for(int cx=(x>>4)-3;cx<=(x>>4)+3;cx++)break_tile(cx,cy);P.abcd=480;ptoast("FLASHBANG - BREACH");}break;
  case AB_WALL:case AB_AMPHIB:P.dashT=14;P.abcd=110;sfx_whoosh();break;
  case AB_BREACH:{int x=(P.x>>8)+P.face*20,y=(P.y>>8)-10;for(int cy=(y>>4)-1;cy<=(y>>4)+1;cy++)for(int cx=(x>>4);cx!=(x>>4)+P.face*2;cx+=P.face)break_tile(cx,cy);
    blast(x,y,26,6,0);P.abcd=300;ptoast("BREACH CHARGE");}break;
  case AB_DIVER:pb_new(PK_GREN,0,P.x+P.face*(6<<8),P.y-(6<<8),P.face*0x80,0,70,8);P.abcd=200;ptoast("LIMPET MINE");sfx_menu();break;
  case AB_DEMO:if(!P.on){P.chute^=1;sfx_whoosh();}else{pb_new(PK_GREN,0,P.x+P.face*(8<<8),P.y-(8<<8),P.face*0x120,-0x180,100,10);P.abcd=240;ptoast("DEMO CHARGE - 2 SEC");sfx_menu();}break;
  case AB_RECON:P.abT=300;P.abcd=720;ptoast("STEALTH RECON");sfx_menu();break;
  case AB_S13:if(P.swim){P.dashT=18;P.abcd=60;sfx_whoosh();}else{pb_new(PK_GREN,0,P.x+P.face*(6<<8),P.y-(6<<8),P.face*0x80,0,70,8);P.abcd=200;ptoast("LIMPET MINE");sfx_menu();}break;
  case AB_RESCUE:if(!P.on){P.chute^=1;sfx_whoosh();}else if(P.hp<P.maxhp){P.hp++;P.abcd=700;sfx_ok();ptoast("CASEVAC KIT +1");}break;
  case AB_SPY:P.abT=240;P.abcd=600;ptoast("DISGUISED - DON'T SHOOT");sfx_menu();break;
  case AB_HEAVY:P.abT=300;P.abcd=600;ptoast("HEAVY MG PLANTED");sfx_menu();break;
  case AB_DRONE:{PB*b=pb_new(PK_FPV,0,P.x,P.y-(20<<8),P.face*0x200,-0x100,240,8);(void)b;P.abcd=360;sfx_whoosh();ptoast("FPV DRONE AWAY");}break;
  case AB_MELEE:{int x=(P.x>>8)+P.face*14,y=(P.y>>8)-8;fx_add(x,y,FXK_SPARK);fx_add(x+P.face*6,y-4,FXK_SPARK);
    for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(e->alive&&e->k<10&&iabs((e->x>>8)-x)<18&&iabs((e->y>>8)-8-y)<16)ent_dmg(e,4);}
    if(boss_hit(x,y))boss_dmg(3);P.abcd=18;sfx1(0x08,0xA,1,2,1500,6);}break;
  case AB_FOCUS:pSlow=240;P.abcd=900;ptoast("FOCUS");break;
  case AB_BARRAGE:for(int k=-1;k<=1;k++)pb_new(PK_GREN,0,P.x,P.y-(10<<8),P.face*(0x200+k*0x0C0)+k*0x40,-0x380-k*0x40,80,4);P.abcd=480;sfx_whoosh();break;}}
static void pl_update(void){
 if(P.dead){P.deadT++;P.vy+=60;P.y+=P.vy;return;}
 int u=PS.unit;
 if(P.inv>0)P.inv--;if(P.cd>0)P.cd--;if(P.gcd>0)P.gcd--;if(P.abcd>0)P.abcd--;if(P.abT>0)P.abT--;if(P.rapidT>0)P.rapidT--;if(P.jetT>0)P.jetT--;if(P.dashT>0)P.dashT--;if(pSlow>0)pSlow--;
 if(u==AB_AIR&&P.abT==1){for(int k=0;k<6;k++)pb_new(PK_BOMB,0,(camX+30+k*36)<<8,(camY-8-k*10)<<8,0x40,0x100,200,5);}
 int left=keys&K_LEFT,right=keys&K_RIGHT,down=keys&K_DOWN,up=keys&K_UP,jp=pressed&K_A,jh=keys&K_A;
 int lowg=pFl&SF_L,zg=pFl&SF_Z,ice=pFl&SF_I;
 if(pFl&SF_I)ice=1;
 // platform carry
 if(P.stand>0){PEnt*e=&PE[P.stand-1];if(e->alive){P.x+=e->vx;P.y=e->y;}else P.stand=0;}
 int cx=P.x>>12,cyb=((P.y>>8)-8)>>4;
 int midT=ptile(cx,cyb),midF=TFL[midT];
 int tideY=(PH*16)-pTide;
 P.swim=(midF&F_WAT)||((pFl&SF_R)&&(P.y>>8)-6>tideY);
 if(!P.swim){P.air=600;}
 else if(!is_swimmer(u)&&(TFL[ptile(cx,((P.y>>8)-14)>>4)]&F_WAT||((pFl&SF_R)&&(P.y>>8)-14>tideY))){if(--P.air<=0){P.air=120;pl_hurt(1);}}
 if((midF&F_CLB)&&(up||down)&&!P.swim){P.climb=1;P.chute=0;}
 if(P.climb&&!(midF&F_CLB)&&!(TFL[ptile(cx,(P.y>>8)>>4)]&F_CLB))P.climb=0;
 P.crouch=down&&P.on&&!P.climb&&!P.swim;
 int maxv=0x1C0;if(u==AB_SPRINT&&P.abT>0)maxv=0x2E0;if(P.swim)maxv=is_swimmer(u)?0x1A0:0xE0;if(P.crouch||(u==AB_HEAVY&&P.abT>0))maxv=0;
 int acc=P.on?(ice?0x0C:0x30):0x22,fr=P.on?(ice?0x05:0x30):0x0A;
 if(P.hookT>0){P.hookT--;int dx=P.hx-(P.x>>8),dy=P.hy-(P.y>>8);int d=idist(dx,dy);if(d<10){P.hookT=0;P.vy=-0x200;}else{P.vx=dx*0x600/d;P.vy=dy*0x600/d;}}
 else if(P.dashT>0){P.vx=P.face*0x420;}
 else{
  if(right&&!P.crouch){P.vx+=acc;P.face=1;}else if(left&&!P.crouch){P.vx-=acc;P.face=-1;}
  else{if(P.vx>0){P.vx-=fr;if(P.vx<0)P.vx=0;}else if(P.vx<0){P.vx+=fr;if(P.vx>0)P.vx=0;}}
  if(right&&P.crouch)P.face=1;if(left&&P.crouch)P.face=-1;
  if(P.vx>maxv)P.vx-=ice?0x08:0x40;if(P.vx<-maxv)P.vx+=ice?0x08:0x40;if(iabs(P.vx)<0x40&&maxv==0)P.vx=0;
  if(pWindOn&&((pWindT%210)<130))P.vx+=pWindDir*14;}
 if(P.on){int ft=ptile(P.x>>12,(P.y>>8)>>4);if(ft==T_CONVL)P.x-=0xC0;else if(ft==T_CONVR)P.x+=0xC0;}
 // vertical
 if(P.climb){int sp=(u==AB_SPRINT?0x240:0x120);P.vy=up?-sp:down?sp:0;P.vx=(left||right)?P.vx/2:0;if(jp){P.climb=0;P.vy=-0x400;}}
 else if(P.hookT>0){}
 else if(P.swim){P.vy+=18;if(P.vy>0x100)P.vy=0x100;if(jp)P.vy=-0x230;if(up)P.vy-=12;if(down)P.vy+=14;P.chute=0;P.jumps=0;}
 else{
  if(jp){
   if(P.on||P.wallT>0){P.vy=(u==AB_HIJUMP)?-0x600:-0x540;P.jumps=1;P.on=0;P.stand=0;P.chute=0;sfx1(0x16,0x7,1,2,1500,6);}
   else if(u==AB_MELEE&&P.jumps<2){P.vy=-0x4A0;P.jumps=2;fx_add(P.x>>8,P.y>>8,FXK_SMOKE);}
   else if(u==AB_WALL&&(box_hit(P.x+0x300,P.y,5,14)||box_hit(P.x-0x300,P.y,5,14))){int dir=box_hit(P.x+0x300,P.y,5,14)?-1:1;P.vy=-0x4C0;P.vx=dir*0x240;P.face=dir;sfx1(0x16,0x7,1,2,1500,6);}
  }
  int gh=46,gn=108,mf=0x600;if(lowg){gh=18;gn=44;mf=0x380;}if(zg){gh=8;gn=16;mf=0x160;}
  P.vy+=(P.vy<0&&jh)?gh:gn;
  if(zg&&jh&&!P.on){P.vy-=22;if(P.vy<-0x220)P.vy=-0x220;}
  if(P.jetT>0&&jh){P.vy-=150;if(P.vy<-0x230)P.vy=-0x230;if(frame&1)fx_add((P.x>>8)-P.face*4,(P.y>>8)-2,FXK_SPARK);}
  if(P.chute){mf=u==AB_BREACH?0x260:0x90;if(P.on){P.chute=0;if(u==AB_BREACH)fx_add(P.x>>8,P.y>>8,FXK_SMOKE);}}
  if(u==AB_GLIDE&&jh&&P.vy>0&&!P.on)mf=0xA0;
  if(P.vy>mf)P.vy=mf;}
 if(P.wallT>0)P.wallT--;
 int hh=P.crouch?9:14;
 // move X
 {s32 dx=P.vx;while(dx){s32 st=dx>0?(dx>256?256:dx):(dx<-256?-256:dx);if(box_hit(P.x+st,P.y,5,hh)){if(u==AB_WALL&&!P.on)P.wallT=6;P.vx=0;break;}P.x+=st;dx-=st;}}
 // move Y
 {s32 dy=P.vy;int wasOn=P.on;P.on=0;int pst=P.stand;P.stand=0;
  if(dy>=0){if(dy==0)dy=1;while(dy>0){s32 st=dy>256?256:dy;int oldf=P.y>>8;s32 ny=P.y+st;int nf=ny>>8;
    int land=stand_on((P.x>>8)-4,nf,oldf)||stand_on((P.x>>8)+4,nf,oldf);
    int pl=0;if(!land&&!P.climb)pl=plat_under(P.x>>8,nf,oldf);
    if(land){P.y=(nf&~15)<<8;P.vy=0;P.on=1;P.jumps=0;break;}
    if(pl){P.y=PE[pl-1].y;P.vy=0;P.on=1;P.stand=pl;P.jumps=0;break;}
    P.y=ny;dy-=st;}
   if(!P.on&&wasOn&&pst&&P.vy>=0){}
  }else{while(dy<0){s32 st=dy<-256?-256:dy;s32 ny=P.y+st;int hy=(ny>>8)-hh;
    int t1=ptile(((P.x>>8)-4)>>4,hy>>4),t2=ptile(((P.x>>8)+4)>>4,hy>>4);
    int c=(TFL[t1]&F_SOL)||(TFL[t2]&F_SOL)||t1==T_HIDDEN||t2==T_HIDDEN;
    if(c){int bx=((TFL[t1]&F_SOL)||t1==T_HIDDEN)?((P.x>>8)-4)>>4:((P.x>>8)+4)>>4;int t=ptile(bx,hy>>4);
     if(t==T_HIDDEN){pm[PMI(bx,hy>>4)]=T_CRATE;}
     bump_tile(bx,hy>>4);P.vy=0;break;}
    P.y=ny;dy-=st;}}}
 // hazards and specials
 {int fx_=P.x>>8,fy=P.y>>8;int ft=ptile(fx_>>4,fy>>4),bt=ptile(fx_>>4,(fy-6)>>4);
  if((TFL[bt]|TFL[ptile(fx_>>4,(fy-1)>>4)])&F_KILL)pl_die(0);
  else if(TFL[bt]&F_HAZ||(P.on&&0))pl_hurt(1);
  if(P.on&&!P.stand&&(TFL[ptile(fx_>>4,fy>>4)]&F_HAZ)){if(TFL[ptile(fx_>>4,fy>>4)]&F_KILL)pl_die(0);else{pl_hurt(1);P.vy=-0x420;}}
  if(P.on&&ft==T_SPRING){P.vy=jh?-0x980:-0x7A0;P.on=0;sfx1(0x17,0xA,1,2,1300,10);}
  if(P.on&&ft==T_CRUMB){int cxx=fx_>>4,cyy=fy>>4,f=0;for(int i=0;i<NCR;i++)if(CR[i].t&&CR[i].x==cxx&&CR[i].y==cyy)f=1;
   if(!f)for(int i=0;i<NCR;i++)if(!CR[i].t){CR[i].x=cxx;CR[i].y=cyy;CR[i].t=1;break;}}
  if(P.on&&ft==T_DOOR){}
  if(u==AB_GRAPPLE||u==AB_SPY){int dt=ptile((fx_+P.face*8)>>4,(fy-8)>>4);if(dt==T_DOOR&&(left||right)){if(++P.abT>24){pm[PMI((fx_+P.face*8)>>4,(fy-8)>>4)]=T_AIR;ptoast("DOOR HACKED");sfx_ok();P.abT=0;}}}
  if(fy>PH*16+40)pl_die(1);}
 for(int i=0;i<NCR;i++)if(CR[i].t){if(++CR[i].t>26){if(ptile(CR[i].x,CR[i].y)==T_CRUMB){pm[PMI(CR[i].x,CR[i].y)]=T_AIR;fx_add(CR[i].x*16+8,CR[i].y*16+4,FXK_SMOKE);}CR[i].t=0;}}
 if(P.x<(8<<8))P.x=8<<8;if(P.x>((pW*16-8)<<8))P.x=(pW*16-8)<<8;
 if(pAuto&&!pBossOn){int mn=(camX+10)<<8;if(P.x<mn){P.x=mn;if(box_hit(P.x,P.y,5,hh))pl_die(1);}int mx=(camX+232)<<8;if(P.x>mx)P.x=mx;}
 if(pBossOn){int mn=(pArenaX+8)<<8;if(P.x<mn)P.x=mn;}
 // weapons
 int rate=WPN_RATE[PF_UW[u]];int dmg=WPN_DMG[PF_UW[u]];if(u==AB_RECON&&P.abT>0)dmg*=2;int k=PK_BUL;
 if(P.spaceW==1){rate=6;dmg=2;k=PK_PLASMA;}else if(P.spaceW==2){rate=14;dmg=4;}
 if(P.rapidT>0||((u==AB_AMPHIB||u==AB_DIVER)&&pRaft>=0&&P.stand==pRaft+1))rate=(rate+1)/2;
 if(u==AB_HEAVY&&P.abT>0){rate=(rate+1)/2;dmg++;}
 if((keys&K_B)&&P.cd<=0&&!P.climb){P.cd=rate;int upA=up&&!P.swim;
  s32 bx=P.x+(upA?0:P.face*(9<<8)),by=P.y-((P.crouch?6:9)<<8)-(upA?(6<<8):0);
  s32 vx=upA?0:P.face*0x700,vy=upA?-0x700:0;
  if(PF_UW[u]==2&&!upA)vy=((frame*37)%5-2)*0x20;
  PB*b=pb_new(k,0,bx,by,vx,vy,40,dmg);if(b&&(PF_UW[u]==4||P.spaceW==2))b->pierce=1;
  if(u==AB_SPY&&P.abT>0){P.abT=0;ptoast("COVER BLOWN");}
  if(PF_UW[u]!=3&&PF_UW[u]!=8)fx_add((bx>>8)+P.face*2,by>>8,FXK_FLASH);sfx_shot(PF_UW[u]==4);}
 if((pressed&K_L)&&P.gren>0&&P.gcd<=0){P.gren--;P.gcd=30;pb_new(PK_GREN,0,P.x,P.y-(10<<8),P.face*0x260+P.vx/2,-0x380,90,4);sfx_whoosh();}
 if(pressed&K_R)pl_ability();
 P.anim++;}
// ---------------------------------------------------------------- projectiles
static void pb_update(void){
 for(int i=0;i<NPB;i++){PB*b=&PBs[i];if(b->life<=0)continue;
  if(pSlow&&b->team==1&&(frame&1))continue;
  b->life--;
  if(b->k==PK_GREN||b->k==PK_ARC||b->k==PK_ESHELL&&0){b->vy+=40;}
  if(b->k==PK_BOMB)b->vy+=24;
  if(b->k==PK_FPV){int best=-1,bd=220;int bx=b->x>>8,by=b->y>>8;
   for(int j=0;j<nPE;j++){PEnt*e=&PE[j];if(!e->alive||e->k>9)continue;int d=idist((e->x>>8)-bx,(e->y>>8)-8-by);if(d<bd){bd=d;best=j;}}
   int tx=-1,ty=0;if(best>=0){tx=PE[best].x>>8;ty=(PE[best].y>>8)-8;}if(pBossOn&&B.hp>0){tx=B.x>>8;ty=(B.y>>8)-16;}
   if(tx>=0){int dx=tx-bx,dy=ty-by,d=idist(dx,dy);if(d>0){b->vx+=(dx*40/d)-b->vx/24;b->vy+=(dy*40/d)-b->vy/24;}}}
  b->x+=b->vx;b->y+=b->vy;int x=b->x>>8,y=b->y>>8;
  if(x<camX-40||x>camX+280||y>PH*16+20||y<-80){b->life=0;continue;}
  int f=pflag(x,y);
  if(f&F_SOL){if(b->k==PK_ROCKET||b->k==PK_GREN||b->k==PK_BOMB||b->k==PK_FPV||b->k==PK_ARC||b->k==PK_ESHELL)blast(x,y,b->k==PK_ROCKET?30:24,b->dmg,b->team);
   else fx_add(x,y,FXK_SPARK);b->life=0;continue;}
  if(b->team==0){
   for(int j=0;j<nPE&&b->life>0;j++){PEnt*e=&PE[j];if(!e->alive)continue;
    int hw=e->k>=6&&e->k<=7?14:7,ht=e->k>=6&&e->k<=7?14:15;
    if(e->k==28||e->k==29||e->k>=10)continue;
    if(iabs((e->x>>8)-x)<hw&&y<(e->y>>8)+1&&y>(e->y>>8)-ht){
     if(b->k==PK_ROCKET||b->k==PK_GREN||b->k==PK_BOMB||b->k==PK_FPV){blast(x,y,30,b->dmg,0);b->life=0;break;}
     ent_dmg(e,(e->k==7&&b->k==PK_BUL)?(b->dmg+1)/2:b->dmg);fx_add(x,y,FXK_SPARK);if(!b->pierce)b->life=0;}}
   for(int j=0;j<NPB&&b->life>0;j++){PB*s=&PBs[j];if(s->life>0&&s->team==1&&s->k==PK_ESHELL&&iabs((s->x>>8)-x)<8&&iabs((s->y>>8)-y)<6){s->life=0;fx_add(x,y,FXK_SMALL);pTags++;if(!b->pierce)b->life=0;}}
   for(int j=0;j<nPE&&b->life>0;j++){PEnt*e=&PE[j];if(e->alive&&e->k==29&&!e->st&&iabs((e->x>>8)-x)<8&&iabs((e->y>>8)-6-y)<8){e->st=1;pSwOn=600;sfx_ok();b->life=0;}}
   if(b->life>0&&boss_hit(x,y)){if(b->k==PK_ROCKET||b->k==PK_GREN||b->k==PK_BOMB||b->k==PK_FPV)blast(x,y,30,b->dmg,0);else{boss_dmg(b->dmg);fx_add(x,y,FXK_SPARK);}b->life=0;}
   if(b->life==0&&b->k==PK_GREN){}
   if(b->life==1&&(b->k==PK_GREN||b->k==PK_FPV||b->k==PK_ROCKET))blast(x,y,28,b->dmg,0);
  }else{
   if(!P.dead&&iabs((P.x>>8)-x)<6&&y<(P.y>>8)&&y>(P.y>>8)-(P.crouch?9:14)){if(b->k==PK_ARC||b->k==PK_ESHELL||b->k==PK_ROCKET||b->k==PK_BOMB)blast(x,y,20,1,1);else pl_hurt(1);b->life=0;}
   if(b->life==1&&(b->k==PK_ARC||b->k==PK_BOMB))blast(x,y,20,1,1);}}}
// ---------------------------------------------------------------- enemies & obstacles
static void e_fire(PEnt*e,int k,int spd,int life){int dx=(P.x>>8)-(e->x>>8),dy=((P.y>>8)-8)-((e->y>>8)-9);int dir=dx<0?-1:1;int adx=iabs(dx)<8?8:iabs(dx);
 s32 vy=dy*spd/adx;if(vy>spd/2)vy=spd/2;if(vy<-spd/2)vy=-spd/2;int bj=BOTD[SET.bot].jit;vy+=((int)(rnd()%(bj*2+1))-bj)*6;
 pb_new(k,1,e->x+dir*(8<<8),e->y-(9<<8),dir*spd,k==PK_ARC?-0x380:vy,life,1);}
static int tri(int t,int n){if(n<=0)return 0;t%=2*n;return t<n?t:2*n-t;}
static void ents_update(void){int px=P.x>>8,py=P.y>>8;int stealth=(PS.unit==AB_STEALTH||PS.unit==AB_RECON||PS.unit==AB_SPY)&&P.abT>0;
 for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(!e->alive)continue;int ex=e->x>>8,ey=e->y>>8;int dx=px-ex;
  if(iabs(ex-(camX+120))>300&&e->k!=25&&e->k!=26)continue;
  if(e->c>0)e->c--;
  if(e->k<=7){if(pSlow&&(frame&1))continue;if(e->stun>0){e->stun--;continue;}
   e->face=dx<0?-1:1;e->t++;
   int range=(PF_UW[PS.unit]==3||PF_UW[PS.unit]==8||(PS.unit==AB_SNIPER&&(PF_THSTY[pTheme]==3||PF_THSTY[pTheme]==7)))?120:170;
   int vis=iabs(dx)<range&&iabs(py-ey)<72&&!stealth&&!P.dead;
   if(e->cd>0)e->cd--;
   switch(e->k){
    case 1:if(!vis||e->cd>40){int nx=ex+(e->a?1:-1);int ahead=nx+(e->a?6:-6);if((TFL[ptile(ahead>>4,(ey+4)>>4)]&(F_SOL|F_ONE))&&!(TFL[ptile(ahead>>4,(ey-8)>>4)]&F_SOL)&&(e->t&1))e->x=nx<<8;else if(e->t&1)e->a^=1;}
     if(vis&&e->cd<=0){e_fire(e,PK_EBUL,0x280,90);e->cd=BDcd(100);}break;
    case 2:if(vis&&e->cd<=0){e_fire(e,PK_EBUL,0x300,80);if(++e->b>=4){e->b=0;e->cd=BDcd(130);}else e->cd=7;}break;
    case 3:if(vis&&e->cd<=0){int d=dx<0?-1:1;pb_new(PK_ROCKET,1,e->x+d*(8<<8),e->y-(11<<8),d*0x1C0,0,160,1);e->cd=BDcd(170);sfx_shot(1);}break;
    case 4:if(vis&&e->cd<=0){if(!e->b){e->b=45;}}if(e->b>0){if(--e->b==0){e_fire(e,PK_EBUL,0x600,60);e->cd=BDcd(150);sfx_shot(1);}}break;
    case 5:{int tx=px-e->face*0,ty=py-60+(SIN(e->t*3)>>5);e->x+=clampi((tx-ex)*8,-0x120,0x120);e->y+=clampi((ty-ey)*8,-0x100,0x100);
     if(vis&&e->cd<=0&&iabs(dx)<14){pb_new(PK_BOMB,1,e->x,e->y,0,0x80,120,1);e->cd=BDcd(110);}}break;
    case 6:{if(!e->b)e->b=1;int nx=ex+e->b;if(iabs(nx-e->x0*16)>56||(TFL[ptile((nx+e->b*16)>>4,(ey-4)>>4)]&F_SOL))e->b=-e->b;else if(e->t&1)e->x=nx<<8;
     if(vis&&e->cd<=0){e_fire(e,PK_EBUL,0x300,80);if(++e->a>=5){e->a=0;e->cd=BDcd(120);}else e->cd=6;}}break;
    case 7:{if(!e->b)e->b=1;if((e->t&3)==0){int nx=ex+e->b;if(iabs(nx-e->x0*16)>40)e->b=-e->b;else e->x=nx<<8;}
     if(vis&&e->cd<=0){if(++e->a&1){int d=dx<0?-1:1;pb_new(PK_ESHELL,1,e->x+d*(14<<8),e->y-(12<<8),d*0x280,0,140,1);sfx_shot(1);e->cd=BDcd(140);}else{e_fire(e,PK_EBUL,0x300,80);e->cd=BDcd(50);}}}break;}
   int hw=e->k>=6?14:6;if(!P.dead&&iabs(dx)<hw+4&&py>ey-16&&py-14<ey){if(P.dashT>0){ent_dmg(e,3);}else if(!(PS.unit==AB_SPY&&P.abT>0))pl_hurt(1);}
   continue;}
  switch(e->k){
   case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:if(e->st==2)break;
    if(!P.dead&&iabs(dx)<11&&iabs((py-8)-(ey-8))<13){e->alive=0;sfx_ok();
     switch(e->k){case 10:pIntel|=1<<e->p;{int n=(pIntel&1)+(pIntel>>1&1)+(pIntel>>2&1);ptoast(cat3("INTEL FILE ",itoa(n),"/3"));}break;
      case 11:pTags++;break;case 12:P.gren+=3;if(P.gren>12)P.gren=12;ptoast("GRENADES +3");break;
      case 13:P.hp+=PS.unit==AB_MEDIC?2:1;if(P.hp>P.maxhp){if(P.maxhp<5)P.maxhp++;P.hp=P.maxhp;}ptoast(PS.unit==AB_MEDIC?"MEDKIT +2":"MEDKIT");break;
      case 14:P.armor=2;ptoast("BODY ARMOR");break;case 15:P.rapidT=600;ptoast("RAPID FIRE");break;case 16:P.jetT=480;ptoast("JETPACK - HOLD A");break;
      case 17:P.lives++;ptoast("1UP");break;case 18:P.spaceW=P.spaceW==1?2:1;ptoast(P.spaceW==1?"PLASMA RIFLE":"COILGUN");break;}}break;
   case 20:if(!e->st&&dx>-6&&iabs(py-ey)<40){e->st=1;pChkX=ex;ptoast("KC: WOOF! CHECKPOINT!");sfx_ok();}break;
   case 21:case 22:if(!P.dead&&iabs(dx)<10&&py>ey-40&&py<=ey+2&&!pDone){pDone=1;if(e->k==22)pSecret=1;e->st=1;music_jingle(TR_CLEAR);}break;
   case 24:{e->a+=(e->p&16)?-2:2;int n=e->p&15;for(int k=1;k<=n;k++){int bx=ex+(COS(e->a)*k*8>>8),by=ey-8+(SIN(e->a)*k*8>>8);if(!P.dead&&iabs(px-bx)<7&&iabs(py-7-by)<8)pl_hurt(1);}}break;
   case 25:{int nx=e->x0*16+tri(pT,e->p*16+16);s32 old=e->x;e->x=nx<<8;e->vx=e->x-old;e->y=(e->y0*16)<<8;}break;
   case 26:{int ny=e->y0*16-tri(pT/2,e->p*16);e->x=(e->x0*16-8)<<8;s32 old=e->y;e->y=ny<<8;e->vx=0;(void)old;}break;
   case 27:{int top=e->y0*16;switch(e->st){
     case 0:if(iabs(dx)<26&&py>ey)e->st=1,e->t=0;break;
     case 1:if(++e->t>14){e->st=2;e->vy=0;}break;
     case 2:e->vy+=0x60;if(e->vy>0x700)e->vy=0x700;e->y+=e->vy;if(TFL[ptile(ex>>4,((e->y>>8)+32)>>4)]&(F_SOL)){e->y=((((e->y>>8)+32)&~15)-32)<<8;e->st=3;e->t=0;shake(5);sfx_boom();}break;
     case 3:if(++e->t>40)e->st=4;break;
     case 4:e->y-=0x100;if((e->y>>8)<=top){e->y=top<<8;e->st=0;}break;}
    int cy=e->y>>8;if(!P.dead&&iabs(dx)<12&&py>cy+4&&py-14<cy+32){if(e->st==2)pl_hurt(1),P.x+=(dx<0?-6:6)<<8;}}break;
   case 28:if(e->cd>0)e->cd--;else if(iabs(dx)>28&&iabs(dx)<210){int d=dx<0?-1:1;pb_new(PK_ESHELL,1,e->x+d*(8<<8),e->y-(8<<8),d*0x180,0,320,1);e->cd=BDcd(170);sfx_shot(1);fx_add(ex+d*8,ey-8,FXK_SMOKE);}break;
   case 29:if(!e->st&&!P.dead&&iabs(dx)<10&&py>ey-6&&py<ey+2&&P.vy>=0){e->st=1;pSwOn=600;sfx_ok();ptoast("SWITCH!");}if(e->st&&pSwOn<=0)e->st=0;break;
   case 30:if(!P.dead&&P.on&&(pressed&K_DOWN)&&px>=e->x0*16&&px<e->x0*16+32&&iabs(py-(e->y0*16+16))<6){
     for(int j=0;j<nPE;j++)if(PE[j].alive&&PE[j].k==31&&PE[j].p==e->p){P.x=(PE[j].x0*16+8)<<8;P.y=(PE[j].y0*16+16)<<8;P.vx=P.vy=0;sfx_whoosh();camX=-999;break;}}break;
   case 32:if(px>=e->x0*16&&px<e->x0*16+8){pWindOn=e->p?1:0;pWindDir=e->p==2?1:-1;if(pWindOn&&!e->st)ptoast("CROSSWIND!");e->st=1;}break;
   case 34:if(!pBossOn&&px>ex+8&&!P.dead){pBossOn=1;pArenaX=e->x0*16-16;boss_start();}break;
   case 35:{s32 old=e->x;if(pAuto)e->x=((camX+40)<<8);e->vx=e->x-old;e->y=(9*16+2+((SIN(pT*4))>>7))<<8;}break;}}
 if(pSwOn>0){pSwOn--;int on=pSwOn>0;for(int x=(camX>>4)-20;x<(camX>>4)+40;x++)for(int y=0;y<PH;y++){if(x<0||x>=pW)continue;u8*t=&pm[PMI(x,y)];if(on&&*t==T_SWOFF)*t=T_SWON;else if(!on&&*t==T_SWON)*t=T_SWOFF;}
  if(pSwOn==1){for(int x=0;x<pW;x++)for(int y=0;y<PH;y++)if(pm[PMI(x,y)]==T_SWON)pm[PMI(x,y)]=T_SWOFF;}}
 if(pFl&SF_R){pTide=48+((SIN(pT/3)+256)*40>>9);int ty=PH*16-pTide;for(int r=6;r<PH;r++){int on=r*16+8>ty;for(int x=(camX>>4)-2;x<(camX>>4)+18;x++){if(x<0||x>=pW)continue;u8*t=&pm[PMI(x,r)];int m=tideM[r-6][x>>3]>>(x&7)&1;
   if(on&&*t==T_AIR){*t=(r*16<ty+8)?T_WTOP:T_WATER;tideM[r-6][x>>3]|=1<<(x&7);}else if(m&&!on){*t=T_AIR;tideM[r-6][x>>3]&=~(1<<(x&7));}else if(m&&on)*t=(r*16<ty+8)?T_WTOP:T_WATER;}}}
 if(pWindOn)pWindT++;}
// ---------------------------------------------------------------- bosses
static const char*const BKN[BK_NONE+1]={"HEAVY TANK","GUNSHIP","ARMORED TRAIN","ARMORED TRAIN","RIVER GUNBOAT","COMMAND WALKER","FORTRESS CANNON","MI-8 GUNSHIP",
 "PMC COMMANDER 'GRANIT'","T-55 TANK","MI-24 HIND","AA EMPLACEMENT","TIGER TANK","BUNKER COMPLEX","ZERO SQUADRON","ME 262","BROOD QUEEN","MOTHERSHIP CORE",
 "CARTEL ARMORED TRUCK","HEAVY TECHNICAL","KPA TANK","T-90 TANK","TYPE 99 TANK","Z-10 ATTACK HELO","MERC CONVOY"};
_Static_assert(sizeof BKN/sizeof BKN[0]==BK_NONE+1,"boss names");
static int pPlaneX=-1,pPlaneT;
static const char*boss_name(void){const PfStage*S=&PF_ST[pw][ps_];if(B.ai==BA_MI8&&B.ph==1)return BKN[BK_CMDR];return BKN[S->ba==BK_NONE?BK_NONE:S->ba];}
static void boss_start(void){const PfStage*S=&PF_ST[pw][ps_];bgm_t(TR_BOSS);memset(&B,0,sizeof B);B.ai=S->ai;int st=S->stars;
 B.max=B.hp=BDhp(24+st*8);B.x=(pArenaX+17*16)<<8;B.y=(9*16)<<8;B.face=-1;pPlaneX=-1;pPlaneT=0;
 switch(B.ai){case BA_HELI:case BA_MI8:B.y=72<<8;break;case BA_MOTHER:B.y=64<<8;B.x=(pArenaX+12*16)<<8;break;
  case BA_FORT:case BA_FORTFLY:B.x=(pArenaX+22*16-34)<<8;break;
  case BA_TRAIN:B.x=(pArenaX+20*16)<<8;break;
  case BA_CONVOY:B.n=0;B.max=B.hp=BDhp(14)*2+BDhp(26)*(st>=4?2:1);break;
  case BA_BOAT:B.x=(pArenaX+18*16)<<8;for(int x=(pArenaX>>4)+13;x<(pArenaX>>4)+22;x++){pm[PMI(x,9)]=T_WTOP;pm[PMI(x,10)]=T_WATER;pm[PMI(x,11)]=T_WATER;}
   for(int x=(pArenaX>>4)+5;x<(pArenaX>>4)+13;x+=4){pm[PMI(x,6)]=T_PLAT;pm[PMI(x+1,6)]=T_PLAT;}B.y=(9*16+6)<<8;break;}
 ptoast(boss_name());sfx_boom();shake(10);}
static void boss_dmg(int d){if(B.hp<=0||B.ph==9)return;if(B.ai==BA_MOTHER&&(B.t%300)<170){fx_add(B.x>>8,(B.y>>8)-16,FXK_SPARK);return;}
 if(B.ai==BA_CONVOY)return;B.hp-=d;B.flash=4;if(B.hp<=0){if(B.ai==BA_MI8&&B.ph==0){B.ph=1;B.max=B.hp=BDhp(40);B.y=(9*16)<<8;B.x=(pArenaX+18*16)<<8;B.t=0;fx_add(B.x>>8,60,FXK_BIG);ptoast(BKN[BK_CMDR]);sfx_boom();}
  else{B.ph=9;B.t=0;}}}
static void arc_at(int x0,int y0,int tx,int k){int T=44;s32 vx=((tx-x0)<<8)/T;if(vx>0x300)vx=0x300;if(vx<-0x300)vx=-0x300;pb_new(k,1,x0<<8,y0<<8,vx,-0x380,160,1);}
static void aim_at(int x0,int y0,int spd,int spread){int dx=(P.x>>8)-x0,dy=((P.y>>8)-8)-y0;int d=idist(dx,dy);if(d<1)d=1;pb_new(PK_EBUL,1,x0<<8,y0<<8,dx*spd/d+spread,dy*spd/d,120,1);}
static void boss_update(void){int bx=B.x>>8,by=B.y>>8,px=P.x>>8;int dx=px-bx;B.t++;if(B.flash>0)B.flash--;if(B.cd>0)B.cd--;if(B.cd2>0)B.cd2--;B.fr=(B.t>>3)&1;
 if(pSlow&&(frame&1))return;
 const PfStage*S=&PF_ST[pw][ps_];int st=S->stars;int ang=B.hp*2<B.max;
 if(B.ph==9){if(B.t%8==0){fx_add(bx-30+(int)(rnd()%60),by-30+(int)(rnd()%28),FXK_BIG);sfx_boom();shake(6);}if(B.t>100){pDone=1;music_jingle(TR_CLEAR);B.ph=10;}return;}
 if(B.ph==10)return;
 int ax0=pArenaX+16,ax1=pArenaX+22*16-16;
 switch(B.ai){
  case BA_TANK:B.face=dx<0?-1:1;{int tx=px-B.face*96;tx=clampi(tx,ax0+32,ax1-32);if(iabs(tx-bx)>6)B.x+=(tx>bx?1:-1)*(ang?0x90:0x60);}
   if(B.cd<=0){arc_at(bx+B.face*30,by-22,px,PK_ARC);B.cd=BDcd(ang?70:95);sfx_shot(1);fx_add(bx+B.face*32,by-22,FXK_SMOKE);}
   if(B.cd2<=0&&iabs((P.y>>8)-by)<24){aim_at(bx+B.face*20,by-14,0x300,0);B.n++;B.cd2=B.n%4?8:BDcd(110);}break;
  case BA_HELI:case BA_MI8:
   if(B.ai==BA_MI8&&B.ph==1){B.face=dx<0?-1:1;if(iabs(dx)>50)B.x+=B.face*(ang?0xC0:0x80);B.vy+=60;B.y+=B.vy;if(B.y>(9*16)<<8){B.y=(9*16)<<8;B.vy=0;}
    if(B.t%200==100&&B.vy==0)B.vy=-0x500;
    if(B.cd<=0){aim_at(bx+B.face*14,by-16,0x320,0);if(++B.n%4)B.cd=7;else B.cd=BDcd(100);}
    if(B.cd2<=0){arc_at(bx,by-26,px,PK_ARC);B.cd2=BDcd(150);}break;}
   B.x=(((ax0+ax1)/2)+(SIN(B.t/2)*110>>8))<<8;B.y=(54+(SIN(B.t*2)*10>>8))<<8;B.face=dx<0?-1:1;
   if(B.cd<=0){aim_at(bx+B.face*20,by-8,0x300,0);if(++B.n%3)B.cd=8;else B.cd=BDcd(ang?60:85);sfx_shot(0);}
   if(B.cd2<=0){if(B.ai==BA_MI8||S->ba==BK_HIND){for(int k=0;k<3;k++)pb_new(PK_ROCKET,1,(bx+B.face*10)<<8,(by)<<8,B.face*(0x180+k*0x40),0x140+k*0x30,140,1);}else pb_new(PK_BOMB,1,B.x,B.y,0,0x80,140,1);B.cd2=BDcd(ang?110:150);}break;
  case BA_TRAIN:B.x=((pArenaX+20*16)+(SIN(B.t)*10>>8))<<8;
   if(B.cd<=0){arc_at(bx-64,by-30,px,PK_ARC);B.cd=BDcd(ang?60:85);sfx_shot(1);}
   if(B.cd2<=0){aim_at(bx-20,by-26,0x300,0);if(++B.n%5)B.cd2=7;else B.cd2=BDcd(120);}break;
  case BA_BOAT:B.y=((9*16+6)+(SIN(B.t*3)*3>>8))<<8;
   if(B.cd<=0){arc_at(bx-20,by-26,px,PK_ARC);B.cd=BDcd(ang?55:80);sfx_shot(1);}
   if(B.cd2<=0){aim_at(bx-30,by-16,0x2C0,0);if(++B.n%4)B.cd2=8;else B.cd2=BDcd(120);}break;
  case BA_WALKER:B.face=dx<0?-1:1;B.vy+=50;B.y+=B.vy;B.x+=B.vx;if(B.y>=(9*16)<<8){if(B.vy>0x200){shake(8);sfx_boom();pb_new(PK_EBUL,1,(bx-20)<<8,(by-3)<<8,-0x200,0,60,1);pb_new(PK_EBUL,1,(bx+20)<<8,(by-3)<<8,0x200,0,60,1);}B.y=(9*16)<<8;B.vy=0;B.vx=0;}
   B.x=clampi(B.x>>8,ax0+30,ax1-30)<<8;
   if(B.vy==0&&B.t%180==90){B.vy=-0x600;B.vx=B.face*0x160;}
   if(B.cd<=0){pb_new(S->ba==BK_BROOD?PK_ARC:PK_ROCKET,1,(bx+B.face*28)<<8,(by-20)<<8,B.face*0x200,S->ba==BK_BROOD?-0x300:0,140,1);B.cd=BDcd(ang?60:90);}
   if(S->ba==BK_BROOD&&B.t%300==150){int n=0;for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].k==1)n++;if(n<3){PEnt*e=pe_new();if(e){e->k=1;e->hp=BDhp(2);e->x=B.x;e->y=B.y;e->x0=bx>>4;}}}break;
  case BA_FORT:case BA_FORTFLY:
   if(B.cd<=0){if(S->ba==BK_AAGUN){for(int k=-1;k<=1;k++)aim_at(bx-20,by-28,0x2C0,k*0x60);B.cd=BDcd(ang?55:75);}else{arc_at(bx-34,by-16,px,PK_ARC);B.cd=BDcd(ang?60:90);}sfx_shot(1);}
   if(B.t%360==200){int n=0;for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].k==1)n++;if(n<2){PEnt*e=pe_new();if(e){e->k=1;e->hp=BDhp(2);e->x=(bx-40)<<8;e->y=(9*16)<<8;e->x0=(bx-40)>>4;}}}
   if(B.ai==BA_FORTFLY){if(pPlaneX<0&&B.t%260==130)pPlaneX=camX+260;if(pPlaneX>=0){pPlaneX-=3;if((pPlaneX&15)==0&&pPlaneX>camX&&pPlaneX<camX+240)pb_new(PK_EBUL,1,pPlaneX<<8,30<<8,-0x80,0x300,80,1);if(pPlaneX<camX-70)pPlaneX=-1;}}break;
  case BA_CONVOY:{int alive=0,hp=0;for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].st==7){alive++;hp+=PE[i].hp;}
   int tot=st>=4?4:3;if(alive==0&&B.n<tot&&(B.t>40)){PEnt*e=pe_new();if(e){e->k=B.n<2?6:7;e->st=7;e->hp=e->k==6?BDhp(14):BDhp(26);e->x=((pArenaX+21*16))<<8;e->y=(9*16)<<8;e->x0=(pArenaX>>4)+16;e->b=-1;B.n++;ptoast(B.n<3?"TECHNICAL INBOUND":"BTR INBOUND");}}
   B.hp=hp+(tot-B.n)*BDhp(14);if(B.hp<1&&B.n>=tot){B.hp=0;B.ph=9;B.t=0;}}break;
  case BA_MOTHER:B.x=(((ax0+ax1)/2)+(SIN(B.t/3)*70>>8))<<8;B.y=(56+(SIN(B.t*2)*8>>8))<<8;
   if(B.cd<=0){for(int k=0;k<8;k++){int a=k*32+(B.t&31);pb_new(PK_EBUL,1,B.x,(by-16)<<8,COS(a)*0x200>>8,SIN(a)*0x200>>8,120,1);}B.cd=BDcd(ang?70:100);sfx_laser();}
   if(B.t%240==120){int n=0;for(int i=0;i<nPE;i++)if(PE[i].alive&&PE[i].k==5)n++;if(n<3){PEnt*e=pe_new();if(e){e->k=5;e->hp=BDhp(2);e->x=B.x;e->y=B.y;}}}break;}
 if(!P.dead&&B.ai!=BA_CONVOY&&boss_hit(px,(P.y>>8)-6))pl_hurt(1);}
// ---------------------------------------------------------------- draw
static void pl_tiles(void){int tx0=camX>>4,ty0=camY>>4;
 for(int cy=ty0;cy<ty0+11;cy++)for(int cx=tx0;cx<tx0+16;cx++){int t=ptile(cx,cy);if(cy>=PH)t=(pFl&(SF_A))?T_AIR:T_AIR;
  if(t==T_HIDDEN)t=P.nvg?T_GHOST:T_AIR;else if(t==T_INVIS)t=T_AIR;
  u16 b=(t*4)|(PF_MTB[t]<<12);int X=(cx*2)&31,Y=(cy*2)&31;bgw[Y*32+X]=b;bgw[Y*32+X+1]=b+1;bgw[(Y+1)*32+X]=b+2;bgw[(Y+1)*32+X+1]=b+3;}}
static void pl_draw(void){oam_begin();int u=PS.unit;
 // player
 if(!(P.inv>0&&(frame&2))||P.dead){int f=0;if(P.climb)f=5;else if(P.swim)f=6;else if(P.crouch)f=4;else if(!P.on)f=3;else if(iabs(P.vx)>0x60)f=1+((P.anim>>3)&1);
  int sx=(P.x>>8)-8-camX,sy=(P.y>>8)-16-camY;
  if(P.chute&&u==AB_BREACH){for(int k=1;k<8;k++)sprf(sx+4,sy-k*8,OT_PROJ+4,0,0,4,1,0);}
  else if(P.chute||(u==AB_GLIDE&&(keys&K_A)&&P.vy>0&&!P.on))sprf(sx,sy-14,OT_CHUTE,0,1,4,1,0);
  if(P.hookT>0){int hx=P.hx-camX,hy=P.hy-14-camY;for(int k=1;k<6;k++)sprf(sx+8+(hx-sx-8)*k/6-4,sy+6+(hy-sy-6)*k/6-4,OT_PROJ+4,0,0,4,1,0);}
  if((u==AB_STEALTH||u==AB_RECON)&&P.abT>0&&(frame&1)){}else sprf(sx,sy,OT_PL+f*4,0,1,(u==AB_SPY&&P.abT>0)?1:0,1,P.face<0);
  if(P.jetT>0)sprf(sx+(P.face<0?8:0),sy+4,OT_PICK+6*4,0,1,4,1,P.face<0);}
 // entities
 for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(!e->alive)continue;int sx=(e->x>>8)-camX,sy=(e->y>>8)-camY;if(sx<-40||sx>280)continue;
  int k=e->k;int fl=(e->c>0&&(frame&1));
  if(k<=7){int fr=(e->t>>4)&1;int pal=fl?3:1;
   if(k<=4)sprf(sx-8,sy-16,OT_EN+((k-1)*2+fr)*4,0,1,pal,1,e->face<0);
   else if(k==5)sprf(sx-8,sy-12,OT_EN+32+fr*4,0,1,pal,1,0);
   else sprf(sx-16,sy-16,OT_EN+40+(k-6)*16+fr*8,1,2,pal,1,e->face<0);
   if(e->stun>0&&(frame&8))sprf(sx-4,sy-24,OT_FX+FX_FLASH,0,0,3,0,0);
   if(k==4&&e->b>0&&(frame&2)){for(int q=1;q<5;q++)sprf(sx+(e->face*q*14)-4,sy-12,OT_PROJ+6,0,0,4,1,0);}
   continue;}
  switch(k){case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:if(e->st==2)break;
    sprf(sx-8,sy-16-((frame>>4)&1),OT_PICK+(k==18?(8+(P.spaceW==1)):(k-10))*4,0,1,4,1,0);break;
   case 20:sprf(sx-8,sy-32,OT_FLAG+(e->st?0:8),2,2,4,1,0);sprf(sx+6,sy-16,OT_KC+(e->st?((frame>>4)&1?16:8):8),0,1,5,1,1);break;
   case 21:sprf(sx-8,sy-32,OT_FLAG+(pDone?0:8),2,2,4,1,0);break;
   case 22:sprf(sx-8,sy-32,OT_FLAG+(pDone?0:8),2,2,3,1,0);break;
   case 24:{int n=e->p&15;for(int q=0;q<=n;q++)sprf(sx+(COS(e->a)*q*8>>8)-4,sy-8+(SIN(e->a)*q*8>>8)-4,OT_BALL,0,0,4,1,0);}break;
   case 25:case 26:sprf(sx,sy,OT_MPLAT,1,1,4,1,0);break;
   case 27:sprf(sx-8+(e->st==1?((frame&2)?1:-1):0),sy,OT_CRUSH,2,2,4,1,0);break;
   case 29:sprf(sx-8,sy-16+(e->st?6:0),OT_PSW,0,1,4,1,0);break;
   case 35:sprf(sx,sy,OT_RAFT,1,2,4,1,0);break;}}
 // projectiles
 for(int i=0;i<NPB;i++){PB*b=&PBs[i];if(b->life<=0)continue;int sx=(b->x>>8)-camX,sy=(b->y>>8)-camY;
  switch(b->k){case PK_BUL:sprf(sx-4,sy-4,OT_PROJ+0,0,0,4,1,0);break;case PK_EBUL:sprf(sx-4,sy-4,OT_PROJ+1,0,0,4,1,0);break;
   case PK_ROCKET:sprf(sx-4,sy-4,OT_PROJ+2,0,0,4,1,b->vx<0);break;case PK_GREN:sprf(sx-4,sy-4,OT_PROJ+3,0,0,4,1,0);break;
   case PK_ARC:sprf(sx-4,sy-4,OT_PROJ+4,0,0,4,1,0);break;case PK_BOMB:sprf(sx-4,sy-4,OT_PROJ+5,0,0,4,1,0);break;
   case PK_PLASMA:sprf(sx-4,sy-4,OT_PROJ+7,0,0,4,1,0);break;case PK_FPV:sprf(sx-8,sy-8,OT_FPVD,0,1,4,1,b->vx<0);break;
   case PK_ESHELL:sprf(sx-8,sy-4,OT_SHELLS,1,0,4,1,b->vx>0);break;}}
 // boss
 if(pBossOn&&B.ph<10){const PfStage*S=&PF_ST[pw][ps_];int sx=(B.x>>8)-camX,sy=(B.y>>8)-camY;int pal=B.flash&&(frame&1)?3:2;
  if(B.ai==BA_MOTHER&&(B.t%300)<170&&(frame&4))pal=3;
  if(B.ai==BA_MI8&&B.ph==1)sprf(sx-16,sy-32,OT_BB+B.fr*16,0,2,pal,1,B.face>0?0:1);
  else if(B.ai==BA_CONVOY){}
  else if(B.ai==BA_TRAIN){sprf(sx-32,sy-32,OT_BA+B.fr*32,1,3,pal,1,0);sprf(sx-96,sy-32,OT_BB,1,3,pal,1,0);}
  else sprf(sx-32,sy-32,OT_BA+B.fr*32,1,3,pal,1,(B.ai==BA_TANK||B.ai==BA_HELI||B.ai==BA_MI8||B.ai==BA_WALKER)&&B.face>0);
  if(B.ai==BA_FORTFLY&&pPlaneX>=0&&S->bb!=BK_NONE)sprf(pPlaneX-camX-32,14,OT_BB+((frame>>2)&1)*32,1,3,2,0,0);}
 fx_update_draw();}
static void pl_hud(void){hud_clear();
 hearts(0,0,P.maxhp,P.hp,P.maxhp);if(P.armor)htxt(P.maxhp,0,P.armor>1?"[]":"[",HB_BLUE);
 htxt(8,0,cat("x",itoa(P.lives)),HB_WHITE);htxt(12,0,cat("T",itoa(pTags)),HB_YEL);
 {char b[4];for(int i=0;i<3;i++)b[i]=(pIntel>>i&1)?'#':'-';b[3]=0;htxt(17,0,b,HB_GREEN);}
 htxt(21,0,cat("G",itoa(P.gren)),HB_WHITE);
 htxt(25,0,P.abcd>0?"R..":"R OK",P.abcd>0?HB_GRAY:HB_GREEN);
 if(P.rapidT>0)htxt(0,1,"RAPID",HB_YEL);if(P.jetT>0)htxt(6,1,"JET",HB_YEL);if(P.spaceW)htxt(10,1,P.spaceW==1?"PLASMA":"COIL",HB_BLUE);
 if(pWindOn&&((pWindT%210)<130))htxt(17,1,pWindDir<0?"<< WIND":"WIND >>",HB_WHITE);
 if(P.swim&&!is_swimmer(PS.unit)&&P.air<600){hbar(22,1,6,P.air,600);}
 if(pBossOn&&B.ph<9){htxt(1,18,boss_name(),HB_RED);hbar(1,19,14,B.hp,B.max);}
 if(pToastT>0){pToastT--;hctxt(16,pToast1,HB_YEL,0);}
 if(pDemo)htxt(23,19,"DEMO",HB_GRAY);}
// ---------------------------------------------------------------- demo autopilot (hold R + A on a course node)
static int dHold,dStuck,dLastX,dBack,dTgt,dJetY;
static int d_standable(int cx,int cy){int t=ptile(cx,cy),b=ptile(cx,cy+1);if(cy<0||cy>=PH)return 0;if(TFL[t]&(F_SOL|F_HAZ))return 0;if(TFL[ptile(cx,cy-1)]&F_SOL)return 0;return (TFL[b]&(F_SOL|F_ONE))&&!(TFL[b]&F_HAZ);}
static int d_target(int px,int py){int cx=px>>4,cy=(py-1)>>4;for(int c=1;c<=5;c++){int best=-1;for(int dy=-3;dy<=4;dy++){if(d_standable(cx+c,cy+dy)){if(best<0||iabs(dy)<iabs(best))best=dy;}}
  if(best>=0&&!(c==1&&best==0))return ((cx+c)*16+8)|((best+8)<<12);}return -1;}
static u16 demo_keys(void){u16 k=K_RIGHT|K_B;int px=P.x>>8,py=P.y>>8;
 if(pBossOn&&B.ph<9){int bx=B.x>>8,by=B.y>>8;k=K_B;int want=bx-60;if(B.ai==BA_HELI||B.ai==BA_MI8&&B.ph==0||B.ai==BA_MOTHER)want=bx;
  if(px<want-8)k|=K_RIGHT;else if(px>want+8)k|=K_LEFT;else k|=(bx>px)?0:0;
  if(by<py-40&&iabs(bx-px)<40)k|=K_UP;else if(B.face!=0&&!(k&(K_LEFT|K_RIGHT)))k|=(bx>px)?K_RIGHT:K_LEFT;
  if(P.on&&(pT%50)==0)dHold=14;if(dHold>0){dHold--;k|=K_A;}return k;}
 if(dBack>0){dBack--;k=K_LEFT|K_B;}
 if(dHold>0){dHold--;k|=K_A;}
 if(pAuto&&!pBossOn&&P.on&&!dHold){int gn=0;for(int c=16;c<=64;c+=16)if(!(TFL[ptile((px+c)>>4,(py+4)>>4)]&(F_SOL|F_ONE|F_WAT)))gn=1;
  if(gn&&px>camX+100)return (k&~K_RIGHT)|K_LEFT;if(!gn&&px>camX+150)k&=~K_RIGHT;}
 if(P.jetT>0&&!P.on){if(!dJetY)dJetY=py-8;if(py>dJetY)k|=K_A;}else if(P.on)dJetY=0;
 if(!P.on&&dTgt>0&&!P.jetT){k&=~(K_RIGHT|K_LEFT);if(px<dTgt-3)k|=K_RIGHT;else if(px>dTgt+3)k|=K_LEFT;}
 if(P.on)dTgt=0;
 if(P.swim){if((pT&15)<6)k|=K_A;return k;}
 if(P.climb){k|=K_UP;return k;}
 int wall=(TFL[ptile((px+10)>>4,(py-4)>>4)]&F_SOL)||(TFL[ptile((px+10)>>4,(py-14)>>4)]&F_SOL);
 if(!P.on&&P.vy<0&&wall)k|=K_A;
 if(P.on){int g1=TFL[ptile((px+12)>>4,(py+4)>>4)],g2=TFL[ptile((px+24)>>4,(py+4)>>4)];
  int gap=!(g1&(F_SOL|F_ONE|F_WAT))||!(g2&(F_SOL|F_ONE|F_WAT));int haz=((g1|g2)&F_HAZ)||(TFL[ptile((px+16)>>4,(py-4)>>4)]&F_HAZ);
  if(gap){for(int i=0;i<nPE;i++){PEnt*e=&PE[i];if(!e->alive||(e->k!=25&&e->k!=26))continue;int lx=e->x>>8;
    if(e->x0*16>px-8&&e->x0*16<px+64){if(e->k==25&&(lx>px+28||lx+32<px+4)){k&=~K_RIGHT;if(!dHold)return k;}
     if(e->k==26&&iabs((e->y>>8)-py)>10){k&=~K_RIGHT;if(!dHold)return k;}}}}
  if(ptile(px>>4,py>>4)==T_SPRING||ptile((px+8)>>4,py>>4)==T_SPRING)dHold=24;
  if((wall||gap||haz)&&!dHold){int t=d_target(px,py);dHold=20;if(t>=0){int tx=t&4095,dy=(t>>12)-8;int dc=(tx-px)/16;dTgt=tx;dHold=dy<0?20:(dc<=2?8:dc==3?12:18);if(dy<=-2)dHold=22;}k|=K_A;}}
 if(iabs(px-dLastX)<1){if(++dStuck>70){dStuck=0;dHold=22;if((pT/70)%3==2)dBack=14;}}else{dStuck=0;dLastX=px;}
 return k;}
// ---------------------------------------------------------------- course loop
static int pl_result(void){ // returns 1
 int w=pw,s=ps_;u8 v=PS.cs[w][s];v|=1;v|=(pIntel&7)<<1;if(pSecret)v|=16;PS.cs[w][s]=v;PS.tags+=pTags;PS.lastW=w;PS.lastS=s;
 if(!pDemo)ps_write();
 int n=(pIntel&1)+(pIntel>>1&1)+(pIntel>>2&1);
 for(int t=0;;t++){BEAT->screen=SCR_PLAT;BEAT->phase=4;hud_clear();hpanel(4,4,22,11);hctxt(5,pSecret?"SECRET EXIT FOUND!":"COURSE CLEAR",HB_YEL,1);
  hptxt(6,7,PF_ST[w][s].n,HB_WHITE);hptxt(6,9,cat3("INTEL FILES ",itoa(n),"/3"),HB_GREEN);hptxt(6,10,cat("DOG TAGS ",itoa(pTags)),HB_WHITE);hptxt(6,11,cat("ENEMIES ",itoa(pKills)),HB_WHITE);
  if(t>40)hptxt(9,13,"A: CONTINUE",(t>>4)&1?HB_YEL:HB_WHITE);
  oam_begin();present(1,camX/3,72+camY/2);if(t>40&&(pressed&(K_A|K_START)))break;if(pDemo&&t>90)break;}
 return 1;}
static int pl_run(int w,int s,int demo){pDemo=demo;pKills=0;pToastT=0;dHold=dStuck=dBack=0;pl_start(w,s);if(demo)P.lives=40;
 static const u8 TRW[PF_NW]={TR_JUNGLE,TR_DUNE,TR_DMZ,TR_COLD,TR_FROST,TR_ISLAND,TR_SAND,TR_PADDY,TR_OLDC,TR_XENO};bgm_t((PF_ST[w][s].fl&SF_A)||PF_ST[w][s].ai?TR_FORT:TRW[w]);
 ptoast(PF_ST[w][s].n);
 int doneT=0,deaths=0;camX=0;
 for(;;){BEAT->screen=SCR_PLAT;BEAT->mode=w;BEAT->era=s;BEAT->hp=P.hp;BEAT->x=P.x>>12;BEAT->y=P.y>>12;BEAT->phase=pDone?1:P.dead?2:pBossOn?3:0;BEAT->deaths=deaths;BEAT->kills=pKills;
  BEAT->scoreA=pTags;BEAT->scoreB=pIntel;BEAT->extra=pW|(PS.unit<<10);BEAT->timer=pT;
  if(demo){u16 k=demo_keys();pressed=k&~dprev;dprev=k;keys=k;}
  pT++;
  if(!pDone){pl_update();ents_update();if(pBossOn)boss_update();pb_update();}
  else{doneT++;if(pBossOn&&B.ph<10)boss_update();}
  // camera
  {int tx;if(pAuto&&!pBossOn){pCamX8+=0xC0;tx=pCamX8>>8;}else tx=(P.x>>8)-112+P.face*16;
   int lo=0,hi=pMainW-240;if((P.x>>8)>=pMainW){lo=pMainW;hi=pW*16-240;}
   if(pBossOn){lo=pArenaX;hi=pArenaX+23*16-240;}
   if(hi<lo)hi=lo;tx=clampi(tx,lo,hi);
   if(camX==-999||iabs(tx-camX)>200)camX=tx;else if(pAuto&&!pBossOn)camX=tx;else camX+=(tx-camX)/4+((tx>camX)?1:(tx<camX)?-1:0)*(iabs(tx-camX)>3);
   camX=clampi(camX,lo,hi);if(pAuto&&(pCamX8>>8)!=camX)pCamX8=camX<<8;
   int ty=clampi((P.y>>8)-100,0,PH*16-160);camY+=(ty-camY)/4;camY=clampi(camY,0,PH*16-160);}
  pl_tiles();pl_draw();pl_hud();
  present(1,camX/3,72+camY/2);
  if(P.dead&&P.deadT>80){deaths++;P.lives--;if(P.lives<=0){music_defeat();for(int t=0;t<120;t++){hud_clear();hpanel(8,7,14,4);hctxt(8,"GAME OVER",HB_RED,1);oam_begin();BEAT->phase=5;present(1,camX/3,72+camY/2);}return 0;}
   int lv=P.lives;pl_respawn(pChkX>0);P.lives=lv;camX=-999;}
  if(pDone&&doneT>(pBossOn?60:90))return pl_result();
  if(pressed&K_START){if(pause_menu())return 0;}}}
// ---------------------------------------------------------------- menus
static void m3_unit(int u,int x0,int y0,int sc,int frm){const u32*t=pf_unit_spr+(u*28+frm*4)*8;const u16*p=pf_unit_pal+u*16;
 for(int ty=0;ty<2;ty++)for(int tx=0;tx<2;tx++)for(int y=0;y<8;y++){u32 w=t[(ty*2+tx)*8+y];for(int x=0;x<8;x++){int c=(w>>(x*4))&15;if(!c)continue;
  int X=x0+(tx*8+x)*sc,Y=y0+(ty*8+y)*sc;for(int a=0;a<sc;a++)for(int b=0;b<sc;b++)m3_px(X+b,Y+a,p[c]);}}}
static void m3_stars(int x,int y,int n,int sc){for(int i=0;i<n;i++){int cx=x+i*6;static const u8 S[5]={0x04,0x1F,0x0E,0x0E,0x11};
 for(int r=0;r<5;r++)for(int c=0;c<5;c++)if(S[r]>>c&1){m3_px(cx+c,y+r,sc?RGB(255,220,60):RGB(120,110,80));}
 m3_px(cx+2,y+5,0);}}
static const char*course_tags(int w,int s){int f=PF_ST[w][s].fl;
 if(PF_ST[w][s].ai)return "BOSS FORTRESS";if(f&SF_K)return "BONUS COURSE";if(f&SF_A)return "AUTO-SCROLL AIRSHIP";if(f&SF_B)return "RIVER BOAT - AUTO-SCROLL";
 if(f&SF_Z)return "ZERO-G";if(f&SF_L)return "LOW GRAVITY";if(f&SF_R)return "RISING TIDE";if(f&SF_N)return "WIND GUSTS";if(f&SF_I)return "ICE";
 if(f&SF_T)return "TUNNELS";if(f&SF_D)return "NIGHT - NVG HELPS";if(f&SF_J)return "JETPACK CHASMS";if(f&SF_C)return "PARADROP";if(f&SF_X)return "SECRET EXIT HIDDEN";return "";}
// v1.3: unit select is a nation-grouped grid (one row per nation, flag-colour tag + portrait); all units free from the start
#define NNAT 8
static const char*const NAT_N[NNAT]={"USA","UK","UKRAINE","JAPAN","GERMANY","MEXICO","POLAND","ISRAEL"};
static const char*const NAT_S[NNAT]={"US","UK","UA","JP","DE","MX","PL","IL"};
static int unit_nat(int u){const char*c=PF_UC[u];for(int n=0;n<NNAT;n++){const char*a=NAT_N[n];int k=0;while(a[k]&&a[k]==c[k])k++;if(!a[k]&&!c[k])return n;}return 0;}
static void flag_tag(int x,int y,int n){ // generic flag-colour stripes, no emblems
 static const u16 F3[NNAT][3]={{RGB(200,40,50),RGB(240,240,240),RGB(40,60,140)},{RGB(30,50,130),RGB(240,240,240),RGB(200,30,40)},{RGB(40,100,210),RGB(40,100,210),RGB(250,210,40)},
  {RGB(245,245,245),RGB(245,245,245),RGB(245,245,245)},{RGB(20,20,20),RGB(210,20,30),RGB(250,200,40)},{RGB(0,120,70),RGB(245,245,245),RGB(200,30,40)},
  {RGB(245,245,245),RGB(245,245,245),RGB(215,30,50)},{RGB(245,245,245),RGB(30,70,180),RGB(245,245,245)}};
 if(n==1||n==2||n==4||n==6||n==7){for(int k=0;k<3;k++)m3_fill(x,y+k*3,12,3,F3[n][k]);}else for(int k=0;k<3;k++)m3_fill(x+k*4,y,4,9,F3[n][k]);
 if(n==0){m3_fill(x,y,12,9,RGB(240,240,240));for(int k=0;k<9;k+=2)m3_fill(x,y+k,12,1,RGB(200,40,50));m3_fill(x,y,5,5,RGB(40,60,140));}
 if(n==3)m3_fill(x+4,y+3,4,3,RGB(200,30,40));
 if(n==1){m3_fill(x+5,y,2,9,RGB(200,30,40));}
 m3_px(x-1,y-1,0);}
static int unit_select(void){int sel=PS.unit,redraw=1;if(sel>=PF_NU)sel=0;int rs[NNAT],rn[NNAT];
 for(int n=0;n<NNAT;n++){rs[n]=-1;rn[n]=0;}for(int u=0;u<PF_NU;u++){int n=unit_nat(u);if(rs[n]<0)rs[n]=u;rn[n]++;}
 for(;;){BEAT->screen=SCR_PUNIT;BEAT->menu=sel;BEAT->extra=PF_NU;int sn=unit_nat(sel),col=sel-rs[sn];
  if(redraw){redraw=0;m3_title(1);m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);m3_ctext(8,"SELECT UNIT - ALL 25 AVAILABLE",C_YEL,1);
   for(int n=0;n<NNAT;n++){int y=19+n*13;flag_tag(10,y+3,n);m3_text(25,y+4,NAT_S[n],n==sn?C_YEL:C_GRAY,1);
    for(int k=0;k<rn[n];k++){int u=rs[n]+k,x=40+k*18;if(u==sel){m3_fill(x-1,y-1,18,16,RGB(90,80,30));}m3_unit(u,x,y-2,1,0);}}
   // big portrait for the current pick, over the free space right of the short rows
   m3_fill(150,44,80,76,RGB(20,30,70));m3_unit(sel,158,46,4,(frame>>3)%3==2?1:0);flag_tag(214,108,sn);
   m3_text(10,125,PF_UN[sel],C_YEL,1);m3_text(160,125,NAT_N[sn],C_BLUE,1);m3_text(10,135,cat3(WPN_N[PF_UW[sel]]," / ",AB_N[sel]),C_WHITE,1);m3_text(10,145,AB_D[sel],C_GRAY,1);
   m3_present();}
  next_frame();
  if(pressed&K_RIGHT){sel=(sel+1)%PF_NU;redraw=1;sfx_menu();}
  if(pressed&K_LEFT){sel=(sel+PF_NU-1)%PF_NU;redraw=1;sfx_menu();}
  if(pressed&(K_UP|K_DOWN)){int n=(sn+((pressed&K_UP)?NNAT-1:1))%NNAT;sel=rs[n]+clampi(col,0,rn[n]-1);redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){PS.unit=sel;ps_write();sfx_ok();return sel;}if(pressed&K_B){sfx_menu();return -1;}}}
// v1.3: quick confirm before every course
static int deploy_prompt(int w,int s){int redraw=1;
 for(;;){BEAT->screen=47;BEAT->menu=PS.unit;BEAT->extra=0x100|PS.unit;
  if(redraw){redraw=0;m3_fill(20,46,200,66,C_PANEL);m3_box(20,46,200,66,2);m3_unit(PS.unit,30,58,2,0);flag_tag(32,94,unit_nat(PS.unit));
   {char tb[40];int k=0;const char*a[4]={itoa(w+1),"-",itoa(s+1)," "};for(int i=0;i<4;i++)for(const char*q=a[i];*q;q++)tb[k++]=*q;for(const char*q=PF_ST[w][s].n;*q&&k<27;q++)tb[k++]=*q;tb[k]=0;m3_text(70,54,tb,C_GRAY,1);}
   m3_text(70,66,"DEPLOY AS:",C_WHITE,1);m3_text(70,76,PF_UN[PS.unit],C_YEL,1);
   m3_text(70,90,"A=GO  SELECT=CHANGE UNIT",C_GREEN,1);m3_text(70,99,"B=BACK",C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&(K_A|K_START)){sfx_ok();return 1;}
  if(pressed&K_SEL){unit_select();return 2;}
  if(pressed&K_B){sfx_menu();return 0;}}}
static void pf_wmap_run(int w){bgm_t(TR_MAP);int sel=(PS.lastW==w)?PS.lastS:0,redraw=1;if(sel>=PF_NS[w])sel=0;
 for(;;){BEAT->screen=SCR_PMAP;BEAT->mode=w;BEAT->menu=sel;BEAT->extra=total_clear();
  if(redraw){redraw=0;dma32(PF_WMAP[w],fb,240*160/2);
   m3_fill(0,0,240,16,RGB(20,24,52));m3_box(0,0,240,17,2);{char tb[64];int k=0;const char*a[4]={"W",itoa(w+1)," ",PF_WN[w]};for(int i=0;i<4;i++)for(const char*q=a[i];*q&&k<38;q++)tb[k++]=*q;const char*v=" VS ";for(const char*q=v;*q&&k<60;q++)tb[k++]=*q;for(const char*q=PF_WE[w];*q&&k<60;q++)tb[k++]=*q;tb[k]=0;if(k>38){tb[k-slen(PF_WE[w])-4]=0;}m3_ctext(5,tb,C_YEL,1);}
   for(int s=0;s<PF_NS[w];s++){int x=PF_NODE[w][s][0],y=PF_NODE[w][s][1];int op=course_open(w,s);
    m3_stars(x-((PF_ST[w][s].stars*6)/2),y+9,PF_ST[w][s].stars,op);
    if(cs_clear(w,s)){m3_text(x-2,y-3,"+",C_GREEN,1);}else if(!op)m3_text(x-2,y-3,"X",C_GRAY,1);
    if(s==sel){for(int k=-10;k<=10;k++){m3_px(x+k,y-11,C_WHITE);m3_px(x+k,y+17,C_WHITE);}m3_unit(PS.unit,x-8,y-28,1,0);}}
   m3_fill(0,128,240,32,RGB(20,24,52));m3_box(0,127,240,33,2);
   const PfStage*S=&PF_ST[w][sel];m3_text(8,133,cat3(itoa(w+1),"-",itoa(sel+1)),C_GRAY,1);m3_text(32,133,S->n,course_open(w,sel)?C_WHITE:C_GRAY,1);
   m3_stars(176,134,S->stars,1);
   m3_text(8,143,course_open(w,sel)?(S->ai?cat("BOSS: ",BKN[S->ba]):course_tags(w,sel)):(is_bonus(w,sel)?"LOCKED: SECRET EXIT / 9 INTEL":"LOCKED"),course_open(w,sel)?C_BLUE:C_RED,1);
   {int n=cs_intel(w,sel);m3_text(152,143,cat3("INTEL ",itoa(n),"/3"),n==3?C_GREEN:C_GRAY,1);}
   m3_text(8,152,cat("UNIT: ",PF_US[PS.unit]),C_WHITE,1);m3_text(116,152,"SELECT: CHANGE UNIT",C_GREEN,1);
   m3_present();}
  next_frame();
  if(pressed&(K_RIGHT|K_DOWN)){if(sel<PF_NS[w]-1){sel++;redraw=1;sfx_menu();}}
  if(pressed&(K_LEFT|K_UP)){if(sel>0){sel--;redraw=1;sfx_menu();}}
  if(pressed&K_SEL){unit_select();redraw=1;}
  int demo=(keys&K_R)&&(pressed&K_A);
  if((pressed&K_A)&&!demo&&course_open(w,sel)){int d;while((d=deploy_prompt(w,sel))==2){}redraw=1;if(!d)continue;}
  if(((pressed&K_A)||demo)&&course_open(w,sel)){sfx_ok();PS.lastW=w;PS.lastS=sel;pl_run(w,sel,demo);aff_reset();bgm_t(TR_MAP);redraw=1;
   if(cs_clear(w,sel)&&sel<PF_NS[w]-1&&!demo){int n=sel+1;if(is_bonus(w,n)&&!course_open(w,n)&&n<PF_NS[w]-1)n++;sel=n;}}
  if(pressed&K_B){sfx_menu();return;}}}
static void spec_ops(void){ps_read();bgm_t(TR_MAP);if(unit_select()<0)return;int w=PS.lastW,redraw=1;
 for(;;){BEAT->screen=SCR_PGLOBE;BEAT->mode=w;BEAT->menu=keys;BEAT->extra=total_clear()|(total_intel()<<8);
  if(redraw){redraw=0;dma32(pf_globe,fb,240*160/2);m3_fill(0,0,240,16,RGB(20,24,52));m3_box(0,0,240,17,2);m3_ctext(5,"SPEC OPS - CAMPAIGN MAP",C_YEL,1);
   for(int i=0;i<PF_NW;i++){int x=PF_WPOS[i][0],y=PF_WPOS[i][1];int op=world_open(i);m3_text(x-(i==9?5:2),y-3,itoa(i+1),op?C_WHITE:C_GRAY,1);
    if(i==w){for(int k=-16;k<=16;k++){m3_px(x+k,y-14,C_YEL);m3_px(x+k,y+13,C_YEL);}m3_unit(PS.unit,x-8,y-32,1,0);}
    int cl=0;for(int s=0;s<PF_NS[i];s++)cl+=cs_clear(i,s);if(cl==PF_NS[i])m3_text(x+8,y+4,"*",C_YEL,1);}
   m3_fill(0,128,240,32,RGB(20,24,52));m3_box(0,127,240,33,2);
   m3_text(8,132,cat3("WORLD ",itoa(w+1),cat(": ",PF_WN[w])),world_open(w)?C_YEL:C_GRAY,1);
   m3_text(8,141,world_open(w)?cat("VS ",PF_WE[w]):"LOCKED - CLEAR THE PREVIOUS WORLD",world_open(w)?C_WHITE:C_RED,1);
   m3_text(170,141,cat3(itoa(total_clear()),"/90 CLR",""),C_GRAY,1);m3_text(8,150,cat("UNIT: ",PF_US[PS.unit]),C_WHITE,1);m3_text(116,150,"SELECT: CHANGE UNIT",C_GREEN,1);
   m3_present();}
  next_frame();
  if((keys&K_L)&&(keys&K_R)&&(pressed&K_SEL)){PS.allun^=1;ps_write();sfx_ok();redraw=1;continue;}
  if(pressed&(K_RIGHT|K_DOWN)){w=(w+1)%PF_NW;redraw=1;sfx_menu();}
  if(pressed&(K_LEFT|K_UP)){w=(w+PF_NW-1)%PF_NW;redraw=1;sfx_menu();}
  if(pressed&K_SEL){unit_select();redraw=1;}
  if((pressed&(K_A|K_START))&&world_open(w)){sfx_ok();pf_wmap_run(w);redraw=1;}
  if(pressed&K_B){sfx_menu();ps_write();return;}}}
