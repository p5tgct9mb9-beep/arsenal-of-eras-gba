"""dscontrols.py - DS-only source patches for the native DSi button layout (used by port.py).

The physical-button translation lives in source/controls.c.  These literal replacements
(1) widen the key poll to the two DS-only bits K_DSX/K_DSY, (2) add the DS-only actions that
have no GBA key (infantry manual reload + quick swap, armor/naval smoke screen, aircraft flares),
(3) turn the GBA 'RUMBLE N/A' settings row into SETTINGS > CONTROLS (DS / CLASSIC (GBA)), and
(4) rewrite the on-screen control hints, HOW TO PLAY and tutorial text to the A/B/X/Y/L/R layout.
The GBA sources in ../src are never modified.
"""
DECOY = ('static u32 dsDecoyEnd,dsDecoyReady;static int ds_decoy_on(void){return (s32)(dsDecoyEnd-frame)>0;}\n'
 'static void ds_decoy(Unit*u,int fl){if(!u->isp)return;if((s32)(dsDecoyReady-frame)>0){feed(fl?"FLARES RELOADING":"SMOKE RELOADING",HB_RED);return;}\n'
 ' dsDecoyEnd=frame+(fl?75:240);dsDecoyReady=frame+(fl?300:720);int x=u->x>>8,y=u->y>>8,a=u->ang>>8;\n'
 ' if(fl){for(int k=0;k<6;k++){int b=a+128+((k&1)?36:-36);fx_add(x+(SIN(b)*(8+k*3)>>8),y-(COS(b)*(8+k*3)>>8),FXK_FLASH);}feed("FLARES - LOCKS BROKEN",HB_YEL);}\n'
 ' else{smoke_add(x+(SIN(a)*24>>8),y-(COS(a)*24>>8));smoke_add(x,y);feed("SMOKE SCREEN - LOCKS BROKEN",HB_YEL);}sfx_whoosh();}\n')
CODE = [
 # key poll: keep the DS-only X/Y bits
 ('static inline void kl_poll(void){u16 k=~REG_KEYINPUT&0x3FF,', 'static inline void kl_poll(void){u16 k=~REG_KEYINPUT&0xFFF,'),
 ('static void poll(void){prevk=keys;keys=~REG_KEYINPUT&0x3FF;', 'static void poll(void){prevk=keys;keys=~REG_KEYINPUT&0xFFF;'),
 # infantry: X = take the weapon at your feet, else reload; Y = swap weapon
 ('  if(pressed&K_L){int d=wd_near(u);if(d>=0)wd_take(u,d);else{next_wpn(u);feed(PW[u->wpn].n,HB_YEL);sfx_menu();}}\n',
  '  if(pressed&K_L){int d=wd_near(u);if(d>=0)wd_take(u,d);else{next_wpn(u);feed(PW[u->wpn].n,HB_YEL);sfx_menu();}}\n'
  '  if(pressed&K_DSX){int d=wd_near(u);if(d>=0)wd_take(u,d);else if(u->wpn!=2&&u->rl==0&&u->mag[u->wpn]<PW[u->wpn].mag){u->rl=PW[u->wpn].reload;feed("RELOADING",HB_YEL);sfx_menu();}}\n'
  '  if(pressed&K_DSY){next_wpn(u);feed(PW[u->wpn].n,HB_YEL);sfx_menu();}\n'),
 ('static void tank_b(int i){Unit*u=&U[i];const TankS*T=TV(u);\n', DECOY + 'static void tank_b(int i){Unit*u=&U[i];const TankS*T=TV(u);\n'),
 ('  tank_drive(u,th);if(keys&K_A)tank_fire(i);if(keys&K_B)tank_b(i);return;}', '  if(pressed&K_DSY)ds_decoy(u,0);tank_drive(u,th);if(keys&K_A)tank_fire(i);if(keys&K_B)tank_b(i);return;}'),
 ('  ship_drive(u,maxs*u->mode/3);if(keys&K_A)ship_main(i);', '  if(pressed&K_DSY)ds_decoy(u,0);ship_drive(u,maxs*u->mode/3);if(keys&K_A)ship_main(i);'),
 ('  if(burn){ts=ts*45/100;}air_move(u,ts);if(keys&K_A)air_fire(i);if(pressed&K_B)air_missile(i);return;}',
  '  if(pressed&K_DSY)ds_decoy(u,1);if(burn){ts=ts*45/100;}air_move(u,ts);if(keys&K_A)air_fire(i);if(pressed&K_B)air_missile(i);return;}'),
 ('  if(s->kind==K_MISSILE&&s->tgt>=0&&U[s->tgt].alive){Unit*t=&U[s->tgt];',
  '  if(s->kind==K_MISSILE&&s->tgt>=0&&U[s->tgt].isp&&ds_decoy_on())s->tgt=-1;\n  if(s->kind==K_MISSILE&&s->tgt>=0&&U[s->tgt].alive){Unit*t=&U[s->tgt];'),
 # arcade cabinet: tell controls.c / touchui.c when the cabinet (2) or a cabinet game (1) is running
 ('static void arc_play(int g){const AGame*G=&AGS[g];', 'static void arc_play(int g){const AGame*G=&AGS[g];ds_arc_on=1;'),
 (' pressed=0;bgCached=-1;}\n// ---- cabinet screen', ' pressed=0;bgCached=-1;ds_arc_on=2;}\n// ---- cabinet screen'),
 ('static int cab_screen(void){as_load();static int g=0;', 'static int cab_screen(void){as_load();ds_arc_on=2;static int g=0;'),
 ('  if(pressed&K_B){sfx_menu();pressed=0;bgCached=-1;return 0;}}}', '  if(pressed&K_B){sfx_menu();pressed=0;bgCached=-1;ds_arc_on=0;ds_fb_end(fb);return 0;}}}'),
 # arcade bug pass (DS): present the frame straight from fb (one 77 KB copy less per frame: cabinet,
 # Chopper Rescue and Space Pinball no longer drop to 30-40 fps), attract preview at full speed,
 # Starship Hopper waits for the first A press (it used to crash into the floor before you could react)
 # and shows its single life, Cargo Stack shows no lives (it has none)
 ('static void a_show(void){next_frame();dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);}',
  'static void a_show(void){REG_DISPCNT=3|(1<<10);ds_fb_src=fb;next_frame();}'),
 # arcade: filled discs (Chopper Rescue hills, ...) - same pixels, row widths found incrementally, off-screen rows skipped
 ('static void a_disc(int cx,int cy,int r,u16 c){for(int dy=-r;dy<=r;dy++){int w=0;while((w+1)*(w+1)+dy*dy<=r*r)w++;a_rect(cx-w,cy+dy,2*w+1,1,c);}}',
  'static void a_disc(int cx,int cy,int r,u16 c){if(cx+r<0||cx-r>=240||cy+r<0||cy-r>=160)return;int w=r;for(int d=0;d<=r;d++){while(w*w+d*d>r*r)w--;if(cy+d<160)a_rect(cx-w,cy+d,2*w+1,1,c);if(d&&cy-d>=0)a_rect(cx-w,cy-d,2*w+1,1,c);}}'),
 # arcade: Tank Duel moved a tank SIN*speed/2 in 8.8 fixed point, i.e. 55-110 px per frame, so tanks jumped
 # across the arena (or through gaps) in one frame. Now speed/128 px: player 1.7 px/frame forward, 1.1 reverse, CPU 1.4
 ('static void td_move(TdT*t,int fw){s32 nx=t->x+SIN(t->a)*fw/2,ny=t->y-COS(t->a)*fw/2;',
  'static void td_move(TdT*t,int fw){s32 nx=t->x+(SIN(t->a)*fw>>7),ny=t->y-(COS(t->a)*fw>>7);'),
 ('  aiH=aiP=0;AGS[g].ai();agT++;if(AGS[g].step(aiH,aiP)||agT>60*90){agScore=0;agLives=3;agLvl=0;agT=0;AGS[g].init();}AGS[g].draw();',
  '  {static u32 cv;int n=(int)(vblN-cv);cv=vblN;if(n<1)n=1;if(n>2)n=2;for(int q=0;q<n;q++){aiH=aiP=0;AGS[g].ai();agT++;if(AGS[g].step(aiH,aiP)||agT>60*90){agScore=0;agLives=3;agLvl=0;agT=0;AGS[g].init();}}}AGS[g].draw();'),
 ('static void sh_init(void){shY=70<<8;', 'static int shWait;static void sh_init(void){shWait=1;agLives=1;shY=70<<8;'),
 ('static int sh_step(u16 h,u16 p){(void)h;', 'static int sh_step(u16 h,u16 p){(void)h;if(shWait){++shWait;if((shWait>20&&(p&K_A))||agAttract||FR_AUTO||shWait>300)shWait=0;else return 0;}'),
 (' m3_ctext(20,itoa(agScore),C_WHITE,2);}', ' m3_ctext(20,itoa(agScore),C_WHITE,2);if(shWait&&(agT&16))m3_ctext(96,"PRESS A TO FIRE THE ENGINE",C_YEL,1);}'),
 ('static void cs_init(void){memset(csG,0,sizeof csG);', 'static void cs_init(void){agLives=0;memset(csG,0,sizeof csG);'),
 # SETTINGS: the GBA 'RUMBLE (N/A)' row becomes CONTROLS: DS / CLASSIC (GBA)
 ('"CONTROL HINTS","RUMBLE",""', '"CONTROL HINTS","CONTROLS",""'),
 ('u16 c=k==sel?C_YEL:(k==7?C_GRAY:C_WHITE);', 'u16 c=k==sel?C_YEL:C_WHITE;'),
 ('case 7:v="N/A (NO RUMBLE HW)";break;', 'case 7:v=ds_ctl_name();break;'),
 ('if(k==sel&&k!=7){m3_text(104,y,"<"', 'if(k==sel){m3_text(104,y,"<"'),
 ('sel==7?"GBA EMULATORS: NO RUMBLE MOTOR"', 'sel==7?(ds_ctl_preset?"GBA LAYOUT (X = SELECT)":"A B X Y L R - LEGEND ON TOUCH SCREEN")'),
 ('if(pressed&K_UP){sel=(sel+8)%9;if(sel==7)sel=6;', 'if(pressed&K_UP){sel=(sel+8)%9;if(sel==7&&ds_ctl_testmode())sel=6;'),
 ('if(pressed&K_DOWN){sel=(sel+1)%9;if(sel==7)sel=8;', 'if(pressed&K_DOWN){sel=(sel+1)%9;if(sel==7&&ds_ctl_testmode())sel=8;'),
 ('case 6:SET.hints^=1;break;}', 'case 6:SET.hints^=1;break;case 7:ds_ctl_set(ds_ctl_preset^1);break;}'),
]
# on-screen hint text -> DS layout (same or shorter length so the GBA layouts still fit)
TEXT = [
 ('"L: TAKE "', '"X: TAKE "'),
 ('"A FIRE B GRENADE L SWAP"', '"A FIRE R GRENADE Y SWAP"'),
 ('"R STRAFE SEL TACTICAL"', '"L AIM X RELOAD B GEAR"'),
 ('"UP/DN DRIVE SEL SQUAD/CENTER"', '"UP/DN DRIVE X SQUAD Y SMOKE"'),
 ('"UP FAST DN SLOW SEL SCORE"', '"UP FAST DN SLOW Y FLARES"'),
 ('"A FIRE   B GRENADE   L SWAP"', '"A FIRE   R GRENADE   Y SWAP"'),
 ('"R STRAFE   SELECT TACTICAL"', '"L AIM  X RELOAD  B GEAR"'),
 ('"SELECT CALLS YOUR SQUAD."', '"X SQUAD   Y SMOKE SCREEN"'),
 ('"L/R BARREL ROLL TO DODGE"', '"L/R ROLL  Y FLARES  X SCORE"'),
 ('"SELECT SWAPS WEAPONS"', '"X/Y SWAP WEAPONS"'),
 ('"L SMOKE   SELECT DISMOUNTS"', '"L SMOKE   X DISMOUNTS"'),
 ('"SELECT: SENSOR/CLAYMORE/STROBE"', '"X: SENSOR/CLAYMORE/STROBE"'),
 ('"B OPENS DOORS / RELOADS"', '"X OPENS DOORS / RELOADS"'),
 ('"SELECT SWAPS WEAPON  START QUITS"', '"Y SWAPS WEAPON  START QUITS"'),
 ('"PROFILES: HIT EACH CUE (A/SEL/L/R)"', '"PROFILES: HIT EACH CUE (A/X/L/R)"'),
 ('"LAND GENTLY: UP = ENGINE, SEL CHUTE"', '"LAND GENTLY: UP = ENGINE, X CHUTE"'),
 ('"SURFACE: SELECT PICKS EXPERIMENT,"', '"SURFACE: X PICKS EXPERIMENT,"'),
 ('"(SELECT IN FLIGHT = AUTOPILOT)."', '"(X IN FLIGHT = AUTOPILOT)."'),
 ('"R CHANGE CAMERA  SELECT MAP"', '"R CHANGE CAMERA  X MAP"'),
 ('"ROADSTER: SELECT TOGGLES THE"', '"ROADSTER: X TOGGLES THE"'),
 ('"SELECT: PROFILE CARD."', '"X: PROFILE CARD."'),
 ('"EMPTY - SELECT: SWITCH"', '"EMPTY - Y: SWITCH"'),
 ('"DOORS: B TO OPEN"', '"DOORS: X TO OPEN"'),
 ('"UP/DOWN: MOVE  </>: TURN  L/R: STRAFE|A: FIRE  B: DOOR/RELOAD (HOLD: FAST TURN)|SELECT: SWITCH WEAPON  START: QUIT|',
  '"UP/DOWN: MOVE  </>: TURN  L/R: STRAFE|A: FIRE  X: DOOR/RELOAD  B: FAST TURN|Y: SWITCH WEAPON  START: QUIT|'),
 ('"L: UNDERBARREL SHOTGUN IF NO LAUNCHER"', '"Y: UNDERBARREL SHOTGUN IF NO LAUNCHER"'),
 ('"L CYCLES TO IT. SPLASH DAMAGE"', '"Y CYCLES TO IT. SPLASH DAMAGE"'),
 ('"AUTOCANNON + B: ATGM, SEL: SQUAD"', '"AUTOCANNON + B: ATGM, X: SQUAD"'),
 ('"HMG, SEL: DISMOUNT SQUAD"', '"HMG, X: DISMOUNT SQUAD"'),
 ('"A FIRE  B GRENADE  R STRAFE"', '"A FIRE  R GRENADE  L AIM"'),
 ('"GRENADE THAT BUNKER (B)!"', '"GRENADE THAT BUNKER (R)!"'),
 # boot camp (tutorial uses the infantry layout)
 ('"PRESS B TO THROW A GRENADE."', '"PRESS R TO THROW A GRENADE."'),
 ('"BEHIND THEM, THEN HOLD R TO"', '"BEHIND THEM, THEN HOLD L TO"'),
 ('"PRESS L TO SWAP BETWEEN"', '"PRESS Y TO SWAP BETWEEN"'),
 ('"SELECT IS YOUR USE BUTTON:"', '"X IS YOUR USE BUTTON:"'),
 ('"SELECT SHOWS THE SCORES."', '"X SCORES, Y DROPS FLARES."'),
 ('"- SELECT CHANGES UNIT ON"', '"- X CHANGES UNIT ON"'),
 ('"SELECT DISMOUNTS THE SQUAD."', '"X DISMOUNTS THE SQUAD."'),
 ('"FIRE, B OPENS DOORS. GRAB"', '"FIRE, X OPENS DOORS. GRAB"'),
 # other modes: X (or Y) is the SELECT action
 ('"SELECT: CHANGE UNIT"', '"X: CHANGE UNIT"'),
 ('"A=GO  SELECT=CHANGE UNIT"', '"A=GO  X=CHANGE UNIT"'),
 ('" - SEL SWAPS"', '" - X SWAPS"'),
 ('"SELECT IN COURSE: SWAP WEAPON"', '"X IN COURSE: SWAP WEAPON"'),
 ('"SELECT: JETTISON"', '"X: JETTISON"'),
 ('"LOST GRIP! L+R: SAFER"', '"LOST GRIP! L+R: SAFER"'),
 ('"IN TARGET ORBIT - SELECT: DEPLOY"', '"IN TARGET ORBIT - X: DEPLOY"'),
 ('"HOLD DIR + ALTERNATE L/R  SEL: CLIP"', '"HOLD DIR + ALTERNATE L/R  X: CLIP"'),
 ('"B: SUIT CAM  SELECT: CLIP TETHER"', '"B: SUIT CAM  X: CLIP TETHER"'),
 ('L = RETRO.  SELECT: FLY THE ASCENT|', 'L = RETRO.  X: FLY THE ASCENT|'),
 ('"A/SELECT/L/R ON THE CUES.', '"A/X/L/R ON THE CUES.'),
 ('"SMART ROUNDS: SELECT TAGS"', '"SMART ROUNDS: X TAGS"'),
 ('"SELECT TAGS - ROUNDS STEER."', '"X TAGS - ROUNDS STEER."'),
 ('"SELECT: SPACEX THRUSTERS "', '"X: SPACEX THRUSTERS "'),
 ('"UP/DN DRIVE SEL SQUAD/CENTER"', '"UP/DN DRIVE X SQUAD Y SMOKE"'),
 ('"L/R WEAPON SEL THERMAL"', '"L/R WEAPON X THERMAL"'),
 ('"A:SWEEP/ACT B:BIP SEL:ROBOT"', '"A:SWEEP/ACT B:BIP X:ROBOT"'),
 ('"A:ACT B:BIP SEL:TECH"', '"A:ACT B:BIP X:TECH"'),
 ('"D-PAD: RCS  SELECT: AUTOPILOT"', '"D-PAD: RCS  X: AUTOPILOT"'),
 ('"A/UP: ENGINE  L/R: DRIFT  SEL: AUTO"', '"A/UP: ENGINE  L/R: DRIFT  X: AUTO"'),
 ('"<>AGENCY A:GO SEL:TIP"', '"<>AGENCY A:GO X:TIP"'),
 ('"UP/A: ENGINE  </>: RCS  SELECT: CHUTE"', '"UP/A: ENGINE  </>: RCS  X: CHUTE"'),
 ('"<>BROWSE L/R:ERA SEL:TYPE A:FLY B:BACK"', '"<>BROWSE L/R:ERA X:TYPE A:FLY B:BACK"'),
 ('"L/R ERA U/D PICK SEL MODE A GO B BACK"', '"L/R ERA U/D PICK X MODE A GO B BACK"'),
 ('"A: DRIVE  SELECT: GARAGE  B: BACK"', '"A: DRIVE  X: GARAGE  B: BACK"'),
 ('"UP/DN DRIVE SEL SQUAD/CENTER"', '"UP/DN DRIVE X SQUAD Y SMOKE"'),
]
REPL = CODE + TEXT
hits = set()
def apply(fn, t):
    for a, b in REPL:
        if a in t: t = t.replace(a, b); hits.add(a)
    return t
def report():
    miss = [a for a, b in REPL if a not in hits]
    print('dscontrols: %d/%d patches applied%s' % (len(REPL) - len(miss), len(REPL), (' MISSING: ' + ' | '.join(m[:50] for m in miss)) if miss else ''))
    return miss
