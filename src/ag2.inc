// ---------------------------------------------------------------- 5) BUNKER BUSTER: tank paddle bounces shells to break fortress walls
#define BB_C 12
#define BB_R 8
static u8 bbW[BB_R][BB_C] EWRAM;typedef struct{s32 x,y,vx,vy;u8 on,stuck;}BbS;static BbS bbS[3] EWRAM;static s16 bbX,bbW2,bbWide,bbLeft;
typedef struct{s16 x,y;u8 k,on;}BbP;static BbP bbP[3] EWRAM;
static void bk_level(void){bbLeft=0;for(int r=0;r<BB_R;r++)for(int c=0;c<BB_C;c++){int v=0;int pat=agLvl%3;
  if(pat==0)v=r<6?1+(r<2):0;else if(pat==1)v=((r+c)&1)||r==0?1+(r<3):0;else v=(c>1&&c<10&&r<7)?(r==0||c==2||c==9?3:1+(r&1)):0;
  bbW[r][c]=v;if(v&&v<3)bbLeft++;}
 memset(bbS,0,sizeof bbS);bbS[0].on=1;bbS[0].stuck=1;memset(bbP,0,sizeof bbP);}
static void bk_init(void){bbX=120;bbW2=14;bbWide=0;agLvl=0;bk_level();}
static int bk_step(u16 h,u16 p){int sp=(h&K_R)?5:3;if(h&K_LEFT)bbX-=sp;if(h&K_RIGHT)bbX+=sp;bbW2=bbWide?22:14;bbX=clampi(bbX,bbW2,240-bbW2);if(bbWide)bbWide--;
 int alive=0;for(int i=0;i<3;i++){BbS*s=&bbS[i];if(!s->on)continue;alive++;
  if(s->stuck){s->x=bbX<<8;s->y=140<<8;if((p&K_A)||(agT>120&&agAttract)){s->stuck=0;s->vx=(rr(2)?1:-1)*(160+rr(80));s->vy=-(400+agLvl*25);sfx_shot(0);}continue;}
  for(int sub=0;sub<2;sub++){s->x+=s->vx/2;s->y+=s->vy/2;int x=s->x>>8,y=s->y>>8;
   if(x<2){s->x=2<<8;s->vx=iabs(s->vx);}if(x>237){s->x=237<<8;s->vx=-iabs(s->vx);}if(y<12){s->y=12<<8;s->vy=iabs(s->vy);}
   int c=(x-0)/20,r=(y-16)/8;if(y>=16&&r<BB_R&&c>=0&&c<BB_C&&bbW[r][c]){u8 v=bbW[r][c];if(v<3){if(v==2)bbW[r][c]=1;else{bbW[r][c]=0;bbLeft--;agScore+=10*(BB_R-r);
      if(rr(7)==0)for(int k=0;k<3;k++)if(!bbP[k].on){bbP[k].on=1;bbP[k].x=c*20+10;bbP[k].y=16+r*8;bbP[k].k=rr(3);break;}}sfx1(0,0x4,2,2,1600-r*90,6);}else sfx1(0,0x4,1,2,600,4);
    s->vy=-s->vy;s->y+=s->vy/2;}
   if(s->vy>0&&y>=139&&y<=144&&iabs(x-bbX)<=bbW2+1){int off=(x-bbX)*256/bbW2;s->vx=off*2;if(iabs(s->vx)<90)s->vx=(s->vx<0||(s->vx==0&&(agT&1)))?-90-rr(40):90+rr(40);int spd=400+agLvl*25+((agT/600)*10);if(spd>640)spd=640;s->vy=-spd;s->y=138<<8;sfx_menu();}
   if(y>162){s->on=0;break;}}}
 if(!alive){agLives--;sfx_boom();if(agLives<=0)return 1;bbS[0].on=1;bbS[0].stuck=1;}
 for(int k=0;k<3;k++){BbP*q=&bbP[k];if(!q->on)continue;q->y++;if(q->y>=136&&q->y<=144&&iabs(q->x-bbX)<bbW2+4){q->on=0;sfx_ok();agScore+=50;
   if(q->k==0)bbWide=900;else if(q->k==1){for(int i=0;i<3;i++)if(!bbS[i].on){for(int j=0;j<3;j++)if(bbS[j].on&&!bbS[j].stuck){bbS[i]=bbS[j];bbS[i].vx=-bbS[j].vx+(i*50);break;}}}else agLives=agLives<5?agLives+1:5;}
  if(q->y>160)q->on=0;}
 if(bbLeft<=0){agLvl++;agScore+=300;bk_level();}return 0;}
static void bk_draw(void){a_clear(RGB(30,36,60));a_rect(0,148,240,12,RGB(70,64,48));
 for(int r=0;r<BB_R;r++)for(int c=0;c<BB_C;c++){u8 v=bbW[r][c];if(!v)continue;int x=c*20,y=16+r*8;u16 col=v==3?RGB(90,90,100):v==2?RGB(150,110,80):RGB(190,150,100);
  a_rect(x+1,y+1,18,6,col);a_rect(x+1,y+1,18,1,RGB(230,210,170));if(v==3)a_rect(x+8,y+2,4,4,RGB(60,60,70));if(v==2)a_rect(x+9,y+1,1,6,RGB(110,80,60));}
 for(int k=0;k<3;k++)if(bbP[k].on){static const u16 PC[3]={RGB(60,200,90),RGB(250,200,60),RGB(240,80,80)};static const char*const PL[3]={"W","M","+"};a_rect(bbP[k].x-5,bbP[k].y-4,10,8,PC[bbP[k].k]);m3_text(bbP[k].x-2,bbP[k].y-3,PL[bbP[k].k],0,1);}
 a_rect(bbX-bbW2,142,bbW2*2,6,RGB(90,110,70));a_rect(bbX-bbW2+2,148,bbW2*2-4,3,RGB(40,40,40));a_rect(bbX-5,138,10,4,RGB(110,130,84));a_rect(bbX-1,134,2,4,RGB(70,80,60));
 for(int i=0;i<3;i++)if(bbS[i].on){int x=bbS[i].x>>8,y=bbS[i].y>>8;a_rect(x-1,y-2,3,5,RGB(240,220,120));m3_px(x,y-3,RGB(255,255,255));}}
static void bk_ai(void){int tx=120,best=-1;s32 by=-1;for(int i=0;i<3;i++)if(bbS[i].on&&!bbS[i].stuck&&bbS[i].vy>0&&bbS[i].y>by){by=bbS[i].y;best=i;}
 if(best>=0){BbS*s=&bbS[best];s32 x=s->x,y=s->y,vx=s->vx;int n=0;while(y<(139<<8)&&n<400){x+=vx;y+=s->vy;if(x<(2<<8)){x=(4<<8)-x;vx=-vx;}if(x>(237<<8)){x=(474<<8)-x;vx=-vx;}n++;}int px=x>>8;int sx=0,cn=0;for(int r=0;r<BB_R;r++)for(int c=0;c<BB_C;c++)if(bbW[r][c]&&bbW[r][c]<3){sx+=c*20+10;cn++;}int off=cn?clampi((sx/cn-px)/6,-11,11):0;tx=px-off;}
 else{for(int k=0;k<3;k++)if(bbP[k].on&&bbP[k].y>90)tx=bbP[k].x;for(int i=0;i<3;i++)if(bbS[i].on&&bbS[i].stuck&&(agT&31)==0)aiP|=K_A;}
 if(tx<bbX-2)aiH|=K_LEFT;if(tx>bbX+2)aiH|=K_RIGHT;if(iabs(tx-bbX)>30)aiH|=K_R;}
// ---------------------------------------------------------------- 6) CARGO STACK: original falling-crate puzzle in a C-17 bay
#define CS_W 10
#define CS_H 16
static u8 csG[CS_H][CS_W] EWRAM;static int csK,csR,csX,csY,csNext,csFall,csLines,csClr,csLock;
// 7 original pieces made of 1x1 crates: pallet pairs and triples, 2x2 container, corner, long 4, step
static const s8 CSP[7][4][2]={{{0,0},{1,0},{0,0},{1,0}},{{0,0},{1,0},{2,0},{2,0}},{{0,0},{1,0},{0,1},{1,1}},{{0,0},{1,0},{0,1},{0,1}},{{0,0},{1,0},{2,0},{3,0}},{{0,0},{1,0},{1,1},{2,1}},{{0,0},{1,0},{2,0},{1,1}}};
static const u16 CSC[7]={RGB(200,170,90),RGB(110,150,90),RGB(80,120,170),RGB(180,110,70),RGB(150,150,160),RGB(170,90,140),RGB(90,170,170)};
static void cs_cell(int k,int r,int i,int*x,int*y){int a=CSP[k][i][0],b=CSP[k][i][1];for(int n=0;n<(r&3);n++){int t=a;a=-b;b=t;}*x=a;*y=b;}
static int cs_fits(int k,int r,int X,int Y){for(int i=0;i<4;i++){int x,y;cs_cell(k,r,i,&x,&y);x+=X;y+=Y;if(x<0||x>=CS_W||y>=CS_H)return 0;if(y>=0&&csG[y][x])return 0;}return 1;}
static int csSer;
static int cs_new(void){csSer++;csK=csNext;csNext=rr(7);csR=0;csX=4;csY=0;csLock=0;return cs_fits(csK,csR,csX,csY);}
static void cs_init(void){memset(csG,0,sizeof csG);csLines=0;csClr=0;agLvl=0;csNext=rr(7);cs_new();csFall=0;}
static void cs_lock(void){for(int i=0;i<4;i++){int x,y;cs_cell(csK,csR,i,&x,&y);x+=csX;y+=csY;if(y>=0)csG[y][x]=csK+1;}
 int n=0;for(int y=CS_H-1;y>=0;y--){int f=1;for(int x=0;x<CS_W;x++)f&=csG[y][x]!=0;if(f){n++;for(int yy=y;yy>0;yy--)memcpy(csG[yy],csG[yy-1],CS_W);memset(csG[0],0,CS_W);y++;}}
 static const u16 PTS[5]={0,100,300,600,1000};if(n){agScore+=PTS[n>4?4:n]*(agLvl<9?agLvl+1:10);csLines+=n;csClr=12;sfx_ok();agLvl=csLines/8;}else sfx1(0,0x3,1,2,900,3);}
static int cs_step(u16 h,u16 p){if(csClr)csClr--;
 if(p&K_LEFT&&cs_fits(csK,csR,csX-1,csY))csX--;if(p&K_RIGHT&&cs_fits(csK,csR,csX+1,csY))csX++;
 if(p&(K_A|K_UP)){int nr=(csR+1)&3;if(cs_fits(csK,nr,csX,csY))csR=nr;else if(cs_fits(csK,nr,csX-1,csY)){csR=nr;csX--;}else if(cs_fits(csK,nr,csX+1,csY)){csR=nr;csX++;}}
 if(p&K_B){while(cs_fits(csK,csR,csX,csY+1)){csY++;agScore+=2;}csFall=999;}
 int spd=clampi(40-agLvl*4,4,40);if(h&K_DOWN)spd=2;
 if(++csFall>=spd){csFall=0;if(cs_fits(csK,csR,csX,csY+1)){csY++;if(h&K_DOWN)agScore++;}else{cs_lock();if(!cs_new())return 1;}}return 0;}
static void cs_draw(void){a_clear(RGB(30,34,40));int X0=60,Y0=12,S=9;
 a_rect(X0-6,Y0-2,CS_W*S+12,CS_H*S+6,RGB(70,76,70));a_rect(X0-2,Y0,CS_W*S+4,CS_H*S+2,RGB(14,16,20));for(int y=0;y<CS_H;y+=2)a_rect(X0-5,Y0+y*S+4,2,2,RGB(240,200,60));
 for(int y=0;y<CS_H;y++)for(int x=0;x<CS_W;x++){u8 v=csG[y][x];if(!v)continue;int px=X0+x*S,py=Y0+y*S;a_rect(px,py,S-1,S-1,CSC[v-1]);a_rect(px+1,py+1,S-3,1,RGB(240,240,220));a_line(px+1,py+2,px+S-3,py+S-3,RGB(60,50,40));}
 int gy=csY;while(cs_fits(csK,csR,csX,gy+1))gy++;
 for(int i=0;i<4;i++){int x,y;cs_cell(csK,csR,i,&x,&y);a_rect(X0+(x+csX)*S,Y0+(y+gy)*S+S-3,S-1,2,RGB(70,70,90));if(y+csY>=0){int px=X0+(x+csX)*S,py=Y0+(y+csY)*S;a_rect(px,py,S-1,S-1,CSC[csK]);a_rect(px+1,py+1,S-3,1,RGB(255,255,255));}}
 m3_text(170,20,"NEXT",C_GRAY,1);for(int i=0;i<4;i++){int x,y;cs_cell(csNext,0,i,&x,&y);a_rect(176+x*8,34+y*8,7,7,CSC[csNext]);}
 m3_text(170,62,"LOADS",C_GRAY,1);m3_text(170,72,itoa(csLines),C_WHITE,1);m3_text(170,88,"LEVEL",C_GRAY,1);m3_text(170,98,itoa(agLvl+1),C_WHITE,1);
 m3_text(4,30,"C-17",RGB(150,160,170),1);m3_text(4,40,"BAY",RGB(150,160,170),1);if(csClr)m3_ctext(150,"LOAD SECURED!",C_YEL,1);}
static int cs_eval(void){int hgt=0,holes=0,bump=0,lines=0,ph=-1;for(int y=0;y<CS_H;y++){int f=1;for(int x=0;x<CS_W;x++)f&=csG[y][x]!=0;lines+=f;}
 for(int x=0;x<CS_W;x++){int y=0;while(y<CS_H&&!csG[y][x])y++;int h=CS_H-y;hgt+=h;for(int yy=y;yy<CS_H;yy++)if(!csG[yy][x])holes++;if(ph>=0)bump+=iabs(h-ph);ph=h;}
 return lines*760-hgt*510-holes*360-bump*180;}
static int csAiX=-99,csAiR,csAiK=-1;
static void cs_ai(void){if(csAiK!=csSer){csAiK=csSer;int best=-0x7FFFFFF;static u8 save[CS_H][CS_W] EWRAM;
  for(int r=0;r<4;r++)for(int X=-3;X<CS_W+3;X++){if(!cs_fits(csK,r,X,0)&&!cs_fits(csK,r,X,csY))continue;int Y=csY<0?0:csY;if(!cs_fits(csK,r,X,Y))continue;while(cs_fits(csK,r,X,Y+1))Y++;
   memcpy(save,csG,sizeof save);for(int i=0;i<4;i++){int x,y;cs_cell(csK,r,i,&x,&y);if(y+Y>=0)csG[y+Y][x+X]=1;}int e=cs_eval()+rr(30);memcpy(csG,save,sizeof save);if(e>best){best=e;csAiX=X;csAiR=r;}}}
 if(agT&1)return;if(csR!=csAiR){aiP|=K_A;return;}if(csX<csAiX)aiP|=K_RIGHT;else if(csX>csAiX)aiP|=K_LEFT;else if(!agAttract)aiP|=K_B;else aiH|=K_DOWN;}
// ---------------------------------------------------------------- 7) CONVOY: snake-style truck convoy picking up supply crates
#define CV_W 30
#define CV_H 18
static u8 cvX[CV_W*CV_H] EWRAM,cvY[CV_W*CV_H] EWRAM;static int cvLen,cvHead,cvDx,cvDy,cvNdx,cvNdy,cvFx,cvFy,cvTick,cvGrow;static u8 cvRock[CV_H][CV_W] EWRAM;
static u8 cvOcc[CV_H][CV_W] EWRAM; // occupancy grid (fast self-collision + autopilot flood fill)
static int cv_on(int x,int y){return cvOcc[y][x];}
static void cv_food(void){for(int n=0;n<500;n++){int x=rr(CV_W),y=rr(CV_H);if(!cvRock[y][x]&&!cv_on(x,y)){cvFx=x;cvFy=y;return;}}}
static void cv_init(void){memset(cvRock,0,sizeof cvRock);memset(cvOcc,0,sizeof cvOcc);cvLen=3;cvHead=2;for(int i=0;i<3;i++){cvX[i]=5+i;cvY[i]=9;cvOcc[9][5+i]=1;}cvDx=1;cvDy=0;cvNdx=1;cvNdy=0;cvTick=0;cvGrow=0;agLvl=0;cv_food();}
static int cv_step(u16 h,u16 p){(void)h;
 if((p|h)&K_UP&&cvDy==0){cvNdx=0;cvNdy=-1;}else if((p|h)&K_DOWN&&cvDy==0){cvNdx=0;cvNdy=1;}else if((p|h)&K_LEFT&&cvDx==0){cvNdx=-1;cvNdy=0;}else if((p|h)&K_RIGHT&&cvDx==0){cvNdx=1;cvNdy=0;}
 int spd=clampi(8-cvLen/6,3,8);if(++cvTick<spd)return 0;cvTick=0;cvDx=cvNdx;cvDy=cvNdy;
 int nx=cvX[cvHead]+cvDx,ny=cvY[cvHead]+cvDy;if(nx<0||ny<0||nx>=CV_W||ny>=CV_H||cvRock[ny][nx]){agLives=0;sfx_boom();return 1;}
 int tail=(cvHead-cvLen+1+CV_W*CV_H)%(CV_W*CV_H);if(cv_on(nx,ny)&&!(cvX[tail]==nx&&cvY[tail]==ny&&!cvGrow)){agLives=0;sfx_boom();return 1;}
 if(cvGrow){cvGrow--;cvLen++;}else cvOcc[cvY[tail]][cvX[tail]]=0;
 cvHead=(cvHead+1)%(CV_W*CV_H);cvX[cvHead]=nx;cvY[cvHead]=ny;cvOcc[ny][nx]=1;
 if(nx==cvFx&&ny==cvFy){agScore+=10+cvLen;cvGrow+=2;sfx_ok();if(cvLen%10<2&&cvLen>8){agLvl++;for(int k=0;k<3;k++){int x=rr(CV_W),y=rr(CV_H);if(!cv_on(x,y)&&iabs(x-nx)+iabs(y-ny)>4)cvRock[y][x]=1;}}cv_food();}
 return 0;}
static void cv_draw(void){a_clear(RGB(150,130,90));for(int i=0;i<60;i++)a_rect((i*71)%240,12+(i*37)%148,2,1,RGB(130,112,76));a_rect(0,10,240,2,RGB(90,80,60));
 int S=8,Y0=12;for(int y=0;y<CV_H;y++)for(int x=0;x<CV_W;x++)if(cvRock[y][x]){a_rect(x*S,Y0+y*S,S,S,RGB(110,100,90));a_rect(x*S+1,Y0+y*S+1,4,3,RGB(140,130,120));}
 {int x=cvFx*S,y=Y0+cvFy*S;a_rect(x+1,y+1,6,6,RGB(160,110,60));a_rect(x+1,y+3,6,1,RGB(100,70,40));a_rect(x+3,y+1,1,6,RGB(100,70,40));}
 for(int i=cvLen-1;i>=0;i--){int k=(cvHead-i+CV_W*CV_H)%(CV_W*CV_H);int x=cvX[k]*S,y=Y0+cvY[k]*S;
  if(i==0){a_rect(x,y,S,S,RGB(70,90,50));int dx=cvDx,dy=cvDy;a_rect(x+3+dx*2,y+3+dy*2,2,2,RGB(150,200,240));}
  else{a_rect(x+1,y+1,6,6,(i&1)?RGB(90,110,64):RGB(80,98,56));a_rect(x+2,y+2,4,4,RGB(110,120,80));}}}
static void cv_ai(void){int hx=cvX[cvHead],hy=cvY[cvHead];static const s8 D[4][2]={{0,-1},{1,0},{0,1},{-1,0}};static const u16 KD[4]={K_UP,K_RIGHT,K_DOWN,K_LEFT};
 int bk=-1;s32 bs=-0x7FFFFFF;for(int k=0;k<4;k++){if(D[k][0]==-cvDx&&D[k][1]==-cvDy)continue;int nx=hx+D[k][0],ny=hy+D[k][1];if(nx<0||ny<0||nx>=CV_W||ny>=CV_H||cvRock[ny][nx]||cv_on(nx,ny))continue;
  // free space flood (bounded) to avoid traps
  static u8 seen[CV_H][CV_W] EWRAM;static u8 qx[540] EWRAM,qy[540] EWRAM;memset(seen,0,sizeof seen);int h=0,t=0;qx[t]=nx;qy[t]=ny;t++;seen[ny][nx]=1;
  while(h<t&&t<cvLen*2+40){int x=qx[h],y=qy[h];h++;for(int j=0;j<4;j++){int a=x+D[j][0],b=y+D[j][1];if(a<0||b<0||a>=CV_W||b>=CV_H||seen[b][a]||cvRock[b][a]||cv_on(a,b))continue;seen[b][a]=1;qx[t]=a;qy[t]=b;t++;}}
  s32 sc=-(iabs(nx-cvFx)+iabs(ny-cvFy))*10+(t>=cvLen*2+40?1000:t*20);if(sc>bs){bs=sc;bk=k;}}
 if(bk>=0)aiH|=KD[bk];}
// ---------------------------------------------------------------- 8) CHOPPER RESCUE: Black Hawk rescues hostages and brings them to base
typedef struct{s16 x;s8 st;u8 a;}ChH; // st 0 in hut 1 waving 2 aboard 3 saved 4 lost
typedef struct{s32 x,y,vx;u8 on,k;s16 hp,cd;}ChE; // k 0 tank 1 jet
typedef struct{s32 x,y,vx,vy;u8 on,own;}ChB;
#define CH_WW 960
static ChH chH[16] EWRAM;static ChE chE[6] EWRAM;static ChB chB[10] EWRAM;static s32 chX,chY,chVx,chVy;static int chFace,chLoad,chSaved,chLost,chInv,chCool,chHuts,chCam;
static void ch_wave(void){agLvl++;for(int i=0;i<16;i++){chH[i].x=CH_WW-260+(i/4)*60+(i%4)*6;chH[i].st=0;chH[i].a=0;}chHuts=4;
 for(int i=0;i<6;i++){chE[i].on=i<2+agLvl;chE[i].k=i>=3;chE[i].x=(300+rr(600))<<8;chE[i].y=(i>=3?40:142)<<8;chE[i].vx=(rr(2)?1:-1)*(i>=3?300:60);chE[i].hp=3;chE[i].cd=60+rr(60);}chX=60<<8;chY=120<<8;chVx=chVy=0;chLoad=0;}
static void ch_init(void){agLvl=0;chSaved=chLost=0;memset(chB,0,sizeof chB);ch_wave();chFace=1;chInv=60;chCool=0;}
static void ch_fire(s32 x,s32 y,s32 vx,s32 vy,int own){for(int i=0;i<10;i++)if(!chB[i].on){ChB*b=&chB[i];b->on=1;b->x=x;b->y=y;b->vx=vx;b->vy=vy;b->own=own;return;}}
static int ch_step(u16 h,u16 p){if(h&K_LEFT)chVx-=24;if(h&K_RIGHT)chVx+=24;if(h&K_UP)chVy-=20;if(h&K_DOWN)chVy+=20;chVx=chVx*15/16;chVy=chVy*14/16+4;
 if(p&K_B)chFace=-chFace;chX+=chVx;chY+=chVy;chX=clampi(chX,8<<8,(CH_WW-8)<<8);if(chY<(16<<8))chY=16<<8;int gnd=(chY>>8)>=138;if(gnd){chY=138<<8;chVy=0;chVx=chVx/2;}
 if(chInv)chInv--;if(chCool)chCool--;
 if((h&K_A)&&!chCool&&!gnd){ch_fire(chX+chFace*(10<<8),chY+(3<<8),chFace*(4<<8)+chVx,256+(chVy/2),1);chCool=10;sfx_shot(0);}
 int x=chX>>8;
 // huts: shooting one opens it; hostages run to a landed chopper
 for(int i=0;i<16;i++){ChH*q=&chH[i];if(q->st==1||q->st==0){if(q->st==1){if(gnd&&iabs(q->x-x)<60){q->x+=q->x<x?1:-1;if(iabs(q->x-x)<5&&chLoad<8){q->st=2;chLoad++;sfx_menu();}}else if(agT&32)q->x+=(agT&64)?1:-1;}}}
 if(gnd&&x<70&&chLoad){chSaved+=chLoad;agScore+=chLoad*150;for(int i=0;i<16;i++)if(chH[i].st==2)chH[i].st=3;chLoad=0;sfx_ok();}
 int left=0;for(int i=0;i<16;i++)if(chH[i].st<3)left++;if(!left){agScore+=500;ch_wave();}
 for(int i=0;i<10;i++){ChB*b=&chB[i];if(!b->on)continue;b->x+=b->vx;b->y+=b->vy;int bx=b->x>>8,by=b->y>>8;if(by>146||bx<0||bx>CH_WW||by<10){b->on=0;
   if(b->own&&by>146){for(int k=0;k<4;k++){int hx=CH_WW-260+k*60+9;if(iabs(bx-hx)<14)for(int j=k*4;j<k*4+4;j++)if(chH[j].st==0)chH[j].st=1;}
    for(int j=0;j<16;j++)if(chH[j].st==1&&iabs(chH[j].x-bx)<6){chH[j].st=4;chLost++;agScore-=300;if(agScore<0)agScore=0;sfx_boom();}}continue;}
  if(b->own){for(int k=0;k<6;k++){ChE*e=&chE[k];if(!e->on)continue;if(iabs(bx-(e->x>>8))<10&&iabs(by-(e->y>>8))<6){b->on=0;if(--e->hp<=0){e->on=0;agScore+=e->k?200:100;sfx_boom();
       e->on=1;e->hp=3;e->x=(x+(rr(2)?260:-260))<<8;if(e->x<(200<<8))e->x=(500<<8);}break;}}
   for(int j=0;j<16;j++)if(chH[j].st==1&&iabs(chH[j].x-bx)<3&&iabs(by-142)<4){b->on=0;chH[j].st=4;chLost++;agScore=agScore>300?agScore-300:0;sfx_boom();}
   for(int k=0;k<4;k++){int hx=CH_WW-260+k*60+9;if(iabs(bx-hx)<12&&by>130){b->on=0;for(int j=k*4;j<k*4+4;j++)if(chH[j].st==0)chH[j].st=1;}}}
  else if(!chInv&&iabs(bx-x)<10&&iabs(by-(chY>>8))<6){b->on=0;agLives--;chInv=120;sfx_boom();for(int j=0;j<16;j++)if(chH[j].st==2){chH[j].st=4;chLost++;}chLoad=0;chX=60<<8;chY=120<<8;if(agLives<=0)return 1;}}
 for(int k=0;k<6;k++){ChE*e=&chE[k];if(!e->on)continue;e->x+=e->vx;if((e->x>>8)<220||(e->x>>8)>CH_WW-20)e->vx=-e->vx;if(e->k&&(agT&63)==0)e->vx=((e->x>>8)<x?1:-1)*300;
  if(--e->cd<=0){e->cd=clampi(160-agLvl*12,50,160)+rr(60);int ex=e->x>>8,ey=e->y>>8;if(iabs(ex-x)<160){int dx=x-ex,dy=(chY>>8)-ey;int d=iabs(dx)+iabs(dy)+1;ch_fire(e->x,e->y,dx*420/d,dy*420/d,0);}}}
 if(chLost>=8)return 1;
 int tc=x-120+chFace*30;chCam+=(tc-chCam)/8;chCam=clampi(chCam,0,CH_WW-240);return 0;}
static void ch_draw(void){for(int y=10;y<150;y+=14)a_rect(0,y,240,14,RGB(80+y/3,140+y/4,220));int c=chCam;
 for(int i=0;i<12;i++){int mx=((i*137)-c/3)%400;if(mx<-60)mx+=400;a_disc(mx,150,30+(i%3)*8,RGB(120,140,110));}
 a_rect(0,146,240,14,RGB(150,130,80));
 {int bx=20-c;a_rect(bx,128,40,18,RGB(90,100,80));a_rect(bx+2,124,36,4,RGB(70,80,60));a_rect(bx+14,118,2,6,RGB(200,200,200));a_rect(bx+16,118,8,5,RGB(60,140,240));m3_text(bx+8,134,"BASE",C_WHITE,1);a_rect(bx+40,144,30,2,RGB(240,240,240));}
 for(int k=0;k<4;k++){int hx=CH_WW-260+k*60-c;int open=0;for(int j=k*4;j<k*4+4;j++)open|=chH[j].st!=0;a_rect(hx,130,18,16,open?RGB(70,50,40):RGB(130,90,60));a_rect(hx-2,126,22,4,RGB(110,70,50));if(open)a_rect(hx+6,136,6,10,RGB(20,10,10));}
 for(int j=0;j<16;j++){ChH*q=&chH[j];if(q->st!=1)continue;int hx=q->x-c;a_rect(hx-1,138,3,8,RGB(240,220,180));a_rect(hx-1,136,3,3,RGB(220,180,140));if(agT&16)a_rect(hx+2,134,1,4,RGB(240,220,180));}
 for(int k=0;k<6;k++){ChE*e=&chE[k];if(!e->on)continue;int ex=(e->x>>8)-c,ey=e->y>>8;if(ex<-20||ex>260)continue;
  if(!e->k){a_rect(ex-9,ey-2,18,6,RGB(100,90,80));a_rect(ex-5,ey-6,10,4,RGB(120,110,96));a_rect(ex,ey-5,9,1,RGB(70,60,50));}
  else{a_rect(ex-8,ey,16,3,RGB(150,150,160));a_rect(ex-2,ey-3,6,9,RGB(130,130,140));a_rect(ex+(e->vx>0?7:-9),ey,2,2,RGB(255,160,60));}}
 for(int i=0;i<10;i++)if(chB[i].on){int bx=(chB[i].x>>8)-c,by=chB[i].y>>8;a_rect(bx-1,by,3,2,chB[i].own?RGB(255,255,160):RGB(255,90,60));}
 if(!(chInv&4)){int x=(chX>>8)-c,y=chY>>8;int f=chFace;u16 B=RGB(40,50,40);a_rect(x-9,y-3,18,7,B);a_rect(x-6*f-(f<0?6:0)+(f>0?-10:4),y-2,10,2,B);
  a_rect(x+(f>0?4:-9),y-2,5,3,RGB(140,190,230));a_rect(x-(f>0?20:-12),y-5,2,6,B);a_rect(x-1,y-6,2,3,B);int bl=(agT&2)?14:6;a_rect(x-bl,y-7,bl*2,1,RGB(30,30,30));a_rect(x-6,y+5,12,1,RGB(60,60,60));
  if(chLoad)m3_text(x-3,y-16,itoa(chLoad),C_YEL,1);}
 m3_text(4,152,cat("SAVED ",itoa(chSaved)),RGB(255,255,255),1);m3_text(90,152,cat("ABOARD ",itoa(chLoad)),C_YEL,1);m3_text(180,152,cat("LOST ",itoa(chLost)),RGB(255,120,120),1);
 {int mx=8+(chX>>8)*224/CH_WW;a_rect(8,12,224,2,RGB(40,40,60));a_rect(mx,11,3,4,C_GREEN);}}
static void ch_ai(void){int x=chX>>8,y=chY>>8;int waiting=0,closed=-1;for(int j=0;j<16;j++){if(chH[j].st==1)waiting++;if(chH[j].st==0&&closed<0)closed=j/4;}
 int tx,ty=90;int gohome=chLoad>=8||(chLoad>0&&!waiting&&closed<0)||(chLoad>=4&&waiting==0);
 if(gohome){tx=40;ty=x<80?150:90;}
 else if(waiting){int nx=0,nd=9999;for(int j=0;j<16;j++)if(chH[j].st==1&&iabs(chH[j].x-x)<nd){nd=iabs(chH[j].x-x);nx=chH[j].x;}tx=nx+(nx>x?-30:30);ty=iabs(tx-x)<20?150:100;if(iabs(nx-x)<50)ty=150;}
 else if(closed>=0){int hx=CH_WW-260+closed*60+9;tx=hx-60;ty=110;if(iabs(x-tx)<16){if(chFace<0)aiP|=K_B;aiH|=K_A;}}
 else{tx=x;ty=100;}
 for(int k=0;k<6;k++){ChE*e=&chE[k];if(e->on&&iabs((e->x>>8)-x)<110){int ex=e->x>>8;if((ex>x)!=(chFace>0)&&(agT&15)==0)aiP|=K_B;if((ex>x)==(chFace>0))aiH|=K_A;}}
 if(tx<x-6)aiH|=K_LEFT;if(tx>x+6)aiH|=K_RIGHT;if(ty<y-4)aiH|=K_UP;if(ty>y+4)aiH|=K_DOWN;}
