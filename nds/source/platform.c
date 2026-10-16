// platform.c - Nintendo DS platform layer for Arsenal of Eras (BlocksDS / libnds).
// Runs the shared GBA game code (gen/game.c) on the DS: pushes the shadowed GBA display
// registers/OAM to the main engine (top screen, 240x160 picture centred at 8,16), converts
// GBA bitmap modes, translates the GBA PSG sound registers to DS PSG/PCM channels, keeps the
// GBA SRAM image in RAM and persists it to the DS save chip (EEPROM), pages big assets in from
// NitroFS, and drives the bottom screen touch UI (see touchui.c).
#include <nds.h>
#include <filesystem.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"

u16 gba_io[0x200] __attribute__((aligned(4)));
u16 gba_oam[512] __attribute__((aligned(4)));
u16 gba_bmp[0x14000/2] __attribute__((aligned(32)));
u8  gba_sram[0x10000] __attribute__((aligned(4)));
u32 gba_beat_area[1+64]={0x5EA7A0E5u};
u32 gba_dummy[4];
u16 ds_vkeys;
volatile u32 ds_frame;
int ds_last_screen;
// harness debug block (nds/tools/dsharness scans RAM for the two magic words)
volatile u32 ds_dbg[12]={0x5EA7DB60u,0xDB60DB60u,(u32)&gba_beat_area[1],(u32)gba_sram,(u32)gba_io,(u32)gba_bmp,(u32)&ds_frame,0};

void mus_isr(void);
void isr_stub(void){}

// ------------------------------------------------------------------ assets (NitroFS, LRU)
extern const unsigned int ast_index[][2];
extern const char*const ast_names[];
const void* ast_ptr[AST_N];
static u32 ast_used[AST_N];
static FILE* ast_f;
u32 ds_ast_bytes, ds_ast_loads;
const void* ast_load(int id){
 if(ast_ptr[id])return ast_ptr[id];
 u32 off=ast_index[id][0],sz=ast_index[id][1];
 void*p=malloc(sz);
 while(!p){ // evict the least recently loaded asset that was not loaded this frame
  int best=-1;u32 bu=0xFFFFFFFFu;
  for(int i=0;i<AST_N;i++)if(ast_ptr[i]&&i!=id&&ast_used[i]<bu&&ast_used[i]+1<ds_frame){bu=ast_used[i];best=i;}
  if(best<0)break;
  free((void*)ast_ptr[best]);ast_ptr[best]=0;ds_ast_bytes-=ast_index[best][1];
  p=malloc(sz);}
 if(!p)ds_fatal("OUT OF MEMORY LOADING ASSET",ast_names[id]);
 fseek(ast_f,off,SEEK_SET);
 if(fread(p,1,sz,ast_f)!=sz)ds_fatal("NITROFS READ FAILED",ast_names[id]);
 DC_FlushRange(p,sz);
 ast_ptr[id]=p;ast_used[id]=ds_frame;ds_ast_bytes+=sz;ds_ast_loads++;
 return p;}

// ------------------------------------------------------------------ fatal error screen
void ds_fatal(const char*a,const char*b){
 consoleDemoInit();printf("\x1b[2J ARSENAL OF ERAS DS\n\n FATAL: %s\n %s\n",a,b?b:"");
 for(;;)swiWaitForVBlank();}

// ------------------------------------------------------------------ display push
#define IO(o) gba_io[(o)>>1]
#define IO32(o) (*(u32*)&gba_io[(o)>>1])
static int obj_hidden_y(int y,int h){return y>=160&&y+h<=256;}
static void push_oam(void){
 static const u8 SW[4][4]={{8,16,32,64},{16,32,32,64},{8,8,16,32},{0,0,0,0}};
 static const u8 SH[4][4]={{8,16,32,64},{8,8,16,32},{16,32,32,64},{0,0,0,0}};
 volatile u16*o=(volatile u16*)0x07000000;
 for(int i=0;i<128;i++){u16 a0=gba_oam[i*4],a1=gba_oam[i*4+1],a2=gba_oam[i*4+2];
  o[i*4+3]=gba_oam[i*4+3];
  if((a0&0x300)==0x200){o[i*4]=a0;o[i*4+1]=a1;o[i*4+2]=a2;continue;}
  int sh=a0>>14,sz=a1>>14,w=SW[sh][sz],h=SH[sh][sz];if((a0&0x300)==0x300){w*=2;h*=2;}
  int y=a0&255,x=a1&511;
  if(obj_hidden_y(y,h)||(x>=240&&x+w<=512)){o[i*4]=(a0&~0x300)|0x200;o[i*4+1]=a1;o[i*4+2]=a2;continue;}
  o[i*4]=(a0&0xFF00)|((y+16)&255);o[i*4+1]=(a1&0xFE00)|((x+8)&511);o[i*4+2]=a2;}}
static void push_display(void){
 u16 dc=IO(0);int mode=dc&7;int bmp=mode>=3;
 u32 ds=0x10000u|(dc&0xFF80)|((dc&0x40)?0x10:0);
 int dm=mode==0?0:(mode<=2?2:5);ds|=dm;
 // BG control + scroll
 for(int n=0;n<4;n++){u16 c=IO(8+2*n);
  if(bmp&&n==2)c=(mode==4)?0x4080:0x4084;
  (&REG_BG0CNT)[n]=c;
  (&REG_BG0HOFS)[n*2]=(IO(0x10+4*n)-8)&0x1FF;(&REG_BG0VOFS)[n*2]=(IO(0x12+4*n)-16)&0x1FF;}
 // affine BG2/BG3 (also used for the bitmap modes)
 for(int n=0;n<2;n++){int b=0x20+n*0x10;s16 pa=(s16)IO(b),pb=(s16)IO(b+2),pc=(s16)IO(b+4),pd=(s16)IO(b+6);
  s32 x=(s32)(IO32(b+8)<<4)>>4,y=(s32)(IO32(b+12)<<4)>>4;
  if(bmp&&n==0){x=0;y=0;pa=256;pb=0;pc=0;pd=256;} // bitmap is rewritten at its own origin
  volatile u16*r=(volatile u16*)(0x04000020+n*0x10);
  r[0]=pa;r[1]=pb;r[2]=pc;r[3]=pd;
  *(volatile s32*)(r+4)=x-8*pa-16*pb;*(volatile s32*)(r+6)=y-8*pc-16*pd;}
 // HBlank DMA (GBA DMA0, used by the Grand Prix per-scanline affine floor): the GBA table is copied
 // into a 193-line DS table (centred 16 lines down, x origin shifted 8 px) and replayed by DS DMA0.
 {u32 dc0=IO32(0xB8);DMA_CR(0)=0;
  if((dc0&0x80000000u)&&((dc0>>28)&3)==2&&IO32(0xB4)==0x04000020u&&(dc0&0xFFFF)==4){
   static s32 hd[194][4] __attribute__((aligned(32)));
   const s32*T=(const s32*)IO32(0xB0)-4; // the GBA code points the DMA at line 1
   for(int l=0;l<194;l++){int g=l-16;if(g<0)g=0;if(g>159)g=159;const s32*e=T+4*g;
    s16 pa=(s16)e[0],pc=(s16)e[1];hd[l][0]=e[0];hd[l][1]=e[1];hd[l][2]=e[2]-8*pa;hd[l][3]=e[3]-8*pc;}
   DC_FlushRange(hd,sizeof hd);
   volatile u32*r=(volatile u32*)0x04000020;r[0]=hd[0][0];r[1]=hd[0][1];r[2]=hd[0][2];r[3]=hd[0][3];
   DMA_SRC(0)=(u32)&hd[1];DMA_DEST(0)=0x04000020;
   DMA_CR(0)=DMA_ENABLE|DMA_REPEAT|DMA_START_HBL|DMA_32_BIT|DMA_DST_RESET|4;}}
 // windows: game windows are offset into the centred frame; a free window masks the border
 u16 w0h=IO(0x40),w1h=IO(0x42),w0v=IO(0x44),w1v=IO(0x46);
 #define WH(v) ((u16)((((v)>>8)+8>255?255:((v)>>8)+8)<<8|(((v)&255)+8>255?255:((v)&255)+8)))
 #define WV(v) ((u16)((((v)>>8)+16>191?191:((v)>>8)+16)<<8|(((v)&255)+16>191?191:((v)&255)+16)))
 u16 winin=IO(0x48),winout=IO(0x4A);
 if(!(dc&0xE000)){REG_WIN0H=(8<<8)|248;REG_WIN0V=(16<<8)|176;REG_WININ=0x3F;REG_WINOUT=0;ds|=1<<13;}
 else if((dc&0xE000)==0x2000){REG_WIN0H=WH(w0h);REG_WIN0V=WV(w0v);REG_WIN1H=(8<<8)|248;REG_WIN1V=(16<<8)|176;
  REG_WININ=(winin&0xFF)|((winout&0xFF)<<8);REG_WINOUT=0;ds|=1<<14;}
 else{REG_WIN0H=WH(w0h);REG_WIN0V=WV(w0v);REG_WIN1H=WH(w1h);REG_WIN1V=WV(w1v);REG_WININ=winin;REG_WINOUT=winout;}
 REG_MOSAIC=IO(0x4C);REG_BLDCNT=IO(0x50);REG_BLDALPHA=IO(0x52);REG_BLDY=IO(0x54);
 REG_DISPCNT=ds;
 if(bmp){ // convert the 240-stride GBA bitmap into the DS 256-stride bitmap
  if(mode==3||mode==5){u32*d=(u32*)0x06000000;const u32*s=(const u32*)gba_bmp;
   for(int y=0;y<160;y++){for(int i=0;i<120;i++)d[i]=s[i]|0x80008000u;d+=128;s+=120;}}
  else{const u8*s=(const u8*)gba_bmp+((dc&0x10)?0xA000:0);u32*d=(u32*)0x06000000;
   for(int y=0;y<160;y++){memcpy(d,s,240);d+=64;s+=240;}}}
 push_oam();}

// ------------------------------------------------------------------ vblank
static void vbl_isr(void){ds_frame++;IO(0x202)|=1;mus_isr();IO(0x202)=0;}
void gba_vsync(void){
 swiWaitForVBlank();
 push_display();
 snd_update();
 ui_frame();
 save_tick();
 ds_dbg[7]=ds_dbg[0]; // keep the block referenced
 {extern u32 ds_save_writes;extern int ds_save_dbg_type;extern u32 ds_save_verified;ds_dbg[8]=ds_save_kind;ds_dbg[9]=ds_save_dbg_type;ds_dbg[10]=ds_save_verified;ds_dbg[11]=ds_save_writes;}
}
u16 gba_keyinput(void){return (REG_KEYINPUT&0x3FF)&~ds_vkeys;}

int main(int argc,char**argv){
 defaultExceptionHandler();
 for(int i=0;i<3;i++)swiWaitForVBlank();
 powerOn(POWER_ALL_2D);lcdMainOnTop();
 vramSetBankA(VRAM_A_MAIN_BG_0x06000000);vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
 vramSetBankC(VRAM_C_SUB_BG_0x06200000);
 videoSetMode(MODE_0_2D);
 IO(0x20)=IO(0x26)=IO(0x30)=IO(0x36)=256; // GBA reset state of the affine matrices
 memset((void*)0x06000000,0,128*1024);memset((void*)0x06400000,0,128*1024);
 for(int i=0;i<128;i++)gba_oam[i*4]=0x200;
 ui_init();
 soundEnable();
 if(!nitroFSInit(NULL))ds_fatal("NITROFS INIT FAILED","(the .nds must be launched from its own file)");
 ast_f=fopen("nitro:/assets.bin","rb");if(!ast_f)ds_fatal("NO nitro:/assets.bin",0);
 setvbuf(ast_f,NULL,_IONBF,0);
 save_init();
 irqSet(IRQ_VBLANK,vbl_isr);irqEnable(IRQ_VBLANK);
 int gba_main(void);
 gba_main();
 for(;;)swiWaitForVBlank();}
