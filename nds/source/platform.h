#pragma once
#include <nds.h>
#include "assets_ds.h"

extern u16 gba_io[0x200];
extern u16 gba_oam[512];
extern u16 gba_bmp[0x14000/2];
extern u8 gba_sram[0x10000];
extern u32 gba_beat_area[1+64];
extern u16 ds_vkeys;
extern volatile u32 ds_frame;
extern u32 ds_ast_bytes, ds_ast_loads;
void ds_fatal(const char*a,const char*b) __attribute__((noreturn));
void snd_update(void);
void ui_init(void);
void ui_frame(void);
void save_init(void);
void save_tick(void);
extern const char* ds_save_status;
extern int ds_save_kind;      // 0 none, 1 EEPROM, 2 FAT file
// hooks implemented inside the game translation unit (ds_hooks.inc)
int ds_ui_query(int scr,int key,int idx,const char**l1,const char**l2);
typedef struct{u32 magic,frame,screen,mode,era,hp,scoreA,scoreB,timer,phase,kills,deaths,menu,x,y,extra;}DsBeat;
#define DSBEAT ((volatile DsBeat*)&gba_beat_area[1])
