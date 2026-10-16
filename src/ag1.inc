// ---------------------------------------------------------------- 1) IRON DOME: intercept rockets + drones over military bases
typedef struct{s32 x,y,vx,vy,sx,sy;u8 on,k,tg;}DmT;typedef struct{s32 x,y,vx,vy;s16 tx,ty;u8 on;}DmI;typedef struct{s16 x,y,r,t;}DmB;
static DmT dmT[14] EWRAM;static DmI dmI[8] EWRAM;static DmB dmB[10] EWRAM;static u8 dmBase[4],dmAmmo[2];static s16 dmCx,dmCy,dmLeft,dmSpawnT,dmCool;
static const u8 DM_BX[4]={30,95,145,210};static const u8 DM_GX[2]={60,180};
static void dome_wave(void){agLvl++;dmLeft=6+agLvl*3;dmSpawnT=60;dmAmmo[0]=dmAmmo[1]=10+agLvl;}
static void dome_init(void){memset(dmT,0,sizeof dmT);memset(dmI,0,sizeof dmI);memset(dmB,0,sizeof dmB);for(int i=0;i<4;i++)dmBase[i]=1;dmCx=120;dmCy=70;dmCool=0;agLvl=0;dome_wave();}
static void dome_spawn(void){for(int i=0;i<14;i++)if(!dmT[i].on){DmT*t=&dmT[i];t->on=1;t->tg=0;t->k=(agLvl>1&&rr(4)==0);int b;do b=rr(4);while(!dmBase[b]&&(dmBase[0]|dmBase[1]|dmBase[2]|dmBase[3]));
  int sp=48+agLvl*10;if(sp>230)sp=230;
  if(!t->k){t->x=(10+rr(220))<<8;t->y=12<<8;s32 dx=(DM_BX[b]<<8)-t->x,dy=(150<<8)-t->y;int n=(140*256)/sp;t->vx=dx/n;t->vy=dy/n;}
  else{int l=rr(2);t->x=(l?250:-10)<<8;t->y=(24+rr(30))<<8;t->vx=(l?-1:1)*(sp*3/4);t->vy=0;}
  t->sx=t->x;t->sy=t->y;return;}}
static void dome_blast(int x,int y){for(int i=0;i<10;i++)if(!dmB[i].t){dmB[i].x=x;dmB[i].y=y;dmB[i].r=0;dmB[i].t=40;sfx_boom();return;}}
static int dome_step(u16 h,u16 p){
 if(h&K_LEFT)dmCx-=3;if(h&K_RIGHT)dmCx+=3;if(h&K_UP)dmCy-=3;if(h&K_DOWN)dmCy+=3;dmCx=clampi(dmCx,4,236);dmCy=clampi(dmCy,14,130);if(dmCool)dmCool--;
 if((p&K_A)&&!dmCool){int g=iabs(dmCx-DM_GX[0])<iabs(dmCx-DM_GX[1])?0:1;if(!dmAmmo[g])g^=1;if(dmAmmo[g])for(int i=0;i<8;i++)if(!dmI[i].on){DmI*m=&dmI[i];m->on=1;m->x=DM_GX[g]<<8;m->y=146<<8;m->tx=dmCx;m->ty=dmCy;
    s32 dx=(dmCx<<8)-m->x,dy=(dmCy<<8)-m->y;int d=0;{s32 ax=iabs(dx>>8),ay=iabs(dy>>8);d=ax>ay?ax+ay/2:ay+ax/2;}if(d<1)d=1;m->vx=dx*5/d;m->vy=dy*5/d;dmAmmo[g]--;dmCool=8;sfx_shot(0);break;}}
 if(dmLeft>0&&--dmSpawnT<=0){dome_spawn();dmLeft--;dmSpawnT=clampi(70-agLvl*6,8,70)+rr(clampi(30-agLvl,6,30));}
 for(int i=0;i<8;i++){DmI*m=&dmI[i];if(!m->on)continue;m->x+=m->vx;m->y+=m->vy;if(iabs((m->x>>8)-m->tx)<4&&iabs((m->y>>8)-m->ty)<4){m->on=0;dome_blast(m->tx,m->ty);}}
 for(int i=0;i<10;i++){DmB*b=&dmB[i];if(!b->t)continue;b->t--;b->r=b->t>20?(40-b->t)*16/20:b->t*16/20;
  for(int k=0;k<14;k++){DmT*t=&dmT[k];if(!t->on)continue;int dx=(t->x>>8)-b->x,dy=(t->y>>8)-b->y;if(dx*dx+dy*dy<=b->r*b->r){t->on=0;agScore+=t->k?50:25;}}}
 int alive=0;for(int k=0;k<14;k++){DmT*t=&dmT[k];if(!t->on)continue;alive++;
  if(t->k&&t->vy==0){t->x+=t->vx;int b=-1;for(int j=0;j<4;j++)if(dmBase[j]&&iabs((t->x>>8)-DM_BX[j])<3)b=j;if(b>=0||(t->x>>8)<-20||(t->x>>8)>260){if(b>=0){t->vx=0;t->vy=200;}else t->on=0;}}
  else{t->x+=t->vx;t->y+=t->vy;}
  if((t->y>>8)>=148){t->on=0;for(int j=0;j<4;j++)if(iabs((t->x>>8)-DM_BX[j])<14&&dmBase[j]){dmBase[j]=0;sfx_boom();}dome_blast(t->x>>8,148);}}
 int bases=dmBase[0]+dmBase[1]+dmBase[2]+dmBase[3];if(!bases)return 1;
 if(!dmLeft&&!alive){int any=0;for(int i=0;i<10;i++)any|=dmB[i].t;if(!any){agScore+=bases*100+(dmAmmo[0]+dmAmmo[1])*5;dome_wave();}}
 agLives=bases;return 0;}
static void dome_draw(void){for(int y=10;y<150;y+=10)a_rect(0,y,240,10,RGB(4,6+y/12,20+y/8));a_rect(0,150,240,10,RGB(50,60,40));
 for(int j=0;j<4;j++){int x=DM_BX[j];if(dmBase[j]){a_rect(x-10,143,20,7,RGB(110,120,90));a_rect(x-6,139,12,4,RGB(90,100,74));a_rect(x-1,134,2,5,RGB(200,200,200));a_rect(x,133,5,3,RGB(60,140,240));}else a_rect(x-10,147,20,3,RGB(40,30,30));}
 for(int g=0;g<2;g++){int x=DM_GX[g];a_rect(x-7,142,14,8,RGB(70,80,70));for(int k=0;k<4;k++)a_rect(x-6+k*3,136,2,6,k<dmAmmo[g]*4/14+1&&dmAmmo[g]?RGB(230,230,230):RGB(60,60,60));m3_text(x-6,152,itoa(dmAmmo[g]),C_WHITE,1);}
 for(int k=0;k<14;k++){DmT*t=&dmT[k];if(!t->on)continue;int x=t->x>>8,y=t->y>>8;
  if(!t->k){a_line(t->sx>>8,t->sy>>8,x,y,RGB(120,40,30));a_rect(x-1,y-1,3,3,RGB(255,90,60));}else{a_rect(x-4,y,9,2,RGB(200,200,210));a_rect(x-1,y-2,3,5,RGB(160,160,170));}}
 for(int i=0;i<8;i++)if(dmI[i].on){int x=dmI[i].x>>8,y=dmI[i].y>>8;a_rect(x-1,y-1,2,3,RGB(255,255,255));a_rect(x,y+2,1,3,RGB(120,200,255));}
 for(int i=0;i<10;i++)if(dmB[i].t){a_disc(dmB[i].x,dmB[i].y,dmB[i].r,(dmB[i].t&4)?RGB(255,220,120):RGB(255,140,40));}
 a_rect(dmCx-5,dmCy,4,1,C_GREEN);a_rect(dmCx+2,dmCy,4,1,C_GREEN);a_rect(dmCx,dmCy-5,1,4,C_GREEN);a_rect(dmCx,dmCy+2,1,4,C_GREEN);
 if(agT<90)m3_ctext(70,cat("WAVE ",itoa(agLvl)),C_YEL,1);}
static void dome_ai(void){static int tgt=-1;int best=-1,by=-1;for(int k=0;k<14;k++){DmT*t=&dmT[k];if(t->on&&!t->tg&&(t->y>>8)>40&&(t->y>>8)>by){by=t->y>>8;best=k;}}
 if(best<0){for(int k=0;k<14;k++)if(dmT[k].on&&!dmT[k].tg&&(dmT[k].y>>8)>by){by=dmT[k].y>>8;best=k;}}
 if(best<0)return;DmT*t=&dmT[best];int fl=24;int px=(t->x+t->vx*fl)>>8,py=(t->y+t->vy*fl)>>8;py=clampi(py,14,130);
 if(px<dmCx-2)aiH|=K_LEFT;if(px>dmCx+2)aiH|=K_RIGHT;if(py<dmCy-2)aiH|=K_UP;if(py>dmCy+2)aiH|=K_DOWN;
 if(iabs(px-dmCx)<5&&iabs(py-dmCy)<5&&!dmCool){aiP|=K_A;t->tg=1;}(void)tgt;}
// ---------------------------------------------------------------- 2) DEBRIS SWEEPER: Dragon capsule vs space junk (big pieces split)
typedef struct{s32 x,y,vx,vy;u8 on,sz,sh;}SwD;typedef struct{s32 x,y,vx,vy;u8 life;}SwB;
static SwD swD[28] EWRAM;static SwB swB[6] EWRAM;static s32 swX,swY,swVx,swVy;static int swA,swInv,swCool;
static const u8 SW_R[4]={0,4,7,12};
static void sw_wave(void){agLvl++;for(int i=0;i<3+agLvl&&i<10;i++)for(int k=0;k<28;k++)if(!swD[k].on){SwD*d=&swD[k];d->on=1;d->sz=3;d->sh=rr(256);int e=rr(4);d->x=(e==0?0:e==1?239:rr(240))<<8;d->y=(e==2?12:e==3?159:12+rr(140))<<8;
  int a=rr(256),s=60+agLvl*10;d->vx=SIN(a)*s>>8;d->vy=-COS(a)*s>>8;break;}}
static void sw_init(void){memset(swD,0,sizeof swD);memset(swB,0,sizeof swB);swX=120<<8;swY=85<<8;swVx=swVy=0;swA=0;swInv=90;swCool=0;agLvl=0;sw_wave();}
static void sw_wrap(s32*x,s32*y){if(*x<0)*x+=240<<8;if(*x>=240<<8)*x-=240<<8;if(*y<(10<<8))*y+=150<<8;if(*y>=160<<8)*y-=150<<8;}
static int sw_step(u16 h,u16 p){if(h&K_LEFT)swA-=4;if(h&K_RIGHT)swA+=4;swA&=255;
 if(h&K_UP){swVx+=SIN(swA)/8;swVy-=COS(swA)/8;}swVx=swVx*63/64;swVy=swVy*63/64;swX+=swVx;swY+=swVy;sw_wrap(&swX,&swY);if(swInv)swInv--;if(swCool)swCool--;
 if((p&K_A)||((h&K_A)&&!swCool)){if(!swCool)for(int i=0;i<6;i++)if(!swB[i].life){SwB*b=&swB[i];b->x=swX+SIN(swA)*6;b->y=swY-COS(swA)*6;b->vx=SIN(swA)*4+swVx;b->vy=-COS(swA)*4+swVy;b->life=40;swCool=10;sfx_shot(0);break;}}
 for(int i=0;i<6;i++){SwB*b=&swB[i];if(!b->life)continue;b->life--;b->x+=b->vx;b->y+=b->vy;sw_wrap(&b->x,&b->y);
  for(int k=0;k<28;k++){SwD*d=&swD[k];if(!d->on)continue;int dx=(b->x-d->x)>>8,dy=(b->y-d->y)>>8;int r=SW_R[d->sz]+1;if(dx*dx+dy*dy<=r*r){b->life=0;d->on=0;agScore+=d->sz==3?20:d->sz==2?50:100;sfx_boom();
    if(d->sz>1)for(int s=0;s<2;s++)for(int j=0;j<28;j++)if(!swD[j].on){SwD*n=&swD[j];*n=*d;n->on=1;n->sz=d->sz-1;int a=rr(256),sp=80+agLvl*10+(3-n->sz)*30;n->vx=SIN(a)*sp>>8;n->vy=-COS(a)*sp>>8;n->sh=rr(256);break;}break;}}}
 int any=0;for(int k=0;k<28;k++){SwD*d=&swD[k];if(!d->on)continue;any=1;d->x+=d->vx;d->y+=d->vy;sw_wrap(&d->x,&d->y);
  if(!swInv){int dx=(swX-d->x)>>8,dy=(swY-d->y)>>8;int r=SW_R[d->sz]+4;if(dx*dx+dy*dy<r*r){agLives--;sfx_boom();swInv=120;swX=120<<8;swY=85<<8;swVx=swVy=0;if(agLives<=0)return 1;}}}
 if(!any){agScore+=200;sw_wave();}return 0;}
static void sw_draw(void){a_clear(RGB(2,2,8));for(int i=0;i<50;i++)m3_px((i*97+13)%240,12+(i*53)%146,i%5?RGB(90,90,110):RGB(220,220,240));
 for(int k=0;k<28;k++){SwD*d=&swD[k];if(!d->on)continue;int x=d->x>>8,y=d->y>>8,r=SW_R[d->sz];u16 c=d->sz==3?RGB(150,150,160):d->sz==2?RGB(170,160,140):RGB(200,190,170);
  int px=0,py=0;for(int a=0;a<=256;a+=32){int rr2=r-((d->sh+a*7)%3);int qx=x+(SIN(a+d->sh)*rr2>>8),qy=y-(COS(a+d->sh)*rr2>>8);if(a)a_line(px,py,qx,qy,c);px=qx;py=qy;}a_rect(x-1,y-1,2,2,RGB(90,90,100));}
 for(int i=0;i<6;i++)if(swB[i].life)a_rect((swB[i].x>>8)-1,(swB[i].y>>8)-1,2,2,RGB(120,255,160));
 if(!(swInv&4)){int x=swX>>8,y=swY>>8;int nx=x+(SIN(swA)*7>>8),ny=y-(COS(swA)*7>>8);int lx=x+(SIN(swA+90)*6>>8),ly=y-(COS(swA+90)*6>>8),rx=x+(SIN(swA-90)*6>>8),ry=y-(COS(swA-90)*6>>8);
  a_line(nx,ny,lx,ly,RGB(240,240,240));a_line(nx,ny,rx,ry,RGB(240,240,240));a_line(lx,ly,rx,ry,RGB(40,40,40));a_disc(x,y,2,RGB(230,230,230));m3_px(nx,ny,RGB(80,160,240));}
 if(agT<90)m3_ctext(70,cat("SECTOR ",itoa(agLvl)),C_YEL,1);}
static void sw_ai(void){int best=-1;s32 bd=0x7FFFFFFF;for(int k=0;k<28;k++)if(swD[k].on){s32 dx=(swD[k].x-swX)>>8,dy=(swD[k].y-swY)>>8,d=dx*dx+dy*dy;if(d<bd){bd=d;best=k;}}
 if(best<0)return;int dx=((swD[best].x+swD[best].vx*12)-swX)>>8,dy=((swD[best].y+swD[best].vy*12)-swY)>>8;int a=a_ang(dx,dy);int df=a_adiff(a,swA);
 if(df>3)aiH|=K_RIGHT;else if(df<-3)aiH|=K_LEFT;if(iabs(df)<14)aiH|=K_A;if(bd>70*70&&(agT&7)<2)aiH|=K_UP;
 if(bd<26*26){int ea=a_adiff(a+128,swA);if(iabs(ea)<64)aiH|=K_UP;}}
// ---------------------------------------------------------------- 3) KC CROSSING: roads, a river with boats and a minefield to the doghouse
#define KX_C 16
typedef struct{s8 dir;u8 spd,len,per;u8 kind;}KxL; // kind 0 safe 1 road 2 river 3 mines 4 goal
static const KxL KXL[9]={{0,0,0,0,4},{0,0,0,0,3},{1,3,3,96,2},{-1,4,4,112,2},{0,0,0,0,0},{-1,6,2,80,1},{1,4,3,104,1},{-1,8,2,72,1},{0,0,0,0,0}};
static s32 kxOff[9] EWRAM,kxAcc;static s16 kxX,kxRow,kxHop,kxBest,kxTime;static u8 kxHome[5],kxMine[15];
static const u8 KX_SLOT[5]={1,4,7,10,13};
static int kx_speed(int r){return KXL[r].spd*(8+agLvl)/8;}
static void kx_reset(void){kxX=7*KX_C;kxRow=8;kxHop=0;kxBest=8;kxTime=60*35;}
static void kx_mines(void){for(int i=0;i<15;i++)kxMine[i]=0;for(int n=0;n<3+agLvl&&n<8;n++){int c=rr(15);int bad=0;for(int s=0;s<5;s++)if(KX_SLOT[s]==c)bad=1;if(!bad)kxMine[c]=1;}}
static void kx_init(void){for(int r=0;r<9;r++)kxOff[r]=rr(200)<<8;for(int i=0;i<5;i++)kxHome[i]=0;agLvl=0;kx_mines();kx_reset();}
static int kx_objat(int r,int x,s32 off){const KxL*L=&KXL[r];int per=L->per,w=L->len*KX_C;int p=((x-(off>>8))%per+per*8)%per;return p<w;} // 1 if an object covers screen x
static int kx_safe(int r,int x,s32 off){const KxL*L=&KXL[r];int c=(x+8)/KX_C;
 switch(L->kind){case 1:return !kx_objat(r,x+2,off)&&!kx_objat(r,x+13,off);case 2:return kx_objat(r,x+8,off)&&x>-8&&x<232;case 3:return c<0||c>14||!kxMine[c];default:return 1;}}
static void kx_die(void){agLives--;sfx_boom();kx_reset();}
static int kx_step(u16 h,u16 p){(void)h;for(int r=0;r<9;r++){kxOff[r]+=KXL[r].dir*kx_speed(r)*32;if(KXL[r].per){s32 P=KXL[r].per<<8;while(kxOff[r]<0)kxOff[r]+=P;while(kxOff[r]>=P)kxOff[r]-=P;}}
 if(kxHop)kxHop--;
 if(!kxHop&&(p&(K_UP|K_DOWN|K_LEFT|K_RIGHT))){int nr=kxRow,nx=kxX;if(p&K_UP)nr--;else if(p&K_DOWN)nr++;else if(p&K_LEFT)nx-=KX_C;else nx+=KX_C;
  if(KXL[kxRow].kind==2&&nr!=kxRow)nx=((nx+8)/KX_C)*KX_C;
  if(nr>=0&&nr<=8&&nx>=0&&nx<=14*KX_C){if(nr==0){int c=(nx+8)/KX_C,s=-1;for(int i=0;i<5;i++)if(KX_SLOT[i]==c)s=i;if(s>=0&&!kxHome[s]){kxHome[s]=1;agScore+=200+kxTime/30;sfx_ok();kx_reset();
      int all=1;for(int i=0;i<5;i++)all&=kxHome[i];if(all){agLvl++;agScore+=500;for(int i=0;i<5;i++)kxHome[i]=0;kx_mines();}return 0;}}
   else{kxRow=nr;kxX=nx;kxHop=4;sfx_menu();if(kxRow<kxBest){kxBest=kxRow;agScore+=10;}}}}
 if(KXL[kxRow].kind==2){kxAcc+=KXL[kxRow].dir*kx_speed(kxRow)*32;kxX+=kxAcc>>8;kxAcc-=(kxAcc>>8)<<8;if(kxX<-8||kxX>232){kx_die();if(agLives<=0)return 1;return 0;}}else kxAcc=0;
 if(!kxHop&&!kx_safe(kxRow,kxX,kxOff[kxRow])){kx_die();if(agLives<=0)return 1;}
 if(--kxTime<=0){kx_die();if(agLives<=0)return 1;}return 0;}
static void kx_dog(int x,int y,int f){u16 B=RGB(40,34,30),T=RGB(140,96,56),W=RGB(230,230,220);a_rect(x+3,y+6,10,5,B);a_rect(x+11,y+3,4,5,B);a_rect(x+11,y+1,1,3,B);a_rect(x+14,y+1,1,3,B);
 m3_px(x+13,y+5,W);a_rect(x+12,y+8,3,1,T);a_rect(x+4,y+11,2,3-f,B);a_rect(x+10,y+11,2,2+f,B);a_rect(x+1,y+5,2,2,B);a_rect(x+7,y+9,3,2,W);}
static void kx_draw(void){int y0=16;a_clear(RGB(40,90,40));
 for(int r=0;r<9;r++){int y=y0+r*KX_C;const KxL*L=&KXL[r];
  switch(L->kind){case 1:a_rect(0,y,240,KX_C,RGB(50,50,56));for(int x=0;x<240;x+=24)a_rect(x+(r&1)*8,y+7,10,1,RGB(220,220,120));break;
   case 2:a_rect(0,y,240,KX_C,RGB(30,70,150));for(int x=(agT/4+r*7)%16;x<240;x+=16)a_rect(x,y+4+(r&1)*6,5,1,RGB(90,140,220));break;
   case 3:a_rect(0,y,240,KX_C,RGB(120,100,60));for(int c=0;c<15;c++)if(kxMine[c]){a_disc(c*KX_C+8,y+9,3,RGB(50,50,50));a_rect(c*KX_C+8,y+1,1,6,RGB(200,200,200));a_rect(c*KX_C+9,y+1,4,3,RGB(220,40,40));}break;
   case 4:a_rect(0,y,240,KX_C,RGB(30,60,30));for(int s=0;s<5;s++){int x=KX_SLOT[s]*KX_C;a_rect(x,y+2,16,13,RGB(150,90,50));a_rect(x+2,y,12,3,RGB(180,60,40));a_rect(x+5,y+7,6,8,RGB(20,14,10));if(kxHome[s])kx_dog(x,y,0);}break;
   default:a_rect(0,y,240,KX_C,r==8?RGB(70,110,60):RGB(90,120,70));break;}
  if(L->kind==1||L->kind==2){int per=L->per,w=L->len*KX_C;for(int k=-2;k<240/per+3;k++){int x=(int)(kxOff[r]>>8)%per+k*per;
    if(L->kind==1){u16 c=r==5?RGB(110,120,80):r==6?RGB(200,190,150):RGB(90,100,70);a_rect(x+1,y+3,w-2,10,c);a_rect(x+(L->dir>0?w-6:1),y+4,5,4,RGB(150,200,240));a_rect(x+3,y+12,3,3,0);a_rect(x+w-6,y+12,3,3,0);}
    else{a_rect(x,y+3,w,10,RGB(120,80,40));a_rect(x+2,y+5,w-4,6,RGB(160,110,60));a_rect(x+w/2-2,y+1,4,4,RGB(220,220,220));}}}}
 kx_dog(kxX,y0+kxRow*KX_C-(kxHop?2:0),kxHop?1:0);a_rect(0,160-4,kxTime*240/(60*35),3,RGB(240,200,60));}
static void kx_ai(void){if(kxHop)return;int r=kxRow;int t=10;
 if(r==1){int c=(kxX+8)/KX_C;int want=-1,bd=99;for(int s=0;s<5;s++)if(!kxHome[s]&&iabs(KX_SLOT[s]-c)<bd){bd=iabs(KX_SLOT[s]-c);want=KX_SLOT[s];}
  if(want==c){aiP|=K_UP;return;}int nc=c+(want>c?1:-1);if(!kxMine[nc])aiP|=want>c?K_RIGHT:K_LEFT;else if(c>0&&c<14)aiP|=K_DOWN;return;}
 if(r==2||r==4){int c=(kxX+8)/KX_C;int need=r==4?-1:0;(void)need;}
 int nr=r-1;if(nr<0)return;int nx=kxX;if(KXL[r].kind==2)nx=((nx+8)/KX_C)*KX_C;
 if(nr==1){int c=(nx+8)/KX_C;if(kxMine[c]){/* sidestep along this row first if safe */int d=c<7?1:-1;int sx=kxX+d*KX_C;if(KXL[r].kind!=2&&kx_safe(r,sx,kxOff[r]+KXL[r].dir*kx_speed(r)*32*6))aiP|=d>0?K_RIGHT:K_LEFT;return;}}
 int ok=1;for(int k=2;k<=t;k+=2){s32 o=kxOff[nr]+KXL[nr].dir*kx_speed(nr)*32*k;int x=nx+(KXL[nr].kind==2?(KXL[nr].dir*kx_speed(nr)*32*(k-4))/256:0);if(!kx_safe(nr,x,o)){ok=0;break;}}
 if(ok){aiP|=K_UP;return;}
 // current cell getting dangerous? step back
 s32 o2=kxOff[r]+KXL[r].dir*kx_speed(r)*32*8;if(!kx_safe(r,kxX,o2)&&r<8&&KXL[r].kind==1)aiP|=K_DOWN;
 if(KXL[r].kind==2&&(kxX<20||kxX>200))aiP|=K_UP;}
// ---------------------------------------------------------------- 4) BUNKER MAZE: treats, patrol robots, bone power-up
static const char*const BM_MAP[12]={"####################","#o.......##.......o#","#.##.###.##.###.##.#","#..................#","#.##.#.######.#.##.#","#....#...HH...#....#",
 "####.###.##.###.####","#........##........#","#.##.###.##.###.##.#","#o.#............#.o#","##...#.#....#.#...##","####################"};
#define BM_W 20
#define BM_H 12
#define BM_S 12
static u8 bmG[BM_H][BM_W] EWRAM;static int bmLeft,bmPow,bmChain;
typedef struct{s16 x,y;s8 dx,dy,ndx,ndy;u8 st;s16 t;}BmE; // st: 0 normal 1 frightened 2 eaten
static BmE bmK EWRAM,bmR[3] EWRAM;
static int bm_open(int cx,int cy){return cx>=0&&cy>=0&&cx<BM_W&&cy<BM_H&&bmG[cy][cx]!=1;}
static void bm_pos(void){bmK.x=9*BM_S+6;bmK.y=7*BM_S+6;bmK.dx=bmK.dy=bmK.ndx=bmK.ndy=0;for(int i=0;i<3;i++){BmE*r=&bmR[i];r->x=(8+i)*BM_S+6;r->y=5*BM_S+6;r->dx=i==1?0:(i?1:-1);r->dy=0;r->st=0;r->t=i*90;}bmPow=0;}
static void bm_level(void){bmLeft=0;for(int y=0;y<BM_H;y++)for(int x=0;x<BM_W;x++){char c=BM_MAP[y][x];bmG[y][x]=c=='#'?1:c=='.'?2:c=='o'?3:0;if(c=='.'||c=='o')bmLeft++;}bm_pos();}
static void bm_init(void){agLvl=0;bm_level();}
static void bm_move(BmE*e,int sp){for(int s=0;s<sp;s++){int cx=e->x/BM_S,cy=e->y/BM_S;
  if(e->x%BM_S==6&&e->y%BM_S==6){if((e->ndx||e->ndy)&&bm_open(cx+e->ndx,cy+e->ndy)){e->dx=e->ndx;e->dy=e->ndy;}if(!bm_open(cx+e->dx,cy+e->dy)){e->dx=e->dy=0;}}
  e->x+=e->dx;e->y+=e->dy;}}
static void bm_rdecide(BmE*r,int i){int cx=r->x/BM_S,cy=r->y/BM_S;int tx,ty;
 if(r->st==2){tx=9;ty=5;}else{int kx=bmK.x/BM_S,ky=bmK.y/BM_S;tx=kx;ty=ky;if(i==1){tx+=bmK.dx*4;ty+=bmK.dy*4;}if(i==2&&iabs(kx-cx)+iabs(ky-cy)<6){tx=i&1?18:1;ty=10;}}
 static const s8 D[4][2]={{0,-1},{-1,0},{0,1},{1,0}};int bd=99999,bi=-1;
 for(int k=0;k<4;k++){int nx=cx+D[k][0],ny=cy+D[k][1];if(!bm_open(nx,ny))continue;if(D[k][0]==-r->dx&&D[k][1]==-r->dy&&(r->dx||r->dy))continue;
  int d=(nx-tx)*(nx-tx)+(ny-ty)*(ny-ty);if(r->st==1)d=rr(100);if(d<bd){bd=d;bi=k;}}
 if(bi<0){r->ndx=-r->dx;r->ndy=-r->dy;}else{r->ndx=D[bi][0];r->ndy=D[bi][1];}}
static int bm_step(u16 h,u16 p){(void)p;if(h&K_UP){bmK.ndx=0;bmK.ndy=-1;}else if(h&K_DOWN){bmK.ndx=0;bmK.ndy=1;}else if(h&K_LEFT){bmK.ndx=-1;bmK.ndy=0;}else if(h&K_RIGHT){bmK.ndx=1;bmK.ndy=0;}
 bm_move(&bmK,(agT%3)?1:2);int cx=bmK.x/BM_S,cy=bmK.y/BM_S;
 if(bmG[cy][cx]==2){bmG[cy][cx]=0;agScore+=10;bmLeft--;if(!(agT&3))sfx1(0,0x6,1,2,1900,4);}
 if(bmG[cy][cx]==3){bmG[cy][cx]=0;agScore+=50;bmLeft--;bmPow=420;bmChain=0;for(int i=0;i<3;i++)if(bmR[i].st==0){bmR[i].st=1;bmR[i].ndx=-bmR[i].dx;bmR[i].ndy=-bmR[i].dy;}sfx_ok();}
 if(bmPow&&--bmPow==0)for(int i=0;i<3;i++)if(bmR[i].st==1)bmR[i].st=0;
 for(int i=0;i<3;i++){BmE*r=&bmR[i];if(r->t>0){r->t--;continue;}if(r->x%BM_S==6&&r->y%BM_S==6)bm_rdecide(r,i);
  int sp=r->st==2?3:r->st==1?((agT&1)?1:0):((agT%4)<(2+(agLvl>1)+(agLvl>3))?1:0)+((agT%4)==0&&agLvl>2);bm_move(r,sp);
  if(r->st==2&&r->x/BM_S==9&&r->y/BM_S==5)r->st=0;
  if(iabs(r->x-bmK.x)<8&&iabs(r->y-bmK.y)<8){if(r->st==1){r->st=2;bmChain++;agScore+=200<<(bmChain>3?3:bmChain-1);sfx_boom();}else if(r->st==0){agLives--;sfx_boom();if(agLives<=0)return 1;bm_pos();return 0;}}}
 if(bmLeft<=0){agLvl++;agScore+=500;bm_level();}return 0;}
static void bm_draw(void){a_clear(RGB(6,8,14));int y0=16;
 for(int y=0;y<BM_H;y++)for(int x=0;x<BM_W;x++){int X=x*BM_S,Y=y0+y*BM_S;u8 v=bmG[y][x];
  if(v==1){a_rect(X,Y,BM_S,BM_S,RGB(60,70,60));a_rect(X+1,Y+1,BM_S-2,BM_S-2,RGB(84,96,80));}else if(v==2)a_rect(X+5,Y+5,3,2,RGB(200,150,90));
  else if(v==3&&(agT&16)){a_rect(X+2,Y+5,8,2,RGB(240,240,230));a_rect(X+1,Y+4,2,4,RGB(240,240,230));a_rect(X+9,Y+4,2,4,RGB(240,240,230));}}
 for(int i=0;i<3;i++){BmE*r=&bmR[i];int x=r->x-6,y=y0+r->y-6;static const u16 RC[3]={RGB(220,60,50),RGB(60,160,240),RGB(240,160,40)};
  u16 c=r->st==1?((bmPow<90&&(agT&8))?RGB(240,240,240):RGB(60,60,200)):RC[i];if(r->st==2){a_rect(x+3,y+4,2,2,RGB(255,255,255));a_rect(x+7,y+4,2,2,RGB(255,255,255));continue;}
  a_rect(x+2,y+1,8,8,c);a_rect(x+1,y+3,10,4,c);a_rect(x+3,y+3,2,2,RGB(255,255,200));a_rect(x+7,y+3,2,2,RGB(255,255,200));a_rect(x+2,y+9,2,2,RGB(80,80,80));a_rect(x+8,y+9,2,2,RGB(80,80,80));a_rect(x+5,y-1,2,2,RGB(200,200,200));}
 kx_dog(bmK.x-8,y0+bmK.y-9,(agT>>3)&1);}
static void bm_ai(void){ // BFS to the nearest treat (or a frightened robot) avoiding cells near active robots
 static u8 dist[BM_H][BM_W] EWRAM;static u8 first[BM_H][BM_W] EWRAM;static u8 qx[240] EWRAM,qy[240] EWRAM;
 int cx=bmK.x/BM_S,cy=bmK.y/BM_S;if(bmK.x%BM_S!=6||bmK.y%BM_S!=6){return;}
 memset(dist,255,sizeof dist);int h=0,t=0;dist[cy][cx]=0;first[cy][cx]=255;qx[t]=cx;qy[t]=cy;t++;static const s8 D[4][2]={{0,-1},{-1,0},{0,1},{1,0}};int goal=-1;
 while(h<t){int x=qx[h],y=qy[h];h++;
  int isg=(bmG[y][x]==2||bmG[y][x]==3);for(int i=0;i<3;i++)if(bmR[i].st==1&&bmR[i].x/BM_S==x&&bmR[i].y/BM_S==y)isg=1;
  if(isg&&dist[y][x]>0){goal=first[y][x];break;}
  for(int k=0;k<4;k++){int nx=x+D[k][0],ny=y+D[k][1];if(!bm_open(nx,ny)||dist[ny][nx]!=255)continue;int bad=0;
   for(int i=0;i<3;i++){if(bmR[i].st!=0||bmR[i].t>0)continue;int rx=bmR[i].x/BM_S,ry=bmR[i].y/BM_S;if(iabs(rx-nx)+iabs(ry-ny)<=2)bad=1;}
   if(bad)continue;dist[ny][nx]=dist[y][x]+1;first[ny][nx]=dist[y][x]==0?k:first[y][x];if(t<240){qx[t]=nx;qy[t]=ny;t++;}}}
 if(goal<0){/* flee: pick any open dir away from nearest robot */int bk=-1,bs=-1;for(int k=0;k<4;k++){int nx=cx+D[k][0],ny=cy+D[k][1];if(!bm_open(nx,ny))continue;int m=99;for(int i=0;i<3;i++){int d=iabs(bmR[i].x/BM_S-nx)+iabs(bmR[i].y/BM_S-ny);if(d<m)m=d;}if(m>bs){bs=m;bk=k;}}goal=bk;}
 if(goal<0)return;static const u16 KD[4]={K_UP,K_LEFT,K_DOWN,K_RIGHT};aiH|=KD[goal];}
