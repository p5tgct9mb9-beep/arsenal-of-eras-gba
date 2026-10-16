// ===================================================================== v1.3 MENU: categories -> themed category screen -> per-mode hub
#define SCR_CAT 53
#define SCR_HUB 54
#define SCR_HOW 55
#define SCR_REC 56
#define SCR_OPT 57
enum{MD_CAMP,MD_INF,MD_TANK,MD_AIR,MD_SEA,MD_SWORD,MD_SPEC,MD_BRAD,MD_SNIPE,MD_EOD,MD_AA,MD_DRONE,MD_LAUNCH,MD_FLIGHT,MD_LRRP,MD_FPS,MD_SPACE,MD_ASTRO,MD_N};
static const char*const MDN[MD_N]={"CAMPAIGN","6V6 INFANTRY","6V6 ARMOR","6V6 DOGFIGHT","6V6 NAVAL","SWORD FIGHT 1V1","SPEC OPS","SPEC OPS: BRADLEY","SNIPER","EOD - BOMB DISPOSAL","ANTI-AIRCRAFT","DRONE OPS","SPACE LAUNCH","WORLD FLIGHT","LRRP","FPS","SPACE PROGRAM","ASTRONAUT EXPLORER"};
static const char*const MSN[MD_N+2]={"CAMPAIGN","6V6 INFANTRY","6V6 ARMOR","6V6 DOGFIGHT","6V6 NAVAL","SWORD FIGHT 1V1","SPEC OPS","BRADLEY OPS","SNIPER","EOD","ANTI-AIRCRAFT","DRONE OPS","SPACE LAUNCH","WORLD FLIGHT","LRRP","FPS","SPACE PROGRAM","ASTRONAUT EXPLORER"};
static const char*const MDD[MD_N]={"MISSIONS ACROSS EVERY ERA","PICK A FACTION, ANY-ERA ARSENAL","TANKS, TDS, IFVS, HALFTRACKS, JEEPS","AIR SUPERIORITY - GUNS + MISSILES","BATTLESHIPS, CARRIERS, DESTROYERS",
 "KNIGHT, SAMURAI, VIKING... BEST OF 3","PLATFORMER: WORLDS OF COURSES","M2 BRADLEY VEHICLE PLATFORMER","LONG RANGE: DROP, WIND, BREATH","FIND, APPROACH, DEFUSE",
 "GUNS + SAMS VS PLANES AND DRONES","MODERN UAV STRIKES - MQ-9, TB2","35 REAL ROCKETS: ASCENT, LAND, DOCK","FREE ROAM IN THE X-15, LANDMARKS","VIETNAM 1966-70: 6-MAN RECON TEAM","FIRST-PERSON: MISSIONS, FFA, SURVIVAL","REAL PROFILES, SATS, ISS DOCKING, EVA","PLANETS, MOONS, STARS: GRAVITY LAB"};
static const char*const HOW[MD_N][6]={
 {"PICK A MISSION FROM ANY ERA.","EACH HAS 1-3 OBJECTIVES SHOWN","AT THE TOP OF THE SCREEN.","FOLLOW THE MINIMAP MARKERS.","CLEARING A MISSION UNLOCKS THE NEXT.","SOME MISSIONS LET YOU PLAY BOTH SIDES"},
 {"A FIRE   B GRENADE   L SWAP","R STRAFE   SELECT TACTICAL","FIRST TEAM TO 30 KILLS WINS.","FACTIONS SET COLOURS + BOT KIT.","THE ARSENAL MIXES WEAPONS","FROM EVERY ERA."},
 {"UP/DOWN DRIVE  LEFT/RIGHT TURN","L/R ROTATE THE TURRET","A MAIN GUN   B MG / ATGM","SELECT CALLS YOUR SQUAD.","AIM FOR SIDES AND REAR ARMOUR.","FIRST TO 30 WINS."},
 {"UP FAST  DOWN SLOW","A GUNS   B ROCKETS / MISSILES","L/R BARREL ROLL TO DODGE","STAY ON A TARGET'S TAIL.","WATCH YOUR AMMO AND FUEL.","FIRST TO 30 WINS."},
 {"UP/DOWN ENGINE TELEGRAPH","LEFT/RIGHT HELM  L/R TURRETS","A MAIN GUNS  B SECONDARY","SHELLS ARC OVER ISLANDS.","CARRIERS LAUNCH AIR WINGS.","FIRST TO 30 SHIPS SUNK."},
 {"U+A HIGH  A MID  D+A LOW","B GUARD (HOLD)  L/R DODGE","GUARD THE RIGHT HEIGHT, THEN","COUNTER WHILE THEY RECOVER.","BEST OF THREE ROUNDS.","BOT DIFFICULTY SETS THE CPU."},
 {"B JUMP  A FIRE  L THROW","SELECT SWAPS WEAPONS","R USES YOUR UNIT ABILITY","3 INTEL PER COURSE, SECRET EXITS.","HISTORIC TAB: KNIGHTS, SAMURAI...","START IN UNIT SELECT = LOADOUT."},
 {"DRIVE LEFT/RIGHT  B HOP","A 25MM BUSHMASTER  UP/DN AIM","R TOW MISSILE (STEER UP/DN)","L SMOKE   SELECT DISMOUNTS","SPOT MINES + IEDS, SHOOT THEM.","TOW IS KING VS TANKS + BOSSES."},
 {"D-PAD MOVES THE SCOPE","R ZOOM   A FIRE   B HOLD BREATH","L RANGE FINDER - READ THE MILS","HOLD OVER FOR DROP, LEAD WIND.","THE SPOTTER CALLS CORRECTIONS.","COUNTER-SNIPERS GLINT - FIND THEM."},
 {"SWEEP THE DETECTOR OR SEND","THE ROBOT. WALK IN THE SUIT","SLOWLY. CUT THE RIGHT WIRES","BEFORE THE TIMER RUNS OUT -","OR BLOW IT IN PLACE.","WATCH FOR DECOYS + TRIPWIRES."},
 {"LEFT/RIGHT AIM  L/R FINE AIM","A GUNS (LEAD THE TARGET)","HOLD B TO LOCK, RELEASE = MISSILE","WATCH HEAT - OVERHEAT JAMS.","JETS DROP FLARES - GET CLOSE.","PROTECT THE BASE BUILDINGS."},
 {"PICK A DRONE, CALL-IN, MISSION.","A FIRE THE SELECTED WEAPON","L/R CYCLE WEAPONS","WATCH FOR SAMS AND MANPADS.","CONFIRM TARGETS - ROE MATTERS.","SCORE IS SHOWN AFTER EACH SORTIE."},
 {"PICK A REAL ROCKET + MISSION.","A THROTTLE  LEFT/RIGHT PITCH","STAGE WITH B AT BURNOUT.","LAND BOOSTERS ON THE PAD OR","DRONESHIP; DOCK WITH THE ISS.","GENTLE = MORE POINTS."},
 {"FLY THE X-15 OR OTHERS FREELY.","UP/DOWN PITCH  L/R ROLL","A THROTTLE  B BRAKE","VISIT LANDMARKS FOR STAMPS.","COURSES RACE THROUGH RINGS.","WATCH THE FUEL GAUGE."},
 {"MOVE SLOW - THE METER IS NOISE.","A ACT/FIRE  B PRONE  R SPRINT","L RADIO: FIRE MISSION, ADJUST","SELECT: SENSOR/CLAYMORE/STROBE","SNATCH FROM BEHIND. SPARE CIVS.","BREAK CONTACT, RUN FOR THE LZ."},
 {"UP/DOWN MOVE  LEFT/RIGHT TURN","L/R STRAFE   A FIRE","B OPENS DOORS / RELOADS","SELECT SWAPS WEAPON  START QUITS","WALK OVER DROPS TO TAKE WEAPONS.","3 RAIDS + DEATHMATCH VS BOTS."},
 {"PROFILES: HIT EACH CUE (A/SEL/L/R)","AT THE REAL T+ TIME. B = TIME WARP.","SATS: BURN PROGRADE AT PERIGEE TO","RAISE APOGEE. ISS: SLOW + STRAIGHT,","UNDER 0.25 M/S AT CONTACT. EVA:","CLIP YOUR TETHER BEFORE YOU MOVE."},
 {"PICK A WORLD: </> BODY, R WARP.","LAND GENTLY: UP = ENGINE, SEL CHUTE","SURFACE: SELECT PICKS EXPERIMENT,","A RUNS IT, UP JUMPS, DOWN SAMPLES.","JOURNAL (L) COMPARES VS EARTH 9.81","3 SAMPLES UNLOCK THE WARP DRIVE."}};
typedef struct{const char*n;const char*sub;u8 nm;u8 m[6];u8 trk;}MCat;
enum{CAT_CAMP,CAT_MP,CAT_SPEC,CAT_SPECIAL,CAT_EXPLORE,CAT_LOAD,CAT_SET,CAT_KC,CAT_N};
static const MCat MC[CAT_N]={
 {"CAMPAIGN","CAMPAIGN BY ERA",4,{0},TR_OMAHA},
 {"BATTLES / BOTS","6V6, 1V1 AND FPS VS BOTS",6,{MD_INF,MD_TANK,MD_AIR,MD_SEA,MD_SWORD,MD_FPS},TR_MARCH},
 {"SPEC OPS","SIDE-SCROLLING OPERATIONS",2,{MD_SPEC,MD_BRAD},TR_JUNGLE},
 {"SPECIALIST","ONE JOB. DO IT RIGHT.",5,{MD_SNIPE,MD_EOD,MD_LRRP,MD_AA,MD_DRONE},TR_COLD},
 {"EXPLORE","ROCKETS, SPACE + FLIGHT",4,{MD_SPACE,MD_ASTRO,MD_LAUNCH,MD_FLIGHT},TR_SPACE},
 {"LOADOUT/CHARACTER","KITS, ARSENAL, CHARACTER",0,{0},0},
 {"SETTINGS","MUSIC, VOLUME, DIFFICULTY",0,{0},0},
 {"BOOT CAMP WITH KC","CONTROLS + EVERY MODE",0,{0},0}};
// ---- themed backgrounds (procedural, Mode 3 into fb)
static void grad(int y0,int y1,u16 a,u16 b){for(int y=y0;y<y1;y++){int t=(y-y0)*256/(y1-y0>0?y1-y0:1);int r=((a&31)*(256-t)+(b&31)*t)>>8,g=(((a>>5)&31)*(256-t)+((b>>5)&31)*t)>>8,bl=(((a>>10)&31)*(256-t)+((b>>10)&31)*t)>>8;m3_fill(0,y,240,1,r|(g<<5)|(bl<<10));}}
static void mstars(int n,int ymax,u16 c){for(int i=0;i<n;i++)m3_fill((i*73+11)%240,(i*37+5)%ymax,1,1,c);}
static void cat_bg(int c){
 switch(c){
 case CAT_CAMP:{grad(0,160,RGB(196,170,120),RGB(150,120,80));for(int x=0;x<240;x+=20)m3_fill(x,0,1,160,RGB(170,144,100));for(int y=0;y<160;y+=20)m3_fill(0,y,240,1,RGB(170,144,100));
   for(int i=0;i<9;i++){int x=(i*53)%200+20,y=(i*31)%100+40;m3_fill(x,y,14,8,RGB(120,140,90));m3_fill(x+3,y-3,8,4,RGB(120,140,90));}
   for(int i=0;i<40;i++){m3_fill(40+i*4,120-i,3,2,RGB(170,40,30));}m3_fill(196,76,8,8,RGB(170,40,30));m3_fill(30,40,30,1,RGB(60,40,30));break;}
 case CAT_MP:{grad(0,110,RGB(60,40,70),RGB(240,140,70));m3_fill(200,30,14,14,RGB(255,220,150));grad(110,160,RGB(60,50,40),RGB(30,26,22));
   for(int x=0;x<240;x+=3){int h=6+((x*29)%11);m3_fill(x,110-h,3,h,RGB(40,30,36));}
   for(int k=0;k<3;k++){int x=20+k*80,y=104;m3_fill(x,y,40,8,RGB(24,20,24));m3_fill(x+10,y-6,16,6,RGB(24,20,24));m3_fill(x+26,y-4,22,2,RGB(24,20,24));}
   m3_fill(150,40,24,3,RGB(30,24,30));m3_fill(160,36,4,11,RGB(30,24,30));m3_fill(150,38,3,4,RGB(30,24,30));break;}
 case CAT_SPEC:{grad(0,160,RGB(10,20,40),RGB(30,60,60));mstars(40,80,RGB(200,210,230));for(int y=-12;y<=12;y++)for(int x=-12;x<=12;x++)if(x*x+y*y<=144)m3_fill(184+x,34+y,1,1,RGB(240,240,210));
   for(int k=0;k<6;k++){int x=10+k*42,h=60+(k*17)%30;m3_fill(x+8,160-h,4,h,RGB(10,24,16));for(int j=-3;j<=3;j++)m3_fill(x+10+j*5-(j<0?6:0),160-h-2+iabs(j)*2,10,2,RGB(10,30,18));}
   m3_fill(0,140,240,20,RGB(8,20,12));break;}
 case CAT_SPECIAL:{grad(0,160,RGB(8,10,24),RGB(40,36,70));mstars(30,60,RGB(180,190,220));for(int x=0;x<240;x+=12){int h=30+((x*37)%50);m3_fill(x,160-h,11,h,RGB(16,18,30));for(int w=0;w<h-6;w+=7)if(((x+w)*13)%5==0)m3_fill(x+3,160-h+4+w,3,2,RGB(240,210,110));}
   for(int a=0;a<360;a+=2){int x=180+(COS(deg2a(a))*30>>8),y=60+(SIN(deg2a(a))*30>>8);m3_fill(x,y,2,2,RGB(200,40,40));}m3_fill(150,60,60,1,RGB(200,40,40));m3_fill(180,30,1,60,RGB(200,40,40));break;}
 default:{m3_fill(0,0,240,160,RGB(4,6,16));mstars(80,160,RGB(220,220,255));for(int y=110;y<160;y++){int w=(y-110)*4+40;m3_fill(120-w,y,2*w,1,y<114?RGB(120,180,255):RGB(30,70,150));}
   m3_fill(60,40,4,30,RGB(230,230,236));m3_fill(59,70,6,4,RGB(200,80,40));for(int i=0;i<6;i++)m3_fill(60+(i&1),74+i*3,2+(i&1),3,RGB(255,190-i*20,80));break;}}}
static int bgCached=-1;
static void chrome(int c,int lay){int key=c*4+lay;if(bgCached==key){dma32(dl_bg,fb,240*160/2);return;}cat_bg(c);
 if(lay==1){m3_fill(0,2,240,22,RGB(24,36,90));m3_box(0,2,240,22,2);m3_box(6,30,128,108,0);m3_box(138,30,96,108,0);m3_fill(0,140,240,20,RGB(10,12,24));}
 else if(lay==2)m3_box(8,40,224,112,0);
 dma32(fb,dl_bg,240*160/2);bgCached=key;}
static void cat_bgc(int c){chrome(c,0);}
static void banner(const char*t,const char*sub,u16 c){m3_fill(0,4,240,30,RGB(10,12,24));m3_fill(0,4,240,1,c);m3_fill(0,33,240,1,c);m3_ctext(8,t,C_YEL,2);if(sub)m3_ctext(24,sub,C_GRAY,1);}
// ---- stubs for modes still being built
static void bradley_ops(void);static void sniper_ops(void);static void eod_ops(void);static void lrrp_ops(void);static void fps_ops(void);static void space_program(void);static void astro_explorer(void);
static int mode_avail(int md){(void)md;return 1;}
// ---- per-mode pages
static void how_to(int md,int c){int redraw=1;for(;;){BEAT->screen=SCR_HOW;BEAT->mode=md;
  if(redraw){chrome(c,2);banner(MDN[md],"HOW TO PLAY",C_YEL);for(int i=0;i<6;i++)m3_text(16,50+i*13,HOW[md][i],C_WHITE,1);m3_ctext(138,"B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&(K_A|K_B|K_START)){sfx_menu();pressed=0;return;}}}
static void rec_brad(const char**L);static void rec_snipe(const char**L);static void rec_eod(const char**L);static void rec_lrrp(const char**L);static void rec_space(const char**L);static void rec_astro(const char**L);static void rec_fps(const char**L);
static void rec_lines(int md,const char**L){static char b[5][40];for(int i=0;i<5;i++)L[i]="";
 #define SB(k,s) {const char*_s=(s);int _n=0;while(_s[_n]&&_n<38){b[k][_n]=_s[_n];_n++;}b[k][_n]=0;L[k]=b[k];}
 switch(md){
 case MD_CAMP:{int n=0;for(int i=0;i<NCM;i++)n+=(campUnlock>>i)&1;SB(0,cat3("MISSIONS UNLOCKED: ",itoa(n),cat("/",itoa(NCM))));L[1]="CLEAR A MISSION TO OPEN THE NEXT.";break;}
 case MD_SPEC:{ps_read();SB(0,cat3("COURSES CLEARED: ",itoa(total_clear()),""));SB(1,cat3("INTEL FOUND: ",itoa(total_intel()),""));L[2]="31 PLAYABLE UNITS";break;}
 case MD_AA:{aa_load();static const char*const EN[4]={"WW1     ","WW2     ","COLD WAR","MODERN  "};for(int e=0;e<4;e++){char*o=b[e];int k=0;const char*p[5]={EN[e],"  10-WAVE ",itoa(aaBest[e][0]),"  ENDLESS ",itoa(aaBest[e][1])};for(int j=0;j<5;j++)for(const char*q=p[j];*q&&k<38;q++)o[k++]=*q;o[k]=0;L[e]=o;}L[4]="BEST SCORE PER ERA AND MODE";break;}
 case MD_BRAD:rec_brad(L);break;
 case MD_SNIPE:rec_snipe(L);break;
 case MD_EOD:rec_eod(L);break;
 case MD_LRRP:rec_lrrp(L);break;
 case MD_SPACE:rec_space(L);break;
 case MD_ASTRO:rec_astro(L);break;
 case MD_FPS:rec_fps(L);break;
 default:L[0]="NO SAVED RECORDS FOR THIS MODE YET.";L[1]="RESULTS ARE SHOWN AFTER EACH MATCH.";}
 #undef SB
}
static void records(int md,int c){const char*L[5];rec_lines(md,L);int redraw=1;for(;;){BEAT->screen=SCR_REC;BEAT->mode=md;
  if(redraw){chrome(c,2);banner(MDN[md],"HIGH SCORES / PROGRESS",C_YEL);for(int i=0;i<5;i++)m3_text(16,54+i*14,L[i],i==0?C_YEL:C_WHITE,1);m3_ctext(138,"B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&(K_A|K_B|K_START)){sfx_menu();pressed=0;return;}}}
static void mode_options(int md,int c){int sel=0,redraw=1;int n=md>=MD_INF&&md<=MD_SEA?3:2;
 for(;;){BEAT->screen=SCR_OPT;BEAT->mode=md;BEAT->menu=sel;
  if(redraw){static const char*const BN[4]={"EASY","NORMAL","HARD","ELITE"};chrome(c,2);banner(MDN[md],"OPTIONS",C_YEL);
   const char*R[3]={cat("BOT DIFFICULTY: ",BN[SET.bot&3]),SET.hints?"BUTTON HINTS: ON":"BUTTON HINTS: OFF","FACTIONS..."};
   static char r0[32];{int k=0;const char*s=R[0];while(*s&&k<30)r0[k++]=*s++;r0[k]=0;R[0]=r0;}
   for(int i=0;i<n;i++)m3_text(24,58+i*16,cat(i==sel?">":" ",R[i]),i==sel?C_YEL:C_WHITE,1);
   m3_ctext(124,"LEFT/RIGHT: CHANGE   A: OPEN",C_GRAY,1);m3_ctext(138,"B: BACK (SAVES)",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&K_UP){sel=(sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&(K_LEFT|K_RIGHT|K_A)){if(sel==0){SET.bot=(SET.bot+((pressed&K_LEFT)?3:1))&3;}else if(sel==1)SET.hints^=1;else if(pressed&K_A){faction_select(md);pressed=0;bgCached=-1;}redraw=1;sfx_menu();}
  if(pressed&(K_B|K_START)){save_write();sfx_ok();pressed=0;return;}}}
static void mode_play(int md){
 switch(md){
 case MD_CAMP:mission_select();break;
 case MD_INF:case MD_TANK:case MD_AIR:case MD_SEA:{int sel=md;if(faction_select(sel)){if(arsenal_menu(sel)){int me=0;int mi=map_select(sel,&me);if(mi>=0){loading(cat("DEPLOYING: ",map_def(sel,me,mi)->n));mpMixed=1;mp_run(sel,me,mi);mpMixed=0;}}}break;}
 case MD_SWORD:sword_fight();break;
 case MD_SPEC:spec_ops();break;
 case MD_BRAD:bradley_ops();break;
 case MD_SNIPE:sniper_ops();break;
 case MD_EOD:eod_ops();break;
 case MD_LRRP:lrrp_ops();break;
 case MD_FPS:fps_ops();break;
 case MD_AA:anti_air();break;
 case MD_DRONE:{static int dm=0;if(drone_hangar(&dm)){loading(cat("LAUNCHING: ",DRN[droneSel].n));drone_run(dm);}break;}
 case MD_LAUNCH:space_launch();break;
 case MD_SPACE:space_program();break;
 case MD_ASTRO:astro_explorer();break;
 case MD_FLIGHT:flight_menu();break;}
 pressed=0;bgCached=-1;}
static void mode_loadout(int md){int era=0;if(md==MD_SPEC){ps_read();so_read();so_menu(PS.unit);}else loadout_hub(&era);pressed=0;bgCached=-1;}
// ---- category screen (mockup layout): title bar, mode list left, options panel right
static s8 mpPickI[5]={-1,-1,-1,-1,-1},mpPickE[5];
enum{OP_PLAY,OP_MAP,OP_SIDE,OP_BOTS,OP_LOAD,OP_REC,OP_HOW};
static int mode_ops(int md,u8*o){int n=0;o[n++]=OP_PLAY;if(md>=MD_INF&&md<=MD_SEA){o[n++]=OP_MAP;o[n++]=OP_SIDE;}
 if(md!=MD_LAUNCH&&md!=MD_FLIGHT&&md!=MD_CAMP&&md!=MD_SPACE&&md!=MD_ASTRO)o[n++]=OP_BOTS;if(md!=MD_CAMP&&md!=MD_SPACE&&md!=MD_ASTRO&&md!=MD_FPS)o[n++]=OP_LOAD;o[n++]=OP_REC;o[n++]=OP_HOW;return n;}
static const char*op_label(int md,int op){static char b[24];static const char*const BN[4]={"EASY","NORMAL","VETERAN","ELITE"};const char*v=0;const char*k="";
 switch(op){case OP_PLAY:return "PLAY";case OP_LOAD:return "LOADOUT";case OP_REC:return md==MD_CAMP?"PROGRESS":"HIGH SCORES";case OP_HOW:return "HOW TO PLAY";
 case OP_MAP:k="MAP: ";v=mpPickI[md]<0?"PICK":map_def(md,mpPickE[md],mpPickI[md])->n;break;
 case OP_SIDE:k="SIDE: ";v=FAC[g6.fac0].n;break;case OP_BOTS:k="BOTS: ";v=BN[SET.bot&3];break;}
 int n=0;while(*k)b[n++]=*k++;while(*v&&n<14)b[n++]=*v++;b[n]=0;return b;}
static void play_6v6(int md){if(!arsenal_menu(md))return;int me=mpPickE[md],mi=mpPickI[md];if(mi<0){me=0;mi=map_select(md,&me);}if(mi>=0){loading(cat("DEPLOYING: ",map_def(md,me,mi)->n));mpMixed=1;mp_run(md,me,mi);mpMixed=0;}}
static void cat_screen(int c){const MCat*C=&MC[c];static const char*const EN[4]={"WORLD WAR II","COLD WAR","MODERN","WHAT-IF"};static const char*const ED[4]={"1942-1945: MIDWAY TO OMAHA","1950-1985: KOREA, VIETNAM, FULDA","1993-2026","FICTIONAL SCENARIOS"};
 int camp=c==CAT_CAMP;int nm=camp?4:C->nm;static u8 last[CAT_N];int sel=last[c]<nm?last[c]:0,foc=0,os=0,redraw=1;bgm_t(C->trk);
 for(;;){int md=camp?MD_CAMP:C->m[sel];u8 ops[8];int no=mode_ops(md,ops);if(os>=no)os=no-1;
  BEAT->screen=foc?SCR_HUB:SCR_CAT;BEAT->mode=camp?0:md;BEAT->menu=sel;BEAT->phase=os;
  if(redraw){chrome(c,1);m3_ctext(6,camp?"CAMPAIGN":C->n,C_YEL,2);
   for(int i=0;i<nm;i++){const char*t=camp?EN[i]:MDN[C->m[i]];int y=40+i*(nm>5?15:17);m3_text(14,y,i==sel?">":" ",C_YEL,1);m3_text(20,y,camp?t:MSN[C->m[i]],i==sel?C_YEL:(camp||mode_avail(C->m[i])?C_WHITE:C_GRAY),1);}
   for(int i=0;i<no;i++){int y=38+i*13;int hi=foc&&i==os;m3_text(144,y,hi?">":" ",C_GREEN,1);m3_text(150,y,op_label(md,ops[i]),hi?C_GREEN:(i==0&&!foc?C_GREEN:C_WHITE),1);}
   if(!camp&&!mode_avail(md))m3_text(144,38+no*13,"IN TRAINING",C_RED,1);
   m3_ctext(142,camp?ED[sel]:MDD[md],C_GRAY,1);m3_ctext(151,foc?"A: SELECT   B: BACK TO LIST":"A: OPTIONS   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(!foc){if(pressed&K_UP){sel=(sel+nm-1)%nm;os=0;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%nm;os=0;redraw=1;sfx_menu();}
   if(pressed&(K_A|K_RIGHT|K_START)){foc=1;os=0;redraw=1;sfx_ok();}if(pressed&K_B){sfx_menu();pressed=0;last[c]=sel;return;}}
  else{if(pressed&K_UP){os=(os+no-1)%no;redraw=1;sfx_menu();}if(pressed&K_DOWN){os=(os+1)%no;redraw=1;sfx_menu();}
   if(pressed&(K_B|K_LEFT)){foc=0;redraw=1;sfx_menu();pressed=0;continue;}
   int op=ops[os];
   if(op==OP_BOTS&&(pressed&(K_A|K_RIGHT))){SET.bot=(SET.bot+1)&3;save_write();redraw=1;sfx_menu();continue;}
   if(pressed&(K_A|K_START)){sfx_ok();pressed=0;
    switch(op){case OP_PLAY:if(camp){for(int k=0;k<NCM;k++)if(CM[k].grp==sel){msel_jump=k;break;}mission_select();pressed=0;}else if(md>=MD_INF&&md<=MD_SEA){if(faction_select(md))play_6v6(md);pressed=0;}else mode_play(md);bgCached=-1;bgm_t(C->trk);break;
     case OP_MAP:{int me=0;int mi=map_select(md,&me);if(mi>=0){mpPickI[md]=mi;mpPickE[md]=me;}else mpPickI[md]=-1;pressed=0;bgCached=-1;break;}
     case OP_SIDE:faction_select(md);pressed=0;bgCached=-1;break;
     case OP_LOAD:mode_loadout(md);break;case OP_REC:records(md,c);break;case OP_HOW:how_to(md,c);break;}
    redraw=1;}}}}
static void category(int c){cat_screen(c);}
static int main_menu2(int sel){int redraw=1;static const char*const MN[CAT_N]={"CAMPAIGN","BATTLES","SPEC OPS","SPECIALIST","EXPLORE","LOADOUT","SETTINGS","BOOT CAMP (KC)"};
 static const char*const L1[CAT_N]={"WW2 - COLD WAR - MODERN","6V6 INFANTRY - ARMOR - DOGFIGHT","SPEC OPS PLATFORMER","SNIPER - EOD - LRRP","SPACE LAUNCH","KITS - 6V6 ARSENAL - CHARACTER","MUSIC - VOLUME - DIFFICULTY","CONTROLS + EVERY MODE"};
 static const char*const L2[CAT_N]={"WHAT-IF SCENARIOS","NAVAL - SWORD FIGHT 1V1 - FPS","SPEC OPS: BRADLEY","ANTI-AIRCRAFT - DRONE OPS","WORLD FLIGHT","","BOT SKILL - HINTS","WITH KC THE DOG"};
 for(;;){BEAT->screen=SCR_MENU;BEAT->menu=sel;
  if(redraw){m3_title(0);m3_fill(10,82,220,76,C_PANEL);m3_box(10,82,220,76,2);
   for(int k=0;k<CAT_N;k++){int col=k/4,row=k%4;m3_text(18+col*104,88+row*11,k==sel?">":" ",C_YEL,1);m3_text(24+col*104,88+row*11,MN[k],k==sel?C_YEL:C_WHITE,1);}
   m3_fill(14,133,212,1,RGB(236,196,92));m3_text(18,137,L1[sel],C_GRAY,1);m3_text(18,146,L2[sel],C_GRAY,1);redraw=0;m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+CAT_N-1)%CAT_N;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%CAT_N;redraw=1;sfx_menu();}
  if(pressed&(K_LEFT|K_RIGHT)){sel=(sel+4)%8;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();return sel;}
  if(pressed&K_B)return -1;}}
// temporary entry points for modes in development
static void wip(const char*n){int redraw=1;for(;;){if(redraw){cat_bgc(CAT_SPECIAL);banner(n,"IN TRAINING",C_YEL);m3_box(8,60,224,50,1);m3_ctext(72,"THIS MODE IS STILL IN BOOT CAMP.",C_WHITE,1);m3_ctext(88,"B: BACK",C_GRAY,1);m3_present();redraw=0;}next_frame();if(pressed&(K_A|K_B)){pressed=0;return;}}}
#ifndef HAVE_BRADLEY
#endif
#ifndef HAVE_SNIPER
#endif
#ifndef HAVE_FPS

#endif
