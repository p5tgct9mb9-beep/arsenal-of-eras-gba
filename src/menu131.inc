// ===================================================================== v1.3.1 MAIN MENU: Earth from orbit
// Static layer (bigbuf): deep-space gradient, atmosphere glow, metallic logo, translucent HUD panel; the Earth disc is
// re-rendered progressively (a quarter per frame) from a precomputed sphere LUT so continents + clouds scroll.
// Per frame on top: parallax twinkling stars, the ISS drifting by, periodic launches (Falcon 9 / Saturn V / Shuttle / Soyuz).

static int mmPX1=122;
#define MP_X1 mmPX1
#define MP_Y0 30
#define MP_Y1 136
#define MD_Y0 138
#define MMV ((u16*)0x06000000)
static void mm_kc_bake(int x0,int y0);static void mm_stars_init(void);
static int mm_ls,mm_cs,mm_part;
static inline int mm_under(int x,int y){return (x>=4&&x<MP_X1&&y>=MP_Y0&&y<MP_Y1)||(x>=4&&x<236&&y>=MD_Y0&&y<158);}
IWCODE static void mm_earth_rows(int y0,int y1,int both){u16*B=bigbuf;int ls=mm_ls,cs=mm_cs;
 for(int y=y0;y<y1;y++){kl_poll();int x0=ea_row[y*3],n=ea_row[y*3+1],o=ea_row[y*3+2];
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
 for(int y=y0-1;y<y0+15;y++)for(int x=(kl_poll(),x0-2);x<x0+w+2;x++){u16 c=fb[y*240+x];if(c==RGB(250,250,255)||c==RGB(8,248,8))continue;
  int edge=0;for(int k=0;k<4;k++){int xx=x+(k==0)-(k==1),yy=y+(k==2)-(k==3);u16 q=fb[yy*240+xx];for(int j=0;j<14;j++)if(q==G[j]){edge=1;break;}if(edge)break;}
  int isg=0;for(int j=0;j<14;j++)if(c==G[j]){isg=1;break;}if(edge&&!isg)fb[y*240+x]=RGB(10,14,30);}
 m3_text(120-(20*6)/2,22,"FIELD EDITION V1.3.4",RGB(110,200,230),1);}
static void mm_static(void){
 for(int y=0;y<160;y++){kl_poll();u16 c=RGB(2+y/40,3+y/30,10+y/12);m3_fill(0,y,240,1,c);}
 // atmosphere glow above the limb
 for(int x=0;x<240;x++){int t=ea_top[x];if(t>=160)continue;static const u16 GL[6]={RGB(10,20,60),RGB(20,44,110),RGB(40,90,180),RGB(80,150,230),RGB(150,210,255),RGB(200,236,255)};
  for(int k=0;k<6;k++){int y=t-6+k;if(y>=0&&y<160)fb[y*240+x]=GL[k];}}
 mm_logo();mm_kc_bake(216,3);mm_stars_init();
 // translucent HUD panels over the sky part
 for(int y=MP_Y0;y<158;y++){kl_poll();for(int x=4;x<236;x++){if(!mm_under(x,y))continue;u16 c=fb[y*240+x];int r=c&31,g=(c>>5)&31,b=(c>>10)&31;
  r=(r*3+1*5)>>3;g=(g*3+2*5)>>3;b=(b*3+6*5)>>3;fb[y*240+x]=r|(g<<5)|(b<<10);}}
 dma32(fb,bigbuf,240*160/2);mm_earth_rows(0,160,0);}
static void mm_icon2(int k,int x,int y){u16*F=fb;
 #define IP(a,b,w,h,c) do{for(int _j=0;_j<(h);_j++)for(int _i=0;_i<(w);_i++)F[(y+(b)+_j)*240+x+(a)+_i]=(c);}while(0)
 if(k==CAT_RACE){IP(1,5,10,4,RGB(230,40,40));IP(3,3,5,2,RGB(230,40,40));IP(4,3,3,2,RGB(120,190,240));IP(1,9,3,2,RGB(20,20,20));IP(8,9,3,2,RGB(20,20,20));}
 else if(k==CAT_HANGAR){IP(5,1,2,10,RGB(200,206,220));IP(0,5,12,2,RGB(200,206,220));IP(3,10,6,1,RGB(200,206,220));IP(5,0,2,1,RGB(120,190,240));}
 else if(k==CAT_ARCADE){IP(2,2,8,4,RGB(110,240,120));IP(1,4,10,2,RGB(110,240,120));IP(3,3,2,1,0);IP(7,3,2,1,0);IP(1,6,2,2,RGB(110,240,120));IP(9,6,2,2,RGB(110,240,120));IP(5,8,2,3,RGB(250,250,250));}
 else if(k==CAT_FIRE){IP(3,1,6,4,RGB(20,20,22));IP(1,4,10,2,RGB(20,20,22));IP(0,5,12,1,RGB(20,20,22));IP(5,1,2,3,RGB(236,196,92));IP(4,7,4,4,RGB(255,140,30));IP(5,6,2,2,RGB(255,220,80));}
 else{IP(1,2,10,8,RGB(236,196,92));IP(2,3,8,6,RGB(40,30,20));IP(3,4,6,1,RGB(236,196,92));IP(3,6,4,1,RGB(236,196,92));}
 #undef IP
}
static void mm_icon(int k,int x0,int y0){if(k>=8){mm_icon2(k,x0,y0);return;}const u8*s=&mi_icon[k*144];for(int y=0;y<12;y++)for(int x=0;x<12;x++){int v=s[y*12+x];if(v)fb[(y0+y)*240+x0+x]=mi_pal[v];}}

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
// ---- v1.3.4: ISS, launches, exhaust trail and stage separation are hardware OBJ sprites (8bpp, bitmap-mode OBJ VRAM at
// 0x06014000). They are pre-rendered once per menu entry, positioned through the core OAM shadow and copied to OAM by DMA
// at the start of VBlank, so nothing is erased/redrawn on the visible bitmap any more (v1.3.3 blinked + tore). Windows
// WIN0/WIN1 keep the sprites out of the menu panel and the description bar.
#define MOV ((volatile u16*)0x06014000)
#ifndef REG_WIN0H
#define REG_WIN0H REG16(0x4000040)
#define REG_WIN1H REG16(0x4000042)
#define REG_WIN0V REG16(0x4000044)
#define REG_WIN1V REG16(0x4000046)
#define REG_WININ REG16(0x4000048)
#define REG_WINOUT REG16(0x400004A)
#endif
static u16 mmOP[64] EWRAM;static int mmOPn;static int mmOT; // OBJ palette mirror + next free 32-byte tile slot
static int mm_pi(u16 c){for(int i=1;i<mmOPn;i++)if(mmOP[i]==c)return i;if(mmOPn>=64)return 1;mmOP[mmOPn]=c;((volatile u16*)0x05000200)[mmOPn]=c;return mmOPn++;}
#define MMKEY RGB(255,0,255)
static int mm_bake(int cx,int cy,int w,int h){ // fb canvas -> 8bpp 1D tiles; returns the OBJ tile number
 int t0=mmOT;volatile u16*D=&MOV[mmOT*16];
 for(int ty=0;ty<h;ty+=8)for(int tx=0;tx<w;tx+=8)for(int y=0;y<8;y++)for(int x=0;x<8;x+=2){u16 a=fb[(cy+ty+y)*240+cx+tx+x],b=fb[(cy+ty+y)*240+cx+tx+x+1];
   int ia=a==MMKEY?0:mm_pi(a),ib=b==MMKEY?0:mm_pi(b);*D++=ia|(ib<<8);}
 mmOT+=w*h/32;return 512+t0;}
static void cv_r(int x,int y,int w,int h,u16 c){for(int j=0;j<h;j++)for(int i=0;i<w;i++){int X=x+i,Y=y+j;if((unsigned)X<240&&(unsigned)Y<160)fb[Y*240+X]=c;}}
static int mmTI,mmTR[8],mmTS[4],mmTT[4],mmTL[4];
static const char*const MM_LN[4]={"FALCON 9","SATURN V","SPACE SHUTTLE","SOYUZ"};
static void mm_rocket_cv(int k,int st,int x,int y){u16 W=RGB(236,236,232),B=RGB(30,30,34),F1=RGB(255,250,210),F2=RGB(255,170,60);
 int fl=st?2:5;cv_r(x,y+1,k==1?3:2,fl/2,F1);cv_r(x,y+1+fl/2,k==1?3:2,fl/2+1,F2);
 switch(k){
  case 0:if(!st){cv_r(x,y-11,2,12,W);cv_r(x,y-8,2,1,B);cv_r(x,y-1,2,1,B);}else cv_r(x,y-4,2,4,W);cv_r(x,y-12+(st?8:0),1,1,W);break;
  case 1:if(!st){cv_r(x,y-14,3,15,W);cv_r(x,y-1,3,1,B);cv_r(x,y-5,1,2,B);cv_r(x+2,y-10,1,2,B);}else{cv_r(x,y-8,3,9,W);cv_r(x,y-1,1,1,B);}cv_r(x+1,y-(st?11:17),1,3,RGB(200,90,60));break;
  case 2:cv_r(x,y-10,2,11,RGB(206,120,60));cv_r(x+2,y-7,2,7,W);cv_r(x+2,y-1,2,1,B);if(!st){cv_r(x-1,y-9,1,10,W);cv_r(x+4,y-9,1,10,W);}break;
  default:cv_r(x,y-12,2,13,RGB(150,156,136));cv_r(x,y-14,2,2,RGB(200,204,190));if(!st){cv_r(x-1,y-4,1,5,RGB(150,156,136));cv_r(x+2,y-4,1,5,RGB(150,156,136));}break;}}
static void mm_obj_init(void){mmOPn=1;mmOT=0;((volatile u16*)0x05000200)[0]=0;
 cv_r(0,0,64,32,MMKEY);{int x=32,y=16;u16 TR=RGB(170,174,182),AR=RGB(200,150,60),AD=RGB(110,70,30),MW=RGB(236,236,232),RD=RGB(220,226,236);
  cv_r(x-17,y,35,1,TR);cv_r(x-17,y+1,35,1,RGB(110,114,122));
  for(int s=0;s<4;s++){int ax=s<2?x-17+s*5:x+9+(s-2)*5;for(int k=-1;k<=1;k+=2){int ay=k<0?y-9:y+3;cv_r(ax,ay,4,7,AR);for(int j=0;j<7;j+=2)cv_r(ax,ay+j,4,1,AD);cv_r(ax+1,k<0?y-2:y+2,1,1,TR);}}
  cv_r(x-2,y-4,4,2,RD);cv_r(x-3,y+3,6,2,RD);cv_r(x-1,y-1,3,9,MW);cv_r(x-1,y+8,3,3,RGB(200,204,210));cv_r(x+2,y+5,4,2,MW);cv_r(x-5,y+5,4,2,MW);cv_r(x,y+11,1,3,RGB(170,170,170));}
 mmTI=mm_bake(0,0,64,32);
 for(int k=0;k<4;k++)for(int st=0;st<2;st++){cv_r(0,0,16,32,MMKEY);mm_rocket_cv(k,st,6,24);mmTR[k*2+st]=mm_bake(0,0,16,32);}
 for(int k=0;k<4;k++){cv_r(0,0,8,8,MMKEY);u16 c=k==3?RGB(150,156,136):RGB(236,236,232);if(k>=2)cv_r(3,1,1,6,c);else cv_r(3,1,k==1?3:2,6,c);mmTS[k]=mm_bake(0,0,8,8);}
 {static const u16 PC[4]={RGB(255,236,180),RGB(230,230,220),RGB(170,176,190),RGB(110,116,134)};for(int i=0;i<4;i++){cv_r(0,0,8,8,MMKEY);cv_r(3,3,2,2,PC[i]);mmTT[i]=mm_bake(0,0,8,8);}}
 for(int k=0;k<4;k++){cv_r(0,0,96,8,MMKEY);m3_text(0,0,MM_LN[k],RGB(140,190,220),1);mmTL[k]=mm_bake(0,0,96,8);}}
static void mspr(int x,int y,int tile,int shape,int size){int n=noam;spr(x,y,tile,shape,size,0,0);if(noam>n)oam[n*4]|=1<<13;}
static s16 mmTx[32] EWRAM,mmTy[32] EWRAM;static int mmTn;
static void mm_obj_build(void){oam_begin();
 {int t=frame%1500;int x=262-t/4;if(x>=100&&x<=262){int y=46+((x-190)*(x-190))/1800;mspr(x-32,y-16,mmTI,1,3);}}
 {int cyc=frame/720,t=frame%720;int k=cyc&3;static const u8 LX[4]={196,166,224,184};if(t==0)mmTn=0;
  if(t<=420){int lx=LX[k],by=ea_top[lx]+5;int x=lx+(t*t)/5200,y=by-t*3/10-(t*t)/9000;int st=t>170,sep=t-170;
   if(!(t&1)){for(int i=31;i>0;i--){mmTx[i]=mmTx[i-1];mmTy[i]=mmTy[i-1];}mmTx[0]=x;mmTy[0]=y+2;if(mmTn<32)mmTn++;}
   if(t<200){int lw=slen(MM_LN[k])*6;int lx2=x+12+lw>236?x-6-lw:x+6;for(int p=0;p*32<lw;p++)mspr(lx2+p*32,y-12,mmTL[k]+p*8,1,1);}
   mspr(x-6,y-24,mmTR[k*2+st],2,2);
   if(st&&sep<120){int sx=lx+(170*170)/5200-sep/12,sy=by-51-(170*170)/9000+sep/3+(sep*sep)/700;
    if(k>=2){mspr(sx-3-sep/10-3,sy-7,mmTS[k],0,0);mspr(sx+3+sep/10-3,sy-7,mmTS[k],0,0);}else mspr(sx-3,sy-7,mmTS[k],0,0);}
   {int lim=st?18:mmTn;int px=x,py=y+2;for(int i=0;i<mmTn&&i<lim&&noam<120;i++){int qx=mmTx[i],qy=mmTy[i];int d=iabs(qx-px)+iabs(qy-py);int n=d/2+1;int sh=i<4?0:i<11?1:i<21?2:3;
     for(int j=1;j<=n&&noam<120;j++)mspr(px+(qx-px)*j/n-4,py+(qy-py)*j/n-4,mmTT[sh],0,0);px=qx;py=qy;}}}}
}
// called right after vsync: OAM + windows change only inside VBlank (the shadow was built during the previous frame)
static void mm_obj_push(void){dma32(oam,OAM,256);REG_WIN0H=(4<<8)|MP_X1;REG_WIN0V=(MP_Y0<<8)|MP_Y1;REG_WIN1H=(4<<8)|236;REG_WIN1V=(MD_Y0<<8)|158;}
static void mm_obj_frame(void){mm_obj_build();mm_obj_push();}
static void mm_obj_on(void){REG_WININ=0x0404;REG_WINOUT=0x0014;REG_DISPCNT=3|(1<<10)|(1<<12)|(1<<6)|(1<<13)|(1<<14);}
static void mm_obj_off(void){oam_begin();dma32(oam,OAM,256);REG_DISPCNT=3|(1<<10);}
static void mm_kc_bake(int x0,int y0){for(int y=0;y<20;y++)for(int x=0;x<20;x++){int v=kc_astro[y*20+x];if(v)fb[(y0+y)*240+x0+x]=kc_astro_pal[v];}}
static void mm_kc(int x0,int y0){
 if((frame&127)<6){mm_px(x0+8,y0+6,RGB(200,140,70));mm_px(x0+13,y0+6,RGB(200,140,70));}mm_px(x0+14,y0+3+((frame>>4)&1),RGB(255,255,255));}
static int main_menu3(int sel){static const char*const MN[CAT_N]={"CAMPAIGN","BATTLES","SPEC OPS","SPECIALIST","EXPLORE","LOADOUT","SETTINGS","BOOT CAMP (KC)","RACING","HANGAR","ARCADE","FIREFIGHTER","CREDITS"};
 static const char*const L1[CAT_N]={"WW2 - COLD WAR - MODERN","6V6 INFANTRY - ARMOR - DOGFIGHT","SPEC OPS PLATFORMER","SNIPER - EOD - LRRP - CLASSIFIED","SPACE LAUNCH - MOON - MARS - DEFENSE","KITS - 6V6 ARSENAL - CHARACTER","MUSIC - VOLUME - DIFFICULTY","CONTROLS + EVERY MODE","TESLA EV GRAND PRIX - TIME TRIAL","AIRCRAFT BUILDER: PLANES - DRONES","ROCKET INVADERS","10 CALLS - RESCUE, HOSE, LADDER","ARSENAL OF ERAS"};
 static const char*const L2[CAT_N]={"WHAT-IF SCENARIOS","NAVAL - SWORD FIGHT 1V1 - FPS","SPEC OPS: BRADLEY","ANTI-AIRCRAFT - DRONE OPS","ROCKET BUILDER - ASTEROIDS - HISTORY","","BOT SKILL - HINTS - MUSIC PLAYER","WITH KC THE DOG","FREE DRIVE - 4 TRACKS, MODE 7","HELICOPTERS - TEST FLIGHT","ORIGINAL ROCKET ARCADE SHOOTER","NEW! DEDICATED TO MY DAD","THE PEOPLE (AND DOG) BEHIND IT"};
 bgCached=-1;mm_ls=0;mm_cs=0;mm_part=0;mmLn=0;mm_obj_init();mm_static();lfb=0;dma32(bigbuf,fb,240*160/2);vsync();dma32(bigbuf,MMV,240*160/2);mm_obj_frame();mm_obj_on();int dirty=1;
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  next_frame();
  mm_obj_push();mm_restore();mm_stars();mm_kc(216,3);
  if(dirty){dirty=0;for(int y=MP_Y0;y<158;y++)dma32(&bigbuf[y*240+4],&fb[y*240+4],116);
   u16 CY=RGB(90,210,240),GD=RGB(236,196,92);
   m3_fill(4,MP_Y0,MP_X1-4,1,CY);m3_fill(4,MP_Y1-1,MP_X1-4,1,CY);m3_fill(4,MP_Y0,1,MP_Y1-MP_Y0,CY);m3_fill(MP_X1-1,MP_Y0,1,MP_Y1-MP_Y0,CY);
   m3_fill(4,MP_Y0,14,2,GD);m3_fill(MP_X1-15,MP_Y1-2,14,2,GD);m3_fill(4,MD_Y0,232,1,CY);m3_fill(4,157,232,1,CY);m3_fill(4,MD_Y0,10,2,GD);m3_fill(226,156,10,2,GD);
   int top=sel-7;if(top<0)top=0;if(top>CAT_N-8)top=CAT_N-8;
   if(top>0)m3_text(110,31,"^",C_YEL,1);if(top<CAT_N-8)m3_text(110,128,"v",C_YEL,1);
   for(int k=top;k<top+8;k++){int y=34+(k-top)*13;int on=k==sel;
    if(on){m3_fill(6,y-2,MP_X1-8,12,RGB(16,60,110));m3_fill(6,y-2,2,12,CY);}
    mm_icon(k,10,y-2);m3_text(25,y,MN[k],on?C_YEL:C_WHITE,1);if(k==CAT_FIRE){m3_fill(100,y-1,21,9,RGB(220,40,30));m3_text(102,y,"NEW",C_WHITE,1);}}
   ltx(10,140,L1[sel],RGB(170,200,220));ltx(10,149,L2[sel],RGB(130,150,180));
   for(int y=MP_Y0;y<158;y++)dma32(&fb[y*240+4],&MMV[y*240+4],116);}
  // progressive Earth outside the panel: one sixth of the rows per frame
  {static const u8 PR[11]={0,104,112,119,126,132,138,143,148,154,160};mm_earth_rows(PR[mm_part],PR[mm_part+1],1);if(++mm_part>=10){mm_part=0;mm_ls=(mm_ls+1)&511;if(frame&8)mm_cs=(mm_cs+1)&511;}}mm_obj_build();

  if(pressed&K_UP){sel=(sel+CAT_N-1)%CAT_N;sfx_menu();dirty=1;}
  if(pressed&K_DOWN){sel=(sel+1)%CAT_N;sfx_menu();dirty=1;}
  if(pressed&(K_LEFT|K_RIGHT)){sel=sel<8?(sel+4)%8:(pressed&K_LEFT?0:CAT_N-1);sfx_menu();dirty=1;}
  if(pressed&(K_A|K_START)){sfx_ok();bgCached=-1;mm_obj_off();return sel;}
  if(pressed&K_B){bgCached=-1;mm_obj_off();return -1;}}}
