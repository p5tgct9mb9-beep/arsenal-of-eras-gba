// ===================================================================== v1.3.2 RACING (electric cars, Mode 7 style)
// Mode 1: BG2 = affine 1024x1024 track (8bpp, 128x128 map) re-projected EVERY SCANLINE by an HBlank DMA table
// (PA/PB/PC/PD/X/Y per line = SNES-style "Mode 7" floor), BG1 = sky panorama (4bpp), BG0 = HUD, OBJ = cars (affine scaled).
// Personal homebrew: cars are drawn in the game's own pixel style and named by model; no real logo artwork.
#include "racedb.inc"
#define SCR_RACE 90
#define SCR_RCAR 91
#define SCR_RTRK 92
#define SCR_RRES 93
#define REG_DMA0SAD REG32(0x40000B0)
#define REG_DMA0DAD REG32(0x40000B4)
#define REG_DMA0CNT REG32(0x40000B8)
typedef struct{const char*n,*d1,*d2;s32 vmax;s16 acc,hand,grip;u16 kg;u8 mph;}RCar;
static const RCar RCARS[RC_NCAR]={
 {"ROADSTER","2ND GEN HYPERCAR. FASTEST LAUNCH.","SPACEX PACKAGE: COLD-GAS THRUSTERS",14746,84,560,62,2000,250},
 {"MODEL S PLAID","TRI-MOTOR SEDAN. HUGE TOP END.","LUDICROUS-STYLE STRAIGHT-LINE PACE",14100,78,520,58,2160,200},
 {"MODEL 3","LIGHT COMPACT SEDAN.","NIMBLE: BEST HANDLING IN CLASS",12300,60,600,66,1800,160},
 {"MODEL X","FALCON-WING SUV. HEAVY + PLANTED.","QUICK OFF THE LINE FOR ITS SIZE",12700,64,420,54,2450,160},
 {"MODEL Y","CROSSOVER ALL-ROUNDER.","BALANCED - A GOOD FIRST PICK",12100,54,500,60,2000,155},
 {"CYBERTRUCK","STAINLESS PICKUP. A BATTERING RAM.","BULLIES LIGHTER CARS IN CONTACT",11900,60,400,50,3100,130},
 {"SEMI","ELECTRIC TRACTOR UNIT. VERY HEAVY.","SLOW BUT NOTHING MOVES IT",10200,30,300,44,9000,100},
 {"CYBERCAB","TWO-SEAT ROBOTAXI. LIGHT + AGILE.","SMOOTH: SLOW TOP SPEED, GRIPPY",11500,46,560,66,1500,120}};
enum{RM_GP,RM_TT,RM_FREE};
// ---- save block (SRAM 0x2200, own magic + checksum; older saves simply have no block = empty records)
typedef struct{u32 bestLap[4];u32 bestRace[4];u8 gpBest,gpWins,lastCar,spacex;u16 races,pad;}RSave;
static RSave RS EWRAM;static int rs_loaded;
#define RS_OFF 0x2200
static void rs_save(void){const u8*b=(const u8*)&RS;u8 sum=0x52;SRAM[RS_OFF]='A';SRAM[RS_OFF+1]='O';SRAM[RS_OFF+2]='E';SRAM[RS_OFF+3]='R';SRAM[RS_OFF+4]=1;SRAM[RS_OFF+5]=sizeof RS;
 for(unsigned i=0;i<sizeof RS;i++){SRAM[RS_OFF+8+i]=b[i];sum+=b[i]*(i+5);}SRAM[RS_OFF+7]=sum;}
static void rs_load(void){if(rs_loaded)return;rs_loaded=1;memset(&RS,0,sizeof RS);RS.spacex=1;
 if(SRAM[RS_OFF]!='A'||SRAM[RS_OFF+1]!='O'||SRAM[RS_OFF+2]!='E'||SRAM[RS_OFF+3]!='R'||SRAM[RS_OFF+4]!=1||SRAM[RS_OFF+5]!=sizeof RS)return;
 u8 b[sizeof(RSave)];u8 sum=0x52;for(unsigned i=0;i<sizeof RS;i++){b[i]=SRAM[RS_OFF+8+i];sum+=b[i]*(i+5);}if(sum!=SRAM[RS_OFF+7])return;memcpy(&RS,b,sizeof RS);if(RS.lastCar>=RC_NCAR)RS.lastCar=0;}
static const char*rtime(u32 f){char*o=nb[nbi++&3];u32 cs=f*100/60;u32 m=cs/6000,s=(cs/100)%60,c=cs%100;int k=0;o[k++]='0'+m%10;o[k++]=':';o[k++]='0'+s/10;o[k++]='0'+s%10;o[k++]='.';o[k++]='0'+c/10;o[k++]='0'+c%10;o[k]=0;return o;}
// ---- fine trig (16-bit angle, 0 = north / -y, clockwise), +-256
static inline int rsin(u32 a){a&=0xFFFF;int i=a>>8,f=a&255;int s0=sin_lut[i],s1=sin_lut[(i+1)&255];return s0+(((s1-s0)*f)>>8);}
static inline int rcos(u32 a){return rsin(a+16384);}
static inline int ang16(int dx,int dy){ // atan2 in 16-bit units (0 = -y, clockwise)
 if(!dx&&!dy)return 0;int ax=iabs(dx),ay=iabs(dy);int t;if(ax<=ay)t=atan_lut[(ax<<6)/ay];else t=64-atan_lut[(ay<<6)/ax];t<<=8; // t: 0..16384
 if(dx>=0&&dy<=0)return t;if(dx>=0)return 32768-t;if(dy>0)return 32768+t;return (65536-t)&0xFFFF;}
static inline int ad16(int a,int b){return (s16)(u16)(a-b);}
// ---- race state
#define RQ 12
typedef struct{s32 x,y,vx,vy;u16 h;s16 z,vz;u8 car,ai,air,surf,drift,boost,fin,place,cp,resp;s16 lane,driftT;int wp,lap,finT,lapStart,lastLap,bestLap,bat,respT,skill,boostT,stuck;int prog;s16 line,ovr,band;}RC;
static int rcDiff=1; // v1.3.3 GP difficulty 0 EASY 1 NORMAL 2 HARD (saved in RS.pad as diff+1)
static const char*const RC_DN[3]={"EASY","NORMAL","HARD"};
static RC rc[8] EWRAM;static int rcN,rcTrk,rcMode,rcT,rcPhase,rcLaps,rcNW,rcHalf,rcMsgT,rcPlayerCar,rcSpacex,rcRecLap;static const char*rcMsg;static const short*rcW;static const u8*rcS;
static const u8*rcSurf;static s32 camX,camY;static u16 camH;static int rcOrder[8];static int rcPts[8];static int rcAutoQuit;
typedef struct{s16 pa,pb,pc,pd;s32 x,y;}HLine;
static HLine hlt[2][230] EWRAM;static int hlb;
#define HOR 48
#define CAMH 30
#define FOC 200
#define CAMD 64
static s32 rdist[160] EWRAM;
static void rc_dist_init(void){for(int y=0;y<160;y++){if(y<=HOR){rdist[y]=0;continue;}rdist[y]=(CAMH*FOC*256)/((y-HOR)*2-1)*2;}} // Q8 px
enum{SF_ROAD_,SF_CURB_,SF_OFF_,SF_WALL_,SF_PAD_,SF_RAMP_,SF_WATER_};
static inline int rsurf(s32 x,s32 y){int tx=(x>>(RQ+3)),ty=(y>>(RQ+3));if(tx<0||ty<0||tx>127||ty>127)return SF_WALL_;return rcSurf[ty*128+tx];}
static void rc_table(void){HLine*T=hlt[hlb^1];int c=rcos(camH),s=rsin(camH);s32 cx=camX>>(RQ-8),cy=camY>>(RQ-8);
 for(int y=0;y<160;y++){HLine*L=&T[y];L->pb=0;L->pd=0;
  if(y<HOR){L->pa=0;L->pc=0;L->x=(127*8+4)<<8;L->y=(127*8+4)<<8;continue;}
  s32 d=rdist[y];if(d>1150*256||d<=0){L->pa=0;L->pc=0;L->x=(126*8+4)<<8;L->y=(127*8+4)<<8;continue;}
  s32 sc=d/FOC; // Q8 texels per screen pixel
  int pa=(sc*c)>>8,pc=(sc*s)>>8;L->pa=pa;L->pc=pc;
  L->x=cx+((d>>8)*s)-120*pa;L->y=cy-((d>>8)*c)-120*pc;}
 for(int y=160;y<230;y++)T[y]=T[159];}
static void rc_vbl(void){ // call right after vsync(): latch line 0, restart the HBlank DMA from line 1
 hlb^=1;HLine*T=hlt[hlb];REG_DMA0CNT=0;REG_BG2PA=T[0].pa;REG_BG2PB=0;REG_BG2PC=T[0].pc;REG_BG2PD=0;REG_BG2X=T[0].x;REG_BG2Y=T[0].y;
 REG_DMA0SAD=(u32)&T[1];REG_DMA0DAD=0x04000020;REG_DMA0CNT=4|(3u<<21)|(1u<<25)|(1u<<26)|(2u<<28)|(1u<<31);}
static void rc_stop(void){REG_DMA0CNT=0;aff_reset();REG_BLDCNT=0;}
// ---- setup
static void rc_setup(int t){
 vsync();REG_DISPCNT=0x80;REG_DMA0CNT=0;
 dma32(rc_tiles[t],CHARBLOCK(1),rc_ntile[t]*16);dma32(rc_mapv[t],SCREENBLOCK(16),128*128/4);
 for(int i=0;i<128;i++)PAL_BG[i]=rc_palv[t][i];PAL_BG[0]=rc_skyp[t][1];
 for(int i=0;i<16;i++)PAL_BG[128+i]=rc_skyp[t][i];
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(rc_skyt[t],CHARBLOCK(3),rc_skyn[t]*8);
 {volatile u16*m=SCREENBLOCK(29);for(int y=0;y<32;y++)for(int x=0;x<32;x++){int r=y<6?y:5;m[y*32+x]=rc_skym[t][r*32+x]|(8<<12);}}
 dma32(rc_cart,OBJ_TILES,sizeof(rc_cart)/4);dma32(rc_fxt,OBJ_TILES+RC_NCAR*24*8,sizeof(rc_fxt)/4);
 for(int i=0;i<RC_NCAR*16;i++)PAL_OBJ[i]=rc_carp[i];for(int i=0;i<16;i++)PAL_OBJ[8*16+i]=rc_fxp[i];
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=3|(3<<2)|(29<<8);REG_BGCNT(2)=2|(1<<2)|(16<<8)|(1<<13)|(3<<14);
 REG_BGHOFS(0)=0;REG_BGVOFS(0)=0;REG_BGVOFS(1)=0;hud_clear();oam_begin();
 REG_BLDCNT=(1<<10)|(1<<9);REG_BLDALPHA=7|(9<<8);rc_dist_init();shimmer=0;}
#define FXT (RC_NCAR*24)
// ---- cars
static void rc_place(void){int nw=rcNW;
 for(int i=0;i<rcN;i++){RC*c=&rc[i];int k=(i/2)+1;int back=(k*24+6)/16;int w=(nw-back)%nw;int w2=(w+1)%nw;
  int x0=rcW[w*2],y0=rcW[w*2+1],x1=rcW[w2*2],y1=rcW[w2*2+1];int h=ang16(x1-x0,y1-y0);int sd=(i&1)?1:-1;int off=rc_halfw[rcTrk]*42/100;
  int frac=(k*24+6)%16; // extra offset along the segment
  int px=x0+(rcos(h)*sd*off>>8)-(rsin(h)*frac>>8),py=y0+(rsin(h)*sd*off>>8)+(rcos(h)*frac>>8);
  c->x=px<<RQ;c->y=py<<RQ;c->vx=c->vy=0;c->h=h;c->z=0;c->vz=0;c->air=0;c->wp=w;c->lap=0;c->cp=1;c->fin=0;c->finT=0;c->lapStart=0;c->lastLap=0;c->bestLap=0;
  c->bat=600;c->drift=0;c->driftT=0;c->boost=0;c->boostT=0;c->resp=0;c->respT=0;c->lane=((i*37)%5-2)*rc_halfw[rcTrk]/5;c->stuck=0;c->prog=0;}}
static int rc_progress(RC*c){return (c->fin?100000-c->place*1000:0)+c->lap*rcNW+c->wp;}
static void rc_track_wp(RC*c){int nw=rcNW;int px=c->x>>RQ,py=c->y>>RQ;
 for(int it=0;it<6;it++){int a=c->wp,b=(a+1)%nw;int da=idist(rcW[a*2]-px,rcW[a*2+1]-py),db=idist(rcW[b*2]-px,rcW[b*2+1]-py);
  if(db<=da){c->wp=b;if(b==rcHalf)c->cp=1;
   if(b==0&&c->cp){c->cp=0;if(c->lap>0){int lt=rcT-c->lapStart;c->lastLap=lt;if(!c->bestLap||lt<c->bestLap)c->bestLap=lt;}c->lap++;c->lapStart=rcT;
    if(rcMode!=RM_FREE&&c->lap>rcLaps&&!c->fin){c->fin=1;c->finT=rcT;int p=1;for(int j=0;j<rcN;j++)if(&rc[j]!=c&&rc[j].fin)p++;c->place=p;}}}
  else break;}
 int a=c->wp,b=(a+nw-1)%nw;if(idist(rcW[b*2]-px,rcW[b*2+1]-py)+6<idist(rcW[a*2]-px,rcW[a*2+1]-py)&&a!=0)c->wp=b;}
// v1.3.3 AI: racing line (outside -> apex -> outside), speed-scaled look-ahead, straight-line boost use with a battery
// reserve, difficulty-scaled corner commitment; the AUTO test driver (rc[0] under FR_AUTO) uses the HARD line as a
// stand-in for a skilled human.
// v1.3.3 pace: rivals' top speed + accel are pulled toward the player's car (so a fast pick doesn't trivialise the cup)
static void rc_pace(const RC*c,s32*vmax,int*acc){const RCar*C=&RCARS[c->car];*vmax=C->vmax;*acc=C->acc;if(c==&rc[0])return;const RCar*P=&RCARS[rc[0].car];
 static const s16 K[3]={100,220,256};static const s16 M[3]={246,266,272};int k=K[rcDiff];
 s32 v=C->vmax+((P->vmax-C->vmax)*k>>8);int a=C->acc+((P->acc-C->acc)*k>>8);if(v<C->vmax&&rcDiff)v=C->vmax;if(a<C->acc&&rcDiff)a=C->acc;*vmax=(v*M[rcDiff]>>8)*(256+c->band/2)>>8;*acc=a*M[rcDiff]>>8;}
static void rc_ai(RC*c,int*thr,int*brk,int*steer,int*drift,int*boost){int nw=rcNW;
 int me=c==&rc[0];int D=me?2:rcDiff;
 int vf=(c->vx*rsin(c->h)-c->vy*rcos(c->h))>>8;
 int look=2+clampi(vf/3200,0,3);int t=(c->wp+look)%nw;
 int tx=rcW[t*2],ty=rcW[t*2+1];int t2=(t+1)%nw;int hx=rcW[t2*2]-tx,hy=rcW[t2*2+1]-ty;int hh=ang16(hx,hy);
 // racing line: sign of the heading change over the next few waypoints = turn direction (positive = right)
 {int a0=(c->wp+1)%nw,a1=(c->wp+2)%nw,b0=(c->wp+6)%nw,b1=(c->wp+7)%nw;
  int dh=ad16(ang16(rcW[b1*2]-rcW[b0*2],rcW[b1*2+1]-rcW[b0*2+1]),ang16(rcW[a1*2]-rcW[a0*2],rcW[a1*2+1]-rcW[a0*2+1]));
  int hw=rc_halfw[rcTrk]*11/20;int str=D==0?0:D==1?7:10;int tl=clampi(dh*hw/5000,-hw,hw)*str/10;
  c->line+=(tl-c->line)/6;if(c->ovr>0)c->ovr--;else if(c->ovr<0)c->ovr++;}
 int lane=(D==0?c->lane:c->lane/3)+c->line+c->ovr;
 tx+=rcos(hh)*lane>>8;ty+=rsin(hh)*lane>>8;
 int want=ang16(tx-(c->x>>RQ),ty-(c->y>>RQ));int e=ad16(want,c->h);*steer=clampi(e/(D==2?20:24),-256,256);
 int sp=255;for(int k=1;k<6;k++){int v=rcS[(c->wp+k)%nw];if(v<sp)sp=v;}
 const RCar*C=&RCARS[c->car];s32 pv;int pa;rc_pace(c,&pv,&pa);(void)pa;s32 tgt=(pv*sp>>8)*c->skill>>8;
 // corners: weaker handling slows more; harder drivers commit more corner speed
 if(sp<240)tgt=tgt*(200+C->hand/12+D*8)/256;if(tgt>pv)tgt=pv;
 *thr=vf<tgt;*brk=vf>tgt+900;*drift=0;*boost=0;
 // boost on straights; keep a reserve (EASY rarely boosts)
 {static const s16 RES[3]={420,220,120};static const u8 ODD[3]={90,14,5};int strt=1;for(int k=1;k<9;k++)if(rcS[(c->wp+k)%nw]<238){strt=0;break;}
  if(strt&&c->bat>RES[D]&&!c->boostT&&rr(ODD[D])==0)c->boostT=D==0?30:50;}
 if(c->boostT){c->boostT--;*boost=1;}
 if(iabs(e)>9000&&vf>6000)*brk=1;
 // overtaking: move off the line away from a car close ahead
 for(int j=0;j<rcN;j++){RC*o=&rc[j];if(o==c)continue;int dx=(o->x-c->x)>>RQ,dy=(o->y-c->y)>>RQ;if(iabs(dx)>40||iabs(dy)>40)continue;
  int fwd=(dx*rsin(c->h)-dy*rcos(c->h))>>8,lat=(dx*rcos(c->h)+dy*rsin(c->h))>>8;
  if(fwd>4&&fwd<34&&iabs(lat)<12){int hw=rc_halfw[rcTrk]*6/10;c->ovr=clampi(c->ovr+(lat>0?-3:3),-hw,hw);if(D==2&&fwd<20&&c->bat>80&&!c->boostT)c->boostT=20;}
  if(D==0){if(fwd>4&&fwd<34&&iabs(lat)<12){int hw=rc_halfw[rcTrk]*6/10;c->lane+=lat>0?-2:2;c->lane=clampi(c->lane,-hw,hw);}}}
 if(c->stuck>40){*steer=e>0?256:-256;}}
static void rc_physics(RC*c,int thr,int brk,int steer,int drift,int boost,int me){const RCar*C=&RCARS[c->car];
 if(c->resp){if(--c->respT<=0){c->resp=0;int w=c->wp,w2=(w+1)%rcNW;c->x=rcW[w*2]<<RQ;c->y=rcW[w*2+1]<<RQ;c->h=ang16(rcW[w2*2]-rcW[w*2],rcW[w2*2+1]-rcW[w*2+1]);c->vx=c->vy=0;c->z=0;c->air=0;}return;}
 int fx=rsin(c->h),fy=-rcos(c->h),rx=rcos(c->h),ry=rsin(c->h);
 s32 vf=((s64)c->vx*fx+(s64)c->vy*fy)>>8,vl=((s64)c->vx*rx+(s64)c->vy*ry)>>8;
 int sf=c->air?SF_ROAD_:rsurf(c->x,c->y);c->surf=sf;
 s32 vmax;int acc;rc_pace(c,&vmax,&acc);
 if(sf==SF_OFF_){vmax=vmax*56/100;}
 int boosting=0;if(boost&&c->bat>0){boosting=1;int sx=(me&&c->car==0&&rcSpacex);c->bat-=sx?9:7;vmax=vmax*(sx?135:125)/100;acc=acc*(sx?27:22)/10;if(c->bat<0)c->bat=0;}
 c->boost=boosting;
 if(!c->air){
  if(thr){int f=vf<vmax/2?256:(vmax-vf)*512/(vmax+1);if(f<0)f=0;vf+=acc*f>>8;}
  else if(!brk)vf-=vf/180;
  if(vf>vmax)vf-=(vf-vmax)/16+20;
  if(brk){if(vf>0){if(vf>1500)c->bat+=3;vf-=170;if(vf<0)vf=0;}else vf-=40;}
  if(vf<-3000)vf=-3000;
  if(sf==SF_OFF_&&vf>0)vf-=vf/90;
  int dr=drift&&vf>5000&&steer!=0;
  if(dr){c->drift=1;c->driftT++;c->bat+=2;}else{if(c->drift&&c->driftT>40){vf+=1300;if(me){sfx_whoosh();rcMsg="DRIFT BOOST!";rcMsgT=50;}}c->drift=0;c->driftT=0;}
  int sp=iabs(vf);int tk=sp>7000?256:sp*256/7000;int turn=steer*C->hand/256*tk/256;if(dr)turn=turn*145/100;if(vf<0)turn=-turn;c->h+=turn;
  int g=C->grip;if(dr)g=g*40/100;g=g*(150+106*rc_grav[rcTrk]/256)/256;vl-=vl*g/256;
  if(sf==SF_PAD_){c->bat+=12;if(me&&(rcT&31)==0){rcMsg="SUPERCHARGING";rcMsgT=40;}}
  if(sf==SF_RAMP_&&vf>4000){c->air=1;c->vz=(vf*256/22)>>8;if(me){sfx_whoosh();rcMsg=rcTrk==3?"LOW-G JUMP!":"JUMP!";rcMsgT=60;}}
  if(sf==SF_WATER_){c->resp=1;c->respT=70;if(me){sfx_boom();rcMsg="SPLASH! RESPAWNING";rcMsgT=70;}return;}
 }else{ // airborne: keep momentum; the SpaceX thrusters still push
  if(boosting&&me&&c->car==0&&rcSpacex)vf+=40;
  int g=64*rc_grav[rcTrk]/256;c->vz-=g;c->z+=c->vz;if(c->z<=0){c->z=0;c->air=0;c->vz=0;if(me)shake(4);}}
 if(c->bat>1000)c->bat=1000;
 fx=rsin(c->h);fy=-rcos(c->h);rx=rcos(c->h);ry=rsin(c->h);
 c->vx=((s64)vf*fx+(s64)vl*rx)>>8;c->vy=((s64)vf*fy+(s64)vl*ry)>>8;
 s32 nx=c->x+c->vx,ny=c->y+c->vy;
 if(!c->air&&rsurf(nx,ny)==SF_WALL_){int bx=rsurf(nx,c->y)==SF_WALL_,by=rsurf(c->x,ny)==SF_WALL_;
  if(bx||!by){c->vx=-c->vx/3;nx=c->x;}if(by||!bx){c->vy=-c->vy/3;ny=c->y;}if(me&&iabs(vf)>3000){sfx_shot(1);shake(5);}}
 if(c->air&&rsurf(nx,ny)==SF_WALL_&&(c->z>>8)<6){c->vx=-c->vx/3;c->vy=-c->vy/3;nx=c->x;ny=c->y;}
 if(iabs(nx-c->x)+iabs(ny-c->y)<300&&thr)c->stuck++;else c->stuck=0;
 c->x=nx;c->y=ny;}
static void rc_collide(void){for(int i=0;i<rcN;i++)for(int j=i+1;j<rcN;j++){RC*a=&rc[i],*b=&rc[j];if(a->resp||b->resp)continue;if(iabs(a->z-b->z)>6*256)continue;
 int dx=(b->x-a->x)>>8,dy=(b->y-a->y)>>8; // Q4 px
 if(iabs(dx)>160||iabs(dy)>160)continue;int d=isq(dx*dx+dy*dy);if(d>=144||d==0)continue;
 int ma=RCARS[a->car].kg,mb=RCARS[b->car].kg;int push=(144-d);int nx=dx*256/d,ny=dy*256/d;
 a->x-=(nx*push*mb/(ma+mb))<<0;a->y-=(ny*push*mb/(ma+mb));b->x+=(nx*push*ma/(ma+mb));b->y+=(ny*push*ma/(ma+mb));
 s32 va=(a->vx*nx+a->vy*ny)>>8,vb=(b->vx*nx+b->vy*ny)>>8;if(va-vb>0){s32 dv=va-vb;
  a->vx-=(dv*nx>>8)*mb/(ma+mb);a->vy-=(dv*ny>>8)*mb/(ma+mb);b->vx+=(dv*nx>>8)*ma/(ma+mb);b->vy+=(dv*ny>>8)*ma/(ma+mb);
  if(i==0||j==0){sfx_shot(0);}}}}
static void rc_rank(void){for(int i=0;i<rcN;i++){rcOrder[i]=i;rc[i].prog=rc_progress(&rc[i]);}
 for(int i=0;i<rcN;i++)for(int j=i+1;j<rcN;j++){RC*a=&rc[rcOrder[i]],*b=&rc[rcOrder[j]];int sw=0;
  if(a->fin&&b->fin)sw=b->place<a->place;else if(b->fin)sw=1;else if(!a->fin&&b->prog>a->prog)sw=1;
  if(sw){int t=rcOrder[i];rcOrder[i]=rcOrder[j];rcOrder[j]=t;}}
 for(int i=0;i<rcN;i++)if(!rc[rcOrder[i]].fin)rc[rcOrder[i]].place=i+1;}
// ---- sprites
static int rc_mat(int k,int sc,int flip){if(k>=32)return 31;int p=256*256/(sc<40?40:sc);if(p>32767)p=32767;
 oam[(k*4+0)*4+3]=(u16)(s16)(flip?-p:p);oam[(k*4+1)*4+3]=0;oam[(k*4+2)*4+3]=0;oam[(k*4+3)*4+3]=(u16)(s16)p;return k;}
static void rc_obj(int cx,int cy,int tile,int shape,int sz,int pal,int mat,int prio,int semi){ // affine double-size, centred
 if(noam>=128)return;int w=shape==1?(sz==2?32:sz==1?32:16):(8<<sz),h=shape==1?(sz==2?16:sz==1?8:8):(8<<sz);
 int x=cx-w,y=cy-h;if(x<-2*w||x>240||y<-2*h||y>160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(1<<8)|(1<<9)|(semi?1<<10:0)|(shape<<14);o[1]=(x&511)|(mat<<9)|(sz<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static void rc_spr_plain(int x,int y,int tile,int pal,int prio){if(noam>=128||x<-8||x>=240||y<-8||y>=160)return;u16*o=&oam[noam*4];o[0]=(y&255);o[1]=(x&511);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static void rc_sprites(void){oam_begin();int fx=rsin(camH),fy=-rcos(camH),rx=rcos(camH),ry=rsin(camH);
 int idx[8],zz[8],n=0;
 for(int i=0;i<rcN;i++){RC*c=&rc[i];if(c->resp&&(rcT&4))continue;int dx=(c->x-camX)>>(RQ-4),dy=(c->y-camY)>>(RQ-4); // Q4
  int z=(dx*fx+dy*fy)>>8;if(z<16*8)continue;if(z>16*900)continue;idx[n]=i;zz[n]=z;n++;}
 for(int a=0;a<n;a++)for(int b=a+1;b<n;b++)if(zz[b]<zz[a]){int t=zz[a];zz[a]=zz[b];zz[b]=t;t=idx[a];idx[a]=idx[b];idx[b]=t;}
 int shx[8],shy[8],shm[8],ns=0;
 for(int k=0;k<n;k++){RC*c=&rc[idx[k]];int dx=(c->x-camX)>>(RQ-4),dy=(c->y-camY)>>(RQ-4);int z=zz[k];int xl=(dx*rx+dy*ry)>>8;
  int sx=120+xl*FOC/z,sy=HOR+CAMH*FOC*16/z;int sc=11*FOC*16*256/(28*z); // Q8 sprite scale
  if(sc>512)sc=512;
  int bear=ang16(xl,-z);int rel=ad16(c->h,(camH+bear)&0xFFFF);int ar=iabs(rel);int view=ar<4096?0:ar<13000?1:2;int flip=rel>0;if(view==0)flip=0;
  int m=rc_mat(k,sc,flip);int lift=(c->z>>8)*FOC*16/z;
  rc_obj(sx,sy-(8*sc>>8)-lift,(c->car*3+view)*8,1,2,c->car,m,1,0);
  shx[ns]=sx;shy[ns]=sy-(2*sc>>8);shm[ns]=m;ns++;
  if(idx[k]==0){ // player FX
   if(c->boost){int gas=c->car==0&&rcSpacex;for(int q=0;q<2;q++){int ox=(q?10:-10)*sc>>8;rc_spr_plain(sx+ox-4+(rcT&1),sy-(6*sc>>8)+(rcT&3),FXT+4+(gas?1:3),8,1);}
    for(int q=0;q<6;q++){int yy=60+((q*37+rcT*9)%90);int xx=(q&1)?6+((rcT*5+q*13)%20):214-((rcT*5+q*11)%20);rc_spr_plain(xx,yy,FXT+7,8,0);}}
   if(c->drift&&(rcT&2)){rc_spr_plain(sx-14*sc/256,sy-6,FXT+6,8,1);rc_spr_plain(sx+8*sc/256,sy-6,FXT+6,8,1);}
   if(c->surf==SF_OFF_&&!c->air&&(rcT&3)==0){rc_spr_plain(sx-4+((rcT>>2)&7)-3,sy-10,FXT+4,8,1);}}}
 for(int k=0;k<ns;k++)rc_obj(shx[k],shy[k],FXT,1,1,8,shm[k],2,1);}
// ---- HUD
static const char*const PLC[9]={"","1ST","2ND","3RD","4TH","5TH","6TH","7TH","8TH"};
static void rc_hud(void){hud_clear();RC*p=&rc[0];
 if(rcMode!=RM_FREE){int l=p->lap<1?1:p->lap>rcLaps?rcLaps:p->lap;htxt(1,0,cat3("LAP ",itoa(l),cat("/",itoa(rcLaps))),HB_WHITE);}else htxt(1,0,cat("LAP ",itoa(p->lap<1?1:p->lap)),HB_WHITE);
 htxt(11,0,rtime(rcT>0?rcT:0),HB_YEL);
 if(rcN>1){htxt(23,0,"POS",HB_GRAY);htxt(27,0,PLC[p->place],p->place==1?HB_YEL:HB_WHITE);}
 if(p->lastLap)htxt(1,1,cat("LAST ",rtime(p->lastLap)),HB_GRAY);
 {u32 b=RS.bestLap[rcTrk];if(p->bestLap&&(!b||(u32)p->bestLap<b))b=p->bestLap;if(b)htxt(17,1,cat("BEST ",rtime(b)),HB_GRAY);}
 int vf=(p->vx*rsin(p->h)-p->vy*rcos(p->h))>>8;if(vf<0)vf=-vf;int mph=vf*RCARS[p->car].mph/(RCARS[p->car].vmax);
 htxt(1,19,p->car==0&&rcSpacex?"THR":"BAT",p->bat<150?HB_RED:HB_GREEN);hbar(5,19,8,p->bat,1000);
 htxt(22,19,itoa(mph),HB_WHITE);htxt(26,19,"MPH",HB_GRAY);
 if(rcPhase==0){int s=3-(rcT+180)/60;hpanel(11,7,8,3);htxt(14,8,s>0?itoa(s):"GO!",HB_YEL);}
 else if(rcMsgT>0&&rcMsg){hctxt(5,rcMsg,HB_YEL,0);}
 if(rcPhase==2){hpanel(5,8,20,4);hctxt(9,rcMode==RM_TT?"TIME TRIAL DONE":cat("FINISHED ",PLC[p->place]),HB_YEL,1);hctxt(10,rtime(p->finT),HB_WHITE,1);}}
static int rc_pause(void){int sel=0;static const char*const PO[3]={"RESUME","RESTART","QUIT"};
 for(;;){hud_clear();hpanel(9,6,12,7);for(int i=0;i<3;i++)hptxt(11,7+i*2,cat(i==sel?">":" ",PO[i]),i==sel?HB_YEL:HB_WHITE);
  vsync();rc_vbl();dma32(hud,SCREENBLOCK(28),512);poll();BEAT->screen=SCR_RACE;BEAT->phase=9;BEAT->menu=sel;frame++;BEAT->frame=frame;
  if(pressed&K_UP){sel=(sel+2)%3;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%3;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();return sel;}if(pressed&K_B)return 0;}}
// returns: 1 finished (rc[] holds results), 0 quit, 2 restart
static int rc_race(int trk,int mode,int car,int nAI){
 rcTrk=trk;rcMode=mode;rcLaps=mode==RM_FREE?99:3;rcW=rc_wpv[trk];rcS=rc_spdv[trk];rcNW=rc_nwp[trk];rcHalf=rcNW/2;rcSurf=rc_surfv[trk];rcN=1+nAI;
 rc[0].car=car;rc[0].ai=FR_AUTO?1:0;int k=1;for(int i=0;i<RC_NCAR&&k<rcN;i++)if(i!=car){rc[k].car=i;rc[k].ai=1;k++;}
 static const s16 SK[3]={234,250,260};for(int i=0;i<rcN;i++){rc[i].skill=SK[rcDiff]+((i*7)%9)-4;rc[i].line=0;rc[i].ovr=0;rc[i].band=0;}rc[0].skill=256;
 rc_place();rc_setup(trk);rcT=-180;rcPhase=0;rcMsgT=0;rcMsg=0;rcRecLap=0;int endT=0;
 camH=rc[0].h;camX=rc[0].x-(rsin(camH)*CAMD<<(RQ-8));camY=rc[0].y+(rcos(camH)*CAMD<<(RQ-8));rc_table();hlb^=1;rc_table();
 bgm_t(trk>=2?TR_RACE2:TR_RACE);
 REG_DISPCNT=1|(1<<6)|(1<<8)|(1<<9)|(1<<10)|(1<<12);
 for(;;){
  RC*p=&rc[0];
  // input
  int thr=0,brk=0,steer=0,drift=0,boost=0;
  if(rcPhase>=1){
   if(p->ai||p->fin){rc_ai(p,&thr,&brk,&steer,&drift,&boost);if(p->fin){boost=0;}}
   else{thr=(keys&K_A)!=0;brk=(keys&K_B)!=0;steer=(keys&K_LEFT)?-256:(keys&K_RIGHT)?256:0;drift=(keys&K_R)!=0;boost=(keys&K_L)!=0;}
   if(!FR_AUTO&&(pressed&K_START)){int r=rc_pause();if(r==1){rc_stop();return 2;}if(r==2){rc_stop();return 0;}}}
  if(rcPhase>=1){rc_physics(p,thr,brk,steer,drift,boost,1);
   for(int i=1;i<rcN;i++){int t_,b_,s_,d_,o_;rc_ai(&rc[i],&t_,&b_,&s_,&d_,&o_);
    // v1.3.3 rubber band (continuous): cars behind the player push harder (+battery trickle), cars ahead ease off
    {int gap=rc[0].prog-rc[i].prog;static const u8 UPB[3]={6,16,22},DNB[3]={18,10,4};int adj=gap>0?clampi(gap*48/rcNW,0,UPB[rcDiff]):-clampi(-gap*48/rcNW,0,DNB[rcDiff]);
     rc[i].skill=SK[rcDiff]+((i*7)%9)-4+adj;rc[i].band=adj;if(gap>rcNW/8&&rcDiff&&(rcT&3)==0&&rc[i].bat<600)rc[i].bat++;}
    rc_physics(&rc[i],t_,b_,s_,d_,o_,0);}
   rc_collide();for(int i=0;i<rcN;i++)rc_track_wp(&rc[i]);rc_rank();}
  // lap messages / finish
  if(rcPhase==1){static int lastLap;if(p->lap!=lastLap){if(p->lap==rcLaps&&rcMode!=RM_FREE){rcMsg="FINAL LAP!";rcMsgT=90;sfx_ok();}
     else if(p->lap>1&&p->lastLap&&(!RS.bestLap[trk]||(u32)p->lastLap<RS.bestLap[trk])){rcMsg="NEW LAP RECORD!";rcMsgT=90;rcRecLap=1;}lastLap=p->lap;}
   if(p->fin){rcPhase=2;endT=rcT;music_jingle(TR_FINISH);}}
  if(rcPhase==2&&(rcT-endT>360||({int all=1;for(int i=0;i<rcN;i++)if(!rc[i].fin)all=0;all;})&&rcT-endT>150))break;
  if(rcMode==RM_FREE&&FR_AUTO&&p->lap>2){p->finT=rcT;break;}
  if(rcT>60*60*6){break;}
  // camera
  {int e=ad16(p->h,camH);camH+=e/4;int dist=CAMD;camX=p->x-(rsin(camH)*dist<<(RQ-8));camY=p->y+(rcos(camH)*dist<<(RQ-8));}
  rc_table();rc_sprites();rc_hud();if(rcMsgT>0)rcMsgT--;
  BEAT->screen=SCR_RACE;BEAT->mode=trk;BEAT->phase=rcPhase;BEAT->scoreA=p->place;BEAT->scoreB=p->lap;BEAT->timer=rcT>0?rcT:0;BEAT->hp=p->bat;BEAT->x=p->x>>RQ;BEAT->y=p->y>>RQ;BEAT->extra=p->wp|(p->surf<<12)|(p->air<<15);BEAT->kills=rc[rcOrder[0]].car;
  int kx=0,ky=0;if(shakeT>0){shakeT--;if(SET.shake){kx=((frame*7)&3)-1;ky=((frame*5)&3)-1;}}
  vsync();rc_vbl();dma32(oam,OAM,256);dma32(hud,SCREENBLOCK(28),512);
  REG_BGHOFS(1)=(-(camH>>6))&511;REG_BGHOFS(0)=kx;REG_BGVOFS(0)=ky;
  frame++;BEAT->frame=frame;poll();
  rcT++;if(rcPhase==0&&rcT>=0){rcPhase=1;sfx_ok();}if(rcPhase==0&&(rcT%60)==0)sfx_menu();}
 // unfinished cars: estimate a finish time from their progress
 for(int i=0;i<rcN;i++){RC*c=&rc[i];if(c->fin)continue;int done=c->lap*rcNW+c->wp;int need=(rcLaps+1)*rcNW;int rate=done>0?rcT*256/done:256;c->finT=rcT+(need-done)*rate/256;c->fin=2;}
 for(int i=0;i<rcN;i++)for(int j=0;j<rcN;j++){int p_=1;for(int q=0;q<rcN;q++)if(q!=j&&(rc[q].finT<rc[j].finT||(rc[q].finT==rc[j].finT&&q<j)))p_++;rc[j].place=p_;}
 rc_stop();return 1;}
// ---- Mode 3 screens
static void rc_bg(int t){static const u16 A[4]={RGB(60,120,220),RGB(120,60,140),RGB(24,30,80),RGB(190,120,80)},B[4]={RGB(180,214,250),RGB(250,150,110),RGB(160,130,190),RGB(236,190,140)};
 grad(0,100,A[t&3],B[t&3]);grad(100,160,RGB(60,60,70),RGB(30,30,36));
 for(int y=100;y<160;y++){int w=(y-100)*2+6;m3_fill(120-w/2,y,w,1,RGB(90,92,104));if(((y-100)/6)%2==0)m3_fill(119,y,2,1,RGB(250,250,250));}
 for(int k=0;k<10;k++)m3_fill(k*24+((k&1)?6:0),100,12,2,RGB(250,250,250));}
static void rc_art(const u8*a,const u16*pal,int x0,int y0,int sc){for(int y=0;y<24;y++)for(int x=0;x<64;x++){int v=a[y*64+x];if(!v)continue;m3_fill(x0+x*sc,y0+y*sc,sc,sc,pal[v]);}}
static int rc_car_select(int*car){int sel=*car,redraw=1;
 for(;;){BEAT->screen=SCR_RCAR;BEAT->menu=sel;BEAT->extra=rcSpacex;
  if(redraw){redraw=0;rc_bg(sel%4);m3_fill(4,4,232,22,RGB(10,12,24));m3_box(4,4,232,22,2);m3_ctext(8,"SELECT YOUR CAR",C_YEL,2);
   const RCar*C=&RCARS[sel];m3_fill(16,32,208,58,RGB(12,16,34));m3_box(16,32,208,58,0);
   rc_art(&rc_cara[sel*64*24],&rc_carp[sel*16],56,38,2);
   m3_ctext(92,cat3("< ",C->n," >"),sel==0?RGB(255,120,110):C_WHITE,1);if(sel==0)m3_text(176,40,"FEATURED",C_YEL,1);
   m3_fill(8,102,224,54,RGB(10,12,24));m3_box(8,102,224,54,0);
   m3_bar(14,106,"TOP SPEED",C->vmax-8000,7000,C_BLUE);m3_bar(90,106,"ACCEL",C->acc,90,C_GREEN);m3_bar(166,106,"HANDLING",C->hand,620,C_YEL);
   m3_text(14,124,cat3("WEIGHT ",itoa(C->kg)," KG"),C_GRAY,1);m3_text(120,124,cat3("TOP ",itoa(C->mph)," MPH"),C_GRAY,1);
   m3_text(14,134,C->d1,C_WHITE,1);
   if(sel==0)m3_text(14,144,cat("SELECT: SPACEX THRUSTERS ",rcSpacex?"ON":"OFF"),rcSpacex?C_GREEN:C_GRAY,1);else m3_text(14,144,C->d2,C_GRAY,1);
   m3_present();}
  next_frame();
  if(pressed&(K_LEFT|K_UP)){sel=(sel+RC_NCAR-1)%RC_NCAR;redraw=1;sfx_menu();}
  if(pressed&(K_RIGHT|K_DOWN)){sel=(sel+1)%RC_NCAR;redraw=1;sfx_menu();}
  if((pressed&K_SEL)&&sel==0){rcSpacex^=1;redraw=1;sfx_whoosh();}
  if(pressed&(K_A|K_START)){sfx_ok();*car=sel;return 1;}if(pressed&K_B){sfx_menu();return 0;}}}
static void rc_minimap(int t,int x0,int y0,int sz){const u8*s=rc_surfv[t];m3_fill(x0,y0,sz,sz,RGB(20,24,30));
 for(int y=0;y<sz;y++)for(int x=0;x<sz;x++){int v=s[(y*128/sz)*128+(x*128/sz)];u16 c=v<=1?RGB(220,220,230):v==4?RGB(230,40,50):v==5?RGB(250,210,40):v==6?RGB(40,100,200):v==3?RGB(80,80,90):0;if(c)fb[(y0+y)*240+x0+x]=c;}}
static int rc_track_select(int mode,int*trk){int sel=*trk,redraw=1;
 for(;;){BEAT->screen=SCR_RTRK;BEAT->menu=sel;BEAT->mode=mode;
  if(redraw){redraw=0;rc_bg(sel);m3_fill(4,4,232,22,RGB(10,12,24));m3_box(4,4,232,22,2);m3_ctext(8,mode==RM_TT?"TIME TRIAL - TRACK":"FREE DRIVE - TRACK",C_YEL,2);
   m3_fill(8,30,224,124,RGB(10,12,24));m3_box(8,30,224,124,0);
   for(int i=0;i<4;i++)m3_text(16,40+i*14,cat(i==sel?"> ":"  ",RC_TN[i]),i==sel?C_YEL:C_WHITE,1);
   rc_minimap(sel,140,36,84);
   m3_text(16,100,cat("BEST LAP  ",RS.bestLap[sel]?rtime(RS.bestLap[sel]):"--"),C_WHITE,1);m3_text(16,110,cat("BEST RACE ",RS.bestRace[sel]?rtime(RS.bestRace[sel]):"--"),C_WHITE,1);
   m3_text(16,124,sel==3?"LOW GRAVITY: 0.38 G - LONG JUMPS":sel==1?"STAY ON THE ROAD - THE OCEAN IS COLD":sel==2?"JUMP THE RAMP NEAR THE LAUNCH PAD":"FLAT + FAST, SUPERCHARGER PAD",C_GRAY,1);
   m3_ctext(142,"A: RACE   B: BACK",C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+3)%4;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%4;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();*trk=sel;return 1;}if(pressed&K_B){sfx_menu();return 0;}}}
static const u8 GPPTS[8]={10,8,6,5,4,3,2,1};
static void rc_results(int mode,int trk,int*pts,int newLap,int newRace){int ord[8];for(int i=0;i<rcN;i++)ord[rc[i].place-1]=i;
 rc_bg(trk);m3_fill(4,4,232,152,RGB(10,12,24));m3_box(4,4,232,152,2);m3_ctext(8,mode==RM_TT?"TIME TRIAL":"RACE RESULTS",C_YEL,2);m3_ctext(26,RC_TN[trk],C_GRAY,1);
 for(int k=0;k<rcN;k++){int i=ord[k];int y=38+k*11;u16 c=i==0?C_YEL:C_WHITE;m3_text(12,y,PLC[k+1],c,1);m3_text(40,y,RCARS[rc[i].car].n,c,1);m3_text(136,y,rtime(rc[i].finT),rc[i].fin==2?C_GRAY:c,1);
  if(mode==RM_GP)m3_text(196,y,cat("+",itoa(GPPTS[k])),C_GREEN,1);if(i==0)m3_text(4,y,">",C_YEL,1);}
 if(rcN==1){m3_text(12,54,cat("BEST LAP ",rtime(rc[0].bestLap)),C_WHITE,1);}
 if(rcN>1)m3_text(12,128,"GRAY = ESTIMATED FINISH",C_GRAY,1);m3_ctext(137,newLap&&newRace?"NEW RACE + LAP RECORD!":newLap?"NEW LAP RECORD!":newRace?"NEW RACE RECORD!":"",C_GREEN,1);
 m3_ctext(147,"A: CONTINUE",C_GRAY,1);m3_present();
 for(int t=0;;t++){BEAT->screen=SCR_RRES;BEAT->phase=3;BEAT->scoreA=rc[0].place;BEAT->scoreB=rc[0].finT;BEAT->mode=trk;BEAT->kills=rc[ord[0]].car;{int b=0;for(int i=1;i<rcN;i++)if(!b||rc[i].finT<b)b=rc[i].finT;BEAT->hp=b;BEAT->era=rcDiff;}next_frame();if((pressed&(K_A|K_START))||(FR_AUTO&&t>150))break;}
 sfx_ok();}
static int rc_record(int trk){int nl=0,nr=0;RC*p=&rc[0];if(p->bestLap&&(!RS.bestLap[trk]||(u32)p->bestLap<RS.bestLap[trk])){RS.bestLap[trk]=p->bestLap;nl=1;}
 if(p->fin==1&&rcMode!=RM_FREE&&(!RS.bestRace[trk]||(u32)p->finT<RS.bestRace[trk])){RS.bestRace[trk]=p->finT;nr=1;}RS.races++;RS.lastCar=p->car;RS.spacex=rcSpacex;rs_save();return nl|(nr<<1);}
static void rc_gp_stand(int*pts,int race,int fin){int ord[8];for(int i=0;i<rcN;i++)ord[i]=i;for(int a=0;a<rcN;a++)for(int b=a+1;b<rcN;b++)if(pts[ord[b]]>pts[ord[a]]||(pts[ord[b]]==pts[ord[a]]&&ord[b]<ord[a])){int t=ord[a];ord[a]=ord[b];ord[b]=t;}
 rc_bg(race&3);m3_fill(4,4,232,152,RGB(10,12,24));m3_box(4,4,232,152,2);m3_ctext(8,fin?"GRAND PRIX FINAL":"GRAND PRIX STANDINGS",C_YEL,2);m3_ctext(26,cat3("AFTER RACE ",itoa(race+1),"/4"),C_GRAY,1);
 int myp=1;for(int k=0;k<rcN;k++){int i=ord[k];if(i==0)myp=k+1;int y=38+k*11;u16 c=i==0?C_YEL:C_WHITE;m3_text(12,y,PLC[k+1],c,1);m3_text(40,y,RCARS[rc[i].car].n,c,1);m3_text(170,y,cat(itoa(pts[i])," PTS"),c,1);}
 if(fin){m3_ctext(136,myp==1?"CHAMPION! YOU WIN THE CUP":cat3("YOU FINISH THE CUP ",PLC[myp],""),myp==1?C_GREEN:C_WHITE,1);
  if(!RS.gpBest||myp<RS.gpBest)RS.gpBest=myp;if(myp==1)RS.gpWins++;rs_save();if(myp<=3)music_jingle(TR_VICTORY);}
 m3_ctext(147,"A: CONTINUE",C_GRAY,1);m3_present();
 for(int t=0;;t++){BEAT->screen=SCR_RRES;BEAT->phase=fin?5:4;BEAT->scoreA=myp;BEAT->scoreB=pts[0];next_frame();if((pressed&(K_A|K_START))||(FR_AUTO&&t>150))break;}sfx_ok();}
static int rc_diff_select(void){int sel=rcDiff,redraw=1;static const char*const DD[3]={"RELAXED RIVALS, LOTS OF ROOM","RIVALS FIGHT BACK - GRAND PRIX DEFAULT","RACING LINES, SMART BOOSTS, NO MERCY"};
 for(;;){BEAT->screen=SCR_RCAR;BEAT->menu=sel;BEAT->phase=7;
  if(redraw){redraw=0;rc_bg(1);m3_fill(4,4,232,22,RGB(10,12,24));m3_box(4,4,232,22,2);m3_ctext(8,"GRAND PRIX DIFFICULTY",C_YEL,2);
   m3_fill(20,40,200,96,RGB(10,12,24));m3_box(20,40,200,96,0);
   for(int i=0;i<3;i++){int y=52+i*22;if(i==sel)m3_fill(28,y-4,184,16,RGB(16,60,110));m3_text(36,y,RC_DN[i],i==sel?C_YEL:C_WHITE,2);}
   m3_ctext(122,DD[sel],C_GRAY,1);m3_ctext(146,"UP/DOWN  A: START  B: BACK",C_GRAY,1);m3_present();}
  next_frame();if(FR_AUTO)return 1;
  if(pressed&K_UP){sel=(sel+2)%3;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%3;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();rcDiff=sel;RS.pad=(RS.pad&~3)|(sel+1);rs_save();pressed=0;return 1;}if(pressed&K_B){sfx_menu();pressed=0;return 0;}}}
static void race_mode(int md){rs_load();int mode=md==MD_GP?RM_GP:md==MD_TT?RM_TT:RM_FREE;int car=RS.lastCar;rcSpacex=RS.spacex;static int trk=0;
 rcDiff=(RS.pad&3)?(RS.pad&3)-1:1;if(rcDiff>2)rcDiff=1;
 u32 j=FR_JUMP;int jumped=0;if((j>>16)==0x5243){FR_JUMP=0;mode=(j>>8)&3;trk=(j>>4)&3;car=j&7;jumped=1;if((j>>12)&3)rcDiff=((j>>12)&3)-1;}
 for(;;){
  if(!jumped){bgm_t(TR_RACE);if(!rc_car_select(&car))return;}
  if(mode==RM_GP&&!jumped&&!rc_diff_select())continue;
  if(mode==RM_GP){int pts[8]={0};int quit=0;
   for(int r=0;r<4&&!quit;r++){loading(cat3("GRAND PRIX ",itoa(r+1),cat("/4: ",RC_TN[r])));int res;do res=rc_race(r,RM_GP,car,7);while(res==2);if(!res){quit=1;break;}
    int rec=rc_record(r);for(int i=0;i<rcN;i++)pts[i]+=GPPTS[rc[i].place-1];rc_results(RM_GP,r,pts,rec&1,rec>>1);rc_gp_stand(pts,r,r==3);}
   bgCached=-1;if(jumped||FR_AUTO)return;continue;}
  if(!jumped&&!rc_track_select(mode,&trk))continue;
  loading(cat(mode==RM_TT?"TIME TRIAL: ":"FREE DRIVE: ",RC_TN[trk]));int res;do res=rc_race(trk,mode,car,0);while(res==2);
  if(res){int rec=rc_record(trk);if(mode==RM_TT)rc_results(mode,trk,0,rec&1,rec>>1);}
  bgCached=-1;if(jumped||FR_AUTO)return;}}
static void rec_race(const char**L){rs_load();static char b[5][40];
 for(int t=0;t<4;t++){const char*p[4]={t==0?"TEXAS ":t==1?"PCH   ":t==2?"STARBS":"MARS  "," LAP ",RS.bestLap[t]?rtime(RS.bestLap[t]):"--:--.--",""};
  int k=0;for(int q=0;q<3;q++)for(const char*s=p[q];*s&&k<36;s++)b[t][k++]=*s;const char*r=RS.bestRace[t]?rtime(RS.bestRace[t]):"--";const char*rp=" RACE ";for(const char*s=rp;*s&&k<36;s++)b[t][k++]=*s;for(const char*s=r;*s&&k<38;s++)b[t][k++]=*s;b[t][k]=0;L[t]=b[t];}
 {int k=0;const char*p[4]={"CUP BEST ",RS.gpBest?PLC[RS.gpBest]:"--","  WINS ",itoa(RS.gpWins)};for(int q=0;q<4;q++)for(const char*s=p[q];*s&&k<38;s++)b[4][k++]=*s;b[4][k]=0;L[4]=b[4];}}
