// museum.inc - DS-only Museum / INSPECT browser (Hangar, Motor Pool, Armory). Included by touchui.c.
// Runs modally from the touch UI: the GBA game is paused inside its vsync, the top screen shows a 16bpp
// bitmap from VRAM bank D (main BG 0x06020000) and the bottom screen shows the touch pages. Screens are
// pre-rendered by art/mkinspect.py into nitro:/inspect.bin (palette + RLE per 256x192 image).
#include <stdlib.h>
typedef struct{char name[24];u8 cat,era,ntabs,pad;char tab[3][12];u32 off[4],len[4];}MEnt;
static MEnt*ME;static int MN=-1;static FILE*MF;
static void mus_open(void){if(MN>=0)return;MN=0;MF=fopen("nitro:/inspect.bin","rb");if(!MF)return;
 u32 h[2];if(fread(h,4,2,MF)!=2||h[0]!=0x4D454F41u||h[1]>200)return;
 ME=malloc(sizeof(MEnt)*h[1]);if(!ME)return;if(fread(ME,sizeof(MEnt),h[1],MF)!=h[1])return;MN=h[1];}
static int mus_decode(int e,int img,volatile u16*dst){if(e<0||e>=MN)return 0;u32 L=ME[e].len[img];if(L<512||L>200000)return 0;
 u8*b=malloc(L);if(!b)return 0;fseek(MF,ME[e].off[img],SEEK_SET);if(fread(b,1,L,MF)!=L){free(b);return 0;}
 const u16*pal=(const u16*)b;const u8*in=b+512;int n=L-512,i=0,o=0;
 while(i<n&&o<256*192){u8 c=in[i++];if(c&0x80){int r=(c&0x7F)+3;u16 v=pal[in[i++]];while(r--&&o<256*192)dst[o++]=v;}
  else{int l=c+1;while(l--&&o<256*192)dst[o++]=pal[in[i++]];}}
 free(b);return 1;}
static int musCat,musSel[3],musView,musTab,musEnt;
static int mus_list(int cat,int*ids){int n=0;for(int i=0;i<MN;i++)if(ME[i].cat==cat)ids[n++]=i;return n;}
static const char*const MCAT[3]={"HANGAR","MOTOR POOL","ARMORY"};
static void mcat_rect(int i,int*x,int*y,int*w,int*h){*x=2+i*84;*y=18;*w=82;*h=16;}
static void mitem_rect(int i,int*x,int*y,int*w,int*h){*x=3+(i&1)*126;*y=37+(i>>1)*15;*w=124;*h=14;}
static const char*const MBAR[4]={"BACK (B)","< PREV","NEXT >","INSPECT (A)"};
static void mus_draw_list(void){int ids[64];int n=mus_list(musCat,ids);
 fill(0,0,256,192,BG);fill(0,0,256,15,PANEL);fill(0,15,256,1,CY);text(4,4,"MUSEUM",GD,1);text(46,4,"- TOUCH OR D-PAD, A: INSPECT",GR_,1);
 for(int i=0;i<3;i++){int x,y,w,h;mcat_rect(i,&x,&y,&w,&h);button(x,y,w,h,MCAT[i],i==musCat);}
 static const char*const ER[3]={"WW2","CW","MOD"};
 for(int i=0;i<n&&i<16;i++){int x,y,w,h;mitem_rect(i,&x,&y,&w,&h);int on=i==musSel[musCat];
  fill(x,y,w,h,on?SEL:PANEL);frame(x,y,w,h,on?GD:RGB(50,80,120));char t[26];snprintf(t,26,"%.17s",ME[ids[i]].name);text(x+4,y+4,t,on?GD:WH,1);
  const char*e=ER[ME[ids[i]].era%3];text(x+w-4-tw(e,1),y+4,e,RGB(110,200,120),1);}
 for(int i=0;i<4;i++)button(2+i*63,BAR_Y,61,26,MBAR[i],i==3);
 DC_FlushRange(ui,sizeof ui);dmaCopyWords(3,ui,(void*)0x06200000,sizeof ui);}
static void mus_top(int e){if(!mus_decode(e,0,(volatile u16*)0x06020000)){volatile u16*d=(volatile u16*)0x06020000;for(int i=0;i<256*192;i++)d[i]=0x8000;}}
static void mus_bottom(int e,int tab){if(!mus_decode(e,1+tab,ui)){fill(0,0,256,192,BG);ctext(128,90,"INSPECT DATA MISSING",GD,1);}
 DC_FlushRange(ui,sizeof ui);dmaCopyWords(3,ui,(void*)0x06200000,sizeof ui);}
static void mus_video(void){
 DMA_CR(0)=0;REG_BLDCNT=0;REG_DISPCNT=MODE_5_2D|DISPLAY_BG3_ACTIVE;
 REG_BG3CNT=BG_BMP16_256x256|BG_BMP_BASE(8);REG_BG3PA=256;REG_BG3PB=0;REG_BG3PC=0;REG_BG3PD=256;REG_BG3X=0;REG_BG3Y=0;}
// returns when the player backs out of the museum
void ds_museum(int cat){
 mus_open();if(MN<=0)return;
 u32 bs=DSBEAT->screen,bm=DSBEAT->menu,bp=DSBEAT->phase,bx=DSBEAT->extra;
 musCat=cat;int inspect=0,redraw=1,lastT=1;int ids[64];
 for(;;){
  int n=mus_list(musCat,ids);if(musSel[musCat]>=n)musSel[musCat]=n?n-1:0;int e=n?ids[musSel[musCat]]:-1;
  if(redraw){mus_video();if(e>=0)mus_top(e);if(inspect)mus_bottom(e,musTab);else mus_draw_list();redraw=0;}
  DSBEAT->screen=inspect?201:200;DSBEAT->menu=e;DSBEAT->phase=musTab;DSBEAT->extra=musCat;DSBEAT->frame++;
  swiWaitForVBlank();snd_update();save_tick();mus_video();
  scanKeys();int kd=keysDown(),kh=keysHeld();touchPosition t;int touch=(kh&KEY_TOUCH)!=0,tap=0;int px=0,py=0;
  if(touch){touchRead(&t);px=t.px;py=t.py;tap=!lastT;}lastT=touch;
  // physical keys
  if(!inspect){
   if(kd&KEY_B)break;
   if(kd&(KEY_A|KEY_START)&&e>=0){inspect=1;musTab=0;redraw=1;}
   if(kd&KEY_RIGHT&&musSel[musCat]+1<n){musSel[musCat]++;redraw=1;}
   if(kd&KEY_LEFT&&musSel[musCat]>0){musSel[musCat]--;redraw=1;}
   if(kd&KEY_DOWN&&musSel[musCat]+2<n){musSel[musCat]+=2;redraw=1;}
   if(kd&KEY_UP&&musSel[musCat]>=2){musSel[musCat]-=2;redraw=1;}
   if(kd&KEY_R){musCat=(musCat+1)%3;redraw=1;}if(kd&KEY_L){musCat=(musCat+2)%3;redraw=1;}
   if(tap){
    if(py>=BAR_Y){int b=px/64;if(b>3)b=3;if(b==0)break;if(b==1&&musSel[musCat]>0){musSel[musCat]--;redraw=1;}
     if(b==2&&musSel[musCat]+1<n){musSel[musCat]++;redraw=1;}if(b==3&&e>=0){inspect=1;musTab=0;redraw=1;}}
    else{for(int i=0;i<3;i++){int x,y,w,h;mcat_rect(i,&x,&y,&w,&h);if(px>=x&&px<x+w&&py>=y&&py<y+h){musCat=i;redraw=1;}}
     for(int i=0;i<n&&i<16;i++){int x,y,w,h;mitem_rect(i,&x,&y,&w,&h);if(px>=x&&px<x+w&&py>=y&&py<y+h){
      if(i==musSel[musCat]){inspect=1;musTab=0;}else musSel[musCat]=i;redraw=1;}}}}}
  else{int nt=ME[e].ntabs?ME[e].ntabs:3;
   if(kd&KEY_B){inspect=0;redraw=1;}
   if(kd&KEY_RIGHT){musTab=(musTab+1)%nt;redraw=1;}if(kd&KEY_LEFT){musTab=(musTab+nt-1)%nt;redraw=1;}
   if(kd&KEY_R&&musSel[musCat]+1<n){musSel[musCat]++;musTab=0;redraw=1;}
   if(kd&KEY_L&&musSel[musCat]>0){musSel[musCat]--;musTab=0;redraw=1;}
   if(tap){int w=(256-4-(nt-1)*3)/nt;
    if(py<18){for(int i=0;i<nt;i++){int x=2+i*(w+3);if(px>=x&&px<=x+w){musTab=i;redraw=1;}}}
    else if(py>=178){if(px<40){inspect=0;redraw=1;}else if(px<150){if(musSel[musCat]>0){musSel[musCat]--;musTab=0;redraw=1;}}else{if(musSel[musCat]+1<n){musSel[musCat]++;musTab=0;redraw=1;}}}}}
 }
 // wait for every key and the stylus to be released so nothing leaks into the paused game
 for(int i=0;i<120;i++){swiWaitForVBlank();snd_update();scanKeys();if(!(keysHeld()&(KEY_A|KEY_B|KEY_X|KEY_Y|KEY_L|KEY_R|KEY_START|KEY_SELECT|KEY_TOUCH|0xF0)))break;}
 DSBEAT->screen=bs;DSBEAT->menu=bm;DSBEAT->phase=bp;DSBEAT->extra=bx;}
