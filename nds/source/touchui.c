// touchui.c - bottom-screen touch interface (sub engine, 16bpp bitmap).
// Pages: TITLE (tap to start), TILES (the ten main-menu tiles), LIST (a category's modes),
// PAD (virtual GBA pad for everything else).  Taps on menu items are turned into the key
// presses the GBA menu code expects (navigate, then A), so the game logic is unchanged.
#include "platform.h"
#include <string.h>
#include <stdio.h>
extern const unsigned char font_bits[672];
typedef struct{s16 x,y;u8 team,kind;u16 ang;}DsBlip;
typedef struct{int mw,mh,camx,camy,maxhp,hp,gren,fuel,alive,resp,ang,nb;const char*wpn;}DsTac;
typedef struct{int n,laps,phase,me;const u8*surf;s16 x[8],y[8];u8 lap[8],order[8];const char*name[8];u16 h;}DsRace;
int ds_race(DsRace*r);static DsRace RR;static u16 rmap[128*128];static const u8*rmapSrc;
int ds_tac(int scr,DsTac*t,DsBlip*b,int max);int ds_tac_cell(int cx,int cy);int ds_tac_flags(int t);int ds_tac_colours(u16*col);
static u16 ui[256*192] __attribute__((aligned(32)));
static int curPg=-1,dirty=1,page=-1,lastSel=-1,lastScr=-1,padForce=0,lastTouch=0,tx,ty,uiTick;
u32 ds_ui_taps;
#define RGB(r,g,b) ((u16)(0x8000|((b)>>3)<<10|((g)>>3)<<5|((r)>>3)))
enum{PG_TITLE,PG_TILES,PG_LIST,PG_PAD,PG_TAC};
// GBA key bits
enum{GA=1,GB=2,GSEL=4,GSTART=8,GRIGHT=16,GLEFT=32,GUP=64,GDOWN=128,GR=256,GL=512};
// ---- key macro queue
static struct{u16 k;u8 n;}Q[64];static int qh,qt;
static void q_push(u16 k,int n){int nt=(qt+1)&63;if(nt==qh)return;Q[qt].k=k;Q[qt].n=n;qt=nt;}
static void q_tap(u16 k){q_push(k,3);q_push(0,3);}
// ---- drawing
static void fill(int x,int y,int w,int h,u16 c){for(int j=y;j<y+h;j++){if(j<0||j>=192)continue;for(int i=x;i<x+w;i++)if(i>=0&&i<256)ui[j*256+i]=c;}}
static void frame(int x,int y,int w,int h,u16 c){fill(x,y,w,1,c);fill(x,y+h-1,w,1,c);fill(x,y,1,h,c);fill(x+w-1,y,1,h,c);}
static int text(int x,int y,const char*s,u16 c,int sc){
 for(;s&&*s;s++,x+=6*sc){int ch=*s;if(ch>='a'&&ch<='z')ch-=32;if(ch<32||ch>127)ch='?';const u8*g=&font_bits[(ch-32)*7];
  for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<gx))fill(x+gx*sc,y+gy*sc,sc,sc,c);}
 return x;}
static int tw(const char*s,int sc){return s?(int)strlen(s)*6*sc-sc:0;}
static void ctext(int cx,int y,const char*s,u16 c,int sc){text(cx-tw(s,sc)/2,y,s,c,sc);}
static const u16 BG=RGB(8,14,28),PANEL=RGB(18,34,62),CY=RGB(90,210,240),GD=RGB(236,196,92),WH=RGB(235,240,248),GR_=RGB(130,150,180),SEL=RGB(16,60,110);
static void button(int x,int y,int w,int h,const char*l,int on){fill(x,y,w,h,on?SEL:PANEL);frame(x,y,w,h,on?GD:CY);if(on){fill(x,y,10,2,GD);}ctext(x+w/2,y+h/2-3,l,on?GD:WH,1);}
// ---- layout
#define BAR_Y 164
static const char*const BAR[4]={"BACK (B)","SELECT","START","PAD"};
static void draw_bar(void){for(int i=0;i<4;i++)button(2+i*63,BAR_Y,61,26,i==3&&padForce?"MENU":BAR[i],i==3&&padForce);}
static void tile_rect(int i,int*x,int*y,int*w,int*h){*x=4+(i&1)*126;*y=18+(i>>1)*29;*w=122;*h=27;}
static void list_rect(int i,int n,int*x,int*y,int*w,int*h){int rh=n>7?18:20;*x=4;*y=18+i*rh;*w=150;*h=rh-2;}
static void opt_rect(int j,int*x,int*y,int*w,int*h){*x=160;*y=18+j*20;*w=92;*h=18;}
// pad geometry
typedef struct{s16 x,y,w,h;u16 k;const char*l;}PB;
static const PB PADB[]={{40,56,32,32,GUP,"UP"},{40,124,32,32,GDOWN,"DN"},{6,90,32,32,GLEFT,"LT"},{74,90,32,32,GRIGHT,"RT"},
 {200,70,48,44,GA,"A"},{146,104,48,40,GB,"B"},{2,18,70,22,GL,"L"},{184,18,70,22,GR,"R"},{0,0,0,0,0,0}};
static void draw_pad(void){for(const PB*b=PADB;b->k;b++)button(b->x,b->y,b->w,b->h,b->l,(ds_vkeys&b->k)!=0);ctext(128,26,"VIRTUAL PAD",GR_,1);}
static int pad_hit(int x,int y){int k=0;for(const PB*b=PADB;b->k;b++)if(x>=b->x-4&&x<b->x+b->w+4&&y>=b->y-4&&y<b->y+b->h+4)k|=b->k;return k;}

// ---- tactical page: live map of the arena, unit blips, player status and hold-type action buttons
#define TM_X 2
#define TM_Y 18
#define TM_S 142
static u16 tmap[TM_S*TM_S];static int tmKey=-1,tmTick;static DsTac TT;static DsBlip TB[24];
typedef struct{s16 x,y,w,h;u16 k;const char*l;}AB;
static const char*TAC_NAME(int scr){return scr==90?"GRAND PRIX":scr==5?"INFANTRY":scr==6?"ARMOR":scr==7?"AIR":scr==19?"NAVAL":"CAMPAIGN";}
static void tac_buttons(int scr,const char*l[4],u16 k[4]){
 static const char*const L[5][4]={{"FIRE","GRENADE","SWAP","STRAFE"},{"GUN","MG/ATGM","TURRET L","TURRET R"},{"GUNS","ROCKET","ROLL L","ROLL R"},
  {"MAIN GUNS","SECONDARY","TURRET L","TURRET R"},{"FIRE","GRENADE","TACTICAL","STRAFE"}};
 if(scr==90){static const char*const RL[4]={"THROTTLE","BRAKE","BOOST","DRIFT"};for(int i=0;i<4;i++){l[i]=RL[i];k[i]=i==0?GA:i==1?GB:i==2?GL:GR;}return;}
 int r=scr==5?0:scr==6?1:scr==7?2:scr==19?3:4;
 static const u16 K[5][4]={{GA,GB,GL,GR},{GA,GB,GL,GR},{GA,GB,GL,GR},{GA,GB,GL,GR},{GA,GB,GSEL,GR}};
 for(int i=0;i<4;i++){l[i]=L[r][i];k[i]=K[r][i];}}
static void tac_brect(int i,int*x,int*y,int*w,int*h){*x=148+(i&1)*54;*y=104+(i>>1)*30;*w=52;*h=28;}
static int tac_hit(int scr,int x,int y){const char*l[4];u16 k[4];tac_buttons(scr,l,k);int v=0;
 for(int i=0;i<4;i++){int bx,by,bw,bh;tac_brect(i,&bx,&by,&bw,&bh);if(x>=bx-2&&x<bx+bw+2&&y>=by-2&&y<by+bh+2)v|=k[i];}return v;}
static u16 c15(u16 c){return 0x8000|c;}
static void tac_build(void){u16 col[32];int have=ds_tac_colours(col);
 int W=TT.mw*16,H=TT.mh*16;int span=W>H?W:H;
 for(int y=0;y<TM_S;y++)for(int x=0;x<TM_S;x++){int wx=x*span/TM_S,wy=y*span/TM_S;u16 c;
  if(wx>=W||wy>=H)c=RGB(6,10,20);
  else{int t=ds_tac_cell(wx>>4,wy>>4);if(have)c=c15(col[t]);else{int f=ds_tac_flags(t);c=(f&1)?RGB(120,104,84):(f&4)?RGB(40,90,160):RGB(56,96,52);}
   // darken slightly so blips stand out, and add a faint grid every 64 px
   c=0x8000|((c&0x7BDE)>>1)|((c&0x739C)>>2);if((wx&63)<span/TM_S||(wy&63)<span/TM_S)c=0x8000|((c&0x7BDE)>>1);}
  tmap[y*TM_S+x]=c;}}
static void plot(int x,int y,u16 c){if(x>=0&&x<256&&y>=0&&y<192)ui[y*256+x]=c;}
static void draw_tac(int scr){
 int W=TT.mw*16,H=TT.mh*16;int span=W>H?W:H;
 for(int y=0;y<TM_S;y++)memcpy(&ui[(TM_Y+y)*256+TM_X],&tmap[y*TM_S],TM_S*2);
 frame(TM_X-1,TM_Y-1,TM_S+2,TM_S+2,CY);
 // camera view rectangle
 {int x0=TM_X+TT.camx*TM_S/span,y0=TM_Y+TT.camy*TM_S/span,w=240*TM_S/span,h=160*TM_S/span;
  for(int i=0;i<w;i++){if(x0+i<TM_X+TM_S&&x0+i>=TM_X){if(y0>=TM_Y)plot(x0+i,y0,GR_);if(y0+h<TM_Y+TM_S)plot(x0+i,y0+h,GR_);}}
  for(int j=0;j<=h;j++){if(y0+j<TM_Y+TM_S&&y0+j>=TM_Y){if(x0>=TM_X)plot(x0,y0+j,GR_);if(x0+w<TM_X+TM_S)plot(x0+w,y0+j,GR_);}}}
 for(int pass=0;pass<2;pass++)for(int i=0;i<TT.nb;i++){DsBlip*b=&TB[i];if((b->team==2)!=pass)continue;
  int x=TM_X+b->x*TM_S/span,y=TM_Y+b->y*TM_S/span;if(x<TM_X||y<TM_Y||x>=TM_X+TM_S||y>=TM_Y+TM_S)continue;
  u16 c=b->team==2?WH:b->team==1?RGB(240,70,60):RGB(80,160,255);
  if(b->team==2){int a=b->ang>>8;static const s8 CS[16][2]={{6,0},{6,2},{4,4},{2,6},{0,6},{-2,6},{-4,4},{-6,2},{-6,0},{-6,-2},{-4,-4},{-2,-6},{0,-6},{2,-6},{4,-4},{6,-2}};
   int d=((a+8)>>4)&15;for(int s=0;s<=6;s++)plot(x+CS[d][0]*s/6,y+CS[d][1]*s/6,GD);fill(x-2,y-2,5,5,RGB(0,0,0));fill(x-1,y-1,3,3,WH);}
  else{fill(x-1,y-1,3,3,c);}}
 // right column: status
 int X=148;char s[40];
 text(X,20,TAC_NAME(scr),GD,1);
 int hp=TT.hp<0?0:TT.hp,mx=TT.maxhp>0?TT.maxhp:100;text(X,32,"HP",GR_,1);fill(X+16,32,88,7,RGB(30,30,40));
 fill(X+16,32,hp>=mx?88:hp*88/mx,7,hp*4<mx?RGB(230,60,50):hp*2<mx?RGB(240,190,60):RGB(90,210,90));
 if(!TT.alive){snprintf(s,40,"RESPAWN %d",TT.resp/60+1);text(X,44,s,GD,1);}
 else if(TT.wpn){text(X,44,TT.wpn,WH,1);}
 snprintf(s,40,"KILLS %u  DOWN %u",(unsigned)DSBEAT->kills,(unsigned)DSBEAT->deaths);text(X,56,s,WH,1);
 if(scr!=4){snprintf(s,40,"SCORE %u - %u",(unsigned)DSBEAT->scoreA,(unsigned)DSBEAT->scoreB);text(X,68,s,WH,1);}
 else{snprintf(s,40,"BUNKER %u",(unsigned)DSBEAT->extra);text(X,68,s,WH,1);}
 if(scr==5||scr==4){snprintf(s,40,"GRENADES %d",TT.gren);text(X,80,s,WH,1);}
 else if(TT.fuel>0){snprintf(s,40,"FUEL %d",TT.fuel/60);text(X,80,s,WH,1);}
 {unsigned t=DSBEAT->timer/60;snprintf(s,40,"TIME %u:%02u",t/60,t%60);text(X,92,s,GR_,1);}
 const char*l[4];u16 k[4];tac_buttons(scr,l,k);
 for(int i=0;i<4;i++){int bx,by,bw,bh;tac_brect(i,&bx,&by,&bw,&bh);button(bx,by,bw,bh,l[i],(ds_vkeys&k[i])!=0);}}

static void race_build(void){static const u16 SC[8]={0x8000|(13<<10)|(13<<5)|13,0x8000|(8<<10)|(8<<5)|28,0x8000|(6<<10)|(14<<5)|5,0x8000|(4<<10)|(4<<5)|4,
  0x8000|(28<<10)|(26<<5)|6,0x8000|(4<<10)|(16<<5)|30,0x8000|(22<<10)|(10<<5)|4,0x8000|(6<<10)|(14<<5)|5};
 for(int i=0;i<128*128;i++){int v=RR.surf[i];rmap[i]=SC[v&7];}rmapSrc=RR.surf;}
static void draw_race(void){
 const int X0=8,Y0=20;for(int y=0;y<128;y++)memcpy(&ui[(Y0+y)*256+X0],&rmap[y*128],256);frame(X0-1,Y0-1,130,130,CY);
 for(int i=RR.n-1;i>=0;i--){int x=X0+RR.x[i],y=Y0+RR.y[i];if(x<X0||y<Y0||x>=X0+128||y>=Y0+128)continue;
  if(i==0){fill(x-3,y-3,7,7,RGB(0,0,0));fill(x-2,y-2,5,5,GD);}else{fill(x-2,y-2,4,4,RGB(0,0,0));fill(x-1,y-1,3,3,RGB(240,80,70));}}
 int X=146;char s[40];text(X,20,"GRAND PRIX",GD,1);
 int pos=1;for(int i=0;i<RR.n;i++)if(RR.order[i]==0)pos=i+1;
 snprintf(s,40,"POS %d/%d",pos,RR.n);text(X,32,s,WH,2);
 int lp=RR.lap[0]<1?1:RR.lap[0];if(lp>RR.laps)lp=RR.laps;snprintf(s,40,"LAP %d/%d",lp,RR.laps);text(X,50,s,WH,1);
 for(int i=0;i<RR.n&&i<4;i++){int c=RR.order[i];snprintf(s,40,"%d %.15s",i+1,RR.name[c]?RR.name[c]:"?");text(X,62+i*9,s,c==0?GD:GR_,1);}
 const char*l[4];u16 k[4];tac_buttons(90,l,k);
 for(int i=0;i<4;i++){int bx,by,bw,bh;tac_brect(i,&bx,&by,&bw,&bh);button(bx,by,bw,bh,l[i],(ds_vkeys&k[i])!=0);}}
#include "museum.inc"
static void header(void){fill(0,0,256,15,PANEL);fill(0,15,256,1,CY);text(4,4,"ARSENAL OF ERAS",GD,1);text(100,4,"DS",CY,1);
 const char*s=ds_save_status;if(curPg==PG_TILES){char t[10];snprintf(t,10,"%s",s);text(116,4,t,GR_,1);}else text(252-tw(s,1),4,s,GR_,1);}
static int musBtn(int pg,int scr,int*x,int*y,int*w,int*h,const char**l,int*cat){
 if(pg==PG_TILES){*x=176;*y=0;*w=80;*h=15;*l="MUSEUM";*cat=0;return 1;}
 if(pg==PG_PAD&&(scr==11||scr==18)){*x=80;*y=44;*w=96;*h=20;*l=scr==18?"INSPECT HANGAR":"INSPECT ARMORY";*cat=scr==18?0:2;return 1;}
 return 0;}
static void redraw(int pg,int scr,int key,int sel){curPg=pg;
 fill(0,0,256,192,BG);header();
 if(pg==PG_TITLE){fill(28,40,200,80,PANEL);frame(28,40,200,80,CY);fill(28,40,20,3,GD);ctext(128,62,"TAP TO START",GD,2);ctext(128,96,"OR PRESS START",GR_,1);
  ctext(128,140,"TOP SCREEN: GAME  -  BOTTOM: TOUCH",GR_,1);}
 else if(pg==PG_TILES){const char*a,*b;int n=ds_ui_query(scr,key,-1,&a,&b);
  for(int i=0;i<n;i++){int x,y,w,h;tile_rect(i,&x,&y,&w,&h);ds_ui_query(scr,key,i,&a,&b);int on=i==sel;
   fill(x,y,w,h,on?SEL:PANEL);frame(x,y,w,h,on?GD:CY);fill(x,y,2,h,on?GD:CY);text(x+8,y+5,a,on?GD:WH,2);if(b&&*b)text(x+8,y+20,b,GR_,1);}}
 else if(pg==PG_LIST){const char*a,*b;int n=ds_ui_query(scr,key,-1,&a,&b);
  for(int i=0;i<n;i++){int x,y,w,h;list_rect(i,n,&x,&y,&w,&h);ds_ui_query(scr,key,i,&a,&b);int on=i==sel;
   fill(x,y,w,h,on?SEL:PANEL);frame(x,y,w,h,on?GD:CY);text(x+6,y+h/2-3,a,on?GD:WH,1);}
  int no=ds_ui_query(scr,key,100,&a,&b);int foc=scr==54,os=DSBEAT->phase;
  for(int j=0;j<no;j++){int x,y,w,h;opt_rect(j,&x,&y,&w,&h);ds_ui_query(scr,key,100+j,&a,&b);int on=foc?j==os:j==0;
   fill(x,y,w,h,on?SEL:PANEL);frame(x,y,w,h,on?(foc?GD:CY):RGB(60,80,110));ctext(x+w/2,y+h/2-3,a,on?GD:WH,1);}
  ctext(206,18+no*20+6,foc?"TAP TO START":"TAP A MODE",GR_,1);}
 else if(pg==PG_TAC){if(scr==90)draw_race();else draw_tac(scr);}
 else{draw_pad();char t[48];snprintf(t,48,"SCREEN %u  MODE %u",(unsigned)DSBEAT->screen,(unsigned)DSBEAT->mode);ctext(128,150,t,GR_,1);}
 {int x,y,w,h,c;const char*l;if(musBtn(pg,scr,&x,&y,&w,&h,&l,&c)){fill(x,y,w,h,PANEL);button(x,y,w,h,l,1);}}
 draw_bar();
 DC_FlushRange(ui,sizeof ui);dmaCopyWords(3,ui,(void*)0x06200000,sizeof ui);}
void ui_init(void){videoSetModeSub(MODE_5_2D);bgInitSub(3,BgType_Bmp16,BgSize_B16_256x256,0,0);memset(ui,0,sizeof ui);}
static void tap_item(int pg,int i,int sel,int n){ds_ui_taps++;
 if(pg==PG_TILES){if(i!=sel){if((i&1)!=(sel&1))q_tap(GRIGHT);int dr=(i>>1)-(sel>>1);while(dr>0){q_tap(GDOWN);dr--;}while(dr<0){q_tap(GUP);dr++;}}q_tap(GA);}
 else if(pg==PG_LIST){if(DSBEAT->screen==54)q_tap(GB);int d=i-sel;while(d>0){q_tap(GDOWN);d--;}while(d<0){q_tap(GUP);d++;}q_tap(GA);}}
static void tap_opt(int j){ds_ui_taps++;int foc=DSBEAT->screen==54,os=foc?(int)DSBEAT->phase:0;if(!foc)q_tap(GA);int d=j-os;while(d>0){q_tap(GDOWN);d--;}while(d<0){q_tap(GUP);d++;}q_tap(GA);}
void ui_frame(void){
 uiTick++;
 scanKeys();int held=keysHeld();touchPosition t;int touching=(held&KEY_TOUCH)!=0;if(touching){touchRead(&t);tx=t.px;ty=t.py;}
 int scr=DSBEAT->screen,key=DSBEAT->extra,sel=DSBEAT->menu;
 int tac=scr==90?ds_race(&RR):(scr==4||scr==5||scr==6||scr==7||scr==19)&&ds_tac(scr,&TT,TB,24);
 int pg=padForce?PG_PAD:tac?PG_TAC:scr==1?PG_TITLE:scr==105?PG_TILES:((scr==53||scr==54)&&ds_ui_query(scr,key,-1,&(const char*){0},&(const char*){0})>0)?PG_LIST:PG_PAD;
 u16 vk=0;
 // macro queue
 if(qh!=qt){vk=Q[qh].k;if(--Q[qh].n==0)qh=(qh+1)&63;}
 if(touching&&!lastTouch){ // new tap
  int mx,my,mw,mh,mc;const char*ml;if(musBtn(pg,scr,&mx,&my,&mw,&mh,&ml,&mc)&&tx>=mx&&tx<mx+mw&&ty>=my&&ty<my+mh){ds_ui_taps++;ds_museum(mc);dirty=1;lastTouch=0;ds_vkeys=0;page=-1;return;}
  if(ty>=BAR_Y){int b=tx/64;if(b>3)b=3;if(b==0)q_tap(GB);else if(b==1)q_tap(GSEL);else if(b==2)q_tap(GSTART);else{padForce^=1;dirty=1;}}
  else if(pg==PG_TITLE){q_tap(GSTART);}
  else if(pg==PG_TILES||pg==PG_LIST){int n=10;const char*a,*b;n=ds_ui_query(scr,key,-1,&a,&b);
   for(int i=0;i<n;i++){int x,y,w,h;if(pg==PG_TILES)tile_rect(i,&x,&y,&w,&h);else list_rect(i,n,&x,&y,&w,&h);
    if(tx>=x&&tx<x+w&&ty>=y&&ty<y+h){tap_item(pg,i,sel,n);break;}}
   if(pg==PG_LIST){int no=ds_ui_query(scr,key,100,&a,&b);for(int j=0;j<no;j++){int x,y,w,h;opt_rect(j,&x,&y,&w,&h);if(tx>=x&&tx<x+w&&ty>=y&&ty<y+h){tap_opt(j);break;}}}}}
 if(touching&&pg==PG_PAD&&ty<BAR_Y)vk|=pad_hit(tx,ty);
 if(touching&&pg==PG_TAC&&ty<BAR_Y)vk|=tac_hit(scr,tx,ty);
 if(vk!=ds_vkeys&&(pg==PG_PAD||pg==PG_TAC))dirty=1;
 if(pg==PG_TAC&&scr==90){if(rmapSrc!=RR.surf)race_build();if((uiTick&3)==0)dirty=1;}
 else if(pg==PG_TAC){int key=TT.mw*1000+TT.mh*10+scr;if(key!=tmKey||(++tmTick%240)==0){tmKey=key;tac_build();}if((uiTick&3)==0)dirty=1;}
 ds_vkeys=vk;lastTouch=touching;
 static int lastPh;if(pg!=page||sel!=lastSel||scr!=lastScr||(int)DSBEAT->phase!=lastPh||(uiTick%60)==0)dirty=1;lastPh=DSBEAT->phase;
 if(dirty){redraw(pg,scr,key,sel);dirty=0;page=pg;lastSel=sel;lastScr=scr;}}
