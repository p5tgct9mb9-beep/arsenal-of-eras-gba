// ===================================================================== v1.3.3 MAIN MENU: 10 big tiles over Earth-from-orbit -> clean submenus
// Tiles: icon + label + one-line tag; bottom bar = full description + contents. Submenus: highlight bar, icons, NEW / FEATURED tags,
// Mars Rocks balance in the header. Test hook: 'LGCY' at 0x0203FFC8 keeps the v1.3.2 13-row menu for old regression scripts.
#include "mdicons.inc"
#define LEGACY_MENU (*(volatile u32*)0x0203FFC8==0x5943474Cu)
#define SCR_TILES 105
static void ic_draw(int k,int x0,int y0){const u8*d=IC_DAT[k];for(int y=0;y<12;y++){u16*p=&fb[(y0+y)*240+x0];for(int x=0;x<12;x++){int v=(d[y*6+(x>>1)]>>((x&1)*4))&15;if(v)p[x]=IC_PAL[v];}}}
static int md_icon(int md){
 static const u8 MI[MD_N]={IC_CAMP,IC_RIFLE,IC_TANK,IC_JET,IC_SHIP,IC_SWORD,IC_SPEC,IC_BRAD,IC_SNIPE,IC_EOD,IC_AA,IC_DRONE,IC_ROCKET,IC_FLIGHT,IC_LRRP,IC_FPS,IC_SPACE,IC_ASTRO,IC_MOON,IC_MARS,IC_ODEF,IC_RBUILD,IC_ASTER,IC_WHATIF,IC_HIST,IC_OVER,
  IC_RACE,IC_TT,IC_FREE,IC_BUILD,IC_SAND,IC_INVADE,IC_VBM,IC_FIRE,IC_BOOT,IC_LOADOUT,IC_SETTINGS,IC_CREDITS,IC_CODES,IC_TROPHY,IC_LB,IC_PROFILE,IC_STORE,IC_INVADE};
 return md>=0&&md<MD_N?MI[md]:IC_WHATIF;}
// short list names (<= 14 chars) so icons + NEW tags fit
static const char*const MLN[MD_N]={"CAMPAIGN","INFANTRY 6V6","ARMOR 6V6","DOGFIGHT 6V6","NAVAL 6V6","SWORD FIGHT","SPEC OPS","BRADLEY OPS","SNIPER","EOD","ANTI-AIRCRAFT","DRONE OPS","SPACE LAUNCH","WORLD FLIGHT",
 "LRRP RECON","FPS MISSIONS","SPACE PROGRAM","ASTRO EXPLORER","MOON BASE","MARS MISSION","ORBIT DEFENSE","ROCKET BUILDER","ASTEROIDS","WHAT-IF 1945","HISTORY TOUR","OVERFLIGHT",
 "GRAND PRIX","TIME TRIAL","FREE DRIVE","HANGAR BUILD","SANDBOX","INVADERS","VON BRAUN OPS","FIREFIGHTER","BOOT CAMP (KC)","LOADOUT","SETTINGS","CREDITS","SECRET CODES","ACHIEVEMENTS","LEADERBOARDS","PROFILE CARD","STORE","CABINET (12)"};
static const char*mln(int md){if(md==MD_OVER&&!v131_over_unlocked())return "CLASSIFIED";return MLN[md];}
// ---- NEW tags: content new since v1.3.1, shown until first opened (SRAM 0x26C0 'N')
typedef struct{u8 seen[8];}NSeen;static NSeen NSN EWRAM;static int nsn_ld;
static void nsn_load(void){if(nsn_ld)return;nsn_ld=1;if(!sv_get(0x26C0,'N',1,&NSN,sizeof NSN))memset(&NSN,0,sizeof NSN);}
static int md_isnew(int md){static const u8 NEWM[10]={MD_FIRE,MD_INVADE,MD_GP,MD_TT,MD_FREE,MD_BUILD,MD_SAND,MD_VBM,MD_CABINET,MD_FLIGHT};if(LEGACY_MENU)return 0;
 for(int i=0;i<10;i++)if(NEWM[i]==md){nsn_load();return !((NSN.seen[md>>3]>>(md&7))&1);}return 0;}
static void md_seen(int md){if(md<0||md>=MD_N||!md_isnew(md))return;NSN.seen[md>>3]|=1<<(md&7);sv_put(0x26C0,'N',1,&NSN,sizeof NSN);}
static int md_featured(int md){return md==MD_FIRE;}
// ---- tiles
enum{TI_CAMP,TI_BAT,TI_SPEC,TI_AIR,TI_SPACE,TI_ARC,TI_LEARN,TI_CREATE,TI_STORE,TI_EXTRA,TI_N};
typedef struct{const char*n,*tag,*d1,*d2;u8 ic,bg;}Tile;
enum{BG_AIR=CAT_N,BG_LEARN,BG_EXTRA};
static const Tile TL[TI_N]={
 {"CAMPAIGN","BY ERA","MISSIONS ACROSS EVERY ERA","WW2 - COLD WAR - MODERN - WHAT-IF",IC_CAMP,CAT_CAMP},
 {"BATTLES","6V6 + FPS","BOT BATTLES: LAND, SEA, AIR, 1V1","INFANTRY ARMOR DOGFIGHT NAVAL SWORD FPS",IC_RIFLE,CAT_MP},
 {"SPEC OPS","FIRE+SOF","GROUND OPERATIONS + FIREFIGHTER","FIRE - SPEC OPS - BRADLEY - SNIPER - EOD",IC_SPEC,CAT_SPEC},
 {"AIR","JETS+UAV","FLY, FIGHT, DEFEND AND SPY","DOGFIGHT - ANTI-AIR - DRONES - FLIGHT",IC_JET,BG_AIR},
 {"SPACE","ROCKETS","REAL ROCKETS, ORBITS, MOON, MARS","LAUNCH - SPACE PROGRAM - MOON - MARS",IC_ROCKET,CAT_EXPLORE},
 {"ARCADE","QUICK FUN","QUICK-PLAY ARCADE + EV RACING","INVADERS - GRAND PRIX - TIME TRIAL",IC_INVADE,CAT_RACE},
 {"LEARN","HISTORY","TRAIN WITH KC, TOUR THE HISTORY","BOOT CAMP (KC) - HISTORY TOUR",IC_HIST,BG_LEARN},
 {"CREATE","BUILDERS","BUILD AIRCRAFT, ROCKETS, BATTLES","HANGAR - ROCKETS - SANDBOX - LOADOUT",IC_BUILD,CAT_HANGAR},
 {"STORE","","SPEND MARS ROCKS EARNED IN PLAY","MARS ROCKS: EARN IN ALMOST EVERY MODE",IC_STORE,BG_EXTRA},
 {"EXTRAS","CODES+","SECRET CODES, CREDITS, SETTINGS","CODES - CREDITS - SETTINGS",IC_SETTINGS,BG_EXTRA}};
static const MCat MC3[TI_N]={
 {"CAMPAIGN","CAMPAIGN BY ERA",4,{MD_CAMP},TR_OMAHA},
 {"BATTLES","6V6, 1V1 + FPS",6,{MD_INF,MD_TANK,MD_AIR,MD_SEA,MD_SWORD,MD_FPS},TR_MARCH},
 {"SPEC OPS","GROUND OPERATIONS",6,{MD_FIRE,MD_SPEC,MD_BRAD,MD_SNIPE,MD_EOD,MD_LRRP},TR_JUNGLE},
 {"AIR","FLIGHT + AIR DEFENSE",5,{MD_AIR,MD_AA,MD_DRONE,MD_FLIGHT,MD_OVER},TR_COLD},
 {"SPACE","ROCKETS + EXPLORATION",8,{MD_LAUNCH,MD_SPACE,MD_ASTRO,MD_VBM,MD_MOON,MD_MARS,MD_ODEF,MD_ASTER},TR_SPACE},
 {"ARCADE","QUICK PLAY",5,{MD_CABINET,MD_INVADE,MD_GP,MD_TT,MD_FREE},TR_INVADE},
 {"LEARN","HISTORY + TRAINING",3,{MD_BOOT,MD_HIST,MD_WHATIF},TR_THEME},
 {"CREATE","BUILD YOUR OWN",4,{MD_BUILD,MD_RBUILD,MD_SAND,MD_LOADOUT},TR_HANGAR},
 {"STORE","MARS ROCKS",1,{MD_STORE},TR_THEME},
 {"EXTRAS","CODES, CREDITS, SETTINGS",3,{MD_CODES,MD_CREDITS,MD_SETTINGS},TR_THEME}};
static int tile_new(int t){const MCat*C=&MC3[t];for(int i=0;i<C->nm;i++)if(md_isnew(C->m[i]))return C->m[i];return -1;}
// extra themed backgrounds for the new tiles
static void bg133(int c){
 if(c==BG_AIR){grad(0,120,RGB(40,90,170),RGB(170,210,240));grad(120,160,RGB(90,120,80),RGB(50,70,46));
  for(int i=0;i<6;i++){int x=(i*61)%220+10,y=(i*29)%60+20;m3_fill(x,y,30,6,RGB(230,236,244));m3_fill(x+6,y-3,18,4,RGB(230,236,244));}
  for(int k=0;k<2;k++){int x=60+k*90,y=60+k*18;for(int j=0;j<50;j++)m3_fill(x-j*2,y+j/3,2,1,RGB(240,244,250));m3_fill(x,y-1,10,3,RGB(90,96,110));m3_fill(x+3,y-4,3,9,RGB(90,96,110));}}
 else if(c==BG_LEARN){grad(0,160,RGB(60,40,30),RGB(24,18,14));for(int x=10;x<240;x+=46){m3_fill(x,20,10,120,RGB(150,130,100));m3_fill(x-3,18,16,4,RGB(180,160,120));m3_fill(x-3,138,16,4,RGB(180,160,120));}
  for(int k=0;k<5;k++){int x=24+k*46;m3_fill(x+6,60,20,26,RGB(120,90,50));m3_fill(x+8,62,16,22,RGB(200,180,130));}m3_fill(0,142,240,18,RGB(90,60,40));}
 else{m3_fill(0,0,240,160,RGB(12,10,24));for(int y=0;y<160;y+=8)for(int x=(y/8&1)*8;x<240;x+=16)m3_fill(x,y,8,8,RGB(18,16,34));
  for(int i=0;i<14;i++){int x=(i*53)%230,y=(i*37)%150;m3_fill(x,y,3,3,(i&1)?RGB(236,196,92):RGB(90,210,240));}}}
// ---- Mars Rocks balance chip
static void rocks_chip(int x,int y){mr_load();u32 b=MR.bal;if(b>999999)b=999999;const char*s=itoa(b);int w=slen(s)*6+16;m3_fill(x-w,y,w,11,RGB(40,16,10));m3_fill(x-w,y,w,1,RGB(220,100,50));
 m3_fill(x-w+3,y+3,6,6,RGB(200,80,40));m3_fill(x-w+4,y+2,4,1,RGB(230,120,70));m3_px(x-w+5,y+5,RGB(140,50,30));m3_text(x-w+12,y+2,s,RGB(255,190,120),1);}
// ---- v1.3.5 Mars Rocks: every mode pays. Wins/losses are tallied from the victory/defeat jingles,
// plus a time bonus; modes that already pay inline (Firefighter, Arcade) just show their session total.
static u32 moV0,moE0;static int moLast;
static void mo_begin(void){moWin=moLoss=0;mr_load();moE0=MR.earned;moV0=vblN;}
static int mo_wbase(int md){switch(md){case MD_CAMP:case MD_FPS:case MD_LRRP:case MD_BRAD:case MD_SPEC:return 30;
 case MD_INF:case MD_TANK:case MD_AIR:case MD_SEA:case MD_SNIPE:case MD_EOD:case MD_DRONE:return 25;
 case MD_SWORD:case MD_INVADE:case MD_GP:case MD_TT:return 15;case MD_FIRE:return 0;default:return 20;}}
static void mo_end(int md){u32 secs=(vblN-moV0)/60;int w=moWin>4?4:moWin,l=moLoss>4?4:moLoss;int b=0;
 if(md!=MD_FIRE&&md!=MD_CABINET&&!MD_PSEUDO(md)){b=w*mo_wbase(md)+l*4+(int)(secs/45>12?12:secs/45);if(secs<20&&!w&&!l)b=0;else if(b<2)b=2;if(b>120)b=120;}
 int bn=b?mr_add(b):0;int tot=(int)(MR.earned-moE0);moLast=tot;if(tot<=0)return;
 for(int t=0;;t++){BEAT->screen=110;BEAT->scoreA=tot;BEAT->scoreB=MR.bal;BEAT->phase=w;
  if(t==0){chrome(BG_EXTRA,2);banner("MISSION PAY","MARS ROCKS EARNED",RGB(220,100,50));rocks_chip(232,8);
   m3_fill(30,46,180,46,RGB(40,16,10));m3_box(30,46,180,46,0);
   for(int k=0;k<3;k++){int x=46+k*5,y=60-k*3;m3_fill(x,y,12,12,RGB(170-k*20,64,34));m3_fill(x+2,y+1,8,2,RGB(230,130,80));m3_px(x+4,y+6,RGB(110,40,24));}
   m3_text(78,56,cat("+",itoa(tot)),RGB(255,190,120),3);
   int y=100;char bf[40],*o;
   if(md==MD_FIRE||tot!=bn){o=sp_put(bf,"EARNED IN PLAY: +");sp_int(o,tot-bn);m3_ctext(y,bf,C_WHITE,1);y+=10;}
   if(bn){int wb=w*mo_wbase(md);if(w){o=sp_put(bf,"WINS x");o=sp_int(o,w);o=sp_put(o,"  +");sp_int(o,wb);m3_ctext(y,bf,C_GREEN,1);y+=10;}
    if(l){o=sp_put(bf,"TRIED x");o=sp_int(o,l);o=sp_put(o,"  +");sp_int(o,l*4);m3_ctext(y,bf,C_GRAY,1);y+=10;}
    o=sp_put(bf,"PLAY TIME ");o=sp_int(o,(int)(secs/60));o=sp_put(o," MIN  +");sp_int(o,bn-wb-l*4);m3_ctext(y,bf,C_GRAY,1);y+=10;}
   m3_ctext(126,cat("WALLET: ",itoa(MR.bal)),RGB(255,190,120),1);m3_ctext(138,"A: CONTINUE",C_YEL,1);m3_present();sfx_ok();}
  next_frame();if((t>20&&(pressed&(K_A|K_B|K_START)))||((*(volatile u32*)0x0203FFD0==0x4F545541)&&t>150)){pressed=0;return;}}}
// ---- generalized submenu: list (icons, NEW/FEATURED) + per-mode options; same key flow as v1.3 categories (A focus, A select)
static void md_pseudo_open(int md);
static void cat_screen_p(const MCat*C,int key,int bgc){static const char*const EN[4]={"WORLD WAR II","COLD WAR","MODERN","WHAT-IF"};static const char*const ED[4]={"1942-1945: MIDWAY TO OMAHA","1950-1985: KOREA, VIETNAM, FULDA","1993-2026","FICTIONAL SCENARIOS"};
 int camp=C->m[0]==MD_CAMP;int nm=C->nm;if(C->m[0]==MD_LAUNCH)v131_cat_enter(CAT_EXPLORE);static u8 last[CAT_N+TI_N+2] EWRAM;int sel=last[key]<nm?last[key]:0,foc=0,os=0,redraw=1;bgm_t(C->trk);
 int rh=nm>7?13:15,y0=34;int hy=(y0+sel*rh)<<4;
 for(;;){int md=camp?MD_CAMP:C->m[sel];u8 ops[8];int no=mode_ops(md,ops);if(os>=no)os=no-1;
  BEAT->screen=foc?SCR_HUB:SCR_CAT;BEAT->mode=camp?0:md;BEAT->menu=sel;BEAT->phase=os;BEAT->extra=key;
  int ty=(y0+sel*rh)<<4;if(hy!=ty){int d=(ty-hy)*5/8;if(!d)d=ty>hy?1:-1;hy+=d;if(iabs(ty-hy)<24)hy=ty;redraw=1;}
  if(redraw){chrome(bgc,1);kl_poll();m3_text(10,6,C->n,C_YEL,2);rocks_chip(232,8);
   {int y=hy>>4;m3_fill(8,y-3,124,rh-1,RGB(16,60,110));m3_fill(8,y-3,2,rh-1,RGB(90,210,240));}
   for(int i=0;i<nm;i++){int y=y0+i*rh;int m=camp?MD_CAMP:C->m[i];int on=i==sel;
    kl_poll();if(camp){ic_draw(IC_CAMP,12,y-2);}else ic_draw(md_icon(m),12,y-2);
    m3_text(27,y,camp?EN[i]:mln(m),on?C_YEL:C_WHITE,1);
    if(!camp&&md_isnew(m)){m3_fill(111,y-1,20,9,RGB(220,40,30));m3_text(112,y,"NEW",C_WHITE,1);}
    else if(!camp&&md_featured(m)){m3_fill(111,y-1,20,9,RGB(236,196,92));m3_text(112,y,"TOP",RGB(40,24,8),1);}}
   if(MD_PSEUDO(md)){m3_text(144,38,foc?">":" ",C_GREEN,1);m3_text(150,38,"OPEN",C_GREEN,1);m3_text(144,56,"A: OPEN",C_GRAY,1);}
   else for(int i=0;i<no;i++){int y=38+i*13;int hi=foc&&i==os;m3_text(144,y,hi?">":" ",C_GREEN,1);m3_text(150,y,op_label(md,ops[i]),hi?C_GREEN:(i==0&&!foc?C_GREEN:C_WHITE),1);}
   if(!camp&&md_featured(md)){m3_fill(142,118,88,16,RGB(60,44,10));m3_fill(142,118,88,1,RGB(236,196,92));m3_text(146,123,"FEATURED MODE",RGB(236,196,92),1);}
   else if(!camp&&md_isnew(md)){m3_fill(142,118,88,16,RGB(70,14,10));m3_fill(142,118,88,1,RGB(220,40,30));m3_text(146,123,md==MD_FLIGHT?"113 AIRCRAFT!":md==MD_CABINET?"NEW IN V1.3.4":"NEW IN V1.3.2",RGB(255,150,130),1);}
   m3_ctext(142,camp?ED[sel]:MDD[md],C_GRAY,1);m3_ctext(151,foc?"A: SELECT   B: BACK TO LIST":MD_PSEUDO(md)?"A: OPEN   B: BACK":"A: OPTIONS   B: BACK",C_GRAY,1);kl_poll();m3_present();redraw=0;}
  next_frame();
  if(!foc){if(pressed&K_UP){sel=(sel+nm-1)%nm;os=0;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%nm;os=0;redraw=1;sfx_menu();}
   if(pressed&(K_A|K_RIGHT|K_START)){if(MD_PSEUDO(md)&&(pressed&(K_A|K_START))){sfx_ok();pressed=0;last[key]=sel;md_pseudo_open(md);bgCached=-1;bgm_t(C->trk);redraw=1;if(C->nm==1)return;continue;}
    foc=1;os=0;redraw=1;sfx_ok();}
   if(pressed&K_B){sfx_menu();pressed=0;last[key]=sel;return;}}
  else{if(pressed&K_UP){os=(os+no-1)%no;redraw=1;sfx_menu();}if(pressed&K_DOWN){os=(os+1)%no;redraw=1;sfx_menu();}
   if(pressed&(K_B|K_LEFT)){foc=0;redraw=1;sfx_menu();pressed=0;continue;}
   int op=ops[os];
   if(op==OP_BOTS&&(pressed&(K_A|K_RIGHT))){SET.bot=(SET.bot+1)&3;save_write();redraw=1;sfx_menu();continue;}
   if(pressed&(K_A|K_START)){sfx_ok();pressed=0;last[key]=sel;
    switch(op){case OP_PLAY:mo_begin();if(camp){for(int k=0;k<NCM;k++)if(CM[k].grp==sel){msel_jump=k;break;}mission_select();pressed=0;}else if(md>=MD_INF&&md<=MD_SEA){md_seen(md);if(faction_select(md))play_6v6(md);pressed=0;}else mode_play(md);mo_end(camp?MD_CAMP:md);bgCached=-1;bgm_t(C->trk);break;
     case OP_MAP:{int me=0;int mi=map_select(md,&me);if(mi>=0){mpPickI[md]=mi;mpPickE[md]=me;}else mpPickI[md]=-1;pressed=0;bgCached=-1;break;}
     case OP_SIDE:faction_select(md);pressed=0;bgCached=-1;break;
     case OP_LOAD:mode_loadout(md);break;case OP_REC:records(md,bgc);break;case OP_HOW:how_to(md,bgc);break;}
    redraw=1;}}}}
// ---- store front (v1.3.3 checkpoint: wallet + earn guide; the item catalogue lands in the next checkpoint)
static void store_screen(void){int redraw=1;bgm_t(TR_THEME);
 for(int t=0;;t++){BEAT->screen=106;BEAT->menu=0;mr_load();BEAT->scoreA=MR.bal;
  if(redraw){chrome(BG_EXTRA,2);banner("MARS ROCKS STORE","YOUR WALLET",RGB(220,100,50));rocks_chip(232,8);
   m3_fill(20,44,200,34,RGB(40,16,10));m3_box(20,44,200,34,0);m3_text(30,50,"BALANCE",RGB(255,190,120),1);m3_text(30,60,cat(itoa(MR.bal)," MARS ROCKS"),C_WHITE,2);
   m3_text(16,86,"EARNED SO FAR: ",C_GRAY,1);m3_text(106,86,itoa(MR.earned),C_WHITE,1);
   m3_ctext(100,"AIRCRAFT, WEAPONS, ROCKETS,",C_WHITE,1);m3_ctext(110,"SATELLITES AND 16 ALLIED SOF UNITS",C_WHITE,1);m3_ctext(120,"ARRIVE IN THE NEXT UPDATE.",C_YEL,1);
   m3_ctext(138,"FIREFIGHTER CALLS PAY ROCKS NOW",C_GRAY,1);m3_ctext(148,"B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&(K_A|K_B|K_START)){sfx_menu();pressed=0;return;}}}
static int cab_screen(void);
static void md_pseudo_open(int md){md_seen(md);
 switch(md){case MD_BOOT:tutorial(0);break;case MD_LOADOUT:{int era=0;loadout_hub(&era);break;}case MD_SETTINGS:settings_menu();break;case MD_CREDITS:credits_roll(0);break;
  case MD_CODES:ov_code();break;case MD_STORE:store_screen();break;case MD_CABINET:cab_screen();break;default:break;}
 pressed=0;bgCached=-1;}
// ---- main menu: 2 x 5 tiles in a wide translucent panel; the Earth, ISS and launches keep animating on the right
static void mm4_tile(int i,int on){int x=7+(i&1)*75,y=32+(i>>1)*21;
 if(!on){m3_fill(x,y,72,19,RGB(8,16,34));m3_fill(x,y,72,1,RGB(40,70,110));m3_fill(x,y+18,72,1,RGB(20,34,60));}
 ic_draw(TL[i].ic,x+3,y+4);m3_text(x+18,y+3,TL[i].n,on?C_YEL:C_WHITE,1);
 if(i==TI_STORE){mr_load();u32 b=MR.bal;if(b>99999)b=99999;m3_text(x+18,y+11,b<1000?cat(itoa(b)," ROCKS"):cat(itoa(b)," MR"),RGB(255,170,110),1);}
 else m3_text(x+18,y+11,TL[i].tag,on?RGB(170,210,236):RGB(120,140,170),1);
 if(tile_new(i)>=0){m3_fill(x+66,y+1,5,5,RGB(230,40,30));m3_px(x+68,y+3,RGB(255,200,190));}}
static int main_menu4(int sel){if(sel<0||sel>=TI_N)sel=0;
 mmPX1=156;bgCached=-1;mm_ls=0;mm_cs=0;mm_part=0;mmLn=0;mm_obj_init();mm_static();lfb=0;dma32(bigbuf,fb,240*160/2);vsync();dma32(bigbuf,MMV,240*160/2);mm_obj_frame();mm_obj_on();int dirty=1;
 int hx=(7+(sel&1)*75)<<4,hy=(32+(sel>>1)*21)<<4;
 for(;;){BEAT->screen=SCR_TILES;BEAT->menu=sel;
  next_frame();
  mm_obj_push();mm_restore();mm_stars();mm_kc(216,3);
  int tx=(7+(sel&1)*75)<<4,ty=(32+(sel>>1)*21)<<4;
  if(hx!=tx||hy!=ty){int dx=(tx-hx)/2,dy=(ty-hy)/2;if(!dx&&tx!=hx)dx=tx>hx?1:-1;if(!dy&&ty!=hy)dy=ty>hy?1:-1;hx+=dx;hy+=dy;if(iabs(tx-hx)<16&&iabs(ty-hy)<16){hx=tx;hy=ty;}dirty=1;}
  if(dirty){dirty=0;for(int y=MP_Y0;y<158;y++)dma32(&bigbuf[y*240+4],&fb[y*240+4],116);
   u16 CY=RGB(90,210,240),GD=RGB(236,196,92);
   m3_fill(4,MP_Y0,MP_X1-4,1,CY);m3_fill(4,MP_Y1-1,MP_X1-4,1,CY);m3_fill(4,MP_Y0,1,MP_Y1-MP_Y0,CY);m3_fill(MP_X1-1,MP_Y0,1,MP_Y1-MP_Y0,CY);
   m3_fill(4,MP_Y0,14,2,GD);m3_fill(MP_X1-15,MP_Y1-2,14,2,GD);m3_fill(4,MD_Y0,232,1,CY);m3_fill(4,157,232,1,CY);m3_fill(4,MD_Y0,10,2,GD);m3_fill(226,156,10,2,GD);
   for(int i=0;i<TI_N;i++)if(i!=sel)mm4_tile(i,0);
   {int x=hx>>4,y=hy>>4;m3_fill(x-1,y-1,74,21,CY);m3_fill(x,y,72,19,RGB(16,60,110));m3_fill(x,y,2,19,GD);}
   mm4_tile(sel,1);
   ltx(10,140,TL[sel].d1,RGB(200,220,236));{int nw=tile_new(sel);ltx(10,149,nw>=0?cat("NEW: ",MLN[nw]):TL[sel].d2,nw>=0?RGB(255,150,130):RGB(130,150,180));}
   for(int y=MP_Y0;y<158;y++)dma32(&fb[y*240+4],&MMV[y*240+4],116);}
  {static const u8 PR[11]={0,104,112,119,126,132,138,143,148,154,160};mm_earth_rows(PR[mm_part],PR[mm_part+1],1);if(++mm_part>=10){mm_part=0;mm_ls=(mm_ls+1)&511;if(frame&8)mm_cs=(mm_cs+1)&511;}}mm_obj_build();

  if(pressed&K_UP){sel=(sel+TI_N-2)%TI_N;sfx_menu();dirty=1;}
  if(pressed&K_DOWN){sel=(sel+2)%TI_N;sfx_menu();dirty=1;}
  if(pressed&(K_LEFT|K_RIGHT)){sel^=1;sfx_menu();dirty=1;}
  if(pressed&(K_A|K_START)){sfx_ok();bgCached=-1;mmPX1=122;mm_obj_off();return sel;}
  if(pressed&K_B){bgCached=-1;mmPX1=122;mm_obj_off();return -1;}}}
static void tile_open(int t){if(t==TI_STORE){store_screen();bgCached=-1;return;}cat_screen_p(&MC3[t],CAT_N+t,TL[t].bg);}
