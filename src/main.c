// Arsenal of Eras - GBA demake (personal build). Bare-metal C, no external libs.
#include "gba.h"
#include "assets.h"
#include "core.inc"
#include "loadout.inc"
#include "mp.inc"
#include "camp.inc"

static void title_screen(void){
 m3_title(0);m3_present();BEAT->screen=SCR_TITLE;bgm(0,0);
 for(int i=0;;i++){
  next_frame();
  if((i&31)==0||(i&31)==20){int on=(i&31)==0;
   for(int y=134;y<146;y++)for(int x=60;x<180;x++)M3[y*240+x]=title_bmp[y*240+x];
   if(on)m3_ctext(136,"PRESS START",C_WHITE,1);m3_present();}
  if(i==1){m3_ctext(88,"A GBA FIELD EDITION - BY LT VASQUEZ",C_GRAY,1);m3_present();}
  if(pressed&(K_START|K_A)){sfx_ok();return;}}}

static void set_preview(void){if(SET.track==NTRK)music_stop();else if(SET.track==0)bgm(1,0);else music_play(SET.track-1);}
static void settings_menu(void){
 static const char*const LAB[8]={"MUSIC TRACK","MUSIC VOLUME","SFX VOLUME","SCREEN SHAKE","DIFFICULTY","CONTROL HINTS","RUMBLE",""};
 static const char*const DN[3]={"EASY","NORMAL","HARD"};
 int sel=0,redraw=2;
 for(;;){BEAT->screen=SCR_SETTINGS;BEAT->menu=sel;BEAT->extra=SET.track|(SET.musVol<<8)|(SET.sfxVol<<12)|(SET.diff<<16)|(SET.shake<<18)|(SET.hints<<19);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;
   m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);m3_ctext(9,"SETTINGS",C_YEL,2);
   for(int k=0;k<8;k++){int y=34+k*12+(k==7?4:0);u16 c=k==sel?C_YEL:(k==6?C_GRAY:C_WHITE);
    if(k==7){m3_text(14,y,k==sel?"> SAVE & BACK":"  SAVE & BACK",c,1);continue;}
    m3_text(14,y,LAB[k],k==sel?C_YEL:C_GRAY,1);const char*v="";
    switch(k){case 0:v=SET.track==0?"AUTO (BY ERA)":SET.track==NTRK?"OFF":TRK_NAME[SET.track-1];break;
     case 1:case 2:{int n=k==1?SET.musVol:SET.sfxVol;m3_fill(100,y,102,7,RGB(40,44,70));if(n)m3_fill(100,y,n*102/10,7,k==1?C_BLUE:C_GREEN);v=itoa(n);m3_text(208,y,v,c,1);v=0;break;}
     case 3:v=SET.shake?"ON":"OFF";break;case 4:v=DN[SET.diff];break;case 5:v=SET.hints?"ON":"OFF";break;case 6:v="N/A (NO RUMBLE HW)";break;}
    if(v){m3_text(100,y,v,c,1);if(k==sel&&k!=6){m3_text(94,y,"<",C_YEL,1);m3_text(100+slen(v)*6+1,y,">",C_YEL,1);}}}
   const char*d=sel==0?(SET.track==0?"ERA-APPROPRIATE TRACK FOR EACH MODE":SET.track==NTRK?"NO MUSIC":TRK_DESC[SET.track-1]):sel==1?"PSG MUSIC LEVEL (0 = MUTE)":sel==2?"GUNFIRE / EXPLOSION / UI SOUND LEVEL":
    sel==3?"SHAKE ON EXPLOSIONS, HITS, TANK FIRE":sel==4?"ENEMY DAMAGE TO YOU AND FIRE RATE":sel==5?"SHOW BUTTON HINTS AT MATCH START":sel==6?"GBA EMULATORS: NO RUMBLE MOTOR":"SAVES TO SRAM";
   m3_ctext(140,d,C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+7)%8;if(sel==6)sel=5;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%8;if(sel==6)sel=7;redraw=1;sfx_menu();}
  int dir=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;if((pressed&K_A)&&sel<7)dir=1;
  if(dir){switch(sel){case 0:SET.track=(SET.track+NTRK+1+dir)%(NTRK+1);set_preview();break;
    case 1:SET.musVol=clampi(SET.musVol+dir,0,10);if(!SET.musVol)mus_silence();break;
    case 2:SET.sfxVol=clampi(SET.sfxVol+dir,0,10);sfx_shot(0);break;
    case 3:SET.shake^=1;shake(10);break;case 4:SET.diff=(SET.diff+3+dir)%3;break;case 5:SET.hints^=1;break;}
   redraw=1;if(sel!=2)sfx_menu();}
  if(((pressed&K_A)&&sel==7)||(pressed&(K_B|K_START))){save_write();sfx_ok();return;}}}
static int main_menu(int sel){
 static const char*const items[6]={"CAMPAIGN","6V6 INFANTRY","6V6 TANKS","6V6 DOGFIGHT","LOADOUT","SETTINGS"};
 static const char*const desc[6]={"MISSIONS: WW2, COLD WAR, MODERN","TEAM DEATHMATCH VS BOTS","ARMOR BATTLE - HULL + TURRET","AIR SUPERIORITY - GUNS + MISSILES","CUSTOMIZE WEAPONS, PERKS, VEHICLES","MUSIC, VOLUME, SHAKE, DIFFICULTY"};
 int redraw=1;
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  if(redraw){m3_title(0);m3_fill(30,82,180,76,RGB(14,18,36));m3_box(30,82,180,76,2);
   for(int k=0;k<6;k++)m3_text(56,85+k*10,cat(k==sel?"> ":"  ",items[k]),k==sel?C_YEL:C_WHITE,1);
   m3_ctext(147,desc[sel],C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+5)%6;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%6;redraw=1;sfx_menu();}
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
   if(mode==MM_AIR){m3_ctext(122,cat(e==0?"YOUR PLANE: ":"YOUR JET: ",AIRV[e][airSel[e]].n),C_WHITE,1);if(e==0)m3_ctext(132,AIRV[0][airSel[0]].side?"AXIS VS ALLIED AIRCRAFT":"ALLIED VS AXIS AIRCRAFT",C_WHITE,1);else m3_ctext(132,cat("ENEMY: ",AIRV[e][(airSel[e]+1)%NAIR[e]].n),C_WHITE,1);}
   m3_ctext(144,"A: LOADOUT   B: BACK",C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if((pressed&(K_UP|K_LEFT))&&e>0){e--;redraw=1;sfx_menu();}
  if((pressed&(K_DOWN|K_RIGHT))&&e<4){e++;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();*era=e;return 1;}
  if(pressed&K_B){sfx_menu();return 0;}}}

static int map_select(int mode,int era){
 static int last=0;int sel=last,redraw=2;
 for(;;){BEAT->screen=SCR_MAPSEL;BEAT->menu=sel;BEAT->era=era;BEAT->mode=mode;
  if(redraw){m3_title(1);m3_fill(4,4,232,152,RGB(14,18,36));m3_box(4,4,232,152,2);
   m3_ctext(8,cat("SELECT MAP - ",ERA_NAME[era]),C_YEL,1);
   for(int k=0;k<=NMAPS;k++){const char*n=k<NMAPS?map_def(mode,era,k)->n:"RANDOM";m3_text(10,24+k*12,k==sel?">":" ",C_YEL,1);m3_text(18,24+k*12,n,k==sel?C_YEL:C_WHITE,1);}
   if(sel<NMAPS){const MapDef*md=map_def(mode,era,sel);gen_map(mode,era,sel);
    const u32*tl=mode==MM_AIR?sky_t(era):arena_t(era);const u16*pl=mode==MM_AIR?sky_p(era):arena_p(era);const u16*wp=water_p(era);
    int PX=146,PY=22,PS=84;m3_fill(PX-2,PY-2,PS+4,PS+4,RGB(0,0,0));
    for(int y=0;y<PS;y++)for(int x=0;x<PS;x++){int t=map[(y*mapH/PS)*MAXW+(x*mapW/PS)];u32 w=tl[(t*4+3)*8+((x+y)&3)];int ci=(w>>(((x>>1)&7)*4))&15;
     u16 c=(mode!=MM_AIR&&mtBank[t])?wp[ci?ci:2]:pl[ci?ci:1];M3[(PY+y)*240+PX+x]=tint_c(c,md->theme);}
    m3_text(10,110,md->d,C_GRAY,1);
    static const char*const LN[4]={"OPEN FIELD","TOWN BLOCKS","RIVER CROSSING","FOREST / RUINS"};static const char*const SN[4]={"COAST + TERRAIN","OCEAN + ISLANDS","MOUNTAINS","DEBRIS FIELD"};
    static const char*const TN[5]={"","  SNOW","  DESERT","  NIGHT","  DUSK"};
    m3_text(10,121,cat(mode==MM_AIR?SN[md->layout]:LN[md->layout],TN[md->theme]),C_BLUE,1);}
   else{m3_text(160,56,"???",C_YEL,2);m3_text(10,110,"ONE OF THE MAPS AT RANDOM",C_GRAY,1);}
   m3_text(10,132,cat(itoa(NMAPS)," MAPS PER MODE PER ERA"),C_GRAY,1);
   m3_ctext(142,"A: DEPLOY   B: BACK",C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+NMAPS)%(NMAPS+1);redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%(NMAPS+1);redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();last=sel;return sel<NMAPS?sel:rr(NMAPS);}
  if(pressed&K_B){sfx_menu();return -1;}}}
static void loading(const char*s){m3_title(1);m3_ctext(76,s,C_YEL,1);m3_present();next_frame();}
int main(void){
 REG_WAITCNT=0x4317;snd_init();save_load();irq_init();beat_common(SCR_TITLE);BEAT->frame=0;
 for(;;){
  title_screen();
  int sel=0,era=0;
  for(;;){bgm(1,0);sel=main_menu(sel);if(sel<0)break;
   if(sel==5){settings_menu();}
   else if(sel==4){loadout_menu(0,era);}
   else if(sel==0){if(loadout_menu(2,0)){loading("DEPLOYING TO DOG GREEN...");campaign();}}
   else if(era_select(sel,&era)){if(loadout_menu(sel==1?1:sel==2?3:4,era)){int mi=map_select(sel,era);if(mi>=0){loading(cat("DEPLOYING: ",map_def(sel,era,mi)->n));mp_run(sel,era,mi);}}}
   }
 }
}
