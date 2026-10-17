// gba_ds.h - GBA->DS compatibility layer for the shared Arsenal of Eras source (src/).
// The DS build compiles the GBA game code unchanged in spirit: tools/port.py copies src/ into
// nds/gen/, rewrites the few hard-coded hardware literals to the macros below, and includes this
// header instead of src/gba.h.  Display/sound/DMA/IRQ registers are shadowed in RAM and pushed to
// the real DS hardware once per frame by platform.c (centering the 240x160 GBA picture on the
// 256x192 top screen); palettes, BG VRAM (bank A @0x06000000) and the key register are used
// directly because they live at the same addresses on the DS main engine.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
typedef signed char s8; typedef signed short s16; typedef signed int s32;

extern u16 gba_io[0x200];            // shadow of GBA I/O 0x04000000-0x040003FF
extern u16 gba_oam[512];             // shadow OAM (GBA coordinates)
extern u16 gba_bmp[0x14000/2];       // GBA mode 3/4/5 bitmap VRAM 0x06000000-0x06013FFF (240-pixel stride)
extern u8  gba_sram[0x10000];        // GBA SRAM image (same layout as the GBA .sav)
extern u32 gba_beat_area[1+64];      // [0]=0x5EA7A0E5 tag for the harness, [1..64] = GBA 0x0203FF00..FF
extern u32 gba_dummy[4];
extern u16 ds_vkeys;                 // virtual (touch) keys, active high, GBA bit layout
u16 gba_keyinput(void);
void gba_vsync(void);

#define GBA_BEATBASE ((u32)&gba_beat_area[1])
#define GBA_SRAM_ADDR ((u32)gba_sram)
#define GBA_BMP_ADDR ((u32)gba_bmp)
#define GBA_OAM_ADDR ((u32)gba_oam)
#define GBA_DUMMY_ADDR ((u32)gba_dummy)

// I/O mapping: everything goes to the shadow except VCOUNT (real), timers (real, same layout) and
// KEYINPUT (real keys merged with the touch-screen virtual buttons).
static inline volatile void* gba_ioaddr(u32 a){
 if(a>=0x4000000u&&a<0x4000400u){u32 o=a-0x4000000u;
  if(o==6||(o>=0x100&&o<0x110))return (volatile void*)a;
  return (volatile void*)((u8*)gba_io+o);}
 return (volatile void*)a;}
#define GBA_IO(a) ((u32)gba_ioaddr(a))
#define REG16(a) (*(volatile u16*)gba_ioaddr(a))
#define REG32(a) (*(volatile u32*)gba_ioaddr(a))
#define REG_DISPCNT REG16(0x4000000)
#define REG_VCOUNT  (*(volatile u16*)0x4000006)
#define REG_BGCNT(n) REG16(0x4000008+2*(n))
#define REG_BGHOFS(n) REG16(0x4000010+4*(n))
#define REG_BGVOFS(n) REG16(0x4000012+4*(n))
#define REG_BLDCNT REG16(0x4000050)
#define REG_BLDALPHA REG16(0x4000052)
#define REG_BLDY REG16(0x4000054)
#define REG_KEYINPUT (gba_keyinput())
#define REG_SOUNDCNT_L REG16(0x4000080)
#define REG_SOUNDCNT_H REG16(0x4000082)
#define REG_SOUNDCNT_X REG16(0x4000084)
#define REG_SND1SWEEP REG16(0x4000060)
#define REG_SND1CNT REG16(0x4000062)
#define REG_SND1FREQ REG16(0x4000064)
#define REG_SND2CNT REG16(0x4000068)
#define REG_SND2FREQ REG16(0x400006C)
#define REG_SND4CNT REG16(0x4000078)
#define REG_SND4FREQ REG16(0x400007C)
#define REG_DMA3SAD REG32(0x40000D4)
#define REG_DMA3DAD REG32(0x40000D8)
#define REG_DMA3CNT REG32(0x40000DC)
#define PAL_BG ((volatile u16*)0x05000000)
#define PAL_OBJ ((volatile u16*)0x05000200)
#define VRAM ((volatile u16*)0x06000000)
#define CHARBLOCK(n) ((volatile u32*)(0x06000000+(n)*0x4000))
#define SCREENBLOCK(n) ((volatile u16*)(0x06000000+(n)*0x800))
#define OBJ_TILES ((volatile u32*)0x06400000)
#define OAM ((volatile u16*)gba_oam)
enum{K_A=1,K_B=2,K_SEL=4,K_START=8,K_RIGHT=16,K_LEFT=32,K_UP=64,K_DOWN=128,K_R=256,K_L=512,K_DSX=1024,K_DSY=2048}; // K_DSX/K_DSY: DS X/Y (controls.c)
extern int ds_ctl_preset;void ds_ctl_set(int v);const char*ds_ctl_name(void);int ds_ctl_testmode(void);extern int ds_arc_on;extern const void*volatile ds_fb_src;void ds_fb_end(const void*fb);
extern int ds_ter_on;int ds_terrain(int era);unsigned short ds_ter_entry(int t,int cx,int cy,unsigned short orig);
void ds_foam_pal(void);
