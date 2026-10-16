// ===================================================================== v1.3 ANTI-AIRCRAFT
// Crew an AA vehicle defending a base: guns (lead / flak bursts / overheat) + lock-on missiles (ammo, reload, flares).
// Mode 3 static backdrop (VRAM) + all moving things as OBJ sprites; only the barrel rect + changed HUD fields are redrawn.
#include "aadb.inc"
#define SCR_AASEL 51
#define SCR_AA 52
enum{AG_NONE,AG_MG,AG_AUTO,AG_FLAK};enum{AM_NONE,AM_IR,AM_RADAR,AM_IIR,AM_INT};enum{HL_TRUCK,HL_HALF,HL_TRACK,HL_TOWED,HL_8X8,HL_BATT,HL_HMMWV};
typedef struct{const char*n,*d;u8 era,gun,rate,dmg,flakR,heat,nmis,mis,radar,barrels,hull,hp;u16 bspd,misRel,col;}AAV;
#define NAAV 17
static const AAV AAVS[NAAV]={
 {"QF 13-PDR AA LORRY","13-PDR ON A LORRY: SLOW, BIG FLAK",0,AG_FLAK,46,26,17,0,0,0,0,1,HL_TRUCK,90,0x480,0,RGB(110,100,70)},
 {"M16 MGMC","QUAD .50 CAL ON A HALFTRACK",1,AG_MG,4,4,0,3,0,0,0,4,HL_HALF,90,0x700,0,RGB(92,104,58)},
 {"FLAKPANZER WIRBELWIND","QUAD 20MM FLAK TURRET",1,AG_AUTO,6,6,8,4,0,0,0,4,HL_TRACK,110,0x600,0,RGB(150,140,104)},
 {"BOFORS 40MM","TOWED 40MM: CLIP-FED FLAK",1,AG_FLAK,20,16,12,0,0,0,0,1,HL_TOWED,70,0x5C0,0,RGB(96,104,70)},
 {"M19 GMC","TWIN 40MM ON A LIGHT TANK",1,AG_FLAK,13,15,12,0,0,0,0,2,HL_TRACK,110,0x5C0,0,RGB(92,104,58)},
 {"ZSU-23-4 SHILKA","RADAR QUAD 23MM - LEAD SIGHT",2,AG_AUTO,3,5,7,3,0,0,1,4,HL_TRACK,100,0x680,0,RGB(104,112,72)},
 {"M163 VADS","20MM VULCAN GATLING + RADAR",2,AG_MG,2,4,0,2,0,0,1,1,HL_TRACK,90,0x780,0,RGB(92,104,58)},
 {"GEPARD","RADAR TWIN 35MM FLAK",2,AG_AUTO,5,9,10,3,0,0,1,2,HL_TRACK,120,0x700,0,RGB(80,92,66)},
 {"M48 CHAPARRAL","4 IR MISSILES, NO GUN",2,AG_NONE,0,0,0,0,4,AM_IR,0,0,HL_TRACK,100,0,200,RGB(92,104,58)},
 {"M-SHORAD STRYKER","30MM RADAR GUN + 4 STINGERS",3,AG_AUTO,7,9,8,3,4,AM_IR,1,1,HL_8X8,120,0x700,240,RGB(96,100,80)},
 {"AVENGER","8 STINGERS + .50 CAL",3,AG_MG,5,4,0,3,8,AM_IR,0,1,HL_HMMWV,80,0x700,300,RGB(150,140,104)},
 {"PANTSIR-S1","TWIN 30MM + 12 RADAR MISSILES",3,AG_AUTO,3,7,8,3,12,AM_RADAR,1,2,HL_8X8,130,0x700,320,RGB(110,116,84)},
 {"TOR-M2","8 RADAR MISSILES, QUICK RELOAD",3,AG_NONE,0,0,0,0,8,AM_RADAR,1,0,HL_TRACK,120,0,200,RGB(110,116,84)},
 {"IRIS-T SLM","8 IMAGING-IR: IGNORE FLARES",3,AG_NONE,0,0,0,0,8,AM_IIR,1,0,HL_TRUCK,110,0,360,RGB(90,96,84)},
 {"GEPARD 1A2","RADAR TWIN 35MM, PROGRAMMED FUZE",3,AG_AUTO,4,10,11,3,0,0,1,2,HL_TRACK,120,0x740,0,RGB(80,92,66)},
 {"IRON DOME","20 INTERCEPTORS VS ROCKETS/DRONES",3,AG_NONE,0,0,0,0,20,AM_INT,1,0,HL_BATT,100,0,600,RGB(170,160,120)},
 {"M42 DUSTER","TWIN 40MM, COLD WAR VETERAN",2,AG_FLAK,12,15,12,0,0,0,0,2,HL_TRACK,110,0x5C0,0,RGB(92,104,58)}};
static const char*const AA_ERA[4]={"WW1","WW2","COLD WAR","MODERN"};
enum{EB_PLANE,EB_BOMB,EB_DIVE,EB_HELI,EB_KAMI,EB_HUNT,EB_SPOT,EB_MQ9};
typedef struct{u8 hp,beh,strafe,flares,low;u16 spd,score;}AAE;
static const AAE AAES[AA_NEN]={{6,EB_PLANE,1,0,0,0x0C0,50},{16,EB_BOMB,0,0,0,0x090,120},{60,EB_BOMB,0,0,0,0x040,400},{10,EB_PLANE,1,0,0,0x140,80},{12,EB_DIVE,0,0,0,0x110,120},
 {30,EB_BOMB,0,0,0,0x0C0,200},{14,EB_PLANE,1,1,0,0x220,150},{18,EB_BOMB,0,1,0,0x1C0,180},{30,EB_HELI,0,1,1,0x0A0,250},{20,EB_HELI,0,1,1,0x0C0,200},
 {24,EB_BOMB,0,1,0,0x1C0,220},{26,EB_HELI,0,1,1,0x0B0,260},{4,EB_KAMI,0,0,0,0x0E0,60},{3,EB_HUNT,0,0,0,0x180,80},{5,EB_SPOT,0,0,0,0x070,120},
 {2,EB_HUNT,0,0,1,0x1A0,60},{14,EB_MQ9,0,0,0,0x080,200},{6,EB_KAMI,0,0,1,0x260,150},{10,EB_PLANE,1,0,0,0x150,80},{14,EB_PLANE,1,1,0,0x220,150}};
typedef struct{u8 t,minw,wt;}AAPool;
static const AAPool AAP[4][10]={{{0,1,6},{1,2,3},{2,5,1}},{{3,1,5},{18,1,3},{4,2,3},{5,3,3}},{{19,1,4},{6,2,3},{7,2,3},{9,3,2},{8,4,2}},
 {{10,2,3},{11,3,2},{8,4,2},{12,1,5},{13,2,3},{14,3,1},{15,2,3},{16,4,1},{17,5,2}}};
static const u8 AAPN[4]={3,4,5,9};
// ---- runtime
typedef struct{s32 x,y,vx,vy;s16 hp,t,cd,st,dir,flash,fl;u8 k,alive,tgt;}AEn;
#define NAEN 20
static AEn AE[NAEN];
typedef struct{s32 x,y,vx,vy;s16 life,dmg,r;u8 k,tgt,decoy,alive;}APr; // k: 0 tracer 1 flak shell 2 missile 3 flare 4 bomb 5 ebul 6 rocket 7 emissile
#define NAPR 72
static APr AP[NAPR];
typedef struct{s16 x,y,t;u8 k;}AFx; // k 0 burst 1 boom 2 smoke
#define NAFX 20
static AFx AF[NAFX];
static int aaV,aaEra,aaMode,aaAng,aaHeat,aaOver,aaMis,aaRel,aaHP,aaMaxHP,aaWave,aaLeft,aaSpawnT,aaIntro,aaScore,aaCombo,aaComboT,aaLock,aaLockT,aaCd,aaArtT,aaToastT,aaKills;
static int aaBld[3];static const s16 BLDX[3][2]={{12,58},{70,96},{176,228}};static const char*aaToast;
#define AAGY 128
#define AAPX 120
#define AAPY 114
static u16 aaOam[128*4];static int aaNo;
#define AA_OFF 0x880
static u32 aaBest[4][2];
static void aa_load(void){if(SRAM[AA_OFF]=='A'&&SRAM[AA_OFF+1]=='O'&&SRAM[AA_OFF+2]=='E'&&SRAM[AA_OFF+3]=='A'&&SRAM[AA_OFF+4]==1){for(int i=0;i<8;i++){u32 v=0;for(int k=0;k<4;k++)v|=(u32)SRAM[AA_OFF+8+i*4+k]<<(8*k);aaBest[i>>1][i&1]=v;}}else memset(aaBest,0,sizeof aaBest);}
static void aa_save(void){SRAM[AA_OFF]='A';SRAM[AA_OFF+1]='O';SRAM[AA_OFF+2]='E';SRAM[AA_OFF+3]='A';SRAM[AA_OFF+4]=1;for(int i=0;i<8;i++)for(int k=0;k<4;k++)SRAM[AA_OFF+8+i*4+k]=aaBest[i>>1][i&1]>>(8*k);}
static void aa_toast(const char*s){aaToast=s;aaToastT=90;}
static void aspr(int x,int y,int tile,int shape,int size,int hf,int pal){if(aaNo>=128||x<=-32||x>=240||y<=-16||y>=160)return;u16*o=&aaOam[aaNo*4];o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14)|(hf?1<<12:0);o[2]=(512+tile)|(pal<<12);aaNo++;}
static APr*ap_new(int k,s32 x,s32 y,s32 vx,s32 vy,int life,int dmg){for(int i=0;i<NAPR;i++)if(!AP[i].alive){APr*p=&AP[i];memset(p,0,sizeof*p);p->alive=1;p->k=k;p->x=x;p->y=y;p->vx=vx;p->vy=vy;p->life=life;p->dmg=dmg;p->tgt=255;return p;}return 0;}
static void afx(int x,int y,int k){for(int i=0;i<NAFX;i++)if(!AF[i].t){AF[i].x=x;AF[i].y=y;AF[i].k=k;AF[i].t=1;return;}}
IWCODE static void aa_line(int x0,int y0,int x1,int y1,int th,u16 c){int dx=x1-x0,dy=y1-y0,n=iabs(dx)>iabs(dy)?iabs(dx):iabs(dy);if(!n)n=1;for(int i=0;i<=n;i++)lf(x0+dx*i/n-th/2,y0+dy*i/n-th/2,th,th,c);}
// ---- backdrop + vehicle art (drawn with lf into fb or VRAM via lfb)
static void aa_veh(int x,int y,int v,int bar){const AAV*V=&AAVS[v];u16 c=V->col,cd=RGB(40,40,44),cl=lit(c);
 switch(V->hull){
  case HL_TRUCK:lf(x-24,y+3,40,7,c);lf(x+14,y-5,11,15,c);lf(x+17,y-3,6,4,RGB(110,170,220));for(int k=0;k<3;k++){int wx=x-17+k*14+(k==2?6:0);lf(wx-3,y+9,7,6,cd);lf(wx-1,y+11,3,2,RGB(120,120,120));}break;
  case HL_HALF:lf(x-22,y+2,38,8,c);lf(x+14,y-3,10,12,c);lf(x+16,y-1,5,3,RGB(110,170,220));lf(x-24,y+9,30,6,cd);for(int k=0;k<5;k++)lf(x-22+k*6,y+11,3,2,RGB(110,110,110));lf(x+15,y+9,7,6,cd);break;
  case HL_TRACK:lf(x-22,y+1,44,9,c);lf(x-20,y,40,2,cl);lf(x-25,y+8,50,7,cd);for(int k=0;k<7;k++)lf(x-21+k*7,y+10,4,3,RGB(110,110,110));break;
  case HL_TOWED:lf(x-14,y+6,28,4,c);aa_line(x-14,y+8,x-24,y+14,2,c);aa_line(x+14,y+8,x+24,y+14,2,c);lf(x-12,y+9,7,6,cd);lf(x+6,y+9,7,6,cd);lf(x-6,y-2,12,9,c);break;
  case HL_8X8:lf(x-25,y+2,50,8,c);lf(x-25,y+2,8,4,cl);for(int k=0;k<4;k++){lf(x-21+k*12,y+9,7,6,cd);lf(x-19+k*12,y+11,3,2,RGB(120,120,120));}break;
  case HL_HMMWV:lf(x-18,y+3,36,7,c);lf(x-6,y-3,14,7,c);lf(x-3,y-2,8,3,RGB(110,170,220));lf(x-15,y+9,8,6,cd);lf(x+8,y+9,8,6,cd);break;
  default:lf(x-26,y+6,52,4,c);lf(x-22,y+9,7,6,cd);lf(x+15,y+9,7,6,cd);lf(x+12,y-14,10,18,RGB(200,200,190));lf(x+13,y-12,8,13,RGB(150,160,170));}
 if(V->hull!=HL_TOWED&&V->hull!=HL_BATT){lf(x-8,y-4,16,6,c);lf(x-7,y-4,14,1,cl);}
 if(V->radar){lf(x-1,y-12,2,8,cd);lf(x-6,y-14,12,3,RGB(180,190,200));}
 if(!bar)return;
 int a=aaAng,ca=COS(deg2a(a)),sa=SIN(deg2a(a));int px=x,py=y-2;
 if(V->gun==AG_NONE){int L=14;int bx=px+(ca*L>>8),by=py-(sa*L>>8);aa_line(px,py,bx,by,7,RGB(70,74,64));aa_line(px,py,bx,by,4,c);if(V->mis==AM_INT){aa_line(px+3,py,bx+3,by,5,RGB(150,140,110));}}
 else{int L=V->gun==AG_FLAK?20:V->gun==AG_AUTO?17:13;int nb=V->barrels;for(int b=0;b<nb;b++){int off=(b-(nb-1)/2)*2-(nb==2||nb==4?1:0);int ox=(-sa*off)>>8,oy=(-ca*off)>>8;
   aa_line(px+ox,py+oy,px+ox+(ca*L>>8),py+oy-(sa*L>>8),V->gun==AG_FLAK?2:1,RGB(40,40,44));}
  if(V->nmis){int L2=9;aa_line(px+6,py-1,px+6+(ca*L2>>8),py-1-(sa*L2>>8),4,RGB(150,150,140));}}}
static void aa_backdrop(int era){static const u16 SKY[4][2]={{RGB(150,160,170),RGB(200,200,196)},{RGB(110,140,200),RGB(240,190,140)},{RGB(130,150,170),RGB(196,206,214)},{RGB(20,30,70),RGB(80,90,140)}};
 for(int y=0;y<AAGY;y++){int t=y*256/AAGY;u16 a=SKY[era][0],b=SKY[era][1];int r=((a&31)*(256-t)+(b&31)*t)>>8,g=(((a>>5)&31)*(256-t)+((b>>5)&31)*t)>>8,bl=(((a>>10)&31)*(256-t)+((b>>10)&31)*t)>>8;lf(0,y,240,1,r|(g<<5)|(bl<<10));}
 if(era==3)for(int i=0;i<40;i++)lf((i*61)%240,(i*23)%70,1,1,RGB(230,230,240));
 for(int x=0;x<240;x+=4){int h=8+((x*37)%13)+(era==2?((x*7)%9):0);lf(x,AAGY-h,4,h,era==3?RGB(30,34,50):RGB(90,100,96));}
 if(era==3)for(int i=0;i<30;i++)lf((i*47)%236,AAGY-4-(i*13)%14,2,2,RGB(250,220,120));
 lf(0,AAGY,240,160-AAGY,era==0?RGB(110,94,70):era==3?RGB(50,56,52):RGB(96,120,70));for(int x=0;x<240;x+=12)lf(x,AAGY+(x*7)%9+2,6,1,era==0?RGB(90,76,56):RGB(80,100,58));
 if(era==0)for(int x=0;x<240;x+=3)lf(x,AAGY+12+((x/3)&1),3,1,RGB(70,60,46)); // trench line
 for(int b=0;b<3;b++){int x0=BLDX[b][0],x1=BLDX[b][1];
  if(aaBld[b]<=0){lf(x0,AAGY-5,x1-x0,5,RGB(60,56,50));for(int k=x0;k<x1;k+=5)lf(k,AAGY-7-(k*3)%4,3,3,RGB(80,74,64));continue;}
  if(b==0){lf(x0,AAGY-20,x1-x0,20,RGB(120,124,120));lf(x0+2,AAGY-22,x1-x0-4,3,RGB(150,154,150));lf(x0+12,AAGY-14,x1-x0-24,14,RGB(50,50,56));}
  else if(b==1){for(int k=0;k<2;k++){lf(x0+k*13,AAGY-12,11,12,RGB(170,150,110));lf(x0+k*13,AAGY-13,11,2,RGB(200,180,140));}}
  else{lf(x0,AAGY-14,x1-x0,14,era==0?RGB(160,150,120):RGB(140,130,100));lf(x0+4,AAGY-10,8,6,RGB(240,220,140));lf(x1-12,AAGY-10,8,6,RGB(240,220,140));lf(x0+20,AAGY-30,3,16,RGB(60,60,60));lf(x0+14,AAGY-32,15,3,RGB(180,190,200));}}}
static void aa_drawall(void){u16*save=lfb;lfb=0;aa_backdrop(aaEra);aa_veh(AAPX,AAPY,aaV,0);dma32(fb,dl_bg,240*160/2);lfb=save;}
static void aa_barrel(void){ // restore turret rect from bg, redraw barrel straight into VRAM
 u16*V=(u16*)0x06000000;int x0=AAPX-28,y0=AAPY-30,w=56,h=34;for(int y=y0;y<y0+h;y++)dma32(dl_bg+y*240+x0,V+y*240+x0,w/2);lfb=V;aa_veh(AAPX,AAPY,aaV,1);lfb=0;}
// ---- HUD (direct VRAM, change-detected)
static void aa_hud(int force){static int last[9];const AAV*Vv=&AAVS[aaV];int cur[9]={aaScore,aaWave,aaHP,aaBld[0]+aaBld[1]*256+aaBld[2]*65536,aaHeat>>2,aaMis,aaRel>0,aaCombo,aaOver};
 int ch=force;for(int i=0;i<9;i++)if(cur[i]!=last[i])ch=1;if(!ch)return;for(int i=0;i<9;i++)last[i]=cur[i];
 lfb=(u16*)0x06000000;lf(0,0,240,10,RGB(16,18,28));ltx(2,1,cat("SCORE ",itoa(aaScore)),C_WHITE);
 ltx(96,1,aaMode?cat("WAVE ",itoa(aaWave)):cat3("WAVE ",itoa(aaWave),"/10"),C_YEL);if(aaCombo>1)ltx(170,1,cat3("X",itoa(aaCombo)," COMBO"),C_GREEN);
 lf(0,150,240,10,RGB(16,18,28));ltx(2,151,"HP",C_GRAY);lf(16,152,40,5,RGB(60,20,20));lf(16,152,aaHP*40/aaMaxHP,5,aaHP*3>aaMaxHP?C_GREEN:C_RED);
 ltx(60,151,"BASE",C_GRAY);for(int b=0;b<3;b++){lf(86+b*12,152,10,5,RGB(60,20,20));lf(86+b*12,152,clampi(aaBld[b],0,40)*10/40,5,C_BLUE);}
 if(Vv->gun){ltx(126,151,aaOver?"HOT!":"HEAT",aaOver?C_RED:C_GRAY);lf(152,152,30,5,RGB(40,40,50));lf(152,152,clampi(aaHeat,0,100)*30/100,5,aaOver?C_RED:C_YEL);}
 if(Vv->nmis){ltx(188,151,aaRel>0?"RELOAD":cat("MSL ",itoa(aaMis)),aaRel>0?C_RED:C_WHITE);}
 lfb=0;}
// ---- enemies
static int aa_pick(void){const AAPool*P=AAP[aaEra];int tot=0;for(int i=0;i<AAPN[aaEra];i++)if(aaWave>=P[i].minw)tot+=P[i].wt;int r=rr(tot);for(int i=0;i<AAPN[aaEra];i++)if(aaWave>=P[i].minw){if(r<P[i].wt)return P[i].t;r-=P[i].wt;}return P[0].t;}
static int aa_spawn(int k){for(int i=0;i<NAEN;i++)if(!AE[i].alive){AEn*e=&AE[i];memset(e,0,sizeof*e);const AAE*S=&AAES[k];e->alive=1;e->k=k;e->hp=S->hp+(SET.bot>=2?S->hp/4:0)+aaWave/4;e->dir=rr(2)?1:-1;
  e->x=(e->dir>0?-30:270)<<8;int y=S->low?70+rr(30):S->beh==EB_MQ9?14+rr(10):S->beh==EB_BOMB?16+rr(30):20+rr(50);e->y=y<<8;
  int sp=S->spd+(SET.bot*S->spd)/8+aaWave*4;e->vx=e->dir*sp;e->cd=60+rr(60);e->fl=S->flares?2:0;e->tgt=rr(3);return i;}return -1;}
static void aa_kill(AEn*e){e->alive=0;afx(e->x>>8,e->y>>8,1);afx((e->x>>8)+8,(e->y>>8)+2,2);sfx_boom();shake(4);aaKills++;
 if(aaComboT>0)aaCombo++;else aaCombo=1;aaComboT=150;aaScore+=AAES[e->k].score*(aaCombo>8?8:aaCombo);}
static void aa_dmg(AEn*e,int d){if(!e->alive)return;e->hp-=d;e->flash=4;if(e->hp<=0)aa_kill(e);}
static void aa_hitbase(int x,int d){for(int b=0;b<3;b++)if(x>=BLDX[b][0]-4&&x<=BLDX[b][1]+4&&aaBld[b]>0){aaBld[b]-=d;if(aaBld[b]<=0){aaBld[b]=0;aa_toast("BASE BUILDING DESTROYED!");aa_drawall();dma32(dl_bg,(void*)0x06000000,240*160/2);aa_hud(1);}return;}}
static void aa_hitveh(int d){aaHP-=d;shake(5);sfx_hurt();if(aaHP<0)aaHP=0;}
static void aa_blast(int x,int y,int dmg){afx(x,y,1);sfx_boom();if(y>=AAGY-24){aa_hitbase(x,dmg);if(iabs(x-AAPX)<26)aa_hitveh(dmg/2+2);}}
static void aa_enemies(void){int px=AAPX,py=AAPY;
 for(int i=0;i<NAEN;i++){AEn*e=&AE[i];if(!e->alive)continue;const AAE*S=&AAES[e->k];int ex=e->x>>8,ey=e->y>>8;e->t++;if(e->flash)e->flash--;
  int tx=(BLDX[e->tgt][0]+BLDX[e->tgt][1])/2;if(aaBld[e->tgt]<=0)tx=AAPX;
  switch(S->beh){
   case EB_PLANE:if(S->strafe&&iabs(ex-px)<90&&--e->cd<=0){int dx=px-ex,dy=py-ey;int d=idist(dx,dy)+1;int ac=4-SET.bot;ap_new(5,e->x,e->y,(dx<<8)*3/d+rr(ac*30)-ac*15,(dy<<8)*3/d,90,3);e->cd=50-SET.bot*6;}break;
   case EB_BOMB:if(iabs(ex-tx)<30&&--e->cd<=0){ap_new(4,e->x,e->y+(6<<8),e->vx/2,0x40,200,e->k==2?14:10);e->cd=e->k==2?22:34;}break;
   case EB_DIVE:if(e->st==0&&iabs(ex-tx)<50){e->st=1;}if(e->st==1){e->vy+=14;if(e->vy>0x200)e->vy=0x200;if(ey>AAGY-50){ap_new(4,e->x,e->y,e->vx/2,e->vy/2,120,12);e->st=2;}}if(e->st==2){e->vy-=20;if(e->vy<-0x180)e->vy=-0x180;if(ey<30){e->vy=0;e->st=3;}}break;
   case EB_HELI:if(e->st==0&&iabs(ex-(e->dir>0?60+i*9%40:180-i*9%40))<6){e->st=1;e->vx=0;}if(e->st==1){e->vy=((e->t>>5)&1)?0x30:-0x30;if(--e->cd<=0){int dx=(rr(2)?px:tx)-ex,dy=AAGY-6-ey;int d=idist(dx,dy)+1;ap_new(6,e->x,e->y+(4<<8),(dx<<8)*2/d,(dy<<8)*2/d,140,8);e->cd=110-SET.bot*12;}
     if(e->t>900){e->st=2;e->vx=e->dir*S->spd;}}break;
   case EB_KAMI:if(iabs(ex-tx)<70||e->st){e->st=1;int dx=tx-ex,dy=AAGY-4-ey;int d=idist(dx,dy)+1;e->vx=(dx*S->spd*3/2)/d;e->vy=(dy*S->spd*3/2)/d;}
     if(ey>=AAGY-4){e->alive=0;aa_blast(ex,AAGY-2,12);}break;
   case EB_HUNT:{int dx=px-ex,dy=py-ey;int d=idist(dx,dy)+1;if(d<120||e->st){e->st=1;e->vx=(dx*S->spd)/d+(e->k==15?((e->t*7)%5-2)*24:0);e->vy=(dy*S->spd)/d;}if(d<10){e->alive=0;afx(ex,ey,1);sfx_boom();aa_hitveh(12);}}break;
   case EB_SPOT:if(iabs(ex-120)<80){e->vx=e->dir*0x30;if(++aaArtT>360){aaArtT=0;aa_toast("SPOTTER DRONE - ARTILLERY INBOUND!");ap_new(4,(BLDX[e->tgt][0]+10)<<8,0,0,0x180,200,10);}}break;
   case EB_MQ9:if(--e->cd<=0&&iabs(ex-px)<110){ap_new(7,e->x,e->y+(4<<8),0,0x100,260,15);e->cd=260-SET.bot*20;aa_toast("MISSILE LAUNCH!");}break;}
  e->x+=e->vx;e->y+=e->vy;if(e->y<(10<<8))e->y=10<<8;
  if((e->x>>8)<-50||(e->x>>8)>290){if(S->beh==EB_BOMB||S->beh==EB_PLANE||S->beh==EB_SPOT||S->beh==EB_MQ9||S->beh==EB_HELI||S->beh==EB_DIVE){e->dir=-e->dir;e->vx=-e->vx;e->st=0;e->vy=0;e->y=(16+rr(50))<<8;}else e->alive=0;}}}
// ---- projectiles
static int aa_near(int x,int y,int r){int bi=-1,bd=r;for(int i=0;i<NAEN;i++){AEn*e=&AE[i];if(!e->alive)continue;int d=idist((e->x>>8)+0-x,(e->y>>8)-y);if(d<bd){bd=d;bi=i;}}return bi;}
static void aa_flak(int x,int y,int r,int dmg){afx(x,y,0);sfx1(0x00,0x6,1,2,900,3);for(int i=0;i<NAEN;i++){AEn*e=&AE[i];if(!e->alive)continue;int d=idist((e->x>>8)-x,(e->y>>8)-y);if(d<r+8)aa_dmg(e,d<r/2?dmg:dmg*2/3);}
 for(int i=0;i<NAPR;i++){APr*p=&AP[i];if(p->alive&&(p->k==6||p->k==7)&&idist((p->x>>8)-x,(p->y>>8)-y)<r+4){p->alive=0;afx(p->x>>8,p->y>>8,1);aaScore+=25;}}}
static void aa_proj(void){const AAV*V=&AAVS[aaV];
 for(int i=0;i<NAPR;i++){APr*p=&AP[i];if(!p->alive)continue;int x=p->x>>8,y=p->y>>8;
  if(--p->life<=0){p->alive=0;if(p->k==1)aa_flak(x,y,p->r,p->dmg);if(p->k==2)afx(x,y,0);continue;}
  switch(p->k){
   case 0:{int hi=aa_near(x,y,12);if(hi>=0){AEn*e=&AE[hi];if(iabs((e->x>>8)-x)<14&&iabs((e->y>>8)-y)<7){aa_dmg(e,p->dmg);p->alive=0;afx(x,y,0);}}
     for(int j=0;j<NAPR&&p->alive;j++){APr*q=&AP[j];if(q->alive&&(q->k==6||q->k==7||q->k==4)&&iabs((q->x>>8)-x)<4&&iabs((q->y>>8)-y)<4){q->alive=0;p->alive=0;afx(x,y,0);aaScore+=25;}}
     p->vy+=2;break;}
   case 1:{p->vy+=3;int hi=aa_near(x,y,p->r*3/4);if(hi>=0){p->alive=0;aa_flak(x,y,p->r,p->dmg);}
     for(int j=0;j<NAPR&&p->alive;j++){APr*q=&AP[j];if(q->alive&&(q->k==6||q->k==7)&&idist((q->x>>8)-x,(q->y>>8)-y)<p->r/2){p->alive=0;aa_flak(x,y,p->r,p->dmg);}}break;}
   case 2:{ // homing missile (target index or flare index+100)
     int tx=-1,ty=0;if(p->decoy){APr*f=&AP[p->tgt];if(f->alive&&f->k==3){tx=f->x>>8;ty=f->y>>8;}else{p->decoy=0;p->tgt=255;}}
     else if(p->tgt<NAEN&&AE[p->tgt].alive){AEn*e=&AE[p->tgt];tx=e->x>>8;ty=e->y>>8;int d=idist(tx-x,ty-y);
      if(e->fl>0&&d<60&&AAES[e->k].flares){e->fl--;aa_toast("FLARES!");int fi=-1;for(int q=0;q<3;q++){APr*f=ap_new(3,e->x,e->y,-e->vx/2+(q-1)*0x60,0x40,70,0);if(f&&fi<0)fi=f-AP;}
       int ch=V->mis==AM_IR?60:V->mis==AM_IIR?12:V->mis==AM_RADAR?18:8;if(fi>=0&&rr(100)<ch){p->decoy=1;p->tgt=fi;}}
      if(d<9){aa_dmg(e,V->mis==AM_INT?18:24);p->alive=0;afx(x,y,1);}}
     else{int bi=-1,bd=999;for(int q=0;q<NAPR;q++){APr*o=&AP[q];if(o->alive&&(o->k==6||o->k==7)&&V->mis==AM_INT){int d=idist((o->x>>8)-x,(o->y>>8)-y);if(d<bd){bd=d;bi=q;}}}
      if(bi>=0){APr*o=&AP[bi];tx=o->x>>8;ty=o->y>>8;if(bd<8){o->alive=0;p->alive=0;afx(x,y,1);aaScore+=60;break;}}}
     if(p->decoy&&tx>=0&&idist(tx-x,ty-y)<6){p->alive=0;afx(x,y,0);break;}
     int sp=p->life>200?0x300:0x480;if(tx>=0){int dx=tx-x,dy=ty-y;int d=idist(dx,dy)+1;p->vx+=(dx*sp/d-p->vx)/6;p->vy+=(dy*sp/d-p->vy)/6;}
     if((p->life&3)==0)afx(x,y,2);break;}
   case 3:p->vy+=2;break;
   case 4:p->vy+=10;if(y>=AAGY-2){p->alive=0;aa_blast(x,AAGY-2,p->dmg);}break;
   case 5:if(iabs(x-AAPX)<16&&y>AAPY-10&&y<AAPY+12){p->alive=0;aa_hitveh(p->dmg);}if(y>AAGY+8)p->alive=0;break;
   case 6:if(y>=AAGY-2||(iabs(x-AAPX)<16&&y>AAPY-8)){p->alive=0;aa_blast(x,y,p->dmg);}if((p->life&3)==0)afx(x,y,2);break;
   case 7:{int dx=AAPX-x,dy=AAPY-y;int d=idist(dx,dy)+1;p->vx+=(dx*0x240/d-p->vx)/10;p->vy+=(dy*0x240/d-p->vy)/10;if(d<10){p->alive=0;afx(x,y,1);sfx_boom();aa_hitveh(p->dmg);}if((p->life&3)==0)afx(x,y,2);break;}}
  p->x+=p->vx;p->y+=p->vy;if(x<-20||x>260||y<-30||y>170)p->alive=0;}}
// lead + lock targeting
static int aa_aimtarget(int cone){int bi=-1,bd=cone;for(int i=0;i<NAEN;i++){AEn*e=&AE[i];if(!e->alive)continue;int dx=(e->x>>8)-AAPX,dy=AAPY-(e->y>>8);if(dy<4)continue;
  int ang=acos_deg(dx*256/(idist(dx,dy)+1));if(dx<0)ang=ang; // acos gives 0..101 in this helper; fall back to slope compare
  int a2=dx>=0?(dy*45/(dx+dy+1)):(180-dy*45/(-dx+dy+1)); (void)ang;
  int dd=iabs(a2*2-aaAng);if(dd<bd){bd=dd;bi=i;}}return bi;}
static void aa_fire(void){const AAV*V=&AAVS[aaV];int ca=COS(deg2a(aaAng)),sa=SIN(deg2a(aaAng));
 if(V->gun&&(keys&K_A)&&aaCd<=0&&!aaOver){aaCd=V->rate;int L=V->gun==AG_FLAK?20:16;s32 bx=(AAPX<<8)+ca*L,by=((AAPY-2)<<8)-sa*L;int spr=V->gun==AG_MG?24:12;
  int nb=V->gun==AG_FLAK?1:(V->barrels>2?2:1);for(int b=0;b<nb;b++){s32 vx=(ca*V->bspd>>8)+rr(spr)-spr/2,vy=-(sa*V->bspd>>8)+rr(spr)-spr/2;
   APr*p=ap_new(V->gun==AG_MG?0:1,bx,by,vx,vy,V->gun==AG_FLAK?70:V->gun==AG_AUTO?50:60,V->dmg);if(p)p->r=V->flakR?V->flakR:0;if(p&&V->gun==AG_AUTO&&V->radar&&aaLock>=0){int d=idist((AE[aaLock].x>>8)-AAPX,AAPY-(AE[aaLock].y>>8));p->life=clampi(d*256/V->bspd+4,10,70);}}
  aaHeat+=V->heat;if(aaHeat>=100){aaOver=1;aa_toast("OVERHEAT!");}sfx_shot(V->gun==AG_FLAK);if(V->gun==AG_FLAK)shake(2);}
 if(V->nmis&&aaRel<=0){if(keys&K_B){int t=aa_aimtarget(V->radar?80:55);if(t>=0&&t==aaLock)aaLockT++;else{aaLock=t;aaLockT=0;}
   if(t<0&&V->mis==AM_INT){aaLock=-1;}}
  if(!(keys&K_B)&&(prevk&K_B)){int need=V->mis==AM_RADAR?18:V->mis==AM_INT?8:28;if((aaLock>=0&&aaLockT>=need)||V->mis==AM_INT){
    APr*p=ap_new(2,AAPX<<8,(AAPY-6)<<8,(ca*0x200)>>8,-(sa*0x200)>>8,V->mis==AM_IIR?300:240,0);if(p)p->tgt=aaLock>=0&&aaLockT>=need?aaLock:255;sfx_whoosh();if(--aaMis<=0){aaRel=V->misRel;aa_toast("RELOADING MISSILES");}}aaLockT=0;}}
 else if(!V->nmis||aaRel>0){if(V->radar){aaLock=aa_aimtarget(50);}}}
static void aa_draw(void){aaNo=0;
 for(int i=0;i<NAEN;i++){AEn*e=&AE[i];if(!e->alive)continue;int f=(e->t>>2)&1;aspr((e->x>>8)-16,(e->y>>8)-8,(e->k*2+f)*8,1,2,e->dir<0,e->flash?1:0);}
 for(int i=0;i<NAPR;i++){APr*p=&AP[i];if(!p->alive)continue;static const u16 TT[8]={AT_TRACER,AT_SHELL,AT_MISSILE,AT_FLARE,AT_BOMB,AT_EBUL,AT_ROCKET,AT_MISSILE};aspr((p->x>>8)-4,(p->y>>8)-4,TT[p->k],0,0,0,0);}
 for(int i=0;i<NAFX;i++){AFx*f=&AF[i];if(!f->t)continue;int t=f->t;int tile=f->k==0?AT_BURST0+(t>12?8:t>5?4:0):f->k==1?AT_BOOM0+(t>16?8:t>7?4:0):AT_SMOKE;
  if(f->k==2)aspr(f->x-4,f->y-4-(t>>3),tile,0,0,0,0);else aspr(f->x-8,f->y-8,tile,0,1,0,0);if(++f->t>(f->k==0?20:f->k==1?24:30))f->t=0;}
 const AAV*V=&AAVS[aaV];
 if(aaLock>=0&&AE[aaLock].alive){AEn*e=&AE[aaLock];
  if(V->nmis&&(keys&K_B)){int need=V->mis==AM_RADAR?18:V->mis==AM_INT?8:28;aspr((e->x>>8)-8,(e->y>>8)-8,aaLockT>=need?AT_LOCK1:AT_LOCK0,0,1,0,0);}
  if(V->radar&&V->gun){int dx=(e->x>>8)-AAPX,dy=(e->y>>8)-AAPY;int d=idist(dx,dy);int t=d*256/(V->bspd?V->bspd:0x700);int lx=(e->x+e->vx*t)>>8,ly=(e->y+e->vy*t)>>8;aspr(lx-4,ly-4,AT_LEAD,0,0,0,0);}}
 for(int i=aaNo;i<128;i++)aaOam[i*4]=1<<9;}
static void aa_banner(const char*s,u16 c){lfb=(u16*)0x06000000;ltc(60,s,c);lfb=0;}
static void aa_unbanner(void){dma32(dl_bg+58*240,(void*)(0x06000000+58*480),240*12/2);}
static int aa_run(void){const AAV*V=&AAVS[aaV];memset(AE,0,sizeof AE);memset(AP,0,sizeof AP);memset(AF,0,sizeof AF);
 aaAng=90;aaHeat=0;aaOver=0;aaMis=V->nmis;aaRel=0;aaMaxHP=aaHP=V->hp;aaWave=1;aaScore=0;aaCombo=0;aaComboT=0;aaLock=-1;aaLockT=0;aaCd=0;aaArtT=0;aaToastT=0;aaKills=0;for(int b=0;b<3;b++)aaBld[b]=40;
 aaLeft=5+SET.bot;aaSpawnT=90;aaIntro=120;int toastShown=0,bannerOn=0;
 vsync();REG_DISPCNT=3|(1<<10);aa_drawall();dma32(dl_bg,(void*)0x06000000,240*160/2);dma32(aa_tiles,(void*)0x06014000,sizeof aa_tiles/4);
 for(int i=0;i<16;i++){PAL_OBJ[i]=aa_pal[i];PAL_OBJ[16+i]=i?RGB(255,255,255):0;}for(int i=0;i<128;i++)aaOam[i*4]=1<<9;
 REG_DISPCNT=3|(1<<6)|(1<<10)|(1<<12);bgm_t(TR_AA);aa_hud(1);int res=0,endT=0;
 for(int fr=0;;fr++){BEAT->screen=SCR_AA;BEAT->mode=aaEra;BEAT->menu=aaV;BEAT->hp=aaHP;BEAT->scoreA=aaScore;BEAT->scoreB=aaKills;BEAT->phase=aaWave;BEAT->timer=aaLeft;BEAT->x=aaAng;BEAT->y=aaBld[0]+aaBld[1]+aaBld[2];BEAT->extra=aaMis|(aaHeat<<8)|(aaOver<<16)|(aaLock<0?0xFF000000u:(u32)aaLock<<24);
  if(!res){
   int sp=(keys&(K_L|K_R))?1:3;if(keys&K_LEFT)aaAng+=sp;if(keys&K_RIGHT)aaAng-=sp;aaAng=clampi(aaAng,8,172);
   if(aaCd>0)aaCd--;if(aaHeat>0)aaHeat-=V->gun==AG_MG?1:(fr&1);if(aaOver&&aaHeat<40)aaOver=0;if(aaRel>0&&--aaRel==0){aaMis=V->nmis;aa_toast("MISSILES READY");}if(aaComboT>0&&--aaComboT==0)aaCombo=0;
   if(aaIntro>0){if(aaIntro==120){aa_banner(aaWave==10&&!aaMode?"FINAL WAVE 10!":cat("WAVE ",itoa(aaWave)),C_YEL);bannerOn=1;}if(--aaIntro==0){aa_unbanner();bannerOn=0;}}
   else{if(aaLeft>0&&--aaSpawnT<=0){int k=aa_pick();int n=k==12?3+rr(3):1;for(int q=0;q<n&&aaLeft>0;q++){int s=aa_spawn(k);if(s>=0&&q){AE[s].x=AE[s-q>=0?s-q:s].x-AE[s].dir*(q*22<<8);AE[s].y+=(q*6)<<8;}aaLeft--;}aaSpawnT=70+rr(60)-SET.bot*10-aaWave*2;if(aaSpawnT<25)aaSpawnT=25;}
    int alive=0;for(int i=0;i<NAEN;i++)alive+=AE[i].alive;
    if(aaLeft<=0&&!alive){if(!aaMode&&aaWave>=10){res=2;endT=fr;}else{aaWave++;aaLeft=4+aaWave*2+SET.bot*2;if(aaLeft>40)aaLeft=40;aaIntro=120;aaScore+=100*aaWave;if(aaHP<aaMaxHP)aaHP+=aaMaxHP/10;if(aaHP>aaMaxHP)aaHP=aaMaxHP;}}}
   aa_fire();aa_enemies();aa_proj();
   if(aaHP<=0||(aaBld[0]<=0&&aaBld[1]<=0&&aaBld[2]<=0)){res=1;endT=fr;}
   if(res){aa_banner(res==2?"10 WAVES HELD - VICTORY!":aaHP<=0?"VEHICLE DESTROYED":"BASE OVERRUN",res==2?C_GREEN:C_RED);if(res==2)music_victory();}}
  else if(fr-endT>150)break;
  aa_draw();
  next_frame();dma32(aaOam,(void*)0x07000000,128*2);aa_barrel();aa_hud(0);
  if(aaToastT>0){if(!toastShown||aaToastT==90){dma32(dl_bg+12*240,(void*)(0x06000000+12*480),240*9/2);lfb=(u16*)0x06000000;ltc(13,aaToast,C_YEL);lfb=0;toastShown=1;}if(--aaToastT==0){dma32(dl_bg+12*240,(void*)(0x06000000+12*480),240*9/2);toastShown=0;}}
  (void)bannerOn;
  if(pressed&K_START&&!res){vsync();lfb=(u16*)0x06000000;ltc(76,"PAUSED - START: RESUME  SELECT: QUIT",C_WHITE);lfb=0;for(;;){next_frame();if(pressed&K_START)break;if(pressed&K_SEL){res=1;endT=fr-150;break;}}dma32(dl_bg+74*240,(void*)(0x06000000+74*480),240*10/2);}}
 {volatile u16*O=(volatile u16*)0x07000000;for(int i=0;i<128;i++)O[i*4]=1<<9;}
 int slot=aaMode;int nb=aaScore>(int)aaBest[aaEra][slot];if(nb){aaBest[aaEra][slot]=aaScore;aa_save();}
 for(int fr=0;;fr++){BEAT->screen=SCR_AA;BEAT->phase=99;BEAT->scoreA=aaScore;
  if(fr==0){m3_title(1);m3_fill(16,30,208,100,C_PANEL);m3_box(16,30,208,100,2);m3_ctext(38,res==2?"VICTORY!":"MISSION OVER",res==2?C_YEL:C_RED,2);
   m3_ctext(62,cat("SCORE ",itoa(aaScore)),C_WHITE,1);m3_ctext(73,cat3("WAVE ",itoa(aaWave),cat("  KILLS ",itoa(aaKills))),C_WHITE,1);
   m3_ctext(86,nb?"NEW HIGH SCORE!":cat("BEST ",itoa(aaBest[aaEra][slot])),nb?C_GREEN:C_GRAY,1);m3_ctext(97,AAVS[aaV].n,C_BLUE,1);m3_ctext(114,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();if(fr>30&&(pressed&(K_A|K_START|K_B))){sfx_ok();return res;}}}
static void anti_air(void){static int era=1,sel=0,mode=0;aa_load();int redraw=1;bgm_t(TR_AA);
 for(;;){int list[NAAV],n=0;for(int v=0;v<NAAV;v++)if(AAVS[v].era==era)list[n++]=v;if(sel>=n)sel=n-1;int row=sel;
  BEAT->screen=SCR_AASEL;BEAT->mode=era;BEAT->menu=row;BEAT->extra=list[row<n?row:0]|(mode<<8);
  if(redraw){redraw=0;aaV=list[sel<n?sel:0];aaEra=era;aaAng=60;memset(aaBld,0,sizeof aaBld);for(int b=0;b<3;b++)aaBld[b]=40;
   lfb=0;aa_backdrop(era);lf(0,0,240,30,RGB(16,18,28));ltc(3,"ANTI-AIRCRAFT - DEFEND THE BASE",C_YEL);
   for(int k=0;k<4;k++){int x=8+k*58;lf(x,13,54,10,k==era?RGB(90,80,30):RGB(30,36,70));ltx(x+27-slen(AA_ERA[k])*3,15,AA_ERA[k],k==era?C_YEL:C_GRAY);}
   lf(4,34,150,n*9+6,RGB(16,18,28));for(int k=0;k<n;k++){ltx(8,37+k*9,k==sel?">":" ",C_YEL);ltx(16,37+k*9,AAVS[list[k]].n,k==sel?C_YEL:C_WHITE);}
   int y2=34+n*9+8;lf(4,y2,150,22,RGB(16,18,28));ltx(8,y2+2,cat("MODE: ",mode?"ENDLESS":"10-WAVE CHALLENGE"),C_GREEN);ltx(8,y2+12,cat("BEST: ",itoa(aaBest[era][mode])),C_GRAY);
   aa_veh(196,AAPY-4,aaV,1);
   lf(0,134,240,26,RGB(16,18,28));ltc(136,AAVS[aaV].d,C_WHITE);ltc(146,"L/R ERA U/D PICK SEL MODE A GO B BACK",C_GRAY);
   m3_present();}
  next_frame();
  if(pressed&(K_L|K_R)){era=(era+((pressed&K_R)?1:3))%4;sel=0;redraw=1;sfx_menu();}
  if(pressed&K_UP){sel=(sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&K_SEL){mode^=1;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();aaMode=mode;aaEra=era;aaV=list[sel];aa_run();bgm_t(TR_AA);redraw=1;}
  if(pressed&K_B){sfx_menu();return;}}}
