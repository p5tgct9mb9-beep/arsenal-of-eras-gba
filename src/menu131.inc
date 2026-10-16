// ===================================================================== v1.3.1 MAIN MENU: Earth from orbit
// Static layer (bigbuf): deep-space gradient, atmosphere glow, metallic logo, translucent HUD panel; the Earth disc is
// re-rendered progressively (a quarter per frame) from a precomputed sphere LUT so continents + clouds scroll.
// Per frame on top: parallax twinkling stars, the ISS drifting by, periodic launches (Falcon 9 / Saturn V / Shuttle / Soyuz).
#include "v131db.inc"
#define MP_X1 122
#define MP_Y0 30
#define MP_Y1 136
#define MD_Y0 138
#define MMV ((u16*)0x06000000)
static void mm_kc_bake(int x0,int y0);static void mm_stars_init(void);
static int mm_ls,mm_cs,mm_part;
static inline int mm_under(int x,int y){return (x>=4&&x<MP_X1&&y>=MP_Y0&&y<MP_Y1)||(x>=4&&x<236&&y>=MD_Y0&&y<158);}
IWCODE static void mm_earth_rows(int y0,int y1,int both){u16*B=bigbuf;int ls=mm_ls,cs=mm_cs;
 for(int y=y0;y<y1;y++){int x0=ea_row[y*3],n=ea_row[y*3+1],o=ea_row[y*3+2];
  int pu0=(y>=MP_Y0&&y<MP_Y1)?MP_X1:(y>=MD_Y0&&y<158)?236:0;int i0=0;if(both&&pu0>x0){i0=pu0-x0;if(i0>n)i0=n;}
  u16*d=&B[y*240+x0];u16*f=&MMV[y*240+x0];const u16*L=&ea_lut[o];const u8*S=&ea_sh[o];
  for(int i=i0;i<n;i++){int l=L[i];int u=l&511,v=l>>9;int sh=S[i];int t=ea_tex[(v<<9)+((u+ls)&511)];int c=ea_cld[(v<<9)+((u+cs)&511)];
   if(c==2)t=7;else if(c==1&&t!=6&&((x0+i+y)&1))t=8;
   if(t==6&&(sh&15))t=2;
   int k=(t*16+(sh&15))*4+(sh>>4);if(x0+i<pu0&&x0+i>=4)k+=EA_NT*64;u16 cc=ea_col[k];d[i]=cc;if(both)f[i]=cc;}}}
static void mm_logo(void){const char*s="ARSENAL OF ERAS";int w=slen(s)*12,x0=120-w/2,y0=5;u16 M=RGB(8,248,8);
 m3_text(x0,y0,s,M,2);
 static const u16 G[14]={RGB(250,250,255),RGB(236,240,248),RGB(214,222,236),RGB(190,200,220),RGB(160,172,196),RGB(120,132,160),RGB(250,224,150),RGB(240,200,110),RGB(222,176,80),RGB(200,150,60),RGB(176,128,48),RGB(150,104,40),RGB(120,84,32),RGB(100,70,28)};
 for(int y=y0;y<y0+14;y++)for(int x=x0-1;x<x0+w+1;x++){u16*p=&fb[y*240+x];if(*p==M)*p=G[y-y0];}
 for(int y=y0-1;y<y0+15;y++)for(int x=x0-2;x<x0+w+2;x++){u16 c=fb[y*240+x];if(c==RGB(250,250,255)||c==RGB(8,248,8))continue;
  int edge=0;for(int k=0;k<4;k++){int xx=x+(k==0)-(k==1),yy=y+(k==2)-(k==3);u16 q=fb[yy*240+xx];for(int j=0;j<14;j++)if(q==G[j]){edge=1;break;}if(edge)break;}
  int isg=0;for(int j=0;j<14;j++)if(c==G[j]){isg=1;break;}if(edge&&!isg)fb[y*240+x]=RGB(10,14,30);}
 m3_text(120-(20*6)/2,22,"FIELD EDITION V1.3.1",RGB(110,200,230),1);}
static void mm_static(void){
 for(int y=0;y<160;y++){u16 c=RGB(2+y/40,3+y/30,10+y/12);m3_fill(0,y,240,1,c);}
 // atmosphere glow above the limb
 for(int x=0;x<240;x++){int t=ea_top[x];if(t>=160)continue;static const u16 GL[6]={RGB(10,20,60),RGB(20,44,110),RGB(40,90,180),RGB(80,150,230),RGB(150,210,255),RGB(200,236,255)};
  for(int k=0;k<6;k++){int y=t-6+k;if(y>=0&&y<160)fb[y*240+x]=GL[k];}}
 mm_logo();mm_kc_bake(216,3);mm_stars_init();
 // translucent HUD panels over the sky part
 for(int y=MP_Y0;y<158;y++)for(int x=4;x<236;x++){if(!mm_under(x,y))continue;u16 c=fb[y*240+x];int r=c&31,g=(c>>5)&31,b=(c>>10)&31;
  r=(r*3+1*5)>>3;g=(g*3+2*5)>>3;b=(b*3+6*5)>>3;fb[y*240+x]=r|(g<<5)|(b<<10);}
 dma32(fb,bigbuf,240*160/2);mm_earth_rows(0,160,0);}
static void mm_icon(int k,int x0,int y0){const u8*s=&mi_icon[k*144];for(int y=0;y<12;y++)for(int x=0;x<12;x++){int v=s[y*12+x];if(v)fb[(y0+y)*240+x0+x]=mi_pal[v];}}

static u16 mmLog[1600] EWRAM;static int mmLn;
static inline void mm_px(int x,int y,u16 c){if((unsigned)x<240&&(unsigned)y<160&&!mm_under(x,y)&&mmLn<1600){int i=y*240+x;mmLog[mmLn++]=i;MMV[i]=c;}}
static void mm_restore(void){u16*B=bigbuf;for(int k=0;k<mmLn;k++){int i=mmLog[k];MMV[i]=B[i];}mmLn=0;}
static void mm_txt(int x,int y,const char*s,u16 col){for(;*s;s++,x+=6){int c=*s;if(c<=32||c>127)continue;const u8*g=&font_bits[(c-32)*7];
  for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<gx))mm_px(x+gx,y+gy,col);}}
IWCODE static void mm_rect(int x,int y,int w,int h,u16 c){for(int j=0;j<h;j++)for(int i=0;i<w;i++)mm_px(x+i,y+j,c);}
static s16 mmSx[72] EWRAM;static u8 mmSy[72] EWRAM;
static void mm_stars_init(void){for(int i=0;i<72;i++){mmSx[i]=((i*97+13)%240)<<3;mmSy[i]=(i*53+7)%150+2;}}
static void mm_stars(void){
 for(int i=0;i<72;i++){int lay=i%3;mmSx[i]-=lay==0?1:lay==1?2:4;if(mmSx[i]<0)mmSx[i]+=240<<3;int x=mmSx[i]>>3,y=mmSy[i];
  if(y<24||y>=ea_top[x]-7)continue;int tw=(frame+i*11)&63;u16 c=lay==2?(tw<6?RGB(255,255,255):RGB(190,200,230)):lay==1?RGB(130,140,170):RGB(80,86,110);
  if(lay==2&&tw<3){mm_px(x-1,y,RGB(140,150,190));mm_px(x+1,y,RGB(140,150,190));mm_px(x,y-1,RGB(140,150,190));mm_px(x,y+1,RGB(140,150,190));}
  mm_px(x,y,c);}}
static void mm_iss(void){int t=frame%1500;int x=262-t/4;if(x<100||x>262)return;int y=46+((x-190)*(x-190))/1800;
 u16 TR=RGB(170,174,182),AR=RGB(200,150,60),AD=RGB(110,70,30),MW=RGB(236,236,232),RD=RGB(220,226,236);
 mm_rect(x-17,y,35,1,TR);mm_rect(x-17,y+1,35,1,RGB(110,114,122)); // integrated truss
 for(int s=0;s<4;s++){int ax=s<2?x-17+s*5:x+9+(s-2)*5; // 4 pairs of solar array wings
  for(int k=-1;k<=1;k+=2){int ay=k<0?y-9:y+3;mm_rect(ax,ay,4,7,AR);for(int j=0;j<7;j+=2)mm_rect(ax,ay+j,4,1,AD);mm_rect(ax+1,k<0?y-2:y+2,1,1,TR);}}
 mm_rect(x-2,y-4,4,2,RD);mm_rect(x-3,y+3,6,2,RD);// radiators
 mm_rect(x-1,y-1,3,9,MW);mm_rect(x-1,y+8,3,3,RGB(200,204,210));mm_rect(x+2,y+5,4,2,MW);mm_rect(x-5,y+5,4,2,MW);mm_rect(x,y+11,1,3,RGB(170,170,170)); // modules
 if((frame&31)<4)mm_px(x+1,y+2,RGB(255,255,255));}
// launches: 0 Falcon 9, 1 Saturn V, 2 Shuttle, 3 Soyuz
static s16 mmTx[24],mmTy[24];static int mmTn;
static void mm_launch(void){int cyc=frame/720,t=frame%720;int k=cyc&3;static const u8 LX[4]={196,166,224,184};static const char*const LN[4]={"FALCON 9","SATURN V","SPACE SHUTTLE","SOYUZ"};
 if(t==0)mmTn=0;if(t>420)return;int lx=LX[k],by=ea_top[lx]+5;
 int x=lx+(t*t)/5200,y=by-t*3/10-(t*t)/9000;
 if(!(t&3)){for(int i=23;i>0;i--){mmTx[i]=mmTx[i-1];mmTy[i]=mmTy[i-1];}mmTx[0]=x;mmTy[0]=y+2;if(mmTn<24)mmTn++;}
 for(int i=1;i<mmTn;i++){u16 c=i<4?RGB(255,236,180):i<9?RGB(230,230,220):i<16?RGB(170,176,190):RGB(110,116,134);if(t>170&&i>12)continue;mm_px(mmTx[i],mmTy[i],c);if(i<10)mm_px(mmTx[i]+1,mmTy[i],c);}
 int st=t>170;int sep=t-170;
 if(st&&sep<120){int sx=lx+(170*170)/5200-sep/12,sy=by-51-(170*170)/9000+sep/3+(sep*sep)/700;u16 c=k==2?RGB(236,236,232):k==3?RGB(150,156,136):RGB(236,236,232);
  if(k==2||k==3){mm_rect(sx-3-sep/10,sy-6,1,6,c);mm_rect(sx+3+sep/10,sy-6,1,6,c);if(k==3){mm_rect(sx-1,sy+2+sep/10,1,4,c);mm_rect(sx+1,sy-12-sep/12,1,4,c);}}
  else mm_rect(sx,sy-6,k==1?3:2,6,c);}
 u16 W=RGB(236,236,232),B=RGB(30,30,34),F1=RGB(255,250,210),F2=RGB(255,170,60);
 int fl=st?2:4+(frame&1);mm_rect(x,y+1,k==1?3:2,fl/2,F1);mm_rect(x,y+1+fl/2,k==1?3:2,fl/2+1,F2);
 switch(k){
  case 0:if(!st){mm_rect(x,y-11,2,12,W);mm_rect(x,y-8,2,1,B);mm_rect(x,y-1,2,1,B);}else mm_rect(x,y-4,2,4,W);mm_px(x,y-12+(st?8:0),W);break;
  case 1:if(!st){mm_rect(x,y-14,3,15,W);mm_rect(x,y-1,3,1,B);mm_rect(x,y-5,1,2,B);mm_rect(x+2,y-10,1,2,B);}else{mm_rect(x,y-8,3,9,W);mm_rect(x,y-1,1,1,B);}mm_rect(x+1,y-(st?11:17),1,3,RGB(200,90,60));break;
  case 2:mm_rect(x,y-10,2,11,RGB(206,120,60));mm_rect(x+2,y-7,2,7,W);mm_rect(x+2,y-1,2,1,B);if(!st){mm_rect(x-1,y-9,1,10,W);mm_rect(x+4,y-9,1,10,W);}break;
  default:mm_rect(x,y-12,2,13,RGB(150,156,136));mm_rect(x,y-14,2,2,RGB(200,204,190));if(!st){mm_rect(x-1,y-4,1,5,RGB(150,156,136));mm_rect(x+2,y-4,1,5,RGB(150,156,136));}break;}
 if(t<200)mm_txt(x+12+slen(LN[k])*6>236?x-6-slen(LN[k])*6:x+6,y-12,LN[k],RGB(140,190,220));}
static void mm_kc_bake(int x0,int y0){for(int y=0;y<20;y++)for(int x=0;x<20;x++){int v=kc_astro[y*20+x];if(v)fb[(y0+y)*240+x0+x]=kc_astro_pal[v];}}
static void mm_kc(int x0,int y0){
 if((frame&127)<6){mm_px(x0+8,y0+6,RGB(200,140,70));mm_px(x0+13,y0+6,RGB(200,140,70));}mm_px(x0+14,y0+3+((frame>>4)&1),RGB(255,255,255));}
static int main_menu3(int sel){static const char*const MN[CAT_N]={"CAMPAIGN","BATTLES","SPEC OPS","SPECIALIST","EXPLORE","LOADOUT","SETTINGS","BOOT CAMP (KC)"};
 static const char*const L1[CAT_N]={"WW2 - COLD WAR - MODERN","6V6 INFANTRY - ARMOR - DOGFIGHT","SPEC OPS PLATFORMER","SNIPER - EOD - LRRP - CLASSIFIED","SPACE LAUNCH - MOON - MARS - DEFENSE","KITS - 6V6 ARSENAL - CHARACTER","MUSIC - VOLUME - DIFFICULTY","CONTROLS + EVERY MODE"};
 static const char*const L2[CAT_N]={"WHAT-IF SCENARIOS","NAVAL - SWORD FIGHT 1V1 - FPS","SPEC OPS: BRADLEY","ANTI-AIRCRAFT - DRONE OPS","ROCKET BUILDER - ASTEROIDS - HISTORY","","BOT SKILL - HINTS - MUSIC PLAYER","WITH KC THE DOG"};
 bgCached=-1;mm_ls=0;mm_cs=0;mm_part=0;mmLn=0;mm_static();lfb=0;dma32(bigbuf,fb,240*160/2);vsync();dma32(bigbuf,MMV,240*160/2);REG_DISPCNT=3|(1<<10);int dirty=1;
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  next_frame();
  mm_restore();
  if(dirty){dirty=0;for(int y=MP_Y0;y<158;y++)dma32(&bigbuf[y*240+4],&fb[y*240+4],116);
   u16 CY=RGB(90,210,240),GD=RGB(236,196,92);
   m3_fill(4,MP_Y0,MP_X1-4,1,CY);m3_fill(4,MP_Y1-1,MP_X1-4,1,CY);m3_fill(4,MP_Y0,1,MP_Y1-MP_Y0,CY);m3_fill(MP_X1-1,MP_Y0,1,MP_Y1-MP_Y0,CY);
   m3_fill(4,MP_Y0,14,2,GD);m3_fill(MP_X1-15,MP_Y1-2,14,2,GD);m3_fill(4,MD_Y0,232,1,CY);m3_fill(4,157,232,1,CY);m3_fill(4,MD_Y0,10,2,GD);m3_fill(226,156,10,2,GD);
   for(int k=0;k<CAT_N;k++){int y=34+k*13;int on=k==sel;
    if(on){m3_fill(6,y-2,MP_X1-8,12,RGB(16,60,110));m3_fill(6,y-2,2,12,CY);}
    mm_icon(k,10,y-2);m3_text(25,y,MN[k],on?C_YEL:C_WHITE,1);}
   ltx(10,140,L1[sel],RGB(170,200,220));ltx(10,149,L2[sel],RGB(130,150,180));
   for(int y=MP_Y0;y<158;y++)dma32(&fb[y*240+4],&MMV[y*240+4],116);}
  // progressive Earth outside the panel: one sixth of the rows per frame
  {static const u8 PR[11]={0,104,112,119,126,132,138,143,148,154,160};mm_earth_rows(PR[mm_part],PR[mm_part+1],1);if(++mm_part>=10){mm_part=0;mm_ls=(mm_ls+1)&511;if(frame&8)mm_cs=(mm_cs+1)&511;}}
  mm_stars();mm_iss();mm_launch();mm_kc(216,3);
  if(pressed&K_UP){sel=(sel+CAT_N-1)%CAT_N;sfx_menu();dirty=1;}
  if(pressed&K_DOWN){sel=(sel+1)%CAT_N;sfx_menu();dirty=1;}
  if(pressed&(K_LEFT|K_RIGHT)){sel=(sel+4)%8;sfx_menu();dirty=1;}
  if(pressed&(K_A|K_START)){sfx_ok();bgCached=-1;return sel;}
  if(pressed&K_B){bgCached=-1;return -1;}}}
