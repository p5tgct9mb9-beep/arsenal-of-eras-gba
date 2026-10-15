// Arsenal of Eras - GBA demake (personal build). Bare-metal C, no external libs.
#include "gba.h"
#include "assets.h"
#include "core.inc"
#include "loadout.inc"
#include "mp.inc"
#include "camp.inc"

static void title_screen(void){
 m3_title(0);m3_present();BEAT->screen=SCR_TITLE;music_start(title_song,SONG_LEN,9);
 for(int i=0;;i++){
  next_frame();
  if((i&31)==0||(i&31)==20){int on=(i&31)==0;
   for(int y=134;y<146;y++)for(int x=60;x<180;x++)M3[y*240+x]=title_bmp[y*240+x];
   if(on)m3_ctext(136,"PRESS START",C_WHITE,1);m3_present();}
  if(i==1){m3_ctext(88,"A GBA FIELD EDITION - BY LT VASQUEZ",C_GRAY,1);m3_present();}
  if(pressed&(K_START|K_A)){sfx_ok();return;}}}
static int main_menu(int sel){
 static const char*const items[5]={"CAMPAIGN","6V6 INFANTRY","6V6 TANKS","6V6 DOGFIGHT","LOADOUT"};
 static const char*const desc[5]={"MISSION: DOG GREEN - OMAHA 1944","TEAM DEATHMATCH VS BOTS","ARMOR BATTLE - HULL + TURRET","AIR SUPERIORITY - GUNS + MISSILES","CUSTOMIZE WEAPONS, PERKS, VEHICLES"};
 int redraw=1;
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  if(redraw){m3_title(0);m3_fill(30,82,180,76,RGB(14,18,36));m3_box(30,82,180,76,2);
   for(int k=0;k<5;k++)m3_text(56,86+k*11,cat(k==sel?"> ":"  ",items[k]),k==sel?C_YEL:C_WHITE,1);
   m3_ctext(145,desc[sel],C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+4)%5;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%5;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();return sel;}
  if(pressed&K_B)return -1;}}
static int era_select(int mode,int *era){
 static const char*const MN[4]={"","6V6 INFANTRY","6V6 TANKS","6V6 DOGFIGHT"};
 static const char*const ED[5]={"NORMANDY HEDGEROWS 1944","FROZEN FRONT 1968","DESERT CITY 2026","NEON MEGACITY 2089","LUNAR BASE 2140"};
 static const u16 EC[5]={RGB(150,170,100),RGB(200,214,232),RGB(220,190,130),RGB(80,240,255),RGB(200,170,255)};
 int e=*era,redraw=1;
 for(;;){BEAT->screen=SCR_ERA;BEAT->era=e;BEAT->mode=mode;
  if(redraw){m3_title(1);m3_fill(12,8,216,146,RGB(14,18,36));m3_box(12,8,216,146,2);m3_ctext(14,MN[mode],C_YEL,2);m3_ctext(32,"SELECT ERA",C_GRAY,1);
   for(int k=0;k<5;k++)m3_text(30,46+k*12,cat(k==e?"> ":"  ",ERA_NAME[k]),k==e?EC[k]:C_WHITE,1);
   m3_text(130,46+e*12,TEAMN[e][0],C_BLUE,1);m3_text(172,46+e*12,TEAMN[e][1],C_RED,1);
   m3_ctext(110,ED[e],EC[e],1);
   if(mode==MM_INF){m3_ctext(122,cat("PRIMARY: ",PRIM[e][LO[e].prim].n),C_WHITE,1);m3_ctext(132,cat("SIDEARM: ",SEC[e][LO[e].sec].n),C_WHITE,1);}
   if(mode==MM_TANK){m3_ctext(122,cat("YOUR TANK: ",TANKV[e][tankSel[e]].n),C_WHITE,1);m3_ctext(132,cat("ENEMY: ",TANKV[e][1-tankSel[e]].n),C_WHITE,1);}
   if(mode==MM_AIR){m3_ctext(122,cat("YOUR JET: ",AIRV[e][airSel[e]].n),C_WHITE,1);m3_ctext(132,cat("ENEMY: ",AIRV[e][(airSel[e]+1)%NAIR[e]].n),C_WHITE,1);}
   m3_ctext(144,"A: LOADOUT   B: BACK",C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if((pressed&(K_UP|K_LEFT))&&e>0){e--;redraw=1;sfx_menu();}
  if((pressed&(K_DOWN|K_RIGHT))&&e<4){e++;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();*era=e;return 1;}
  if(pressed&K_B){sfx_menu();return 0;}}}
static void loading(const char*s){m3_title(1);m3_ctext(76,s,C_YEL,1);m3_present();next_frame();}
int main(void){
 REG_WAITCNT=0x4317;snd_init();save_load();beat_common(SCR_TITLE);BEAT->frame=0;
 for(;;){
  title_screen();
  int sel=0,era=0;
  for(;;){sel=main_menu(sel);if(sel<0)break;music_stop();
   if(sel==4){loadout_menu(0,era);}
   else if(sel==0){if(loadout_menu(2,0)){loading("DEPLOYING TO DOG GREEN...");campaign();}}
   else if(era_select(sel,&era)){if(loadout_menu(sel==1?1:sel==2?3:4,era)){loading("DEPLOYING...");mp_run(sel,era);}}
   music_start(title_song,SONG_LEN,9);}
 }
}
