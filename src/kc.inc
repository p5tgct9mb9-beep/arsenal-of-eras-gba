// v1.2: KC - the boot-camp tutorial guide (the developer's real dog). First boot + main menu "TUTORIAL".
#define SCR_TUT 44
#define KC_OFF 0x600
static int kc_done(void){return SRAM[KC_OFF]=='A'&&SRAM[KC_OFF+1]=='O'&&SRAM[KC_OFF+2]=='E'&&SRAM[KC_OFF+3]=='K'&&SRAM[KC_OFF+4]==1;}
static void kc_mark(void){SRAM[KC_OFF]='A';SRAM[KC_OFF+1]='O';SRAM[KC_OFF+2]='E';SRAM[KC_OFF+3]='K';SRAM[KC_OFF+4]=1;}
static void m3_kc(int x0,int y0,int frm,int flip){const u32*t=kc_spr+frm*32;
 for(int ty=0;ty<2;ty++)for(int tx=0;tx<2;tx++)for(int y=0;y<8;y++){u32 w=t[(ty*2+tx)*8+y];for(int x=0;x<8;x++){int c=(w>>(x*4))&15;if(!c)continue;int X=tx*8+x;if(flip)X=15-X;m3_px(x0+X,y0+ty*8+y,kc_pal[c]);}}}
static void m3_kcport(int x0,int y0,int fr){const u16*p=kc_portrait+fr*48*48;for(int y=0;y<48;y++)for(int x=0;x<48;x++){u16 c=p[y*48+x];if(!(c&0x8000))M3[(y0+y)*240+x0+x]=c;}}
enum{TK_TALK,TK_MOVE,TK_SHOOT,TK_GREN,TK_COVER,TK_SWAP,TK_USE};
typedef struct{u8 k;const char*cap;const char*l[4];}KStep;
static const KStep KST[]={
 {TK_TALK,"BOOT CAMP",{"WOOF! WELCOME, CAPTAIN!","I'M KC, YOUR BOOT CAMP","GUIDE. LET'S LEARN THE","BASICS TOGETHER!"}},
 {TK_MOVE,"MOVE",{"USE THE D-PAD TO MOVE.","RUN OVER TO THE FLAG -","I'LL BE RIGHT BEHIND YOU!",""}},
 {TK_SHOOT,"AIM + SHOOT",{"PRESS A TO FIRE. YOU AIM","THE WAY YOU FACE (D-PAD).","KNOCK DOWN ALL 3 TARGETS!",""}},
 {TK_GREN,"GRENADES",{"PRESS B TO THROW A GRENADE.","IT ARCS AHEAD, THEN BOOM!","BLOW UP THE SUPPLY CRATE.",""}},
 {TK_COVER,"COVER",{"SANDBAGS STOP BULLETS! GET","BEHIND THEM, THEN HOLD R TO","STRAFE (KEEP YOUR AIM) AND","PRESS A TO RETURN FIRE."}},
 {TK_SWAP,"WEAPONS",{"PRESS L TO SWAP BETWEEN","YOUR PRIMARY AND SIDEARM.","",""}},
 {TK_USE,"USE / TACTICAL",{"SELECT IS YOUR USE BUTTON:","SMOKE OR FLASH - AND IT","DISMOUNTS YOU FROM AN IFV.","TRY IT NOW!"}},
 {TK_TALK,"CROUCH",{"IN SPEC OPS, HOLD DOWN TO","CROUCH UNDER FIRE. A JUMPS,","B SHOOTS, L GRENADE, R USES","YOUR UNIT'S SPECIAL. WOOF!"}},
 {TK_TALK,"LOADOUT",{"LOADOUT (MAIN MENU) SETS","YOUR KIT FOR EACH ERA:","PRIMARY, SIDEARM, LETHAL,","TACTICAL, TANK AND PLANE."}},
 {TK_TALK,"VEHICLES",{"PICK A VEHICLE IN 6V6 ARMOR","OR A MISSION. D-PAD DRIVES,","A MAIN GUN, B COAX / ATGM,","L/R TURN THE TURRET."}},
 {TK_TALK,"PLANES",{"D-PAD TURNS + THROTTLE,","A GUNS, B ROCKETS/MISSILES","/BOMBS, L/R BARREL ROLL,","SELECT SHOWS THE SCORES."}},
 {TK_TALK,"MODES 1/3",{"CAMPAIGN: STORY MISSIONS","FROM WW2 TO TODAY. 6V6:","INFANTRY, ARMOR, DOGFIGHT","AND NAVAL BATTLES."}},
 {TK_TALK,"MODES 2/3",{"DRONE OPS: FLY MODERN UAVS.","WORLD FLIGHT: X-15 + GLOBE.","SPACE LAUNCH: FLY 35 REAL","ROCKETS TO ORBIT!"}},
 {TK_TALK,"MODES 3/3",{"SPEC OPS: PICK ANY OF 25","UNITS FROM 8 NATIONS FIRST","- SELECT CHANGES UNIT ON","THE MAPS. 90 COURSES! WOOF!"}},
 {TK_TALK,"BRADLEY OPS",{"BRADLEY OPS: A FIRES 25MM,","R LAUNCHES TOW, L SMOKE,","SELECT DISMOUNTS THE SQUAD.","SPOT IEDS + SHOOT THEM!"}},
 {TK_TALK,"SNIPER",{"SNIPER: R ZOOMS, L RANGES,","B HOLDS YOUR BREATH, A","FIRES. HOLD OVER FOR DROP","AND LEAD THE WIND. SHHH..."}},
 {TK_TALK,"EOD",{"EOD: SEND THE ROBOT FIRST.","X-RAY, THEN CUT THE RIGHT","WIRE - READ THE CIRCUIT.","NEVER RUSH A BOMB. WOOF."}},
 {TK_TALK,"LRRP",{"LRRP: STAY HIDDEN IN THE","JUNGLE. MOVE SLOW, WATCH","TRAPS, CALL FIRE MISSIONS","AND MAKE THE HUEY EXFIL!"}},
 {TK_TALK,"FPS",{"FPS VS BOTS: UP/DOWN MOVE,","</> TURN, L/R STRAFE, A","FIRE, B OPENS DOORS. GRAB","ENEMY GUNS THEY DROP!"}},
 {TK_TALK,"SPACE PROGRAM",{"SPACE PROGRAM: REAL FLIGHT","PROFILES, SATELLITES, ISS","DOCKING AND SPACEWALKS.","SLOW IS SMOOTH. WOOF!"}},
 {TK_TALK,"EXPLORER",{"ASTRONAUT EXPLORER: LAND","ON REAL WORLDS, JUMP, DROP","A FEATHER, WEIGH YOURSELF.","MOON JUMPS ARE 6X HIGHER!"}},
 {TK_TALK,"DISMISSED!",{"THAT'S BOOT CAMP, CAPTAIN!","REPLAY IT ANY TIME FROM","TUTORIAL ON THE MAIN MENU.","GOOD LUCK OUT THERE. WOOF!"}}};
#define NKST ((int)(sizeof KST/sizeof KST[0]))
static u16 kc_bg[240*104] EWRAM;static int kc_bgok;
static void tut_yard(void){if(kc_bgok){dma32(kc_bg,fb,240*104/2);return;}u16 g0=RGB(92,150,74),g1=RGB(104,164,82);m3_fill(0,0,240,104,g0);
 for(int y=0;y<104;y+=8)for(int x=((y>>3)&1)*8;x<240;x+=16)m3_fill(x,y,8,8,g1);
 for(int i=0;i<40;i++){int x=(i*53)%236,y=(i*37)%100;m3_px(x,y,RGB(70,120,60));m3_px(x+1,y-1,RGB(70,120,60));m3_px(x+2,y,RGB(70,120,60));}
 m3_fill(0,0,240,3,RGB(120,90,60));m3_fill(0,101,240,3,RGB(120,90,60));dma32(fb,kc_bg,240*104/2);kc_bgok=1;}
static int kc_tkey=-1,kc_tdirty;
static void tut_text(int st,int ch,int mouth,int ok){static int lst=-1,lch=-1,lmo=-1,lok=-1;int fullp=kc_tkey<0||st!=lst||ok!=lok;
 if(!fullp&&ch==lch&&mouth==lmo)return;kc_tkey=1;kc_tdirty=1;
 if(fullp){m3_fill(0,104,240,56,C_PANEL);m3_box(0,104,240,56,2);m3_fill(5,108,50,50,RGB(36,54,104));m3_text(60,107,"KC",C_YEL,1);m3_text(78,107,KST[st].cap,C_GRAY,1);
  m3_text(132,151+1,ok==1?"NICE!  A: NEXT":ok==2?"A:NEXT START:SKIP":"      START:SKIP",ok==1?C_GREEN:C_GRAY,1);
  char pg[8];pg[0]='0'+(st+1)/10;pg[1]='0'+(st+1)%10;pg[2]='/';pg[3]='0'+NKST/10;pg[4]='0'+NKST%10;pg[5]=0;m3_text(60,152,pg,C_GRAY,1);lch=-1;lmo=-1;}
 if(mouth!=lmo){m3_burst(6,109,48,48,RGB(250,214,90),RGB(236,128,48));m3_kcport(6,109,mouth);}
 if(ch!=lch){int n=0,from=lch<0?0:lch;for(int k=0;k<4;k++){const char*s=KST[st].l[k];for(int j=0;s[j]&&j<30;j++,n++){if(n>=ch)break;if(n>=from){char b[2]={s[j],0};m3_text(60+j*6,117+k*8,b,C_WHITE,1);}}}}
 lst=st;lch=ch;lmo=mouth;lok=ok;}
static void yfill(int x,int y,int w,int h,u16 c){if(y<3){h-=3-y;y=3;}if(y+h>101)h=101-y;if(x<0){w+=x;x=0;}if(x+w>240)w=240-x;if(w>0&&h>0)m3_fill(x,y,w,h,c);}
typedef struct{s16 x,y,w,h;}KRect;static KRect krc[2][40];static int knr[2],kcur;
static void kmark(int x,int y,int w,int h){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>104)h=104-y;if(w<=0||h<=0||knr[kcur]>=40)return;KRect*r=&krc[kcur][knr[kcur]++];r->x=x&~1;r->y=y;r->w=(w+(x&1)+1)&~1;r->h=h;if(r->x+r->w>240)r->w=240-r->x;}
static void krow_copy(u16*d,const u16*s,int n){*(volatile u32*)0x40000D4=(u32)s;*(volatile u32*)0x40000D8=(u32)d;*(volatile u32*)0x40000DC=(u32)n|0x80000000u;}
static void krestore(int b){for(int i=0;i<knr[b];i++){KRect*r=&krc[b][i];for(int y=r->y;y<r->y+r->h;y++)krow_copy(fb+y*240+r->x,kc_bg+y*240+r->x,r->w);}}
static void kflush(int b){u16*V=(u16*)0x06000000;for(int i=0;i<knr[b];i++){KRect*r=&krc[b][i];for(int y=r->y;y<r->y+r->h;y++)krow_copy(V+y*240+r->x,fb+y*240+r->x,r->w);}}
static int kc_ab(int v){return v<0?-v:v;}
static void kc_reset(int k,int*px,int*py,int*fx,int*fy,int*tg,int*crate){*fx=1;*fy=0;
 if(k==TK_SHOOT){*tg=0;*px=60;*py=40;}if(k==TK_GREN){*crate=1;*px=98;*py=42;}if(k==TK_COVER){*px=110;*py=46;}if(k==TK_SWAP||k==TK_USE){*px=100;*py=46;}}
static void tutorial(int first){
 int st=0,t=0,px=40,py=60,fx=1,fy=0,kx=16,ky=64,kf=2,kflip=0,done=0,okT=0,tg=0,crate=1,smoke=0,swap=0,fired=0,anim=0;
 int full_redraw=1,fullcopy=1,lastst=-1;int bx[4],by[4],bvx[4],bvy[4],bl[4];int gx=0,gy=0,gvx=0,gvy=0,gt=0,boomT=0,boomX=0,boomY=0;for(int i=0;i<4;i++)bl[i]=0;
 static const char*const WPN[2]={"PRIMARY: RIFLE","SIDEARM: PISTOL"};
 bgm_t(TR_KC);kc_tkey=-1;knr[0]=knr[1]=0;kcur=0;
 for(;;){BEAT->screen=SCR_TUT;BEAT->menu=st;BEAT->era=st;BEAT->x=px;BEAT->y=py;BEAT->extra=done|(first<<8);
  if(st!=lastst){lastst=st;full_redraw=1;kc_tkey=-1;}
  const KStep*S=&KST[st];int ch=t*2;int full=0;for(int k=0;k<4;k++){const char*s=S->l[k];while(*s++)full++;}if(ch>full)ch=full;
  int typing=ch<full;
  // ---- interactive step logic
  int dx=((keys&K_RIGHT)?1:0)-((keys&K_LEFT)?1:0),dy=((keys&K_DOWN)?1:0)-((keys&K_UP)?1:0);
  int canmove=S->k!=TK_TALK;
  if(canmove&&(dx||dy)){px+=dx*3;py+=dy*3;if(!(keys&K_R)){fx=dx;fy=dy;}anim++;}
  if(px<4)px=4;if(px>220)px=220;if(py<6)py=6;if(py>84)py=84;
  if(S->k==TK_COVER&&px>148&&px<172&&py>30&&py<62)px=px<160?148:172; // sandbags are solid
  if(canmove&&(pressed&K_A)){for(int i=0;i<4;i++)if(!bl[i]){bx[i]=px+8;by[i]=py+8;bvx[i]=fx*5;bvy[i]=fy*5;if(!fx&&!fy)bvx[i]=5;bl[i]=40;sfx_shot(0);fired++;break;}}
  if(canmove&&(pressed&K_B)&&!gt&&S->k>=TK_GREN){gx=(px+8)<<4;gy=(py+8)<<4;gvx=(fx?fx:1)*36;gvy=fy*36;gt=40;sfx_whoosh();}
  if(canmove&&(pressed&K_L)&&S->k>=TK_SWAP){swap^=1;sfx_menu();full_redraw=1;}
  if(canmove&&(pressed&K_SEL)&&S->k>=TK_USE){smoke=90;sfx_whoosh();}
  for(int i=0;i<4;i++)if(bl[i]){bl[i]--;bx[i]+=bvx[i];by[i]+=bvy[i];if(bx[i]<0||bx[i]>238||by[i]<4||by[i]>99){bl[i]=0;continue;}if(S->k==TK_SHOOT)for(int k=0;k<3;k++)if(!(tg>>k&1)&&kc_ab(bx[i]-(200))<9&&kc_ab(by[i]-(16+k*35))<10){tg|=1<<k;bl[i]=0;sfx_hurt();full_redraw=1;}
   if(S->k==TK_COVER&&bx[i]>148&&bx[i]<172&&by[i]>30&&by[i]<62&&px>172)bl[i]=0;}
  if(gt){gt--;gx+=gvx;gy+=gvy;if((gx>>4)<4)gx=4<<4;if((gx>>4)>234)gx=234<<4;if((gy>>4)<8)gy=8<<4;if((gy>>4)>96)gy=96<<4;if(!gt){boomT=20;boomX=gx>>4;boomY=gy>>4;sfx_boom();if(S->k==TK_GREN&&crate&&kc_ab(boomX-196)<26&&kc_ab(boomY-50)<26)crate=0;full_redraw=1;}}
  if(boomT)boomT--;if(smoke)smoke--;
  int okNow=0;switch(S->k){case TK_TALK:okNow=1;break;case TK_MOVE:okNow=px>186&&py>36&&py<70;break;case TK_SHOOT:okNow=tg==7;break;case TK_GREN:okNow=!crate;break;
   case TK_COVER:okNow=px<=148&&py>24&&py<68&&(keys&K_R)&&(pressed&K_A);break;case TK_SWAP:okNow=swap;break;case TK_USE:okNow=smoke>0;break;}
  if(okNow&&!done&&S->k!=TK_TALK){done=1;sfx_ok();}
  if(S->k==TK_TALK)done=!typing;
  // ---- KC follows
  int tx=px-20,ty=py+4;if(S->k==TK_TALK){tx=px+22;ty=py+2;}
  int mv=0;if(kc_ab(kx-tx)>3){kx+=kx<tx?1+(kc_ab(kx-tx)>30):-1-(kc_ab(kx-tx)>30);mv=1;kflip=tx<kx;}if(kc_ab(ky-ty)>3){ky+=ky<ty?1:-1;mv=1;}
  int woof=(S->l[0][0]=='W'||S->l[3][slen(S->l[3])>4?slen(S->l[3])-5:0]=='W')&&typing&&((t>>3)&1);
  kf=mv?((t>>3)&1):(woof?3:((t>>4)%3==0?4:2));if(!mv&&S->k!=TK_TALK&&!done)kflip=0;
  // ---- draw
  if(full_redraw){full_redraw=0;fullcopy=1;kc_bgok=0;tut_yard();kc_bgok=0;
   m3_text(6,6,S->cap,C_WHITE,1);
   if(S->k==TK_MOVE){m3_fill(206,30,2,26,RGB(200,200,200));m3_fill(208,30,14,9,RGB(230,60,50));m3_fill(198,54,20,4,RGB(70,50,30));}
   if(S->k==TK_SHOOT)for(int k=0;k<3;k++){int ty2=16+k*35;if(tg>>k&1){m3_fill(192,ty2+6,16,4,RGB(120,90,60));continue;}
    m3_fill(193,ty2-7,14,14,RGB(240,240,240));m3_fill(196,ty2-4,8,8,RGB(220,40,40));m3_fill(199,ty2-1,2,2,RGB(240,240,240));m3_fill(199,ty2+7,2,8,RGB(120,90,60));}
   if(S->k==TK_GREN&&crate){m3_fill(186,40,20,20,RGB(150,110,60));m3_fill(186,40,20,2,RGB(90,60,30));m3_fill(186,58,20,2,RGB(90,60,30));m3_fill(195,40,2,20,RGB(90,60,30));m3_text(186,62,"AMMO",C_YEL,1);}
   if(S->k==TK_GREN&&!crate){for(int k=0;k<5;k++)m3_fill(184+k*5,52+(k&1)*4,5,3,RGB(110,80,40));}
   if(S->k==TK_COVER){for(int r=0;r<4;r++)for(int c=0;c<2;c++)m3_fill(150+c*10-(r&1)*5+((r&1)&&!c?5:0),30+r*8,10,7,(r+c)&1?RGB(170,150,100):RGB(196,176,120));m3_fill(212,40,10,14,RGB(120,120,130));m3_fill(214,34,6,6,RGB(140,140,150));m3_text(206,26,"DUMMY",C_GRAY,1);}
   if(S->k==TK_SWAP||S->k==TK_USE)m3_text(120,6,WPN[swap],C_YEL,1);
   if(st==0){const u16*q=kc_portrait;for(int y=0;y<96;y++)for(int x=0;x<96;x++){u16 c=q[(y>>1)*48+(x>>1)];if(!(c&0x8000))M3[(y+5)*240+x+124]=c;}m3_text(132,92,"",C_YEL,1);}
   if(S->k==TK_TALK&&st>0){int cx=120-slen(S->cap)*6;m3_fill(cx-6,26,slen(S->cap)*12+12,24,RGB(30,40,70));m3_text(cx,30,S->cap,C_YEL,2);}
   dma32(fb,kc_bg,240*104/2);kc_bgok=1;knr[0]=knr[1]=0;}
  else krestore(kcur^1);
  knr[kcur]=0;
  if(S->k==TK_COVER&&(t&31)<4){kmark(176,46,30,1);m3_fill(176,46,30,1,RGB(255,240,120));}
  for(int i=0;i<4;i++)if(bl[i])kmark(bx[i]-1,by[i]-1,3,3);if(gt)kmark((gx>>4)-2,(gy>>4)-40,4,42);if(boomT)kmark(boomX-20,boomY-20,40,40);if(smoke)kmark(px-10,py-4,48,20);
  kmark(kx,ky,16,16);kmark(px-1,py-12,18,30);if(fx||fy)kmark(px+8+fx*11-1,py+8+fy*11-1,3,3);if(woof)kmark(kx-32,ky-10,64,32);
  for(int i=0;i<4;i++)if(bl[i])yfill(bx[i]-1,by[i]-1,3,3,RGB(255,236,90));
  if(gt){int h=(gt*(40-gt))/30;yfill((gx>>4)-2,(gy>>4)-2-h,4,4,RGB(50,70,40));}
  if(boomT){int r=24-boomT;for(int a=0;a<16;a++){int ox=r*((a&3)-1)/2,oy=r*(((a>>2)&3)-1)/2;yfill(boomX+ox-3,boomY+oy-3,6,6,a&1?RGB(255,160,40):RGB(255,230,90));}}
  m3_kc(kx,ky,kf,kflip);
  m3_unit(PS.unit,px,py,1,(dx||dy)&&canmove?1+((anim>>3)&1):0);
  if(fx||fy)yfill(px+8+fx*11-1,py+8+fy*11-1,3,3,RGB(255,255,255));
  if(smoke)for(int k=0;k<6;k++)yfill(px-10+k*6,py-4+((k*7+t)&7),12,12,RGB(200,200,206));
  if(woof)m3_text(kx+(kflip?-30:16),ky<12?ky+18:ky-8,"WOOF!",C_YEL,1);
  if(done&&S->k!=TK_TALK)kmark(px-6,py<14?py+16:py-12,24,12);if(done&&S->k!=TK_TALK)m3_text(px-4,py<14?py+18:py-10,"OK!",C_GREEN,1);
  tut_text(st,ch,typing&&((t>>2)&1)?1:0,done&&S->k!=TK_TALK?1:done?2:0);
  int pk=kc_tkey;(void)pk;
  next_frame();if(fullcopy){dma32(fb,(void*)0x06000000,240*160/2);fullcopy=0;}else{kflush(kcur^1);kflush(kcur);if(kc_tdirty){dma32(fb+104*240,(u16*)0x06000000+104*240,240*56/2);}}kc_tdirty=0;REG_DISPCNT=3|(1<<10);kcur^=1;t++;
  if(pressed&K_START){sfx_ok();break;}
  if((pressed&K_A)&&S->k==TK_TALK&&typing){t=full;continue;}
  if((pressed&K_A)&&done&&(S->k==TK_TALK||okT>20)){st++;t=0;done=0;okT=0;sfx_menu();if(st>=NKST)break;kc_reset(KST[st].k,&px,&py,&fx,&fy,&tg,&crate);continue;}
  if(done&&S->k!=TK_TALK)okT++;
  if(done&&S->k!=TK_TALK&&okT>90){st++;t=0;done=0;okT=0;if(st>=NKST)break;kc_reset(KST[st].k,&px,&py,&fx,&fy,&tg,&crate);}
 }
 kc_mark();(void)first;}
