// ===================================================================== 6v6 MODES
enum{MM_INF=1,MM_TANK=2,MM_AIR=3};
static const char*const TEAMN[5][2]={{"ALLIES","AXIS"},{"NATO","PACT"},{"BLUFOR","OPFOR"},{"UNION","SYNDIC"},{"TERRAN","VOID"}};
#define TV(u) (&TANKV[mpEra][(u)->veh])
#define AV(u) (&AIRV[mpEra][(u)->veh])
static Wstat PW[2];static int pSpeed,pPerk,pLeth,pTact,stealth;
static int mpMode,mpEra,score[2],mpTimer,msgT,msgB;static const char*msgS;static char msgBuf[32];
static int worldPx(void){return mapW*16;}
static int bulletTile(void){return mpEra<3?FX_BULLET:mpEra==3?FX_LASER:FX_PLASMA;}
static void feed(const char*s,int b){int k=0;while(s[k]&&k<30){msgBuf[k]=s[k];k++;}msgBuf[k]=0;msgS=msgBuf;msgT=120;msgB=b;}
// ---------------------------------------------------------------- map generation
static void gen_arena(int W,int Hh,int tankmap){
 mapW=W;mapH=Hh;mapWrap=0;borderTile=AM_BORDER;rs=0xBEEF+mpEra*977+tankmap*31;
 for(int i=0;i<32;i++)tflag[i]=0;tflag[AM_WALL]=tflag[AM_CRATE]=tflag[AM_BAGS]=tflag[AM_TREE]=tflag[AM_BORDER]=TF_SOLID|TF_SHOT;
 for(int y=0;y<Hh;y++)for(int x=0;x<W;x++){int r=rr(100);map[y*MAXW+x]=r<70?AM_G0:r<93?AM_G1:AM_G2;}
 for(int x=0;x<W;x++){map[(Hh/2)*MAXW+x]=AM_PATH;}for(int y=0;y<Hh;y++)map[y*MAXW+W/2]=AM_PATH;
 int nf=tankmap?16:26;
 for(int f=0;f<nf;f++){int x=2+rr(W-4),y=2+rr(Hh/2-2),k=rr(10);
  if(idist(x-W/2,y-3)<(tankmap?9:6))continue;
  int len=3+rr(3);
  #define SETC(xx,yy,v) do{int _x=(xx),_y=(yy);if(_x>1&&_y>1&&_x<W-2&&_y<Hh-2){map[_y*MAXW+_x]=v;map[(Hh-1-_y)*MAXW+(W-1-_x)]=v;}}while(0)
  if(k<3){int h=rr(2);for(int i=0;i<len;i++)SETC(x+(h?i:0),y+(h?0:i),AM_WALL);}
  else if(k<5){SETC(x,y,AM_CRATE);SETC(x+1,y,AM_CRATE);if(!tankmap)SETC(x,y+1,AM_CRATE);}
  else if(k<7){for(int i=0;i<3;i++)SETC(x+i,y,AM_BAGS);}
  else if(k<9){SETC(x,y,AM_TREE);if(rr(2))SETC(x+1,y+1,AM_TREE);}
  else{SETC(x,y,AM_CRATER);}
 }
 for(int y=0;y<Hh;y++){map[y*MAXW]=map[y*MAXW+W-1]=AM_BORDER;}for(int x=0;x<W;x++){map[x]=map[(Hh-1)*MAXW+x]=AM_BORDER;}
 for(int i=-2;i<=2;i++){map[2*MAXW+W/2+i]=AM_SPAWN;map[(Hh-3)*MAXW+W/2+i]=AM_SPAWN;}
 for(int y=3;y<6;y++)for(int i=-3;i<=3;i++){if(map[y*MAXW+W/2+i]!=AM_PATH)map[y*MAXW+W/2+i]=AM_G0;if(map[(Hh-1-y)*MAXW+W/2+i]!=AM_PATH)map[(Hh-1-y)*MAXW+W/2+i]=AM_G0;}}
static int vnoise(int x,int y,int seed,int sh){ // 0..255, periodic over 64; cell = 1<<sh
 int cs=1<<sh,gx=x>>sh,gy=y>>sh,fx=x&(cs-1),fy=y&(cs-1),n=(64>>sh)-1;
 #define HV(a,b) ((int)(((((a)&n)*73856093u)^(((b)&n)*19349663u)^(seed*83492791u))*2654435761u>>24))
 int v00=HV(gx,gy),v10=HV(gx+1,gy),v01=HV(gx,gy+1),v11=HV(gx+1,gy+1);
 int a=v00+(((v10-v00)*fx)>>sh),b=v01+(((v11-v01)*fx)>>sh);return a+(((b-a)*fy)>>sh);}
#define RB(m) ((int)(rnd()>>9)&(m))
static void gen_sky(void){
 mapW=mapH=64;mapWrap=1;borderTile=0;for(int i=0;i<32;i++)tflag[i]=0;rs=0xA11+mpEra*55;
 for(int y=0;y<64;y++)for(int x=0;x<64;x++){int v=(vnoise(x,y,mpEra+1,4)*2+vnoise(x,y,mpEra+7,3))/3;int t;
  if(mpEra==4){t=v<110?(RB(15)==0?7:(RB(3)?0:1)):v<150?2:v<175?3:4;if(RB(63)==0)t=5;if(RB(127)==0)t=6;}
  else if(mpEra==3){t=v<90?RB(1):v<100?2:v<200?3+(RB(3)%3):(RB(1)?6:7);if((x&7)==0||(y&7)==0)t=(x&7)==0?2:1;}
  else{t=v<120?(RB(15)==0?1:0):v<132?2:v<170?(RB(3)?3:6):v<200?4:5;if(t==0&&RB(255)==0)t=7;}
  map[y*MAXW+x]=t;
  int c=vnoise(x,y,mpEra+31,3);cmap[y*MAXW+x]=(mpEra!=4&&c>170)?24+(RB(7)%7):31;}}
// ---------------------------------------------------------------- units
static void spawn(Unit*u,int i){
 int team=u->team;int W=worldPx(),Hp=mapH*16;
 u->alive=1;u->inv=90;u->hurt=0;u->vel=0;u->roll=0;u->evade=0;u->wander=0;u->tgt=-1;u->cd=30;u->cd2=0;
 int slot=(i%6)-3;
 if(mpMode==MM_AIR){u->x=((W/2+slot*40)&(W-1))<<8;u->y=(team?120:Hp-120)<<8;u->ang=team?0x8000:0;u->vel=AV(u)->spd;u->hp=u->maxhp=AV(u)->hp;u->ammo=AV(u)->ammo;}
 else{int sp=mpMode==MM_TANK?36:20;u->x=(W/2+slot*sp+sp/2)<<8;u->y=(team?3*16+8:Hp-3*16-8)<<8;
  if(mpMode==MM_TANK){u->y+=(team?16:-16)<<8;}
  for(int k=0;k<20&&!box_free(u->x,u->y,mpMode==MM_TANK?10:5);k++)u->x+=(8<<8);
  u->ang=u->tur=team?0x8000:0;u->hp=u->maxhp=mpMode==MM_TANK?TV(u)->hp:(u->isp&&pPerk==4?130:100);u->gren=2;u->tact=u->isp?(pTact==0?2:1):0;u->rl=0;u->stun=0;
  if(u->isp){u->mag[0]=PW[0].mag;u->mag[1]=PW[1].mag;}}
 u->lx=u->x;u->ly=u->y;}
static void damage(int vi,int dmg,int ai){Unit*v=&U[vi];if(!v->alive||v->inv>0||v->roll>0)return;v->hp-=dmg;v->hurt=6;if(v->isp){sfx_hurt();}
 if(v->hp<=0){v->hp=0;v->alive=0;v->resp=mpMode==MM_TANK?240:180;v->deaths++;score[1-v->team]++;if(ai>=0)U[ai].kills++;
  if(ai==0&&mpMode==MM_INF&&pPerk==2){Unit*p=&U[0];if(p->gren<2)p->gren++;p->mag[0]=PW[0].mag;p->mag[1]=PW[1].mag;p->rl=0;}
  sfx_boom();fx_add(v->x>>8,v->y>>8,mpMode==MM_INF?FXK_SMALL:FXK_BIG);fx_add(v->x>>8,(v->y>>8)-4,FXK_SMOKE);
  if(ai==0)feed(mpMode==MM_INF?"+1 KILL":"+1 KILL - TARGET DESTROYED",HB_GREEN);else if(vi==0)feed("YOU WERE KILLED",HB_RED);}}
static int dxw(s32 a,s32 b){return wrapd((a-b)>>8,worldPx());}
static int dyw(s32 a,s32 b){return wrapd((a-b)>>8,mapH*16);}
static int nearest(int i,int maxd,int needlos){Unit*u=&U[i];int best=-1,bd=maxd;
 for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==u->team)continue;int d=idist(dxw(e->x,u->x),dyw(e->y,u->y));if(k==0&&stealth&&needlos&&d>130)continue;
  if(d<bd&&(!needlos||los(u->x>>8,u->y>>8,e->x>>8,e->y>>8))){bd=d;best=k;}}return best;}
static int steer(Unit*u,int a,int probe,int r){static const s8 off[9]={0,24,-24,48,-48,72,-72,100,-100};
 for(int k=0;k<9;k++){int b=(a+off[k])&255;if(box_free(u->x+SIN(b)*probe,u->y-COS(b)*probe,r)&&box_free(u->x+SIN(b)*probe/2,u->y-COS(b)*probe/2,r))return b;}return (a+128)&255;}
static void fire_bullet(int i,int a,int spd,int dmg,int life,int kind,int ox){Unit*u=&U[i];Shot*s=shot_new();if(!s)return;
 int px=-COS(a),py=-SIN(a); // perpendicular
 s->x=u->x+SIN(a)*10+px*ox;s->y=u->y-COS(a)*10+py*ox;s->vx=SIN(a)*spd>>8;s->vy=-COS(a)*spd>>8;
 if(mpMode==MM_AIR){s->vx+=SIN(u->ang>>8)*u->vel>>8;s->vy-=COS(u->ang>>8)*u->vel>>8;}
 s->life=life;s->dmg=dmg;s->team=u->team;s->owner=i;s->kind=kind;s->ang=a;}
static void turn_to(u16*ang,int target,int rate){int d=(s16)((u16)(target<<8)-*ang);if(d>rate)d=rate;if(d<-rate)d=-rate;*ang+=d;}
// ---------------------------------------------------------------- infantry
static void inf_fire(int i){Unit*u=&U[i];if(u->cd>0||u->rl>0)return;Wstat w;
 if(u->isp){w=PW[u->wpn];if(u->mag[u->wpn]<=0){u->rl=w.reload;return;}u->mag[u->wpn]--;if(u->mag[u->wpn]==0)u->rl=w.reload;}
 else{const Wpn*b=&PRIM[mpEra][u->wpn];w.dmg=b->dmg;w.rate=b->rate+4;w.spd16=b->spd16;w.spread=b->spread;w.pellets=b->pellets;w.life=b->life;}
 u->cd=w.rate;
 for(int p=0;p<w.pellets;p++){int a=(u->tur>>8)+(w.spread?rr(w.spread*2+1)-w.spread:0)+(u->isp?0:rr(7)-3);fire_bullet(i,a,w.spd16*16,w.dmg,w.life,K_BUL,0);}
 if(u->isp||idist(dxw(u->x,U[0].x),dyw(u->y,U[0].y))<150){if(mpEra>=3)sfx_laser();else sfx_shot(0);}
 if(!(u->isp&&stealth))fx_add((u->x>>8)+(SIN(u->tur>>8)*11>>8),(u->y>>8)-(COS(u->tur>>8)*11>>8),FXK_FLASH);}
static void throw_nade(Unit*u,int i,int a,int sub){Shot*s=shot_new();if(!s)return;
 s->x=u->x;s->y=u->y;s->vx=SIN(a)*3;s->vy=-COS(a)*3;s->life=sub==1?30:44;s->team=u->team;s->owner=i;s->kind=K_GREN;s->sub=sub;sfx_whoosh();}
static void throw_gren(int i,int a){Unit*u=&U[i];if(u->gren<=0)return;u->gren--;throw_nade(u,i,a,u->isp?pLeth:0);}
static void use_tact(int i){Unit*u=&U[i];if(u->tact<=0)return;u->tact--;
 if(pTact==2){u->hp+=60;if(u->hp>u->maxhp)u->hp=u->maxhp;sfx_ok();feed("STIM: +60 HP",HB_GREEN);}else throw_nade(u,i,u->tur>>8,10+pTact);}
static void (*xdmg)(int k,int dmg,int owner);
static void det_explosive(Shot*s){int gx=s->x>>8,gy=s->y>>8;
 if(s->sub==10){fx_add(gx,gy,FXK_FLASH);for(int j=0;j<6;j++)fx_add(gx+rr(30)-15,gy+rr(30)-15,FXK_SPARK);sfx_shot(1);
  for(int k=0;k<NU;k++){Unit*e=&U[k];if(e->alive&&e->team!=s->team&&idist(dxw(e->x,s->x),dyw(e->y,s->y))<60)e->stun=150;}return;}
 if(s->sub==11){smoke_add(gx,gy);sfx_whoosh();return;}
 int n=s->sub==2?3:1;
 if(idist(gx-(U[0].x>>8),gy-(U[0].y>>8))<220)sfx_boom();
 for(int j=0;j<n;j++){int bx=gx+(n>1?(j-1)*16:0),by=gy+(n>1?((j&1)?10:-6):0);int R=n>1?24:(s->sub==1?30:34);
  fx_add(bx,by,FXK_BIG);if(j==0)fx_add(bx,by-4,FXK_SMOKE);
  for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==s->team)continue;int d=idist(wrapd((e->x>>8)-bx,mapW*16),wrapd((e->y>>8)-by,mapH*16));
   if(d<R){int dm=n>1?60-d:(d<14?100:80-d);if(e->isp&&pPerk==1)dm/=2;xdmg(k,dm,s->owner);}}}}
static void inf_update(int i){Unit*u=&U[i];if(u->cd>0)u->cd--;int sp=u->isp?pSpeed:250;
 if(u->rl>0){if(--u->rl==0&&u->isp)u->mag[u->wpn]=PW[u->wpn].mag;}
 if(u->isp){int dx=0,dy=0;if(keys&K_LEFT)dx--;if(keys&K_RIGHT)dx++;if(keys&K_UP)dy--;if(keys&K_DOWN)dy++;
  sp=sp*PW[u->wpn].move/100;
  if(dx||dy){int a=atan2i(dx,dy);if(!(keys&K_R))u->tur=u->ang=a<<8;else u->ang=a<<8;if(dx&&dy)sp=sp*181/256;move_unit(u,dx*sp,dy*sp,5);u->anim++;}
  if(pressed&K_L){u->wpn^=1;u->rl=0;if(u->mag[u->wpn]<=0)u->rl=PW[u->wpn].reload;feed(PW[u->wpn].n,HB_YEL);sfx_menu();}
  if(keys&K_A)inf_fire(i);if(pressed&K_B)throw_gren(i,u->tur>>8);if(pressed&K_SEL)use_tact(i);return;}
 if(u->stun>0){u->stun--;return;}
 if(--u->think<=0){u->think=8;int t=nearest(i,190,1);if(t>=0){u->tgt=t;u->mode=1;}else{u->tgt=nearest(i,4000,0);u->mode=0;}}
 if(u->wander>0){u->wander--;int a=steer(u,u->wang,12,5);move_unit(u,SIN(a)*sp>>8,-COS(a)*sp>>8,5);u->ang=u->tur=a<<8;u->anim++;return;}
 if(u->tgt<0||!U[u->tgt].alive){u->think=0;int gy=u->team?(mapH-4)*16:4*16;int a=steer(u,atan2i(0,gy-(u->y>>8)),12,5);move_unit(u,SIN(a)*sp>>8,-COS(a)*sp>>8,5);u->ang=u->tur=a<<8;u->anim++;return;}
 Unit*t=&U[u->tgt];int dx=dxw(t->x,u->x),dy=dyw(t->y,u->y),d=idist(dx,dy),aim=atan2i(dx,dy);
 if(u->mode&&d<180){turn_to(&u->tur,aim,10<<8);int mv;
  if(d>120)mv=aim;else if(d<50)mv=aim+128;else{if(rr(90)==0)u->wang^=1;mv=aim+(u->wang&1?64:-64);}
  int a=steer(u,mv&255,10,5);if(move_unit(u,SIN(a)*sp/2>>8,-COS(a)*sp/2>>8,5))u->anim++;
  if(iabs(angd(u->tur>>8,aim))<10)inf_fire(i);
  if(u->gren>0&&d<100&&d>40&&rr(160)==0)throw_gren(i,aim);
 }else{int a=steer(u,aim,14,5);if(move_unit(u,SIN(a)*sp>>8,-COS(a)*sp>>8,5))u->anim++;u->ang=u->tur=a<<8;
  if(u->gren>0&&d<90&&rr(200)==0)throw_gren(i,aim);}
 if((frame+i)%30==0){if(idist((u->x-u->lx)>>8,(u->y-u->ly)>>8)<4){u->wander=30+rr(30);u->wang=rr(256);}u->lx=u->x;u->ly=u->y;}}
// ---------------------------------------------------------------- tanks
static void tank_fire(int i){Unit*u=&U[i];const TankS*T=TV(u);if(u->cd>0)return;u->cd=T->reload+(u->isp?0:25);
 fire_bullet(i,u->tur>>8,T->sspd,T->dmg,90,K_SHELL,0);u->vel-=60;
 if(u->isp||idist(dxw(u->x,U[0].x),dyw(u->y,U[0].y))<200)sfx_shot(1);
 fx_add((u->x>>8)+(SIN(u->tur>>8)*18>>8),(u->y>>8)-(COS(u->tur>>8)*18>>8),FXK_FLASH);}
static void tank_coax(int i){Unit*u=&U[i];if(u->cd2>0)return;u->cd2=7;fire_bullet(i,(u->tur>>8)+rr(5)-2,1300,4,40,K_BUL,4);if(u->isp)sfx_shot(0);}
static void tank_drive(Unit*u,int throttle){ // throttle -256..256
 const TankS*T=TV(u);int tv=T->spd*throttle/256;if(u->vel<tv)u->vel+=16;if(u->vel>tv)u->vel-=16;if(iabs(u->vel-tv)<16)u->vel=tv;
 int a=u->ang>>8;int dx=SIN(a)*u->vel>>8,dy=-COS(a)*u->vel>>8;if(!move_unit(u,dx,dy,10))u->vel=u->vel*3/4;if(u->vel)u->anim++;}
static void tank_update(int i){Unit*u=&U[i];const TankS*T=TV(u);if(u->cd>0)u->cd--;if(u->cd2>0)u->cd2--;
 if(u->isp){int th=0;if(keys&K_UP)th=256;if(keys&K_DOWN)th=-150;int tr=0;if(keys&K_LEFT)tr=-T->turn;if(keys&K_RIGHT)tr=T->turn;
  u->ang+=tr;u->tur+=tr;if(keys&K_L)u->tur-=T->turn*2;if(keys&K_R)u->tur+=T->turn*2;if(pressed&K_SEL)u->tur=u->ang;
  tank_drive(u,th);if(keys&K_A)tank_fire(i);if(keys&K_B)tank_coax(i);return;}
 if(--u->think<=0){u->think=10;int t=nearest(i,280,1);if(t>=0){u->tgt=t;u->mode=1;}else{u->tgt=nearest(i,5000,0);u->mode=0;}}
 int th=0,want=u->ang>>8;
 if(u->wander>0){u->wander--;want=u->wang;th=u->wander>20?-200:200;}
 else if(u->tgt>=0&&U[u->tgt].alive){Unit*t=&U[u->tgt];int dx=dxw(t->x,u->x),dy=dyw(t->y,u->y),d=idist(dx,dy),aim=atan2i(dx,dy);
  turn_to(&u->tur,aim,T->turn*3/2);
  if(u->mode){if(iabs(angd(u->tur>>8,aim))<5)tank_fire(i);if(d<130&&iabs(angd(u->tur>>8,aim))<8)tank_coax(i);}
  if(u->mode&&d<170){th=d<90?-120:0;want=steer(u,aim,28,10);}else{want=steer(u,aim,30,10);th=256;}}
 else{u->think=0;}
 int df=angd(want,u->ang>>8);int tr=clampi(df<<8,-T->turn,T->turn);u->ang+=tr;u->tur+=tr;if(th>0&&iabs(df)>40)th=60;
 tank_drive(u,th);
 if((frame+i)%40==0){if(th!=0&&idist((u->x-u->lx)>>8,(u->y-u->ly)>>8)<4){u->wander=40;u->wang=rr(256);}u->lx=u->x;u->ly=u->y;}}
static void tank_separate(void){for(int a=0;a<NU;a++)for(int b=a+1;b<NU;b++){Unit*p=&U[a],*q=&U[b];if(!p->alive||!q->alive)continue;
 int dx=(q->x-p->x)>>8,dy=(q->y-p->y)>>8,d=idist(dx,dy);if(d>=22||d==0)continue;int push=(22-d)*128;
 int ux=dx*256/d,uy=dy*256/d;move_unit(q,ux*push>>8>>1,uy*push>>8>>1,10);move_unit(p,-(ux*push>>8>>1),-(uy*push>>8>>1),10);p->vel/=2;q->vel/=2;}}
// ---------------------------------------------------------------- air
static void air_fire(int i){Unit*u=&U[i];const AirS*A=AV(u);if(u->cd>0)return;u->cd=A->rate+(u->isp?0:3);u->anim^=1;
 fire_bullet(i,(u->ang>>8)+(u->isp?0:rr(5)-2),A->bspd,A->dmg,36,K_BUL,u->anim?6:-6);
 if(u->isp||idist(dxw(u->x,U[0].x),dyw(u->y,U[0].y))<140){if(mpEra>=3)sfx_laser();else sfx_shot(0);}}
static void air_missile(int i){Unit*u=&U[i];const AirS*A=AV(u);if(u->ammo<=0||u->cd2>0)return;u->ammo--;u->cd2=40;
 Shot*s=shot_new();if(!s)return;int a=u->ang>>8;s->x=u->x;s->y=u->y;s->ang=a<<8;s->kind=A->mturn?K_MISSILE:K_ROCKET;s->team=u->team;s->owner=i;s->dmg=A->mturn?65:50;s->life=A->mturn?170:80;
 int sp=u->vel+(A->mturn?250:500);s->vx=SIN(a)*sp>>8;s->vy=-COS(a)*sp>>8;s->dist=sp;s->tgt=-1;
 if(A->mturn){int best=-1,bd=330;for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==u->team)continue;int dx=dxw(e->x,u->x),dy=dyw(e->y,u->y),d=idist(dx,dy);
   if(d<bd&&iabs(angd(atan2i(dx,dy),a))<40){bd=d;best=k;}}s->tgt=best;}
 sfx_whoosh();if(u->isp)feed(s->tgt>=0?"FOX TWO - LOCKED":(A->mturn?"FOX TWO - NO LOCK":"ROCKETS AWAY"),HB_YEL);}
static void air_move(Unit*u,int target_spd){const AirS*A=AV(u);if(u->vel<target_spd)u->vel+=6;if(u->vel>target_spd)u->vel-=6;
 int a=u->ang>>8;int W=worldPx()<<8;u->x=(u->x+(SIN(a)*u->vel>>8))&(W-1);u->y=(u->y-(COS(a)*u->vel>>8))&(W-1);
 if(u->roll>0){u->roll--;int p=a+(u->wang?64:-64);u->x=(u->x+SIN(p)*1)&(W-1);u->y=(u->y-COS(p)*1)&(W-1);}if(u->rollcd>0)u->rollcd--;(void)A;}
static void air_update(int i){Unit*u=&U[i];const AirS*A=AV(u);if(u->cd>0)u->cd--;if(u->cd2>0)u->cd2--;
 if(u->isp){if(keys&K_LEFT)u->ang-=A->turn;if(keys&K_RIGHT)u->ang+=A->turn;int ts=A->spd;if(keys&K_UP)ts=ts*21/16;if(keys&K_DOWN)ts=ts*11/16;
  if((pressed&(K_L|K_R))&&u->rollcd==0){u->roll=28;u->rollcd=140;u->wang=(pressed&K_R)?1:0;sfx_whoosh();}
  air_move(u,ts);if(keys&K_A)air_fire(i);if(pressed&K_B)air_missile(i);return;}
 if(--u->think<=0){u->think=6;u->tgt=nearest(i,5000,0);
  if(u->evade<=0)for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==u->team)continue;int dx=dxw(u->x,e->x),dy=dyw(u->y,e->y);
   if(idist(dx,dy)<110&&iabs(angd(atan2i(dx,dy),e->ang>>8))<18&&rr(3)==0){u->evade=40+rr(30);u->wang=rr(2);if(u->rollcd==0&&rr(3)==0){u->roll=28;u->rollcd=200;}break;}}}
 int ts=A->spd;
 if(u->evade>0){u->evade--;u->ang+=u->wang?A->turn:-A->turn;ts=ts*20/16;}
 else if(u->tgt>=0&&U[u->tgt].alive){Unit*t=&U[u->tgt];int dx=dxw(t->x,u->x),dy=dyw(t->y,u->y),d=idist(dx,dy);
  int tt=d*256/(A->bspd+u->vel);int ta=t->ang>>8;int lx=dx+(SIN(ta)*t->vel>>8)*tt/256,ly=dy-(COS(ta)*t->vel>>8)*tt/256;int aim=atan2i(lx,ly);
  turn_to(&u->ang,aim,A->turn*7/8);if(d<80)ts=ts*13/16;
  if(iabs(angd(u->ang>>8,aim))<7&&d<170)air_fire(i);
  if(u->ammo>0&&d<300&&d>70&&iabs(angd(u->ang>>8,aim))<24&&rr(110)==0)air_missile(i);}
 air_move(u,ts);}
// ---------------------------------------------------------------- shots
static void shots_update(void){int W=worldPx();
 for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0)continue;s->life--;
  if(s->kind==K_GREN){s->x+=s->vx;s->y+=s->vy;s->vx=s->vx*15/16;s->vy=s->vy*15/16;int gx=s->x>>8,gy=s->y>>8;
   if(s->sub!=1&&(flag_at(gx,gy)&TF_SOLID)){s->vx=-s->vx/2;s->vy=-s->vy/2;s->x+=s->vx*2;s->y+=s->vy*2;}
   if(s->sub==1&&(flag_at(gx+(s->vx>>6),gy+(s->vy>>6))&TF_SOLID)){s->vx=s->vy=0;}
   if(s->life<=0)det_explosive(s);
   continue;}
  if(s->kind==K_MISSILE&&s->tgt>=0&&U[s->tgt].alive){Unit*t=&U[s->tgt];int aim=atan2i(dxw(t->x,s->x),dyw(t->y,s->y));turn_to(&s->ang,aim,AV(&U[s->owner])->mturn);
   int a=s->ang>>8;s->vx=SIN(a)*s->dist>>8;s->vy=-COS(a)*s->dist>>8;if(s->dist<900)s->dist+=12;}
  if(s->kind==K_MISSILE||s->kind==K_ROCKET){if((s->life&3)==0)fx_add(s->x>>8,s->y>>8,FXK_SMOKE);}
  s->x+=s->vx;s->y+=s->vy;
  if(mapWrap){s->x&=(W<<8)-1;s->y&=(W<<8)-1;}
  int bx=s->x>>8,by=s->y>>8;
  if(!mapWrap){if(bx<0||by<0||bx>=W||by>=mapH*16){s->life=0;continue;}
   if(flag_at(bx,by)&TF_SHOT){int fresh=(s->kind==K_SHELL)?(s->life>84):0;
    if(!fresh&&!(s->kind!=K_SHELL&&idist(bx-(U[s->owner].x>>8),by-(U[s->owner].y>>8))<14)){s->life=0;if(s->kind==K_SHELL){fx_add(bx,by,FXK_SMALL);
      for(int k=0;k<NU;k++){Unit*e=&U[k];if(e->alive&&e->team!=s->team&&idist((e->x>>8)-bx,(e->y>>8)-by)<20)damage(k,(e->isp&&pPerk==1)?s->dmg/6:s->dmg/3,s->owner);}}
     else fx_add(bx,by,FXK_SPARK);continue;}}}
  int r=mpMode==MM_INF?7:mpMode==MM_TANK?12:(s->kind==K_BUL?9:12);
  for(int k=0;k<NU;k++){Unit*e=&U[k];if(!e->alive||e->team==s->team)continue;int dx=wrapd(bx-(e->x>>8),W),dy=wrapd(by-(e->y>>8),mapH*16);
   if(iabs(dx)<r&&iabs(dy)<r){if(e->roll>0)continue;s->life=0;damage(k,s->dmg,s->owner);
    fx_add(bx,by,(s->kind==K_SHELL||s->kind==K_MISSILE||s->kind==K_ROCKET)?FXK_SMALL:FXK_SPARK);
    if(s->kind==K_MISSILE||s->kind==K_ROCKET||s->kind==K_SHELL){if(e->isp||idist(dx,dy)<300)sfx_boom();}break;}}
  if(s->life==0&&s->kind==K_ROCKET)fx_add(bx,by,FXK_SMALL);}}
// ---------------------------------------------------------------- rendering
static void draw_shots(void){int bt=bulletTile();
 for(int i=0;i<NS;i++){Shot*s=&S[i];if(s->life<=0)continue;int x=scrx(s->x),y=scry(s->y);
  switch(s->kind){case K_GREN:spr(x-4,y-4,OT_FX+FX_GRENADE,0,0,3,1);break;
   case K_SHELL:spr(x-4,y-4,OT_FX+(mpEra>=3?FX_PLASMA:FX_SHELL),0,0,3,1);break;
   case K_MISSILE:spr_aff(x,y,OT_FX+FX_MISSILE,0,3,s->ang>>8,0x100,1,0);break;
   case K_ROCKET:spr_aff(x,y,OT_FX+FX_ROCKET,0,3,(atan2i(s->vx,s->vy)),0x100,1,0);break;
   default:spr(x-4,y-4,OT_FX+bt,0,0,3,1);}}}
static void draw_radar(void){int W=worldPx(),Hp=mapH*16;hpanel(25,14,5,5);
 for(int k=NU-1;k>=0;k--){Unit*u=&U[k];if(!u->alive)continue;int x=200+1+(u->x>>8)*37/W,y=112+1+(u->y>>8)*37/Hp;
  spr(x,y,OT_FX+FX_RADAR+(k==0?2:u->team?1:0),0,0,3,0);}}
static void draw_units(void){
 for(int k=0;k<NU;k++){Unit*u=&U[k];if(!u->alive)continue;if(u->inv>0&&(frame&4))continue;int x=scrx(u->x),y=scry(u->y);int pal=k==0?2:u->team?1:0;
  if(u->hurt>0)u->hurt--;
  if(mpMode==MM_INF)spr_aff(x,y,OT_ERA+ES_SOLDIER+((u->anim>>3)&1)*4,1,pal,u->tur>>8,0x100,1,0);
  else if(mpMode==MM_TANK){spr_aff(x,y,OT_ERA+ES_TUR,2,pal,u->tur>>8,0x100,1,0);spr_aff(x,y,OT_ERA+ES_HULL,2,pal,u->ang>>8,0x100,1,0);}
  else{int sx=0x100;if(u->roll>0){int ph=(28-u->roll)*256/28;sx=iabs(COS(ph/2))+24;if(sx>256)sx=256;}
   spr_aff(x,y,OT_ERA+(mpEra==0&&u->veh==2?ES_PLANE2:ES_PLANE),2,pal,u->ang>>8,sx,1,0);}}
 if(mpMode==MM_AIR&&mpEra!=4)for(int k=0;k<NU;k++){Unit*u=&U[k];if(!u->alive||(u->inv>0&&(frame&4)))continue;spr_aff(scrx(u->x)+10,scry(u->y)+16,OT_ERA+(mpEra==0&&u->veh==2?ES_PLANE2:ES_PLANE),2,4,u->ang>>8,0x100,2,1);}}
static void inf_hud(Unit*P){htxt(7,19,PW[P->wpn].n,HB_WHITE);
 if(P->rl>0)htxt(20,19,"RLD",(frame&8)?HB_RED:HB_YEL);else htxt(20,19,itoa(P->mag[P->wpn]),P->mag[P->wpn]*4<PW[P->wpn].mag?HB_RED:HB_WHITE);
 hput(24,19,HT_ICON,15);htxt(25,19,itoa(P->gren),HB_WHITE);htxt(27,19,pTact==0?"F":pTact==1?"S":"+",HB_GREEN);htxt(28,19,itoa(P->tact),HB_WHITE);}
static void draw_hud(void){Unit*P=&U[0];
 htxt(0,0,TEAMN[mpEra][0],HB_BLUE);htxt(slen(TEAMN[mpEra][0])+1,0,itoa(score[0]),HB_WHITE);
 const char*en=TEAMN[mpEra][1];const char*es=itoa(score[1]);htxt(30-slen(en),0,en,HB_RED);htxt(29-slen(en)-slen(es),0,es,HB_WHITE);
 hctxt(0,mmss(mpTimer),mpTimer<60*30?HB_RED:HB_WHITE,0);
 hput(0,19,HT_ICON+4,15);hbar(1,19,5,P->hp,P->maxhp);
 if(mpMode==MM_INF)inf_hud(P);
 else if(mpMode==MM_TANK){htxt(7,19,TV(P)->n,HB_WHITE);hbar(20,19,3,TV(P)->reload-P->cd,TV(P)->reload);if(P->cd==0)htxt(23,19,"RDY",HB_GREEN);}
 else{htxt(7,19,AV(P)->n,HB_WHITE);hput(7+slen(AV(P)->n)+1,19,HT_ICON+1,15);htxt(7+slen(AV(P)->n)+2,19,itoa(P->ammo),HB_WHITE);}
 if(!P->alive)hctxt(8,cat("RESPAWN IN ",itoa(P->resp/60+1)),HB_YEL,0);
 if(msgT>0){msgT--;hctxt(2,msgS,msgB,0);}
 if(keys&K_SEL&&mpMode==MM_AIR){hpanel(4,4,22,8);hctxt(5,"SCOREBOARD",HB_YEL,1);
  hptxt(6,7,cat(TEAMN[mpEra][0],"  "),HB_BLUE);hptxt(16,7,itoa(score[0]),HB_WHITE);hptxt(6,8,TEAMN[mpEra][1],HB_RED);hptxt(16,8,itoa(score[1]),HB_WHITE);
  hptxt(6,10,cat("YOU  K ",itoa(P->kills)),HB_WHITE);hptxt(16,10,cat("D ",itoa(P->deaths)),HB_WHITE);}
 draw_radar();}
static void mp_result(void){Unit*P=&U[0];int win=score[0]>score[1]?1:score[0]<score[1]?-1:0;
 if(win>0)music_start(fanfare,FANFARE_LEN,10);
 BEAT->screen=SCR_RESULT;
 for(int i=0;;i++){hud_clear();hpanel(3,3,24,13);hctxt(4,win>0?"VICTORY":win<0?"DEFEAT":"DRAW",win>0?HB_GREEN:win<0?HB_RED:HB_YEL,1);
  hctxt(6,cat(ERA_NAME[mpEra],mpMode==MM_INF?" - INFANTRY":mpMode==MM_TANK?" - TANKS":" - DOGFIGHT"),HB_GRAY,1);
  hptxt(6,8,TEAMN[mpEra][0],HB_BLUE);hptxt(19,8,itoa(score[0]),HB_WHITE);hptxt(6,9,TEAMN[mpEra][1],HB_RED);hptxt(19,9,itoa(score[1]),HB_WHITE);
  hptxt(6,11,"YOUR KILLS",HB_WHITE);hptxt(19,11,itoa(P->kills),HB_WHITE);hptxt(6,12,"YOUR DEATHS",HB_WHITE);hptxt(19,12,itoa(P->deaths),HB_WHITE);
  if(i>40)hctxt(14,"A: CONTINUE",HB_YEL,1);
  oam_begin();present(mpMode==MM_AIR&&mpEra!=4,0,0);if(i>40&&(pressed&(K_A|K_START)))break;}
 music_stop();}
static void mp_run(int mode,int era){
 mpMode=mode;mpEra=era;BEAT->mode=mode;BEAT->era=era;
 int clouds=0;
 if(mode==MM_AIR){gen_sky();clouds=mpEra!=4;const u32*t=era==0?sky_tiles_0:era==1?sky_tiles_1:era==2?sky_tiles_2:era==3?sky_tiles_3:sky_tiles_4;
  const u16*p=era==0?sky_pal_0:era==1?sky_pal_1:era==2?sky_pal_2:era==3?sky_pal_3:sky_pal_4;setup_mode0(t,32*4*8,p,era,clouds);}
 else{gen_arena(mode==MM_TANK?56:40,mode==MM_TANK?56:40,mode==MM_TANK);const u32*t=era==0?arena_tiles_0:era==1?arena_tiles_1:era==2?arena_tiles_2:era==3?arena_tiles_3:arena_tiles_4;
  const u16*p=era==0?arena_pal_0:era==1?arena_pal_1:era==2?arena_pal_2:era==3?arena_pal_3:arena_pal_4;setup_mode0(t,AM_N*4*8,p,era,0);}
 memset(U,0,sizeof U);memset(S,0,sizeof S);memset(FXs,0,sizeof FXs);score[0]=score[1]=0;mpTimer=60*60*5;msgT=0;
 rs^=frame*2654435761u;
 {Loadout*l=&LO[era];PW[0]=wcalc(era,l,0);PW[1]=wcalc(era,l,1);pSpeed=290;pPerk=l->perk;pLeth=l->leth;pTact=l->tact;stealth=l->muz==1;}
 if(mode!=MM_INF){pPerk=-1;stealth=0;}
 xdmg=damage;memset(smT,0,sizeof smT);
 for(int i=0;i<NU;i++){U[i].team=i<6?0:1;U[i].isp=i==0;U[i].think=i;
  int vs=mode==MM_TANK?tankSel[era]:airSel[era];int nv=mode==MM_AIR?NAIR[era]:2;U[i].veh=i<6?vs:(vs+1+(i&1)*(nv>2))%nv;U[i].wpn=i==0?0:rr(NPRIM[era]);spawn(&U[i],i);}
 feed(mode==MM_INF?"TEAM DEATHMATCH - FIRST TO 30":mode==MM_TANK?"TANK BATTLE - FIRST TO 30":"DOGFIGHT - FIRST TO 30",HB_YEL);
 int scr=mode==MM_INF?SCR_INF:mode==MM_TANK?SCR_TANK:SCR_AIR;int over=0;
 for(;;){
  BEAT->screen=scr;BEAT->hp=U[0].hp;BEAT->scoreA=score[0];BEAT->scoreB=score[1];BEAT->timer=mpTimer;BEAT->kills=U[0].kills;BEAT->deaths=U[0].deaths;BEAT->x=U[0].x>>8;BEAT->y=U[0].y>>8;
  if(pressed&K_START){if(pause_menu())return;}
  if((keys&(K_L|K_R))==(K_L|K_R)&&(pressed&K_SEL)&&!over)mpTimer=1; // debug: end match now
  if(over){if(--over==0){mp_result();return;}}
  else{mpTimer--;if(score[0]>=30||score[1]>=30||mpTimer<=0){over=90;feed(score[0]>score[1]?"YOUR TEAM WINS!":score[0]<score[1]?"ENEMY TEAM WINS":"DRAW",HB_YEL);}}
  for(int i=0;i<NU;i++){Unit*u=&U[i];
   if(!u->alive){if(--u->resp<=0&&!over)spawn(u,i);continue;}
   if(u->inv>0)u->inv--;
   if(mode==MM_INF)inf_update(i);else if(u->stun>0)u->stun--;else if(mode==MM_TANK)tank_update(i);else air_update(i);}
  if(mode==MM_TANK)tank_separate();
  shots_update();
  // camera
  Unit*P=&U[0];int px=P->x>>8,py=P->y>>8;
  if(mapWrap){camX=(px-120)&(worldPx()-1);camY=(py-80)&(mapH*16-1);}
  else{int tx=clampi(px-120,0,worldPx()-240),ty=clampi(py-80,0,mapH*16-160);camX+=(tx-camX)/3;camY+=(ty-camY)/3;if(iabs(tx-camX)<3)camX=tx;if(iabs(ty-camY)<3)camY=ty;}
  oam_begin();hud_clear();
  draw_hud();draw_shots();fx_update_draw();smoke_tick_draw();draw_units();
  draw_layer(map,bgw,camX,camY,0,mapWrap);
  int ccx=0,ccy=0;if(clouds){ccx=(camX+camX/4+frame/3)&1023;ccy=(camY+camY/4)&1023;draw_layer(cmap,bgc,ccx,ccy,0,1);}
  present(clouds,ccx,ccy);}
}
