// ===================================================================== v1.3 SPEC OPS: BRADLEY
// Side-scrolling vehicle platformer: drive only the M2 Bradley. 4 worlds x 7 courses (desert, Europe forest, urban, snow).
// A: 25mm Bushmaster (hold UP to aim high)  B: jump / ramp-hop  R: TOW (wire-guided: UP/DOWN steer)  L: smoke  SELECT: dismount squad
// SRAM 0x980 'AOEB': per-course medals/clear + best score, chosen variant/camo/upgrades.
#include "braddb.inc"
#define SCR_BRAD 58
#define SCR_BRSEL 59
#define BR_OFF 0x980
enum{UP_ERA=1,UP_BUSK=2,UP_TOW=4,UP_TURRET=8};
static const u8 BRV_HP[5]={24,26,30,24,26},BRV_TOW[5]={4,4,5,6,3},BRV_RATE[5]={7,6,6,7,7},BRV_SQ[5]={2,2,2,0,2};
static const u16 BRV_SPD[5]={0x1C0,0x1C0,0x1B0,0x200,0x1C0};
static const char*const BRV_D[5]={"DESERT STORM ERA. 4 TOW, SQUAD","CIV SIGHT SPOTS IEDS. FASTER GUN","HEAVIEST ARMOUR. 5 TOW","CAVALRY SCOUT: FAST, 6 TOW, NO SQUAD","FIRE SUPPORT: R CALLS ARTILLERY"};
static const u8 UPG_COST[4]={9,18,30,45};static const char*const UPG_N[4]={"ERA ARMOUR","BUSK KIT","EXTRA TOW","FAST TURRET"};
static const char*const UPG_D[4]={"-40% FROM RPG / ATGM / SHELLS","+4 HULL, HALF MINE/IED DAMAGE","+2 TOW MISSILES","25MM FIRES 40% FASTER"};
typedef struct{u8 cs[28];u16 best[28];u8 var,camo,upg,lastW,lastC,allun;}BrSave;
static BrSave BS;
static void bs_write(void){const u8*b=(const u8*)&BS;u8 s=0x3C;SRAM[BR_OFF]='A';SRAM[BR_OFF+1]='O';SRAM[BR_OFF+2]='E';SRAM[BR_OFF+3]='B';SRAM[BR_OFF+4]=1;
 for(int i=0;i<(int)sizeof BS;i++){SRAM[BR_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[BR_OFF+5+sizeof BS]=s;}
static void bs_read(void){memset(&BS,0,sizeof BS);BS.camo=255;if(SRAM[BR_OFF]!='A'||SRAM[BR_OFF+1]!='O'||SRAM[BR_OFF+2]!='E'||SRAM[BR_OFF+3]!='B'||SRAM[BR_OFF+4]!=1)return;
 u8 b[sizeof BS];u8 s=0x3C;for(int i=0;i<(int)sizeof BS;i++){b[i]=SRAM[BR_OFF+5+i];s+=b[i]*(i+1);}if(s!=SRAM[BR_OFF+5+sizeof BS])return;memcpy(&BS,b,sizeof BS);}
static int br_medals(void){int n=0;for(int i=0;i<28;i++)n+=(BS.cs[i]>>1&1)+(BS.cs[i]>>2&1)+(BS.cs[i]>>3&1);return n;}
static int br_wclear(int w){return BS.cs[w*7+6]&1;}
static int br_var_open(int v){return BS.allun||v==0||(v<=3&&br_wclear(v-1))||(v==4&&(br_wclear(3)||br_medals()>=60));}
static int br_course_open(int i){return BS.allun||i==0||(BS.cs[i-1]&1)||(BS.cs[i]&1);}
static int br_upg_open(int k){return BS.allun||br_medals()>=UPG_COST[k];}
// ---------------------------------------------------------------- state
typedef struct{s32 x,y,vx,vy;s16 hp,maxhp,tow,smoke,sq,inv,cd,face,on,aimUp,lives,dead,deadT,anim,towId,arty,water;}BrV;
static BrV V;
typedef struct{s32 x,y,vx,vy;s16 hp,t,cd,c;u8 k,alive,face,st;}BrE;
#define NBE 48
static BrE BE[NBE] EWRAM;static int nBE,brEi; // brEi: next course entity to activate (sorted by x)
typedef struct{s32 x,y,vx,vy;s16 life,dmg;u8 k,team,guided,pad;}BrB;
#define NBB 40
static BrB BB[NBB];
enum{BK_25,BK_TOW,BK_EBUL,BK_RPG,BK_ATGM,BK_SHELL,BK_ARTY,BK_ROCKET,BK_HE,BK_ALLY};
typedef struct{s32 x,y,vx;s16 hp,t,cd;u8 alive;}BrAlly;
static BrAlly BA[3];
typedef struct{s16 x,t;}BrWarn;static BrWarn BW[6];
typedef struct{s16 cx,cy,t;}BrCrumb;static BrCrumb BCR[12];
static struct{s32 x,y;s16 hp,max,t,cd,cd2,flash,dead,on,era,aps,kind,face,x0;}BO;
static int brW,brC,brI,brT,brDone,brKills,brMedal,brChkX,brSmokeT,brToastT,brArenaX,brScore,brDeaths,brFire;static const char*brToast;
static const BrLv*BL;
#define OT_V 64
#define OT_BO 128
#define OT_C 192
static void br_toast(const char*s){brToast=s;brToastT=100;}
static void bspr(int x,int y,int tile,int shape,int size,int pal,int hf){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14)|(hf?1<<12:0);o[2]=tile|(1<<10)|(pal<<12);noam++;}
static void br_decode(void){pW=BL->w;const u8*t=BL->t;
 for(int y=0;y<PH;y++){int x=0;while(x<pW){int v=*t++,n=*t++;for(int i=0;i<n;i++)pm[PMI(x+i,y)]=v;x+=n;}}
 for(int y=1;y<PH;y++)for(int x=0;x<pW;x++)if(pm[PMI(x,y)]==T_TOP){int a=pm[PMI(x,y-1)];if(a==T_TOP||a==T_FILL||a==T_WTOP||a==T_WATER)pm[PMI(x,y)]=a==T_WTOP||a==T_WATER?T_TOP:T_FILL;}}
static void br_load_gfx(void){int th=BL->th;
 vsync();REG_DISPCNT=0x80;
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(PF_TILES[th],CHARBLOCK(1),34*32);dma32(PF_BDT[th],(u8*)CHARBLOCK(1)+160*32,PF_BDN[th]*8);
 for(int i=0;i<48;i++)PAL_BG[i]=pf_pal[th*48+i];
 for(int i=0;i<1024;i++)bgc[i]=PF_BDM[th][i];
 dma32(fx_tiles,OBJ_TILES,sizeof(fx_tiles)/4);
 dma32(br_veh+BS.var*64*8,(u8*)OBJ_TILES+OT_V*32,64*8);
 if(BL->boss<5)dma32(br_boss+BL->boss*64*8,(u8*)OBJ_TILES+OT_BO*32,64*8);
 dma32(br_com,(u8*)OBJ_TILES+OT_C*32,sizeof(br_com)/4);
 int cm=BS.camo<3?BS.camo:BR_WCAMO[brW];
 for(int i=0;i<16;i++){PAL_OBJ[i]=BR_CAMO[cm][i];PAL_OBJ[16+i]=brp_enemy[i];PAL_OBJ[32+i]=brp_misc[i];PAL_OBJ[48+i]=fx_pal[i];}
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=1|(1<<2)|(30<<8);REG_BGCNT(2)=3|(1<<2)|(29<<8);
 REG_BLDCNT=0;hud_clear();oam_begin();memset(bgw,0,2048);
 REG_DISPCNT=0|(1<<6)|(1<<8)|(1<<9)|(1<<10)|(1<<12);}
static int br_solid(int px,int py){return TFL[ptile(px>>4,py>>4)]&F_SOL;}
static int br_floor(int px,int py,int oldpy){int f=TFL[ptile(px>>4,py>>4)];if(f&F_SOL)return 1;if((f&F_ONE)&&((oldpy-1)>>4)<(py>>4))return 1;return 0;}
static void br_spawn_ent(const BrEnt*d){if(d->k==BE_BOSS){brArenaX=d->x;return;}
 for(int i=0;i<NBE;i++)if(!BE[i].alive){BrE*e=&BE[i];memset(e,0,sizeof*e);e->alive=1;e->k=d->k;e->x=d->x<<8;e->y=(d->y*16+16)<<8;e->face=-1;
  static const u8 HP[BE_N]={1,1,2,4,10,14,1,1,1,1,1,1,1,1,1,1};e->hp=HP[d->k]>1?BDhp(HP[d->k]):1;e->cd=60+(i*37)%90;if(i>=nBE)nBE=i+1;
  if(d->k==BE_ORLAN||d->k==BE_FPV)e->y=(d->y*16)<<8;return;}}
static void br_reset_life(void){int v=BS.var;memset(&V,0,sizeof V);V.maxhp=BRV_HP[v]+((BS.upg&UP_BUSK)?4:0);V.hp=V.maxhp;
 V.tow=BRV_TOW[v]+((BS.upg&UP_TOW)&&v!=4?2:0);V.smoke=3;V.sq=BRV_SQ[v];V.face=1;V.inv=90;
 V.x=((brChkX?brChkX:48))<<8;V.y=0;for(int y=0;y<PH*16;y+=4)if(br_solid(V.x>>8,y)||br_floor(V.x>>8,y,y-1)){V.y=y<<8;break;}
 memset(BB,0,sizeof BB);memset(BA,0,sizeof BA);memset(BW,0,sizeof BW);brSmokeT=0;}
static void br_start(int i){brI=i;brW=i/7;brC=i%7;BL=&BR_LV[i];br_decode();memset(BE,0,sizeof BE);nBE=0;brEi=0;brChkX=0;brT=0;brDone=0;brKills=0;brMedal=0;brArenaX=-1;brDeaths=0;
 memset(&BO,0,sizeof BO);memset(BCR,0,sizeof BCR);V.lives=3;int l=3;br_reset_life();V.lives=l;br_load_gfx();camX=0;camY=32;}
static BrB*bb_new(int k,int team,s32 x,s32 y,s32 vx,s32 vy,int life,int dmg){for(int i=0;i<NBB;i++)if(BB[i].life<=0){BrB*b=&BB[i];b->x=x;b->y=y;b->vx=vx;b->vy=vy;b->k=k;b->team=team;b->life=life;b->dmg=dmg;b->guided=0;return b;}return 0;}
static void br_aim(int x0,int y0,int k,int spd,int dmg,int life){int dx=(V.x>>8)-x0,dy=((V.y>>8)-12)-y0;int d=idist(dx,dy);if(d<1)d=1;bb_new(k,1,x0<<8,y0<<8,dx*spd/d,dy*spd/d,life,dmg);}
static void br_hurt(int n,int kind){if(V.inv>0||V.dead||brDone)return;
 if((BS.upg&UP_ERA)&&(kind==BK_RPG||kind==BK_ATGM||kind==BK_SHELL||kind==BK_ROCKET)){n=n*3/5;if(n<1)n=1;}
 V.hp-=n;V.inv=40;sfx_hurt();if(V.hp<=0){V.hp=0;V.dead=1;V.deadT=0;fx_add(V.x>>8,(V.y>>8)-12,FXK_BIG);sfx_boom();shake(10);}}
static void br_blast(int x,int y,int r,int dmg,int team){fx_add(x,y,FXK_BIG);sfx_boom();shake(5);
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||team!=0)continue;if(iabs((e->x>>8)-x)<r&&iabs((e->y>>8)-10-y)<r+8){e->hp-=dmg;e->c=6;}}
 if(team==1&&iabs((V.x>>8)-x)<r+20&&iabs((V.y>>8)-12-y)<r+10)br_hurt(dmg,BK_ARTY);
 for(int cx=(x-r)>>4;cx<=(x+r)>>4;cx++)for(int cy=(y-r)>>4;cy<=(y+r)>>4;cy++)if(ptile(cx,cy)==T_BRICK)break_tile(cx,cy);}
static void br_kill(BrE*e){e->alive=0;brKills++;brScore+=e->k>=BE_TECH&&e->k<=BE_BMP2?300:100;int big=e->k>=BE_TECH&&e->k<=BE_BMP2;fx_add(e->x>>8,(e->y>>8)-(big?14:8),big?FXK_BIG:FXK_SMALL);if(big){sfx_boom();shake(6);}else sfx_shot(1);}
static int ent_w(int k){return k==BE_BTR||k==BE_BMP2?28:k==BE_TECH?14:7;}
static int ent_h(int k){return k==BE_BTR||k==BE_BMP2?24:k==BE_TECH?14:k>=BE_MINE&&k<=BE_IED?6:k<=BE_ATGM?22:14;}
static int boss_box(int x,int y){if(!BO.on||BO.dead)return 0;int bx=BO.x>>8,by=BO.y>>8;return iabs(x-bx)<28&&y<by-2&&y>by-26;}
static void boss_hurt(int d,int tow){if(BO.dead||!BO.on)return;
 if(tow&&BO.kind==4&&((BO.aps++)&1)){fx_add(BO.x>>8,(BO.y>>8)-28,FXK_SPARK);br_toast("AFGANIT APS INTERCEPT!");return;}
 if(tow&&BO.kind==3&&BO.era<4){BO.era++;d=3;br_toast("ERA BLOCKED - KEEP HITTING");}
 BO.hp-=d;BO.flash=4;if(BO.hp<=0){BO.hp=0;BO.dead=1;BO.t=0;brScore+=5000;sfx_boom();shake(14);}}
// ---------------------------------------------------------------- player vehicle
static void br_crumb(int cx,int cy){for(int i=0;i<12;i++)if(BCR[i].t>0&&BCR[i].cx==cx&&BCR[i].cy==cy)return;for(int i=0;i<12;i++)if(BCR[i].t<=0){BCR[i].cx=cx;BCR[i].cy=cy;BCR[i].t=22;return;}}
static void br_vehicle(void){int v=BS.var;if(V.inv>0)V.inv--;if(V.cd>0)V.cd--;
 if(V.dead){V.deadT++;return;}
 int px=V.x>>8,py=V.y>>8;
 int wat=(TFL[ptile(px>>4,(py-6)>>4)]&F_WAT)!=0;V.water=wat;
 int max=BRV_SPD[v];if(wat)max=max*11/20;
 int dir=(keys&K_RIGHT)?1:(keys&K_LEFT)?-1:0;
 if(dir){V.vx+=dir*0x18;if(V.vx>max)V.vx=max;if(V.vx<-max)V.vx=-max;V.face=dir;V.anim++;}
 else{if(V.vx>0){V.vx-=0x14;if(V.vx<0)V.vx=0;}else if(V.vx<0){V.vx+=0x14;if(V.vx>0)V.vx=0;}}
 if(iabs(V.vx)>0x40)V.anim++;
 V.aimUp=(keys&K_UP)!=0;
 if((pressed&K_B)&&V.on){V.vy=wat?-0x400:-0x4C0;V.on=0;sfx_whoosh();}
 // horizontal move with 1-tile auto-climb
 s32 nx=V.x+V.vx;int fx=(nx>>8)+(V.vx>0?26:-26);
 if(V.vx){int hitLow=br_solid(fx,py-4),hitMid=br_solid(fx,py-14)||br_solid(fx,py-22);
  if(hitLow&&!hitMid&&V.on){int top=((py-4)>>4)*16;if(!br_solid(fx,top-8)&&!br_solid(fx,top-20)){V.y=top<<8;nx=V.x+V.vx/2;}else nx=V.x;}
  else if(hitLow||hitMid){nx=V.x;V.vx=0;}}
 if((nx>>8)<20)nx=20<<8;if((nx>>8)>pW*16-20)nx=(pW*16-20)<<8;
 if(brArenaX>=0&&BO.on&&!BO.dead){int lo=brArenaX+20,hi=brArenaX+22*16-20;if((nx>>8)<lo)nx=lo<<8;if((nx>>8)>hi)nx=hi<<8;}
 if(BO.on&&!BO.dead&&BO.kind!=2){int bx=BO.x>>8;if(iabs((nx>>8)-bx)<56){nx=(bx+((nx>>8)<bx?-56:56))<<8;V.vx=0;}}
 V.x=nx;px=V.x>>8;
 // vertical
 V.vy+=wat?0x28:0x40;if(V.vy>0x600)V.vy=0x600;s32 oy=V.y;V.y+=V.vy;py=V.y>>8;int opy=oy>>8;
 V.on=0;if(V.vy>=0){for(int k=-20;k<=20;k+=20){if(br_floor(px+k,py,opy)){int top=(py>>4)*16;V.y=top<<8;V.vy=0;V.on=1;
    int t=ptile((px+k)>>4,py>>4);if(t==T_CRUMB)br_crumb((px+k)>>4,py>>4);}}
  if(V.on&&opy+0<(V.y>>8)-1){}}
 else if(br_solid(px,py-26)){V.vy=0;V.y=((((py-26)>>4)+1)*16+26)<<8;}
 if(py>PH*16+24){V.hp=0;V.dead=1;V.deadT=40;sfx_hurt();}
 // weapons
 int rate=BRV_RATE[v];if(BS.upg&UP_TURRET)rate=rate*3/5;
 if((keys&K_A)&&V.cd<=0){V.cd=rate;s32 gx=V.x+(V.face*30<<8),gy=V.y-(19<<8);
  if(V.aimUp)bb_new(BK_25,0,gx-(V.face*6<<8),gy-(6<<8),V.face*0x380,-0x380,80,1);else bb_new(BK_25,0,gx,gy,V.face*0x500,(brT&1)?16:-16,72,1);
  sfx_shot(0);brFire=3;}
 if(pressed&K_R){if(v==4){if(V.tow>0){int best=-1,bd=999;for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||e->k>=BE_MINE)continue;int dx=(e->x>>8)-px;if(dx>0&&dx<220&&dx<bd){bd=dx;best=i;}}
    int tx=best>=0?(BE[best].x>>8):(BO.on&&!BO.dead?(BO.x>>8):px+120);for(int i=0;i<6;i++)if(BW[i].t<=0){BW[i].x=tx;BW[i].t=60;break;}V.tow--;br_toast("FIRE MISSION - SHOT, OVER");sfx_ok();}}
  else if(V.tow>0&&(V.towId<0||BB[V.towId].life<=0||BB[V.towId].k!=BK_TOW)){BrB*b=bb_new(BK_TOW,0,V.x+(V.face*20<<8),V.y-(26<<8),V.face*0x240,V.aimUp?-0x200:0,150,10);
   if(b){b->guided=1;V.towId=b-BB;V.tow--;sfx_whoosh();}}}
 if((pressed&K_L)&&V.smoke>0&&brSmokeT<=0){V.smoke--;brSmokeT=300;sfx_whoosh();br_toast("SMOKE! ATGMS LOSE LOCK");}
 if((pressed&K_SEL)&&V.sq>0){V.sq--;for(int i=0;i<3;i++){BrAlly*a=&BA[i];a->alive=1;a->x=V.x-(i*10<<8);a->y=V.y;a->hp=2;a->t=480;a->cd=20+i*7;}br_toast("DISMOUNT! SQUAD OUT");sfx_ok();}}
// ---------------------------------------------------------------- projectiles
static void br_bullets(void){
 for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life<=0)continue;b->life--;
  if(b->k==BK_TOW&&b->guided){if(b->vx>0&&b->vx<0x480)b->vx+=0x10;if(b->vx<0&&b->vx>-0x480)b->vx-=0x10;if(keys&K_UP)b->vy-=0x18;if(keys&K_DOWN)b->vy+=0x18;if(b->vy>0x200)b->vy=0x200;if(b->vy<-0x300)b->vy=-0x300;
   if(brT&3)fx_add((b->x>>8)-(b->vx>0?8:-8),b->y>>8,FXK_SMOKE);}
  if(b->k==BK_ATGM&&brSmokeT<=0){int ty=((V.y>>8)-12)<<8;b->vy+=(ty>b->y)?0x0C:-0x0C;if(b->vy>0x100)b->vy=0x100;if(b->vy<-0x100)b->vy=-0x100;}
  if(b->k==BK_ARTY||b->k==BK_HE)b->vy+=0x20;
  b->x+=b->vx;b->y+=b->vy;int x=b->x>>8,y=b->y>>8;
  if(br_solid(x,y)){int t=ptile(x>>4,y>>4);if(t==T_BRICK&&b->team==0){break_tile(x>>4,y>>4);brScore+=10;}
   if(b->k==BK_TOW||b->k>=BK_ARTY||b->k==BK_RPG||b->k==BK_SHELL)br_blast(x,y,b->k==BK_TOW?20:16,b->k==BK_TOW?4:3,b->team);else fx_add(x,y,FXK_SPARK);b->life=0;continue;}
  if(x<camX-40||x>camX+280||y<-60||y>PH*16+20){b->life=0;continue;}
  if(b->team==0){
   if(boss_box(x,y)){boss_hurt(b->dmg,b->k==BK_TOW);fx_add(x,y,b->k==BK_TOW?FXK_BIG:FXK_SPARK);if(b->k==BK_TOW){sfx_boom();shake(6);}b->life=0;continue;}
   for(int j=0;j<nBE;j++){BrE*e=&BE[j];if(!e->alive||e->k>BE_IED)continue;int ex=e->x>>8,ey=e->y>>8;
    if(iabs(x-ex)<ent_w(e->k)+2&&y<ey+2&&y>ey-ent_h(e->k)-2){
     if(e->k==BE_MINE||e->k==BE_IED){e->alive=0;br_blast(ex,ey-4,18,2,0);brScore+=50;}
     else{e->hp-=b->dmg;e->c=6;if(e->hp<=0)br_kill(e);}
     if(b->k==BK_TOW)br_blast(x,y,20,4,0);b->life=0;break;}}}
  else{int vx=V.x>>8,vy=V.y>>8;
   for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(a->alive&&iabs(x-(a->x>>8))<5&&y>(a->y>>8)-14&&y<(a->y>>8)){a->hp-=b->dmg;b->life=0;if(a->hp<=0){a->alive=0;fx_add(a->x>>8,(a->y>>8)-8,FXK_SMALL);}}}
   if(b->life>0&&iabs(x-vx)<26&&y>vy-24&&y<vy+2){if(b->k==BK_ARTY||b->k==BK_HE)br_blast(x,y,16,4,1);else{br_hurt(b->dmg,b->k);fx_add(x,y,b->dmg>2?FXK_BIG:FXK_SPARK);}b->life=0;}}}
 // artillery warnings (Orlan spotting / M7 fire missions)
 for(int i=0;i<6;i++)if(BW[i].t>0){BW[i].t--;if(BW[i].t==0){for(int k=0;k<3;k++)bb_new(BK_ARTY,BS.var==4&&BW[i].x>=0?0:1,(BW[i].x+(k-1)*18)<<8,(camY-20-k*30)<<8,0,0x200,120,BS.var==4?6:4);}}
 for(int i=0;i<12;i++)if(BCR[i].t>0&&--BCR[i].t==0){pm[PMI(BCR[i].cx,BCR[i].cy)]=T_AIR;fx_add(BCR[i].cx*16+8,BCR[i].cy*16+8,FXK_SMOKE);}}
// ---------------------------------------------------------------- enemies
static void br_ents(void){int px=V.x>>8,py=V.y>>8;
 while(brEi<BL->ne&&BL->e[brEi].x<camX+300){br_spawn_ent(&BL->e[brEi]);brEi++;}
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive)continue;int ex=e->x>>8,ey=e->y>>8;int dx=px-ex,adx=iabs(dx);
  if(e->c>0)e->c--;if(ex<camX-120){e->alive=0;continue;}e->t++;if(e->cd>0)e->cd--;
  int vis=ex<camX+250;e->face=dx<0?-1:1;
  switch(e->k){
  case BE_INF:case BE_RPG:case BE_ATGM:
   if(iabs(dx)<30&&iabs(py-ey)<20&&iabs(V.vx)>0x60&&!V.dead){br_kill(e);br_toast("OVERRUN!");break;}
   if(vis&&e->cd<=0&&!V.dead&&adx<220){if(e->k==BE_INF){br_aim(ex,ey-10,BK_EBUL,0x200,1,120);e->cd=BDcd(70+(i*13)%40);}
    else if(brSmokeT>0){e->cd=30;}
    else if(e->k==BE_RPG){br_aim(ex,ey-11,BK_RPG,0x260,3,120);e->cd=BDcd(150);sfx_whoosh();}
    else{bb_new(BK_ATGM,1,(ex+e->face*8)<<8,(ey-8)<<8,e->face*0x180,0,200,4);e->cd=BDcd(220);sfx_whoosh();}}
   break;
  case BE_TECH:case BE_BTR:case BE_BMP2:{int stop=e->k==BE_TECH?90:130;
   if(vis&&adx>stop){int sp=e->k==BE_TECH?0x90:0x50;e->x+=dx<0?-sp:sp;}
   if(e->k==BE_TECH&&iabs(dx)<40&&iabs(py-ey)<20&&iabs(V.vx)>0x80&&!V.dead){br_kill(e);br_toast("RAMMED!");break;}
   if(vis&&e->cd<=0&&!V.dead&&adx<230){
    if(e->k==BE_BMP2&&(e->t/200)&1&&brSmokeT<=0){bb_new(BK_ATGM,1,(ex+e->face*24)<<8,(ey-22)<<8,e->face*0x180,0,200,5);e->cd=BDcd(160);}
    else{for(int k=0;k<3;k++)br_aim(ex+e->face*(e->k==BE_TECH?6:24),ey-(e->k==BE_TECH?13:22)+k*3,BK_EBUL,0x280,e->k==BE_TECH?1:2,100);e->cd=BDcd(e->k==BE_TECH?80:110);}sfx_shot(1);}
   // keep on ground
   {int gy=ey;while(gy<PH*16&&!br_floor(ex,gy,gy-1))gy++;while(gy>0&&br_solid(ex,gy-1))gy--;e->y=gy<<8;}
   break;}
  case BE_ORLAN:{int ty=(16+((e->t>>5)&7))<<8;e->y+=(ty-e->y)/16;if(vis){int sp=0x80;if(dx>20)e->x+=sp;else if(dx<-20)e->x-=sp;}
   if(vis&&adx<40&&e->cd<=0&&!V.dead){for(int k=0;k<6;k++)if(BW[k].t<=0){BW[k].x=px+(V.vx>>4);BW[k].t=BDcd(90);break;}e->cd=BDcd(260);br_toast("ORLAN SPOTTED YOU - MOVE!");}
   break;}
  case BE_FPV:if(vis&&adx<170&&!V.dead){int ddy=(py-12)-ey;int d=idist(dx,ddy);if(d<1)d=1;e->x+=dx*0x1C0/d;e->y+=ddy*0x1C0/d;
    if(adx<24&&iabs(ddy)<14){e->alive=0;fx_add(ex,ey,FXK_BIG);sfx_boom();br_hurt(4,BK_ROCKET);}}break;
  case BE_MINE:case BE_IED:{int r=e->k==BE_IED?22:24;if(!V.dead&&iabs(dx)<r&&iabs(py-ey)<10){e->alive=0;int d=e->k==BE_IED?5:3;if(BS.upg&UP_BUSK)d/=2;br_blast(ex,ey-4,10,0,0);br_hurt(d,BK_ARTY);br_toast(e->k==BE_IED?"IED!":"MINE STRIKE!");}}break;
  default:{int hit=!V.dead&&iabs(dx)<28&&py>ey-20&&py<ey+20;
   if(e->k==BE_GOAL){if(!V.dead&&dx>-8&&iabs(py-ey)<40&&!(BO.on&&!BO.dead)){brDone=1;e->alive=0;sfx_ok();}}
   else if(hit){e->alive=0;sfx_ok();
    if(e->k==BE_TOWC){V.tow+=2;br_toast("+2 TOW");}else if(e->k==BE_REP){V.hp=V.hp+6>V.maxhp?V.maxhp:V.hp+6;br_toast("FIELD REPAIR +6");}
    else if(e->k==BE_MEDAL){brMedal|=1<<(__builtin_popcount(brMedal));brScore+=500;br_toast("MEDAL!");}
    else if(e->k==BE_CHECK){brChkX=ex;br_toast("CHECKPOINT");}}}}
  if(e->k<=BE_IED&&e->hp<=0&&e->alive)br_kill(e);}
 // dismounted squad
 for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(!a->alive)continue;if(--a->t<=0){a->alive=0;continue;}int ax=a->x>>8;
  int tgt=-1,bd=999;for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive||e->k>BE_FPV)continue;int d=iabs((e->x>>8)-ax);if(d<bd){bd=d;tgt=i;}}
  if(tgt>=0&&bd<130){if(--a->cd<=0){BrE*e=&BE[tgt];int dx=(e->x>>8)-ax,dy=((e->y>>8)-8)-((a->y>>8)-9);int d=idist(dx,dy);if(d<1)d=1;bb_new(BK_ALLY,0,a->x,a->y-(9<<8),dx*0x300/d,dy*0x300/d,60,1);a->cd=18;}}
  else{a->x+=0xA0;}
  {int gy=a->y>>8;while(gy<PH*16&&!br_floor(ax,gy,gy-1))gy++;while(gy>0&&br_solid(ax,gy-1))gy--;a->y=gy<<8;}}}
// ---------------------------------------------------------------- bosses
static void br_bossup(void){
 if(!BO.on){if(brArenaX>=0&&(V.x>>8)>brArenaX+40){BO.on=1;BO.kind=BL->boss;static const u8 HP[5]={60,50,55,75,90};BO.max=BO.hp=BDhp(HP[BO.kind]);
   BO.x=(brArenaX+19*16)<<8;BO.y=(9*16)<<8;if(BO.kind==2)BO.y=72<<8;BO.x0=BO.x>>8;BO.cd=120;BO.cd2=60;bgm_t(TR_BOSS);br_toast(BR_BOSSN[BO.kind]);}return;}
 int bx=BO.x>>8,by=BO.y>>8,px=V.x>>8;BO.t++;if(BO.flash>0)BO.flash--;if(BO.cd>0)BO.cd--;if(BO.cd2>0)BO.cd2--;BO.face=px<bx?-1:1;
 if(BO.dead){if(BO.t<60&&(BO.t&7)==0){fx_add(bx-20+(BO.t*7)%40,by-12-(BO.t*3)%16,FXK_BIG);sfx_boom();}if(BO.t==60)bgm_t(TR_BRAD);return;}
 int k=BO.kind;
 if(k==2){int side=((BO.t/360)&1)?-1:1;int tx=clampi(px+side*100,brArenaX+40,brArenaX+22*16-40);BO.x+=((tx<<8)-BO.x)/40;BO.y=(72+((BO.t>>2)&15)-8)<<8;
  if(BO.cd<=0){for(int r=0;r<4;r++)bb_new(BK_ROCKET,1,(bx+BO.face*20)<<8,(by-6)<<8,BO.face*(0x200+r*0x40),0x1C0+r*0x30,140,3);BO.cd=BDcd(130);sfx_whoosh();}
  if(BO.cd2<=0){br_aim(bx+BO.face*26,by-8,BK_EBUL,0x300,2,90);if((BO.t&3)==0)BO.cd2=BDcd(70);else BO.cd2=6;}
  return;}
 // ground armour: patrol in right half of arena, fire main gun / coax
 {int lo=brArenaX+130,hi=brArenaX+22*16-36;int tx=lo+tri(BO.t/(k==4?1:2),hi-lo);BO.x+=((tx<<8)-BO.x)/60;}
 int gy=by-(k==1?22:18);
 if(BO.cd<=0){
  if(k==1){int dx=px-bx;bb_new(BK_HE,1,(bx+BO.face*30)<<8,gy<<8,dx*0x100/60,-0x380,160,4);BO.cd=BDcd(110);}
  else{bb_new(BK_SHELL,1,(bx+BO.face*34)<<8,gy<<8,BO.face*(k==4?0x500:0x400),0,140,5);BO.cd=BDcd(k==4?120:150);fx_add(bx+BO.face*34,gy,FXK_FLASH);}
  sfx_boom();if(k==3&&(BO.t/300)&1&&brSmokeT<=0)bb_new(BK_ATGM,1,(bx+BO.face*34)<<8,(gy-4)<<8,BO.face*0x180,0,220,5);}
 if(BO.cd2<=0){br_aim(bx+BO.face*20,gy-3,BK_EBUL,0x2C0,1,100);BO.cd2=(BO.t&7)?8:BDcd(90);}
 if(k==4&&(BO.t%BDcd(320))==0){for(int i=0;i<2;i++){BrEnt d={BE_FPV,(u16)(bx-20+i*30),2};br_spawn_ent(&d);}br_toast("T-14 LAUNCHES FPV DRONES");}}
// ---------------------------------------------------------------- draw
static void br_tiles(void){int tx0=camX>>4,ty0=camY>>4;
 for(int cy=ty0;cy<ty0+11;cy++)for(int cx=tx0;cx<tx0+16;cx++){int t=ptile(cx,cy);if(cy>=PH)t=T_AIR;
  u16 b=(t*4)|(PF_MTB[t]<<12);int X=(cx*2)&31,Y=(cy*2)&31;bgw[Y*32+X]=b;bgw[Y*32+X+1]=b+1;bgw[(Y+1)*32+X]=b+2;bgw[(Y+1)*32+X+1]=b+3;}}
static void br_draw(void){oam_begin();int fr=(V.anim>>2)&1;
 // HUD-level sprites first (priority by order)
 for(int i=0;i<6;i++)if(BW[i].t>0&&(BW[i].t&8))bspr(BW[i].x-4-camX,24,OT_C+BT_WARN,0,0,0,0);
 for(int i=0;i<NBB;i++){BrB*b=&BB[i];if(b->life<=0)continue;int x=(b->x>>8)-camX,y=(b->y>>8)-camY;int hf=b->vx<0;
  switch(b->k){case BK_25:case BK_ALLY:bspr(x-4,y-4,OT_C+BT_ROUND,0,0,0,hf);break;case BK_EBUL:bspr(x-4,y-4,OT_C+BT_EBUL,0,0,1,hf);break;
   case BK_SHELL:case BK_HE:bspr(x-4,y-4,OT_C+BT_SHELL,0,0,1,hf);break;case BK_ARTY:bspr(x-4,y-4,OT_C+BT_ARTY,0,0,1,0);break;
   case BK_TOW:bspr(x-8,y-4,OT_C+BT_M_TOW,1,1,0,hf);break;case BK_RPG:bspr(x-8,y-4,OT_C+BT_M_RPG,1,1,1,hf);break;
   case BK_ATGM:bspr(x-8,y-4,OT_C+BT_M_ATGM,1,1,1,hf);break;case BK_ROCKET:bspr(x-8,y-4,OT_C+BT_M_ROCKET,1,1,1,hf);break;}}
 for(int j=0;j<3;j++){BrAlly*a=&BA[j];if(a->alive)bspr((a->x>>8)-8-camX,(a->y>>8)-16-camY,OT_C+BT_ALLY0+((brT>>3)&1)*4,0,1,0,0);}
 if(!V.dead&&!(V.inv>0&&(brT&2))){int sx=(V.x>>8)-32-camX,sy=(V.y>>8)-31-camY+(V.water?3:0);int hf=V.face<0;
  if(V.aimUp)bspr(hf?sx+2:sx+46,sy+0,OT_C+BT_GUN1,0,1,0,hf);else bspr(hf?sx-8:sx+56,sy+6,OT_C+BT_GUN0,0,1,0,hf);
  if(brFire>0){bspr(hf?(V.aimUp?sx-2:sx-14):(V.aimUp?sx+58:sx+70),V.aimUp?sy-6:sy+7,0,0,0,3,0);brFire--;}
  if(BS.upg&UP_ERA)bspr(sx,sy,OT_C+BT_ERA,1,3,0,hf);
  if(BS.upg&UP_BUSK)bspr(hf?sx+24:sx+24,sy-4,OT_C+BT_BUSK,0,1,0,hf);
  bspr(sx,sy,OT_V+fr*32,1,3,0,hf);}
 if(brSmokeT>0){int sx=(V.x>>8)-camX,sy=(V.y>>8)-camY;for(int k=0;k<3;k++)bspr(sx-48+k*32-((brT>>3)&3),sy-34-(k&1)*6,OT_C+BT_SMOKE0+((brT>>4)&1)*16,0,2,0,0);}
 if(BO.on&&!(BO.dead&&BO.t>40)&&!(BO.flash&1)){int x=(BO.x>>8)-32-camX,y=(BO.y>>8)-31-camY;bspr(x,y,OT_BO+((BO.t>>2)&1)*32,1,3,1,BO.face<0);}
 for(int i=0;i<nBE;i++){BrE*e=&BE[i];if(!e->alive)continue;int x=(e->x>>8)-camX,y=(e->y>>8)-camY;if(x<-40||x>280)continue;if(e->c&1)continue;int hf=e->face<0,f=(e->t>>3)&1;
  switch(e->k){
  case BE_INF:bspr(x-8,y-16,OT_C+BT_RIFLE0+f*4,0,1,1,hf);break;case BE_RPG:bspr(x-8,y-16,OT_C+BT_RPG0+f*4,0,1,1,hf);break;case BE_ATGM:bspr(x-8,y-16,OT_C+BT_ATGM0,0,1,1,hf);break;
  case BE_TECH:bspr(x-16,y-16,OT_C+BT_TECH0+f*8,1,2,1,hf);break;case BE_BTR:bspr(x-32,y-31,OT_C+BT_BTR0+f*32,1,3,1,hf);break;case BE_BMP2:bspr(x-32,y-31,OT_C+BT_BMP2_0+f*32,1,3,1,hf);break;
  case BE_ORLAN:bspr(x-8,y-8,OT_C+BT_ORLAN0+((brT>>1)&1)*4,0,1,1,hf);break;case BE_FPV:bspr(x-8,y-8,OT_C+BT_FPV0+((brT>>1)&1)*4,0,1,1,0);break;
  case BE_MINE:bspr(x-8,y-6,OT_C+BT_MINE,1,1,1,0);break;
  case BE_IED:bspr(x-8,y-7,OT_C+BT_IED,1,1,1,0);if(BS.var>=1&&BS.var!=3&&iabs(x-(V.x>>8)+camX)<130&&(brT&16))bspr(x-4,y-20,OT_C+BT_WARN,0,0,0,0);break;
  case BE_TOWC:bspr(x-8,y-15,OT_C+BT_CRATE0,0,1,0,0);break;case BE_REP:bspr(x-8,y-15,OT_C+BT_CRATE1,0,1,0,0);break;
  case BE_MEDAL:bspr(x-8,y-8+((brT>>4)&1),OT_C+BT_CRATE2,0,1,0,0);break;
  case BE_CHECK:bspr(x-8,y-31,OT_C+BT_FLAG0,2,2,0,0);break;case BE_GOAL:bspr(x-8,y-31,OT_C+BT_FLAG1,2,2,0,0);break;}}
 fx_update_draw();}
static void br_hud(void){hud_clear();
 htxt(0,0,"HULL",HB_WHITE);hbar(4,0,6,V.hp,V.maxhp);
 htxt(11,0,cat(BS.var==4?"ARTY":"TOW",itoa(V.tow)),V.tow?HB_YEL:HB_GRAY);
 htxt(17,0,cat("SMK",itoa(V.smoke)),brSmokeT>0?HB_GREEN:HB_WHITE);
 htxt(23,0,cat("x",itoa(V.lives)),HB_WHITE);
 {char b[4];for(int i=0;i<3;i++)b[i]=(brMedal>>i&1)?'#':'-';b[3]=0;htxt(27,0,b,HB_YEL);}
 if(V.sq)htxt(0,1,cat("SQUAD ",itoa(V.sq)),HB_GREEN);
 htxt(20,1,itoa(brScore),HB_WHITE);
 if(BO.on&&!BO.dead){htxt(1,18,BR_BOSSN[BO.kind],HB_RED);hbar(1,19,14,BO.hp,BO.max);}
 if(brToastT>0){brToastT--;hctxt(4,brToast,HB_YEL,0);}}
static int br_result(void){int i=brI;u8 v=BS.cs[i]|1|((brMedal&7)<<1);BS.cs[i]=v;int sc=brScore+V.hp*50+(brT<60*120?3000-brT/24:0);if(sc<0)sc=0;brScore=sc;
 int nb=0;if(sc/10>BS.best[i]){BS.best[i]=sc/10>65535?65535:sc/10;nb=1;}BS.lastW=brW;BS.lastC=brC;bs_write();music_jingle(TR_CLEAR);
 int n=__builtin_popcount(brMedal&7);
 for(int t=0;;t++){BEAT->screen=SCR_BRAD;BEAT->phase=4;BEAT->scoreA=sc;BEAT->scoreB=n;hud_clear();hpanel(4,4,22,12);hctxt(5,"COURSE CLEAR",HB_YEL,1);
  hptxt(6,7,BL->n,HB_WHITE);hptxt(6,9,cat3("MEDALS ",itoa(n),"/3"),HB_YEL);hptxt(6,10,cat("KILLS ",itoa(brKills)),HB_WHITE);hptxt(6,11,cat("TIME ",mmss(brT)),HB_WHITE);
  hptxt(6,12,cat("SCORE ",itoa(sc)),nb?HB_GREEN:HB_WHITE);if(nb)hptxt(19,12,"BEST!",HB_GREEN);
  if(t>40)hptxt(9,14,"A: CONTINUE",(t>>4)&1?HB_YEL:HB_WHITE);
  oam_begin();present(1,camX/3,72+camY/2);if(t>40&&(pressed&(K_A|K_START)))break;}
 return 1;}
static int br_run(int i){brScore=0;br_start(i);bgm_t(TR_BRAD);br_toast(BL->n);int doneT=0;
 for(;;){BEAT->screen=SCR_BRAD;BEAT->mode=brW;BEAT->era=brC;BEAT->hp=V.hp;BEAT->x=V.x>>12;BEAT->y=V.y>>12;BEAT->phase=brDone?1:V.dead?2:(BO.on&&!BO.dead)?3:0;
  BEAT->kills=brKills;BEAT->deaths=brDeaths;BEAT->scoreA=brScore;BEAT->scoreB=brMedal|(BO.on?BO.hp<<4:0);BEAT->extra=pW|(BS.var<<10);BEAT->timer=brT;
  brT++;if(brSmokeT>0)brSmokeT--;
  if(!brDone){br_vehicle();br_ents();br_bossup();br_bullets();}else doneT++;
  {int tx=(V.x>>8)-100+V.face*20;int lo=0,hi=pW*16-240;if(BO.on&&!BO.dead){lo=brArenaX;hi=brArenaX+22*16-240;}tx=clampi(tx,lo,hi);
   if(iabs(tx-camX)>200)camX=tx;else camX+=(tx-camX)/6+((tx>camX)-(tx<camX));camX=clampi(camX,lo,hi);
   int ty=clampi((V.y>>8)-112,0,PH*16-160);camY+=(ty-camY)/4;}
  br_tiles();br_draw();br_hud();present(1,camX/3,72+camY/2);
  if(V.dead&&V.deadT>90){brDeaths++;V.lives--;if(V.lives<=0){music_defeat();for(int t=0;t<120;t++){hud_clear();hpanel(8,7,14,4);hctxt(8,"GAME OVER",HB_RED,1);oam_begin();BEAT->phase=5;present(1,camX/3,72+camY/2);}return 0;}
   int lv=V.lives;br_reset_life();V.lives=lv;camX=clampi((V.x>>8)-100,0,pW*16-240);
   if(BO.on&&!BO.dead){BO.on=0;brChkX=brArenaX-40;br_reset_life();V.lives=lv;}}
  if(brDone&&doneT>60)return br_result();
  if(pressed&K_START){if(pause_menu())return 0;}}}
// ---------------------------------------------------------------- menus (mode 3)
static void m3_tiles(const u32*t,int w8,int h8,const u16*pal,int x0,int y0,int sc,int hf){
 for(int ty=0;ty<h8;ty++)for(int tx=0;tx<w8;tx++){const u32*q=t+(ty*w8+tx)*8;for(int y=0;y<8;y++){u32 r=q[y];for(int x=0;x<8;x++){int c=(r>>(4*x))&15;if(!c)continue;
  int X=hf?(w8*8-1-(tx*8+x)):(tx*8+x);for(int a=0;a<sc;a++)for(int b=0;b<sc;b++){int px=x0+X*sc+a,py=y0+(ty*8+y)*sc+b;if(px>=0&&px<240&&py>=0&&py<160)fb[py*240+px]=pal[c];}}}}}
static void br_vpreview(int x,int y,int sc){int cm=BS.camo<3?BS.camo:0;m3_tiles(br_veh+BS.var*64*8,8,4,BR_CAMO[cm],x,y,sc,0);}
static int br_garage(void){int sel=0,redraw=1;
 for(;;){BEAT->screen=SCR_BRSEL;BEAT->mode=1;BEAT->x=sel;BEAT->extra=BS.var|(BS.camo<<4)|(BS.upg<<8);
  if(redraw){cat_bgc(CAT_SPEC);banner("BRADLEY GARAGE","VARIANT / CAMO / UPGRADES",C_YEL);m3_box(4,38,232,118,1);
   br_vpreview(150,44,1);m3_text(150,78,BR_VARN[BS.var],C_YEL,1);
   m3_text(12,44,sel==0?">":" ",C_YEL,1);m3_text(20,44,"VARIANT",C_WHITE,1);m3_text(70,44,br_var_open(BS.var)?BR_VARN[BS.var]:"LOCKED",sel==0?C_YEL:C_WHITE,1);
   m3_text(12,56,sel==1?">":" ",C_YEL,1);m3_text(20,56,"CAMO",C_WHITE,1);m3_text(70,56,BS.camo<3?BR_CAMON[BS.camo]:"AUTO (WORLD)",sel==1?C_YEL:C_WHITE,1);
   for(int k=0;k<4;k++){int y=70+k*12,op=br_upg_open(k);m3_text(12,y,sel==2+k?">":" ",C_YEL,1);m3_text(20,y,UPG_N[k],op?C_WHITE:C_GRAY,1);
    m3_text(90,y,op?((BS.upg>>k&1)?"ON":"OFF"):cat3("MEDALS ",itoa(UPG_COST[k]),""),op?((BS.upg>>k&1)?C_GREEN:C_GRAY):C_RED,1);}
   m3_text(12,120,sel==6?">":" ",C_YEL,1);m3_text(20,120,"DONE",sel==6?C_YEL:C_WHITE,1);
   m3_text(150,120,cat3("MEDALS ",itoa(br_medals()),"/84"),C_YEL,1);
   const char*d=sel==0?BRV_D[BS.var]:sel>=2&&sel<6?UPG_D[sel-2]:sel==1?"DESERT / WOODLAND / WINTER":"SAVED TO SRAM";m3_ctext(136,d,C_GRAY,1);
   if(sel==0&&!br_var_open(BS.var))m3_ctext(146,BS.var==4?"CLEAR WORLD 4 OR 60 MEDALS":cat3("CLEAR WORLD ",itoa(BS.var)," BOSS"),C_RED,1);
   m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+6)%7;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%7;redraw=1;sfx_menu();}
  int d=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(d||(pressed&K_A)){if(!d)d=1;
   if(sel==0){BS.var=(BS.var+5+d)%5;redraw=1;sfx_menu();}
   else if(sel==1){BS.camo=BS.camo>=3?(d>0?0:2):(BS.camo+d<0||BS.camo+d>2?255:BS.camo+d);redraw=1;sfx_menu();}
   else if(sel<6){if(br_upg_open(sel-2)){BS.upg^=1<<(sel-2);redraw=1;sfx_ok();}}
   else if(pressed&K_A){if(!br_var_open(BS.var))BS.var=0;bs_write();pressed=0;return 0;}}
  if(pressed&K_B){if(!br_var_open(BS.var))BS.var=0;bs_write();pressed=0;return 0;}}}
static void bradley_ops(void){bs_read();int w=BS.lastW<4?BS.lastW:0,c=BS.lastC<7?BS.lastC:0,redraw=1;
 if(!br_var_open(BS.var))BS.var=0;bgm_t(TR_BRAD);
 if(keys&K_R)BS.allun=1; // demo/test unlock (hold R entering the mode)
 for(;;){int i=w*7+c;BEAT->screen=SCR_BRSEL;BEAT->mode=0;BEAT->x=i;BEAT->extra=BS.var;
  if(redraw){cat_bgc(CAT_SPEC);banner("SPEC OPS: BRADLEY",BR_WN[w],C_YEL);m3_box(4,38,232,118,1);
   m3_text(12,44,cat3("WORLD ",itoa(w+1)," <L R>"),C_YEL,1);
   for(int k=0;k<7;k++){int j=w*7+k,y=56+k*11,op=br_course_open(j);const BrLv*L=&BR_LV[j];
    m3_text(12,y,k==c?">":" ",C_YEL,1);m3_text(20,y,cat3(itoa(w+1),"-",itoa(k+1)),op?C_WHITE:C_GRAY,1);m3_text(44,y,op?L->n:"LOCKED",k==c?C_YEL:op?C_WHITE:C_GRAY,1);
    if(BS.cs[j]&1){char b[4];for(int q=0;q<3;q++)b[q]=(BS.cs[j]>>(q+1)&1)?'*':'-';b[3]=0;m3_text(164,y,b,C_YEL,1);}
    if(L->boss<5)m3_text(186,y,"BOSS",C_RED,1);}
   br_vpreview(170,40,1);
   m3_text(12,136,cat3("BEST ",itoa(BS.best[i]*10),""),C_WHITE,1);m3_text(110,136,cat3("MEDALS ",itoa(br_medals()),"/84"),C_YEL,1);
   m3_ctext(147,"A: DRIVE  SELECT: GARAGE  B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){c=(c+6)%7;redraw=1;sfx_menu();}if(pressed&K_DOWN){c=(c+1)%7;redraw=1;sfx_menu();}
  if(pressed&(K_L|K_LEFT)){w=(w+3)%4;redraw=1;sfx_menu();}if(pressed&(K_R|K_RIGHT)){w=(w+1)%4;redraw=1;sfx_menu();}
  if(pressed&K_SEL){br_garage();redraw=1;}
  if(pressed&K_A){if(br_course_open(i)){sfx_ok();int ok=br_run(i);pressed=0;bgm_t(TR_BRAD);if(ok&&c<6)c++;bgCached=-1;redraw=1;}else sfx_hurt();}
  if(pressed&K_B){pressed=0;bgCached=-1;return;}}}
static void rec_brad(const char**L){static char b[5][40];bs_read();int n=0,best=0,bi=0;for(int i=0;i<28;i++){n+=BS.cs[i]&1;if(BS.best[i]>best){best=BS.best[i];bi=i;}}
 const char*s[5];s[0]=cat3("COURSES CLEARED: ",itoa(n),"/28");for(int k=0;k<5;k++){}
 int k=0;const char*a=s[0];while(a[k]&&k<38){b[0][k]=a[k];k++;}b[0][k]=0;L[0]=b[0];
 a=cat3("MEDALS: ",itoa(br_medals()),"/84");k=0;while(a[k]&&k<38){b[1][k]=a[k];k++;}b[1][k]=0;L[1]=b[1];
 a=cat3("BEST SCORE: ",itoa(best*10),best?cat("  ",BR_LV[bi].n):"");k=0;while(a[k]&&k<38){b[2][k]=a[k];k++;}b[2][k]=0;L[2]=b[2];
 int nv=0;for(int v=0;v<5;v++)nv+=br_var_open(v);a=cat3("VARIANTS UNLOCKED: ",itoa(nv),"/5");k=0;while(a[k]&&k<38){b[3][k]=a[k];k++;}b[3][k]=0;L[3]=b[3];
 int bw=0;for(int w=0;w<4;w++)bw+=br_wclear(w);a=cat3("BOSSES DEFEATED: ",itoa(bw+((BS.cs[17]&1)?1:0)),"/5");k=0;while(a[k]&&k<38){b[4][k]=a[k];k++;}b[4][k]=0;L[4]=b[4];}
