// Arsenal of Eras - GBA demake (personal build). Bare-metal C, no external libs.
#include "gba.h"
#include "assets.h"
#include "core.inc"
#include "loadout.inc"
#include "soldier.inc"
#include "mp.inc"
#include "camp.inc"
#include "drone.inc"

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
 static const char*const LAB[9]={"MUSIC TRACK","MUSIC VOLUME","SFX VOLUME","SCREEN SHAKE","DAMAGE TAKEN","BOT DIFFICULTY","CONTROL HINTS","RUMBLE",""};
 static const char*const DN[3]={"LOW (EASY)","NORMAL","HIGH (HARD)"};
 int sel=0,redraw=2;
 for(;;){BEAT->screen=SCR_SETTINGS;BEAT->menu=sel;BEAT->extra=SET.track|(SET.musVol<<8)|(SET.sfxVol<<12)|(SET.diff<<16)|(SET.shake<<18)|(SET.hints<<19)|(SET.bot<<20);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;
   m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);m3_ctext(9,"SETTINGS",C_YEL,2);
   for(int k=0;k<9;k++){int y=30+k*11+(k==8?4:0);u16 c=k==sel?C_YEL:(k==7?C_GRAY:C_WHITE);
    if(k==8){m3_text(14,y,k==sel?"> SAVE & BACK":"  SAVE & BACK",c,1);continue;}
    m3_text(14,y,LAB[k],k==sel?C_YEL:C_GRAY,1);const char*v="";
    switch(k){case 0:v=SET.track==0?"AUTO (BY ERA)":SET.track==NTRK?"OFF":TRK_NAME[SET.track-1];break;
     case 1:case 2:{int n=k==1?SET.musVol:SET.sfxVol;m3_fill(110,y,92,7,RGB(40,44,70));if(n)m3_fill(110,y,n*92/10,7,k==1?C_BLUE:C_GREEN);v=itoa(n);m3_text(208,y,v,c,1);v=0;break;}
     case 3:v=SET.shake?"ON":"OFF";break;case 4:v=DN[SET.diff];break;case 5:v=BOTN[SET.bot];break;case 6:v=SET.hints?"ON":"OFF";break;case 7:v="N/A (NO RUMBLE HW)";break;}
    if(v){m3_text(110,y,v,c,1);if(k==sel&&k!=7){m3_text(104,y,"<",C_YEL,1);m3_text(110+slen(v)*6+1,y,">",C_YEL,1);}}}
   static const char*const BDD[4]={"SLOW AIM, LOW HP, TIMID BOTS","STANDARD ENEMY BOTS","SHARPER AIM, MORE HP, PUSHY","FAST, ACCURATE, TOUGH, AGGRESSIVE"};
   const char*d=sel==0?(SET.track==0?"ERA-APPROPRIATE TRACK FOR EACH MODE":SET.track==NTRK?"NO MUSIC":TRK_DESC[SET.track-1]):sel==1?"PSG MUSIC LEVEL (0 = MUTE)":sel==2?"GUNFIRE / EXPLOSION / UI SOUND LEVEL":
    sel==3?"SHAKE ON EXPLOSIONS, HITS, TANK FIRE":sel==4?"HOW MUCH DAMAGE YOU TAKE":sel==5?BDD[SET.bot]:sel==6?"SHOW BUTTON HINTS AT MATCH START":sel==7?"GBA EMULATORS: NO RUMBLE MOTOR":"SAVES TO SRAM";
   m3_ctext(136,d,C_GRAY,1);if(sel==5)m3_ctext(146,"6V6, CAMPAIGN ENEMIES, DRONE AA",C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+8)%9;if(sel==7)sel=6;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%9;if(sel==7)sel=8;redraw=1;sfx_menu();}
  int dir=(pressed&K_RIGHT)?1:(pressed&K_LEFT)?-1:0;if((pressed&K_A)&&sel<8)dir=1;
  if(dir){switch(sel){case 0:SET.track=(SET.track+NTRK+1+dir)%(NTRK+1);set_preview();break;
    case 1:SET.musVol=clampi(SET.musVol+dir,0,10);if(!SET.musVol)mus_silence();break;
    case 2:SET.sfxVol=clampi(SET.sfxVol+dir,0,10);sfx_shot(0);break;
    case 3:SET.shake^=1;shake(10);break;case 4:SET.diff=(SET.diff+3+dir)%3;break;case 5:SET.bot=(SET.bot+4+dir)%4;break;case 6:SET.hints^=1;break;}
   redraw=1;if(sel!=2)sfx_menu();}
  if(((pressed&K_A)&&sel==8)||(pressed&(K_B|K_START))){save_write();sfx_ok();return;}}}
#define NMM 15
static int main_menu(int sel){
 static const char*const items[NMM]={"CAMPAIGN","6V6 INFANTRY","6V6 ARMOR","6V6 DOGFIGHT","6V6 NAVAL","DRONE OPS","WORLD FLIGHT","SPEC OPS","LOADOUT","SETTINGS","TUTORIAL","SPACE LAUNCH","SWORD FIGHT","ANTI-AIR","BRADLEY OPS"};
 static const char*const desc[NMM]={"MISSIONS: WW2, COLD WAR, MODERN","PICK A FACTION, ANY-ERA ARSENAL","TANKS, TDS, IFVS, HALFTRACKS, JEEPS","AIR SUPERIORITY - GUNS + MISSILES","BATTLESHIPS, CARRIERS, DESTROYERS","MODERN UAV STRIKES - MQ-9, TB2, TRITON","FREE ROAM IN THE X-15, LANDMARKS","PLATFORMER: 10 WORLDS, 90 COURSES","ERA KITS, 6V6 ARSENAL, CHARACTER","MUSIC, VOLUME, BOT DIFFICULTY","BOOT CAMP WITH KC - CONTROLS + MODES","35 REAL ROCKETS: ASCENT, LAND, DOCK","1V1 DUEL: KNIGHT, SAMURAI, VIKING...","AA GUNS + SAMS VS PLANES AND DRONES","SPEC OPS: BRADLEY - IFV PLATFORMER"};
 int redraw=1;
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  if(redraw){m3_title(0);m3_fill(4,74,232,84,C_PANEL);m3_box(4,74,232,84,2);
   for(int k=0;k<NMM;k++){int col=k/5,row=k%5,xo=0;m3_text(4+col*74+xo,80+row*12,k==sel?">":" ",C_YEL,1);m3_text(10+col*74+xo,80+row*12,trunc12(items[k]),k==sel?C_YEL:C_WHITE,1);}
   m3_ctext(147,desc[sel],C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+NMM-1)%NMM;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%NMM;redraw=1;sfx_menu();}
  if(pressed&(K_LEFT|K_RIGHT)){{int r=sel%5,c=sel/5,d=(pressed&K_RIGHT)?1:2;do c=(c+d)%3;while(c*5+r>=NMM);sel=c*5+r;}redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();return sel;}
  if(pressed&K_B)return -1;}}
static int era_select(int mode,int *era){
 static const char*const MN[4]={"","6V6 INFANTRY","6V6 TANKS","6V6 DOGFIGHT"};
 static const char*const ED[NERA]={"NORMANDY HEDGEROWS 1944","FROZEN FRONT 1968","DESERT CITY 2026","NEON MEGACITY 2089","LUNAR BASE 2140","WESTERN FRONT 1917","COLONIES 1776","AGINCOURT 1415"};
 static const u8 ORD[NERA]={7,6,5,0,1,2,3,4}; // chronological display order
 static const u16 EC[NERA]={RGB(150,170,100),RGB(200,214,232),RGB(220,190,130),RGB(80,240,255),RGB(200,170,255),RGB(190,170,120),RGB(130,160,230),RGB(230,190,110)};
 int e=*era,redraw=1;int pos=0;for(int k=0;k<NERA;k++)if(ORD[k]==e)pos=k;
 for(;;){BEAT->screen=SCR_ERA;BEAT->era=e;BEAT->mode=mode;
  if(redraw){m3_title(1);m3_fill(12,8,216,146,C_PANEL);m3_box(12,8,216,146,2);m3_ctext(14,MN[mode],C_YEL,2);m3_ctext(32,"SELECT ERA",C_GRAY,1);
   for(int k=0;k<NERA;k++){int q=ORD[k];m3_text(22,44+k*9,cat(q==e?"> ":"  ",ERA_NAME[q]),q==e?EC[q]:C_WHITE,1);}
   m3_text(150,44+pos*9,TEAMN[e][0],C_BLUE,1);m3_text(196,44+pos*9,TEAMN[e][1],C_RED,1);
   m3_ctext(116,ED[e],EC[e],1);
   if(mode==MM_INF){m3_ctext(126,cat("PRIMARY: ",PRIM[e][LO[e].prim].n),C_WHITE,1);m3_ctext(135,cat("SIDEARM: ",SEC[e][LO[e].sec].n),C_WHITE,1);}
   if(mode==MM_TANK){m3_ctext(126,cat("YOUR TANK: ",TANKV[e][tankSel[e]].n),C_WHITE,1);m3_ctext(135,cat("ENEMY: ",TANKV[e][1-tankSel[e]].n),C_WHITE,1);}
   if(mode==MM_AIR){m3_ctext(126,cat(e==0||e>=5?"YOUR AIRCRAFT: ":"YOUR JET: ",AIRV[e][airSel[e]].n),C_WHITE,1);if(e==0)m3_ctext(135,AIRV[0][airSel[0]].side?"AXIS VS ALLIED AIRCRAFT":"ALLIED VS AXIS AIRCRAFT",C_WHITE,1);else m3_ctext(135,cat("ENEMY: ",AIRV[e][(airSel[e]+1)%NAIR[e]].n),C_WHITE,1);}
   m3_ctext(145,"A: LOADOUT   B: BACK",C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if((pressed&(K_UP|K_LEFT))&&pos>0){pos--;e=ORD[pos];redraw=1;sfx_menu();}
  if((pressed&(K_DOWN|K_RIGHT))&&pos<NERA-1){pos++;e=ORD[pos];redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();*era=e;return 1;}
  if(pressed&K_B){sfx_menu();return 0;}}}


static int drone_hangar(int*mis){int sel=0,redraw=2;static const char*const RL[4]={"DRONE","CALL-IN","MISSION",""};
 for(;;){const DroneS*Dr=&DRN[droneSel];BEAT->screen=SCR_HANGAR;BEAT->menu=sel;BEAT->extra=droneSel|(droneCall<<4)|(*mis<<8);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;m3_fill(4,4,232,152,RGB(10,20,14));m3_box(4,4,232,152,2);
   m3_ctext(8,"DRONE OPS - MODERN - HANGAR",C_GREEN,1);
   for(int k=0;k<4;k++){int y=24+k*12+(k==3?4:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(k==3){m3_text(10,y,k==sel?"> LAUNCH":"  LAUNCH",c,1);continue;}
    m3_text(10,y,RL[k],k==sel?C_YEL:C_GRAY,1);const char*v=k==0?Dr->n:k==1?CALLN[droneCall]:DMN[*mis];m3_text(58,y,v,c,1);}
   int sx=10,sy=74;m3_text(sx,sy,Dr->role,C_GREEN,1);sy+=10;
   int pay=0;for(int k=0;k<3;k++)if(Dr->w[k]!=255&&Dr->c[k])pay+=Dr->c[k]*DWP[Dr->w[k]].dmg;
   m3_bar(sx,sy,"SPEED",Dr->spd,640,C_BLUE);m3_bar(sx+76,sy,"ALTITUDE",Dr->alt,5,C_WHITE);m3_bar(sx+152,sy,"LOITER",Dr->loiter,600,C_GREEN);
   m3_bar(sx,sy+18,"SENSOR",Dr->sensor,200,C_YEL);m3_bar(sx+76,sy+18,"PAYLOAD",pay,800,C_RED);m3_bar(sx+152,sy+18,"ARMOR",Dr->hp,120,RGB(255,140,60));
   {int x=sx,y=sy+36;for(int k=0;k<3;k++)if(Dr->w[k]!=255&&Dr->c[k]){m3_text(x,y,cat3(itoa(Dr->c[k]),"X ",DWP[Dr->w[k]].n),C_WHITE,1);y+=8;}if(Dr->recon)m3_text(x+130,sy+36,"RECON: B MARKS",C_GRAY,1);}
   m3_ctext(147,sel==2?DMD[*mis]:sel==1?"OPTIONAL OFF-BOARD SUPPORT, 1-2 USES":sel==0?"L/R OR LEFT/RIGHT: CHANGE DRONE":"A: LAUNCH   B: BACK",C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+3)%4;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%4;redraw=1;sfx_menu();}
  int dir=(pressed&(K_RIGHT|K_R))?1:(pressed&(K_LEFT|K_L))?-1:0;if((pressed&K_A)&&sel<3)dir=1;
  if((pressed&(K_L|K_R))&&sel!=0){droneSel=(droneSel+NDRONE+dir)%NDRONE;redraw=1;sfx_menu();dir=0;}
  if(dir){if(sel==0)droneSel=(droneSel+NDRONE+dir)%NDRONE;else if(sel==1)droneCall=(droneCall+3+dir)%3;else *mis=(*mis+DM_N+dir)%DM_N;redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(sel==3||(pressed&K_START))){save_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();sfx_menu();return 0;}}}
static int map_select(int mode,int*era){ // all maps of the mode across every era (+ RANDOM); returns map index, *era = map's era
 static int lastm[5];int last=lastm[mode];int N=(mode==MM_SEA?3:NERA)*NMAPS;int sel=last,redraw=2,top=0;
 for(;;){BEAT->screen=SCR_MAPSEL;BEAT->menu=sel;BEAT->mode=mode;
  if(sel<top)top=sel;if(sel>top+7)top=sel-7;
  if(redraw){m3_title(1);m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);
   m3_ctext(8,"SELECT MAP - ANY ERA",C_YEL,1);
   for(int k=top;k<=N&&k<top+8;k++){int y=22+(k-top)*11;const char*n=k<N?map_def(mode,k/NMAPS,k%NMAPS)->n:"RANDOM";m3_text(8,y,k==sel?">":" ",C_YEL,1);m3_text(15,y,trunc12(n),k==sel?C_YEL:C_WHITE,1);
    if(k<N)m3_text(92,y,ERA_TAG[k/NMAPS],C_GRAY,1);}
   if(sel<N){int e=sel/NMAPS,mi=sel%NMAPS;const MapDef*md=map_def(mode,e,mi);gen_map(mode,e,mi);
    int ts=mode==MM_SEA?(md->theme==TH_SNOW?1:0):e;const u32*tl=mode>=MM_AIR?sky_t(ts):arena_t(e);const u16*pl=mode>=MM_AIR?sky_p(ts):arena_p(e);const u16*wp=water_p(e);
    int PX=146,PY=22,PS=84;m3_fill(PX-2,PY-2,PS+4,PS+4,RGB(0,0,0));
    for(int y=0;y<PS;y++)for(int x=0;x<PS;x++){int t=map[(y*mapH/PS)*MAXW+(x*mapW/PS)];u32 w=tl[(t*4+3)*8+((x+y)&3)];int ci=(w>>(((x>>1)&7)*4))&15;
     u16 c=(mode<MM_AIR&&mtBank[t])?wp[ci?ci:2]:pl[ci?ci:1];M3[(PY+y)*240+PX+x]=tint_c(c,mode==MM_SEA&&md->theme==TH_SNOW?0:md->theme);}
    m3_text(8,114,md->n,C_WHITE,1);m3_text(8,124,md->d,C_GRAY,1);
    static const char*const LN[4]={"OPEN FIELD","TOWN BLOCKS","RIVER CROSSING","FOREST / RUINS"};static const char*const SN[4]={"COAST + TERRAIN","OCEAN + ISLANDS","MOUNTAINS","DEBRIS FIELD"};static const char*const WN[4]={"OPEN OCEAN","ISLAND GROUP","NARROW STRAIT","FJORD / ICE"};
    static const char*const TN[5]={"","  SNOW","  DESERT","  NIGHT","  DUSK"};
    m3_text(8,134,cat3(ERA_NAME[e]," - ",cat(mode==MM_SEA?WN[md->layout]:mode==MM_AIR?SN[md->layout]:LN[md->layout],TN[md->theme])),C_BLUE,1);}
   else{m3_text(160,56,"???",C_YEL,2);m3_text(8,114,"RANDOM",C_WHITE,1);m3_text(8,124,"ANY MAP FROM ANY ERA",C_GRAY,1);}
   m3_ctext(146,cat3(itoa(N)," MAPS   A: DEPLOY   B: BACK",""),C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+N)%(N+1);redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%(N+1);redraw=1;sfx_menu();}
  last=sel;if(pressed&K_L){sel=sel>=NMAPS?sel-NMAPS:N;redraw=1;sfx_menu();}
  if(pressed&K_R){sel=sel+NMAPS<=N?sel+NMAPS:0;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();lastm[mode]=sel;int k=sel<N?sel:rr(N);*era=k/NMAPS;return k%NMAPS;}
  if(pressed&K_B){sfx_menu();return -1;}}}
static void swatch(int x,int y,int f){const Fac*F=&FAC[f];m3_fill(x,y,8,8,fac_c(F->m,1,1));m3_fill(x+8,y,4,8,fac_c(F->t,1,1));}
static int faction_select(int mode){int col=0,redraw=2;
 for(;;){BEAT->screen=SCR_ERA;BEAT->mode=mode;BEAT->menu=col;BEAT->extra=g6.fac0|(g6.fac1<<8);
  if(redraw){m3_title(1);m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);
   m3_ctext(8,mode==MM_INF?"6V6 INFANTRY - CHOOSE SIDE":mode==MM_TANK?"6V6 ARMOR - CHOOSE SIDE":mode==MM_SEA?"6V6 NAVAL - CHOOSE SIDE":"6V6 DOGFIGHT - CHOOSE SIDE",C_YEL,1);
   m3_text(20,22,"YOUR FACTION",col==0?C_YEL:C_GRAY,1);m3_text(132,22,"ENEMY FACTION",col==1?C_YEL:C_GRAY,1);
   for(int k=0;k<NFAC;k++){int y=33+k*8;
    m3_text(14,y,k==g6.fac0?">":" ",col==0?C_YEL:C_BLUE,1);swatch(22,y,k);m3_text(38,y,FAC[k].n,k==g6.fac0?C_BLUE:C_WHITE,1);
    m3_text(126,y,k==g6.fac1?">":" ",col==1?C_YEL:C_RED,1);swatch(134,y,k);m3_text(150,y,FAC[k].n,k==g6.fac1?C_RED:(k==g6.fac0?C_GRAY:C_WHITE),1);}
   m3_ctext(136,"COLORS BY FACTION, GENERIC MARKINGS",C_GRAY,1);
   m3_ctext(146,"UP/DN:PICK  </>:SWAP  A:NEXT  B:BACK",C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  u8*f=col?&g6.fac1:&g6.fac0;u8 other=col?g6.fac0:g6.fac1;
  if(pressed&(K_UP|K_DOWN)){int d=(pressed&K_UP)?NFAC-1:1;do{*f=(*f+d)%NFAC;}while(*f==other);redraw=1;sfx_menu();}
  if(pressed&(K_LEFT|K_RIGHT)){col^=1;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();save_write();return 1;}
  if(pressed&K_B){sfx_menu();return 0;}}}
static void loading(const char*s){m3_title(1);m3_ctext(76,s,C_YEL,1);m3_present();next_frame();}
#include "portraits.inc"
static void credits_roll(int after_campaign);
#include "camp2.inc"
#include "char.inc"
#include "world.inc"
#include "plat.inc"
#include "kc.inc"
#include "launch.inc"
#include "duel.inc"
#include "aa.inc"
#include "menu.inc"
#include "brad.inc"
#include "sniper.inc"
#include "eod.inc"
#include "lrrp.inc"
#include "space.inc"
#include "explorer.inc"
#include "fps.inc"
#include "frontier.inc"
#include "frontier2.inc"
#include "frontier3.inc"
#include "frontier4.inc"
#include "over.inc"
#include "frontier5.inc"
#include "frontier_stub.inc"
#include "race.inc"
#include "v132core.inc"
#include "hangar.inc"
#include "credits.inc"
#include "vbm.inc"
#include "sandbox.inc"
#include "invaders.inc"
#include "fire.inc"
#include "v132_stub.inc"
#include "menu131.inc"
#include "menu133.inc"
#include "arcade.inc"
int main(void){
 REG_WAITCNT=0x4317;snd_init();save_load();irq_init();beat_common(SCR_TITLE);BEAT->frame=0;
 for(;;){
  title_screen();
  if(!kc_done()){ps_read();tutorial(1);}
  int sel=0,era=0;
  int tsel=0;
  for(;;){bgm(1,0);if(!LEGACY_MENU){tsel=main_menu4(tsel);if(tsel<0)break;tile_open(tsel);pressed=0;continue;}
   sel=main_menu3(sel);if(sel<0)break;
   if(sel==CAT_LOAD)loadout_hub(&era);else if(sel==CAT_SET)settings_menu();else if(sel==CAT_KC)tutorial(0);else if(sel==CAT_CRED)credits_roll(0);else category(sel);
   pressed=0;
   }
 }
}
