// ---------------------------------------------------------------- 9) STARSHIP HOPPER: one-button hops through launch-tower gaps
typedef struct{s16 x,gy;u8 sc;}ShT;static ShT shT[4] EWRAM;static s32 shY,shVy;static int shDead,shGap;
static void sh_init(void){shY=70<<8;shVy=0;shDead=0;for(int i=0;i<4;i++){shT[i].x=200+i*70;shT[i].gy=40+rr(70);shT[i].sc=0;}shGap=58;}
static int sh_step(u16 h,u16 p){(void)h;if(shDead){if(++shDead>40)return 1;shVy+=40;shY+=shVy;if(shY>(150<<8))shY=150<<8;return 0;}
 if(p&K_A){shVy=-540;sfx1(0,0x2,1,2,1300,3);}shVy+=32;if(shVy>800)shVy=800;shY+=shVy;int y=shY>>8;
 int sp=2+(agScore>=10)+(agScore>=30);shGap=clampi(64-agScore/2,46,64);
 for(int i=0;i<4;i++){ShT*t=&shT[i];t->x-=sp;if(t->x<-20){int mx=0;for(int j=0;j<4;j++)if(shT[j].x>mx)mx=shT[j].x;t->x=mx+70;int ny=t->gy+rr(50)-25;t->gy=clampi(ny,22,150-shGap-8);t->sc=0;}
  if(!t->sc&&t->x+14<50){t->sc=1;agScore++;sfx_menu();}
  if(t->x<56&&t->x+16>44&&(y-5<t->gy||y+5>t->gy+shGap)){shDead=1;sfx_boom();}}
 if(y<12||y>148){shDead=1;sfx_boom();}return 0;}
static void sh_draw(void){for(int y=10;y<160;y+=10)a_rect(0,y,240,10,RGB(10+y/4,20+y/3,60+y/2));for(int i=0;i<30;i++)m3_px((i*83+agT/(1+(i&1)))%240,12+(i*41)%100,RGB(200,200,220));
 a_rect(0,150,240,10,RGB(60,70,60));
 for(int i=0;i<4;i++){ShT*t=&shT[i];u16 S=RGB(150,150,160),D=RGB(90,90,100);a_rect(t->x,10,16,t->gy-10,S);a_rect(t->x,t->gy+shGap,16,150-t->gy-shGap,S);
  for(int y=12;y<t->gy;y+=8)a_line(t->x,y,t->x+15,y+7,D);for(int y=t->gy+shGap;y<146;y+=8)a_line(t->x,y,t->x+15,y+7,D);a_rect(t->x-3,t->gy-4,22,4,RGB(200,60,40));a_rect(t->x-3,t->gy+shGap,22,4,RGB(200,60,40));}
 int y=shY>>8;int tilt=clampi(shVy/150,-4,5);a_rect(46,y-4,14,8,RGB(220,220,230));a_rect(60,y-3,3,6,RGB(200,200,210));a_rect(44,y-6,4,3,RGB(60,60,70));a_rect(44,y+3,4,3,RGB(60,60,70));
 a_rect(47,y-3+tilt/2,2,1,RGB(30,30,30));if(!shDead&&shVy<0){a_rect(38,y-2,6,4,RGB(255,180,60));a_rect(34,y-1,4,2,RGB(255,240,140));}if(shDead)a_disc(52,y,4+(shDead&7),RGB(255,140,40));
 m3_ctext(20,itoa(agScore),C_WHITE,2);}
static void sh_ai(void){if(shDead)return;int y=shY>>8;ShT*n=0;for(int i=0;i<4;i++)if(shT[i].x+16>40&&(!n||shT[i].x<n->x))n=&shT[i];
 int top=n?n->gy+10:40,bot=n?n->gy+shGap-10:120;s32 yp=shY+shVy*8+32*36;if((yp>>8)>bot&&y>top)aiP|=K_A;if(y>140)aiP|=K_A;}
// ---------------------------------------------------------------- 10) TANK PONG: vs CPU, ricochet shell
static s32 tpA,tpB,tpX,tpY,tpVx,tpVy;static int tpSa,tpSb,tpServe,tpPow,tpHit;
static void tp_ball(int dir){tpX=120<<8;tpY=80<<8;tpVx=dir*(260);tpVy=(rr(2)?1:-1)*(100+rr(120));tpServe=60;}
static void tp_init(void){tpA=tpB=80<<8;tpSa=tpSb=0;tp_ball(1);tpPow=0;tpHit=0;}
static int tp_step(u16 h,u16 p){(void)p;if(h&K_UP)tpA-=3<<8;if(h&K_DOWN)tpA+=3<<8;tpA=clampi(tpA,24<<8,146<<8);tpPow=(h&K_A)?1:0;if(tpHit)tpHit--;
 // CPU: tracks the shell with a reaction lag that shrinks as the match goes on
 int lag=clampi(18-agScore/400,8,18);{s32 t=tpVx>0?tpY:(80<<8);s32 d=t-tpB;int mx=(2<<8)+(tpSa*20);if(d>mx)d=mx;if(d<-mx)d=-mx;if(agT%lag<lag-2)tpB+=d;tpB=clampi(tpB,24<<8,146<<8);}
 if(tpServe){tpServe--;return 0;}
 tpX+=tpVx;tpY+=tpVy;int x=tpX>>8,y=tpY>>8;if(y<14){tpY=14<<8;tpVy=iabs(tpVy);sfx1(0,0x2,1,2,800,2);}if(y>154){tpY=154<<8;tpVy=-iabs(tpVy);sfx1(0,0x2,1,2,800,2);}
 if(tpVx<0&&x<=20&&x>=12&&iabs(y-(tpA>>8))<=13){int off=y-(tpA>>8);tpVx=iabs(tpVx)*17/16+(tpPow?100:0);if(tpVx>900)tpVx=900;tpVy=off*40;tpHit=10;agScore+=10+(tpPow?10:0);sfx_shot(0);}
 if(tpVx>0&&x>=220&&x<=228&&iabs(y-(tpB>>8))<=13){int off=y-(tpB>>8);tpVx=-iabs(tpVx)*17/16;if(tpVx<-900)tpVx=-900;tpVy=off*40+rr(60)-30;sfx_shot(0);}
 if(x<0){tpSb++;agLives=3-tpSb;sfx_boom();if(tpSb>=3)return 1;tp_ball(1);}if(x>240){tpSa++;agScore+=250;sfx_ok();tp_ball(-1);}return 0;}
static void tp_tank(int x,int y,int f,u16 c){a_rect(x-4,y-11,8,22,c);a_rect(x-6,y-12,2,24,RGB(40,40,40));a_rect(x+4,y-12,2,24,RGB(40,40,40));a_disc(x,y,4,RGB(60,70,50));a_rect(f>0?x:x-8,y-1,8,2,RGB(50,50,50));}
static void tp_draw(void){a_clear(RGB(80,100,60));for(int y=14;y<160;y+=8)a_rect(119,y,2,4,RGB(200,200,160));a_rect(0,10,240,3,RGB(60,70,50));
 tp_tank(14,tpA>>8,1,tpPow?RGB(150,170,90):RGB(110,130,80));tp_tank(226,tpB>>8,-1,RGB(150,110,80));
 if(!tpServe||(tpServe&8)){int x=tpX>>8,y=tpY>>8;a_disc(x,y,2,RGB(250,230,140));a_rect(x-(tpVx>0?6:-3),y,3,1,RGB(255,160,60));}
 m3_text(90,20,itoa(tpSa),C_WHITE,2);m3_text(140,20,itoa(tpSb),C_WHITE,2);}
static void tp_ai(void){s32 x=tpX,y=tpY,vy=tpVy;int t=(80<<8);if(tpVx<0){while(x>(16<<8)){x+=tpVx;y+=vy;if(y<(14<<8)){y=(28<<8)-y;vy=-vy;}if(y>(154<<8)){y=(308<<8)-y;vy=-vy;}}t=y+(((agT/120)%3)-1)*(6<<8);}
 if(tpA<t-(2<<8))aiH|=K_DOWN;if(tpA>t+(2<<8))aiH|=K_UP;if(tpVx<0&&(tpX>>8)<40)aiH|=K_A;}
// ---------------------------------------------------------------- 11) TANK DUEL: top-down 1v1 in mazes vs CPU, ricochet shells
static const char*const TD_MAP[3][8]={{"###############","#.............#","#..##.....##..#","#.....###.....#","#.....###.....#","#..##.....##..#","#.............#","###############"},
 {"###############","#......#......#","#.##.......##.#","#.#..##.##..#.#","#.#..##.##..#.#","#.##.......##.#","#......#......#","###############"},
 {"###############","#.....#.......#","#.###.#.###.#.#","#...#...#...#.#","#.#...#...#...#","#.#.###.#.###.#","#.......#.....#","###############"}};
typedef struct{s32 x,y;int a,cd,hp;}TdT;typedef struct{s32 x,y,vx,vy;int life,own,bn;}TdB;static TdT tdP EWRAM,tdC EWRAM;static TdB tdB[6] EWRAM;static int tdMap,tdRound,tdWin,tdLose,tdPause;
#define TD_S 16
static int td_wall(int x,int y){int c=x/TD_S,r=(y-16)/TD_S;if(y<16||r>=8||c<0||c>=15)return 1;return TD_MAP[tdMap][r][c]=='#';}
static void td_round(void){tdMap=tdRound%3;tdP.x=(1*TD_S+8)<<8;tdP.y=(16+6*TD_S+8)<<8;tdP.a=0;tdC.x=(13*TD_S+8)<<8;tdC.y=(16+1*TD_S+8)<<8;tdC.a=128;tdP.cd=tdC.cd=30;memset(tdB,0,sizeof tdB);tdPause=60;}
static void td_init(void){tdRound=0;tdWin=tdLose=0;td_round();}
static void td_shoot(TdT*t,int own){int n=0;for(int i=0;i<6;i++)if(tdB[i].life&&tdB[i].own==own)n++;if(n>=2||t->cd)return;for(int i=0;i<6;i++)if(!tdB[i].life){TdB*b=&tdB[i];b->x=t->x+SIN(t->a)*9;b->y=t->y-COS(t->a)*9;b->vx=SIN(t->a)*3;b->vy=-COS(t->a)*3;b->life=200;b->own=own;b->bn=0;t->cd=40;sfx_shot(0);return;}}
static void td_move(TdT*t,int fw){s32 nx=t->x+SIN(t->a)*fw/2,ny=t->y-COS(t->a)*fw/2;int r=5;
 if(!td_wall((nx>>8)-r,(t->y>>8)-r)&&!td_wall((nx>>8)+r,(t->y>>8)-r)&&!td_wall((nx>>8)-r,(t->y>>8)+r)&&!td_wall((nx>>8)+r,(t->y>>8)+r))t->x=nx;
 if(!td_wall((t->x>>8)-r,(ny>>8)-r)&&!td_wall((t->x>>8)+r,(ny>>8)-r)&&!td_wall((t->x>>8)-r,(ny>>8)+r)&&!td_wall((t->x>>8)+r,(ny>>8)+r))t->y=ny;}
static int td_los(s32 ax,s32 ay,s32 bx,s32 by){for(int k=1;k<32;k++){int x=(ax+(bx-ax)*k/32)>>8,y=(ay+(by-ay)*k/32)>>8;if(td_wall(x,y))return 0;}return 1;}
static void td_cpu(void){TdT*c=&tdC;int dx=(tdP.x-c->x)>>8,dy=(tdP.y-c->y)>>8;int want;int los=td_los(c->x,c->y,tdP.x,tdP.y);
 if(los)want=a_ang(dx,dy);else{ // wander along open cells towards the player
  int cc=(c->x>>8)/TD_S,cr=((c->y>>8)-16)/TD_S;static const s8 D[4][2]={{0,-1},{1,0},{0,1},{-1,0}};int bk=0,bd=99999;int pc=(tdP.x>>8)/TD_S,pr=((tdP.y>>8)-16)/TD_S;
  for(int k=0;k<4;k++){int nc=cc+D[k][0],nr=cr+D[k][1];if(nr<0||nr>7||nc<0||nc>14||TD_MAP[tdMap][nr][nc]=='#')continue;int d=(nc-pc)*(nc-pc)+(nr-pr)*(nr-pr)+((agT/90+k)%4==0?6:0);if(d<bd){bd=d;bk=k;}}
  int tx=(cc+D[bk][0])*TD_S+8,ty=16+(cr+D[bk][1])*TD_S+8;want=a_ang(tx-(c->x>>8),ty-(c->y>>8));}
 int df=a_adiff(want,c->a);int tr=2+(tdRound>2);if(df>tr)c->a+=tr;else if(df<-tr)c->a-=tr;else c->a=want;c->a&=255;
 if(!los||dx*dx+dy*dy>50*50)if(iabs(df)<40)td_move(c,180);
 if(los&&iabs(df)<6)td_shoot(c,0);}
static int td_step(u16 h,u16 p){(void)p;if(tdPause){tdPause--;return 0;}if(h&K_LEFT)tdP.a-=3;if(h&K_RIGHT)tdP.a+=3;tdP.a&=255;if(h&K_UP)td_move(&tdP,220);if(h&K_DOWN)td_move(&tdP,-140);
 if(tdP.cd)tdP.cd--;if(tdC.cd)tdC.cd--;if(h&K_A)td_shoot(&tdP,1);td_cpu();
 for(int i=0;i<6;i++){TdB*b=&tdB[i];if(!b->life)continue;b->life--;
  s32 nx=b->x+b->vx*256/256,ny=b->y+b->vy;if(td_wall(nx>>8,b->y>>8)){b->vx=-b->vx;b->bn++;nx=b->x;}if(td_wall(b->x>>8,ny>>8)){b->vy=-b->vy;b->bn++;ny=b->y;}b->x=nx;b->y=ny;if(b->bn>3)b->life=0;
  for(int k=0;k<2;k++){TdT*t=k?&tdC:&tdP;if(b->life&&iabs((b->x-t->x)>>8)<6&&iabs((b->y-t->y)>>8)<6&&(b->bn>0||b->own!=k?1:0)&&!(b->own==1&&k==0&&b->bn==0)&&!(b->own==0&&k==1&&b->bn==0)){b->life=0;sfx_boom();
    if(k==1){tdWin++;agScore+=500+tdRound*100;tdRound++;td_round();return 0;}else{tdLose++;agLives--;if(agLives<=0)return 1;tdRound++;td_round();return 0;}}}}
 return 0;}
static void td_tank(TdT*t,u16 c){int x=t->x>>8,y=t->y>>8;a_disc(x,y,6,RGB(40,40,40));a_disc(x,y,5,c);int bx=x+(SIN(t->a)*9>>8),by=y-(COS(t->a)*9>>8);a_line(x,y,bx,by,RGB(30,30,30));a_line(x+1,y,bx+1,by,RGB(30,30,30));a_disc(x,y,2,RGB(200,200,180));}
static void td_draw(void){a_clear(RGB(110,100,70));for(int r=0;r<8;r++)for(int c=0;c<15;c++)if(TD_MAP[tdMap][r][c]=='#'){a_rect(c*TD_S,16+r*TD_S,TD_S,TD_S,RGB(80,70,60));a_rect(c*TD_S+1,17+r*TD_S,TD_S-2,TD_S-2,RGB(120,108,90));a_rect(c*TD_S+1,17+r*TD_S+7,TD_S-2,1,RGB(80,70,60));}
 td_tank(&tdP,RGB(90,140,70));td_tank(&tdC,RGB(170,90,70));for(int i=0;i<6;i++)if(tdB[i].life)a_rect((tdB[i].x>>8)-1,(tdB[i].y>>8)-1,3,3,RGB(255,240,160));
 m3_text(4,152,cat("ROUND ",itoa(tdRound+1)),C_WHITE,1);m3_text(150,152,cat3(itoa(tdWin)," - ",itoa(tdLose)),C_YEL,1);if(tdPause)m3_ctext(70,cat("ROUND ",itoa(tdRound+1)),C_YEL,2);}
static void td_ai(void){TdT*c=&tdP;int dx=(tdC.x-c->x)>>8,dy=(tdC.y-c->y)>>8;int want;int los=td_los(c->x,c->y,tdC.x,tdC.y);
 if(los)want=a_ang(dx,dy);else{int cc=(c->x>>8)/TD_S,cr=((c->y>>8)-16)/TD_S;static const s8 D[4][2]={{0,-1},{1,0},{0,1},{-1,0}};int bk=0,bd=99999;int pc=(tdC.x>>8)/TD_S,pr=((tdC.y>>8)-16)/TD_S;
  for(int k=0;k<4;k++){int nc=cc+D[k][0],nr=cr+D[k][1];if(nr<0||nr>7||nc<0||nc>14||TD_MAP[tdMap][nr][nc]=='#')continue;int d=(nc-pc)*(nc-pc)+(nr-pr)*(nr-pr)+((agT/70+k)%4==0?6:0);if(d<bd){bd=d;bk=k;}}
  want=a_ang((cc+D[bk][0])*TD_S+8-(c->x>>8),16+(cr+D[bk][1])*TD_S+8-(c->y>>8));}
 int df=a_adiff(want,c->a);if(df>2)aiH|=K_RIGHT;else if(df<-2)aiH|=K_LEFT;if(iabs(df)<30&&(!los||dx*dx+dy*dy>40*40))aiH|=K_UP;if(los&&iabs(df)<5)aiH|=K_A;}
// ---------------------------------------------------------------- 12) SPACE PINBALL: ramps, MARS targets -> multiball, launch-pad plunger
typedef struct{s32 x,y,vx,vy;u8 on,lane;}PbB;static PbB pbB[3] EWRAM;static int pbSave,pbPl,pbFl,pbFr,pbFlA,pbFrA,pbMars,pbMulti,pbBonus,pbBump[3],pbRamp;
static const s16 PB_BX[3]={80,130,105},PB_BY[3]={50,50,80};
#define PB_G 9
static void pn_new(void){memset(pbB,0,sizeof pbB);pbB[0].on=1;pbB[0].lane=1;pbB[0].x=222<<8;pbB[0].y=140<<8;pbPl=0;pbSave=900;}
static void pn_init(void){pbMars=0;pbMulti=0;pbBonus=1;pbRamp=0;pbFlA=pbFrA=0;for(int i=0;i<3;i++)pbBump[i]=0;pn_new();}
static void pn_seg(PbB*b,int x0,int y0,int x1,int y1,int kick){ // reflect off a segment (8.8 fixed in ball coords)
 s32 bx=b->x>>4,by=b->y>>4;s32 X0=x0<<4,Y0=y0<<4,X1=x1<<4,Y1=y1<<4;s32 dx=X1-X0,dy=Y1-Y0;s32 L2=dx*dx+dy*dy;if(!L2)return;s32 t=((bx-X0)*dx+(by-Y0)*dy);if(t<0)t=0;if(t>L2)t=L2;
 s32 px=X0+(s32)((long long)dx*t/L2),py=Y0+(s32)((long long)dy*t/L2);s32 ex=bx-px,ey=by-py;s32 d2=ex*ex+ey*ey;const s32 R=3<<4;if(d2>=R*R||d2==0)return;
 int d=0;{s32 a=iabs(ex),c=iabs(ey);d=a>c?a+c/2:c+a/2;}if(d<1)d=1;s32 nx=ex*256/d,ny=ey*256/d;s32 vn=(b->vx*nx+b->vy*ny)>>8;
 if(vn<0){b->vx-=(vn*nx>>8)*(kick?2:19)/(kick?1:10);b->vy-=(vn*ny>>8)*(kick?2:19)/(kick?1:10);if(kick){b->vx+=nx*kick>>8;b->vy+=ny*kick>>8;}}
 b->x=(px+nx*R/256+(nx>0?1:-1))<<4;b->y=(py+ny*R/256+(ny>0?1:-1))<<4;}
static int pn_step(u16 h,u16 p){(void)p;
 int pl=(h&(K_DOWN|K_A))&&pbB[0].lane;if(pl){if(pbPl<60)pbPl++;}
 int la=(h&(K_L|K_LEFT))?1:0,ra=(h&(K_R|K_RIGHT|K_A))?1:0;if(pbB[0].lane&&pbB[0].on)ra=(h&(K_R|K_RIGHT))?1:0;
 int pfl=pbFlA,pfr=pbFrA;pbFlA=la?clampi(pbFlA+64,0,128):clampi(pbFlA-32,0,128);pbFrA=ra?clampi(pbFrA+64,0,128):clampi(pbFrA-32,0,128);if((pbFlA>pfl&&pfl==0)||(pbFrA>pfr&&pfr==0))sfx1(0,0x2,1,2,700,2);
 for(int i=0;i<3;i++)if(pbBump[i])pbBump[i]--;if(pbRamp)pbRamp--;
 int alive=0;for(int i=0;i<3;i++){PbB*b=&pbB[i];if(!b->on)continue;alive++;
  if(b->lane){if(!pl&&pbPl>0&&i==0){b->vy=-(pbPl*40+300);pbPl=0;b->vx=0;sfx_shot(0);}
   if(b->vy==0){b->x=222<<8;b->y=140<<8;continue;}b->vy+=PB_G;b->y+=b->vy;if((b->y>>8)<30){b->lane=0;b->vx=-500;b->vy=0;b->x=196<<8;b->y=44<<8;}if((b->y>>8)>140){b->y=140<<8;b->vy=0;}continue;}
  for(int sub=0;sub<4;sub++){b->vy+=PB_G/4+((sub&1)?1:0);b->x+=b->vx/4;b->y+=b->vy/4;
   int x=b->x>>8,y=b->y>>8;
   // walls: outer table (40..214), top arc approximated by segments
   pn_seg(b,40,150,40,40,0);pn_seg(b,214,150,214,40,0);pn_seg(b,40,40,70,16,0);pn_seg(b,70,16,184,16,0);pn_seg(b,184,16,214,40,0);
   pn_seg(b,40,118,94,140,0);pn_seg(b,214,118,160,140,0);
   // flippers: pivot (94,140)/(160,140), length 26, raised by angle
   {int ly=150-pbFlA*24/128,ry=150-pbFrA*24/128;int kl=pbFlA>pfl?600:0,kr=pbFrA>pfr?600:0;pn_seg(b,94,140,121,ly,kl);pn_seg(b,160,140,133,ry,kr);}
   // bumpers
   for(int k=0;k<3;k++){int dx=x-PB_BX[k],dy=y-PB_BY[k];if(dx*dx+dy*dy<11*11){int d=iabs(dx)+iabs(dy)+1;b->vx=dx*900/d;b->vy=dy*900/d;b->x=(PB_BX[k]+dx*12/ (d?d:1)*1)<<8;b->x=(PB_BX[k]<<8)+dx*12*256/d;b->y=(PB_BY[k]<<8)+dy*12*256/d;pbBump[k]=10;agScore+=100*pbBonus;sfx1(0,0x5,2,2,1500+k*200,5);}}
   // MARS drop targets on the left lane (4 letters)
   if(x<52&&y>60&&y<100&&b->vx<0){int k=(y-60)/10;if(!(pbMars&(1<<k))){pbMars|=1<<k;agScore+=250*pbBonus;sfx_ok();b->vx=iabs(b->vx);
     if(pbMars==15){pbMars=0;pbMulti++;pbBonus=clampi(pbBonus+1,1,5);agScore+=1000;sfx_ok();for(int j=1;j<3;j++)if(!pbB[j].on){pbB[j]=pbB[0];pbB[j].on=1;pbB[j].lane=0;pbB[j].x=127<<8;pbB[j].y=24<<8;pbB[j].vx=(j==1?-300:300);pbB[j].vy=100;}}}}
   // ramp: entering the top-right orbit at speed scores + boosts the bonus
   if(x>186&&y<46&&b->vy<-200&&!pbRamp){pbRamp=40;agScore+=500*pbBonus;b->vx=-700;b->vy=200;sfx_ok();}
   if(b->x<(43<<8)){b->x=43<<8;b->vx=iabs(b->vx);}if(b->x>(211<<8)){b->x=211<<8;b->vx=-iabs(b->vx);}if(b->y<(19<<8)){b->y=19<<8;b->vy=iabs(b->vy);}
   int sp=iabs(b->vx)+iabs(b->vy);if(sp>1400){b->vx=b->vx*1400/sp;b->vy=b->vy*1400/sp;}
   if(y>162){b->on=0;break;}}}
 if(pbSave)pbSave--;
 if(!alive&&pbSave){pn_new();pbSave=0;sfx_ok();return 0;}
 if(!alive){agLives--;pbBonus=1;sfx_boom();if(agLives<=0)return 1;pn_new();}return 0;}
static void pn_draw(void){a_clear(RGB(6,4,16));for(int i=0;i<40;i++)m3_px((i*61)%240,12+(i*29)%146,RGB(160,160,200));
 a_rect(40,16,175,144,RGB(40,14,24));for(int y=20;y<160;y+=12)a_rect(42,y,171,1,RGB(54,20,32));a_disc(127,105,22,RGB(150,60,30));a_disc(121,100,6,RGB(120,46,24));a_disc(136,113,4,RGB(120,46,24));
 a_line(40,150,40,40,C_WHITE);a_line(214,150,214,40,C_WHITE);a_line(40,40,70,16,C_WHITE);a_line(70,16,184,16,C_WHITE);a_line(184,16,214,40,C_WHITE);a_line(40,118,94,140,C_WHITE);a_line(214,118,160,140,C_WHITE);
 a_rect(216,40,14,120,RGB(30,30,40));a_rect(219,146+pbPl/6,8,4,RGB(200,200,210));a_rect(220,150+pbPl/6,6,8,RGB(140,140,150));m3_text(216,30,"GO",C_GRAY,1);
 for(int k=0;k<3;k++){a_disc(PB_BX[k],PB_BY[k],9,pbBump[k]?RGB(255,255,200):RGB(60,140,240));a_disc(PB_BX[k],PB_BY[k],5,RGB(240,240,250));}
 static const char*const MS[4]={"M","A","R","S"};for(int k=0;k<4;k++){u16 c=(pbMars&(1<<k))?RGB(255,140,60):RGB(90,50,40);a_rect(42,62+k*10,7,8,c);m3_text(50,63+k*10,MS[k],c,1);}
 a_rect(178,20,30,6,pbRamp?C_YEL:RGB(90,90,120));m3_text(176,28,"RAMP",C_GRAY,1);
 {int ly=150-pbFlA*24/128,ry=150-pbFrA*24/128;for(int w=-1;w<=1;w++){a_line(94,140+w,121,ly+w,RGB(240,200,60));a_line(160,140+w,133,ry+w,RGB(240,200,60));}}
 for(int i=0;i<3;i++)if(pbB[i].on){int x=pbB[i].x>>8,y=pbB[i].y>>8;a_disc(x,y,3,RGB(220,220,230));m3_px(x-1,y-1,RGB(255,255,255));}
 if(pbSave&&(agT&16))m3_text(110,152,"BALL SAVE",C_GREEN,1);
 m3_text(2,20,"BONUS",C_GRAY,1);m3_text(2,30,cat("X",itoa(pbBonus)),C_YEL,1);m3_text(2,50,"MULTI",C_GRAY,1);m3_text(2,60,itoa(pbMulti),C_WHITE,1);}
static void pn_ai(void){if(pbB[0].on&&pbB[0].lane&&pbB[0].vy==0){static int hold;if(hold<40+((agT/600)%3)*8){aiH|=K_DOWN;hold++;}else hold=0;return;}
 for(int i=0;i<3;i++){PbB*b=&pbB[i];if(!b->on||b->lane)continue;int x=b->x>>8,y=b->y>>8;if(y>122&&y<150&&b->vy>-100){if(x<=127)aiH|=K_L;else aiH|=K_R;}}}
