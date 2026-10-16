// ---------------------------------------------------------------- shared arena helpers (Mode 3, 30 fps: static layer in bigbuf)
static void fr_earth_bg(int dy,int shift){ // menu Earth (sphere LUT) pushed down by dy rows, no HUD panel
 for(int y=0;y<160;y++){int sy=y-dy;if(sy<0||sy>=160)continue;int x0=ea_row[sy*3],n=ea_row[sy*3+1],o=ea_row[sy*3+2];
  for(int i=0;i<n;i++){int l=ea_lut[o+i];int u=l&511,v=l>>9;int sh=ea_sh[o+i];int t=ea_tex[(v<<9)+((u+shift)&511)];int c=ea_cld[(v<<9)+((u+shift*2)&511)];
   if(c==2)t=7;else if(c==1&&t!=6&&((x0+i+y)&1))t=8;if(t==6&&(sh&15))t=2;fb[y*240+x0+i]=ea_col[(t*16+(sh&15))*4+(sh>>4)];}}
 for(int x=0;x<240;x++){int t=ea_top[x]+dy;static const u16 GL[5]={RGB(10,20,60),RGB(20,44,110),RGB(40,90,180),RGB(80,150,230),RGB(160,215,255)};for(int k=0;k<5;k++){int y=t-5+k;if(y>=0&&y<160)fb[y*240+x]=GL[k];}}}
#define FRV ((u16*)0x06000000)
static void fr_save_bg(void){dma32(fb,bigbuf,240*160/2);}
// dirty-rect arena: draw into fb (logged), after vsync copy only last + this frame's rects to VRAM
static LRect lfP[384] EWRAM;static int lfPN;
static void fr_vstart(void){dma32(bigbuf,fb,240*160/2);vsync();dma32(bigbuf,FRV,240*160/2);REG_DISPCNT=3|(1<<10);lfb=fb;lfN=0;lfPN=0;}
IWCODE static void fr_cpy(const LRect*R,int n,const u16*src,u16*dst){for(int k=0;k<n;k++){const LRect*r=&R[k];int w=r->w,h=r->h;const u16*s=&src[r->y*240+r->x];u16*d=&dst[r->y*240+r->x];
  for(int j=0;j<h;j++,s+=240,d+=240)for(int i=0;i<w;i++)d[i]=s[i];}}
static void fr_vrestore(void){ // call at the start of drawing: show last frame, then erase it from the back buffer
 fr_cpy(lfP,lfPN,fb,FRV);fr_cpy(lfR,lfN,fb,FRV);fr_cpy(lfR,lfN,bigbuf,fb);for(int k=0;k<lfN;k++)lfP[k]=lfR[k];lfPN=lfN;lfN=0;}
static void fr_vstop(void){lfN=-1;lfb=0;}
static void fr_load_bg(void){dma32(bigbuf,fb,240*160/2);}
static const char*fr_s2(const char*a,int v1,const char*b,int v2){static char o[48];int k=0;const char*p=a;while(*p&&k<46)o[k++]=*p++;p=itoa(v1);while(*p&&k<46)o[k++]=*p++;p=b;while(*p&&k<46)o[k++]=*p++;if(v2!=-99999){p=itoa(v2);while(*p&&k<46)o[k++]=*p++;}o[k]=0;return o;}
typedef struct{s32 x,y;s16 vx,vy;u8 on,k,hp,t;s16 a,b;}FE;
static FE fe[40] EWRAM,fpb[24] EWRAM,feb[32] EWRAM;
typedef struct{s16 x,y,vx,vy;u8 t;u16 c;}FSp;static FSp fsp[48] EWRAM;
static void fsp_add(int x,int y,int n,u16 c){for(int k=0;k<n;k++)for(int i=0;i<48;i++)if(!fsp[i].t){FSp*p=&fsp[i];p->x=x<<4;p->y=y<<4;p->vx=rr(64)-32;p->vy=rr(64)-32;p->t=10+rr(14);p->c=c;break;}}
static void fsp_run(void){for(int i=0;i<48;i++)if(fsp[i].t){FSp*p=&fsp[i];p->t--;p->x+=p->vx;p->y+=p->vy;int x=p->x>>4,y=p->y>>4;lf(x,y,1,1,p->t<5?dimc(p->c):p->c);}}
static void fr_clear_pools(void){for(int i=0;i<40;i++)fe[i].on=0;for(int i=0;i<24;i++)fpb[i].on=0;for(int i=0;i<32;i++)feb[i].on=0;for(int i=0;i<48;i++)fsp[i].t=0;}
static FE*fe_new(FE*P,int n){for(int i=0;i<n;i++)if(!P[i].on){FE*e=&P[i];e->on=1;e->t=0;e->a=e->b=0;e->hp=1;e->vx=e->vy=0;return e;}return 0;}
static void fr_bar(int x,int y,int w,int v,int mx,u16 c){lf(x,y,w,4,RGB(30,30,44));if(mx>0)lf(x,y,w*clampi(v,0,mx)/mx,4,c);}
static int fr_result(const char*title,const char*l1,const char*l2,int ok,int score,int best){if(ok)music_victory();else music_defeat();
 for(int t=0;;t++){BEAT->phase=ok?3:4;BEAT->scoreA=score;
  if(t==0){fr_bgspace();fr_earth_bg(56,frame&511);fr_panel(16,24,208,112);m3_ctext(34,title,ok?C_YEL:C_RED,2);m3_ctext(58,l1,C_WHITE,1);m3_ctext(70,l2,C_WHITE,1);
   m3_ctext(88,cat("SCORE ",itoa(score)),C_WHITE,2);if(best)m3_ctext(108,"NEW RECORD!",C_YEL,1);m3_ctext(124,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();if(t>30&&((pressed&(K_A|K_B|K_START))||(FR_AUTO&&t>60))){sfx_ok();pressed=0;return ok;}}}
static int fr_menu(const char*title,const char*sub,const char*const*items,int n,const u8*done,int*sel,int scr){int redraw=1;
 for(;;){BEAT->screen=scr;BEAT->menu=*sel;BEAT->phase=0;
  if(redraw){redraw=0;fr_bgspace();fr_panel(8,6,224,148);m3_ctext(12,title,C_YEL,2);m3_ctext(30,sub,RGB(110,200,230),1);
   for(int i=0;i<n;i++){int y=46+i*11;m3_text(18,y,i==*sel?">":" ",C_YEL,1);m3_text(26,y,items[i],i==*sel?C_YEL:C_WHITE,1);if(done&&done[i])m3_text(210,y,"*",C_GREEN,1);}
   m3_ctext(142,"A: START   B: BACK",C_GRAY,1);m3_present();}
  next_frame();{u32 j=FR_JUMP;if((j&0xFFFFFF00)==0x46520000+(scr<<8)){FR_JUMP=0;*sel=j&255;return *sel;}}
  if(pressed&K_UP){*sel=(*sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&K_DOWN){*sel=(*sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();pressed=0;return *sel;}if(pressed&K_B){sfx_menu();pressed=0;return -1;}}}
// ===================================================================== ORBITAL DEFENSE
// Side-scrolling orbital shooter over the Earth's limb: shatter debris (big pieces split), rogue satellites shoot back,
// a Kessler cascade, and the SONNENGEWEHR "sun gun" mirror station (fictional what-if) as the boss.
enum{OK_ROCK,OK_ROGUE,OK_ASAT,OK_PANEL,OK_CORE,OK_SAM,OK_MIG};
static const char*const ODN[4]={"1 LOW ORBIT CLEANUP","2 ROGUE SATELLITES","3 KESSLER CASCADE","4 STOP THE SUN GUN"};
static int odPx,odPy,odHp,odEn,odSc,odKills,odShots,odHits,odDmg;
static void od_rock(int x,int y,int sz,int vx,int vy){FE*e=fe_new(fe,40);if(!e)return;e->k=OK_ROCK;e->x=x<<8;e->y=y<<8;e->vx=vx;e->vy=vy;e->hp=sz==2?3:sz==1?2:1;e->a=sz;e->b=rr(256);}
static void od_draw_rock(int x,int y,int sz,int rot){int r=sz==2?7:sz==1?4:2;u16 c0=RGB(150,140,130),c1=RGB(100,92,86),c2=RGB(200,190,176);
 for(int j=-r;j<=r;j++){int w=r-iabs(j)/2+((j+rot/32)&1);lf(x-w,y+j,w*2,1,j<0?c2:j>r/2?c1:c0);}if(r>3){lf(x-1+(rot>>6),y-1,2,2,c1);lf(x+2,y+2,1,1,c1);}}
static void od_draw_sat(int x,int y,int k,int t){u16 B=RGB(200,200,206),P=RGB(60,90,170),R=RGB(220,60,50);
 if(k==OK_ROGUE){lf(x-3,y-3,6,6,B);lf(x-11,y-1,7,3,P);lf(x+4,y-1,7,3,P);lf(x-1,y-6,2,3,R);if(t&8)lf(x,y-7,1,1,RGB(255,80,80));}
 else{lf(x-4,y-2,8,4,RGB(120,124,130));for(int j=0;j<4;j++)lf(x+4+j,y-2+j/2,1,4-j,R);lf(x-6,y-1,2,2,RGB(255,200,80));}}
static void od_draw_player(int x,int y,int inv){if(inv&&(frame&2))return;u16 W=RGB(236,236,232),G=RGB(220,180,60),P=RGB(50,80,160),C=RGB(90,210,240);
 lf(x-5,y-3,10,7,W);lf(x-5,y-3,10,1,G);lf(x-14,y-1,9,3,P);lf(x+5,y-1,9,3,P);for(int k=0;k<3;k++){lf(x-13+k*3,y-1,1,3,RGB(30,40,90));lf(x+6+k*3,y-1,1,3,RGB(30,40,90));}
 lf(x+5,y,4,1,C);lf(x-1,y-6,2,3,RGB(170,170,176));if(frame&4)lf(x-7,y,2,1,RGB(255,190,80));}
// Sun gun boss: 8 mirror panels around a focus core; charges a beam at a city below
static int sgCharge,sgBurns,sgPanels,sgX,sgY,sgTx;
static void od_sungun_spawn(void){sgX=196;sgY=50;sgCharge=0;sgBurns=0;sgPanels=8;sgTx=60+rr(120);
 for(int i=0;i<8;i++){FE*e=fe_new(fe,40);e->k=OK_PANEL;e->a=i;e->hp=4;e->x=sgX<<8;e->y=sgY<<8;}
 FE*c=fe_new(fe,40);c->k=OK_CORE;c->hp=14;c->x=sgX<<8;c->y=sgY<<8;}
static void od_draw_sungun(void){int cx=sgX,cy=sgY;lf(cx-1,cy-30,2,60,RGB(90,92,100));lf(cx-26,cy-1,52,2,RGB(90,92,100));
 // focus beam telegraph
 if(sgCharge>0){int ty=ea_top[sgTx]+26;int n=sgCharge>700?3:1;for(int k=0;k<40;k++){int x=cx+(sgTx-cx)*k/40,y=cy+(ty-cy)*k/40;u16 c=sgCharge>760?RGB(255,255,200):sgCharge>600?RGB(255,220,120):RGB(255,170,60);
   if(sgCharge>700||((k+frame)&3)==0)lf(x,y,n,n,c);}if(sgCharge>500&&(frame&4))ltx(sgTx-24,ty-14,"TARGET",C_RED);}}
static void od_draw_panel(int x,int y,int hp){u16 m=hp>2?RGB(230,236,250):RGB(170,176,196);lf(x-6,y-4,12,8,m);lf(x-6,y-4,12,1,RGB(255,255,255));lf(x-6,y+3,12,1,RGB(120,126,150));if(frame&16)lf(x-3,y-2,2,2,RGB(255,255,255));}
static int od_run(int mis){fr_clear_pools();odPx=40;odPy=60;odHp=12;odEn=100;odSc=0;odKills=0;odShots=odHits=0;odDmg=0;int inv=0,cd=0,spawnT=0,waveN=0,goal=0,tmr=0,res=-1,endT=0,shieldT=0;
 fr_bgspace();fr_earth_bg(28,mis*60);fr_save_bg();fr_vstart();bgm_t(mis==3?TR_BOSS:TR_FUTURE);
 if(mis==3)od_sungun_spawn();int need=mis==0?30:mis==1?8:0;int dur=mis==2?1800:0;
 for(int fr=0;;fr++){BEAT->screen=SCR_ODEF;BEAT->mode=mis;BEAT->hp=odHp;BEAT->scoreA=odSc;BEAT->kills=odKills;BEAT->timer=tmr;BEAT->phase=res<0?1:res?3:4;BEAT->deaths=odDmg;BEAT->x=odPx;BEAT->y=odPy;BEAT->extra=mis==3?sgPanels|(sgBurns<<8):goal;
  if(res>=0&&fr-endT>60)break;
  int up=keys&K_UP,dn=keys&K_DOWN,lt=keys&K_LEFT,rt=keys&K_RIGHT,fire=keys&K_A,shp=pressed&(K_L|K_R);
  if(FR_AUTO){int ty=odPy,best=9999;for(int i=0;i<40;i++)if(fe[i].on){int ex=fe[i].x>>8,ey=fe[i].y>>8;if(ex<odPx)continue;int d=ex-odPx;if(d<best){best=d;ty=ey;}}
   up=ty<odPy-2;dn=ty>odPy+2;lt=odPx>44;rt=odPx<36;fire=1;shp=0;
   for(int i=0;i<32;i++)if(feb[i].on){int bx=feb[i].x>>8,by=feb[i].y>>8;if(iabs(bx-odPx)<16&&iabs(by-odPy)<10){if(odEn>30)shp=1;else{up=by>odPy;dn=!up;}}}
   for(int i=0;i<40;i++)if(fe[i].on&&fe[i].k!=OK_PANEL&&fe[i].k!=OK_CORE){int ex=fe[i].x>>8,ey=fe[i].y>>8;if(ex>odPx&&ex-odPx<34&&iabs(ey-odPy)<10){if(odEn>=30&&ex-odPx<16)shp=1;up=ey>=odPy;dn=!up;if(odPy<20){up=0;dn=1;}if(odPy>ea_top[odPx]+16){dn=0;up=1;}}}}
  if(res<0){tmr++;
   {int sp=(fr&1)?2:1;if(up&&odPy>14)odPy-=sp;if(dn&&odPy<ea_top[odPx]+20)odPy+=sp;if(lt&&odPx>12)odPx-=sp;if(rt&&odPx<150)odPx+=sp;}
   if(cd>0)cd--;if(fire&&cd==0){FE*b=fe_new(fpb,24);if(b){b->x=(odPx+9)<<8;b->y=odPy<<8;b->vx=0x600;b->k=0;odShots++;sfx_laser();}cd=6;}
   if(shp&&odEn>=30&&!shieldT){odEn-=30;shieldT=40;sfx_whoosh();}if(shieldT)shieldT--;if(odEn<100&&(fr&3)==0)odEn++;if(inv)inv--;
   // spawning
   spawnT--;if(mis<3&&spawnT<=0){waveN++;
    if(mis==0||mis==2||rr(3)){int n=mis==2?1+rr(3):1+rr(2);for(int k=0;k<n;k++)od_rock(244,16+rr(ea_top[200]-10),rr(3)==0?2:1+rr(2)-1,-(0x60+rr(0x80)+mis*0x20),rr(0x40)-0x20);}
    if(mis==1&&waveN%2==0){FE*e=fe_new(fe,40);if(e){e->k=OK_ROGUE;e->x=244<<8;e->y=(20+rr(70))<<8;e->vx=-0x70;e->hp=4;}}
    if(mis==2&&waveN%5==0){FE*e=fe_new(fe,40);if(e){e->k=OK_ASAT;e->x=244<<8;e->y=(20+rr(70))<<8;e->vx=-0xC0;e->hp=2;}}
    spawnT=mis==2?48:mis==1?70:55;}
   if(mis==3){if(sgPanels>0||1){sgCharge++;if(sgCharge>=900){sgBurns++;sgCharge=0;shake(14);sfx_boom();fsp_add(sgTx,ea_top[sgTx]+26,20,RGB(255,200,80));sgTx=60+rr(120);if(sgBurns>=3){res=0;endT=fr;}}}
    spawnT--;if(spawnT<=0){spawnT=90;od_rock(244,20+rr(80),0,-0x90,0);if(rr(2)){FE*e=fe_new(fe,40);if(e){e->k=OK_ROGUE;e->x=244<<8;e->y=(20+rr(70))<<8;e->vx=-0x70;e->hp=3;}}}}
   // enemies
   for(int i=0;i<40;i++)if(fe[i].on){FE*e=&fe[i];e->t++;
    if(e->k==OK_PANEL){int a=e->a*32+(fr>>1);e->x=(sgX+(COS(a)*30>>8))<<8;e->y=(sgY+(SIN(a)*24>>8))<<8;}
    else if(e->k==OK_CORE){e->x=sgX<<8;e->y=sgY<<8;}
    else{e->x+=e->vx;e->y+=e->vy;if(e->k==OK_ROGUE){e->vy=SIN(e->t*3)>>2;if(e->t%70==35){FE*b=fe_new(feb,32);if(b){int dx=odPx-(e->x>>8),dy=odPy-(e->y>>8);int d=idist(dx,dy)+1;b->x=e->x;b->y=e->y;b->vx=dx*0x280/d;b->vy=dy*0x280/d;}}}
     if(e->k==OK_ASAT){int dy=odPy-(e->y>>8);e->vy=clampi(dy*8,-0x100,0x100);}
     if((e->x>>8)<-20||(e->y>>8)<-20||(e->y>>8)>180){e->on=0;continue;}}
    int ex=e->x>>8,ey=e->y>>8;int r=e->k==OK_ROCK?(e->a==2?7:e->a==1?4:2):e->k==OK_PANEL?6:e->k==OK_CORE?8:6;
    if(e->k==OK_CORE&&sgPanels>0){}else if(!inv&&iabs(ex-odPx)<r+6&&iabs(ey-odPy)<r+3&&e->k!=OK_PANEL){if(shieldT){e->hp=0;}else{odHp-=e->k==OK_ASAT?4:2;odDmg++;inv=50;shake(6);sfx_hurt();if(e->k!=OK_CORE)e->on=0;}}
    for(int j=0;j<24;j++)if(fpb[j].on){FE*b=&fpb[j];int bx=b->x>>8,by=b->y>>8;if(iabs(bx-ex)<=r+2&&iabs(by-ey)<=r+1){
      if(e->k==OK_CORE&&sgPanels>0){b->on=0;fsp_add(bx,by,2,RGB(200,200,255));continue;}
      b->on=0;odHits++;e->hp--;fsp_add(bx,by,3,RGB(255,220,140));if(e->hp>0)sfx1(0x08,0x6,1,2,1500,4);}}
    if(e->hp==0&&e->on){e->on=0;odKills++;sfx_boom();fsp_add(ex,ey,10,RGB(255,180,80));
     switch(e->k){case OK_ROCK:odSc+=e->a==2?50:e->a==1?30:15;if(e->a>0){for(int k=0;k<2;k++)od_rock(ex,ey,e->a-1,e->vx-0x40+rr(0x40),(k?1:-1)*(0x40+rr(0x40)));}goal++;break;
      case OK_ROGUE:odSc+=150;goal+=mis==1;break;case OK_ASAT:odSc+=200;break;case OK_PANEL:odSc+=250;sgPanels--;sgCharge=sgCharge>150?sgCharge-150:0;break;case OK_CORE:odSc+=2000;res=1;endT=fr;shake(20);break;}}}
   for(int j=0;j<24;j++)if(fpb[j].on){FE*b=&fpb[j];b->x+=b->vx;if((b->x>>8)>244)b->on=0;}
   for(int j=0;j<32;j++)if(feb[j].on){FE*b=&feb[j];b->x+=b->vx;b->y+=b->vy;int bx=b->x>>8,by=b->y>>8;if(bx<-4||bx>244||by<-4||by>164){b->on=0;continue;}
    if(iabs(bx-odPx)<8&&iabs(by-odPy)<4){b->on=0;if(shieldT)continue;if(!inv){odHp-=2;odDmg++;inv=40;sfx_hurt();}}}
   if(odHp<=0){res=0;endT=fr;}
   if(mis==0&&goal>=need){res=1;endT=fr;}if(mis==1&&goal>=need){res=1;endT=fr;}if(mis==2&&tmr>=dur){res=1;endT=fr;}}
  next_frame();fr_vrestore();
  if(mis==3)od_draw_sungun();
  for(int i=0;i<40;i++)if(fe[i].on){FE*e=&fe[i];int ex=e->x>>8,ey=e->y>>8;
   switch(e->k){case OK_ROCK:od_draw_rock(ex,ey,e->a,e->b+fr*4);break;case OK_PANEL:od_draw_panel(ex,ey,e->hp);break;
    case OK_CORE:{u16 c=sgPanels>0?RGB(120,120,140):(fr&2)?RGB(255,240,180):RGB(255,160,60);for(int j=-7;j<=7;j++){int w=7-iabs(j)/2;lf(ex-w,ey+j,w*2,1,c);}lf(ex-3,ey-3,6,6,RGB(255,255,230));break;}
    default:od_draw_sat(ex,ey,e->k,e->t);}}
  for(int j=0;j<24;j++)if(fpb[j].on)lf((fpb[j].x>>8)-3,fpb[j].y>>8,5,1,RGB(120,255,255));
  for(int j=0;j<32;j++)if(feb[j].on)lf(feb[j].x>>8,feb[j].y>>8,2,2,RGB(255,90,80));
  od_draw_player(odPx,odPy,inv);if(shieldT){for(int a=0;a<32;a++)lf(odPx+(COS(a*8)*14>>8),odPy+(SIN(a*8)*9>>8),1,1,RGB(120,220,255));}
  fsp_run();
  // HUD
  ltx(4,3,"HULL",C_GRAY);fr_bar(30,4,40,odHp,12,odHp>4?C_GREEN:C_RED);ltx(76,3,"EN",C_GRAY);fr_bar(90,4,30,odEn,100,C_BLUE);ltx(128,3,cat("SCORE ",itoa(odSc)),C_WHITE);
  if(mis==0)ltx(4,12,fr_s2("DEBRIS ",goal,"/",need),C_YEL);else if(mis==1)ltx(4,12,fr_s2("ROGUE SATS ",goal,"/",need),C_YEL);
  else if(mis==2)ltx(4,12,cat3("HOLD ",itoa((dur-tmr)/60)," S"),C_YEL);else{ltx(4,12,cat("MIRRORS ",itoa(sgPanels)),C_YEL);ltx(70,12,cat3("BURNS ",itoa(sgBurns),"/3"),sgBurns?C_RED:C_YEL);fr_bar(150,13,60,sgCharge,900,RGB(255,180,60));ltx(212,12,"!",sgCharge>700?C_RED:C_GRAY);}
  if(fr<150&&res<0)ltc(70,mis==3?"BREAK THE MIRRORS - THEN THE CORE!":ODN[mis]+2,C_YEL);
  if(res>=0)ltc(70,res?"SECTOR CLEAR":"DEFENSE FAILED",res?C_GREEN:C_RED);
  if(FR_AUTO)ltx(196,150,"AUTO",C_BLUE);
  if(pressed&K_START&&!FR_AUTO){int q=0;ltc(76,"PAUSED - START RESUME, B QUIT",C_YEL);for(;;){next_frame();if(pressed&K_START)break;if(pressed&K_B){q=1;break;}}if(q){res=0;break;}}}
 fr_vstop();
 int acc=odShots?odHits*100/odShots:0;int sc=odSc+(res==1?1000+odHp*50+acc*5:0);return res==1?sc:-sc-1;}
static void orbital_defense(int boss){v131_load();static int sel=0;
 if(boss){int r=od_run(3);int sc=r<0?-r-1:r;fr_best(2,sc);fr_result(r>=0?"SUN GUN DESTROYED":"THE SUN GUN FIRED",cat("SCORE ",itoa(sc)),"WHAT-IF 1945: STATION HAZARD",r>=0,sc,0);return;}
 for(;;){u8 dn[4];for(int i=0;i<4;i++)dn[i]=VS.prog[2]>i;
  int m=fr_menu("ORBITAL DEFENSE","DEBRIS, ROGUE SATS, THE SUN GUN",ODN,4,dn,&sel,SCR_ODEF);if(m<0)return;
  if(m>VS.prog[2]&&!FR_AUTO){guide_tip("LOCKED","CLEAR THE EARLIER SECTORS FIRST.");continue;}
  int r=od_run(m);int sc=r<0?-r-1:r;int nb=fr_best(2,sc);
  if(r>=0&&VS.prog[2]<=m){VS.prog[2]=m+1;v131_save();}
  fr_result(r>=0?"SECTOR CLEAR":"DEFENSE FAILED",ODN[m],fr_s2("KILLS ",odKills,"  HITS TAKEN ",odDmg),r>=0,sc,nb);bgm_t(TR_SPACE);}}
