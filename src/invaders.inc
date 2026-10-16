// ===================================================================== v1.3.2 ROCKET INVADERS - original fixed shooter (Mode 3, dirty rects)
#define SCR_INV 102
#define SCR_INVS 103
typedef struct{HiE t[5];u32 games;u16 bestWave,pad;}IvSave;static IvSave IVS EWRAM;static int ivs_ld;
static void ivs_load(void){if(ivs_ld)return;ivs_ld=1;if(!sv_get(0x2480,'I',1,&IVS,sizeof IVS)){memset(&IVS,0,sizeof IVS);static const char*const N[5]={"KC ","LT ","WVB","DAD","GRK"};static const u32 S[5]={12000,8000,5000,3000,1500};hi_seed(IVS.t,N,S);}}
static void ivs_save(void){sv_put(0x2480,'I',1,&IVS,sizeof IVS);}
// 1-bit art, 11x8 (two frames each); '#' = body colour, '+' = accent
static const char*const IVA[3][2][8]={
 {{"   #   #   ","    # #    ","   #####   ","  ## # ##  "," ######### "," # ##### # "," #       # ","  ##   ##  "},{"   #   #   ","  # # # #  ","  #######  "," ##+###+## "," ######### ","   #   #   ","  #     #  ","   #   #   "}},
 {{"    ###    ","  #######  "," ##+#+#+## ","###########","  ##   ##  "," ##  #  ## ","##       ##","           "},{"    ###    ","  #######  "," ##+#+#+## ","###########","   ## ##   ","  ##   ##  ","   ## ##   ","           "}},
 {{"  ##   ##  "," ######### ","## ##### ##","###+###+###"," ######### ","  # # # #  "," #       # ","           "},{"  ##   ##  "," ######### ","## ##### ##","###+###+###"," ######### "," #  # #  # ","  #     #  ","           "}}};
static const u16 IVC[3][2]={{RGB(120,255,140),RGB(255,255,255)},{RGB(255,120,220),RGB(255,240,90)},{RGB(110,200,255),RGB(255,90,70)}};
static void iv_blit(int x,int y,const char*const*a,int w,int h,u16 c1,u16 c2){lf(x,y,0,0,0);if(lfN>=0&&lfN<384){LRect*r=&lfR[lfN++];r->x=clampi(x,0,239);r->y=clampi(y,0,159);r->w=w;r->h=h;}
 for(int j=0;j<h;j++){int yy=y+j;if(yy<0||yy>=160)continue;const char*s=a[j];for(int i=0;i<w;i++){int xx=x+i;if(xx<0||xx>=240)continue;char ch=s[i];if(ch=='#')fb[yy*240+xx]=c1;else if(ch=='+')fb[yy*240+xx]=c2;}}}
typedef struct{const char*n,*d;s16 spd,rate,hp,spread,w;u16 c;}IvShip;
static const IvShip IVSH[4]={{"FALCON 9","BALANCED. ONE FAST SHOT.",3,14,1,0,5,RGB(236,238,242)},{"FALCON HEAVY","TRIPLE SHOT: 3 CORES FIRE.",2,22,1,1,9,RGB(236,238,242)},
 {"STARSHIP","BIG STEEL SHIP, 4 HITS PER LIFE.",3,15,4,0,9,RGB(196,200,210)},{"DRAGON","FAST AND NIMBLE CAPSULE.",4,12,1,0,5,RGB(240,240,250)}};
#define IV_COLS 9
#define IV_ROWS 5
typedef struct{s16 x,y,vx,vy;u8 on,k;}IvB;
static IvB ivPB[8] EWRAM,ivEB[12] EWRAM,ivPU[3] EWRAM,ivDeb[4] EWRAM;static u8 ivA[IV_ROWS][IV_COLS] EWRAM;static u8 ivSh[4][16][22] EWRAM;
static void iv_shields(int wave){u16 bg=RGB(4,6,20);for(int s=0;s<4;s++){int x0=24+s*56,y0=118;for(int y=0;y<16;y++)for(int x=0;x<22;x++){int on;
  if(wave&1){on=y<14&&!(y>8&&x>7&&x<14)&&(x>2&&x<19||y>3);if(y<4&&(x<6||x>15))on=0;} // launch tower w/ flame trench
  else{on=(y>5&&y<10)||(y>=3&&y<13&&((x/5)&1));} // Starlink barrier: a chain of flat-panel sats
  ivSh[s][y][x]=on;u16 c=on?((wave&1)?((y&3)==0?RGB(160,160,170):RGB(110,112,124)):((x/5)&1?RGB(60,90,170):RGB(200,210,230))):bg;fb[(y0+y)*240+x0+x]=c;}}}
static int iv_shield_hit(int x,int y,int r){for(int s=0;s<4;s++){int x0=24+s*56,y0=118;int lx=x-x0,ly=y-y0;if(lx<0||lx>=22||ly<0||ly>=16)continue;if(!ivSh[s][ly][lx])continue;
  u16 bg=RGB(4,6,20);for(int dy=-r;dy<=r;dy++)for(int dx=-r;dx<=r;dx++){int qx=lx+dx,qy=ly+dy;if(qx<0||qx>=22||qy<0||qy>=16)continue;if(dx*dx+dy*dy>r*r+1||rr(3)==0)continue;ivSh[s][qy][qx]=0;bigbuf[(y0+qy)*240+x0+qx]=bg;lf(x0+qx,y0+qy,1,1,bg);}return 1;}return 0;}
static int invaders_run(int ship){{volatile u32*sd=(volatile u32*)0x0203FFC4;if(FR_AUTO&&*sd){rs^=*sd;if(!rs)rs=1;*sd=0;}} // test-only RNG seed mailbox
const IvShip*S=&IVSH[ship];int au=FR_AUTO,bad=vb_fail();int px=120,lives=3,hp=S->hp,score=0,wave=1,fr=0,cd=0,inv=0,over=0,endT=0;
 int rapid=0,laser=0,ufoX=-40,ufoD=0,ufoT=600,stepT=0,step=0,anim=0,dir=1,ox=0,oy=0,boss=0,bossHp=0,bossX=120,bossD=1,bossMax=1,msgT=0;const char*msg="";int kills=0,pus=0,ufos=0;
 #define IVNEW() do{memset(ivPB,0,sizeof ivPB);memset(ivEB,0,sizeof ivEB);memset(ivPU,0,sizeof ivPU);memset(ivDeb,0,sizeof ivDeb);boss=wave%5==0;ox=0;oy=wave>1?((wave-1)%5)*4:0;dir=1;\
  for(int r=0;r<IV_ROWS;r++)for(int c=0;c<IV_COLS;c++)ivA[r][c]=!boss;if(boss){bossMax=bossHp=40+wave*12;bossX=120;}\
  fr_vstop();fr_bgspace();for(int i=0;i<3;i++)fx_disc_fb(30+i*90,20+i*7,1,RGB(200,200,220),RGB(255,255,255));m3_fill(0,152,240,1,RGB(80,90,120));iv_shields(wave);fr_save_bg();fr_vstart();msg=boss?"WARNING: HIVE QUEEN APPROACHING":"WAVE INCOMING";msgT=120;}while(0)
 IVNEW();bgm_t(TR_INVADE);
 for(;;fr++){int alive=0;for(int r=0;r<IV_ROWS;r++)for(int c=0;c<IV_COLS;c++)alive+=ivA[r][c];
  BEAT->screen=SCR_INVS;BEAT->mode=ship;BEAT->phase=over?4:1;BEAT->scoreA=score;BEAT->scoreB=wave;BEAT->hp=lives;BEAT->kills=kills;BEAT->x=px;BEAT->y=alive;BEAT->extra=boss?bossHp:0;BEAT->timer=pus*100+ufos;
  if(!over){
   // ---- input / autopilot
   int L=0,R=0,F=0;
   if(au&&!bad){int tx=px;int best=999;
    if(boss)tx=bossX;else for(int c=0;c<IV_COLS;c++){int any=0;for(int r=0;r<IV_ROWS;r++)any|=ivA[r][c];if(!any)continue;int ax=12+c*16+ox+5;if(iabs(ax-px)<best){best=iabs(ax-px);tx=ax;}}
    for(int i=0;i<12;i++)if(ivEB[i].on&&ivEB[i].y>96&&iabs(ivEB[i].x-px)<10){tx=px+(ivEB[i].x<px?18:-18);}
    for(int i=0;i<3;i++)if(ivPU[i].on)tx=ivPU[i].x;
    L=tx<px-1;R=tx>px+1;F=1;}
   else if(au&&bad){L=(fr/60)&1;R=!L;F=0;}
   else{L=(keys&K_LEFT)!=0;R=(keys&K_RIGHT)!=0;F=(keys&(K_A|K_B))!=0;}
   if(L)px-=S->spd;if(R)px+=S->spd;px=clampi(px,8,232);
   if(cd>0)cd--;if(F&&cd==0&&!laser){int rt=rapid?S->rate/3:S->rate;cd=rt;int n=S->spread?3:1;for(int k=0;k<n;k++)for(int i=0;i<8;i++)if(!ivPB[i].on){ivPB[i]=(IvB){px+(n>1?(k-1)*5:0),138,(s16)(n>1?(k-1):0),-5,1,0};break;}sfx1(0,0x9,1,2,1950,4);}
   // ---- player shots
   for(int i=0;i<8;i++)if(ivPB[i].on){IvB*b=&ivPB[i];b->y+=b->vy;if(fr&1)b->x+=b->vx;/* v1.3.3: narrower Falcon Heavy spread so all 3 cores can reach the Hive Queen */if(b->y<10){b->on=0;continue;}if(iv_shield_hit(b->x,b->y,1)){b->on=0;continue;}
    if(boss){if(iabs(b->x-bossX)<22&&b->y<40&&b->y>14){b->on=0;bossHp--;score+=10;if(bossHp<=0){score+=5000;boss=0;fsp_add(bossX,28,24,RGB(255,200,80));sfx_boom();shake(14);msg="HIVE QUEEN DESTROYED!";msgT=120;}}continue;}
    int c=(b->x-12-ox)/16,r=(b->y-20-oy)/12;if(c>=0&&c<IV_COLS&&r>=0&&r<IV_ROWS&&ivA[r][c]){int ax=12+c*16+ox,ay=20+r*12+oy;if(b->x>=ax&&b->x<ax+11&&b->y>=ay&&b->y<ay+8){ivA[r][c]=0;b->on=0;kills++;score+=(r==0?30:r<3?20:10)*(1+wave/6);fsp_add(ax+5,ay+4,6,IVC[r==0?0:r<3?1:2][0]);sfx1((1<<4)|3,0xA,2,1,1200,6);
      if(rr(14)==0)for(int q=0;q<3;q++)if(!ivPU[q].on){ivPU[q]=(IvB){ax+5,ay,0,1,1,(u8)rr(3)};break;}}}
    if(ufoD&&iabs(b->x-ufoX)<10&&b->y<18){b->on=0;int pts=(1+rr(6))*50;score+=pts;ufoD=0;ufos++;fsp_add(ufoX,12,12,RGB(255,80,80));sfx_boom();msg=cat3("MOTHERSHIP +",itoa(pts),"");msgT=60;}}
   // laser: continuous beam
   if(laser>0){laser--;int x=px;for(int r=IV_ROWS-1;r>=0;r--){int c=(x-12-ox)/16;if(c>=0&&c<IV_COLS&&ivA[r][c]&&(fr&3)==0){ivA[r][c]=0;kills++;score+=15;fsp_add(12+c*16+ox+5,20+r*12+oy+4,4,RGB(120,220,255));break;}}if(boss&&iabs(x-bossX)<22&&(fr&3)==0){bossHp--;score+=5;if(bossHp<=0){boss=0;score+=5000;sfx_boom();}}}
   if(rapid>0)rapid--;
   // ---- formation movement: faster as it thins and each wave
   if(!boss&&alive){int per=hmx(2,40-wave*3-(45-alive)*3/4);if(++stepT>=per){stepT=0;anim^=1;step=(step+1)&3;static const int TN[4]={430,470,520,580};sfx1(0,0x7,1,2,TN[step],3);
     int minc=9,maxc=-1;for(int c=0;c<IV_COLS;c++)for(int r=0;r<IV_ROWS;r++)if(ivA[r][c]){if(c<minc)minc=c;if(c>maxc)maxc=c;}
     int nx=ox+dir*4;if(12+minc*16+nx<4||12+maxc*16+nx+11>236){dir=-dir;oy+=6;}else ox=nx;}
    int lowest=0;for(int r=0;r<IV_ROWS;r++)for(int c=0;c<IV_COLS;c++)if(ivA[r][c])lowest=20+r*12+oy+8;if(lowest>=134){lives=0;msg="THE INVADERS LANDED";}}
   if(boss){bossX+=bossD*(1+wave/10);if(bossX<30||bossX>210)bossD=-bossD;}
   // ---- enemy fire
   int rate=hmx(12,70-wave*5);if(boss)rate=hmx(10,30-wave/2);
   if(fr%rate==0){int sx=-1,sy=0;if(boss){sx=bossX+rr(30)-15;sy=40;}else{int c=rr(IV_COLS);for(int r=IV_ROWS-1;r>=0;r--)if(ivA[r][c]){sx=12+c*16+ox+5;sy=20+r*12+oy+8;break;}}
    if(sx>=0)for(int i=0;i<12;i++)if(!ivEB[i].on){ivEB[i]=(IvB){sx,sy,(s16)(boss?rr(3)-1:0),(s16)(2+wave/8),1,0};break;}}
   for(int i=0;i<12;i++)if(ivEB[i].on){IvB*b=&ivEB[i];if(fr&1)b->x+=b->vx;b->y+=b->vy;if(b->y>154){b->on=0;continue;}if(iv_shield_hit(b->x,b->y,2)){b->on=0;continue;}
    if(inv==0&&b->y>136&&b->y<150&&iabs(b->x-px)<(ship==2?7:S->w)){b->on=0;if(--hp<=0){lives--;hp=S->hp;inv=120;fsp_add(px,142,16,RGB(255,180,60));sfx_boom();shake(10);}else{inv=40;sfx_hurt();}}}
   if(inv>0)inv--;
   // ---- debris drifting down (destructible, 5 pts) and power-ups
   if(fr%240==120)for(int i=0;i<4;i++)if(!ivDeb[i].on){ivDeb[i]=(IvB){(s16)(20+rr(200)),12,(s16)(rr(3)-1),1,1,0};break;}
   for(int i=0;i<4;i++)if(ivDeb[i].on){IvB*d=&ivDeb[i];if((fr&1)==0){d->y++;d->x+=d->vx*((fr&7)==0);}if(d->y>150){d->on=0;continue;}
    for(int k=0;k<8;k++)if(ivPB[k].on&&iabs(ivPB[k].x-d->x)<4&&iabs(ivPB[k].y-d->y)<4){ivPB[k].on=0;d->on=0;score+=5;fsp_add(d->x,d->y,4,RGB(160,150,140));}
    if(d->on&&inv==0&&d->y>136&&iabs(d->x-px)<(ship==2?7:S->w)){d->on=0;if(--hp<=0){lives--;hp=S->hp;inv=120;sfx_boom();}else{inv=40;sfx_hurt();}}}
   for(int i=0;i<3;i++)if(ivPU[i].on){IvB*p=&ivPU[i];p->y++;if(p->y>150){p->on=0;continue;}if(p->y>134&&iabs(p->x-px)<S->w+3){p->on=0;sfx_ok();pus++;
     if(p->k==0){rapid=600;msg="RAPTOR RAPID FIRE";}else if(p->k==1){lives++;msg="MECHAZILLA CATCH! +1 LIFE";}else{laser=300;msg="STARLINK LASER";}msgT=90;}}
   // ---- UFO mothership
   if(!boss&&!ufoD&&--ufoT<=0){ufoD=rr(2)?1:-1;ufoX=ufoD>0?-20:260;ufoT=900+rr(600);}
   if(ufoD){ufoX+=ufoD;if(ufoX<-30||ufoX>270)ufoD=0;}
   // ---- wave clear / game over
   if(!boss&&alive==0&&!bossHp){wave++;IVNEW();sfx_ok();}
   else if(!boss&&bossHp<=0&&wave%5==0){wave++;bossHp=0;IVNEW();}
   if(lives<=0){over=1;endT=fr;music_defeat();}}
  else if(fr-endT>150)break;
  next_frame();fr_vrestore();
  // ---- draw
  if(!boss)for(int r=0;r<IV_ROWS;r++){int k=r==0?0:r<3?1:2;for(int c=0;c<IV_COLS;c++)if(ivA[r][c])iv_blit(12+c*16+ox,20+r*12+oy,IVA[k][anim],11,8,IVC[k][0],IVC[k][1]);}
  if(boss){int bx=bossX-22;lf(bx+6,16,32,4,RGB(150,60,170));lf(bx,20,44,10,RGB(190,80,210));lf(bx+4,30,36,6,RGB(120,40,140));for(int k=0;k<5;k++)lf(bx+4+k*8,24,4,3,(fr>>3)&1?RGB(255,240,90):RGB(255,120,60));
   lf(60,10,120,3,RGB(40,20,40));lf(60,10,120*bossHp/(bossMax?bossMax:1),4,RGB(230,60,200));}
  if(ufoD){lf(ufoX-9,10,18,4,RGB(230,70,70));lf(ufoX-5,7,10,3,RGB(255,180,180));lf(ufoX-7,14,3,1,RGB(255,240,90));lf(ufoX+4,14,3,1,RGB(255,240,90));}
  for(int i=0;i<8;i++)if(ivPB[i].on)lf(ivPB[i].x,ivPB[i].y,1,4,RGB(255,240,120));
  for(int i=0;i<12;i++)if(ivEB[i].on)lf(ivEB[i].x-1,ivEB[i].y,2,4,(fr&2)?RGB(255,90,90):RGB(255,200,90));
  for(int i=0;i<4;i++)if(ivDeb[i].on){lf(ivDeb[i].x-2,ivDeb[i].y-2,5,4,RGB(120,112,104));lf(ivDeb[i].x-1,ivDeb[i].y-2,2,1,RGB(170,160,150));}
  for(int i=0;i<3;i++)if(ivPU[i].on){static const u16 PC[3]={RGB(255,140,40),RGB(240,200,60),RGB(110,200,255)};lf(ivPU[i].x-3,ivPU[i].y-3,7,7,PC[ivPU[i].k]);ltx(ivPU[i].x-2,ivPU[i].y-3,ivPU[i].k==0?"R":ivPU[i].k==1?"M":"S",RGB(10,10,20));}
  if(laser>0)lf(px,10,1,128,(fr&1)?RGB(150,230,255):RGB(255,255,255));
  if(!over&&!(inv&2)){int w=S->w;u16 c=S->c;if(ship==0){lf(px-1,130,3,12,c);lf(px-1,128,3,2,RGB(30,30,34));lf(px-3,140,7,2,RGB(60,60,66));}
   else if(ship==1){for(int k=-1;k<=1;k++){lf(px+k*4-1,k?132:129,3,k?10:13,c);}lf(px-6,140,13,2,RGB(60,60,66));}
   else if(ship==2){lf(px-3,124,7,18,c);lf(px-2,122,5,2,c);lf(px-5,126,2,4,RGB(70,70,76));lf(px+4,126,2,4,RGB(70,70,76));lf(px-5,138,2,4,RGB(70,70,76));lf(px+4,138,2,4,RGB(70,70,76));}
   else{lf(px-4,134,9,6,c);lf(px-3,131,7,3,c);lf(px-1,130,3,1,c);lf(px-3,135,2,2,RGB(30,40,60));lf(px-4,140,9,1,RGB(40,40,46));}
   if(fr&2)lf(px-1,142+(ship==2?0:0),3,2+(fr&4?1:0),RGB(255,170,60));(void)w;}
  fsp_run();
  ltx(4,1,cat("SCORE ",itoa(score)),C_WHITE);ltx(100,1,cat("WAVE ",itoa(wave)),C_YEL);ltx(160,1,cat("LIVES ",itoa(lives)),lives>1?C_GREEN:C_RED);if(S->hp>1)ltx(210,1,cat("HP",itoa(hp)),C_WHITE);
  ltx(4,153,cat("HI ",itoa(IVS.t[0].s>(u32)score?(int)IVS.t[0].s:score)),C_GRAY);if(rapid)ltx(90,153,"RAPTOR",RGB(255,140,40));if(laser)ltx(140,153,"LASER",RGB(110,200,255));
  if(msgT>0){msgT--;ltc(70,msg,C_YEL);}if(over)ltc(70,"GAME OVER",C_RED);if(au)ltx(206,153,bad?"FAIL":"AUTO",C_BLUE);}
 fr_vstop();BEAT->scoreA=score;BEAT->scoreB=wave;return score|(wave<<24);}
static void invaders_mode(void){ivs_load();static int ship=0;
 for(;;){int redraw=1;int go=0;
  for(int t=0;;t++){BEAT->screen=SCR_INV;BEAT->menu=ship;
   {u32 j=FR_JUMP;if((j>>16)==0x5249){FR_JUMP=0;ship=j&3;go=1;}}
   if(redraw){redraw=0;fr_bgspace();fr_panel(6,6,228,148);m3_ctext(10,"ROCKET INVADERS",C_YEL,2);m3_ctext(30,"DEFEND THE LAUNCH SITE!",RGB(110,200,230),1);
    for(int i=0;i<4;i++){int y=46+i*12;m3_text(16,y,i==ship?">":" ",C_YEL,1);m3_text(26,y,IVSH[i].n,i==ship?C_YEL:C_WHITE,1);}
    m3_text(16,108,IVSH[ship].d,C_GRAY,1);m3_text(140,46,"HIGH SCORES",C_YEL,1);for(int i=0;i<5;i++){m3_text(140,58+i*10,IVS.t[i].n,C_WHITE,1);m3_text(170,58+i*10,itoa(IVS.t[i].s),C_WHITE,1);}
    m3_text(16,120,"R RAPTOR RAPID FIRE  M MECHAZILLA +1",C_GRAY,1);m3_text(16,130,"S STARLINK LASER  BOSS EVERY 5 WAVES",C_GRAY,1);m3_ctext(143,"A: LAUNCH   B: BACK",C_GRAY,1);m3_present();}
   if(go)break;next_frame();
   if(pressed&K_UP){ship=(ship+3)&3;redraw=1;sfx_menu();}if(pressed&K_DOWN){ship=(ship+1)&3;redraw=1;sfx_menu();}
   if(pressed&(K_A|K_START)){sfx_ok();pressed=0;break;}if(pressed&K_B){pressed=0;return;}}
  int r=invaders_run(ship);u32 sc=r&0xFFFFFF;int wv=r>>24;IVS.games++;if(wv>IVS.bestWave)IVS.bestWave=wv;
  int rk=hi_rank(IVS.t,sc);if(rk>=0){char nm[4]="KCB";hi_initials(nm,cat("NEW HIGH SCORE ",itoa(sc)));hi_put(IVS.t,rk,sc,nm);}
  ivs_save();fr_result("GAME OVER",cat3("REACHED WAVE ",itoa(wv),""),rk>=0?cat("SHARE CODE ",hi_code(1,sc,IVS.t[rk].n)):"",rk>=0,sc,rk==0);bgm_t(TR_INVADE);if(FR_AUTO)return;}}
static void rec_invade(const char**L){ivs_load();static char b[5][40] EWRAM;for(int i=0;i<5;i++){char*o=sp_put(b[i],IVS.t[i].n);o=sp_put(o,"  ");sp_int(o,IVS.t[i].s);L[i]=b[i];}}
