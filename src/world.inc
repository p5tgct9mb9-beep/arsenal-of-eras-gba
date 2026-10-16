// ---------------------------------------------------------------- v1.2 FLIGHT AROUND THE WORLD (free roam, non-combat)
// Mode 1: BG2 affine world (8bpp, 128x128-tile window streamed from a 512x256-cell ROM map that wraps east-west),
// BG1 clouds / near-space curvature vignette (alpha blended), BG0 HUD.
#include "worlddb.inc"
typedef long long s64;
#define WPX (WWID*16)
#define WPY (WHGT*16)
#define REG_BG2PA (*(volatile s16*)0x4000020)
#define REG_BG2PB (*(volatile s16*)0x4000022)
#define REG_BG2PC (*(volatile s16*)0x4000024)
#define REG_BG2PD (*(volatile s16*)0x4000026)
#define REG_BG2X (*(volatile s32*)0x4000028)
#define REG_BG2Y (*(volatile s32*)0x400002C)
#include "wfdb.inc"
#define SCR_WFAC 112
static const char*const WCATN[5]={"PLANES","DRONES","HELICOPTERS","EXPERIMENTAL","WUNDERWAFFEN (WW2 EXPERIMENTAL)"};
static const char*const WCATS[5]={"PLANES","DRONES","HELOS","X-PLANES","WUNDERW."};
static const char*const WERAN[3]={"WW2","COLD WAR","MODERN"};
#define WHELI(A) ((A)->cat==2)
// ---- save block (separate from the main save, SRAM 0x400)
typedef struct{u32 lm;u32 best[NCOURSE];u32 circ;u32 maxAlt;u16 flights;u8 unl,ac,wings,pad[3];}WSave;
static WSave WS;
#define WS_OFF 0x400
static void ws_write(void){const u8*b=(const u8*)&WS;u8 sum=0x77;SRAM[WS_OFF]='A';SRAM[WS_OFF+1]='O';SRAM[WS_OFF+2]='E';SRAM[WS_OFF+3]='W';SRAM[WS_OFF+4]=1;
 for(unsigned i=0;i<sizeof WS;i++){SRAM[WS_OFF+5+i]=b[i];sum+=b[i]*(i+1);}SRAM[WS_OFF+5+sizeof WS]=sum;}
static void ws_read(void){memset(&WS,0,sizeof WS);if(SRAM[WS_OFF]!='A'||SRAM[WS_OFF+1]!='O'||SRAM[WS_OFF+2]!='E'||SRAM[WS_OFF+3]!='W'||SRAM[WS_OFF+4]!=1)return;
 u8 b[sizeof(WSave)];u8 sum=0x77;for(unsigned i=0;i<sizeof WS;i++){b[i]=SRAM[WS_OFF+5+i];sum+=b[i]*(i+1);}if(sum!=SRAM[WS_OFF+5+sizeof WS])return;memcpy(&WS,b,sizeof WS);
 if(WS.pad[0]!=2){WS.ac=WAC_OLD[WS.ac<4?WS.ac:0];WS.pad[0]=2;}if(WS.ac>=NWAC)WS.ac=0;}
static int nlogged(void){int n=0;for(int i=0;i<NLM;i++)if(WS.lm>>i&1)n++;return n;}
static int ac_ok(int i){(void)i;return 1;} // v1.3.5: all 113 aircraft unlocked
static const char*tmr(u32 f){char*o=nb[nbi++&3];u32 t=f/6;u32 m=t/600,s=(t/10)%60,d=t%10;int k=0;if(m>=100)o[k++]='0'+m/100%10;if(m>=10)o[k++]='0'+m/10%10;o[k++]='0'+m%10;o[k++]=':';o[k++]='0'+s/10;o[k++]='0'+s%10;o[k++]='.';o[k++]='0'+d;o[k]=0;return o;}
static const char*commas(s32 v){char*o=nb[nbi++&3];char t[16];int n=0,g=0;if(v<0)v=0;do{if(g==3){t[n++]=',';g=0;}t[n++]='0'+v%10;v/=10;g++;}while(v);int k=0;while(n)o[k++]=t[--n];o[k]=0;return o;}
// ---- flight state
static s32 fx,fy,fv,falt,ffuel,bx,by,cdx;static s16 fpitch;static u16 fhd,bhd;static int fthr,fstate,fac,fbase,fmode,fcourse,fring,ftoastT,fcrashT,fzoom,fband,lastCX=-999,lastCY,lastZ,fdone,fnav;
static u32 ftime;static const char*ft1,*ft2,*ft3;static int ftcol;
enum{FS_CARRY,FS_FLY,FS_LANDED,FS_CRASH,FS_DONE};
static char tb[3][32];static void tcp(char*d,const char*s){int k=0;if(s)while(s[k]&&k<28){d[k]=s[k];k++;}d[k]=0;}
static void toast(const char*a,const char*b,const char*c,int col){tcp(tb[0],a);tcp(tb[1],b);tcp(tb[2],c);ft1=tb[0];ft2=tb[1];ft3=tb[2];ftcol=col;ftoastT=240;}
static void aff_reset(void){REG_BG2PA=256;REG_BG2PB=0;REG_BG2PC=0;REG_BG2PD=256;REG_BG2X=0;REG_BG2Y=0;REG_BLDCNT=0;}
static int wdx(s32 a,s32 b){int d=((a-b)>>8)%WPX;if(d>WPX/2)d-=WPX;if(d<-WPX/2)d+=WPX;return d;}
static s32 dens(s32 alt){if(alt<0)alt=0;int k=alt/20000;if(k>15)return 0;s32 d0=65536>>k;int f=alt%20000;return d0-(d0/2)*f/20000;}
static int wcell(int cx,int cy){if(cy<0)return 21;if(cy>=WHGT)return 15;return world_map[cy*WWID+(cx&(WWID-1))];}
static int terrain_h(int t){return t==13||t==14?12000:t==23?2500:0;}
static void w_palette(void){int a=falt;int hz=a<40000?0:a<150000?(a-40000)*6/110000:6;int dk=a<150000?0:a>330000?11:(a-150000)*11/180000;
 for(int i=1;i<64;i++){u16 c=world_pal[i];int r=c&31,g=(c>>5)&31,b=(c>>10)&31;
  r=(r*(20-hz)+22*hz)/20;g=(g*(20-hz)+25*hz)/20;b=(b*(20-hz)+30*hz)/20; // high-altitude haze
  r=r*(32-dk)/32;g=g*(32-dk)/32;b=(b*(32-dk)+6*dk)/32;               // near-space darkening
  PAL_BG[i]=r|(g<<5)|(b<<10);}
 PAL_BG[0]=a>150000?RGB(4,6,20):world_pal[2];}
static void w_bg1(int band){ // 0 clouds, 1 none, 2 vignette
 if(band==0)dma32(world_cloudmap,SCREENBLOCK(29),512);else if(band==2)dma32(world_vigmap,SCREENBLOCK(29),512);}
static void w_setup(void){
 vsync();REG_DISPCNT=0x80;
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(world_tiles,CHARBLOCK(1),sizeof(world_tiles)/4);dma32(world_bg1t,CHARBLOCK(3),sizeof(world_bg1t)/4);
 for(int i=0;i<16;i++)PAL_BG[144+i]=world_bg1pal[i];
 dma32(world_obj,OBJ_TILES,sizeof(world_obj)/4);for(int i=0;i<7*16;i++)PAL_OBJ[i]=world_objpal[i];
 dma32(wf_tiles+fac*128,OBJ_TILES,128);for(int i=0;i<16;i++)PAL_OBJ[i]=wf_pal[fac*16+i];
 {volatile u32*m=(volatile u32*)SCREENBLOCK(16);for(int i=0;i<4096;i++)m[i]=0;}
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=1|(3<<2)|(29<<8);REG_BGCNT(2)=3|(1<<2)|(16<<8)|(1<<13)|(3<<14);
 REG_BGHOFS(0)=0;REG_BGVOFS(0)=0;hud_clear();oam_begin();fband=-1;lastCX=-999;w_palette();shimmer=0;
 REG_BLDCNT=(1<<1)|(1<<6)|(1<<10)|(1<<12)|(1<<13);REG_BLDALPHA=9|(9<<8);}
static void w_stream(int camx,int camy){int z=fzoom;int hx=(120*z>>8)+40,hy=(80*z>>8)+40;
 int cx0=(camx-hx)>>4,cx1=(camx+hx)>>4,cy0=(camy-hy)>>4,cy1=(camy+hy)>>4;
 if(cx0==lastCX&&cy0==lastCY&&z==lastZ)return;lastCX=cx0;lastCY=cy0;lastZ=z;
 volatile u16*m=SCREENBLOCK(16);
 for(int cy=cy0;cy<=cy1;cy++){int wy=(cy&63)*2;for(int cx=cx0;cx<=cx1;cx++){int t=wcell(cx,cy)*4;int wx=(cx&63)*2;
  m[(wy*128+wx)>>1]=t|((t+1)<<8);m[((wy+1)*128+wx)>>1]=(t+2)|((t+3)<<8);}}}
static void w_mat(int k,int ang,int z){int c=COS(ang),s=SIN(ang);oam[(k*4+0)*4+3]=(u16)(s16)(c*z>>8);oam[(k*4+1)*4+3]=(u16)(s16)(s*z>>8);oam[(k*4+2)*4+3]=(u16)(s16)(-s*z>>8);oam[(k*4+3)*4+3]=(u16)(s16)(c*z>>8);}
static void w_spr(int cx,int cy,int tile,int sz,int pal,int mat,int dbl,int prio,int semi){int w=8<<sz;if(noam>=128)return;int ww=dbl?w*2:w;if(cx<-ww||cx>240+ww||cy<-ww||cy>160+ww)return;
 int x=cx-ww/2,y=cy-ww/2;u16*o=&oam[noam*4];o[0]=(y&255)|(1<<8)|(dbl<<9)|(semi?1<<10:0);o[1]=(x&511)|(mat<<9)|(sz<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static void w_place(int i){fx=WAF[i].x<<8;fy=WAF[i].y<<8;bx=fx;by=fy;fbase=i;}
static int w_clear_hd(void){ // take-off heading that does not run straight into high terrain (and, if possible, not out to sea)
 for(int pass=0;pass<2;pass++)for(int k=0;k<8;k++){int a=(64+k*32)&255,ok=1;for(int d=8;d<=240&&ok;d+=8){int x=(fx>>8)+(SIN(a)*d>>8),y=(fy>>8)-(COS(a)*d>>8);int t=wcell(x>>4,y>>4);if(t==13||t==14)ok=0;if(!pass&&d<=160&&(t<=4||t==21))ok=0;}if(ok)return a<<8;}return 0x4000;}
static void w_carry(int i){w_place(i);fstate=WAC[fac].rocket==1?FS_CARRY:FS_LANDED;falt=fstate==FS_CARRY?45000:0;fv=fstate==FS_CARRY?8000:0;fhd=bhd=0x4000;fpitch=0;fthr=0;ffuel=WAC[fac].fuel*100;}
static int near_af(void){int best=0,bd=1<<30;for(int i=0;i<NAF;i++){int dx=wdx(WAF[i].x<<8,fx),dy=(WAF[i].y-(fy>>8));int d=idist(dx,dy);if(d<bd){bd=d;best=i;}}return best;}
static void w_crash(const char*why){fstate=FS_CRASH;fcrashT=150;int a=near_af();toast(why,cat("RECOVERED TO ",WAF[a].n),fmode?"+30 SEC PENALTY":"",HB_RED);if(fmode)ftime+=60*30;fbase=a;sfx_boom();shake(14);}
static void w_start(void){ftime=0;fring=0;cdx=0;fdone=0;ftoastT=0;fnav=0;
 if(fmode==1){const WRing*r=&WCO[fcourse].r[0];int ex=r->x,ey=r->y;const WRing*r1=&WCO[fcourse].r[1];int dx=wdx(r1->x<<8,ex<<8),dy=r1->y-ey;int a=atan2i(dx,dy);
  fx=((ex-(SIN(a)*150>>8))&(WPX-1))<<8;fy=(ey+(COS(a)*150>>8))<<8;fhd=a<<8;bhd=fhd;bx=fx;by=fy;fbase=0;ffuel=WAC[fac].fuel*100;fpitch=0;
  if(WAC[fac].rocket==1){fstate=FS_CARRY;falt=45000;fv=8000;fthr=0;}else{fstate=FS_FLY;falt=r->alt*10<WAC[fac].ceil?r->alt*10:WAC[fac].ceil-2000;fv=WAC[fac].stall*3;fthr=100;}
  toast(WCO[fcourse].n,WCO[fcourse].desc,"FLY THROUGH ALL 8 RINGS",HB_YEL);}
 else{w_carry(0);toast(fmode==2?"CIRCUMNAVIGATION":"FREE FLIGHT - EDWARDS AFB",fmode==2?"FLY ALL THE WAY AROUND THE WORLD":"FLY ANYWHERE. LOG LANDMARKS.",
   WAC[fac].rocket==1?"A: DROP FROM THE MOTHERSHIP":WHELI(&WAC[fac])?"A: LIFT OFF":"A: TAKE OFF",HB_YEL);}}
static void w_physics(void){const WAc*A=&WAC[fac];
 if(keys&K_A)fthr+=2;if(keys&K_B)fthr-=2;fthr=clampi(fthr,0,100);
 if(keys&K_UP){if(frame&1)fpitch++;}else if(keys&K_DOWN){if(frame&1)fpitch--;}
 fpitch=clampi(fpitch,-40,44);s32 d=dens(falt);
 int tr;if(d<1200)tr=110;else tr=A->turnk*600/(fv/100+150);if(keys&K_LEFT)fhd-=tr;if(keys&K_RIGHT)fhd+=tr;
 int th=0;if(ffuel>0&&fthr>0){static int tacc;tacc+=A->thrq*fthr;th=tacc/400;tacc%=400;ffuel-=fthr;if(ffuel<=0){ffuel=0;toast(A->rocket?"BURNOUT - GLIDE TO AN AIRFIELD":"BINGO FUEL","LAND ON ANY RUNWAY TO REFUEL",A->rocket==1?"OR RE-MATE WITH THE MOTHERSHIP":"",HB_RED);}}
 if(!A->rocket&&falt>A->ceil){s32 o=falt-A->ceil;th=o>A->ceil/8?0:th*(A->ceil/8-o)/(A->ceil/8);}
 s64 drag=(s64)fv*fv*d/((A->rocket?2750000LL:5500000LL)<<16);if(keys&K_L)drag=drag*4+3;
 fv+=th-(s32)drag-(36*SIN(fpitch&255)>>8);
 if(d<1200&&!(keys&(K_UP|K_DOWN))&&(frame%90)==0&&fpitch>-30)fpitch--; // ballistic arc above the sensible atmosphere
 if(fv<A->stall&&d>4000){if((frame&3)==0)fpitch--;falt-=(A->stall-fv)/40;}
 if(fv<400)fv=400;if(fv>100000)fv=100000;
 falt+=(s32)((s64)fv*SIN(fpitch&255)/51200);
 s32 gsp=fv*COS(fpitch&255)>>8;s32 step=gsp*3/200;int ox=fx;
 fx+=SIN(fhd>>8)*step>>8;fy-=COS(fhd>>8)*step>>8;cdx+=(fx-ox);fx=((fx%(WPX<<8))+(WPX<<8))%(WPX<<8);
 if(fy<(48<<8)){fy=48<<8;if(ftoastT<60)toast("POLAR LIMIT","TURN SOUTH","",HB_RED);}if(fy>((WPY-48)<<8)){fy=(WPY-48)<<8;if(ftoastT<60)toast("POLAR LIMIT","TURN NORTH","",HB_RED);}
 if((u32)falt>WS.maxAlt)WS.maxAlt=falt;
 if(falt>264000&&!WS.wings){WS.wings=1;ws_write();toast("ASTRONAUT WINGS!","ABOVE 50 MILES - OFFICIALLY IN SPACE","",HB_YEL);sfx_ok();}
 int t=wcell((fx>>8)>>4,(fy>>8)>>4);int gh=terrain_h(t);
 if(falt<=gh){int rw=t>=18&&t<=20;
  if(rw&&fv<4200&&fpitch<=6){fstate=FS_LANDED;falt=0;fv=0;fpitch=0;fthr=0;int a=near_af();fbase=a;ffuel=A->fuel*100;
   toast(cat("LANDED: ",WAF[a].n),"REFUELED AND SERVICED",A->rocket==1?"A: MOTHERSHIP RELAUNCH":"A: TAKE OFF",HB_GREEN);sfx_ok();}
  else if(rw)w_crash(fv>=4200?"HARD LANDING - TOO FAST":"HARD LANDING - NOSE DOWN");
  else if(t<=4||t==21)w_crash("DITCHED IN THE SEA");else w_crash(gh?"CONTROLLED FLIGHT INTO TERRAIN":"OFF-FIELD CRASH LANDING");}}
static void w_heli(const WAc*A){ // v1.3.5 helicopter flight: hover, collective, yaw, strafe, land on any open ground
 int vm=A->vmax;if(vm<600)vm=600;
 if(keys&K_UP)fv+=vm/150+1;else if(keys&K_DOWN)fv-=vm/90+1;else{fv-=fv/60;if(iabs(fv)<20)fv=0;}
 fv=clampi(fv,-vm/4,vm);
 int tr=A->turnk*(fv>vm/2?3:5);if(keys&K_LEFT)fhd-=tr;if(keys&K_RIGHT)fhd+=tr;
 int vs=0;if(keys&K_A){vs=9;fthr=100;}else if(keys&K_B){vs=-9;fthr=10;}else fthr=50;
 if(ffuel>0){ffuel-=30+iabs(fv)*40/vm+(vs>0?20:0);if(ffuel<=0){ffuel=0;toast("BINGO FUEL","AUTOROTATE - SET DOWN ON OPEN GROUND","",HB_RED);}}else vs=-14;
 falt+=vs;if(falt>A->ceil)falt=A->ceil;fpitch=0;
 int step=fv*3/200;int ss=(keys&K_L)?-vm/3:(keys&K_R)?vm/3:0;ss=ss*3/200;int h=fhd>>8,ox=fx;
 fx+=(SIN(h)*step>>8)+(COS(h)*ss>>8);fy+=-(COS(h)*step>>8)+(SIN(h)*ss>>8);cdx+=(fx-ox);fx=((fx%(WPX<<8))+(WPX<<8))%(WPX<<8);
 if(fy<(48<<8))fy=48<<8;if(fy>((WPY-48)<<8))fy=(WPY-48)<<8;
 if((u32)falt>WS.maxAlt)WS.maxAlt=falt;
 int t=wcell((fx>>8)>>4,(fy>>8)>>4);int gh=terrain_h(t);
 if(falt<=gh){falt=gh;if(t<=4||t==21){w_crash("DITCHED IN THE SEA");return;}
  if(iabs(fv)>vm/3+300){w_crash(gh&&vs>=0?"FLEW INTO HIGH TERRAIN":"ROLLOVER - TOUCHED DOWN TOO FAST");return;}
  int rw=t>=18&&t<=20;fstate=FS_LANDED;fv=0;fthr=0;
  if(rw){int a=near_af();fbase=a;ffuel=A->fuel*100;toast(cat("LANDED: ",WAF[a].n),"REFUELED AND SERVICED","A: LIFT OFF",HB_GREEN);}
  else toast("LANDED ON OPEN GROUND","REFUEL AT ANY AIRFIELD","A: LIFT OFF",HB_GREEN);sfx_ok();}}
static void w_logic(void){const WAc*A=&WAC[fac];
 if(fstate!=FS_DONE&&fmode&&!(fstate==FS_CARRY&&ftime==0&&fmode==1&&0))ftime++;
 // B-52 mothership: carries you, or orbits its base
 if(fstate==FS_CARRY){int tr=keys&K_LEFT?-90:keys&K_RIGHT?90:0;bhd+=tr;fhd=bhd;int st=8000*3/200;bx+=SIN(bhd>>8)*st>>8;by-=COS(bhd>>8)*st>>8;bx=((bx%(WPX<<8))+(WPX<<8))%(WPX<<8);
  by=clampi(by,48<<8,(WPY-48)<<8);fx=bx+(SIN((bhd>>8)+64)*9>>8<<8);fy=by-(COS((bhd>>8)+64)*9>>8<<8);falt=45000;fv=8000;cdx+=st*SIN(bhd>>8)>>8;
  if(pressed&K_A){fstate=FS_FLY;fthr=100;fpitch=4;falt=44500;bgm_t(TR_ASCENT);toast("DROP! ROCKET IGNITION","UP/DN PITCH  A/B THROTTLE  L BRAKE","",HB_YEL);sfx_whoosh();shake(6);}}
 else{bhd+=70;int st=8000*3/200;int cx=WAF[fbase].x<<8,cy=WAF[fbase].y<<8;int ddx=wdx(cx,bx),ddy=(cy-by)>>8;if(idist(ddx,ddy)>220)turn_to(&bhd,atan2i(ddx,ddy),400);
  bx+=SIN(bhd>>8)*st>>8;by-=COS(bhd>>8)*st>>8;bx=((bx%(WPX<<8))+(WPX<<8))%(WPX<<8);}
 if(fstate==FS_FLY){if(WHELI(A))w_heli(A);else w_physics();
  if(A->rocket==1&&ffuel<A->fuel*90){int d=idist(wdx(bx,fx),(by-fy)>>8);if(d<26&&iabs(falt-45000)<10000&&fv<14000){fstate=FS_CARRY;bhd=fhd;ffuel=A->fuel*100;fthr=0;toast("MATED TO THE MOTHERSHIP","FUEL TOPPED OFF","A: DROP",HB_GREEN);sfx_ok();}}}
 else if(fstate==FS_LANDED){if(pressed&K_A){if(WHELI(A)){fstate=FS_FLY;falt+=150;fv=0;fpitch=0;fthr=50;fhd=w_clear_hd();toast("LIFT OFF!","A/B CLIMB-DESCEND  UP/DN FWD-BACK","L/R STRAFE  RELEASE UP/DN TO HOVER",HB_GREEN);}
   else if(A->rocket==1){fstate=FS_CARRY;bx=fx;by=fy;bhd=0x4000;toast("MOTHERSHIP CLIMB-OUT TO 45,000 FT","A: DROP","",HB_YEL);}
   else{fstate=FS_FLY;falt=1500;fv=A->stall*2;fpitch=10;fthr=100;fhd=w_clear_hd();toast("TAKE OFF!","UP/DN PITCH  A/B THROTTLE  L BRAKE","",HB_GREEN);}sfx_whoosh();}}
 else if(fstate==FS_CRASH){if(--fcrashT<=0){w_carry(fbase);}}
 // discovery
 if(fstate==FS_FLY||fstate==FS_CARRY){for(int i=0;i<NLM;i++){if(WS.lm>>i&1)continue;int d=idist(wdx(WLM[i].x<<8,fx),WLM[i].y-(fy>>8));
   if(d<44&&falt<120000){WS.lm|=1u<<i;ws_write();toast(cat("DISCOVERED: ",WLM[i].n),WLM[i].place,WLM[i].fact,HB_YEL);sfx_ok();
    int n=nlogged();if(n==3||n==10)toast(cat("DISCOVERED: ",WLM[i].n),WLM[i].place,cat3("LANDMARKS LOGGED: ",itoa(n),""),HB_GREEN);
    if(n==NLM)toast("ALL LANDMARKS LOGGED!","WORLD TOUR COMPLETE","",HB_GREEN);}}}
 // rings
 if(fmode==1&&fstate!=FS_DONE&&fring<8){const WRing*r=&WCO[fcourse].r[fring];int d=idist(wdx(r->x<<8,fx),r->y-(fy>>8));
  if(d<30&&iabs(falt-r->alt*10)<15000){fring++;sfx_ok();if(fring==8){fstate=FS_DONE;fdone=1;int rec=!WS.best[fcourse]||ftime<WS.best[fcourse];if(rec)WS.best[fcourse]=ftime;ws_write();
    toast("COURSE COMPLETE!",cat("TIME ",tmr(ftime)),rec?"NEW RECORD - SAVED":cat("BEST ",tmr(WS.best[fcourse])),HB_GREEN);}else toast(cat("RING ",itoa(fring)),"","",HB_GREEN);ftoastT=fring==8?600:50;}}
 // circumnavigation: one full lap of longitude, then back over Edwards
 if(fmode==2&&fstate!=FS_DONE){int lap=iabs(cdx>>8);int d=idist(wdx(WAF[0].x<<8,fx),WAF[0].y-(fy>>8));
  if(lap>WPX-200&&d<70){fstate=FS_DONE;fdone=1;int rec=!WS.circ||ftime<WS.circ;if(rec)WS.circ=ftime;ws_write();toast("AROUND THE WORLD!",cat("TIME ",tmr(ftime)),rec?"NEW RECORD - SAVED":cat("BEST ",tmr(WS.circ)),HB_GREEN);ftoastT=600;}}}
static const char*band_name(void){return falt>=264000?"SPACE":falt>=150000?"NEAR SPACE":falt>=40000?"HIGH ALT":"LOW ALT";}
static void w_hud(void){const WAc*A=&WAC[fac];hud_clear();
 htxt(0,0,A->s,HB_YEL);
 if(WHELI(A)){int kt=iabs(fv)*661/10000;htxt(slen(A->s)+1,0,cat(itoa(kt),fv<0?"KT B":"KT"),HB_WHITE);}
 else{int m=fv/100;char b[8];b[0]='M';b[1]='0'+m/100%10;b[2]='.';b[3]='0'+m/10%10;b[4]='0'+m%10;b[5]=0;htxt(slen(A->s)+1,0,b,fv>=10000?HB_YEL:HB_WHITE);}
 htxt(16,0,cat(commas(falt),"FT"),falt>=150000?HB_BLUE:HB_WHITE);htxt(30-slen(band_name()),1,band_name(),falt>=150000?HB_BLUE:HB_GRAY);
 htxt(0,1,"THR",HB_GRAY);hbar(3,1,3,fthr,100);htxt(7,1,A->rocket?"LOX":"FUEL",HB_GRAY);hbar(A->rocket?10:11,1,4,ffuel,A->fuel*100);if(WHELI(A))htxt(0,1,"COL",HB_GRAY);
 {int hd=(fhd*360)>>16;static const char*const CP[8]={"N","NE","E","SE","S","SW","W","NW"};char b[12];b[0]='0'+hd/100;b[1]='0'+hd/10%10;b[2]='0'+hd%10;b[3]=' ';b[4]=0;
  htxt(0,19,cat(b,CP[((fhd+0x1000)>>13)&7]),HB_WHITE);
  int p=fpitch*360/256;htxt(8,19,cat(p>=0?"P+":"P-",itoa(iabs(p))),HB_GRAY);}
 if(fmode==0){htxt(14,19,"LOG",HB_GREEN);htxt(18,19,cat3(itoa(nlogged()),"/",itoa(NLM)),HB_GREEN);}
 if(fmode==1){htxt(14,19,cat3("RING ",itoa(fring<8?fring+1:8),"/8"),HB_YEL);htxt(23,19,tmr(ftime),HB_WHITE);
  if(fring<8){int ra=WCO[fcourse].r[fring].alt*10;int df=ra-falt;htxt(0,2,cat3("RING ALT ",commas(ra),df>15000?" CLIMB":df<-15000?" DIVE":" OK"),iabs(df)<15000?HB_GREEN:HB_YEL);}}
 if(fmode==2){int pc=iabs(cdx>>8)*100/WPX;if(pc>100)pc=100;htxt(14,19,cat(itoa(pc),"% LAP"),HB_YEL);htxt(23,19,tmr(ftime),HB_WHITE);}
 if(fstate==FS_CARRY)hctxt(16,"UNDER THE MOTHERSHIP - A: DROP",HB_YEL,0);
 if(fstate==FS_FLY&&WHELI(A)&&falt<3000)hctxt(16,"B: DESCEND, LAND SLOWLY",HB_GRAY,0);
 else if(fstate==FS_FLY&&falt<20000&&ffuel==0)hctxt(16,"LANDING: SLOW < M0.42, P<+8, ON RUNWAY",HB_GRAY,0);
 if(fstate==FS_FLY&&wcell((fx>>8)>>4,(fy>>8)>>4)>=13&&wcell((fx>>8)>>4,(fy>>8)>>4)<=14&&falt<16000&&(frame&16))hctxt(15,"TERRAIN! PULL UP",HB_RED,0);
 if(ftoastT>0){ftoastT--;int n=ft3&&ft3[0]?3:ft2&&ft2[0]?2:1;hpanel(1,4,28,n+2);hptxt(2,5,ft1,ftcol);if(n>1)hptxt(2,6,ft2,HB_WHITE);if(n>2)hptxt(2,7,ft3,HB_GRAY);}
 // nav readout
 const char*nn=0;int tx=0,ty=0;
 if(fmode==1&&fring<8){tx=WCO[fcourse].r[fring].x;ty=WCO[fcourse].r[fring].y;nn="NEXT RING";}
 else if(fmode==2){tx=(fx>>8)+(cdx>=0?600:-600);ty=WAF[0].y;if(iabs(cdx>>8)>WPX-1400){tx=WAF[0].x;nn="EDWARDS AFB";}else nn=cdx>=0?"KEEP GOING EAST":"KEEP GOING WEST";}
 else if(ffuel==0&&fstate==FS_FLY){int a=near_af();tx=WAF[a].x;ty=WAF[a].y;nn=WAF[a].n;}
 else{int best=-1,bd=1<<30;for(int i=0;i<NLM;i++){if(WS.lm>>i&1)continue;int d=idist(wdx(WLM[i].x<<8,fx),WLM[i].y-(fy>>8));if(d<bd){bd=d;best=i;}}if(best>=0){tx=WLM[best].x;ty=WLM[best].y;nn=WLM[best].n;}}
 fnav=-1;if(nn){int dx=wdx(tx<<8,fx),dy=ty-(fy>>8);int mi=idist(dx,dy)*3;htxt(0,18,nn,HB_GRAY);htxt(slen(nn)+1,18,cat(commas(mi)," MI"),HB_GRAY);fnav=atan2i(dx,dy);}}
static void w_draw(void){oam_begin();int z=fzoom;int camx=fx>>8,camy=fy>>8;
 // plane (prio 0), shadow (low alt)
 int ptile=0;int sx=120,sy=80;w_mat(0,fhd>>8,256);
 if(fstate!=FS_CRASH||(fcrashT&4)){w_spr(sx,sy,ptile,2,0,0,0,0,0);
  if(fstate==FS_FLY&&fthr>0&&ffuel>0&&!WHELI(&WAC[fac])){int a=fhd>>8;w_spr(sx-(SIN(a)*17>>8),sy+(COS(a)*17>>8),WO_FLAME,0,6,0,0,0,0);}}
 if(falt<40000&&fstate!=FS_LANDED){w_mat(1,fhd>>8,256*256/(256+falt/180));int o=falt/1400;w_spr(sx+o,sy+o*2/3,ptile,2,5,1,0,2,1);}
 if(fstate==FS_LANDED)w_spr(sx+2,sy+2,ptile,2,5,0,0,2,1);
 // B-52 (2x, scaled with zoom)
 {int dx=wdx(bx,fx),dy=(by-fy)>>8;int qx=sx+dx*256/z,qy=sy+dy*256/z;if(fstate==FS_CARRY){qx=sx-(SIN((bhd>>8)+64)*9>>8)*256/z;qy=sy+(COS((bhd>>8)+64)*9>>8)*256/z;}
  if(WAC[fac].rocket==1){w_mat(2,bhd>>8,z/2);w_spr(qx,qy,WO_B52,2,4,2,1,fstate==FS_CARRY?1:2,0);}}
 // landmarks + rings (scaled with zoom)
 w_mat(3,0,z*2/3>256?z*2/3:256);int lz=z;
 for(int i=0;i<NLM;i++){int dx=wdx(WLM[i].x<<8,fx),dy=WLM[i].y-camy;int qx=sx+dx*256/lz,qy=sy+dy*256/lz;if(qx<-40||qx>280||qy<-40||qy>200)continue;
  w_spr(qx,qy,WO_ICON+WLM[i].icon*4,1,6,3,z<=300,2,0);}
 if(fmode==1)for(int k=fring;k<8&&k<fring+2;k++){const WRing*r=&WCO[fcourse].r[k];int dx=wdx(r->x<<8,fx),dy=r->y-camy;int qx=sx+dx*256/z,qy=sy+dy*256/z;
  w_mat(4,0,z*(k==fring?1:2)*256/256);w_spr(qx,qy,WO_RING,2,6,4,k==fring,1,k!=fring);}
 if(fnav>=0&&fstate!=FS_CRASH){w_mat(5,fnav,256);w_spr(sx+(SIN(fnav)*44>>8),sy-(COS(fnav)*44>>8),WO_ARROW,1,6,5,0,0,0);}
 if(falt>=150000)for(int k=0;k<6;k++){int px=(k*53+((camx/4)&255))%260-10,py=(k*37+((camy/4)&127))%170-5;int dd=(px-120)*(px-120)/196+(py-80)*(py-80)/108;if(dd>100)w_spr(px,py,WO_STAR,0,6,6,0,1,0),w_mat(6,0,256);}
 // background registers
 int band=falt<40000?0:falt<150000?1:2;
 vsync();
 dma32(oam,OAM,256);dma32(hud,SCREENBLOCK(28),512);
 int kx=0,ky=0;if(shakeT>0){shakeT--;if(SET.shake){kx=((frame*7)&3)-1;ky=((frame*5)&3)-1;}}
 REG_BG2PA=z;REG_BG2PB=0;REG_BG2PC=0;REG_BG2PD=z;REG_BG2X=((fx&((1024<<8)-1)))-120*z+(kx<<8);REG_BG2Y=((fy&((1024<<8)-1)))-80*z+(ky<<8);
 if(band!=fband){fband=band;w_bg1(band);}
 if(band==0){REG_BGHOFS(1)=(camx*5/4)&511;REG_BGVOFS(1)=(camy*5/4)&511;int e=falt<4000?4:falt>30000?4:6;REG_BLDALPHA=e|((16-e)<<8);}
 else if(band==2){REG_BGHOFS(1)=0;REG_BGVOFS(1)=0;int e=4+(falt-150000)*12/180000;if(e>16)e=16;REG_BLDALPHA=e|((16-e)<<8);}
 REG_DISPCNT=1|(1<<6)|(1<<8)|(band!=1?1<<9:0)|(1<<10)|(1<<12);
 frame++;BEAT->frame=frame;poll();
 if((frame&7)==0)w_palette();
 w_stream(camx,camy);}
static void w_worldmap(void){int redraw=1;aff_reset();
 for(;;){BEAT->screen=SCR_WMAP;if(redraw){redraw=0;m3_fill(0,0,240,160,C_PANEL);for(int y=0;y<120;y++)for(int x=0;x<240;x++)fb[(y+22)*240+x]=world_ov[y*240+x];
   {char b[8];const char*n=itoa(nlogged());int k=0;while(n[k]){b[k]=n[k];k++;}b[k]=0;m3_ctext(4,cat3("WORLD MAP - LOGGED ",b,"/19"),C_YEL,1);}
   for(int i=0;i<NAF;i++){int x=WAF[i].x*240/WPX,y=22+WAF[i].y*120/WPY;m3_fill(x-1,y,3,1,C_WHITE);}
   for(int i=0;i<NLM;i++){int x=WLM[i].x*240/WPX,y=22+WLM[i].y*120/WPY;u16 c=(WS.lm>>i&1)?RGB(255,210,60):RGB(120,120,140);m3_fill(x-1,y-1,3,3,RGB(30,20,40));m3_px(x,y,c);}
   if(fmode==1)for(int k=fring;k<8;k++){int x=WCO[fcourse].r[k].x*240/WPX,y=22+WCO[fcourse].r[k].y*120/WPY;m3_px(x,y,RGB(255,140,40));}
   m3_text(4,146,"WHITE: AIRFIELD  GOLD: LOGGED  B: BACK",C_GRAY,1);}
  {int x=(fx>>8)*240/WPX,y=22+(fy>>8)*120/WPY;m3_fill(x-2,y-2,5,5,(frame&16)?RGB(255,40,40):RGB(255,255,255));}
  m3_present();next_frame();if(pressed&(K_B|K_SEL|K_START|K_A)){sfx_menu();return;}}}
static int w_pause(void){int sel=0;static const char*const IT[3]={"RESUME","WORLD MAP","EXIT TO MENU"};
 for(;;){hud_clear();hpanel(8,6,14,7);hptxt(10,7,"PAUSED",HB_YEL);for(int k=0;k<3;k++)hptxt(10,9+k,cat(k==sel?">":" ",IT[k]),k==sel?HB_WHITE:HB_GRAY);
  BEAT->screen=SCR_PAUSE;vsync();dma32(hud,SCREENBLOCK(28),512);frame++;BEAT->frame=frame;poll();
  if(pressed&K_UP){sel=(sel+2)%3;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%3;sfx_menu();}if(pressed&K_START)return 0;
  if(pressed&K_A){sfx_ok();if(sel==1){w_worldmap();w_setup();sel=0;continue;}return sel==2;}}}
static void w_run(void){bgm_t(TR_CRUISE);ws_read();WS.ac=fac;w_setup();w_start();WS.flights++;
 for(;;){BEAT->screen=SCR_WORLD;BEAT->mode=6;BEAT->hp=falt/100;BEAT->scoreA=fv/100;BEAT->scoreB=nlogged();BEAT->timer=ftime;BEAT->phase=fstate;BEAT->x=(fx>>8)>>4;BEAT->y=(fy>>8)>>4;BEAT->extra=ffuel/(WAC[fac].fuel>0?WAC[fac].fuel:1)|(fring<<8)|(fdone<<12)|(fac<<16);BEAT->kills=fzoom;
  int tz=256;if(falt>40000)tz=256+(falt-40000)*330/310000;if(tz>600)tz=600;fzoom+=(tz-fzoom)/8;if(iabs(tz-fzoom)<8)fzoom=tz;
  w_logic();w_hud();w_draw();
  if(fdone&&ftoastT<=1&&(pressed&(K_A|K_START))){ws_write();aff_reset();return;}
  if(pressed&K_SEL){w_worldmap();w_setup();}
  if(pressed&K_START){if(w_pause()){ws_write();aff_reset();return;}w_setup();}}}
static void w_log(void){int sel=0,redraw=1;
 for(;;){BEAT->screen=SCR_WLOG;BEAT->menu=sel;if(redraw){redraw=0;m3_title(1);m3_box(4,4,232,152,1);m3_ctext(8,cat3("LANDMARK LOG  ",itoa(nlogged()),"/19"),C_YEL,1);
   for(int i=0;i<NLM;i++){int y=18+i*6;int got=WS.lm>>i&1;m3_text(14,y,cat(i==sel?">":" ",got?WLM[i].n:"? ? ?"),i==sel?C_YEL:got?C_WHITE:C_GRAY,1);if(got)m3_text(130,y,"*",C_GREEN,1);}
   int got=WS.lm>>sel&1;m3_text(12,136,got?WLM[sel].place:"NOT YET DISCOVERED",C_WHITE,1);m3_text(12,145,got?WLM[sel].fact:"FLY OVER IT BELOW 120,000 FT",C_GRAY,1);m3_present();}
  next_frame();if(pressed&K_UP){sel=(sel+NLM-1)%NLM;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%NLM;redraw=1;sfx_menu();}if(pressed&(K_B|K_A|K_START)){sfx_menu();return;}}}
static void w_m3spr(int ac,int x0,int y0,int sc){const u32*t=wf_tiles+ac*128;const u16*p=wf_pal+ac*16;
 for(int ty=0;ty<4;ty++)for(int tx=0;tx<4;tx++)for(int y=0;y<8;y++){u32 w=t[(ty*4+tx)*8+y];for(int x=0;x<8;x++){int c=(w>>(x*4))&15;if(!c)continue;int X=x0+(tx*8+x)*sc,Y=y0+(ty*8+y)*sc;for(int a=0;a<sc;a++)for(int b=0;b<sc;b++)if(X+b<240&&Y+a<160)fb[(Y+a)*240+X+b]=p[c];}}}
// big side-view art (RLE, shared 8bpp palette) - centred on (cx, top y0 of the 64px art cell)
static void w_big(int ac,int cx,int y0){const WAc*A=&WAC[ac];const u8*d=wf_big+A->off;int w=A->bw,h=A->bh,x0=cx-w/2,yy=y0+A->by;int n=w*h,k=0;
 while(k<n){int r=*d++,c=*d++;if(c){u16 col=wf_bpal[c];for(int j=0;j<r;j++){int q=k+j;int X=x0+q%w,Y=yy+q/w;if((unsigned)X<240&&(unsigned)Y<160)fb[Y*240+X]=col;}}k+=r;}}
static void w_band(int y0){const u8*b=wf_band;for(int y=0;y<72;y++){u16*f=&fb[(y0+y)*240];for(int x=0;x<240;x++)f[x]=wf_bpal[b[y*240+x]];}}
// drop shadow under the side art
static void w_bigshadow(int ac,int cx,int y){const WAc*A=&WAC[ac];int w=A->bw*3/4;for(int x=cx-w/2;x<cx+w/2;x++){if(x<0||x>=240)continue;u16*f=&fb[y*240+x];*f=(*f>>1)&0x3DEF;f[240]=(f[240]>>1)&0x3DEF;}}
static int wf_match(int i,int era,int ct){return (era>2||WAC[i].era==era)&&(ct>4||WAC[i].cat==ct);}
static int wf_count(int era,int ct){int n=0;for(int i=0;i<NWAC;i++)n+=wf_match(i,era,ct);return n;}
static void stat_bar(int x,int y,const char*l,int v,int mx,u16 c){m3_text(x,y,l,C_GRAY,1);int w=v*56/(mx>0?mx:1);if(w>56)w=56;if(w<1)w=1;m3_fill(x+34,y+1,58,5,RGB(20,20,26));m3_fill(x+35,y+2,w,3,c);}
// v1.3.5 aircraft browser: 113 aircraft, all unlocked. L/R era, SELECT category, LEFT/RIGHT browse, A choose, B back
static void w_hangar(void){int era=WAC[fac].era,ct=5,cur=fac,redraw=2;
 for(;;){int cnt=wf_count(era,ct),pos=0;if(!wf_match(cur,era,ct)){for(cur=0;cur<NWAC&&!wf_match(cur,era,ct);cur++);if(cur>=NWAC)cur=0;}
  for(int i=0;i<cur;i++)pos+=wf_match(i,era,ct);
  BEAT->screen=SCR_WFAC;BEAT->menu=cur;BEAT->extra=era|(ct<<4)|(cnt<<8)|(pos<<16);BEAT->scoreA=NWAC;BEAT->scoreB=WAC[cur].cat|(WAC[cur].era<<4);
  if(redraw){redraw=0;const WAc*A=&WAC[cur];
   m3_fill(0,0,240,160,RGB(16,18,14));m3_text(4,2,"AIRCRAFT",C_YEL,1);m3_text(58,2,cat3(itoa(NWAC)," AIRCRAFT - ALL UNLOCKED",""),C_GREEN,1);
   static const char*const EN[4]={"WW2","COLD WAR","MODERN","ALL"};
   {int x=4;for(int e=0;e<4;e++){const char*t=cat3(EN[e]," ",itoa(wf_count(e,5)));int w=slen(t)*6+5;m3_fill(x,12,w,10,e==era?RGB(120,110,60):RGB(40,42,34));m3_text(x+3,14,t,e==era?C_WHITE:C_GRAY,1);x+=w+2;}}
   m3_text(4,25,cat3("TYPE: ",ct>4?"ALL TYPES":WCATN[ct],""),C_YEL,1);
   w_band(34);if(cnt){w_bigshadow(cur,120,34+A->by+A->bh+4);w_big(cur,120,38);}
   m3_text(4,36,cat3(itoa(pos+1),"/",itoa(cnt)),C_WHITE,1);
   if(cnt){m3_text(4,108,A->n,C_YEL,1);{char b[48];int k=0;const char*pp[5]={WERAN[A->era]," - ",WCATS[A->cat]," - ",A->cty};for(int j=0;j<5;j++)for(const char*q=pp[j];*q&&k<46;q++)b[k++]=*q;b[k]=0;m3_text(4,117,b,C_GRAY,1);}
    m3_text(4,126,A->f1,C_WHITE,1);m3_text(4,134,A->f2,C_WHITE,1);
    if(A->cat==2){m3_text(150,108,cat(itoa(A->vmax*661/10000)," KT"),C_GREEN,1);}else{char b[8];int m=A->vmax/100;b[0]='M';b[1]='0'+m/100%10;b[2]='.';b[3]='0'+m/10%10;b[4]='0'+m%10;b[5]=0;m3_text(150,108,cat("TOP ",b),C_GREEN,1);}
    stat_bar(4,143,"SPEED",A->vmax>30000?30000:A->vmax,30000,RGB(240,200,60));stat_bar(100,143,"AGILE",A->agil,10,RGB(90,210,120));
    m3_text(196,143,A->cat==2?"HOVER":A->rocket==1?"DROP":A->rocket?"ROCKET":"",C_YEL,1);}
   m3_text(4,153,"<>BROWSE L/R:ERA SEL:TYPE A:FLY B:BACK",C_GRAY,1);m3_present();}
  next_frame();
  int dir=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(dir&&cnt){int i=cur;do{i=(i+NWAC+dir)%NWAC;}while(!wf_match(i,era,ct));cur=i;redraw=1;sfx_menu();}
  if(pressed&K_R){era=(era+1)%4;redraw=1;sfx_menu();}if(pressed&K_L){era=(era+3)%4;redraw=1;sfx_menu();}
  if(pressed&K_SEL){ct=ct>=5?0:ct+1;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){if(cnt){fac=cur;WS.ac=fac;}sfx_ok();return;}
  if(pressed&K_B){sfx_menu();return;}}}
static void flight_menu(void){ws_read();int sel=0,redraw=2;fcourse=0;fac=WS.ac;
 for(;;){BEAT->screen=SCR_WMENU;BEAT->menu=sel;BEAT->extra=(fac&15)|(fcourse<<4)|(nlogged()<<8)|(fac<<16);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);m3_ctext(10,"FLIGHT AROUND THE WORLD",C_YEL,1);
   static const char*const IT[6]={"FREE FLIGHT","RING COURSE","CIRCUMNAVIGATION","LANDMARK LOG","AIRCRAFT","BACK"};
   for(int k=0;k<6;k++){int y=24+k*11;u16 c=k==sel?C_YEL:C_WHITE;m3_text(14,y,cat(k==sel?"> ":"  ",IT[k]),c,1);
    if(k==1){m3_text(104,y,cat3("<",WCO[fcourse].n,">"),c,1);}
    if(k==2)m3_text(122,y,WS.circ?tmr(WS.circ):"--:--",C_GRAY,1);
    if(k==3)m3_text(122,y,cat3(itoa(nlogged()),"/",itoa(NLM)),C_GREEN,1);
    if(k==4)m3_text(80,y,cat3("<",WAC[fac].s,">"),c,1);}
   const WAc*A=&WAC[fac];w_m3spr(fac,12,92,1);
   m3_text(50,94,A->n,C_YEL,1);m3_text(50,104,cat3(WERAN[A->era]," - ",WCATS[A->cat]),C_GRAY,1);m3_text(12,114,A->f1,C_WHITE,1);
   m3_text(12,123,WHELI(A)?"HOVERS: LAND ON ANY OPEN GROUND":A->rocket==1?"REFUEL: MOTHERSHIP OR AIRFIELD":"REFUEL: LAND AT ANY AIRFIELD",C_GRAY,1);
   m3_text(12,132,cat3("MAX ALT ",commas(WS.maxAlt)," FT"),C_GRAY,1);
   m3_ctext(144,sel==0?"FLY LOW OVER LANDMARKS TO LOG THEM":sel==1?(WS.best[fcourse]?cat3(WCO[fcourse].desc," - BEST ",tmr(WS.best[fcourse])):WCO[fcourse].desc):sel==2?"FULL LAP OF THE GLOBE, TIMED":sel==3?"YOUR DISCOVERIES":sel==4?"A: BROWSE ALL 113 BY ERA + TYPE":"",C_GRAY,1);
   m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+5)%6;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%6;redraw=1;sfx_menu();}
  int dir=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;
  if(dir&&sel==1){fcourse=(fcourse+NCOURSE+dir)%NCOURSE;redraw=1;sfx_menu();}
  if(dir&&sel==4){fac=(fac+NWAC+dir)%NWAC;WS.ac=fac;redraw=1;sfx_menu();}
  if(pressed&K_B){ws_write();return;}
  if(pressed&(K_A|K_START)){
   if(sel==5){ws_write();return;}
   if(sel==3){w_log();redraw=2;continue;}
   if(sel==4){w_hangar();ws_write();redraw=2;continue;}
   WS.ac=fac;fmode=sel==0?0:sel==1?1:2;sfx_ok();loading(fmode==0?"FREE FLIGHT":fmode==1?WCO[fcourse].n:"CIRCUMNAVIGATION");fzoom=256;w_run();bgm(1,0);redraw=2;}}}
