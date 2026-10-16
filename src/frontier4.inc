// ===================================================================== v1.3.1 MOON BASE / MARS MISSION / ASTEROID MINING
// shared: soft-float helpers, a powered lander (LM / skycrane / Starship flip), flip-screen rover, base builder, Mars EDL, DART
static float fx_exp(float x){int neg=x<0;if(neg)x=-x;int k=0;while(x>0.25f&&k<14){x*=0.5f;k++;}float r=1+x*(1+x*(0.5f+x*(0.16666667f+x*0.041666667f)));while(k--)r*=r;return neg?1.0f/r:r;}
static float fx_sqrt(float x){if(x<=0)return 0;float s=x>1?x*0.5f:1;for(int i=0;i<10;i++)s=0.5f*(s+x/s);return s;}
static int fx_i(float f){return (int)(f<0?f-0.5f:f+0.5f);}
static void fx_body(int x,int y,int ang,int len,int w,u16 c){int s=SIN(ang),co=COS(ang);for(int t=-len;t<=len;t+=2){int px=x+(t*s>>8),py=y-(t*co>>8);lf(px-w/2,py-w/2,w,w,c);}}
static void fx_disc(int cx,int cy,int r,u16 c,u16 hi){for(int j=-r;j<=r;j++){int w=isq((r*r-j*j)<<0);lf(cx-w,cy+j,w*2+1,1,(j<-r/3)?hi:c);}}
static void fx_disc_fb(int cx,int cy,int r,u16 c,u16 hi){for(int j=-r;j<=r;j++){int w=isq(r*r-j*j);int y=cy+j;if(y<0||y>=160)continue;for(int x=cx-w;x<=cx+w;x++)if(x>=0&&x<240)fb[y*240+x]=(j<-r/3&&x<cx)?hi:c;}}
static void fx_fact(const char*t,const char*body){guide_card(t,"FACT CARD",body,-1,0,0);pressed=0;}
static int fxSc,fxFuelL;static float fxX0,fxVX0,fxVY0,fxH0;static const char*fxWhy;
// ---------------------------------------------------------------- powered landing
static void fx_land_bg(int w,int near){
 if(w==0){fr_bgspace();fx_disc_fb(214,52,9,RGB(40,90,200),RGB(200,220,255));}
 else for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB((150+y/3>255?255:150+y/3),110+y/5,80+y/8));
 int gy=146;u16 gc=w==0?RGB(120,120,126):RGB(170,88,50),gd=w==0?RGB(80,80,86):RGB(120,60,34);
 m3_fill(0,gy,240,14,gc);for(int i=0;i<40;i++){int x=(i*61+w*17)%236,y=gy+2+(i*7)%11;m3_fill(x,y,3+(i&3),1,gd);}
 int pw=near?(w==0?20:w==1?30:15)*2/3:(w==0?20:w==1?30:15)/4;if(pw<2)pw=2;
 m3_fill(120-pw,gy-1,pw*2,2,w==2?RGB(200,200,205):w==0?RGB(190,190,150):RGB(230,200,120));
 if(!near){for(int i=0;i<8;i++){int x=(i*53+11)%240;if(iabs(x-120)<16)continue;int h=3+(i*5)%6;m3_fill(x,gy-h,6+(i&3)*2,h,gd);}}
 else{for(int i=0;i<5;i++){int x=(i*47+20)%240;if(iabs(x-120)<pw+12)continue;m3_fill(x,gy-6,10,6,gd);m3_fill(x+2,gy-8,6,2,gd);}}
 ltx(4,150,w==0?"SHACKLETON RIM":w==1?"JEZERO CRATER":"ARCADIA PLANITIA",RGB(230,230,230));fr_save_bg();}
static int fx_land(int w){ // 0 = lunar lander, 1 = Mars sky crane, 2 = Starship belly-flop + flip (Mars). returns score>0 or 0
 float g=w?3.71f:1.62f,T=w==0?4.6f:w==1?8.5f:12.0f,h=fxH0,x=fxX0,vy=fxVY0,vx=fxVX0,fuel=w==0?95:w==1?55:60,f0=fuel,dt=1.0f/30;
 int ang=w==2?64:0,thr=0,res=-1,endT=0,near=0,flip=w==2?0:1,flipT=0,crane=0,cut=0,craneH=0,fr=0;float rov=0;int pad=w==0?20:w==1?30:15;
 fx_land_bg(w,0);fr_vstart();bgm_t(TR_ASCENT);fxWhy="";
 for(;;fr++){BEAT->screen=w?SCR_MARS:SCR_MOON;BEAT->phase=1;BEAT->scoreA=fx_i(h);BEAT->scoreB=fx_i(vy*10);BEAT->x=fx_i(x);BEAT->y=fx_i(fuel);
  if(res<0){
   int au=FR_AUTO;int ki=keys;
   if(!flip){ // belly-flop: no thrust, aerodynamic terminal velocity
    vy+=(-g+0.0004f*vy*vy)*dt;vx*=0.995f;if((au&&h<800)||(!au&&(pressed&K_A))){flip=1;flipT=fr;sfx_whoosh();lmsg("FLIP! ENGINES RELIGHT",90);if(h>1150)lmsg("EARLY FLIP - FUEL WASTED",90),fuel-=10;}
    if(h<320){res=0;fxWhy="NO FLIP - IMPACT";}}
   else if(w==2&&ang>0){ang-=1;thr=100;}
   else if(crane){ // sky crane: automatic 0.75 m/s descent, player cuts the bridles at touchdown
    vy=-0.75f;vx*=0.9f;thr=40;float rh=h-7.5f;if(rh<=0)rov+=dt;
    if((au&&rov>0.4f)||(!au&&(pressed&K_A))){cut=1;if(rh>0.6f){res=0;fxWhy="CUT TOO HIGH - ROVER DROPPED";}else{res=1;lmsg("ROVER DOWN! FLYAWAY",150);sfx_ok();}}
    if(rov>1.6f){res=0;fxWhy="LATE CUT - STAGE DRAGGED ROVER";}}
   if(flip&&!crane&&!(w==2&&ang>0)){
    if(au){float vxt=-x*0.12f;if(vxt>25)vxt=25;if(vxt<-25)vxt=-25;float vyt=-(1.2f+h*0.07f);if(vyt<-45)vyt=-45;if(w==1&&h<40)vyt=-(0.9f+h*0.05f);
     float ax=(vxt-vx)*0.6f,ay=g+(vyt-vy)*1.4f;if(ay<0.2f)ay=0.2f;int at=fx_i(ax/ay*40.7f);at=clampi(at,-36,36);if(ang<at)ang++;else if(ang>at)ang--;
     float aq=fx_sqrt(ax*ax+ay*ay);thr=clampi(fx_i(aq/T*100),0,100);}
    else{if(ki&K_UP)thr+=3;if(ki&K_DOWN)thr-=3;if(ki&K_A)thr=100;thr=clampi(thr,0,100);if(ki&K_LEFT)ang--;if(ki&K_RIGHT)ang++;ang=clampi(ang,-40,40);}}
   if(fuel<=0){thr=0;fuel=0;}
   if(flip){float a=T*thr/100.0f;vx+=a*SIN(ang)/256.0f*dt;vy+=(a*COS(ang)/256.0f-g)*dt;fuel-=thr/100.0f*dt*(w==1&&crane?0.5f:1);}
   if(!crane)x+=vx*dt;h+=vy*dt;
   if(w==1&&!crane&&h<=21&&vy>-3.5f&&iabs(fx_i(x))<pad+20){crane=1;craneH=fr;lmsg("SKY CRANE - A: CUT BRIDLES AT TOUCHDOWN",200);}
   if(w!=1&&h<=0&&res<0){h=0;int ok=vy>-(w==2?3.2f:2.6f)&&vx<1.6f&&vx>-1.6f&&iabs(ang)<=8;int onpad=iabs(fx_i(x))<=pad;
    if(!ok){res=0;fxWhy=vy<-(w==2?3.2f:2.6f)?"HARD LANDING":iabs(ang)>8?"TIPPED OVER":"SKIDDED";}else{res=1;if(!onpad)fxWhy="OFF THE TARGET ZONE";sfx_ok();lmsg(w==0?"CONTACT LIGHT... THE EAGLE-STYLE TOUCHDOWN!":"TOUCHDOWN - SHIP SAFED",150);}
    if(res==1)fxSc=3000+fx_i(fuel/f0*2000)+(onpad?1500-iabs(fx_i(x))*40:0)-fx_i(-vy*200);}
   if(w==1&&h<=0&&res<0){res=0;fxWhy="DESCENT STAGE CRASHED";}
   if(res==1&&w==1)fxSc=3000+fx_i(fuel/f0*2000)+1500-iabs(fx_i(x))*30;
   if(res>=0){endT=fr;if(!res){shake(14);sfx_boom();fsp_add(120+(fx_i(x)*(near?3:1))/(near?2:6),144,10,RGB(255,180,60));}}}
  else if(fr-endT>150)break;
  if(!near&&h<170&&res<0){near=1;fr_vstop();fx_land_bg(w,1);fr_vstart();}
  next_frame();fr_vrestore();
  int px,py;if(near){px=120+fx_i(x*1.5f);py=146-fx_i(h*0.75f);}else{px=120+fx_i(x/6);py=146-fx_i(h/14);}
  if(py<6)py=6;if(px<4)px=4;if(px>236)px=236;
  // vehicle
  if(w==0){fx_body(px,py-5,ang,3,8,RGB(210,170,60));fx_body(px+(SIN(ang)*7>>8),py-10-(0),ang,2,6,RGB(170,170,180));lf(px-6,py-1,2,2,RGB(200,200,200));lf(px+4,py-1,2,2,RGB(200,200,200));}
  else if(w==1){lf(px-7,py-6,14,4,RGB(200,200,210));lf(px-8,py-7,2,2,RGB(160,160,170));lf(px+6,py-7,2,2,RGB(160,160,170));
   if(crane){int rpy=py+(near?fx_i(7.5f*0.75f):2);if(!cut){lf(px-4,py-2,1,rpy-py,RGB(230,230,230));lf(px+3,py-2,1,rpy-py,RGB(230,230,230));}lf(px-5,rpy-1,10,3,RGB(230,230,240));lf(px-6,rpy+2,3,2,RGB(60,60,60));lf(px+3,rpy+2,3,2,RGB(60,60,60));}
   else{lf(px-5,py-2,10,3,RGB(230,230,240));}}
  else{fx_body(px,py-12,ang,10,5,RGB(215,218,228));fx_body(px+(SIN(ang)*-9>>8),py-12+(COS(ang)*9>>8),ang,1,8,RGB(120,120,130));}
  if(flip&&thr>0&&fuel>0&&res<0){int L=3+thr/14;int s=SIN(ang),c=COS(ang);int bx=px-(s*(w==2?22:4)>>8),by=py-(w==2?12:5)+(c*(w==2?10:4)>>8)+(w==2?0:3);for(int k=0;k<L;k+=2)lf(bx-(s*k>>8)-1,by+(c*k>>8),3,2,(k+fr)&2?RGB(255,220,120):RGB(255,140,40));}
  if(w&&h<60&&thr>30&&res<0&&near)for(int k=0;k<3;k++)lf(px-20+rr(40),145-rr(4),2,1,RGB(220,170,120));
  fsp_run();
  // HUD
  ltx(4,3,cat("ALT ",itoa(fx_i(h))),C_WHITE);ltx(4,12,cat("V/S ",itoa(fx_i(vy*10))),vy<-(w==2?3.2f:2.6f)&&h<60?C_RED:C_WHITE);ltx(4,21,cat("H/S ",itoa(fx_i(vx*10))),C_WHITE);
  ltx(150,3,"FUEL",C_GRAY);fr_bar(180,4,50,fx_i(fuel),fx_i(f0),fuel<f0/5?C_RED:C_GREEN);ltx(150,12,"THR",C_GRAY);fr_bar(180,13,50,thr,100,C_YEL);
  ltx(150,21,cat("TGT ",itoa(fx_i(x))),iabs(fx_i(x))<=pad?C_GREEN:C_YEL);
  if(w==2&&!flip)ltc(36,h<1150?"A: FLIP AND LAND!":"BELLY-FLOP - WAIT FOR 1100 M",h<1150?C_YEL:C_WHITE);
  if(crane&&!cut)ltc(36,h-7.5f<=0?"TOUCHDOWN! A: CUT":"LOWERING ROVER...",h-7.5f<=0?C_GREEN:C_WHITE);
  if(res==0)ltc(48,fxWhy,C_RED);
  if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}
  if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();fxFuelL=fx_i(fuel);return res==1?(fxSc>100?fxSc:100):0;}
// ---------------------------------------------------------------- Mars entry: guided lifting entry, chute, heat shield
typedef struct{float h,v,gm,s;}EdlS;
static void edl_step(EdlS*S,int u,float dt,float beta,float ld,float*Dout){float rho=0.020f*fx_exp(-S->h/11100.0f);float D=0.5f*rho*S->v*S->v/beta;float L=ld*D;
 float sg=S->gm-S->gm*S->gm*S->gm/6,cg=1-S->gm*S->gm/2+S->gm*S->gm*S->gm*S->gm/24; // small-angle series, |gm| < 0.5 rad
 if(S->gm<-1.2f){sg=-0.93f;cg=0.36f;}
 S->v+=(-D-3.71f*sg)*dt;if(S->v<5)S->v=5;S->gm+=((L/S->v)*u-(3.71f/S->v-S->v/(3389500.0f+S->h))*cg)*dt;if(S->gm<-1.5f)S->gm=-1.5f;S->h+=S->v*sg*dt;S->s+=S->v*cg*dt;if(Dout)*Dout=D;}
static float edl_pred(const EdlS*S0,int u,float*pk){EdlS S=*S0;float D=0,m=0;for(int i=0;i<160;i++){edl_step(&S,u,3.0f,120,0.24f,&D);if(D>m)m=D;if(S.h>130000)return 1e9f;if(S.v<420||S.h<0)break;}if(pk)*pk=m;return S.s;}
static int mars_edl(float tgt,float g0deg){ // returns 1 when the backshell separates (state in fxH0/fxVY0/fxVX0/fxX0)
 EdlS S={125000,5800,g0deg*0.0174533f,0};int u=0,res=-1,endT=0,fr=0,chute=0,shield=1,ph=0;float pk=0,Q=0,pred=0,D=0,dt=0.25f;fxWhy="";
 fr_bgspace();
 for(int y=100;y<160;y++){int dy=y-620;int hw=isq(520*520-dy*dy);if(hw>0){int x0=120-hw;if(x0<0)x0=0;int x1=120+hw;if(x1>240)x1=240;if(x1>x0)m3_fill(x0&~1,y,(x1-x0+1)&~1,1,RGB(150+(y-100),80+(y-100)/2,46));}}
 for(int i=0;i<=9;i++){int x=14+i*24;m3_fill(x,150,1,4,RGB(250,230,200));}
 int tx=14+fx_i(tgt/1000.0f*216/900);m3_fill(tx-3,145,7,3,C_GREEN);m3_text(tx-10,136,"TGT",C_GREEN,1);
 fr_save_bg();fr_vstart();bgm_t(TR_ASCENT);
 for(;;fr++){BEAT->screen=SCR_MARS;BEAT->phase=2;BEAT->scoreA=fx_i(S.h);BEAT->scoreB=fx_i(S.v);BEAT->x=fx_i(S.s/1000);BEAT->y=fx_i(pk*10/9.81f);
  if(res<0){int au=FR_AUTO;
   if(ph==0){if(fr%30==0){if(au){float best=1e12f;int bu=0;for(int c=1;c>=-1;c--){float p=0,sp=edl_pred(&S,c,&p);float e=(sp>tgt?sp-tgt:tgt-sp)+(p>14*9.81f?1e7f:0);if(e<best){best=e;bu=c;}}u=bu;}pred=edl_pred(&S,u,0);}
    if(!au){if(pressed&K_UP){u=u<1?u+1:1;sfx_menu();}if(pressed&K_DOWN){u=u>-1?u-1:-1;sfx_menu();}}
    edl_step(&S,u,dt,120,0.24f,&D);float rho=0.020f*fx_exp(-S.h/11100.0f);float q=fx_sqrt(rho)*(S.v/1000)*(S.v/1000)*(S.v/1000);Q+=q*dt;if(D>pk)pk=D;
    if(S.h>130000&&fr>60){res=0;fxWhy="SKIPPED OUT OF THE ATMOSPHERE";}
    if(pk>15*9.81f){res=0;fxWhy="OVER 15 G - STRUCTURE FAILED";}
    if(Q>250){res=0;fxWhy="HEAT SHIELD BURNED THROUGH";}
    if(S.v<480)ph=1;}
   else{ // supersonic chute + heat shield, then backshell separation
    if(!chute){if((au&&S.v<440)||(!au&&(pressed&K_A))){if(S.v>480){res=0;fxWhy="CHUTE SHREDDED - TOO FAST";}else{chute=1;sfx_whoosh();lmsg("PARACHUTE DEPLOYED",120);}}if(S.v<280&&!chute){res=0;fxWhy="NO CHUTE - TOO LOW AND FAST";}}
    else if(shield&&S.v<165&&((au&&S.v<150)||(!au&&(pressed&K_A)))){shield=0;sfx_shot(0);lmsg("HEAT SHIELD AWAY - RADAR LOCK",120);}
    edl_step(&S,0,dt,chute?11:120,0,&D);
    if(shield&&S.h<4000&&res<0){res=0;fxWhy="HEAT SHIELD BLOCKED THE RADAR";}
    if(S.h<1800&&!shield&&res<0){res=1;lmsg("BACKSHELL SEPARATION - POWERED DESCENT",150);}
    if(S.h<0&&res<0){res=0;fxWhy="IMPACT";}}
   if(res>=0){endT=fr;if(!res){sfx_boom();shake(10);}}}
  else if(fr-endT>(res?40:150))break;
  next_frame();fr_vrestore();
  int px=14+fx_i(S.s/1000.0f*216/900),py=148-fx_i(S.h*118/130000.0f);if(px>236)px=236;if(py<20)py=20;
  if((fr&7)==0&&res<0&&px>=0&&px<240&&py>=0&&py<160){u16 c=RGB(255,200,90);bigbuf[py*240+px]=c;fb[py*240+px]=c;}
  if(ph==0&&D>20&&res<0){int L=2+fx_i(D/12);if(L>10)L=10;for(int k=0;k<L;k++)lf(px+2+k,py-2+((k*5+fr)&3),2,1,(k+fr)&1?RGB(255,240,180):RGB(255,120,40));}
  lf(px-3,py-2,6,4,RGB(200,200,210));lf(px-4,py+1,8,1,shield?RGB(140,90,60):RGB(200,200,210));
  if(chute&&res<0){lf(px-7,py-12,15,3,C_WHITE);lf(px-6,py-9,1,7,RGB(230,230,230));lf(px+6,py-9,1,7,RGB(230,230,230));}
  ltx(4,3,cat3("V ",itoa(fx_i(S.v))," M/S"),C_WHITE);{int hh=fx_i(S.h/100);if(hh<0)hh=0;ltx(4,12,cat(fr_s2("ALT ",hh/10,".",hh%10)," KM"),C_WHITE);}
  {int gg=fx_i(pk/9.81f*10);ltx(4,21,cat(fr_s2("PEAK G ",gg/10,".",gg%10),""),pk>12*9.81f?C_RED:C_WHITE);}
  ltx(120,3,"HEAT",C_GRAY);fr_bar(150,4,80,fx_i(Q),250,Q>200?C_RED:RGB(255,160,60));
  if(ph==0){ltx(120,12,u>0?"LIFT UP":u<0?"LIFT DOWN":"LIFT NEUTRAL",C_YEL);ltx(120,21,fr_s2("PRED ",fx_i(pred/1000)," / TGT ",fx_i(tgt/1000)),iabs(fx_i((pred-tgt)/1000))<15?C_GREEN:C_YEL);ltx(4,30,"UP/DN: BANK",C_GRAY);}
  else{ltx(120,12,chute?"CHUTE OPEN":S.v<480?"A: DEPLOY CHUTE!":"",chute?C_GREEN:C_YEL);if(chute&&shield)ltx(120,21,S.v<165?"A: DROP HEAT SHIELD":"SLOWING...",S.v<165?C_YEL:C_WHITE);}
  if(res==0)ltc(60,fxWhy,C_RED);if(lMsgT>0){lMsgT--;ltc(70,lMsg,C_YEL);}if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();float err=(S.s-tgt);fxH0=1800;fxVY0=-S.v*0.9f;if(fxVY0<-85)fxVY0=-85;fxVX0=12;fxX0=clampi(fx_i(err/40),-500,500)-150;
 fxSc=res==1?2000+clampi(1500-iabs(fx_i(err/100)),0,1500)+(pk<12*9.81f?500:0):0;return res==1;}
// ---------------------------------------------------------------- rover drive (flip-screen)
static u8 rvH[240] EWRAM;static s16 rvRock[4],rvSam[3];static u8 rvSamOn[3],rvGot;
static int rv_hgen(int wx,int w){int h=118+(SIN((wx*3/2)&255)*7>>8)+(SIN((wx/4+w*40)&255)*14>>8)+(SIN((wx*5)&255)*2>>8);int c=(wx%480)-300;if(c>-30&&c<30){h+=10-c*c/90;}else if(c>-36&&c<36)h-=3;return clampi(h,96,148);}
static void rv_screen(int sc,int w,int dark,int storm,int last){
 for(int x=0;x<240;x++)rvH[x]=rv_hgen(sc*240+x,w);
 if(w==0){fr_bgspace();if(!dark)fx_disc_fb(214,44,8,RGB(40,90,200),RGB(200,220,255));}
 else for(int y=0;y<160;y++)m3_fill(0,y,240,1,storm?RGB(130+y/4,90+y/6,70):RGB((190+y/5>255?255:190+y/5),140+y/6,100+y/8));
 for(int x=0;x<240;x++){int h=rvH[x];int d=dark&&((sc*240+x)%480)>200&&((sc*240+x)%480)<400;u16 c=w==0?(d?RGB(38,38,44):RGB(130,130,134)):RGB(176,92,52);for(int y=h;y<160;y++)fb[y*240+x]=y==h?(w==0?(d?RGB(60,60,70):RGB(180,180,186)):RGB(210,120,70)):c;}
 for(int i=0;i<4;i++){int x=30+((sc*37+i*61)%180);rvRock[i]=x;int h=rvH[x];m3_fill(x-3,h-5,6,5,w==0?RGB(90,90,96):RGB(110,58,36));m3_fill(x-2,h-6,4,1,w==0?RGB(150,150,156):RGB(150,80,50));}
 for(int i=0;i<3;i++){rvSamOn[i]=0;rvSam[i]=0;}
 int ns=(sc%2==0)?2:1;for(int i=0;i<ns;i++){int x=50+i*110+((sc*29)%30);for(int k=0;k<4;k++)if(iabs(x-rvRock[k])<14)x+=18;rvSam[i]=x;if((rvGot>>i)&1)continue;rvSamOn[i]=1;int h=rvH[x];m3_fill(x,h-12,1,12,C_WHITE);m3_fill(x+1,h-12,5,4,w==0&&dark?RGB(120,220,255):C_YEL);}
 if(last){m3_fill(220,rvH[220]-18,2,18,C_WHITE);m3_fill(222,rvH[220]-18,10,6,C_GREEN);}
 ltx(4,150,fr_s2("SECTOR ",sc+1,"",-99999),RGB(230,230,230));fr_save_bg();}
static int fx_rover(int w,int nsc,int need,int dark,int storm){ // returns score or 0
 int sc=0,x=12<<8,v=0,bat=100<<8,samp=0,hull=3,res=-1,endT=0,fr=0,drill=0,inv=0;fxWhy="";
 rvGot=0;rv_screen(0,w,dark,storm,nsc==1);fr_vstart();bgm_t(w==0?TR_SPACE:TR_DUNE);
 for(;;fr++){int px=x>>8;BEAT->screen=w?SCR_MARS:SCR_MOON;BEAT->phase=1;BEAT->scoreA=sc;BEAT->scoreB=samp;BEAT->x=px;BEAT->y=bat>>8;
  if(res<0){int au=FR_AUTO,ki=keys,thr=0,brk=0,act=0;
   int ns=-1,nd=999;for(int i=0;i<3;i++)if(rvSamOn[i]){int d=rvSam[i]-px;if(d>-4&&d<nd){nd=d;ns=i;}}
   if(au){int tv=200;if(ns>=0&&nd<40)tv=nd>2?24+nd*4:0;for(int k=0;k<4;k++){int d=rvRock[k]-px;if(d>-6&&d<20)tv=tv>70?70:tv;}
    if(drill)tv=0;if(v<tv-10)thr=1;else if(v>tv+10)brk=1;if(ns>=0&&iabs(nd)<=4&&v<30&&!drill)act=1;}
   else{thr=(ki&K_RIGHT)!=0;brk=(ki&K_LEFT)!=0;act=(pressed&K_A)!=0;}
   if(drill){drill--;if(drill==0){samp++;sfx_ok();lmsg(w==0?(dark?"ICE CORE STORED!":"REGOLITH SAMPLE STORED"):"SAMPLE TUBE SEALED",90);if(ns>=0)rvGot|=1<<ns;
     fr_vstop();rv_screen(sc,w,dark,storm,sc==nsc-1);fr_vstart();}}
   else{if(act&&ns>=0&&iabs(nd)<=5&&v<40){drill=50;v=0;bat-=4<<8;sfx_shot(0);}
    if(thr)v+=12;if(brk)v-=14;if(!thr&&!brk)v-=v>0?3:0;
    int dh=(int)rvH[clampi(px+4,0,239)]-(int)rvH[clampi(px-4,0,239)];v+=clampi(dh*3/4,-7,7);v=clampi(v,-60,storm?170:230);
    for(int k=0;k<4;k++){int d=rvRock[k]-px;if(d>0&&d<6&&v>120&&!inv){hull--;inv=60;v=0;sfx_hurt();shake(6);lmsg("ROCK STRIKE - SLOW DOWN OVER ROCKS",90);}}
    x+=v;if(x<4<<8){x=4<<8;v=0;}}
   if(inv)inv--;
   bat-=(thr?6:1)+(dark&&((sc*240+px)%480)>200&&((sc*240+px)%480)<400?4:0);if(!dark&&!storm)bat+=w==0?4:3;if(storm)bat+=1;if(bat>100<<8)bat=100<<8;
   if(px>=234){if(sc+1>=nsc){if(samp>=need){res=1;sfx_ok();}else{res=0;fxWhy="NOT ENOUGH SAMPLES";}}else{sc++;x=6<<8;rvGot=0;fr_vstop();rv_screen(sc,w,dark,storm,sc==nsc-1);fr_vstart();}}
   if(bat<=0){bat=0;res=0;fxWhy="BATTERY FLAT - ROVER STRANDED";}if(hull<=0){res=0;fxWhy="WHEELS DESTROYED";}
   if(res>=0)endT=fr;}
  else if(fr-endT>120)break;
  next_frame();fr_vrestore();
  px=x>>8;int y1=rvH[clampi(px-5,0,239)],y2=rvH[clampi(px+5,0,239)],yb=(y1+y2)/2;
  u16 bc=w==0?RGB(230,230,236):RGB(236,236,240);lf(px-6,yb-7,13,4,(inv&4)?C_RED:bc);lf(px+3,yb-12,1,5,RGB(150,150,160));lf(px+2,yb-13,3,2,RGB(60,60,70));
  if(w==1)lf(px-5,yb-9,4,2,RGB(40,40,50));else lf(px-5,yb-9,6,2,RGB(200,170,60));
  for(int k=-1;k<=1;k++){int wx=px+k*5;int wy=rvH[clampi(wx,0,239)];lf(wx-1,wy-3,3,3,RGB(40,40,44));}
  if(dark)for(int k=0;k<10;k++)lf(px+8+k*2,yb-6+k/3,2,1+k/3,RGB(255,255,200));
  if(drill){lf(px,yb-3,1,4+(drill&3),RGB(200,200,200));if(fr&2)fsp_add(px,yb,1,RGB(200,200,200));}
  if(storm&&(fr&1))for(int k=0;k<14;k++)lf(rr(240),20+rr(120),3,1,RGB(220,170,120));
  fsp_run();
  ltx(4,3,"BATT",C_GRAY);fr_bar(32,4,44,bat>>8,100,bat<(20<<8)?C_RED:C_GREEN);ltx(84,3,fr_s2("SAMPLES ",samp,"/",need),samp>=need?C_GREEN:C_YEL);
  ltx(170,3,fr_s2("SEC ",sc+1,"/",nsc),C_WHITE);ltx(4,12,cat("HULL ",itoa(hull)),hull>1?C_WHITE:C_RED);ltx(60,12,cat3("SPD ",itoa(v*36/256)," KM/H"),C_WHITE);
  if(fr<200&&res<0)ltc(30,"RIGHT: DRIVE  LEFT: BRAKE  A: DRILL AT FLAG",C_WHITE);
  if(res==0)ltc(48,fxWhy,C_RED);if(res==1)ltc(48,"TRAVERSE COMPLETE",C_GREEN);if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();return res==1?2000+samp*600+(bat>>8)*15+hull*300:0;}
// ---------------------------------------------------------------- lunar base builder (Artemis-era south pole outpost)
enum{BB_NONE,BB_HAB,BB_SOLAR,BB_BATT,BB_ISRU,BB_FISS,BB_N};
static const char*const BBN[BB_N]={"CLEAR","HABITAT","SOLAR ARRAY","BATTERY","ISRU PLANT","FISSION 10KW"};
static const u8 BBC[BB_N]={0,8,3,2,6,9};
static const char*const BBD[BB_N]={"B: REMOVE","+2 CREW, -1 PWR","+2 PWR IN SUN, X2 ON RIM","+12 STORAGE","-2 PWR, +3 O2 NEXT TO ICE","+3 PWR DAY AND NIGHT"};
static u8 bbG[4][9];
#define BB_X 12
#define BB_Y 50
static int bb_ice(int c,int r){return r>=2&&c<=1;}
static int bb_adjice(int c,int r){for(int dc=-1;dc<=1;dc++)for(int dr=-1;dr<=1;dr++)if(bb_ice(c+dc,r+dr)&&c+dc>=0&&r+dr<4)return 1;return 0;}
static void bb_icon(int c,int r,int t){int x=BB_X+c*24,y=BB_Y+r*18;
 switch(t){case BB_HAB:m3_fill(x+4,y+6,16,9,RGB(230,230,236));m3_fill(x+6,y+4,12,2,RGB(230,230,236));m3_fill(x+8,y+9,2,2,RGB(80,160,255));m3_fill(x+14,y+9,2,2,RGB(80,160,255));break;
  case BB_SOLAR:for(int k=0;k<3;k++)m3_fill(x+3+k*6,y+3,5,9,RGB(40,70,170));m3_fill(x+11,y+12,2,4,RGB(170,170,170));break;
  case BB_BATT:m3_fill(x+6,y+5,12,10,RGB(90,90,100));m3_fill(x+8,y+7,8,2,C_GREEN);m3_fill(x+8,y+11,8,2,C_GREEN);break;
  case BB_ISRU:m3_fill(x+4,y+7,14,8,RGB(200,120,60));m3_fill(x+16,y+3,3,6,RGB(160,160,170));m3_fill(x+6,y+9,6,2,RGB(255,220,120));break;
  case BB_FISS:m3_fill(x+8,y+3,8,12,RGB(170,170,180));for(int k=0;k<4;k++)m3_fill(x+2+k*5,y+14,3,2,RGB(120,120,130));m3_fill(x+10,y+6,4,4,RGB(255,200,60));break;}}
static void bb_draw(int cr,int cc,int sel,int cp,int mis,const char*msg){fr_bgspace();fx_disc_fb(206,20,8,RGB(40,90,200),RGB(200,220,255));
 for(int y=40;y<160;y++)m3_fill(0,y,240,1,RGB(110+(y-40)/6,110+(y-40)/6,116+(y-40)/6));
 for(int r=0;r<4;r++)for(int c=0;c<9;c++){int x=BB_X+c*24,y=BB_Y+r*18;u16 base=r==0?RGB(160,160,150):bb_ice(c,r)?RGB(50,54,70):RGB(128,128,134);m3_fill(x,y,23,17,base);
  if(bb_ice(c,r))for(int k=0;k<4;k++)m3_fill(x+3+k*5,y+4+(k&1)*6,3,2,RGB(140,200,255));if(r==0)m3_fill(x,y,23,1,RGB(230,220,160));bb_icon(c,r,bbG[r][c]);}
 int x=BB_X+cc*24,y=BB_Y+cr*18;m3_fill(x-1,y-1,25,1,C_YEL);m3_fill(x-1,y+17,25,1,C_YEL);m3_fill(x-1,y-1,1,19,C_YEL);m3_fill(x+23,y-1,1,19,C_YEL);
 m3_text(4,4,mis==1?"OUTPOST: 2 CREW, 2 LUNAR DAYS":"LUNAR NIGHT: 4 CREW, 3 DAYS",C_YEL,1);m3_text(4,14,cat("CARGO LEFT ",itoa(cp)),C_WHITE,1);
 m3_text(100,14,cat3("L/R: ",BBN[sel]," "),C_WHITE,1);m3_text(4,24,cat3(BBD[sel],"  COST ",itoa(BBC[sel])),RGB(150,210,240),1);
 m3_text(4,124,"ROW 1 = SUNLIT RIM  DARK = ICE (PSR)",RGB(200,200,200),1);m3_text(4,134,"A: BUILD  B: CLEAR  START: RUN SIM",C_GRAY,1);if(msg)m3_text(4,144,msg,C_RED,1);m3_present();}
static int mb_build(int mis){ // returns score or 0
 int cp=mis==1?30:50,crewNeed=mis==1?2:4,days=mis==1?2:3,nightT=mis==1?10:12;for(int r=0;r<4;r++)for(int c=0;c<9;c++)bbG[r][c]=0;
 int cr=1,cc=4,sel=BB_HAB,redraw=1;const char*msg=0;bgm_t(TR_SPACE);
 static const u8 P1[][3]={{1,4,BB_HAB},{0,3,BB_SOLAR},{0,5,BB_SOLAR},{2,2,BB_ISRU},{2,4,BB_BATT},{2,5,BB_BATT},{2,6,BB_BATT},{0,6,BB_SOLAR}};
 static const u8 P3[][3]={{1,4,BB_HAB},{1,5,BB_HAB},{2,2,BB_ISRU},{3,2,BB_ISRU},{1,6,BB_FISS},{1,7,BB_FISS},{2,5,BB_BATT},{2,6,BB_BATT}};
 int ap=0,apT=0;
 for(int fr=0;;fr++){BEAT->screen=SCR_MOON;BEAT->phase=1;BEAT->scoreA=cp;BEAT->menu=sel;
  if(redraw){redraw=0;bb_draw(cr,cc,sel,cp,mis,msg);}
  next_frame();int go=0;
  if(FR_AUTO){if(++apT>=8){apT=0;const u8(*P)[3]=mis==1?P1:P3;int n=mis==1?8:8;if(ap<n){cr=P[ap][0];cc=P[ap][1];sel=P[ap][2];if(BBC[sel]<=cp&&!bbG[cr][cc]){bbG[cr][cc]=sel;cp-=BBC[sel];}ap++;redraw=1;}else go=1;}}
  else{if(pressed&K_LEFT){cc=(cc+8)%9;redraw=1;}if(pressed&K_RIGHT){cc=(cc+1)%9;redraw=1;}if(pressed&K_UP){cr=(cr+3)%4;redraw=1;}if(pressed&K_DOWN){cr=(cr+1)%4;redraw=1;}
   if(pressed&K_L){sel=sel>1?sel-1:BB_N-1;if(sel==BB_FISS&&mis==1)sel--;redraw=1;sfx_menu();}if(pressed&K_R){sel=sel<BB_N-1?sel+1:1;if(sel==BB_FISS&&mis==1)sel=1;redraw=1;sfx_menu();}
   if(pressed&K_A){msg=0;if(bbG[cr][cc])msg="OCCUPIED - B TO CLEAR";else if(bb_ice(cc,cr))msg="ICE FIELD - BUILD NEXT TO IT";else if(BBC[sel]>cp)msg="NOT ENOUGH CARGO";else{bbG[cr][cc]=sel;cp-=BBC[sel];sfx_ok();}redraw=1;}
   if(pressed&K_B){if(bbG[cr][cc]){cp+=BBC[bbG[cr][cc]];bbG[cr][cc]=0;sfx_menu();redraw=1;}else{int q=0;guide_tip("LEAVE THE BASE?","PRESS B AGAIN ON AN EMPTY TILE TO ABORT. A RETURNS TO BUILDING.");for(int t=0;t<240;t++){next_frame();if(pressed&K_B){q=1;break;}if(pressed&K_A)break;}if(q)return -1;redraw=1;}}
   if(pressed&K_START)go=1;}
  if(go)break;}
 // ---- simulate the lunar days
 int hab=0,sol=0,solR=0,bat=0,isru=0,isruI=0,fis=0;for(int r=0;r<4;r++)for(int c=0;c<9;c++){int t=bbG[r][c];hab+=t==BB_HAB;if(t==BB_SOLAR){sol++;if(r==0)solR++;}bat+=t==BB_BATT;fis+=t==BB_FISS;if(t==BB_ISRU){isru++;isruI+=bb_adjice(c,r);}}
 int crew=hab*2>crewNeed?crewNeed:hab*2,cap=6+bat*12,st=cap/2,o2=12,blk=0,res=-1;fxWhy="";const int dayT=24;int minO2=99,minSt=999;
 if(crew<crewNeed){fxWhy="NOT ENOUGH HABITAT FOR THE CREW";res=0;}
 for(int t=0;t<days*dayT&&res<0;t++){int ph=t%dayT,day=ph<dayT-nightT;
  int gen=(day?(sol*2+solR*2):0)+fis*3,use=hab+isru*2;st+=gen-use;int pow=1;if(st<0){st=0;blk++;pow=0;}else blk=0;if(st>cap)st=cap;
  o2+=(pow?isruI*3:0)-crew;if(o2>40)o2=40;if(o2<minO2)minO2=o2;if(st<minSt)minSt=st;
  if(o2<=0){res=0;fxWhy="OXYGEN DEPLETED - CREW EVACUATED";}if(blk>=3){res=0;fxWhy="BLACKOUT - THE BASE FROZE";}
  BEAT->phase=2;BEAT->scoreA=t;BEAT->scoreB=o2;BEAT->x=st;
  for(int y=0;y<40;y++)m3_fill(0,y,240,1,day?RGB(10,10,24):RGB(2,2,8));
  m3_text(4,4,fr_s2("LUNAR DAY ",t/dayT+1," / ",days),C_YEL,1);m3_text(120,4,day?"SUN UP":"LUNAR NIGHT",day?RGB(255,230,120):RGB(140,160,255),1);
  m3_text(4,14,fr_s2("POWER ",st," / ",cap),pow?C_WHITE:C_RED,1);m3_text(120,14,fr_s2("O2 ",o2," CREW ",crew),o2>4?C_WHITE:C_RED,1);
  m3_text(4,24,fr_s2("GEN ",gen,"  USE ",use),C_GRAY,1);m3_text(120,24,fr_s2("ISRU ON ICE ",isruI,"/",isru),C_GRAY,1);
  {int sx=8+ph*224/dayT;m3_fill(sx,34,4,4,day?RGB(255,220,80):RGB(60,60,90));}
  m3_present();for(int k=0;k<(FR_AUTO?4:12);k++)next_frame();}
 if(res<0)res=1;int sc=res?3000+minO2*40+minSt*20+cp*50+crew*300:0;
 if(res)lmsg("BASE SUSTAINED",120);return sc;}
// ---------------------------------------------------------------- DART-style kinetic impactor (aim mini-game)
static int dart_aim(int smallR,int orbR,int frames,int lateSwitch,int*missOut){ // returns hit quality 0..100
 int cx=150,cy=80,bigR=24,res=-1,fr=0,lock=lateSwitch?0:1,ax=cx<<8,ay=cy<<8,fuel=160;int mx=0,my=0;
 fr_bgspace();fx_disc_fb(cx,cy,bigR,RGB(120,110,100),RGB(170,160,150));for(int i=0;i<10;i++)fb[(cy-10+i*2)*240+cx-12+i*3]=RGB(90,84,76);
 m3_text(4,150,lateSwitch?"DRACO LOCK: DIDYMOS. A: SWITCH":"SMARTNAV: STEER TO THE GHOST",RGB(220,220,220),1);fr_save_bg();fr_vstart();
 for(;;fr++){int an=(fr*2/3+40)&255;mx=cx+(COS(an)*orbR>>8);my=cy+(SIN(an)*orbR*2/5>>8);int left=frames-fr;BEAT->phase=1;BEAT->scoreA=left;
  if(res<0){int tx=lock?mx:cx,ty=lock?my:cy;
   int fa=(frames*2/3+40)&255,gx=cx+(COS(fa)*orbR>>8),gy=cy+(SIN(fa)*orbR*2/5>>8);
   if(FR_AUTO){if(!lock&&fr>60){lock=1;sfx_menu();}if(lock&&fuel>0){int px=ax>>8,py=ay>>8;int dx=gx-px,dy=gy-py;if(dx>0){ax+=160;fuel--;}else if(dx<0){ax-=160;fuel--;}if(dy>0){ay+=160;fuel--;}else if(dy<0){ay-=160;fuel--;}}}
   else{if(!lock&&(pressed&K_A)){lock=1;sfx_menu();}int ki=keys;if(fuel>0&&lock){if(ki&K_LEFT){ax-=160;fuel--;}if(ki&K_RIGHT){ax+=160;fuel--;}if(ki&K_UP){ay-=160;fuel--;}if(ki&K_DOWN){ay+=160;fuel--;}}}
   if(!lock&&(ax>>8)!=cx)ax+=((cx<<8)-ax)/16;if(!lock)ay+=((cy<<8)-ay)/16;
   if(left<=0){int dx=(ax>>8)-mx,dy=(ay>>8)-my;int d=isq(dx*dx+dy*dy);*missOut=d;res=d<=smallR?100-d*100/(smallR+1):0;if(res>0){sfx_boom();shake(8);for(int k=0;k<4;k++)fsp_add(mx,my,8,RGB(230,220,200));}else sfx_hurt();}}
  else if(fr>frames+110)break;
  next_frame();fr_vrestore();
  if(res<=0||fr<frames+20)fx_disc(mx,my,smallR,RGB(150,140,128),RGB(200,190,176));
  int px=ax>>8,py=ay>>8;if(res<0){lf(px-8,py,5,1,C_GREEN);lf(px+4,py,5,1,C_GREEN);lf(px,py-8,1,5,C_GREEN);lf(px,py+4,1,5,C_GREEN);}
  fsp_run();ltx(4,3,cat3("IMPACT IN ",itoa(left>0?left/60:0)," S"),C_WHITE);ltx(120,3,lock?"TARGET: DIMORPHOS":"TARGET: DIDYMOS",lock?C_GREEN:C_YEL);ltx(4,12,"RCS",C_GRAY);fr_bar(28,13,40,fuel,160,C_BLUE);if(lock&&res<0){int fa=(frames*2/3+40)&255,gx=cx+(COS(fa)*orbR>>8),gy=cy+(SIN(fa)*orbR*2/5>>8);lf(gx-1,gy-smallR,2,1,RGB(255,120,255));lf(gx-1,gy+smallR,2,1,RGB(255,120,255));lf(gx-smallR,gy-1,1,2,RGB(255,120,255));lf(gx+smallR,gy-1,1,2,RGB(255,120,255));}
  if(res>0)ltc(30,"IMPACT! 6.1 KM/S",C_YEL);if(res==0)ltc(30,"MISS - FLEW PAST",C_RED);if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();return res<0?0:res;}
// ---------------------------------------------------------------- asteroid mining (Newtonian RCS, dock + drill)
static int ast_run(int mis){ // 0 rendezvous, 1 mine, 2 haul to depot
 fr_clear_pools();int sx=30<<8,sy=80<<8,vx=0,vy=0,fuel=mis==0?60<<8:100<<8,hull=3,cargo=0,deliv=0,res=-1,endT=0,fr=0,dock=-1,drillT=0,inv=0;fxWhy="";
 int need=mis==1?40:60,capC=mis==2?30:999;
 int nr=mis==0?1:6;for(int i=0;i<nr;i++){FE*e=&fe[i];e->on=1;e->a=mis==0?14:5+(i*7)%7;e->x=(mis==0?170:70+(i*31)%150)<<8;e->y=(mis==0?60:24+(i*47)%110)<<8;e->vx=mis==0?-24:((i*13)%9-4)*4;e->vy=mis==0?10:((i*7)%9-4)*4;e->hp=mis==0?0:8+(i*5)%12;e->t=0;}
 fr_bgspace();if(mis==2){m3_fill(0,64,14,32,RGB(160,160,170));m3_fill(14,76,8,8,RGB(200,200,90));m3_text(2,98,"DEPOT",C_WHITE,1);}fr_save_bg();fr_vstart();bgm_t(TR_FUTURE);
 for(;;fr++){int px=sx>>8,py=sy>>8;BEAT->screen=SCR_AST;BEAT->phase=1;BEAT->scoreA=mis==2?deliv:cargo;BEAT->scoreB=fuel>>8;BEAT->x=px;BEAT->y=py;
  if(res<0){int au=FR_AUTO,ki=keys,tx=0,ty=0,act=0,tg=-1;
   // target: nearest rock with ore (or the depot when full)
   int goDepot=mis==2&&(cargo>=capC||(cargo>0&&deliv+cargo>=need));
   if(!goDepot){int bd=1<<30;for(int i=0;i<nr;i++)if(fe[i].on&&(mis==0||fe[i].hp>0)){int dx=(fe[i].x>>8)-px,dy=(fe[i].y>>8)-py;int d=dx*dx+dy*dy;if(d<bd){bd=d;tg=i;}}}
   int tvx=0,tvy=0;if(tg>=0){FE*e=&fe[tg];tx=e->x>>8;ty=e->y>>8;tvx=e->vx;tvy=e->vy;}else{tx=26;ty=80;}
   if(au){int dx=tx-px,dy=ty-py;int d=isq(dx*dx+dy*dy);int stand=tg>=0?fe[tg].a+5:2;int sp=d>stand?clampi((d-stand)*6,0,180):0;int wvx=tvx+(d?dx*sp/d:0),wvy=tvy+(d?dy*sp/d:0);for(int j=0;j<nr;j++)if(j!=tg&&fe[j].on){int ox=px-(fe[j].x>>8),oy=py-(fe[j].y>>8);int od=isq(ox*ox+oy*oy);if(od<fe[j].a+16&&od>0){wvx+=ox*90/od+fe[j].vx;wvy+=oy*90/od+fe[j].vy;}}
    if(dock<0){if(vx<wvx-6)ki=K_RIGHT;else if(vx>wvx+6)ki=K_LEFT;else ki=0;if(vy<wvy-6)ki|=K_DOWN;else if(vy>wvy+6)ki|=K_UP;}else ki=0;act=1;}
   else act=(keys&K_A)!=0;
   if(dock<0&&fuel>0){if(ki&K_LEFT){vx-=4;fuel-=40;}if(ki&K_RIGHT){vx+=4;fuel-=40;}if(ki&K_UP){vy-=4;fuel-=40;}if(ki&K_DOWN){vy+=4;fuel-=40;}if(!au&&(keys&K_B)&&tg>=0){vx+=(tvx-vx)/8;vy+=(tvy-vy)/8;fuel-=30;}}
   if(fuel<0)fuel=0;
   if(dock>=0){FE*e=&fe[dock];vx=e->vx;vy=e->vy;
    if(mis==0){res=1;lmsg("SOFT CONTACT - TOUCH-AND-GO SAMPLE",150);sfx_ok();}
    else if(act&&e->hp>0){if(++drillT>=30){drillT=0;int take=e->hp<5?e->hp:5;if(cargo+take>capC)take=capC-cargo;e->hp-=take;cargo+=take;sfx_shot(0);fsp_add(px,py,3,RGB(200,180,120));if(take==0||e->hp<=0)dock=-1,sx-=vx*20,sy-=12<<8;}}
    else if(!act||e->hp<=0){dock=-1;}}
   for(int i=0;i<nr;i++){FE*e=&fe[i];if(!e->on)continue;e->x+=e->vx;e->y+=e->vy;int ex=e->x>>8,ey=e->y>>8;if(ex<e->a+16||ex>236-e->a)e->vx=-e->vx;if(ey<e->a+22||ey>156-e->a)e->vy=-e->vy;
    int dx=px-ex,dy=py-ey;int d=isq(dx*dx+dy*dy);if(d<e->a+4&&dock!=i){int rv=isq(((vx-e->vx)*(vx-e->vx)+(vy-e->vy)*(vy-e->vy)));
     if(rv<=(mis==0?40:56)&&act&&(mis==0||e->hp>0)&&cargo<capC){dock=i;drillT=0;}else if(!inv&&rv>96){hull--;inv=60;sfx_hurt();shake(6);lmsg("COLLISION - MATCH VELOCITY FIRST",100);}
     if(dock!=i){vx=e->vx+(d?dx*48/d:48);vy=e->vy+(d?dy*48/d:0);}}}
   if(dock<0){sx+=vx;sy+=vy;}else{sx=fe[dock].x-(fe[dock].x>>8<px?-(fe[dock].a+3)<<8:(fe[dock].a+3)<<8)*0;}
   if((sx>>8)<4){sx=4<<8;vx=-vx/2;}if((sx>>8)>236){sx=236<<8;vx=-vx/2;}if((sy>>8)<20){sy=20<<8;vy=-vy/2;}if((sy>>8)>156){sy=156<<8;vy=-vy/2;}
   if(mis==2&&px<26&&iabs(py-80)<14&&iabs(vx)<40&&iabs(vy)<40&&cargo>0){deliv+=cargo;cargo=0;fuel=100<<8;sfx_ok();lmsg("ORE DELIVERED - REFUELLED",90);}
   if(inv)inv--;
   if(mis==1&&cargo>=need){res=1;sfx_ok();}if(mis==2&&deliv>=need){res=1;sfx_ok();}
   if(hull<=0){res=0;fxWhy="HULL BREACHED";}if(fuel<=0&&dock<0&&res<0&&iabs(vx)<2&&iabs(vy)<2){res=0;fxWhy="OUT OF PROPELLANT";}if(fr>60*150&&res<0){res=0;fxWhy="LAUNCH WINDOW HOME CLOSED";}
   if(res>=0)endT=fr;}
  else if(fr-endT>120)break;
  next_frame();fr_vrestore();
  for(int i=0;i<nr;i++)if(fe[i].on){FE*e=&fe[i];int ex=e->x>>8,ey=e->y>>8;fx_disc(ex,ey,e->a,e->hp>0||mis==0?RGB(120,104,90):RGB(70,66,62),RGB(160,146,128));if(e->hp>0)lf(ex-1,ey-1,3,3,RGB(240,200,90));}
  px=sx>>8;py=sy>>8;lf(px-3,py-2,7,5,(inv&4)?C_RED:RGB(220,220,230));lf(px-6,py-1,3,3,RGB(60,100,200));lf(px+4,py-1,3,3,RGB(60,100,200));
  if(dock>=0&&mis){lf(px,py+3,1,3+(fr&2),RGB(200,200,200));}
  fsp_run();
  ltx(4,3,"RCS",C_GRAY);fr_bar(26,4,40,fuel>>8,mis==0?60:100,fuel<(15<<8)?C_RED:C_BLUE);ltx(74,3,cat("HULL ",itoa(hull)),hull>1?C_WHITE:C_RED);
  if(mis==0)ltx(120,3,"MATCH SPEED, A: CONTACT",C_YEL);else if(mis==1)ltx(120,3,fr_s2("ORE ",cargo,"/",need),C_YEL);else ltx(120,3,fr_s2("HOLD ",cargo," DELIV ",deliv),C_YEL);
  if(fr<180&&res<0)ltc(14,mis==0?"D-PAD: RCS  B: MATCH VELOCITY":"TOUCH A ROCK SLOWLY, HOLD A TO DRILL",C_WHITE);
  if(res==0)ltc(48,fxWhy,C_RED);if(res==1)ltc(48,mis==0?"RENDEZVOUS COMPLETE":"CARGO TARGET MET",C_GREEN);if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);}
 fr_vstop();return res==1?2000+(fuel>>8)*20+hull*400+(mis==2?deliv*10:cargo*10):0;}
// ---------------------------------------------------------------- mode menus
static const char*const MBN[5]={"1 TOUCHDOWN: SHACKLETON RIM","2 OUTPOST: HAB, SOLAR, ISRU","3 ICE HUNT: PERMANENT SHADOW","4 LUNAR NIGHT: CREW OF 4","5 LONG TRAVERSE"};
static const char*const MMN[5]={"1 SEVEN MINUTES OF TERROR","2 JEZERO SAMPLE DRIVE","3 STARSHIP: FLIP AND LAND","4 DUST STORM DRIVE","5 FULL MISSION: EDL + ROVER"};
static const char*const AMN[5]={"1 RENDEZVOUS: MATCH VELOCITY","2 PROSPECT AND MINE","3 HAUL TO THE DEPOT","4 DART: HIT DIMORPHOS","5 REDIRECT: SAVE EARTH (WHAT-IF)"};
static const char*const MBF[5][2]={
 {"SOUTH POLE","THE LUNAR SOUTH POLE HAS CRATER FLOORS THAT NEVER SEE SUNLIGHT - COLD TRAPS THAT HOLD WATER ICE. NASA'S ARTEMIS PROGRAM TARGETS THIS REGION. APOLLO 11 LANDED IN 1969 WITH THE LOW-FUEL CALLS ALREADY SOUNDING - LATER ANALYSIS FOUND A LITTLE MORE MARGIN THAN FEARED."},
 {"ISRU","IN-SITU RESOURCE UTILISATION MEANS LIVING OFF THE LAND: HEATING ICY REGOLITH TO GET WATER, THEN SPLITTING IT INTO OXYGEN AND HYDROGEN FOR AIR AND ROCKET FUEL."},
 {"COLD TRAPS","SOME PERMANENTLY SHADOWED CRATERS AT THE POLES ARE AMONG THE COLDEST MEASURED PLACES IN THE SOLAR SYSTEM, BELOW -230 C. LCROSS IMPACTED CABEUS CRATER IN 2009 AND DETECTED WATER IN THE PLUME."},
 {"LUNAR NIGHT","AWAY FROM THE POLES A LUNAR NIGHT LASTS ABOUT 14 EARTH DAYS. SOLAR POWER STOPS, SO NASA'S KILOPOWER PROJECT TESTED A SMALL FISSION REACTOR (KRUSTY, 2018) FOR STEADY POWER."},
 {"ROVERS","THE APOLLO LUNAR ROVING VEHICLE FLEW ON APOLLO 15, 16 AND 17. ON APOLLO 17 THE CREW DROVE ABOUT 36 KM IN TOTAL. CHINA'S YUTU-2 HAS EXPLORED THE FAR SIDE SINCE 2019."}};
static const char*const MMF[5][2]={
 {"EDL","PERSEVERANCE HIT THE ATMOSPHERE AT ABOUT 5.4 KM/S. A GUIDED ENTRY, A SUPERSONIC PARACHUTE, HEAT SHIELD JETTISON, RADAR, POWERED DESCENT AND A SKY CRANE BROUGHT IT DOWN IN ABOUT SEVEN MINUTES - FULLY AUTOMATED, BECAUSE RADIO NEEDS MINUTES TO REACH EARTH."},
 {"JEZERO","JEZERO CRATER HELD A LAKE BILLIONS OF YEARS AGO. PERSEVERANCE IS SEALING ROCK CORES IN TUBES FOR A POSSIBLE FUTURE SAMPLE RETURN."},
 {"STARSHIP","STARSHIP IS DESIGNED TO FALL BELLY-FIRST TO SHED SPEED, THEN FLIP VERTICAL AND RELIGHT RAPTOR ENGINES TO LAND. THE FLIP WAS FIRST SHOWN IN 2020-21 HOP TESTS. A MARS LANDING HAS NOT HAPPENED YET - THIS IS A SIMULATION."},
 {"DUST","GLOBAL DUST STORMS CAN BLOT OUT THE SUN FOR WEEKS. THE SOLAR-POWERED OPPORTUNITY ROVER WENT SILENT IN THE 2018 STORM AFTER ALMOST 15 YEARS ON MARS."},
 {"MARS","A ONE-WAY TRIP TO MARS TAKES ABOUT 7 MONTHS, AND LAUNCH WINDOWS OPEN ROUGHLY EVERY 26 MONTHS WHEN EARTH AND MARS LINE UP."}};
static const char*const AMF[5][2]={
 {"SAMPLES","JAPAN'S HAYABUSA2 RETURNED SAMPLES OF RYUGU IN 2020, AND NASA'S OSIRIS-REX DELIVERED BENNU SAMPLES IN 2023 AFTER A BRIEF TOUCH-AND-GO."},
 {"MINING","ASTEROID MINING IS STILL A CONCEPT. METAL-RICH ASTEROIDS LIKE 16 PSYCHE (VISITED BY NASA'S PSYCHE PROBE, LAUNCHED 2023) HINT AT WHAT IS OUT THERE."},
 {"PROPELLANT","WATER FROM ASTEROIDS COULD ONE DAY BE TURNED INTO PROPELLANT IN SPACE, SO IT WOULD NOT HAVE TO BE LIFTED OUT OF EARTH'S GRAVITY WELL."},
 {"DART","ON 26 SEP 2022 NASA'S DART PROBE HIT THE MOONLET DIMORPHOS AT ABOUT 6 KM/S. ITS ORBIT AROUND DIDYMOS SHORTENED BY ABOUT 33 MINUTES - THE FIRST TIME HUMANS MOVED A CELESTIAL BODY. DIMORPHOS POSED NO THREAT TO EARTH."},
 {"PLANETARY DEFENSE","THE EARLIER A THREAT IS FOUND, THE SMALLER THE NUDGE NEEDED. THIS MISSION IS FICTIONAL; ESA'S HERA PROBE (LAUNCHED 2024) IS FLYING TO STUDY DART'S RESULTS."}};
static void fr_mode_done(int k,int m,int sc,const char*const*names,int ok,const char*l2,const char*ft,const char*fb){
 int nb=fr_best(k,sc);if(ok&&VS.prog[k]<=m){VS.prog[k]=m+1;v131_save();}
 fr_result(ok?"MISSION COMPLETE":"MISSION FAILED",names[m],l2,ok,sc,nb);if(ok)fx_fact(ft,fb);bgm_t(TR_SPACE);}
static void moon_base(void){v131_load();static int sel=0;
 for(;;){u8 dn[5];for(int i=0;i<5;i++)dn[i]=VS.prog[0]>i;int m=fr_menu("MOON BASE","LAND, BUILD, EXPLORE THE SOUTH POLE",MBN,5,dn,&sel,SCR_MOON);if(m<0)return;
  if(m>VS.prog[0]&&!FR_AUTO){guide_tip("LOCKED","COMPLETE THE EARLIER MISSIONS FIRST.");continue;}
  int sc=0;if(m==0){fxH0=1400;fxX0=-420;fxVY0=-35;fxVX0=28;sc=fx_land(0);}else if(m==1||m==3){sc=mb_build(m);if(sc<0)continue;}else sc=fx_rover(0,m==2?4:7,m==2?3:5,m==2,0);
  fr_mode_done(0,m,sc,MBN,sc>0,sc>0?"":fxWhy,MBF[m][0],MBF[m][1]);if(FR_AUTO)return;}}
static void mars_mission(void){v131_load();static int sel=0;
 for(;;){u8 dn[5];for(int i=0;i<5;i++)dn[i]=VS.prog[1]>i;int m=fr_menu("MARS MISSION","ENTRY, DESCENT, LANDING AND ROVERS",MMN,5,dn,&sel,SCR_MARS);if(m<0)return;
  if(m>VS.prog[1]&&!FR_AUTO){guide_tip("LOCKED","COMPLETE THE EARLIER MISSIONS FIRST.");continue;}
  int sc=0;
  if(m==0||m==4){if(mars_edl(m==4?700000:640000,-15.5f)){int s1=fxSc;int s2=fx_land(1);sc=s2?s1+s2:0;if(sc&&m==4){int s3=fx_rover(1,4,3,0,0);sc=s3?sc+s3:0;}}}
  else if(m==2){fxH0=2200;fxX0=-300;fxVY0=-95;fxVX0=22;sc=fx_land(2);}
  else sc=fx_rover(1,m==1?5:5,m==1?3:3,0,m==3);
  fr_mode_done(1,m,sc,MMN,sc>0,sc>0?"":fxWhy,MMF[m][0],MMF[m][1]);if(FR_AUTO)return;}}
static int ast_redirect(void){ // what-if planetary defense: three impactors push the b-plane crossing off Earth
 int off=4,sc=0;for(int k=0;k<3;k++){char t[8]={'S','H','O','T',' ','1'+k,0};guide_card(t,"REDIRECT","EACH IMPACT NUDGES THE ASTEROID. THE EARLIER THE SHOT, THE BIGGER THE MISS DISTANCE AT EARTH.",-1,0,0);
  int miss=0,q=dart_aim(6-k,40,600-k*80,0,&miss);off+=q*(12-k*3)/100;sc+=q*20;
  fr_bgspace();for(int y=40;y<120;y++)for(int x=80;x<160;x++){int dx=x-120,dy=y-80;int d=dx*dx+dy*dy;if(d<=20*20)fb[y*240+x]=RGB(40,100,200);else if(d<=24*24&&((x+y)&1))fb[y*240+x]=RGB(120,160,220);}
  int ax=120+off,ay=80-off/3;m3_fill(ax-2,ay-2,5,5,off>24?C_GREEN:C_RED);m3_ctext(8,"B-PLANE: WHERE IT CROSSES EARTH'S ORBIT",C_WHITE,1);
  m3_ctext(128,fr_s2("MISS DISTANCE ",off*1000/20," KM-EQUIV  NEED >",24*1000/20),off>24?C_GREEN:C_YEL,1);m3_ctext(140,q?"HIT":"MISSED THE ASTEROID",q?C_GREEN:C_RED,1);m3_present();
  for(int t=0;t<120;t++)next_frame();if(off>24)break;}
 fxWhy="STILL ON A COLLISION COURSE";return off>24?3000+sc:0;}
static void asteroid_mining(void){v131_load();static int sel=0;
 for(;;){u8 dn[5];for(int i=0;i<5;i++)dn[i]=VS.prog[4]>i;int m=fr_menu("ASTEROID MINING","RENDEZVOUS, MINE, DEFEND THE PLANET",AMN,5,dn,&sel,SCR_AST);if(m<0)return;
  if(m>VS.prog[4]&&!FR_AUTO){guide_tip("LOCKED","COMPLETE THE EARLIER MISSIONS FIRST.");continue;}
  int sc=0;const char*l2="";static char pl[40];
  if(m<3){sc=ast_run(m);if(!sc)l2=fxWhy;}
  else if(m==3){int miss=0;int q=dart_aim(7,46,900,1,&miss);int dmin=q*33/100;if(q){sc=2000+q*40;int p0=11*60+55,p1=p0-dmin;const char*a=fr_s2("PERIOD 11H 55M -> ",p1/60,"H ",p1%60);int i=0;while(a[i]&&i<38){pl[i]=a[i];i++;}pl[i++]='M';pl[i]=0;l2=pl;}else{l2="MISSED DIMORPHOS";fxWhy=l2;}}
  else{sc=ast_redirect();if(!sc)l2=fxWhy;}
  fr_mode_done(4,m,sc,AMN,sc>0,l2,AMF[m][0],AMF[m][1]);if(FR_AUTO)return;}}
