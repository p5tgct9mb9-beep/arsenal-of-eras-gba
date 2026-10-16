// ===================================================================== v1.3.4 ARCADE CABINET: 12 original mini-games
// Each game: init / step(held,pressed) -> 1 when over / draw / ai (autopilot used by attract mode + headless tests).
// Records: SRAM 0x3400 'Z' (5-entry 3-letter tables per game + play counts). Jump code 0x4147000g plays game g (tests).
#define SCR_CAB 107
#define SCR_AG 108
#define SCR_AGOVER 109
enum{AG_DOME,AG_SWEEP,AG_CROSS,AG_MAZE,AG_BUST,AG_STACK,AG_CONVOY,AG_CHOP,AG_HOP,AG_PONG,AG_DUEL,AG_PIN,AG_N};
static const char*const AGNM[AG_N]={"IRON DOME","DEBRIS SWEEPER","KC CROSSING","BUNKER MAZE","BUNKER BUSTER","CARGO STACK","CONVOY","CHOPPER RESCUE","STARSHIP HOPPER","TANK PONG","TANK DUEL","SPACE PINBALL"};
static const char*const AGD[AG_N][2]={
 {"STOP ROCKETS + DRONES OVER THE BASES","D-PAD AIM  A LAUNCH INTERCEPTOR"},
 {"CLEAR SPACE JUNK IN A DRAGON CAPSULE","L/R TURN  UP THRUST  A FIRE"},
 {"GET KC ACROSS ROADS, RIVER, MINES","D-PAD HOP ONE SQUARE"},
 {"EAT TREATS, DODGE PATROL ROBOTS","D-PAD MOVE  BONES = CHASE THEM"},
 {"BOUNCE SHELLS TO BREAK THE FORT","L/R MOVE TANK  A LAUNCH SHELL"},
 {"PACK CRATES INTO THE C-17 BAY","L/R MOVE  A TURN  DOWN DROP"},
 {"GROW THE TRUCK CONVOY WITH SUPPLIES","D-PAD STEER - DON'T HIT YOURSELF"},
 {"FLY A BLACK HAWK, BRING HOSTAGES HOME","D-PAD FLY  A FIRE  B TURN  LAND TO LOAD"},
 {"HOP THE PROTOTYPE THROUGH THE TOWERS","A: ENGINE BURST"},
 {"TANK PADDLES + A RICOCHET SHELL","UP/DOWN MOVE  HOLD A = POWER RETURN"},
 {"1V1 TANK DUEL IN THE MAZE VS CPU","L/R TURN  UP/DN DRIVE  A FIRE"},
 {"ROCKET TABLE: RAMPS + MARS MULTIBALL","L / R FLIPPERS  HOLD DOWN = PLUNGER"}};
static const u32 AG_SEED[AG_N]={3000,4000,1500,3000,2500,2000,600,2000,200,700,500,30000};
static const u16 AG_RDIV[AG_N]={100,120,60,120,100,80,25,80,8,30,25,1500}; // score per Mars Rock
typedef struct{HiE t[AG_N][5];u16 plays[AG_N];}ArcSave;static ArcSave AS EWRAM;static int as_ld;
static void as_save(void){sv_put(0x3400,'Z',1,&AS,sizeof AS);}
static void as_load(void){if(as_ld)return;as_ld=1;if(sv_get(0x3400,'Z',1,&AS,sizeof AS))return;memset(&AS,0,sizeof AS);
 static const char*const N[5]={"KC ","LT ","WVB","DAD","ACE"};for(int g=0;g<AG_N;g++){u32 S[5];for(int i=0;i<5;i++)S[i]=AG_SEED[g]*(5-i)/5;hi_seed(AS.t[g],N,S);}}
// ---- shared state + drawing helpers
static int agScore,agLives,agLvl,agT,agAttract;
static void a_clear(u16 c){fill32(fb,c|(c<<16),240*160/2);}
static void a_rect(int x,int y,int w,int h,u16 c){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w<=0||h<=0)return;
 if(w<8){for(int j=0;j<h;j++){u16*p=&fb[(y+j)*240+x];for(int i=0;i<w;i++)p[i]=c;}return;}
 if(x==0&&w==240){fill32(&fb[y*240],c|(c<<16),h*120);return;}
 u32 cc=c|(c<<16);for(int j=0;j<h;j++){u16*p=&fb[(y+j)*240+x];int ww=w;if(((u32)p)&2){*p++=c;ww--;}fill32(p,cc,ww>>1);if(ww&1)p[ww-1]=c;}}
static void a_disc(int cx,int cy,int r,u16 c){for(int dy=-r;dy<=r;dy++){int w=0;while((w+1)*(w+1)+dy*dy<=r*r)w++;a_rect(cx-w,cy+dy,2*w+1,1,c);}}
static void a_ring(int cx,int cy,int r,u16 c){for(int a=0;a<256;a+=4)m3_px(cx+(SIN(a)*r>>8),cy-(COS(a)*r>>8),c);}
static void a_line(int x0,int y0,int x1,int y1,u16 c){int dx=iabs(x1-x0),dy=-iabs(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,e=dx+dy;
 for(int n=0;n<400;n++){m3_px(x0,y0,c);if(x0==x1&&y0==y1)break;int e2=2*e;if(e2>=dy){e+=dy;x0+=sx;}if(e2<=dx){e+=dx;y0+=sy;}}}
static int a_ang(int dx,int dy){ // 0 = up, clockwise, 256 units
 int best=0;s32 bd=-0x7FFFFFFF;for(int a=0;a<256;a+=2){s32 d=dx*SIN(a)-dy*COS(a);if(d>bd){bd=d;best=a;}}return best;}
static int a_adiff(int a,int b){return (s8)(u8)(a-b);}
static void a_show(void){next_frame();dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);}
static void a_hud(int g){a_rect(0,0,240,10,RGB(0,0,0));m3_text(2,2,AGNM[g],RGB(90,210,240),1);m3_text(120,2,itoa(agScore),C_YEL,1);
 for(int i=0;i<agLives&&i<6;i++)a_rect(200+i*6,3,4,4,RGB(230,60,50));}
// autopilot helper: turn a desired direction into held keys
static u16 aiH,aiP;
#include "ag1.inc"
#include "ag2.inc"
#include "ag3.inc"
typedef struct{void(*init)(void);int(*step)(u16,u16);void(*draw)(void);void(*ai)(void);u8 trk;}AGame;
static const AGame AGS[AG_N]={{dome_init,dome_step,dome_draw,dome_ai,TR_AG0},{sw_init,sw_step,sw_draw,sw_ai,TR_AG1},{kx_init,kx_step,kx_draw,kx_ai,TR_AG2},{bm_init,bm_step,bm_draw,bm_ai,TR_AG3},
 {bk_init,bk_step,bk_draw,bk_ai,TR_AG4},{cs_init,cs_step,cs_draw,cs_ai,TR_AG5},{cv_init,cv_step,cv_draw,cv_ai,TR_AG6},{ch_init,ch_step,ch_draw,ch_ai,TR_AG7},
 {sh_init,sh_step,sh_draw,sh_ai,TR_AG8},{tp_init,tp_step,tp_draw,tp_ai,TR_AG9},{td_init,td_step,td_draw,td_ai,TR_AG10},{pn_init,pn_step,pn_draw,pn_ai,TR_AG11}};
// ---- pause: A resume, B quit (ends the run with the current score)
static int a_pause(int g){m3_fill(60,56,120,48,RGB(10,12,24));m3_box(60,56,120,48,0);m3_ctext(64,"PAUSED",C_YEL,2);m3_ctext(86,"A: RESUME  B: QUIT",C_WHITE,1);(void)g;
 dma32(fb,(void*)0x06000000,240*160/2);for(;;){next_frame();if(pressed&(K_A|K_START))return 0;if(pressed&K_B)return 1;}}
// ---- game over: payout, initials, top-5, share code, fast restart
static int arc_over(int g,int sc){as_load();AS.plays[g]++;int rocks=mr_add(clampi(3+sc/AG_RDIV[g],3,60));
 int rk=sc>0?hi_rank(AS.t[g],sc):-1;if(rk>=0){char nm[4]="LTV";hi_initials(nm,cat("NEW HIGH SCORE ",itoa(sc)));hi_put(AS.t[g],rk,sc,nm);}as_save();
 if(rk>=0)music_jingle(TR_CLEAR);else music_defeat();
 a_clear(RGB(6,8,20));m3_box(14,8,212,144,0);m3_ctext(14,AGNM[g],RGB(90,210,240),1);m3_ctext(26,"GAME OVER",C_YEL,2);
 m3_ctext(46,cat("SCORE ",itoa(sc)),C_WHITE,1);m3_ctext(57,cat3("+",itoa(rocks)," MARS ROCKS"),RGB(255,170,110),1);
 for(int i=0;i<5;i++){int y=72+i*11;u16 c=i==rk?C_YEL:C_WHITE;if(i==rk)a_rect(50,y-2,140,10,RGB(40,60,110));m3_text(60,y,itoa(i+1),c,1);m3_text(76,y,AS.t[g][i].n,c,1);m3_text(120,y,itoa(AS.t[g][i].s),c,1);}
 if(rk>=0)m3_ctext(128,cat("SHARE CODE ",hi_code(100+g,sc,AS.t[g][rk].n)),C_GREEN,1);
 m3_ctext(140,"A: PLAY AGAIN    B: CABINET",C_GRAY,1);dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);
 for(int t=0;;t++){BEAT->screen=SCR_AGOVER;BEAT->mode=g;BEAT->scoreA=sc;BEAT->scoreB=rk;BEAT->kills=rocks;BEAT->hp=AS.t[g][0].s;BEAT->extra=AS.plays[g];next_frame();
  if(FR_AUTO&&t>120)return 0;if(t>20&&(pressed&(K_A|K_START))){sfx_ok();return 1;}if(t>20&&(pressed&K_B)){sfx_menu();return 0;}}}
static void arc_play(int g){const AGame*G=&AGS[g];
 for(;;){u32 lastV=vblN;bgm_t(G->trk);agScore=0;agLives=3;agLvl=0;agT=0;agAttract=0;G->init();int quit=0;
  for(;;){BEAT->screen=SCR_AG;BEAT->mode=g;BEAT->scoreA=agScore;BEAT->hp=agLives;BEAT->phase=agLvl;BEAT->timer=agT;
   if(pressed&K_START){if(a_pause(g)){quit=1;break;}lastV=vblN;continue;}
   // fixed 60 Hz logic: run one step per elapsed vblank (max 3) so a slow full redraw never slows the game down
   int n=(int)(vblN-lastV);lastV=vblN;if(n<1)n=1;if(n>3)n=3;int over=0;
   for(int k=0;k<n&&!over;k++){u16 h=keys,p=k?0:pressed;if(FR_AUTO){aiH=0;aiP=0;G->ai();h=aiH;p=aiP;}agT++;over=G->step(h,p);}
   if(over||(FR_AUTO&&agT>60*60*4))break; // tests: 4 min cap
   G->draw();kl_poll();a_hud(g);a_show();}
  (void)quit;music_stop();if(!arc_over(g,agScore))break;}
 pressed=0;bgCached=-1;}
// ---- cabinet screen: marquee, live attract preview (half scale), description + controls, top score
static void cab_frame(int g){a_clear(RGB(10,6,20));for(int y=0;y<160;y+=6)a_rect(0,y,240,1,RGB(16,10,30));
 a_rect(46,2,148,122,RGB(40,30,70));a_rect(48,4,144,118,RGB(70,40,110));a_rect(48,4,144,22,RGB(20,10,30));
 for(int i=0;i<144;i+=8)a_rect(48+i,4,4,2,(i/8+frame/8)&1?RGB(255,200,60):RGB(220,60,200));
 {const char*n=AGNM[g];int w=slen(n)*6;m3_text(120-w/2,13,n,C_YEL,1);}
 a_rect(54,32,132,88,RGB(0,0,0));a_rect(38,124,164,3,RGB(40,30,70));
 m3_text(8,56,"<",C_YEL,1);m3_text(226,56,">",C_YEL,1);}
static int cab_screen(void){as_load();static int g=0;int ag=-1,cool=6;bgm_t(TR_INVADE);
 for(int t=0;;t++){BEAT->screen=SCR_CAB;BEAT->menu=g;
  {u32 j=FR_JUMP;if((j&0xFFFF0000)==0x41470000){FR_JUMP=0;g=(j&0xFF)%AG_N;arc_play(g);ag=-1;cool=30;kln=0;pressed=0;bgm_t(TR_INVADE);continue;}}
  if(ag!=g){ag=g;rs=0x1234567u+g*977;agScore=0;agLives=3;agLvl=0;agT=0;agAttract=1;AGS[g].init();}
  // one attract step drawn full-screen, then shrunk 2:1 into the cabinet screen via bigbuf
  aiH=aiP=0;AGS[g].ai();agT++;if(AGS[g].step(aiH,aiP)||agT>60*90){agScore=0;agLives=3;agLvl=0;agT=0;AGS[g].init();}AGS[g].draw();a_hud(g);kl_poll();
  for(int y=0;y<80;y++){u16*s=&fb[y*2*240];u16*d=&bigbuf[y*128];for(int x=0;x<120;x++)d[x]=s[x*2];}
  kl_poll();cab_frame(g);kl_poll();for(int y=0;y<80;y++){u16*s=&bigbuf[y*128];u16*d=&fb[(36+y)*240+60];for(int x=0;x<120;x++)d[x]=s[x];}
  m3_text(4,36,cat3(itoa(g+1),"/",itoa(AG_N)),C_GRAY,1);
  m3_text(198,36,"HI",C_GRAY,1);m3_text(198,46,AS.t[g][0].n,C_YEL,1);m3_text(198,66,itoa(AS.t[g][0].s),C_WHITE,1);
  m3_text(4,80,"PLAYS",C_GRAY,1);m3_text(4,90,itoa(AS.plays[g]),C_WHITE,1);
  m3_ctext(130,AGD[g][0],C_WHITE,1);m3_ctext(141,AGD[g][1],RGB(120,220,255),1);
  m3_ctext(152,"< > CHOOSE   A PLAY   B BACK",C_GRAY,1);
  a_show();if(cool){cool--;pressed=0;}
  if(pressed&(K_LEFT|K_L)){g=(g+AG_N-1)%AG_N;sfx_menu();}
  if(pressed&(K_RIGHT|K_R)){g=(g+1)%AG_N;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();pressed=0;arc_play(g);ag=-1;cool=30;kln=0;pressed=0;bgm_t(TR_INVADE);}
  if(pressed&K_B){sfx_menu();pressed=0;bgCached=-1;return 0;}}}
