// ===================================================================== v1.3.1 SPACE FRONTIER
// Space guide + history tour, Moon Base, Mars Mission, Orbital Defense, Rocket Builder, Asteroid Mining,
// What-If Space Race 1945 chain. Records in SRAM 0x2000 'AOEV' (see v131_save).
#include "v131db.inc"
#define V131_OFF 0x2000
#define SCR_GUIDE 70
#define SCR_MOON 71
#define SCR_MARS 72
#define SCR_ODEF 73
#define SCR_RB 74
#define SCR_AST 75
#define SCR_WIF 76
#define SCR_OVER 77
#define SCR_CODE 78
#define FR_AUTO (*(volatile u32*)0x0203FFD0==0x4F545541)
#define FR_JUMP (*(volatile u32*)0x0203FFD8)
typedef struct{u8 guide,over,tips,pad;u8 prog[8];u16 best[8];u8 overStar[16];u16 overBest[2];u8 rbDone[6];u16 rbBest[6];}V131S;
static V131S VS;
static void v131_save(void){const u8*b=(const u8*)&VS;u8 sum=0x31;SRAM[V131_OFF]='A';SRAM[V131_OFF+1]='O';SRAM[V131_OFF+2]='E';SRAM[V131_OFF+3]='V';SRAM[V131_OFF+4]=1;SRAM[V131_OFF+5]=sizeof VS;
 for(unsigned i=0;i<sizeof VS;i++){SRAM[V131_OFF+8+i]=b[i];sum+=b[i]*(i+3);}SRAM[V131_OFF+7]=sum;}
static int v131_loaded;
static void v131_load(void){if(v131_loaded)return;v131_loaded=1;u8*b=(u8*)&VS;for(unsigned i=0;i<sizeof VS;i++)b[i]=0;
 if(SRAM[V131_OFF]!='A'||SRAM[V131_OFF+1]!='O'||SRAM[V131_OFF+2]!='E'||SRAM[V131_OFF+3]!='V'||SRAM[V131_OFF+4]!=1)return;unsigned n=SRAM[V131_OFF+5];if(n>sizeof VS)n=sizeof VS;
 u8 sum=0x31,t[sizeof VS];for(unsigned i=0;i<n;i++){t[i]=SRAM[V131_OFF+8+i];sum+=t[i]*(i+3);}if(sum!=SRAM[V131_OFF+7])return;for(unsigned i=0;i<n;i++)b[i]=t[i];}
static int v131_over_unlocked(void){v131_load();return VS.over;}
static const char*msn_label(int md){if(md==MD_OVER&&!v131_over_unlocked())return "CLASSIFIED";return MSN[md];}
static int fr_best(int k,int sc){v131_load();if(sc>VS.best[k]){VS.best[k]=sc>65535?65535:sc;v131_save();return 1;}return 0;}
// ---------------------------------------------------------------- shared drawing
static void fr_stars(int n,int seed){for(int i=0;i<n;i++){int x=(i*97+seed*13)%240,y=(i*53+seed*7)%160;fb[y*240+x]=(i&7)?RGB(140,140,170):RGB(255,255,255);}}
static void fr_bgspace(void){for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(4,6,18+y/10));fr_stars(70,0);}
static void fr_panel(int x,int y,int w,int h){for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++){u16 c=fb[j*240+i];fb[j*240+i]=((c>>2)&0x1CE7)+RGB(10,24,60);}
 m3_fill(x,y,w,1,RGB(90,210,240));m3_fill(x,y+h-1,w,1,RGB(90,210,240));m3_fill(x,y,1,h,RGB(90,210,240));m3_fill(x+w-1,y,1,h,RGB(90,210,240));m3_fill(x,y,12,2,RGB(236,196,92));m3_fill(x+w-12,y+h-2,12,2,RGB(236,196,92));}
static void fr_guide_px(int x0,int y0,int talk){m3_box(x0-3,y0-3,54,54,1);m3_burst(x0,y0,48,48,RGB(70,110,200),RGB(40,64,140));const u16*p=guide_px+(talk?48*48:0);
 for(int y=0;y<48;y++)for(int x=0;x<48;x++){u16 c=p[y*48+x];if(!(c&0x8000))fb[(y0+y)*240+x0+x]=c;}}
// word-wrap s into lines of at most w chars ('|' forces a break); returns line count
static int fr_wrap(const char*s,char out[][40],int w,int maxl){int n=0,len=0;const char*p=s;
 while(*p&&n<maxl){if(*p=='|'){out[n][len]=0;n++;len=0;p++;continue;}
  const char*q=p;while(*q&&*q!=' '&&*q!='|')q++;int wl=q-p;
  if(len&&len+1+wl>w){out[n][len]=0;n++;len=0;if(n>=maxl)break;}
  if(len)out[n][len++]=' ';for(int i=0;i<wl&&len<39;i++)out[n][len++]=p[i];
  p=q;while(*p==' ')p++;}
 if(len&&n<maxl){out[n][len]=0;n++;}return n;}
// ---------------------------------------------------------------- the guide: Wernher von Braun (1912-1977), historical depiction
typedef struct{const char*t,*yr,*body;u8 ill;}GCard;
static const GCard GTOUR[12]={
 {"WELCOME, EXPLORER!","GUIDE","I AM WERNHER VON BRAUN, ROCKET ENGINEER (1912-1977). I'LL WALK YOU THROUGH HOW PEOPLE REACHED SPACE - THE BRILLIANT PARTS AND THE TERRIBLE ONES.|(ALL DIALOGUE IS PARAPHRASED FOR THIS GAME.)",0},
 {"THE ROCKET EQUATION","1903","KONSTANTIN TSIOLKOVSKY, A RUSSIAN SCHOOLTEACHER, WORKED IT OUT: DV = VE x LN(M0 / MF). FASTER EXHAUST AND A LIGHTER EMPTY ROCKET MEAN MORE SPEED. STAGING - DROPPING DEAD WEIGHT - IS HOW WE BEAT IT.",1},
 {"GODDARD'S FIRST FLIGHT","1926","ON 16 MARCH 1926 ROBERT GODDARD LAUNCHED THE FIRST LIQUID-FUEL ROCKET AT AUBURN, MASSACHUSETTS. IT FLEW ABOUT 12 METRES HIGH FOR 2.5 SECONDS - A TINY HOP THAT OPENED THE WAY.",2},
 {"HERMANN OBERTH","1923","OBERTH'S BOOK 'THE ROCKET INTO PLANETARY SPACE' SHOWED SPACEFLIGHT WAS POSSIBLE ON PAPER. IT INSPIRED A GENERATION OF ENGINEERS - ME AMONG THEM, AS A STUDENT.",3},
 {"THE V-2 REACHES SPACE","1944","MY TEAM BUILT THE A4, OR V-2, AS A WEAPON FOR NAZI GERMANY. IN JUNE 1944 TEST ROCKET MW 18014 CLIMBED TO ABOUT 176 KM - THE FIRST HUMAN-MADE OBJECT IN SPACE.",4},
 {"THE HUMAN COST","1943-45","V-2S WERE BUILT BY CONCENTRATION-CAMP PRISONERS IN FORCED LABOR AT MITTELWERK, BENEATH THE MITTELBAU-DORA CAMP. THOUSANDS OF PRISONERS DIED BUILDING THEM, AND AS WEAPONS THEY KILLED THOUSANDS MORE IN LONDON, ANTWERP AND BEYOND. THIS MUST BE REMEMBERED.",5},
 {"OPERATION PAPERCLIP","1945","AFTER THE WAR THE UNITED STATES BROUGHT ME AND MORE THAN 1,600 GERMAN SCIENTISTS AND ENGINEERS TO AMERICA, OFTEN OVERLOOKING WHAT THEY HAD DONE DURING THE WAR.",6},
 {"SPUTNIK 1","1957","ON 4 OCTOBER 1957 THE SOVIET UNION LAUNCHED SPUTNIK 1, THE FIRST ARTIFICIAL SATELLITE. ITS SIMPLE RADIO BEEP, HEARD AROUND THE WORLD, STARTED THE SPACE RACE.",7},
 {"EXPLORER 1","1958","ON 31 JANUARY 1958 A JUPITER-C ROCKET FROM MY TEAM PUT EXPLORER 1, AMERICA'S FIRST SATELLITE, INTO ORBIT. JAMES VAN ALLEN'S INSTRUMENT ON IT DISCOVERED EARTH'S RADIATION BELTS.",8},
 {"YURI GAGARIN","1961","ON 12 APRIL 1961 YURI GAGARIN ORBITED EARTH ONCE ABOARD VOSTOK 1 - THE FIRST HUMAN IN SPACE. THE WHOLE FLIGHT TOOK ABOUT 108 MINUTES.",9},
 {"SATURN V + APOLLO 11","1969","MY CENTER IN HUNTSVILLE BUILT THE SATURN V. ON 20 JULY 1969 APOLLO 11 LANDED NEIL ARMSTRONG AND BUZZ ALDRIN ON THE MOON WHILE MICHAEL COLLINS ORBITED ABOVE.",10},
 {"SHUTTLE, ISS + REUSE","1981-NOW","AFTER MY TIME, THE SHUTTLE FLEW 135 MISSIONS, THE ISS HAS HOSTED CREWS SINCE 2000, AND TODAY BOOSTERS LAND AND FLY AGAIN. NOW IT'S YOUR TURN, EXPLORER!",11}};
static int rk_find(const char*n){for(int i=0;i<NRK;i++){const char*a=RK[i].n;int k=0;while(a[k]&&a[k]==n[k])k++;if(!a[k]&&!n[k])return i;}return 0;}
static void fr_ill_rk(int x,int y,const char*n1,const char*n2){ // rockets drawn into bigbuf, shrunk 2x into the 66x56 box
 for(int j=0;j<160;j++)for(int i=0;i<132;i++)bigbuf[j*240+i]=RGB(6,10,28);lfb=bigbuf;
 if(n2){draw_rk(&RK[rk_find(n1)],40,156,0,0);draw_rk(&RK[rk_find(n2)],96,156,0,0);}else draw_rk(&RK[rk_find(n1)],66,156,0,0);lfb=0;
 for(int j=0;j<56;j++)for(int i=0;i<66;i++){const u16*q=&bigbuf[(44+j*2)*240+i*2];fb[(y+j)*240+x+i]=q[0];}}
static void fr_ill(int k,int x,int y){ // illustration box (x,y) 66x56
 lfb=0;m3_fill(x,y,66,56,RGB(6,10,28));
 switch(k){
  case 0:fr_ill_rk(x,y,"SATURN V",0);break;
  case 1:m3_text(x+4,y+10,"DV=VE",C_YEL,1);m3_text(x+4,y+22,"xLN(M0",C_YEL,1);m3_text(x+16,y+34,"/MF)",C_YEL,1);break;
  case 2:lf(x+32,y+12,2,34,RGB(160,160,170));lf(x+20,y+44,26,2,RGB(160,160,170));lf(x+28,y+6,10,8,RGB(200,200,210));lf(x+31,y+46,4,6,RGB(255,190,80));lf(x+26,y+46,2,8,RGB(120,120,130));lf(x+38,y+46,2,8,RGB(120,120,130));break;
  case 3:lf(x+14,y+6,38,44,RGB(120,40,40));lf(x+16,y+8,34,40,RGB(150,60,50));m3_text(x+18,y+14,"DIE",C_YEL,1);m3_text(x+16,y+26,"RAKETE",C_YEL,1);break;
  case 4:fr_ill_rk(x,y,"V-2 / A4 MW 18014",0);break;
  case 5:lf(x+10,y+4,46,48,RGB(40,40,46));lf(x+12,y+6,42,44,RGB(70,70,76));m3_text(x+15,y+10,"IN",C_WHITE,1);m3_text(x+15,y+20,"MEMORY",C_WHITE,1);lf(x+31,y+36,4,10,RGB(230,230,220));lf(x+32,y+32,2,4,RGB(255,200,80));break;
  case 6:lf(x,y+40,66,16,RGB(30,60,120));lf(x+8,y+32,50,8,RGB(60,64,72));lf(x+18,y+24,28,8,RGB(200,200,206));lf(x+30,y+14,4,10,RGB(80,80,86));break;
  case 7:{for(int j=-8;j<=8;j++)for(int i=-8;i<=8;i++)if(i*i+j*j<=64)fb[(y+24+j)*240+x+28+i]=(i+j<-4)?RGB(240,240,240):RGB(180,184,190);
   for(int a=0;a<4;a++)for(int t=0;t<20;t++){int dx=a<2?-t+a*2:t-(a-2)*2;fb[(y+30+t)*240+x+28+dx]=RGB(200,200,206);}break;}
  case 8:fr_ill_rk(x,y,"MERCURY-REDSTONE",0);m3_text(x+2,y+2,"JUPITER-C",C_GRAY,1);break;
  case 9:fr_ill_rk(x,y,"VOSTOK 1",0);break;
  case 10:fr_ill_rk(x,y,"SATURN V",0);break;
  default:fr_ill_rk(x,y,"FALCON 9 BLOCK 5","SHUTTLE DISCOVERY");break;}
 m3_fill(x,y,66,1,RGB(90,210,240));m3_fill(x,y+55,66,1,RGB(90,210,240));m3_fill(x,y,1,56,RGB(90,210,240));m3_fill(x+65,y,1,56,RGB(90,210,240));}
// one guide card: returns 1 next, -1 skip (B). Static layer composed once; per frame only the mouth + revealed text.
static int guide_card(const char*title,const char*yr,const char*body,int ill,int n,int of){char L[10][40];int nl=fr_wrap(body,L,37,8);int tot=0;for(int i=0;i<nl;i++)tot+=slen(L[i]);
 fr_bgspace();fr_panel(4,4,232,152);fr_guide_px(12,12,0);m3_text(68,12,"WERNHER",C_GRAY,1);m3_text(68,21,"VON BRAUN",C_GRAY,1);m3_text(68,30,"1912-1977",C_GRAY,1);m3_text(68,46,yr,RGB(110,200,230),1);
 if(ill>=0)fr_ill(ill,164,8);m3_text(10,64,title,C_YEL,1);m3_fill(10,73,slen(title)*6,1,RGB(236,196,92));
 if(of>0)m3_text(10,148,cat3(itoa(n+1),"/",itoa(of)),C_GRAY,1);m3_text(96,148,"A: NEXT   B: SKIP",C_GRAY,1);
 int shown=0,drawn=0,lt=-1;
 for(int t=0;;t++){BEAT->screen=SCR_GUIDE;BEAT->menu=n;BEAT->phase=shown>=tot;
  if(shown<tot){shown+=2;if(shown>tot)shown=tot;}int talk=shown<tot&&((t>>2)&1);
  if(talk!=lt){const u16*p=guide_px+(talk?48*48:0);for(int y=26;y<34;y++)for(int x=16;x<34;x++){u16 c=p[y*48+x];if(!(c&0x8000))fb[(12+y)*240+12+x]=c;}lt=talk;}
  if(drawn<shown){int pos=0;for(int i=0;i<nl;i++){int L0=slen(L[i]);for(int k=0;k<L0;k++,pos++)if(pos>=drawn&&pos<shown){char c2[2]={L[i][k],0};m3_text(10+k*6,77+i*9,c2,C_WHITE,1);}}drawn=shown;}
  m3_present();next_frame();
  if(FR_AUTO&&t>40)return 1;
  if(pressed&K_A){if(shown<tot)shown=tot;else{sfx_menu();return 1;}}if(pressed&(K_B|K_START)){sfx_menu();return -1;}}}
static void guide_tour(void){bgm_t(TR_SPACE);for(int i=0;i<12;i++){if(guide_card(GTOUR[i].t,GTOUR[i].yr,GTOUR[i].body,GTOUR[i].ill,i,12)<0)break;}v131_load();if(!VS.guide){VS.guide=1;v131_save();}pressed=0;}
static void guide_tip(const char*t,const char*body){guide_card(t,"TIP",body,-1,0,0);pressed=0;}
static void v131_cat_enter(int c){if(c!=CAT_EXPLORE)return;v131_load();if(!VS.guide&&*(volatile u32*)0x0203FFE8!=0x4447534E){ // test hook: harness scripts skip the first-visit tour unless tagged #guide
 guide_tour();bgCached=-1;}}
// Space Launch tips: non-modal overlay during the countdown + SELECT on the select screen opens a full card
static const char*const LTIPS[6]={"FOLLOW THE YELLOW TICK. TURN TOO EARLY = DRAG, TOO LATE = GRAVITY.","STAGE THE MOMENT A TANK RUNS DRY. DEAD WEIGHT STEALS DELTA-V.",
 "EASE THE THROTTLE THROUGH MAX-Q, THEN OPEN IT UP AGAIN.","ORBIT IS SIDEWAYS SPEED: ABOUT 7.8 KM/S, NOT JUST HEIGHT.","LAND BOOSTERS LATE AND LOW: KILL SIDEWAYS SPEED FIRST.","SUBORBITAL? GO STRAIGHT UP AND COAST TO APOGEE."};
static int launch_tip_idx(const Rk*r){if(r->kind==LK_SUB)return 5;if(r->land&&(frame&1))return 4;return (r->ns+r->ag+r->nb)%4;}
static void launch_tip_overlay(const Rk*r,int cd){if(cd<=0)return;char L[6][40];int n=fr_wrap(r->ag==10?"WHAT-IF ROCKET: FICTIONAL, GENERIC MARKINGS. FLY IT, BUT REMEMBER THE REAL COST.":LTIPS[launch_tip_idx(r)],L,14,5);
 for(int j=66;j<126;j++)for(int i=2;i<94;i++){u16 c=fb[j*240+i];fb[j*240+i]=((c>>2)&0x1CE7)+RGB(10,24,60);}
 const u16*p=guide_px+((cd>>3)&1?48*48:0);for(int y=0;y<24;y++)for(int x=0;x<24;x++){u16 c=p[(y*2)*48+x*2];if(!(c&0x8000))fb[(68+y)*240+4+x]=c;}
 ltx(30,70,"GUIDE",C_YEL);ltx(30,79,"TIP",RGB(110,200,230));for(int i=0;i<n;i++)ltx(6,94+i*7-0,L[i],C_WHITE);}
static void launch_tip_card(const Rk*r){static const char*const T[6]={"FOLLOW THE YELLOW GUIDE TICK. PITCHING OVER TOO EARLY WASTES FUEL FIGHTING DRAG; TOO LATE WASTES IT FIGHTING GRAVITY.",
  "STAGE THE MOMENT A TANK RUNS DRY. EVERY KILOGRAM OF EMPTY STEEL YOU CARRY STEALS DELTA-V - THAT'S TSIOLKOVSKY'S LESSON.",
  "MAX-Q IS THE MOMENT OF PEAK AERODYNAMIC PRESSURE. EASE THE THROTTLE THERE, THEN OPEN IT UP AGAIN.",
  "ORBIT IS NOT ABOUT HEIGHT - IT'S ABOUT SIDEWAYS SPEED. ABOUT 7.8 KM/S KEEPS YOU FALLING AROUND THE EARTH INSTEAD OF INTO IT.",
  "TOUCH THE BOOSTER DOWN GENTLY: KILL SIDEWAYS SPEED FIRST, THEN LIGHT THE ENGINE LATE AND LOW.",
  "A SOUNDING ROCKET WINS ON APOGEE. GO STRAIGHT UP, THROTTLE BACK THROUGH MAX-Q, THEN COAST. ABOVE 100 KM YOU ARE IN SPACE."};
 if(r->ag==10)guide_tip("WHAT-IF 1945","THIS ROCKET IS FROM A FICTIONAL 1945. GENERIC MARKINGS ONLY. THE REAL WARTIME ROCKET PROGRAM RAN ON FORCED LABOR - THE GAME DOES NOT CELEBRATE THAT.");
 else guide_tip(r->n,T[launch_tip_idx(r)]);}
// ---------------------------------------------------------------- records + dispatch
static const char*const V131CRED[5]={"SPACE GUIDE: A HISTORICAL DEPICTION","OF WERNHER VON BRAUN (1912-1977).","DIALOGUE IS PARAPHRASED GAME TEXT.","NOT ENDORSED BY HIS ESTATE, NASA,","OR ANY AGENCY. ART IS ORIGINAL."};
static void rec_v131(int md,const char**L){v131_load();static char b[5][40];
 #define SB2(k,s) {const char*_s=(s);int _n=0;while(_s[_n]&&_n<38){b[k][_n]=_s[_n];_n++;}b[k][_n]=0;L[k]=b[k];}
 if(md==MD_HIST){for(int i=0;i<5;i++)L[i]=V131CRED[i];return;}
 int k=md-MD_MOON;if(md==MD_OVER){SB2(0,cat("MISSIONS FLOWN: ",itoa(VS.prog[7])));SB2(1,cat("BEST U-2 SCORE: ",itoa(VS.overBest[0])));SB2(2,cat("BEST SR-71 SCORE: ",itoa(VS.overBest[1])));L[3]=VS.over?"ACCESS: GRANTED":"ACCESS: CLASSIFIED";return;}
 SB2(0,cat("MISSIONS CLEARED: ",itoa(VS.prog[k])));SB2(1,cat("BEST SCORE: ",itoa(VS.best[k])));
 if(md==MD_RBUILD){int n=0;for(int i=0;i<6;i++)n+=VS.rbDone[i]!=0;SB2(2,cat3("TARGETS REACHED: ",itoa(n),"/6"));}
 #undef SB2
}
static void moon_base(void);static void mars_mission(void);static void orbital_defense(int boss);static void rocket_builder(void);static void asteroid_mining(void);static void whatif_1945(void);static void overflight(void);
static void v131_play(int md){v131_load();switch(md){
 case MD_HIST:guide_tour();break;case MD_MOON:moon_base();break;case MD_MARS:mars_mission();break;case MD_ODEF:orbital_defense(0);break;
 case MD_RBUILD:rocket_builder();break;case MD_ASTER:asteroid_mining();break;case MD_WHATIF:whatif_1945();break;case MD_OVER:overflight();break;}
 pressed=0;bgCached=-1;}
