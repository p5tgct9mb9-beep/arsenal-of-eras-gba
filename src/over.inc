// ===================================================================== v1.3.1 OVERFLIGHT (secret, SPECIALIST > CLASSIFIED, code 2003)
// What-if Cold War strategic reconnaissance: U-2 (slow, ~70,000 ft) and SR-71 (Mach 3+, ~85,000 ft) over a generic
// 1960s Eastern Bloc. Generic markings, no real leaders. Real SR-71 sorties stayed outside Soviet airspace - see the fact cards.
#include "overdb.inc"
enum{OS_BASE,OS_TGT,OS_RADAR,OS_SAM,OS_MIG,OS_TANK};
enum{TA_AIR,TA_RAIL,TA_HARB,TA_PAD,TA_SAMR,TA_FACT,TA_TEST,TA_SHIP};
typedef struct{s16 lon,lat;u8 th,kind,r,art;u16 val;const char*n;}OvSite;
static const OvSite OVS[]={
 // ---- theatre 0: Europe
 {5,524,0,OS_BASE,5,0,0,"RAF MILDENHALL"},{354,370,0,OS_BASE,5,0,0,"INCIRLIK AB"},
 {30,562,0,OS_TGT,5,TA_SHIP,100,"NORTH SEA TEST SHIP"},{-40,577,0,OS_TGT,5,TA_AIR,100,"SCOTTISH TEST AIRFIELD"},
 {128,524,0,OS_TGT,5,TA_RAIL,250,"EAST GERMANY RAIL YARDS"},{190,521,0,OS_TGT,5,TA_AIR,250,"POLISH FIGHTER BASE"},{210,566,0,OS_TGT,5,TA_HARB,300,"BALTIC NAVAL BASE"},
 {310,490,0,OS_TGT,5,TA_AIR,300,"UKRAINE SSR BOMBER BASE"},{458,486,0,OS_TGT,5,TA_PAD,350,"KAPUSTIN YAR RANGE"},{331,690,0,OS_TGT,5,TA_HARB,400,"KOLA SUBMARINE PENS"},
 {406,629,0,OS_TGT,5,TA_PAD,450,"PLESETSK LAUNCH SITE"},{376,557,0,OS_TGT,5,TA_SAMR,500,"MOSCOW OUTER SAM RING"},
 {122,530,0,OS_RADAR,22,0,0,0},{200,538,0,OS_RADAR,24,0,0,0},{300,500,0,OS_RADAR,24,0,0,0},{238,578,0,OS_RADAR,22,0,0,0},{330,676,0,OS_RADAR,26,0,0,0},
 {400,620,0,OS_RADAR,24,0,0,0},{376,557,0,OS_RADAR,30,0,0,0},{340,452,0,OS_RADAR,22,0,0,0},{450,490,0,OS_RADAR,22,0,0,0},
 {126,520,0,OS_SAM,8,0,0,0},{311,487,0,OS_SAM,9,0,0,0},{374,562,0,OS_SAM,10,0,0,0},{382,552,0,OS_SAM,10,0,0,0},{406,633,0,OS_SAM,8,0,0,0},
 {332,686,0,OS_SAM,8,0,0,0},{206,568,0,OS_SAM,7,0,0,0},{458,482,0,OS_SAM,8,0,0,0},
 {140,518,0,OS_MIG,0,0,0,0},{302,502,0,OS_MIG,0,0,0,0},{340,682,0,OS_MIG,0,0,0,0},{384,562,0,OS_MIG,0,0,0,0},{404,612,0,OS_MIG,0,0,0,0},
 {60,640,0,OS_TANK,5,0,0,"KC-135Q TRACK NORTH"},
 // ---- theatre 1: Asia
 {715,340,1,OS_BASE,5,0,0,"PESHAWAR AS"},{1278,264,1,OS_BASE,5,0,0,"KADENA AB"},
 {633,459,1,OS_TGT,5,TA_PAD,400,"BAIKONUR LAUNCH COMPLEX"},{778,504,1,OS_TGT,5,TA_TEST,350,"SEMIPALATINSK TEST SITE"},{606,568,1,OS_TGT,5,TA_FACT,400,"SVERDLOVSK INDUSTRY"},
 {1586,530,1,OS_TGT,5,TA_HARB,400,"KAMCHATKA SUB BASE"},{1319,432,1,OS_TGT,5,TA_HARB,350,"VLADIVOSTOK FLEET"},
 {650,470,1,OS_RADAR,24,0,0,0},{610,560,1,OS_RADAR,24,0,0,0},{780,500,1,OS_RADAR,22,0,0,0},{1578,532,1,OS_RADAR,26,0,0,0},{1320,436,1,OS_RADAR,24,0,0,0},{690,404,1,OS_RADAR,20,0,0,0},
 {634,462,1,OS_SAM,8,0,0,0},{606,564,1,OS_SAM,9,0,0,0},{781,507,1,OS_SAM,8,0,0,0},{1585,534,1,OS_SAM,8,0,0,0},{1318,435,1,OS_SAM,8,0,0,0},
 {640,480,1,OS_MIG,0,0,0,0},{600,556,1,OS_MIG,0,0,0,0},{1580,525,1,OS_MIG,0,0,0,0},{1322,440,1,OS_MIG,0,0,0,0},
 {1460,410,1,OS_TANK,5,0,0,"KC-135Q TRACK EAST"}};
#define NOVS ((int)(sizeof OVS/sizeof OVS[0]))
static int ov_find(const char*n){for(int i=0;i<NOVS;i++)if(OVS[i].n){const char*a=OVS[i].n,*b=n;while(*a&&*a==*b){a++;b++;}if(!*b)return i;}return -1;}
static void ov_px(int th,int lon,int lat,int*x,int*y){int L0=th?OV1_L0:OV0_L0,L1=th?OV1_L1:OV0_L1,B0=th?OV1_B0:OV0_B0,B1=th?OV1_B1:OV0_B1;*x=(lon-L0)*240/(L1-L0);*y=16+(B1-lat)*144/(B1-B0);}
enum{AC_U2,AC_SR71,AC_RECON};enum{CAM_B,CAM_OBC,CAM_SLAR};
static const char*const CAMN[3]={"TYPE B CAMERA","OPTICAL BAR CAMERA","SIDE-LOOKING RADAR"};
typedef struct{const char*n;u8 ac,th,base,cam,clouds,cband,tank;const char*t[3];const char*brief;}OvMis;
static const OvMis OVM[10]={
 {"TRAINING: NORTH SEA",AC_U2,0,0,CAM_B,0,0,0,{"NORTH SEA TEST SHIP","SCOTTISH TEST AIRFIELD",0},"FRIENDLY SKIES. FLY THE ROUTE, ARM THE CAMERA, TIME THE SHUTTER, COME HOME."},
 {"EAST GERMANY BORDER",AC_U2,0,0,CAM_B,1,0,0,{"EAST GERMANY RAIL YARDS","POLISH FIGHTER BASE",0},"RADARS WILL SEE YOU. MIGS CANNOT CLIMB THIS HIGH - SO FAR."},
 {"BALTIC FLEET",AC_U2,0,0,CAM_SLAR,3,62,0,{"BALTIC NAVAL BASE",0,0},"SOLID CLOUD. THE SIDE-LOOKING RADAR SEES THROUGH IT. WATCH FOR CONTRAILS ABOVE 62,000 FT."},
 {"UKRAINE AIRBASES",AC_U2,0,1,CAM_B,1,0,0,{"UKRAINE SSR BOMBER BASE","KAPUSTIN YAR RANGE",0},"OUT OF INCIRLIK. SA-2 SITES GUARD BOTH TARGETS. B = ECM."},
 {"BAIKONUR RANGE",AC_U2,1,0,CAM_B,0,0,0,{"BAIKONUR LAUNCH COMPLEX","SEMIPALATINSK TEST SITE",0},"FROM PESHAWAR. LONG LEGS, THIN AIR, BIG STAKES."},
 {"GRAND SLAM ROUTE",AC_U2,1,0,CAM_B,1,0,0,{"BAIKONUR LAUNCH COMPLEX","SVERDLOVSK INDUSTRY",0},"THE ROUTE THAT ENDED OVERFLIGHTS IN 1960. SA-2 BATTERIES ARE WAITING."},
 {"KOLA PENINSULA",AC_SR71,0,0,CAM_OBC,1,0,1,{"KOLA SUBMARINE PENS",0,0},"TANK UP FIRST. MACH 3.2 AT 85,000 FT. MIG-25S WILL TRY."},
 {"PLESETSK",AC_SR71,0,0,CAM_OBC,1,0,1,{"PLESETSK LAUNCH SITE","KOLA SUBMARINE PENS",0},"TWO TARGETS, ONE TANK OF JP-7 AFTER THE TANKER. WATCH THE HEAT GAUGE."},
 {"KAMCHATKA",AC_SR71,1,1,CAM_OBC,2,0,1,{"KAMCHATKA SUB BASE","VLADIVOSTOK FLEET",0},"OUT OF KADENA. TANKER EAST OF JAPAN, THEN NORTH."},
 {"MOSCOW OUTSKIRTS",AC_SR71,0,0,CAM_OBC,1,0,1,{"BALTIC NAVAL BASE","MOSCOW OUTER SAM RING",0},"THE DENSEST AIR DEFENCE ON EARTH. SPEED AND ALTITUDE ARE YOUR ONLY ARMOUR."}};
static const char*const OVMN[11]={"1 TRAINING: NORTH SEA","2 EAST GERMANY BORDER","3 BALTIC FLEET","4 UKRAINE AIRBASES","5 BAIKONUR RANGE","6 GRAND SLAM ROUTE","7 KOLA PENINSULA (SR-71)","8 PLESETSK (SR-71)","9 KAMCHATKA (SR-71)","10 MOSCOW OUTSKIRTS (SR-71)","RANDOM SORTIE"};
// ---------------------------------------------------------------- shared state
static u8 ovTrailX[200] EWRAM,ovTrailY[200] EWRAM;static int ovTrailN;
typedef struct{int q[3];u8 cloud[3],tried[3];int alt,fuel,dmg,detect,ecm,sams,migs,refuel;}OvRes;
static OvRes ovR;
static void ov_map_draw(int th){const u8*m=th?ov_map1:ov_map0;for(int y=0;y<144;y++){u16*p=&fb[(y+16)*240];const u8*s=m+y*120;for(int x=0;x<120;x++){u8 b=s[x];p[x*2]=ov_pal[b&15];p[x*2+1]=ov_pal[b>>4];}}
 for(int y=0;y<16;y++)m3_fill(0,y,240,1,RGB(8,12,20));}
static void ov_ring(int cx,int cy,int r,u16 c,int dash){for(int a=0;a<256;a+=dash){int x=cx+(COS(a)*r>>8),y=cy+(SIN(a)*r>>8);if(x>=0&&x<240&&y>=16&&y<160)fb[y*240+x]=c;}}
static void ov_spr(const u8*s,int cx,int cy,int hdg,u16 c1,u16 c2,int logd){int co=COS(hdg),si=SIN(hdg);
 for(int y=0;y<15;y++)for(int x=0;x<15;x++){int dx=x-7,dy=y-7;int sx=(dx*co+dy*si)>>8,sy=(-dx*si+dy*co)>>8;sx+=7;sy+=7;if(sx<0||sx>14||sy<0||sy>14)continue;int v=s[sy*15+sx];if(!v)continue;int X=cx+dx,Y=cy+dy;if(logd)lf(X,Y,1,1,v==1?c1:c2);else if(X>=0&&X<240&&Y>=0&&Y<160)fb[Y*240+X]=v==1?c1:c2;}}
// ---------------------------------------------------------------- threat scene: SA-2 / MiG / Wasserfall (side view)
enum{TH_SA2,TH_MIG19,TH_MIG21,TH_MIG25,TH_WASSER};
static int ovMisFuel;
static int ov_threat(int kind,int ac,int*alt,int*ecm){ // side view; returns 1 survived, 0 shot down. speeds: screen px/frame <<8
 int altk=*alt,res=-1,endT=0,fr=0,mx=0,my=0,mvx=0,mvy=0,mon=0,mfuel=0,jam=0,brk=0,brkCd=0,shots=0,closest=9999,recede=0;
 int mig=kind>=TH_MIG19&&kind<=TH_MIG25,ceil=kind==TH_MIG19?58:kind==TH_MIG21?67:80,mspd=kind==TH_MIG19?120:kind==TH_MIG21?170:205,migx=-40<<8,migy=148<<8;
 int vmax=kind==TH_WASSER?260:mig?(kind==TH_MIG25?330:280):340,burnT=kind==TH_WASSER?150:mig?90:120;
 const char*nm=kind==TH_SA2?"SA-2 GUIDELINE LAUNCH!":kind==TH_WASSER?"WASSERFALL SAM LAUNCH!":kind==TH_MIG19?"MIG-19 SCRAMBLED":kind==TH_MIG21?"MIG-21 ZOOM CLIMB":"MIG-25 FOXBAT INTERCEPT";
 int py=150-altk*14/10;music_jingle(TR_ALERT);fxWhy="";
 for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(6+y/8,10+y/5,(40+y/2>255?255:40+y/2)));for(int i=0;i<40;i++)fb[((i*37)%90)*240+(i*71)%240]=RGB(200,200,220);m3_fill(0,150,240,10,RGB(60,70,50));
 for(int k=30;k<=90;k+=10){int y=150-k*14/10;for(int x=0;x<240;x+=4)fb[y*240+x]=RGB(60,70,110);m3_text(2,y-8,cat(itoa(k),"K"),RGB(90,100,140),1);}
 fr_save_bg();fr_vstart();int gx=0,site=250<<8;lmsg(nm,100);
 for(;;fr++){int pv=ac==AC_SR71?220:ac==AC_U2?52:40;BEAT->screen=SCR_OVER;BEAT->phase=5;BEAT->scoreA=kind;BEAT->scoreB=altk;BEAT->x=mon?mx>>8:-1;BEAT->y=closest;
  if(res<0){int au=FR_AUTO,ki=keys,climb=0,burn=0,doEcm=0,doBrk=0;int ttg=9999;
   if(mon){int dx=(150<<8)-mx,dy=(py<<8)-my;int d=isq((dx>>8)*(dx>>8)+(dy>>8)*(dy>>8));int svx=mvx-pv,svy=mvy;int cl=(-(dx>>8)*svx-(dy>>8)*svy)/(d+1);ttg=cl<0?d*256/(-cl+1):9999;}
   if(au){climb=1;burn=1;if(mon&&ttg<60&&ttg>20&&!jam&&*ecm>0&&ac!=AC_SR71)doEcm=1;if(mon&&ttg<22&&ttg>6&&!brkCd&&!jam)doBrk=1;}
   else{climb=(ki&K_UP)!=0;burn=(ki&K_A)!=0;doEcm=(pressed&K_B)!=0;doBrk=(pressed&(K_LEFT|K_RIGHT))!=0;if((ki&K_DOWN)&&(fr&15)==0)altk--;}
   int amax=ac==AC_SR71?86:ac==AC_U2?72:44;if(climb&&(fr&(ac==AC_SR71?11:31))==0&&altk<amax)altk++;
   if(ac==AC_SR71&&burn)pv=250;
   if(doEcm&&*ecm>0&&!jam){(*ecm)--;jam=100;sfx_laser();lmsg(kind==TH_WASSER?"WINDOW CHAFF OUT!":"ECM JAMMING!",80);}
   if(doBrk&&!brkCd){brk=24;brkCd=100;sfx_whoosh();lmsg("BREAK! HARD TURN",60);if(ac==AC_U2&&altk>60)altk--;}
   if(jam)jam--;if(brk)brk--;if(brkCd)brkCd--;py=150-altk*14/10;gx=(gx+pv/64+1)%32;site-=pv;
   if(!mon&&!mig&&shots==0&&fr==50){mon=1;mx=site;my=148<<8;mvx=-20;mvy=-120;mfuel=burnT;sfx_whoosh();shots++;closest=9999;recede=0;}
   if(mig&&!mon&&res<0){migx+=mspd-pv;int mc=150-ceil*14/10;int tgy=py>mc?py:mc;migy+=((tgy<<8)-migy)/50;int X=migx>>8,Y=migy>>8;
    if(iabs(Y-py)<6&&X>30&&X<140&&shots<2&&fr>120&&(fr%60)==0){mon=1;mx=migx;my=migy;mvx=mspd+40;mvy=0;mfuel=burnT;shots++;sfx_whoosh();lmsg("MISSILE AWAY FROM THE MIG!",80);closest=9999;recede=0;}
    if(fr>500&&!mon){res=1;lmsg(Y>py+8?(ac==AC_SR71?"THE FOXBAT CANNOT CLIMB TO YOU":"MIGS CANNOT REACH YOUR ALTITUDE"):"INTERCEPTOR FELL BEHIND",120);}
    if(X<-60&&fr>200&&!mon){res=1;lmsg("INTERCEPTOR FELL BEHIND",120);}}
   if(mon){int thin=256-altk*256/100;if(thin<24)thin=24;
    int sp=isq((mvx>>2)*(mvx>>2)+(mvy>>2)*(mvy>>2))*4;if(sp<8)sp=8;
    if(mfuel>0){mfuel--;if(sp<vmax){mvx+=mvx*7/sp;mvy+=mvy*7/sp;}}else{mvx-=mvx*thin/9000;mvy-=mvy*thin/9000;mvy+=1;}
    if(!jam){int dx=(150<<8)-mx,dy=(py<<8)-my;int d=isq((dx>>8)*(dx>>8)+(dy>>8)*(dy>>8))+1;int wx=(dx>>8)*sp/d+pv,wy=(dy>>8)*sp/d;int wl=isq((wx>>2)*(wx>>2)+(wy>>2)*(wy>>2))*4+1;wx=wx*sp/wl;wy=wy*sp/wl;
     int turn=sp*thin/(mfuel>0?2600:5200)+1;mvx+=clampi(wx-mvx,-turn,turn);mvy+=clampi(wy-mvy,-turn,turn);}
    {int capk=kind==TH_WASSER?60:mig?ceil+6:84;int capy=(150-capk*14/10)<<8;if(my<capy){my=capy;if(mvy<0)mvy=0;mvx=mvx*31/32;}}
    mx+=mvx-pv;my+=mvy;
    int dx=(mx>>8)-150,dy=(my>>8)-py;int d=isq(dx*dx+dy*dy);if(d<closest){closest=d;recede=0;}else recede++;
    if(d<5){if(brk){mon=0;lmsg("OVERSHOT! IT COULD NOT FOLLOW THE TURN",90);sfx_ok();if(!mig||shots>=2)res=1;}else{res=0;fxWhy=ac==AC_U2?"HIT - THE U-2 COMES APART":"HIT - AIRCRAFT LOST";sfx_boom();shake(14);fsp_add(150,py,14,RGB(255,180,60));}}
    if(mon&&res<0&&((recede>20&&closest<60)||(mfuel<=0&&sp<pv+30&&mvx<pv)||(mx>>8)<-40||(mx>>8)>300||(my>>8)<-30||(my>>8)>152)){mon=0;sfx_ok();lmsg(sp<pv+30?"MISSILE RAN OUT OF ENERGY":"MISSILE MISSED!",90);if(!mig||shots>=2)res=1;}}
   if(res>=0)endT=fr;if(fr>1400&&res<0)res=1;}
  else if(fr-endT>(res?80:140))break;
  next_frame();fr_vrestore();
  for(int x=-gx;x<240;x+=32)lf(x,151,8,2,RGB(90,100,70));if(!mig&&(site>>8)>-10&&(site>>8)<240)lf((site>>8)-3,146,7,4,RGB(120,40,40));
  if(res!=0){ov_spr(ac==AC_SR71?ov_spr_sr:ov_spr_u2,150,py,64,ac==AC_SR71?RGB(84,86,106):RGB(150,150,160),ac==AC_SR71?RGB(190,190,210):RGB(90,90,100),1);if(jam&&(fr&4))lf(146,py-9,9,2,RGB(120,255,120));if(brk)lf(140,py+6,20,1,RGB(255,255,255));}
  if(mig){ov_spr(ov_spr_mig,migx>>8,migy>>8,64,RGB(160,170,160),RGB(120,120,120),1);}
  if(mon){int X=mx>>8,Y=my>>8;lf(X-1,Y-1,3,3,C_WHITE);for(int k=1;k<8;k++)lf(X-((mvx-pv)*k>>7),Y-(mvy*k>>7),2,2,mfuel>0?RGB(255,200,120):RGB(160,160,160));}
  fsp_run();
  ltx(4,3,cat3("ALT ",itoa(altk),",000 FT"),C_WHITE);ltx(120,3,ac==AC_SR71?(pv>220?"MACH 3.3":"MACH 3.2"):ac==AC_U2?"410 KTS":"300 KTS",C_WHITE);ltx(190,3,cat("ECM ",itoa(*ecm)),C_YEL);
  if(fr<120&&res<0)ltc(20,ac==AC_SR71?"UP: CLIMB  A: MORE SPEED  L/R: BREAK":"UP: CLIMB  B: ECM  L/R: BREAK TURN",C_GRAY);
  if(lMsgT>0){lMsgT--;ltc(30,lMsg,C_YEL);}if(res==0)ltc(60,fxWhy,C_RED);if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();*alt=altk;return res!=0;}
// ---------------------------------------------------------------- target art (top-down), used by the photo run and the film lab
static void ov_fill(int x,int y,int w,int h,u16 c){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w>0&&h>0)m3_fill(x,y,w,h,c);}
static void ov_art(int art,int cx,int cy,int sc,int gray){ // sc: 1 = full, 2 = half size; gray = photo print palette
 #define OC(r,g,b) (gray?RGB(((r)+(g)+(b))/3,((r)+(g)+(b))/3,((r)+(g)+(b))/3):RGB(r,g,b))
 #define OR(x,y,w,h,c) ov_fill(cx+(x)/sc,cy+(y)/sc,((w)/sc)>0?(w)/sc:1,((h)/sc)>0?(h)/sc:1,c)
 switch(art){
  case TA_AIR:OR(-50,-6,100,12,OC(110,110,110));OR(-50,-1,100,1,OC(230,230,230));OR(-30,6,10,16,OC(100,100,100));OR(10,6,10,16,OC(100,100,100));for(int k=0;k<5;k++)OR(-34+k*14,22,6,6,OC(60,70,60));break;
  case TA_RAIL:for(int k=0;k<6;k++){OR(-60,-14+k*6,120,2,OC(70,60,50));for(int j=0;j<8;j++)OR(-56+j*14+(k&1)*4,-15+k*6,10,4,OC(140,90,60));}OR(-20,24,40,12,OC(120,120,120));break;
  case TA_HARB:OR(-60,-10,120,40,OC(30,60,110));for(int k=0;k<4;k++)OR(-40+k*24,-24,6,30,OC(150,150,150));for(int k=0;k<3;k++)OR(-34+k*24,-6,4,18,OC(40,40,46));OR(-60,-30,120,6,OC(120,120,110));break;
  case TA_PAD:OR(-16,-16,32,32,OC(170,160,140));OR(-4,-4,8,8,OC(240,240,240));OR(-3,16,6,26,OC(60,50,40));OR(30,-24,14,10,OC(120,120,120));OR(-44,10,10,10,OC(120,120,120));break;
  case TA_SAMR:OR(-3,-3,6,6,OC(120,120,120));for(int k=0;k<6;k++){int a=k*43;OR((COS(a)*24>>8)-3,(SIN(a)*24>>8)-3,6,6,OC(200,200,200));}OR(-30,-30,60,2,OC(90,80,60));break;
  case TA_FACT:for(int k=0;k<4;k++)OR(-50+k*26,-16,20,30,OC(150,140,130));OR(-50,20,100,6,OC(90,90,90));OR(30,-30,6,12,OC(60,60,60));break;
  case TA_TEST:for(int k=0;k<5;k++){int r=6+k*3;OR(-60+k*26,-r/2,r,r,OC(110,100,80));}OR(-4,-40,8,80,OC(150,140,110));break;
  case TA_SHIP:OR(-60,-30,120,60,OC(30,60,110));OR(-26,-5,52,10,OC(170,170,175));OR(-6,-8,12,4,OC(220,220,220));break;}
 #undef OC
 #undef OR
}
// ---------------------------------------------------------------- photo run: time the shutter as the target crosses the camera
static int ov_photo(int art,int cam,int cloudy,int sr,int*film,const char*tn){ // returns quality 0..100 (best exposure)
 int y=-60<<8,v=(sr?3:1)<<8,best=0,shots=0,fr=0,cy=-140<<8;if(cam==CAM_OBC)v=v*3/2;int tx=cam==CAM_SLAR?60:120;int win=cam==CAM_B?8:cam==CAM_OBC?14:18;
 bgm_t(sr?TR_SR71:TR_U2);fxWhy="";
 for(;;fr++){int ty=y>>8;BEAT->screen=SCR_OVER;BEAT->phase=6;BEAT->scoreA=best;BEAT->scoreB=*film;
  // ground
  for(int r=0;r<160;r+=16){int wy=(r-ty+4096)/16;for(int c=0;c<240;c+=30){int h=(wy*7+c*3)%5;ov_fill(c,r,30,16,h==0?RGB(70,100,50):h==1?RGB(90,110,60):h==2?RGB(110,120,70):h==3?RGB(80,96,56):RGB(100,104,66));}}
  for(int r=0;r<160;r+=2)if(((r-ty)&63)==0)ov_fill(0,r,240,2,RGB(120,110,90));
  ov_art(art,tx,ty,1,0);
  int cl=cloudy&&iabs((cy>>8)-ty)<50;if(cloudy)for(int r=(cy>>8)-50;r<(cy>>8)+50;r+=2)if(r>=0&&r<160)ov_fill(cam==CAM_SLAR?0:20,r,cam==CAM_SLAR?240:200,1,RGB(230,230,236));
  // reticle
  u16 rc=iabs(ty-80)<=win?C_GREEN:C_YEL;if(cam==CAM_OBC){ov_fill(0,80-win,240,1,rc);ov_fill(0,80+win,240,1,rc);}else{int rx=cam==CAM_SLAR?tx:120,w=cam==CAM_SLAR?60:40;ov_fill(rx-w,80-win,w*2,1,rc);ov_fill(rx-w,80+win,w*2,1,rc);ov_fill(rx-w,80-win,1,win*2,rc);ov_fill(rx+w,80-win,1,win*2,rc);}
  if(cam==CAM_SLAR){for(int r=0;r<160;r+=3)ov_fill(0,r,240,1,RGB(20,40,30));}
  m3_text(4,4,tn,C_WHITE,1);m3_text(4,14,cat3(CAMN[cam],"  FILM ",itoa(*film)),C_YEL,1);m3_text(4,150,"A: SHUTTER - TARGET IN THE FRAME",C_GRAY,1);
  if(best)m3_text(160,24,cat3("BEST ",itoa(best),"%"),C_GREEN,1);if(FR_AUTO)m3_text(210,4,"AUTO",C_BLUE,1);
  m3_present();next_frame();
  int shut=FR_AUTO?(iabs(ty-80)<=2&&shots==0):(pressed&K_A)!=0;
  if(shut&&*film>0){(*film)--;shots++;sfx_shot(0);int q=iabs(ty-80)<=win?100-iabs(ty-80)*60/(win+1):0;if(cam==CAM_SLAR&&q>70)q=70;if(cl&&cam!=CAM_SLAR)q=q/8;if(q>best)best=q;
   ov_fill(0,0,240,160,C_WHITE);m3_present();next_frame();}
  y+=v;cy+=v+(v/3);if(ty>250||fr>900)break;}
 if(cloudy&&best<20&&cam!=CAM_SLAR)fxWhy="CLOUDS";return best;}
// ---------------------------------------------------------------- KC-135Q tanker rendezvous (JP-7)
static int ov_refuel(void){int x=120<<8,y=110<<8,vx=0,vy=0,got=0,out=0,fr=0,res=-1;bgm_t(TR_SR71);
 for(;;fr++){BEAT->screen=SCR_OVER;BEAT->phase=7;BEAT->scoreA=got;int bx=120+(SIN(fr*2)*10>>8),by=96+(SIN(fr*3+40)*5>>8);
  int au=FR_AUTO,ki=keys;if(au){int dx=(bx<<8)-x,dy=((by+10)<<8)-y;vx+=clampi(dx/64-vx/4,-24,24);vy+=clampi(dy/64-vy/4,-24,24);}
  else{if(ki&K_LEFT)vx-=18;if(ki&K_RIGHT)vx+=18;if(ki&K_UP)vy-=18;if(ki&K_DOWN)vy+=18;}
  vx+=rr(9)-4;vy+=rr(9)-4;vx=vx*15/16;vy=vy*15/16;x+=vx;y+=vy;x=clampi(x,10<<8,230<<8);y=clampi(y,20<<8,150<<8);int px=x>>8,py=y>>8;
  int in=iabs(px-bx)<8&&iabs(py-(by+10))<6;if(in){got++;out=0;}else if(got>0)out++;
  if(out>90){lmsg("DISCONNECT - REJOIN THE BOOM",60);out=0;}
  if(got>=360)res=1;if(fr>2400&&res<0)res=got>180?1:0;
  for(int r=0;r<160;r++)ov_fill(0,r,240,1,RGB(40+r/4,70+r/3,(140+r/2>255?255:140+r/2)));
  ov_fill(70,30,100,10,RGB(170,170,176));ov_fill(112,10,16,50,RGB(180,180,186));ov_fill(40,36,160,4,RGB(150,150,156));ov_fill(116,56,8,6,RGB(120,120,126));
  for(int k=0;k<36;k++){int lx=120+(bx-120)*k/36,ly=60+(by-60)*k/36;ov_fill(lx-1,ly,3,1,RGB(90,90,96));}ov_fill(bx-2,by-2,5,5,RGB(255,200,60));
  ov_fill(bx-9,by+4,1,12,in?C_GREEN:C_YEL);ov_fill(bx+9,by+4,1,12,in?C_GREEN:C_YEL);
  ov_spr(ov_spr_sr,px,py+8,0,RGB(30,30,36),RGB(80,80,90),0);ov_fill(px-1,py-1,3,3,in?C_GREEN:C_RED);
  m3_text(4,4,"KC-135Q: STAY UNDER THE BOOM",C_WHITE,1);m3_text(4,14,"JP-7",C_GRAY,1);ov_fill(30,15,120,5,RGB(30,30,40));ov_fill(30,15,got/3,5,C_GREEN);
  if(lMsgT>0){lMsgT--;m3_text(60,140,lMsg,C_YEL,1);}if(FR_AUTO)m3_text(196,150,"AUTO",C_BLUE,1);
  m3_present();next_frame();if(res>=0)break;}
 if(res)lmsg("TANKS FULL - CLEARED TO PRESS",120);return res;}
// ---------------------------------------------------------------- film lab + intel report
static int ov_develop(const OvMis*M,int base,int survived){bgm_t(TR_INTEL);int tot=0,maxv=0,good=0,n=0;
 ov_fill(0,0,240,160,RGB(40,6,6));m3_ctext(4,"PHOTO LAB - DEVELOPING FILM",C_YEL,1);
 for(int k=0;k<3;k++){if(!M->t[k])continue;n++;int si=ov_find(M->t[k]);const OvSite*S=&OVS[si];int q=ovR.q[k];maxv+=S->val;int v=S->val*q/100;tot+=v;if(q>=60)good++;
  int x=8+k*78,y=20;ov_fill(x,y,72,56,RGB(230,230,224));ov_fill(x+4,y+4,64,44,RGB(60,60,60));
  if(ovR.tried[k]){ov_art(S->art,x+36,y+26,2,1);if(q<60)for(int i=0;i<300-q*3;i++){int a=rr(64),b=rr(44);fb[(y+4+b)*240+x+4+a]=RGB(150,150,150);}if(ovR.cloud[k])for(int r=0;r<44;r+=2)ov_fill(x+4,y+4+r,64,1,RGB(220,220,220));}
  else m3_ctext(y+22,"",C_WHITE,1);
  m3_text(x,y+60,q>=85?"SHARP":q>=60?"USABLE":ovR.cloud[k]?"CLOUDED":ovR.tried[k]?"BLURRED":"NO IMAGE",q>=60?C_GREEN:C_RED,1);m3_text(x,y+70,cat("+",itoa(v)),C_WHITE,1);
  for(int t=0;t<(FR_AUTO?10:30);t++)next_frame();m3_present();}
 int sc=tot*10+(survived?1500:0)+ovR.fuel/4-ovR.detect*2-ovR.dmg*300;if(sc<0)sc=0;int grade=maxv?tot*100/maxv:0;
 int stars=!survived?0:good==n&&ovR.detect<60?3:good==n?2:good?1:0;
 m3_ctext(108,cat3("INTEL VALUE ",itoa(tot)," PTS"),C_WHITE,1);m3_ctext(118,cat("REPORT GRADE ",grade>=85?"A":grade>=60?"B":grade>=30?"C":"D"),C_YEL,1);
 m3_ctext(128,fr_s2("RADAR TIME ",ovR.detect/10,"  SAMS EVADED ",ovR.sams),C_GRAY,1);
 {char st[8]={0};for(int i=0;i<3;i++)st[i]=i<stars?'*':'-';m3_ctext(138,cat("STARS ",st),C_YEL,2);}m3_present();
 music_jingle(TR_INTEL);for(int t=0;;t++){next_frame();if(t>40&&((pressed&(K_A|K_B|K_START))||(FR_AUTO&&t>200)))break;}(void)base;return sc|(stars<<24);}
// ---------------------------------------------------------------- CLASSIFIED: 4-digit code lock
static int ov_code(void){int d[4]={0,0,0,0},p=0,redraw=1;bgm_t(TR_COLD);
 for(int fr=0;;fr++){BEAT->screen=SCR_CODE;BEAT->menu=p;BEAT->scoreA=d[0]*1000+d[1]*100+d[2]*10+d[3];
  if(redraw){redraw=0;ov_fill(0,0,240,160,RGB(8,10,14));for(int y=0;y<160;y+=4)ov_fill(0,y,240,1,RGB(12,16,22));
   m3_ctext(14,"CLASSIFIED",C_RED,2);m3_ctext(36,"ACCESS CODE REQUIRED",C_WHITE,1);
   // padlock
   ov_fill(110,52,20,16,RGB(200,170,60));ov_fill(113,44,14,3,RGB(170,170,180));ov_fill(113,44,3,10,RGB(170,170,180));ov_fill(124,44,3,10,RGB(170,170,180));ov_fill(118,57,4,6,RGB(60,40,10));
   for(int i=0;i<4;i++){int x=72+i*26;ov_fill(x,80,20,26,i==p?RGB(40,60,40):RGB(24,28,34));m3_text(x+5,85,itoa(d[i]),i==p?C_GREEN:C_WHITE,2);if(i==p){ov_fill(x,108,20,2,C_GREEN);}}
   m3_ctext(122,"UP/DOWN: DIGIT  LEFT/RIGHT: MOVE",C_GRAY,1);m3_ctext(134,"A: ENTER   B: BACK",C_GRAY,1);m3_present();}
  next_frame();{u32 j=FR_JUMP;if((j&0xFFFF0000)==0x434F0000){FR_JUMP=0;int c=j&0xFFFF;d[0]=c/1000%10;d[1]=c/100%10;d[2]=c/10%10;d[3]=c%10;pressed|=K_A;}}
  if(pressed&K_UP){d[p]=(d[p]+1)%10;redraw=1;sfx_menu();}if(pressed&K_DOWN){d[p]=(d[p]+9)%10;redraw=1;sfx_menu();}
  if(pressed&K_LEFT){p=(p+3)%4;redraw=1;}if(pressed&K_RIGHT){p=(p+1)%4;redraw=1;}if(pressed&K_B)return 0;
  if(pressed&K_A){int c=d[0]*1000+d[1]*100+d[2]*10+d[3];
   if(c==2003){v131_load();VS.over=1;v131_save();music_jingle(TR_GRANTED);
    for(int t=0;t<150;t++){BEAT->phase=3;if(t%20==0){ov_fill(20,60,200,40,(t/20)&1?RGB(10,40,10):RGB(20,80,20));m3_ctext(74,"ACCESS GRANTED",C_GREEN,2);m3_present();}next_frame();}
    return 1;}
   sfx1(0,0xF,2,3,1000,40);for(int t=0;t<90;t++){BEAT->phase=4;if(t%15==0){ov_fill(20,60,200,40,(t/15)&1?RGB(40,6,6):RGB(90,10,10));m3_ctext(74,"ACCESS DENIED",C_RED,2);m3_present();}if(t==30)sfx1(0,0xF,2,3,900,40);next_frame();}
   d[0]=d[1]=d[2]=d[3]=0;p=0;redraw=1;}}}
// ---------------------------------------------------------------- strategic map flight
static const char*const OVF[10][2]={
 {"U-2","THE LOCKHEED U-2 FIRST FLEW IN 1955 AND CRUISED AROUND 70,000 FT. NEAR ITS CEILING THE GAP BETWEEN STALL AND OVERSPEED WAS ONLY A FEW KNOTS - PILOTS CALLED IT COFFIN CORNER."},
 {"1956","U-2 FLIGHTS OVER THE EASTERN BLOC BEGAN IN JULY 1956. SOVIET RADARS TRACKED THEM FROM THE START, BUT NO FIGHTER OF THE TIME COULD CLIMB HIGH ENOUGH TO REACH THEM."},
 {"SLAR","SIDE-LOOKING AIRBORNE RADAR BUILDS ITS IMAGE FROM ECHOES, SO IT WORKS AT NIGHT AND THROUGH CLOUD - WITH LESS DETAIL THAN FILM."},
 {"SA-2","THE SOVIET S-75 DVINA (NATO NAME SA-2 GUIDELINE) COULD REACH U-2 ALTITUDES. IN 1959 ONE DOWNED A HIGH-FLYING RB-57D OVER CHINA - THE FIRST AIRCRAFT KILLED BY A SURFACE-TO-AIR MISSILE."},
 {"TYURATAM","U-2 PHOTOS OF THE TYURATAM ROCKET RANGE - LATER CALLED BAIKONUR - TAKEN FROM 1957 ON HELPED THE US GAUGE THE SOVIET MISSILE PROGRAM."},
 {"1 MAY 1960","FRANCIS GARY POWERS' U-2 WAS BROUGHT DOWN BY SA-2 MISSILES NEAR SVERDLOVSK ON 1 MAY 1960. HE SURVIVED, WAS TRIED IN MOSCOW AND WAS EXCHANGED IN 1962. OVERFLIGHTS OF THE USSR ENDED."},
 {"SR-71","THE SR-71 (FIRST FLIGHT 1964) CRUISED ABOVE MACH 3 AT ABOUT 80,000-85,000 FT. ITS DEFENCE WAS TO OUTRUN THREATS: NO SR-71 WAS EVER LOST TO ENEMY ACTION."},
 {"JP-7","THE SR-71 BURNED JP-7, A FUEL SO STABLE IT NEEDED A CHEMICAL IGNITER. DEDICATED KC-135Q TANKERS CARRIED IT. THE HOT AIRFRAME STRETCHED SEVERAL INCHES IN FLIGHT."},
 {"MIG-25","THE MIG-25 FOXBAT WAS BUILT TO CHASE FAST, HIGH TARGETS. IT COULD DASH VERY FAST FOR SHORT PERIODS BUT COULD NOT RUN DOWN AN SR-71 FROM BEHIND."},
 {"WHAT-IF","REAL SR-71 SORTIES NEAR THE USSR STAYED IN INTERNATIONAL AIRSPACE - OVERFLIGHTS STOPPED AFTER 1960. THE SR-71 MISSIONS HERE ARE FICTION; THE HISTORY ON THESE CARDS IS REAL."}};
static int ov_bearing(int dx,int dy){int best=0,bv=-(1<<30);for(int a=0;a<256;a++){int v=SIN(a)*dx-COS(a)*dy;if(v>bv){bv=v;best=a;}}return best;} // 0 = north, 64 = east
static int ov_base(int th,int k){int n=0;for(int i=0;i<NOVS;i++)if(OVS[i].th==th&&OVS[i].kind==OS_BASE){if(n==k)return i;n++;}return 0;}
static int ov_tank(int th){for(int i=0;i<NOVS;i++)if(OVS[i].th==th&&OVS[i].kind==OS_TANK)return i;return -1;}
static void ov_static(const OvMis*M,int th,const u8*done,int tankDone){ov_map_draw(th);
 for(int i=0;i<NOVS;i++){const OvSite*S=&OVS[i];if(S->th!=th)continue;int x,y;ov_px(th,S->lon,S->lat,&x,&y);
  switch(S->kind){case OS_BASE:ov_fill(x-2,y-2,5,5,RGB(80,160,255));{int lw=slen(S->n)*6;m3_text(x>150?x-4-lw:x+4,y-10,S->n,RGB(180,210,255),1);}break;
   case OS_RADAR:if(M->n[0]!='T')ov_ring(x,y,S->r,RGB(220,80,80),5);break;
   case OS_SAM:if(M->n[0]!='T'){ov_ring(x,y,S->r,RGB(255,60,60),3);ov_fill(x-1,y-1,3,3,RGB(255,60,60));}break;
   case OS_MIG:if(M->n[0]!='T'){ov_fill(x-2,y,5,1,RGB(255,140,60));ov_fill(x,y-2,1,5,RGB(255,140,60));}break;
   case OS_TANK:if(M->tank){ov_ring(x,y,5,tankDone?RGB(80,120,120):RGB(80,255,255),6);}break;
   case OS_TGT:for(int k=0;k<3;k++)if(M->t[k]&&ov_find(M->t[k])==i){u16 c=done[k]?C_GREEN:C_YEL;ov_fill(x-4,y-4,9,1,c);ov_fill(x-4,y+4,9,1,c);ov_fill(x-4,y-4,1,9,c);ov_fill(x+4,y-4,1,9,c);if(!done[k]){int lw=slen(S->n)*6;m3_text(x>140?x-6-lw:x+6,y+6,S->n,C_YEL,1);}}break;}}
 for(int i=0;i<ovTrailN;i++){int x=ovTrailX[i],y=ovTrailY[i]+16;if(y<160)fb[y*240+x]=RGB(255,255,255);}
 fr_save_bg();}
static int ov_fly(const OvMis*M,int*scOut){ // returns 1 = came home, 0 = lost. *scOut = score | stars<<24
 int th=M->th,sr=M->ac==AC_SR71,bi=ov_base(th,M->base),ti=ov_tank(th);int bx,by;ov_px(th,OVS[bi].lon,OVS[bi].lat,&bx,&by);
 int x=bx<<8,y=by<<8,hdg=0,spd=sr?92:26,alt=sr?72:64,film=M->cam==CAM_B?10:M->cam==CAM_OBC?8:6,ecm=sr?3:2,heat=0,res=-1,endT=0,fr=0,painted=0,samCd=0,migT=-1,migN=0,cam=1,turnAcc=0;
 u8 done[3]={0,0,0};int tankDone=!M->tank;int nt=0;for(int k=0;k<3;k++)if(M->t[k])nt++;
 for(int k=0;k<3;k++){ovR.q[k]=0;ovR.cloud[k]=0;ovR.tried[k]=0;}ovR.dmg=0;ovR.detect=0;ovR.sams=0;ovR.migs=0;ovR.refuel=0;ovTrailN=0;
 // route length -> fuel (frames of flight)
 int L=0,lx=bx,ly=by;if(M->tank){int tx,ty;ov_px(th,OVS[ti].lon,OVS[ti].lat,&tx,&ty);L+=isq((tx-lx)*(tx-lx)+(ty-ly)*(ty-ly));lx=tx;ly=ty;}
 int Lpre=L;for(int k=0;k<3;k++)if(M->t[k]){int s=ov_find(M->t[k]),tx,ty;ov_px(th,OVS[s].lon,OVS[s].lat,&tx,&ty);L+=isq((tx-lx)*(tx-lx)+(ty-ly)*(ty-ly));lx=tx;ly=ty;}
 L+=isq((bx-lx)*(bx-lx)+(by-ly)*(by-ly));int fuel=M->tank?(Lpre*256/spd)*2+700:(L*256/spd)*16/10+900,fuel0=fuel;
 // clouds: drifting blobs that start over the first targets
 int cxs[3],cys[3],cn=M->clouds>3?3:M->clouds;for(int k=0;k<cn;k++){int s=ov_find(M->t[k<nt?k:0]),tx,ty;ov_px(th,OVS[s].lon,OVS[s].lat,&tx,&ty);cxs[k]=(tx-6+k*5)<<8;cys[k]=(ty-2)<<8;}
 int train=M->n[0]=='T';
 ov_static(M,th,done,tankDone);fr_vstart();bgm_t(sr?TR_SR71:TR_U2);lmsg(M->brief,240);
 for(;;fr++){int px=x>>8,py=y>>8;BEAT->screen=SCR_OVER;BEAT->phase=1;BEAT->scoreA=done[0]+done[1]*2+done[2]*4+(tankDone<<4);BEAT->scoreB=fuel*100/(fuel0+1);BEAT->x=px;BEAT->y=py;BEAT->extra=alt|(painted<<8)|(hdg<<16);
  if(res<0){int au=FR_AUTO,ki=keys;
   // next waypoint: tanker, then nearest unphotographed target, then home
   int wx=bx,wy=by,wk=-1;if(!tankDone){ov_px(th,OVS[ti].lon,OVS[ti].lat,&wx,&wy);wk=9;}else{int bd=1<<30;for(int k=0;k<3;k++)if(M->t[k]&&!done[k]){int s=ov_find(M->t[k]),tx,ty;ov_px(th,OVS[s].lon,OVS[s].lat,&tx,&ty);int d=(tx-px)*(tx-px)+(ty-py)*(ty-py);if(d<bd){bd=d;wx=tx;wy=ty;wk=k;}}}
   int want=ov_bearing(wx-px,wy-py);BEAT->timer=want|(wk<<8);
   int trate=sr?1:2;
   if(au){int df=((want-hdg+384)&255)-128;turnAcc++;if(turnAcc>=trate){turnAcc=0;if(df>0)hdg=(hdg+1)&255;else if(df<0)hdg=(hdg-1)&255;}if((fr&(sr?11:31))==0&&alt<(sr?85:70))alt++;cam=1;}
   else{turnAcc++;if(turnAcc>=trate){turnAcc=0;if(ki&K_LEFT)hdg=(hdg-1)&255;if(ki&K_RIGHT)hdg=(hdg+1)&255;}if((fr&15)==0){if(ki&K_UP)alt++;if(ki&K_DOWN)alt--;}alt=clampi(alt,sr?60:50,sr?86:72);if(pressed&K_A){cam^=1;sfx_menu();lmsg(cam?"CAMERA ARMED":"CAMERA SAFE",60);}}
   int burn=sr&&!au&&(ki&K_B);int v=spd+(burn?16:0);if(sr){heat+=burn?3:(heat>0?-1:0);if(heat>900){heat=600;ovR.dmg++;lmsg("OVERHEAT - INLET UNSTART!",90);shake(6);v=spd/2;}}
   x+=SIN(hdg)*v>>8;y-=COS(hdg)*v>>8;fuel-=burn?2:1;
   if(px<1||px>238||py<17||py>158){hdg=(hdg+128)&255;x=clampi(x,2<<8,237<<8);y=clampi(y,18<<8,157<<8);}
   // contrails + trail
   int contrail=M->cband&&alt>=M->cband&&alt<M->cband+6;
   if((fr%(contrail?6:24))==0&&ovTrailN<200){ovTrailX[ovTrailN]=px;ovTrailY[ovTrailN]=py-16;ovTrailN++;if(py>=16&&py<160){u16 c=contrail?RGB(255,255,255):RGB(200,200,200);bigbuf[py*240+px]=c;fb[py*240+px]=c;}}
   // radar
   int pnt=0;if(!train)for(int i=0;i<NOVS;i++){const OvSite*S=&OVS[i];if(S->th!=th||S->kind!=OS_RADAR)continue;int sx,sy;ov_px(th,S->lon,S->lat,&sx,&sy);int r=S->r*(contrail?3:2)/2;if((sx-px)*(sx-px)+(sy-py)*(sy-py)<r*r)pnt=1;}
   if(pnt&&!painted){lmsg(contrail?"CONTRAIL! RADAR SWEEP - YOU ARE PAINTED":"RADAR SWEEP - YOU ARE PAINTED",120);sfx1(0,0xC,2,2,1850,12);}
   painted=pnt;if(pnt){ovR.detect++;}
   if(samCd)samCd--;
   if(pnt&&!samCd&&ovR.detect>40)for(int i=0;i<NOVS;i++){const OvSite*S=&OVS[i];if(S->th!=th||S->kind!=OS_SAM)continue;int sx,sy;ov_px(th,S->lon,S->lat,&sx,&sy);if((sx-px)*(sx-px)+(sy-py)*(sy-py)<S->r*S->r){
     fr_vstop();int ok=ov_threat(TH_SA2,M->ac,&alt,&ecm);ovR.sams++;samCd=sr?240:420;if(!ok){res=0;fxWhy="SHOT DOWN BY AN SA-2";}else bgm_t(sr?TR_SR71:TR_U2);ov_static(M,th,done,tankDone);fr_vstart();break;}}
   if(res<0&&migT<0&&migN<2&&ovR.detect>(migN?260:110)){migT=200;lmsg("MIG SCRAMBLE DETECTED",90);}
   if(migT>0&&--migT==0&&res<0){fr_vstop();int kind=sr?TH_MIG25:migN?TH_MIG21:TH_MIG19;int ok=ov_threat(kind,M->ac,&alt,&ecm);migN++;ovR.migs++;migT=-1;if(!ok){res=0;fxWhy="SHOT DOWN BY A MIG";}else bgm_t(sr?TR_SR71:TR_U2);ov_static(M,th,done,tankDone);fr_vstart();}
   // tanker
   if(res<0&&!tankDone){int tx,ty;ov_px(th,OVS[ti].lon,OVS[ti].lat,&tx,&ty);if((tx-px)*(tx-px)+(ty-py)*(ty-py)<25){fr_vstop();int ok=ov_refuel();ovR.refuel=1;tankDone=1;
     int rem=0,lx2=tx,ly2=ty;for(int k=0;k<3;k++)if(M->t[k]){int s=ov_find(M->t[k]),qx,qy;ov_px(th,OVS[s].lon,OVS[s].lat,&qx,&qy);rem+=isq((qx-lx2)*(qx-lx2)+(qy-ly2)*(qy-ly2));lx2=qx;ly2=qy;}rem+=isq((bx-lx2)*(bx-lx2)+(by-ly2)*(by-ly2));
     fuel=ok?(rem*256/spd)*15/10+600:fuel+(rem*256/spd)*8/10;fuel0=fuel;alt=sr?76:alt;bgm_t(TR_SR71);ov_static(M,th,done,tankDone);fr_vstart();}}
   // photo targets
   for(int k=0;k<3&&res<0;k++)if(M->t[k]&&!done[k]&&cam&&film>0){int s=ov_find(M->t[k]),tx,ty;ov_px(th,OVS[s].lon,OVS[s].lat,&tx,&ty);
    if((tx-px)*(tx-px)+(ty-py)*(ty-py)<16){int cloudy=0;for(int c=0;c<cn;c++){int dx=(cxs[c]>>8)-tx,dy=(cys[c]>>8)-ty;if(dx*dx+dy*dy<100)cloudy=1;}
     if(au&&cloudy&&M->cam!=CAM_SLAR&&fuel>fuel0/3){hdg=(hdg+40)&255;break;} // autopilot: go around and wait for a gap
     fr_vstop();int q=ov_photo(OVS[s].art,M->cam,cloudy,sr,&film,OVS[s].n);ovR.q[k]=q;ovR.tried[k]=1;ovR.cloud[k]=cloudy&&M->cam!=CAM_SLAR;done[k]=1;bgm_t(sr?TR_SR71:TR_U2);
     ov_static(M,th,done,tankDone);fr_vstart();lmsg(q>=85?"GOOD PHOTOS - FILM STOWED":q>=40?"PHOTOS TAKEN - SOME BLUR":cloudy?"CLOUDED OVER":"MISSED THE TARGET",90);}}
   // clouds drift
   for(int c=0;c<cn;c++){cxs[c]+=7;cys[c]+=(c&1)?2:-2;}
   int all=1;for(int k=0;k<3;k++)if(M->t[k]&&!done[k])all=0;
   if(res<0&&tankDone&&(all||film==0)&&(bx-px)*(bx-px)+(by-py)*(by-py)<25){res=1;sfx_ok();lmsg("WHEELS DOWN - RUSH THE FILM TO THE LAB",120);}
   if(res<0&&fuel<=0){res=0;fxWhy=sr?"OUT OF JP-7 - CREW EJECTED":"FLAMEOUT - THE U-2 IS LOST";}
   if(res>=0)endT=fr;}
  else if(fr-endT>120)break;
  next_frame();fr_vrestore();
  px=x>>8;py=y>>8;
  for(int c=0;c<cn;c++){int X=cxs[c]>>8,Y=cys[c]>>8;for(int j=-6;j<=6;j+=2){int w=10-iabs(j);lf(X-w,Y+j,w*2,1,RGB(236,236,240));}}
  if(painted&&(fr&8))ov_ring(px,py,9,RGB(255,80,80),16),lf(px-1,py-10,3,1,RGB(255,80,80));
  ov_spr(sr?ov_spr_sr:ov_spr_u2,px,py,hdg,sr?RGB(20,20,26):RGB(200,200,210),sr?RGB(80,80,96):RGB(120,120,130),1);
  fsp_run();
  ltx(2,3,sr?"SR-71":"U-2",C_WHITE);ltx(36,3,cat(itoa(alt),"K FT"),C_WHITE);ltx(84,3,"FUEL",C_GRAY);fr_bar(110,4,30,fuel,fuel0,fuel<fuel0/5?C_RED:C_GREEN);
  ltx(146,3,cat("FILM ",itoa(film)),cam?C_YEL:C_GRAY);ltx(196,3,cat("ECM ",itoa(ecm)),C_WHITE);
  if(sr){fr_bar(200,12,36,heat,900,heat>700?C_RED:RGB(255,160,60));}
  if(lMsgT>0){lMsgT--;ltx(2,150,lMsg,C_YEL);}if(res==0)ltc(80,fxWhy,C_RED);if(FR_AUTO)ltx(212,150,"AUTO",C_BLUE);}
 fr_vstop();ovR.fuel=fuel*100/(fuel0+1);ovR.alt=alt;*scOut=0;
 if(res==1){*scOut=ov_develop(M,bi,1);}
 return res==1;}
static OvMis ovRnd;static const char*ovRndT[3];
static void ov_random(void){int th=rr(2),ac=rr(2);ovRnd.n="RANDOM SORTIE";ovRnd.ac=ac;ovRnd.th=th;ovRnd.base=rr(2);ovRnd.cam=ac?CAM_OBC:(rr(3)==0?CAM_SLAR:CAM_B);ovRnd.clouds=rr(3);ovRnd.cband=rr(2)?60+rr(4):0;ovRnd.tank=ac;ovRnd.brief="RANDOM TASKING FROM THE DESK. CHECK THE ROUTE AND GO.";
 int idx[12],n=0;for(int i=0;i<NOVS;i++)if(OVS[i].th==th&&OVS[i].kind==OS_TGT&&OVS[i].val>100)idx[n++]=i;int a=rr(n),b=(a+1+rr(n-1))%n;ovRnd.t[0]=OVS[idx[a]].n;ovRnd.t[1]=OVS[idx[b]].n;ovRnd.t[2]=0;(void)ovRndT;}
static void overflight(void){v131_load();
 if(!VS.over){if(!ov_code())return;bgCached=-1;guide_tip("OVERFLIGHT","STRATEGIC RECONNAISSANCE, 1956-1970S, WHAT-IF. GENERIC MARKINGS, FICTIONAL MISSIONS, REAL AIRCRAFT AND TECHNIQUES. GOOD LUCK, DRIVER.");}
 static int sel=0;bgm_t(TR_SPY);
 for(;;){u8 dn[11];for(int i=0;i<10;i++)dn[i]=VS.overStar[i]>0;dn[10]=0;int m=fr_menu("OVERFLIGHT","U-2 AND SR-71 STRATEGIC RECON",OVMN,11,dn,&sel,SCR_OVER);if(m<0)return;
  if(m<10&&m>VS.prog[7]&&!FR_AUTO){guide_tip("LOCKED","FLY THE EARLIER SORTIES FIRST. RANDOM SORTIE IS ALWAYS OPEN.");continue;}
  const OvMis*M;if(m==10){ov_random();M=&ovRnd;}else M=&OVM[m];
  guide_card(M->n,M->ac==AC_SR71?"SR-71 BRIEFING":"U-2 BRIEFING",M->brief,-1,0,0);
  int sc=0;int ok=ov_fly(M,&sc);int stars=(sc>>24)&3;sc&=0xFFFFFF;
  if(ok){if(m<10){if(stars>VS.overStar[m])VS.overStar[m]=stars;if(VS.prog[7]<=m)VS.prog[7]=m+1;}if(sc>VS.overBest[M->ac])VS.overBest[M->ac]=sc>65535?65535:sc;v131_save();}
  fr_result(ok?"MISSION COMPLETE":"AIRCRAFT LOST",M->n,ok?fr_s2("STARS ",stars,"  INTEL GRADE SCORE ",sc):fxWhy,ok,sc,0);
  if(ok&&m<10)fx_fact(OVF[m][0],OVF[m][1]);bgm_t(TR_SPY);if(FR_AUTO)return;}}
