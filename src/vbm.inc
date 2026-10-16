// ===================================================================== v1.3.2 VON BRAUN MISSIONS
// Falcon 9 droneship landing, Super Heavy chopstick catch, Artemis HLS lunar landing, rover sample drive, Starship orbital refuel.
// Side-view float sims at 30 fps (dt 1/30). FR_AUTO flies an autopilot; VBM_FAIL makes the autopilot deliberately fail (test hook).
#define SCR_VBM 98
#define SCR_VBMP 99
#define VBM_FAIL (*(volatile u32*)0x0203FFF0)
typedef struct{u8 done[5];u8 pad[3];u16 best[5];u16 plays;}VBMS;static VBMS VBS EWRAM;static int vbs_ld;
static void vbs_load(void){if(vbs_ld)return;vbs_ld=1;if(!sv_get(0x2380,'B',1,&VBS,sizeof VBS))memset(&VBS,0,sizeof VBS);}
static void vbs_save(void){sv_put(0x2380,'B',1,&VBS,sizeof VBS);}
static int vb_fail(void){return FR_AUTO&&VBM_FAIL==0x4C494146;}
static float vb_abs(float a){return a<0?-a:a;}
// ---------------------------------------------------------------- powered landing family
typedef struct{const char*name,*site;float g,tmax,tmin,fuel,h0,vy0,vx0,x0,vt,maxvy,maxvx,maxtilt,pad;int ign,hover,wind,catch_,len,wid;u16 body,band;}VbL;
static const VbL VBL[3]={
 // Falcon 9 first stage, 1 Merlin 1D: ~845 kN vs ~27 t -> ~31 m/s^2 max, min throttle ~40%: it cannot hover (hoverslam)
 {"FALCON 9 - DRONESHIP","DRONESHIP, ATLANTIC",9.81f,31.0f,12.4f,34.0f,3200,-245,14,-160,290,6.0f,2.0f,8,24,3,0,1,0,12,4,RGB(236,238,242),RGB(30,30,34)},
 // Super Heavy, 3 centre Raptors at landing: can hover; caught by the tower's arms
 {"SUPER HEAVY - TOWER CATCH","STARBASE, TEXAS",9.81f,28.0f,7.0f,42.0f,2600,-180,-6,120,300,1.6f,1.2f,4,4,2,1,2,1,18,7,RGB(196,200,208),RGB(90,94,102)},
 // Starship HLS on the Moon: high-mounted landing thrusters for the last ~100 m
 {"ARTEMIS HLS - LUNAR SOUTH POLE","SHACKLETON CONNECTING RIDGE",1.62f,5.2f,0.6f,95.0f,1800,-55,30,-380,0,2.0f,1.0f,5,30,9,1,0,0,18,7,RGB(240,240,244),RGB(40,40,46)}};
static void vb_land_bg(int k,int near){const VbL*L=&VBL[k];
 if(k==2){fr_bgspace();fx_disc_fb(204,30,10,RGB(40,90,200),RGB(200,220,255));}
 else for(int y=0;y<160;y++)m3_fill(0,y,240,1,k==0?RGB(70+y/2,120+y/3,200):RGB(110+y/2,150+y/3,210));
 int gy=146;
 if(k==0){m3_fill(0,gy-2,240,16,RGB(20,50,110));for(int i=0;i<30;i++)m3_fill((i*37)%236,gy+(i*5)%12,4,1,RGB(70,110,170));}
 else if(k==1){m3_fill(0,gy,240,14,RGB(196,180,140));m3_fill(0,gy+8,240,6,RGB(40,80,140));}
 else{m3_fill(0,gy,240,14,RGB(120,120,126));for(int i=0;i<30;i++)m3_fill((i*61)%236,gy+2+(i*7)%11,3+(i&3),1,RGB(80,80,86));}
 int s=near?3:1;(void)L;
 if(k==0){int w=near?52*3/4:6;m3_fill(120-w,gy-3,w*2,3,RGB(60,60,66));m3_fill(120-w,gy-4,w*2,1,RGB(240,200,40));if(near){for(int x=-w;x<w;x+=12)m3_fill(120+x,gy-3,6,1,RGB(240,240,240));}}
 if(k==1){int tx=near?150:126,th=near?110:20;m3_fill(tx,gy-th,near?10:3,th,RGB(90,92,100));if(near){for(int y=gy-th;y<gy;y+=8)m3_fill(tx,y,10,1,RGB(150,150,160));m3_fill(tx-10,gy-th+6,4,6,RGB(120,124,134));}m3_fill(tx-(near?40:6),gy-4,near?36:5,4,RGB(150,150,150));}
 if(k==2){int pw=near?30*3/4*2:8;m3_fill(120-pw/2,gy-1,pw,2,RGB(190,190,150));if(!near)for(int i=0;i<8;i++){int x=(i*53+11)%240;if(iabs(x-120)<16)continue;m3_fill(x,gy-4,8,4,RGB(80,80,86));}
  else for(int i=0;i<5;i++){int x=(i*47+20)%240;if(iabs(x-120)<40)continue;m3_fill(x,gy-6,10,6,RGB(80,80,86));}}
 (void)s;ltx(4,150,VBL[k].site,RGB(230,230,230));fr_save_bg();}
static int vb_land(int k){const VbL*L=&VBL[k];float g=L->g,h=L->h0,x=L->x0,vy=L->vy0,vx=L->vx0,fuel=L->fuel,f0=fuel,dt=1.0f/30;
 int ign=L->ign,on=0,thr=0,res=-1,endT=0,near=0,legs=0,closed=0,fr=0,tilt=0;float wind=0,gust=0;int au=FR_AUTO,bad=vb_fail();const char*why="";int ldT=0;int sc=0;float deckx=0;
 vb_land_bg(k,0);fr_vstart();bgm_t(TR_ASCENT);
 for(;;fr++){BEAT->screen=SCR_VBMP;BEAT->mode=k;BEAT->phase=res<0?1:res?3:4;BEAT->scoreA=fx_i(h);BEAT->scoreB=fx_i(vy*10);BEAT->x=fx_i(x);BEAT->y=fx_i(fuel);BEAT->hp=ign;BEAT->extra=tilt;
  if(res<0){float sp=fx_sqrt(vx*vx+vy*vy);
   // wind + gusts (Earth only)
   if(L->wind){gust+=((rr(201)-100)/100.0f)*0.06f*L->wind;gust*=0.985f;wind=(SIN(fr&255)/256.0f)*0.6f*L->wind+gust;}
   int want=0,wt=thr;float fin=0;
   if(au){ // autopilot: suicide-burn timing + lateral nulling
    float amax=L->tmax*0.82f-g;float hc=h-(k==1?0:0);float stop=vy<0?vy*vy/(2*(amax>0.5f?amax:0.5f)):0;
    int light=!bad&&(L->hover?(hc<stop*1.15f+120):(hc<stop*1.05f+40));
    if(on||light){want=1;float vyt=-(1.2f+hc*0.06f);if(L->hover&&hc<40)vyt=-(0.6f+hc*0.04f);float areq=vy<-1.5f&&hc>0.5f?(vy*vy-2.25f)/(2*hc):0;
     float ay=L->hover?g+(vyt-vy)*1.2f:(areq+g)*1.02f;if(ay<0.05f)ay=0.05f;
     float ax=(-(x-deckx)*0.10f-vx)*0.7f;int tm=h>200?25:10;int at=fx_i(ax/(ay>0.3f?ay:0.3f)*57.3f);at=clampi(at,-tm,tm);if(tilt<at)tilt++;else if(tilt>at)tilt--;float cs=COS((tilt*256/360)&255)/256.0f;wt=clampi(fx_i(ay/(cs>0.5f?cs:0.5f)/L->tmax*100),0,100);}
    else{fin=-((x-deckx)*0.004f+vx*0.02f);if(fin>1)fin=1;if(fin<-1)fin=-1;if(bad)fin=0;}
    if(h<300)legs=1;if(!L->hover&&on&&h<8&&vy>-0.6f){want=0;ign=0;}
    if(L->catch_&&!closed&&on&&h<1.2f&&vb_abs(x)<3&&vb_abs(vy)<1.6f&&!bad){closed=1;}}
   else{int ki=keys;want=(ki&K_A)!=0;if(ki&K_UP)thr+=3;if(ki&K_DOWN)thr-=3;wt=thr;if(want&&!on&&ign>0)wt=thr<60?80:thr;
    if(ki&K_LEFT){if(on)tilt--;else fin=-1;}if(ki&K_RIGHT){if(on)tilt++;else fin=1;}tilt=clampi(tilt,-15,15);
    if(pressed&K_B){if(L->catch_){if(h<4)closed=1;}else legs=1;}if(h<180)legs=1;}
   // engine state: limited relights (TEA-TEB style ignitions)
   if(want&&!on&&ign>0&&fuel>0){on=1;ign--;sfx_whoosh();lmsg(k==2?"RAPTORS LIT":"LANDING BURN",60);}
   if(!want&&on){on=0;}
   if(fuel<=0){on=0;fuel=0;}
   thr=on?clampi(wt,fx_i(L->tmin/L->tmax*100),100):0;if(!on&&!au)thr=clampi(thr,0,100);
   float a=on?L->tmax*thr/100.0f:0;if(k==2&&h<100&&on){a*=0.85f;} // HLS high-mounted thrusters below 100 m
   float drag=L->vt>0?g*(sp/L->vt)*(sp/L->vt):0;float dx=sp>0.1f?-vx/sp:0,dy=sp>0.1f?-vy/sp:0;
   float finA=L->vt>0&&!on?fin*7.0f*(sp/L->vt)*(sp/L->vt):0; // grid fins: lateral authority grows with dynamic pressure
   vx+=(a*SIN(tilt*256/360&255)/256.0f+drag*dx+finA+wind*0.3f)*dt;vy+=(a*COS(tilt*256/360&255)/256.0f-g+drag*dy)*dt;
   if(!on&&vb_abs(tilt)>0&&(fr&3)==0)tilt+=tilt>0?-1:1;
   fuel-=on?thr/100.0f*dt:0;x+=vx*dt;h+=vy*dt;
   if(k==0)deckx=SIN((fr/3)&255)*1.5f/256.0f; // the droneship holds station, swaying a little
   // outcomes
   if(L->catch_){
    if(h<60&&res<0&&vb_abs(x)>15){res=0;why="WAVE-OFF: TOWER PROTECTED, DIVERTED";}
    if(closed&&res<0){if(vb_abs(x)<3&&vb_abs(vy)<L->maxvy&&iabs(tilt)<=L->maxtilt){res=1;sfx_ok();lmsg("CAUGHT! CHOPSTICKS CLOSED",150);}else{res=0;why="ARMS CLOSED ON A MOVING BOOSTER";}}
    if(h<-6&&res<0){res=0;why=vy<-4?"HARD IMPACT AT THE PAD":"MISSED THE CATCH";}}
   else if(h<=0&&res<0){h=0;float rx=x-deckx;int onpad=vb_abs(rx)<=L->pad;int soft=vb_abs(vy)<L->maxvy&&vb_abs(vx)<L->maxvx&&iabs(tilt)<=L->maxtilt;
    if(k==0&&!onpad){res=0;why="MISSED THE DECK - BOOSTER LOST AT SEA";}
    else if(!legs){res=0;why="LEGS NOT DEPLOYED";}
    else if(!soft){res=0;why=vb_abs(vy)>=L->maxvy?"HARD LANDING":iabs(tilt)>L->maxtilt?"TIPPED OVER":"SLID OFF - TOO MUCH SIDE SPEED";}
    else{res=1;sfx_ok();lmsg(k==0?"THE FALCON HAS LANDED":"TOUCHDOWN - HLS SAFE ON THE MOON",150);if(!onpad)why="OFF TARGET ZONE";}}
   if(res==1)sc=3000+fx_i(fuel/f0*2500)+(int)(1500-fx_i(vb_abs(x-deckx)*40))-fx_i(vb_abs(vy)*150)+ign*200;
   if(res>=0){endT=fr;if(!res){shake(14);sfx_boom();fsp_add(120,140,12,RGB(255,180,60));}}}
  else if(fr-endT>150)break;
  if(!near&&h<220&&res<0){near=1;fr_vstop();vb_land_bg(k,1);fr_vstart();}
  next_frame();fr_vrestore();
  int px,py;if(near){px=(k==1?129:120)+fx_i((x)*(k==0?0.75f:1.2f));py=146-(k==0?3:0)-fx_i(h*0.6f);}else{px=120+fx_i(x/8);py=146-fx_i(h/24);}
  if(L->catch_&&near)py=146-110+14+L->len*2-4-fx_i(h*0.6f); // catch point at the arms (~ upper tower)
  if(py<8)py=8;px=clampi(px,6,234);
  int an=(tilt*256/360)&255;int ln=near?L->len:L->len/3+2,wd=near?L->wid:3;
  fx_body(px,py-ln,an,ln,wd,L->body);fx_body(px+(SIN(an)*(ln*3/2)>>8),py-ln-(COS(an)*(ln*3/2)>>8)+0,an,1,wd,L->band);
  if(near&&k==0){lf(px-5,py-ln*2+1,2,2,RGB(60,60,60));lf(px+3,py-ln*2+1,2,2,RGB(60,60,60));}
  if(legs&&near&&!L->catch_){lf(px-wd-3,py-3,2,3,RGB(60,60,60));lf(px+wd+1,py-3,2,3,RGB(60,60,60));lf(px-wd-4,py-1,wd*2+8,1,RGB(40,40,40));}
  if(L->catch_&&near){int ay=146-110+14;u16 ac=closed?RGB(250,200,60):RGB(170,170,180);lf(150-36,ay-2,closed?36-wd:30,3,ac);lf(150-36,ay+3,closed?36-wd:30,2,ac);}
  if(on&&res<0){int Lf=3+thr/12;for(int q=0;q<Lf;q+=2)lf(px-(SIN(an)*q>>8)-1,py+(COS(an)*q>>8),3,2,(q+fr)&2?RGB(255,220,120):RGB(255,140,40));}
  if(near&&on&&h<40&&res<0)for(int q=0;q<4;q++)lf(px-24+rr(48),143-rr(5),2,1,k==2?RGB(200,200,200):RGB(240,240,240));
  fsp_run();
  ltx(4,3,cat("ALT ",itoa(fx_i(h))),C_WHITE);ltx(4,12,cat("V/S ",itoa(fx_i(vy*10)/10)),vb_abs(vy)>=L->maxvy&&h<60?C_RED:C_WHITE);ltx(4,21,cat("H/S ",itoa(fx_i(vx))),C_WHITE);
  ltx(150,3,k==2?"PROP":"FUEL",C_GRAY);fr_bar(180,4,50,fx_i(fuel),fx_i(f0),fuel<f0/5?C_RED:C_GREEN);ltx(150,12,"THR",C_GRAY);fr_bar(180,13,50,thr,100,C_YEL);
  ltx(150,21,cat("IGN ",itoa(ign)),ign?C_WHITE:C_RED);if(L->wind)ltx(196,21,wind+gust*0.3f>0.2f?"W>":wind<-0.2f?"<W":"W-",RGB(150,210,255));
  ltx(4,30,cat("TGT ",itoa(fx_i(x-deckx))),vb_abs(x-deckx)<=L->pad?C_GREEN:C_YEL);
  if(L->catch_&&!closed&&h<6&&res<0)ltc(40,"B: CLOSE CHOPSTICKS",C_YEL);
  if(res==0)ltc(48,why,C_RED);if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}
  if(au)ltx(196,150,bad?"FAIL":"AUTO",C_BLUE);}
 fr_vstop();fxWhy=why;return res==1?(sc>100?sc:100):0;}
// ---------------------------------------------------------------- rover sample drive (Mars 3.71 m/s^2, side view, rocker-bogie)
static int rd_h(int wx){return 120+(SIN((wx*2)&255)*6>>8)+(SIN((wx/3+40)&255)*16>>8)+(SIN((wx/9)&255)*12>>8)-((wx%173)<24?(2*SIN(((wx%173)*32/3)&255)>>8):0);}
static int vb_rover(void){int au=FR_AUTO,bad=vb_fail();float x=40,vx=0,y=0,vyv=0,bat=100,dmg=0,dt=1.0f/30;int air=0,res=-1,endT=0,fr=0,got=0,drill=0;const char*why="";
 static const int SITE[5]={260,520,780,1040,1300};u8 done[5]={0};int depot=1500;int sol=30*240;int cam=0,lcam=-1,lgot=-1;
 bgm_t(TR_RACE2);
 for(;;fr++){BEAT->screen=SCR_VBMP;BEAT->mode=3;BEAT->phase=res<0?1:res?3:4;BEAT->x=fx_i(x);BEAT->y=fx_i(bat);BEAT->scoreA=got;BEAT->scoreB=fx_i(dmg);BEAT->hp=sol;
  if(res<0){int ix=fx_i(x);float slope=(rd_h(ix+8)-rd_h(ix-8))/16.0f;static int tipT;if(fr==0)tipT=0; // screen-units per unit, ~ tan(pitch)
   int pitch=fx_i(slope*57.3f);int near=-1;for(int i=0;i<5;i++)if(!done[i]&&iabs(ix-SITE[i])<6)near=i;
   int thr=0,brake=0,dr=0;
   if(au){int nx=depot;for(int i=0;i<5;i++)if(!done[i]){nx=SITE[i];break;}float tv=bad?9.0f:(iabs(pitch)>16?2.2f:3.6f);if(!bad&&nx-ix<20)tv=1.0f+(nx-ix)*0.12f;if(!bad&&near>=0){tv=0;dr=1;}
    thr=vx<tv;brake=vx>tv+0.4f;}
   else{thr=(keys&K_A)!=0||(keys&K_RIGHT);brake=(keys&K_B)!=0;dr=(keys&K_DOWN)!=0;}
   if(!air){float acc=thr&&bat>0?1.6f:0;acc-=3.71f*slope*0.30f; // gravity along the slope (Mars)
    if(brake)vx*=0.85f;vx+=acc*dt;vx*=0.995f;if(vx<-1.5f)vx=-1.5f;bat-=thr?0.012f:0;
    float gy=-rd_h(fx_i(x+vx*dt));float cy=-rd_h(ix);if(vx>4.2f&&gy-cy<-0.25f){air=1;vyv=vx*0.18f;y=cy;}} // crest at speed: airborne
   if(air){vyv-=3.71f*dt*0.9f;y+=vyv*dt*3;float gy=-rd_h(fx_i(x));if(y<=gy){air=0;float imp=-vyv;if(imp>1.2f){dmg+=imp*14;shake(8);sfx_boom();lmsg("HARD LANDING - WHEEL DAMAGE",80);}}}
   x+=vx*dt*4;bat+=0.004f; // RTG trickle charge
   if(dr&&near>=0&&vb_abs(vx)<0.3f){drill++;bat-=0.03f;if(drill>=90){done[near]=1;got++;drill=0;sfx_ok();lmsg(cat("SAMPLE TUBE SEALED ",itoa(got)),90);}}else drill=0;
   if(--sol<=0&&res<0){res=0;why="SOL ENDED - SAMPLES NOT CACHED";}
   if(bat<=0&&res<0){res=0;why="BATTERY FLAT";}
   if(iabs(pitch)>30)tipT++;else tipT=0;if(tipT>20&&res<0){res=0;why="ROVER TIPPED OVER (>30 DEG)";}
   if(dmg>=100&&res<0){res=0;why="WHEELS DESTROYED";}
   if(ix>=depot&&res<0){if(got>=4){res=1;sfx_ok();lmsg("SAMPLE DEPOT CACHED!",150);}else{res=0;why="ARRIVED WITH TOO FEW SAMPLES";}}
   if(res>=0)endT=fr;
   cam=(ix/200)*200-20;if(cam<0)cam=0;}
  else if(fr-endT>120)break;
  next_frame();fr_vrestore();
  if(cam!=lcam||got!=lgot){lcam=cam;lgot=got;fr_vstop();for(int yy=0;yy<160;yy++)m3_fill(0,yy,240,1,RGB(180+yy/3>255?255:180+yy/3,120+yy/4,90+yy/6));
   for(int sx=0;sx<240;sx++){int h=rd_h(cam+sx);m3_fill(sx,h,1,160-h,RGB(170,88,50));m3_fill(sx,h,1,2,RGB(200,120,70));}
   for(int i=0;i<5;i++){int sx=SITE[i]-cam;if(sx>-4&&sx<236){int h=rd_h(SITE[i]);m3_fill(sx,h-10,1,10,RGB(240,240,240));m3_fill(sx+1,h-10,5,3,done[i]?RGB(80,220,90):RGB(250,200,40));m3_fill(sx-3,h-2,7,3,RGB(110,60,40));}}
   {int sx=depot-cam;if(sx>=0&&sx<228){int h=rd_h(depot);m3_fill(sx,h-14,12,14,RGB(200,200,210));m3_text(sx-6,h-24,"DEPOT",C_WHITE,1);}}
   fr_save_bg();fr_vstart();}
  {int px=fx_i(x)-cam;int gy=rd_h(fx_i(x));int py=air?-fx_i(y):gy;int t1=rd_h(fx_i(x)-6),t2=rd_h(fx_i(x)+6);if(air){t1=t2=py;}
   for(int q=-1;q<=1;q++){int wx=px+q*6;int wy=(q<0?t1:q>0?t2:py);lf(wx-2,wy-4,4,4,RGB(40,40,40));}
   lf(px-8,py-11+(t1-t2)/4,16,5,RGB(230,230,236));lf(px+4,py-18,2,7,RGB(200,200,210));lf(px+3,py-20,4,2,RGB(60,60,70));lf(px-9,py-13,3,3,RGB(90,90,90));
   if(drill)lf(px+8,py-6,1,6,RGB(250,220,120));}
  ltx(4,3,cat3("SAMPLES ",itoa(got),"/4"),got>=4?C_GREEN:C_WHITE);ltx(4,12,cat("DIST TO DEPOT ",itoa(hmx(0,depot-fx_i(x)))),C_WHITE);
  ltx(150,3,"BATT",C_GRAY);fr_bar(180,4,50,fx_i(bat),100,bat<20?C_RED:C_GREEN);ltx(150,12,"DMG",C_GRAY);fr_bar(180,13,50,fx_i(dmg),100,C_RED);
  ltx(150,21,cat("SOL T-",itoa(sol/30)),sol<600?C_RED:C_WHITE);
  if(drill)ltc(40,"DRILLING CORE...",C_YEL);if(res==0)ltc(48,why,C_RED);if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}if(au)ltx(196,150,bad?"FAIL":"AUTO",C_BLUE);}
 fr_vstop();fxWhy=why;return res==1?2000+got*500+fx_i(bat)*20+sol/3-fx_i(dmg)*10:0;}
// ---------------------------------------------------------------- Starship orbital refuel: Hill-frame RCS approach + settled propellant transfer
static int vb_refuel(void){int au=FR_AUTO,bad=vb_fail();float x=-80,z=14,vx=0.9f,vz=0,rcs=100,dt=1.0f/30,n=0.0011f*30;int res=-1,endT=0,fr=0,phase=0;float xfer=0,settle=50,stress=0;const char*why="";
 fr_bgspace();fr_earth_bg(70,0);fr_save_bg();fr_vstart();bgm_t(TR_SPACE);
 for(;;fr++){BEAT->screen=SCR_VBMP;BEAT->mode=4;BEAT->phase=res<0?1+phase:res?3:4;BEAT->x=fx_i(x*10);BEAT->y=fx_i(z*10);BEAT->scoreA=fx_i(xfer);BEAT->scoreB=fx_i(rcs);
  if(res<0){
   if(phase==0){float ax=0,az=0;
    if(au){float tvx=bad?2.0f:(x<-3?clampi(fx_i(-x*0.06f*100),12,90)/100.0f:0.12f);float tvz=-z*0.05f;ax=(tvx-vx)*0.5f;az=(tvz-vz)*0.5f;}
    else{if(keys&K_RIGHT)ax=0.25f;if(keys&K_LEFT)ax=-0.25f;if(keys&K_UP)az=0.25f;if(keys&K_DOWN)az=-0.25f;}
    ax=ax>0.25f?0.25f:ax<-0.25f?-0.25f:ax;az=az>0.25f?0.25f:az<-0.25f?-0.25f:az;
    // Clohessy-Wiltshire (x along-track, z radial), time-compressed: drift pulls you off the V-bar if you dawdle
    float cax=-2*n*vz,caz=3*n*n*z+2*n*vx;vx+=(ax+cax*0.2f)*dt;vz+=(az+caz*0.2f)*dt;x+=vx*dt;z+=vz*dt;rcs-=(vb_abs(ax)+vb_abs(az))*dt*2.2f;
    if(rcs<=0&&res<0){res=0;why="RCS PROPELLANT EXHAUSTED";}
    if(x>=-0.5f&&res<0){float rv=fx_sqrt(vx*vx+vz*vz);if(vb_abs(z)<0.8f&&rv<0.25f){phase=1;sfx_ok();lmsg("SOFT CAPTURE - LATCHES CLOSED",120);x=0;vx=vz=0;}else{res=0;why=vb_abs(z)>=0.8f?"MISALIGNED - STRUCK THE DEPOT":"CLOSING TOO FAST - COLLISION";}}
    if(x<-260||vb_abs(z)>90){res=0;why="DRIFTED OUT OF THE APPROACH CORRIDOR";}}
   else{ // transfer: hold a gentle ullage thrust so propellant stays settled; too hard strains the docking ring
    int u=au?(bad?0:(settle<55)):((keys&K_A)!=0);settle+=u?1.1f:-0.9f;settle=clampi(fx_i(settle),0,100);if(u)rcs-=0.02f;
    if(settle>85)stress+=0.6f;else if(stress>0)stress-=0.1f;
    if(settle>=35&&settle<=85)xfer+=0.12f;else if(settle<35)lmsg("PROPELLANT UNSETTLED - VAPOUR AT THE INLET",20);
    if(stress>=100&&res<0){res=0;why="DOCKING RING OVERSTRESSED";}if(xfer>=100&&res<0){res=1;sfx_ok();lmsg("DEPOT FULL - READY FOR TRANS-LUNAR INJECTION",150);}
    if(fr>30*240&&res<0){res=0;why="BOIL-OFF: TRANSFER TOO SLOW";}}
   if(res>=0){endT=fr;if(!res){shake(10);sfx_boom();}}}
  else if(fr-endT>150)break;
  next_frame();fr_vrestore();
  int dx=120,dy=80;int sc=x<-40?1:4;int tx=dx-30+fx_i(x*(x<-40?0.35f:sc)),ty=dy+fx_i(-z*(x<-40?0.8f:sc));
  lf(dx,dy-5,52,10,RGB(220,222,230));lf(dx+52,dy-4,6,8,RGB(60,60,70));lf(dx-2,dy-2,2,4,RGB(250,200,60));ltx(dx+6,dy+8,"DEPOT",C_GRAY);
  lf(tx-52,ty-5,50,10,RGB(200,204,214));lf(tx-58,ty-4,6,8,RGB(60,60,70));lf(tx-2,ty-2,2,4,RGB(250,200,60));if(fr&8)lf(tx-30,ty-6,1,1,RGB(255,60,60));
  if(phase==1){lf(30,128,180,6,RGB(30,30,44));lf(30,128,fx_i(xfer*1.8f),6,C_BLUE);ltx(30,137,"LOX + CH4 TRANSFER",C_WHITE);lf(30,148,100,4,RGB(30,30,44));lf(30+35,148,50,4,RGB(30,70,30));lf(30+settle,147,2,6,C_YEL);ltx(136,146,"SETTLE",C_GRAY);fr_bar(180,147,50,fx_i(stress),100,C_RED);}
  ltx(4,3,cat("RANGE ",itoa(fx_i(-x))),C_WHITE);ltx(4,12,cat("CLOSING ",itoa(fx_i(vx*100))),vx>0.25f&&x>-5?C_RED:C_WHITE);ltx(4,21,cat("OFFSET ",itoa(fx_i(z*10))),vb_abs(z)<0.8f?C_GREEN:C_YEL);
  ltx(150,3,"RCS",C_GRAY);fr_bar(180,4,50,fx_i(rcs),100,rcs<20?C_RED:C_GREEN);
  if(phase==1)ltc(40,"HOLD A: GENTLE ULLAGE THRUST",C_YEL);if(res==0)ltc(48,why,C_RED);if(lMsgT>0){lMsgT--;ltc(58,lMsg,C_YEL);}if(au)ltx(196,150,bad?"FAIL":"AUTO",C_BLUE);}
 fr_vstop();fxWhy=why;return res==1?3000+fx_i(rcs)*30+(30*240-fr)/4:0;}
// ---------------------------------------------------------------- menu
static const char*const VBN[5]={"1 FALCON 9 DRONESHIP LANDING","2 SUPER HEAVY TOWER CATCH","3 ARTEMIS HLS MOON LANDING","4 ROVER DRIVE: SAMPLE CACHE","5 STARSHIP ORBITAL REFUEL"};
static const char*const VBB[5][2]={
 {"BOOSTER LANDING","A FALCON 9 FIRST STAGE CANNOT HOVER: ONE MERLIN AT MINIMUM THROTTLE STILL OUT-PUSHES THE NEARLY EMPTY STAGE. SO IT FLIES A 'HOVERSLAM' - LIGHT AT THE LAST MOMENT AND REACH ZERO SPEED AT ZERO HEIGHT. GRID FINS STEER WHILE FALLING. HOLD A TO BURN, L/R GIMBAL, UP/DOWN THROTTLE."},
 {"CATCH THE BOOSTER","SUPER HEAVY IS NOT GIVEN LEGS. THE LAUNCH TOWER'S TWO 'CHOPSTICK' ARMS CATCH IT IN MID-AIR, AS FIRST DONE IN OCTOBER 2024. HOVER WITHIN A FEW METRES OF THE ARMS, THEN PRESS B. IF YOU DRIFT WIDE, THE FLIGHT IS WAVED OFF TO PROTECT THE TOWER."},
 {"ARTEMIS HLS","NASA CHOSE A LUNAR STARSHIP TO LAND ARTEMIS CREWS NEAR THE SOUTH POLE. IN THE LAST ~100 M IT USES HIGH-MOUNTED THRUSTERS SO THE MAIN ENGINES DON'T DIG A CRATER. IN 1/6 G IT CAN HOVER - BUT IT IS TALL, SO KEEP IT UPRIGHT."},
 {"SAMPLE CACHE","PERSEVERANCE SEALS ROCK CORES IN TUBES AND LEFT A DEPOT OF THEM ON THE JEZERO CRATER FLOOR IN 2022-23. DRIVE (A), BRAKE (B), STOP ON A FLAG AND HOLD DOWN TO CORE. MARS GRAVITY IS 0.38 G: CRESTS AT SPEED CAN LAUNCH YOU. IN THIS GAME, OVER 30 DEGREES OF TILT AND YOU ROLL."},
 {"ORBITAL REFUEL","A MOON OR MARS STARSHIP NEEDS SEVERAL TANKER FLIGHTS TO REFILL IT IN EARTH ORBIT. APPROACH ALONG THE V-BAR WITH RCS (D-PAD), CLOSE SLOWER THAN 0.25 M/S, THEN HOLD A FOR A GENTLE ULLAGE PUSH SO THE PROPELLANT STAYS SETTLED WHILE IT FLOWS."}};
static const char*const VBF[5][2]={{"REFLIGHT","THE MOST-FLOWN FALCON 9 BOOSTERS HAVE LAUNCHED AND LANDED MORE THAN 20 TIMES."},{"FLIGHT 5","ON 13 OCTOBER 2024 MECHAZILLA CAUGHT A SUPER HEAVY BOOSTER FOR THE FIRST TIME."},{"1969 AND BEYOND","APOLLO 11'S EAGLE TOUCHED DOWN WITH UNDER A MINUTE OF DESCENT FUEL LEFT."},{"SAMPLE RETURN","THE TUBES WAIT FOR A FUTURE MISSION TO BRING MARS ROCK TO EARTH."},{"CRYOGENIC","LIQUID OXYGEN AND METHANE BOIL OFF IN SUNLIGHT, SO TANKER FLIGHTS MUST COME QUICKLY."}};
static int vbm_run(int m){if(m<3)return vb_land(m);if(m==3)return vb_rover();return vb_refuel();}
static void vbm_mode(void){vbs_load();static int sel=0;bgm_t(TR_SPACE);
 for(;;){int m=fr_menu("VON BRAUN MISSIONS","LANDINGS, ROVERS AND ORBITAL REFUELING",VBN,5,VBS.done,&sel,SCR_VBM);if(m<0)return;
  if(!FR_AUTO&&guide_card(VBB[m][0],"MISSION BRIEFING",VBB[m][1],-1,0,0)<0)continue;
  int sc=vbm_run(m);int nb=0;VBS.plays++;if(sc>0){if(!VBS.done[m])VBS.done[m]=1;if(sc>VBS.best[m]){VBS.best[m]=sc>65535?65535:sc;nb=1;}}vbs_save();
  fr_result(sc>0?"MISSION COMPLETE":"MISSION FAILED",VBN[m]+2,sc>0?"":fxWhy,sc>0,sc,nb);if(sc>0)fx_fact(VBF[m][0],VBF[m][1]);bgm_t(TR_SPACE);if(FR_AUTO)return;}}
static void rec_vbm(const char**L){vbs_load();static char b[5][40] EWRAM;for(int i=0;i<5;i++){char*o=sp_put(b[i],VBN[i]+2);int n=slen(b[i]);if(n>24){b[i][24]=0;o=b[i]+24;}o=sp_put(o," ");sp_put(o,VBS.best[i]?itoa(VBS.best[i]):"--");L[i]=b[i];}}
