// touchui.c - bottom-screen touch interface (sub engine, 16bpp bitmap).
// Pages: TITLE (tap to start), TILES (the ten main-menu tiles), LIST (a category's modes),
// PAD (virtual GBA pad for everything else).  Taps on menu items are turned into the key
// presses the GBA menu code expects (navigate, then A), so the game logic is unchanged.
#include "platform.h"
#include <string.h>
#include <stdio.h>
extern const unsigned char font_bits[672];
static u16 ui[256*192] __attribute__((aligned(32)));
static int dirty=1,page=-1,lastSel=-1,lastScr=-1,padForce=0,lastTouch=0,tx,ty,uiTick;
u32 ds_ui_taps;
#define RGB(r,g,b) ((u16)(0x8000|((b)>>3)<<10|((g)>>3)<<5|((r)>>3)))
enum{PG_TITLE,PG_TILES,PG_LIST,PG_PAD};
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
static void header(void){fill(0,0,256,15,PANEL);fill(0,15,256,1,CY);text(4,4,"ARSENAL OF ERAS",GD,1);text(100,4,"DS",CY,1);
 const char*s=ds_save_status;text(252-tw(s,1),4,s,GR_,1);}
static void redraw(int pg,int scr,int key,int sel){
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
 else{draw_pad();char t[48];snprintf(t,48,"SCREEN %u  MODE %u",(unsigned)DSBEAT->screen,(unsigned)DSBEAT->mode);ctext(128,150,t,GR_,1);}
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
 int pg=padForce?PG_PAD:scr==1?PG_TITLE:scr==105?PG_TILES:((scr==53||scr==54)&&ds_ui_query(scr,key,-1,&(const char*){0},&(const char*){0})>0)?PG_LIST:PG_PAD;
 u16 vk=0;
 // macro queue
 if(qh!=qt){vk=Q[qh].k;if(--Q[qh].n==0)qh=(qh+1)&63;}
 if(touching&&!lastTouch){ // new tap
  if(ty>=BAR_Y){int b=tx/64;if(b>3)b=3;if(b==0)q_tap(GB);else if(b==1)q_tap(GSEL);else if(b==2)q_tap(GSTART);else{padForce^=1;dirty=1;}}
  else if(pg==PG_TITLE){q_tap(GSTART);}
  else if(pg==PG_TILES||pg==PG_LIST){int n=10;const char*a,*b;n=ds_ui_query(scr,key,-1,&a,&b);
   for(int i=0;i<n;i++){int x,y,w,h;if(pg==PG_TILES)tile_rect(i,&x,&y,&w,&h);else list_rect(i,n,&x,&y,&w,&h);
    if(tx>=x&&tx<x+w&&ty>=y&&ty<y+h){tap_item(pg,i,sel,n);break;}}
   if(pg==PG_LIST){int no=ds_ui_query(scr,key,100,&a,&b);for(int j=0;j<no;j++){int x,y,w,h;opt_rect(j,&x,&y,&w,&h);if(tx>=x&&tx<x+w&&ty>=y&&ty<y+h){tap_opt(j);break;}}}}}
 if(touching&&pg==PG_PAD&&ty<BAR_Y)vk|=pad_hit(tx,ty);
 if(vk!=ds_vkeys&&pg==PG_PAD)dirty=1;
 ds_vkeys=vk;lastTouch=touching;
 static int lastPh;if(pg!=page||sel!=lastSel||scr!=lastScr||(int)DSBEAT->phase!=lastPh||(uiTick%60)==0)dirty=1;lastPh=DSBEAT->phase;
 if(dirty){redraw(pg,scr,key,sel);dirty=0;page=pg;lastSel=sel;lastScr=scr;}}
