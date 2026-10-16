// ===================================================================== v1.3.2 FIREFIGHTER - dedicated to LT's dad, a real firefighter
#include "firedb.inc"
#define SCR_FHUB 104
#define SCR_FIRE 106
#define SCR_FTIP 107
#define SCR_FLAD 108
#define SCR_FEXT 109
#define SCR_FDRV 110
#define SCR_FDED 111
#define SCR_FGEAR 112
typedef struct{u8 stars[10];u16 best[10];u32 rescued,runs;HiE t[5];}FSave;static FSave FS EWRAM;static int fs_ld;
static void fs_load(void){if(fs_ld)return;fs_ld=1;if(!sv_get(0x2500,'F',1,&FS,sizeof FS)){memset(&FS,0,sizeof FS);static const char*const N[5]={"DAD","LT ","KC ","E-1","L-1"};static const u32 S[5]={9000,7000,5000,3000,1000};hi_seed(FS.t,N,S);}}
static void fs_save(void){sv_put(0x2500,'F',1,&FS,sizeof FS);}
enum{FT_OUT,FT_FLOOR,FT_WALL,FT_DOOR,FT_LOCK,FT_OPEN,FT_EXIT,FT_HYD,FT_ENG,FT_VENT,FT_FURN,FT_SHELF,FT_GRASS,FT_TREE,FT_HOUSE,FT_ROAD,FT_CAR,FT_BREAK,FT_WIN,FT_AMB};
static u8 fT[FH][FW] EWRAM,fH[FH][FW] EWRAM,fFu[FH][FW] EWRAM,fS[FH][FW] EWRAM,fR[FH][FW] EWRAM,fF0[FH][FW] EWRAM,fPar[FH][FW] EWRAM;static u16 fK[FH][FW] EWRAM;static u8 fQ[FW*FH*2] EWRAM;
typedef struct{u8 x,y,k,st;}FVic;static FVic fV[10] EWRAM;static int fNV;
static u8 fTr[2][120] EWRAM;static int fTrN;static u8 fFo[32] EWRAM;static int fNR;
static int fiFull;static int fTherm,fVent,fHyd,fTank,fPsi,fWind,fTyp,fTick;
static const s8 FDX[4]={0,1,0,-1},FDY[4]={-1,0,1,0};
static int fi_in(int x,int y){return (unsigned)x<FW&&(unsigned)y<FH;}
static int fi_solid(int t){return t==FT_WALL||t==FT_LOCK||t==FT_ENG||t==FT_HYD||t==FT_FURN||t==FT_SHELF||t==FT_TREE||t==FT_HOUSE||t==FT_CAR||t==FT_WIN||t==FT_VENT||t==FT_AMB;}
static int fi_vicat(int x,int y){for(int i=0;i<fNV;i++)if(fV[i].st==0&&fV[i].x==x&&fV[i].y==y)return i;return -1;}
static int fi_walk(int x,int y){if(!fi_in(x,y))return 0;int t=fT[y][x];if(fi_solid(t))return 0;if(fH[y][x]>=10)return 0;return fi_vicat(x,y)<0;}
static int fi_outside(int t){return t==FT_OUT||t==FT_EXIT||t==FT_ROAD||t==FT_GRASS||t==FT_BREAK;}
static int fi_smkp(int t){return t==FT_FLOOR||t==FT_FURN||t==FT_SHELF||t==FT_OPEN;}
static u16 fi_mix(u16 a,u16 b,int t){int r=((a&31)*(8-t)+(b&31)*t)>>3,g=(((a>>5)&31)*(8-t)+((b>>5)&31)*t)>>3,bl=(((a>>10)&31)*(8-t)+((b>>10)&31)*t)>>3;return r|(g<<5)|(bl<<10);}
static u16 fi_therm(int h,int solid){if(h<=0)return solid?RGB(40,40,96):RGB(22,12,58);static const u16 TP[8]={RGB(90,20,120),RGB(150,20,110),RGB(210,30,60),RGB(240,90,20),RGB(255,150,20),RGB(255,210,40),RGB(255,250,140),RGB(255,255,255)};return TP[hmn(7,h/2)];}
// one 8x8 tile -> bigbuf + fb
static void fi_tile(int cx,int cy){int t=fT[cy][cx],h=fH[cy][cx],burnt=fF0[cy][cx]&&!fFu[cy][cx];int sl=fS[cy][cx]>>5;int fl=(fTick+cx*3+cy)&1;
 u16 k=t|(hmn(h,15)<<5)|(sl<<9)|(fTherm<<12)|((h?fl:0)<<13)|(burnt<<14);if(fK[cy][cx]==k)return;fK[cy][cx]=k;
 int x0=cx*8,y0=18+cy*8;u16*B=bigbuf;
 for(int j=0;j<8;j++)for(int i=0;i<8;i++){u16 c;
  if(fTherm){c=fi_therm(h,fi_solid(t));if(t==FT_WALL&&((i+j)&3)==0)c=RGB(60,60,120);}
  else{switch(t){
   case FT_OUT:c=((i*5+j*3+cx+cy)&7)?RGB(70,74,72):RGB(80,84,80);break;
   case FT_FLOOR:c=(j==3||j==7)?RGB(120,84,52):RGB(156,112,70);break;
   case FT_WALL:c=(j==0||j==4||(i==((j<4)?2:6)))?RGB(70,60,58):RGB(150,74,60);break;
   case FT_DOOR:c=(i==0||i==7)?RGB(70,40,20):RGB(130,78,36);if(i==5&&j==4)c=RGB(240,200,90);break;
   case FT_LOCK:c=(i==0||i==7)?RGB(70,40,20):RGB(110,64,30);if(j==3||j==4)c=RGB(150,150,160);break;
   case FT_OPEN:c=(i<2)?RGB(110,64,30):RGB(156,112,70);break;
   case FT_EXIT:c=(j<2)?RGB(60,60,60):RGB(40,90,60);if(j>=2&&(i==0||i==7))c=RGB(200,200,200);break;
   case FT_HYD:c=RGB(70,74,72);if(i>=2&&i<=5&&j>=1)c=RGB(220,40,30);if(j==3&&(i==1||i==6))c=RGB(220,40,30);if(i>=3&&i<=4&&j==0)c=RGB(240,200,60);break;
   case FT_ENG:c=(j==3)?RGB(250,250,250):(j==0&&(i&2))?RGB(255,220,60):RGB(200,24,24);if(j==7&&(i==1||i==6))c=RGB(20,20,20);break;
   case FT_AMB:c=(j==3)?RGB(220,30,30):RGB(240,240,240);if(j==7&&(i==1||i==6))c=RGB(20,20,20);break;
   case FT_VENT:c=RGB(70,74,72);if(i==1||i==6||(j&1))c=RGB(230,200,60);if(!(i==1||i==6)&&(j&1)==0)c=RGB(70,74,72);break;
   case FT_FURN:c=(i==0||j==0||i==7||j==7)?RGB(70,44,30):RGB(110,70,44);break;
   case FT_SHELF:c=(j==0||j==4)?RGB(120,120,130):((i+j/4*3)&3)?RGB(170,130,80):RGB(140,100,60);break;
   case FT_GRASS:c=((i*7+j*5+cx*3+cy)&5)?RGB(150,150,70):RGB(120,130,50);break;
   case FT_TREE:{int dx=i-3,dy=j-3;c=(dx*dx+dy*dy<12)?(((i+j)&3)?RGB(40,100,40):RGB(30,80,30)):RGB(150,150,70);break;}
   case FT_HOUSE:c=(j<3)?RGB(150,50,40):(i==2&&j>=5)?RGB(90,60,40):RGB(220,200,160);break;
   case FT_ROAD:c=(j==3&&(i&4)&&!(cy&1))?RGB(220,200,60):RGB(56,56,62);break;
   case FT_CAR:c=(j<2||j>5)?RGB(40,40,46):RGB(40,90,180);if(j==3&&i>1&&i<6)c=RGB(140,190,230);break;
   case FT_BREAK:c=((i+j)&3)?RGB(110,80,50):RGB(90,64,40);break;
   case FT_WIN:c=(j==0||j==4||(i==((j<4)?2:6)))?RGB(70,60,58):RGB(150,74,60);if(i>=1&&i<=6&&j>=1&&j<=6)c=RGB(120,170,220);break;
   default:c=0;}
   if(burnt)c=fi_mix(c,RGB(26,22,20),6);
   if(h>0){int hs=(i*7+j*13+fl*5+cx)&15;if(hs<h+2){c=j>=5?RGB(255,230,90):j>=2?RGB(255,140,30):RGB(220,50,20);if(hs&8)c=RGB(255,200,60);}}
   if(sl)c=fi_mix(c,RGB(36,36,40),sl+1);}
  int o=(y0+j)*240+x0+i;B[o]=c;fb[o]=c;}
 if(lfN>=0){if(lfN<300){LRect*r=&lfR[lfN++];r->x=x0;r->y=y0;r->w=8;r->h=8;}else fiFull=1;}}
static void fi_load(int m){const FiMis*M=&FIM[m];fNV=0;fTrN=0;fTherm=0;fVent=0;fHyd=0;fTank=M->tank;fPsi=2;fWind=M->wind;fTyp=M->typ;fTick=0;int px=0,py=0;
 for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){char ch=FMAP[m][y][x];int t=FT_OUT,h=0,fu=0;
  switch(ch){case '#':t=FT_WALL;break;case '.':t=FT_FLOOR;fu=40;break;case 'f':t=FT_FURN;fu=110;break;case 's':t=FT_SHELF;fu=150;break;case '!':t=FT_SHELF;fu=150;h=9;break;
   case 'D':t=FT_DOOR;fu=60;break;case 'L':t=FT_LOCK;fu=60;break;case 'E':t=FT_EXIT;break;case 'H':t=FT_HYD;break;case 'T':t=FT_ENG;break;case 'a':t=FT_AMB;break;case 'P':t=fTyp==1?FT_GRASS:FT_OUT;px=x;py=y;fu=fTyp==1?12:0;break;
   case 'V':t=FT_VENT;break;case 'w':t=FT_WIN;if(fNV<10)fV[fNV++]=(FVic){x,y,8,0};break;case '*':t=FT_FLOOR;fu=40;h=8;break;case 'F':t=FT_FURN;fu=110;h=10;break;
   case 'p':case 'c':case 'k':case 'g':t=FT_FLOOR;fu=0;if(fNV<10)fV[fNV++]=(FVic){x,y,(u8)(ch=='p'?0:ch=='c'?1:ch=='k'?2:3),0};break;
   case ',':t=FT_GRASS;fu=12;break;case 'x':t=FT_GRASS;fu=12;h=6;break;case 't':t=FT_TREE;fu=120;break;case 'h':t=FT_HOUSE;fu=250;break;case '=':t=FT_ROAD;break;
   case 'C':t=FT_CAR;fu=160;break;case 'B':t=FT_CAR;fu=160;h=9;break;case '_':t=FT_BREAK;break;}
  fT[y][x]=t;fH[y][x]=h;fFu[y][x]=fu;fF0[y][x]=fu;fS[y][x]=0;fK[y][x]=0xFFFF;fR[y][x]=0;}
 // rooms: 4-connected interior cells, doors split rooms
 fNR=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){int t=fT[y][x];if((t==FT_FLOOR||t==FT_FURN||t==FT_SHELF)&&!fR[y][x]&&fNR<31){fNR++;int qn=0;fQ[qn++]=x;fQ[qn++]=y;fR[y][x]=fNR;
   for(int qi=0;qi<qn;qi+=2){int qx=fQ[qi],qy=fQ[qi+1];for(int d=0;d<4;d++){int nx=qx+FDX[d],ny=qy+FDY[d];if(!fi_in(nx,ny)||fR[ny][nx])continue;int u=fT[ny][nx];if(u==FT_FLOOR||u==FT_FURN||u==FT_SHELF){fR[ny][nx]=fNR;if(qn<(int)sizeof fQ-1){fQ[qn++]=nx;fQ[qn++]=ny;}}}}}}
 memset(fFo,0,sizeof fFo);fTr[0][0]=px;fTr[1][0]=py;fTrN=1;BEAT->x=px;BEAT->y=py;}
static int fi_prop(void){u32 a=0,b=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){int t=fT[y][x];if(fTyp==1&&t!=FT_HOUSE)continue;if(fTyp==2&&t!=FT_CAR)continue;a+=fFu[y][x];b+=fF0[y][x];}return b?(int)(a*100/b):100;}
static int fi_burning(void){int n=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)n+=fH[y][x]>0;return n;}
// fire + smoke simulation tick
static void fi_sim(void){fTick++;
 for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){int h=fH[y][x];if(!h)continue;int t=fT[y][x];int fu=fFu[y][x];
  int hmax=(t==FT_FURN||t==FT_SHELF||t==FT_CAR||t==FT_HOUSE)?15:t==FT_GRASS?9:11;
  if(fu>0){int c=h>=8?2:1;fFu[y][x]=fu>c?fu-c:0;if(h<hmax&&(fTick&1))h++;}else h=h>1?h-2:0;
  if(fVent&&h&&h<hmax&&(fTick&7)==0)h++;
  fH[y][x]=h;if(h<2)continue;
  if(fTyp!=1&&fi_smkp(t))fS[y][x]=hmn(255,fS[y][x]+h);
  for(int d=0;d<4;d++){int nx=x+FDX[d],ny=y+FDY[d];if(!fi_in(nx,ny)||fH[ny][nx]||!fFu[ny][nx])continue;int u=fT[ny][nx];
   int K=u==FT_GRASS?4:u==FT_FLOOR?2:u==FT_DOOR||u==FT_LOCK?1:u==FT_TREE?1:u==FT_HOUSE?1:2;int p=h*K*2;
   if(fWind){int wd=fWind==1?3:fWind==2?1:fWind==3?0:2; // wind blows TOWARD direction wd
    if(d==wd)p*=3;else if(d==((wd+2)&3))p/=3;}
   if(u==FT_DOOR||u==FT_LOCK)p/=3;
   if(rr(256)<p)fH[ny][nx]=1;}}
 if(fTyp==1)return;
 int dec=fVent?6:1;
 for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){int t=fT[y][x];if(!fi_smkp(t)&&t!=FT_DOOR&&t!=FT_LOCK){fS[y][x]=0;continue;}if(t==FT_DOOR||t==FT_LOCK){continue;}
  int s=fS[y][x]*2,n=2;for(int d=0;d<4;d++){int nx=x+FDX[d],ny=y+FDY[d];if(!fi_in(nx,ny))continue;int u=fT[ny][nx];if(fi_smkp(u)){s+=fS[ny][nx];n++;}else if(fi_outside(u)||u==FT_EXIT){n++;}}
  s/=n;s-=dec;fS[y][x]=s<0?0:s;}}
static char fiMB[2][48] EWRAM;static int fiMBk;
static void fi_msg(const char**m,int*mt,const char*s,int t){char*o=fiMB[fiMBk^=1];int k=0;while(s[k]&&k<46){o[k]=s[k];k++;}o[k]=0;*m=o;*mt=t;}
// ---------------------------------------------------- BFS for the autopilot
enum{G_TYPE,G_VIC,G_OUT,G_SPRAY,G_DIG};
static int fi_ray(int x,int y,int d,int reach){for(int k=1;k<=reach;k++){int nx=x+FDX[d]*k,ny=y+FDY[d]*k;if(!fi_in(nx,ny))return 0;if(fH[ny][nx]>0)return k;int t=fT[ny][nx];if(fi_solid(t)||t==FT_DOOR)return 0;}return 0;}
static int fi_goal(int x,int y,int g,int arg,int*fd){
 if(g==G_OUT){int t=fT[y][x];if(fi_outside(t)&&t!=FT_GRASS){*fd=-1;return 1;}return t==FT_GRASS;}
 for(int d=0;d<4;d++){int nx=x+FDX[d],ny=y+FDY[d];if(!fi_in(nx,ny))continue;
  if(g==G_TYPE&&fT[ny][nx]==arg){if(arg==FT_WIN){int ok=0;for(int i=0;i<fNV;i++)if(fV[i].st==0&&fV[i].x==nx&&fV[i].y==ny)ok=1;if(!ok)continue;}*fd=d;return 1;}
  if(g==G_VIC){int v=fi_vicat(nx,ny);if(v>=0&&!(fV[v].k&8)){*fd=d;return 1;}}
  if(g==G_SPRAY&&fi_ray(x,y,d,1+fPsi)){*fd=d;return 1;}
  if(g==G_DIG&&fT[ny][nx]==FT_GRASS&&!fH[ny][nx]&&fFu[ny][nx]){for(int e=0;e<4;e++){int qx=nx+FDX[e],qy=ny+FDY[e];if(fi_in(qx,qy)&&fH[qy][qx]){*fd=d;return 1;}}}}
 return 0;}
// returns first step dir (0-3), 4 = already at goal (face *fd), -1 = none
static int fi_bfs(int sx,int sy,int g,int arg,int*fd){memset(fPar,0xFF,sizeof fPar);int qn=0;fQ[qn++]=sx;fQ[qn++]=sy;fPar[sy][sx]=4;
 for(int qi=0;qi<qn;qi+=2){int x=fQ[qi],y=fQ[qi+1];if(fi_goal(x,y,g,arg,fd)){if(x==sx&&y==sy)return 4;int d=4;while(!(x==sx&&y==sy)){d=fPar[y][x];x-=FDX[d];y-=FDY[d];}return d;}
  for(int d=0;d<4;d++){int nx=x+FDX[d],ny=y+FDY[d];if(!fi_in(nx,ny)||fPar[ny][nx]!=0xFF)continue;int ok=fi_walk(nx,ny)||(fi_in(nx,ny)&&fT[ny][nx]==FT_LOCK);if(!ok)continue;fPar[ny][nx]=d;if(qn<(int)sizeof fQ-1){fQ[qn++]=nx;fQ[qn++]=ny;}}}
 return -1;}
// ---------------------------------------------------- sprites
static void fi_ff(int x,int y,int d,int carry,int fr){ // firefighter, top-down: helmet, tan turnout coat, reflective trim, SCBA bottle
 lf(x+1,y+2,6,5,RGB(196,170,96));lf(x+1,y+4,6,1,RGB(230,240,120));                  // coat + trim
 if(d==0)lf(x+2,y+5,4,3,RGB(170,170,180));else if(d==2)lf(x+2,y,4,3,RGB(170,170,180)); // SCBA bottle on the back
 lf(x+2,y+(d==2?4:1),4,3,RGB(20,20,22));lf(x+3,y+(d==2?5:2),2,1,RGB(240,200,60));     // helmet with shield
 int nx=x+3+FDX[d]*4,ny=y+3+FDY[d]*4;lf(nx,ny,2,2,RGB(60,60,70));                      // nozzle
 if(carry>=0){lf(x+5,y,3,3,carry==2?RGB(240,150,60):carry==3?RGB(30,30,34):RGB(240,200,160));}(void)fr;}
static void fi_vic(int x,int y,int k,int fr){int w=(fr>>4)&1;
 if(k==0){lf(x+2,y+1,4,3,RGB(240,200,160));lf(x+1,y+4,6,3,RGB(60,110,200));lf(x+(w?0:7),y+3,1,2,RGB(240,200,160));}
 else if(k==1){lf(x+3,y+2,3,2,RGB(240,200,160));lf(x+2,y+4,5,3,RGB(240,90,140));}
 else if(k==2){lf(x+1,y+3,6,3,RGB(240,150,60));lf(x+5,y+2,2,2,RGB(240,150,60));lf(x+1,y+2-w,1,2,RGB(240,150,60));}
 else{lf(x,y+3,6,3,RGB(30,30,34));lf(x+5,y+2,3,3,RGB(40,40,44));lf(x+7,y+3,1,1,RGB(150,150,150));lf(x+3,y+5,2,1,RGB(240,240,240));lf(x+5,y+5,1,1,RGB(60,170,70));lf(x+5,y+1-w,1,1,RGB(30,30,34));lf(x+7,y+1,1,1,RGB(30,30,34));lf(x,y+2-w,1,1,RGB(30,30,34));}}
static const char*const FVN[4]={"ADULT","CHILD","CAT","SCOUT THE DOG"};
// ---------------------------------------------------- ladder truck rescue (side view)
static int fire_ladder(int row){int au=FR_AUTO;int wy=row<=1?30:62,wx=150+(row*37%3)*24;int ang=30,len=40,phase=0,pt=0,t=0;
 fr_vstop();for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(20+y/10,24+y/12,50+y/6));m3_fill(130,16,104,124,RGB(150,74,60));
 for(int r=0;r<3;r++)for(int c=0;c<4;c++){int x=140+c*24,y=26+r*32;m3_fill(x,y,14,16,(x==wx-4&&y==wy-4)?RGB(255,200,90):RGB(60,70,90));}
 for(int i=0;i<10;i++)m3_fill(140+(i*13)%90,20+(i*29)%110,3,3,RGB(70,70,74));m3_fill(0,140,240,20,RGB(56,56,62));
 m3_fill(10,124,80,14,RGB(200,24,24));m3_fill(10,129,80,2,RGB(250,250,250));m3_fill(70,114,18,10,RGB(200,24,24));m3_fill(72,116,8,5,RGB(140,190,230));m3_fill(18,136,10,6,RGB(20,20,20));m3_fill(66,136,10,6,RGB(20,20,20));
 m3_text(14,146,"LADDER 1",C_WHITE,1);fr_save_bg();fr_vstart();float bx=40,by=120;
 for(;;t++){BEAT->screen=SCR_FLAD;BEAT->phase=phase;BEAT->x=ang;BEAT->y=len;
  float a=ang*FPI/180;float sway=(phase==0)?fsin(t*0.07f)*1.2f:0;float tx=bx+fcos(a)*len,ty=by-fsin(a)*len+sway;float ex=wx+3-tx,ey=wy+12-ty;int ok=ex*ex+ey*ey<36;
  if(phase==0){int U=0,D=0,L=0,R=0,A=0;
   if(au){float want=fatan2(by-(wy+12),wx+3-bx)*180/FPI,wl=fx_sqrt((wx+3-bx)*(wx+3-bx)+(by-wy-12)*(by-wy-12));U=ang<want-0.5f;D=ang>want+0.5f;R=len<wl-1;L=len>wl+1;A=ok&&t>30;}
   else{U=keys&K_UP;D=keys&K_DOWN;L=keys&K_LEFT;R=keys&K_RIGHT;A=pressed&K_A;}
   if(U&&ang<80)ang++;if(D&&ang>10)ang--;if(R&&len<190)len+=2;if(L&&len>30)len-=2;if(A&&ok){phase=1;pt=0;sfx_ok();}}
  else if(++pt>150){fr_vstop();return 1;}
  next_frame();fr_vrestore();
  for(int k=0;k<len;k+=4){int x=(int)(bx+fcos(a)*k),y=(int)(by-fsin(a)*k+sway*k/len);lf(x,y,2,2,RGB(230,230,230));lf(x+2,y+2,1,1,RGB(150,150,150));}
  lf((int)tx-1,(int)ty-1,3,3,ok?RGB(80,255,80):RGB(255,230,60));
  if(phase==0){int w=(t>>4)&1;lf(wx,wy+4,6,6,RGB(240,200,160));lf(wx-2+w*8,wy+2,2,4,RGB(240,200,160));ltc(4,"UP/DOWN ANGLE  LEFT/RIGHT EXTEND",C_WHITE);ltc(12,ok?"IN POSITION - PRESS A TO CLIMB":"PUT THE TIP AT THE WINDOW",ok?C_GREEN:C_YEL);}
  else{float f=pt<75?pt/75.0f:(150-pt)/75.0f;int x=(int)(bx+fcos(a)*len*f),y=(int)(by-fsin(a)*len*f);lf(x-2,y-8,5,7,RGB(196,170,96));lf(x-2,y-10,5,2,RGB(20,20,22));if(pt>=75)lf(x+3,y-9,4,6,RGB(60,110,200));ltc(8,pt<75?"CLIMBING...":"BRINGING THEM DOWN SAFELY",C_YEL);}
  if(au)ltx(206,150,"AUTO",C_BLUE);}}
// ---------------------------------------------------- extrication (Jaws of Life)
static int fire_extric(void){int au=FR_AUTO,bad=vb_fail();int step=0,cnt=0,bar=0,t=0,miss=0,hold=0;const char*msg="";int mt=0;
 static const char*const ST[5]={"1 STABILIZE THE CAR (TAP B x4)","2 CUT THE A-PILLAR (A IN GREEN)","3 CUT THE B-PILLAR (A IN GREEN)","4 SPREAD THE DOOR (TAP A FAST)","5 COLLAR + BACKBOARD (HOLD A)"};
 fr_vstop();for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(30+y/8,40+y/8,70));m3_fill(0,112,240,48,RGB(56,56,62));for(int x=0;x<240;x+=24)m3_fill(x,134,12,2,RGB(220,200,60));
 m3_fill(60,78,120,30,RGB(40,90,180));m3_fill(84,62,70,18,RGB(40,90,180));m3_fill(90,65,26,13,RGB(140,190,230));m3_fill(122,65,26,13,RGB(140,190,230));m3_fill(160,84,22,22,RGB(30,60,130));m3_fill(172,80,10,6,RGB(90,90,96));
 m3_fill(72,104,22,12,RGB(20,20,20));m3_fill(146,104,22,12,RGB(20,20,20));m3_fill(126,68,8,7,RGB(240,200,160));m3_fill(124,75,12,3,RGB(60,110,200));
 m3_fill(196,96,40,16,RGB(240,240,240));m3_fill(196,101,40,2,RGB(220,30,30));m3_text(198,88,"MEDIC 1",C_WHITE,1);m3_fill(0,0,240,10,RGB(10,10,20));fr_save_bg();fr_vstart();int f0=frame;
 for(;;t++){BEAT->screen=SCR_FEXT;BEAT->phase=step;BEAT->scoreA=miss;
  int p=(t*3)%200;if(p>100)p=200-p;int g0=step==1?38:44,g1=step==1?62:58;
  int A=0,Ah=0,Bp=0;if(au&&!bad){Bp=step==0&&(t%8==0);A=(step==1||step==2)?(p>=g0+3&&p<=g1-3):(step==3?(t%3==0):0);Ah=step==4;}else if(au&&bad){A=step>=1&&(t%37==0);}else{A=pressed&K_A;Ah=keys&K_A;Bp=pressed&K_B;}
  if(step==0&&Bp){cnt++;sfx_menu();if(cnt>=4){step=1;cnt=0;fi_msg(&msg,&mt,"CRIBBING IN - CAR IS STABLE",70);}}
  else if((step==1||step==2)&&A){if(p>=g0&&p<=g1){sfx_shot(1);shake(6);step++;fi_msg(&msg,&mt,"CLEAN CUT!",50);}else{miss++;sfx_hurt();fi_msg(&msg,&mt,"TOOL SLIPPED - RESET THE BLADES",50);}}
  else if(step==3){if(A){bar+=8;sfx1(0,0x6,1,1,900+bar*4,2);}if(t&1)bar--;if(bar<0)bar=0;if(bar>=100){step=4;bar=0;fi_msg(&msg,&mt,"DOOR IS OFF!",60);}}
  else if(step==4){if(Ah)hold++;else if(hold>0)hold-=2;if(hold>=90){sfx_ok();break;}}
  if(au&&bad&&t>900){fr_vstop();return -1;}
  next_frame();fr_vrestore();
  ltx(4,1,"HIGHWAY CRASH - JAWS OF LIFE",C_YEL);for(int i=0;i<5;i++)ltx(8,14+i*9,ST[i],i<step?C_GREEN:i==step?C_WHITE:C_GRAY);
  if(step==0)for(int i=0;i<cnt;i++)lf(70+i*28,112,10,6,RGB(200,160,90));else{lf(70,112,10,6,RGB(200,160,90));lf(98,112,10,6,RGB(200,160,90));lf(126,112,10,6,RGB(200,160,90));lf(154,112,10,6,RGB(200,160,90));}
  if(step>=2)lf(84,62,4,18,RGB(30,30,34));if(step>=3)lf(118,62,4,18,RGB(30,30,34));
  if(step==1||step==2){lf(20,140,200,6,RGB(40,40,50));lf(20+g0*2,140,(g1-g0)*2,6,RGB(40,180,60));lf(20+p*2-1,138,3,10,RGB(255,255,255));}
  if(step==3){lf(20,140,200,6,RGB(40,40,50));lf(20,140,bar*2,6,RGB(255,180,40));}
  if(step==4){lf(20,140,200,6,RGB(40,40,50));lf(20,140,hold*200/90,6,RGB(80,200,255));ltc(130,"PATIENT IS AWAKE AND TALKING",C_WHITE);}
  if(mt>0){mt--;ltc(118,msg,C_YEL);}if(au)ltx(206,150,bad?"FAIL":"AUTO",C_BLUE);}
 fr_vstop();int secs=(frame-f0)/60;
 for(int t2=0;t2<120;t2++){BEAT->phase=9;if(t2==0){fr_bgspace();fr_panel(16,40,208,70);m3_ctext(52,"PATIENT OUT AND STABLE",C_GREEN,1);m3_ctext(66,"MEDIC 1 IS TRANSPORTING.",C_WHITE,1);m3_ctext(80,"GREAT TEAMWORK, FIREFIGHTER.",C_YEL,1);m3_present();}next_frame();if(t2>20&&(pressed&K_A))break;}
 return secs+miss*5;}
// ---------------------------------------------------- the response drive (auto-drive, A = air horn)
static void fire_drive(int m){fr_vstop();int au=FR_AUTO;
 for(int y=0;y<160;y++)m3_fill(0,y,240,1,y<90?RGB(60+y,90+y/2,150):RGB(56,56,62));for(int i=0;i<9;i++){int x=i*28,h=20+(i*37)%40;m3_fill(x,90-h,24,h,RGB(90+(i*20)%60,80,90));for(int j=0;j<h-6;j+=8)m3_fill(x+4,96-h+j,4,4,RGB(250,230,140));}
 m3_fill(0,92,240,2,RGB(150,150,150));m3_ctext(4,FIM[m].name,C_YEL,1);fr_save_bg();fr_vstart();int carX[3]={200,260,330};
 for(int t=0;t<200;t++){BEAT->screen=SCR_FDRV;next_frame();fr_vrestore();if((pressed&(K_START|K_B))&&t>10)break;if(au&&t>120)break;
  for(int x=-(t*6)%40;x<240;x+=40)lf(x,124,20,2,RGB(240,230,120));
  for(int i=0;i<3;i++){int cx=carX[i]-t*2;if(cx<-40)continue;int cy=cx<150?112:104;lf(cx,cy,26,10,i==1?RGB(60,160,80):RGB(80,80,180));lf(cx+4,cy-5,16,5,RGB(140,190,230));}
  int ex=40;lf(ex,110,64,18,RGB(200,24,24));lf(ex+48,100,16,12,RGB(200,24,24));lf(ex+51,102,9,6,RGB(140,190,230));lf(ex,116,64,2,RGB(250,250,250));lf(ex+4,104,40,2,RGB(200,200,210));
  lf(ex+50,97,10,3,(t&8)?RGB(255,40,40):RGB(60,90,255));lf(ex+8,126,10,6,RGB(20,20,20));lf(ex+44,126,10,6,RGB(20,20,20));ltx(ex+8,110,"ENGINE 1",C_WHITE);
  if((au&&t==40)||(pressed&K_A)){sfx1(0,0xC,2,2,700,20);}
  ltc(140,"CARS PULL OVER FOR LIGHTS + SIREN",C_WHITE);ltc(150,"A: AIR HORN   START: ARRIVE",C_GRAY);
  if((t&31)==0)sfx1((2<<4)|(0<<3)|3,0x7,1,2,1500,8);}
 fr_vstop();}
// ---------------------------------------------------- main scene
typedef struct{int ok,secs,saved,total,prop,stars,score,why;}FRes;
static FRes fire_scene(int m){const FiMis*M=&FIM[m];FRes R={0};fi_load(m);int au=FR_AUTO,bad=vb_fail();
 int px=fTr[0][0],py=fTr[1][0],dir=0,mv=0,ox=px,oy=py,steps=6,carry=-1,air=10000,act=0,actT=0,end=0,endT=0,saved=0,total=0,f0=frame,warn=0,lowAir=0,flashes=0,kickT=0,aiWait=0;const char*msg=M->hint;int mt=200;
 for(int i=0;i<fNV;i++)total++;int hasHyd=0,hasVent=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++){hasHyd|=fT[y][x]==FT_HYD;hasVent|=fT[y][x]==FT_VENT;}
 int limit=M->par*2+60;
 fr_vstop();m3_fill(0,0,240,18,RGB(12,12,20));m3_fill(0,154,240,6,RGB(12,12,20));for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fi_tile(x,y);fr_save_bg();fr_vstart();bgm_t(TR_FIRE);
 for(int fr=0;;fr++){int secs=(frame-f0)/60;
  BEAT->screen=SCR_FIRE;BEAT->mode=m;BEAT->phase=end?(end>0?3:4):1;BEAT->x=px;BEAT->y=py;BEAT->hp=air/100;BEAT->kills=saved;BEAT->scoreA=fi_burning();BEAT->scoreB=total;BEAT->timer=secs;BEAT->extra=fTank|(fHyd<<15);
  if(!end){
   int U=0,Dn=0,L=0,Rt=0,A=0,Bh=0,tog=0,psi=0;
   if(au&&!bad&&mv==0&&(act||aiWait<=0||--aiWait<=0)){int fd=-1,st=-1;aiWait=0;
    if(act){Bh=1;}
    else{
     if(fTyp==0&&air<3200&&carry<0){st=fi_bfs(px,py,G_TYPE,FT_ENG,&fd);}
     if(st<0&&carry>=0)st=fi_bfs(px,py,G_OUT,0,&fd);
     if(st<0&&hasHyd&&!fHyd)st=fi_bfs(px,py,G_TYPE,FT_HYD,&fd);
     if(st<0&&hasVent&&!fVent){int lk=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)lk|=fT[y][x]==FT_LOCK;if(lk||fr>600)st=fi_bfs(px,py,G_TYPE,FT_VENT,&fd);}
     if(st<0&&(fTank>0||fHyd)){int st2=fi_bfs(px,py,G_SPRAY,0,&fd);int nb=fi_burning();if(st2==4||nb>6||fTyp)st=st2;}
     if(st<0)st=fi_bfs(px,py,G_VIC,0,&fd);
     if(st<0)st=fi_bfs(px,py,G_TYPE,FT_WIN,&fd);
     if(st<0&&(fTank>0||fHyd))st=fi_bfs(px,py,G_SPRAY,0,&fd);
     if(st<0&&fTyp==1)st=fi_bfs(px,py,G_DIG,0,&fd);
     if(st==4){if(fd>=0&&fd!=dir){dir=fd;}else if(fd>=0){int nx=px+FDX[dir],ny=py+FDY[dir];int t=fT[ny][nx];if(fi_ray(px,py,dir,1+fPsi)&&!(t==FT_HYD||t==FT_ENG||t==FT_VENT||t==FT_WIN||fi_vicat(nx,ny)>=0))A=1;else Bh=1;}}
     else if(st<0)aiWait=10;
     else if(st>=0){int nx=px+FDX[st],ny=py+FDY[st];if(fT[ny][nx]==FT_LOCK){dir=st;Bh=1;}else{U=st==0;Rt=st==1;Dn=st==2;L=st==3;}}}
    if(fr%600==5&&fTyp==0)tog=fTherm==0;}
   else if(au&&bad){U=(fr/90)&1;}
   else if(!au){U=keys&K_UP;Dn=keys&K_DOWN;L=keys&K_LEFT;Rt=keys&K_RIGHT;A=keys&K_A;Bh=keys&K_B;tog=pressed&K_R;psi=pressed&K_L;
    if(pressed&K_START){pressed=0;fr_vstop();int q=0;for(int t=0;;t++){if(t==0){fr_panel(50,60,140,40);m3_ctext(68,"LEAVE THE SCENE?",C_YEL,1);m3_ctext(84,"A: YES   B: NO",C_WHITE,1);m3_present();}next_frame();if(pressed&K_A){q=1;break;}if(pressed&K_B)break;}
     pressed=0;if(q){R.why=3;end=-1;endT=fr;}for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fK[y][x]=0xFFFF;m3_fill(0,0,240,18,RGB(12,12,20));for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fi_tile(x,y);fr_save_bg();fr_vstart();}}
   if(tog){fTherm^=1;sfx_menu();fi_msg(&msg,&mt,fTherm?"THERMAL CAMERA ON - HEAT AND PEOPLE GLOW":"THERMAL CAMERA OFF",60);}
   if(psi){fPsi=fPsi%3+1;sfx_menu();fi_msg(&msg,&mt,fPsi==1?"PUMP 100 PSI: SHORT REACH":fPsi==2?"PUMP 150 PSI: GOOD STREAM":"PUMP 200 PSI: LONG REACH, HOSE KICKS",60);}
   // movement on the grid
   if(mv>0)mv--;
   else{int d=U?0:Rt?1:Dn?2:L?3:-1;
    if(d>=0){dir=d;int nx=px+FDX[d],ny=py+FDY[d];
     if(fi_walk(nx,ny)){ox=px;oy=py;px=nx;py=ny;steps=carry>=0?10:6;if(fS[py][px]>150)steps+=2;mv=steps;
      if(fT[py][px]==FT_DOOR){fT[py][px]=FT_OPEN;sfx_menu();}
      if(fTrN>=2&&fTr[0][fTrN-2]==px&&fTr[1][fTrN-2]==py)fTrN--;else if(fTrN<120){fTr[0][fTrN]=px;fTr[1][fTrN]=py;fTrN++;}
      if(carry>=0&&fi_outside(fT[py][px])&&fT[py][px]!=FT_GRASS){fV[carry].st=2;saved++;FS.rescued++;sfx_ok();sfx_laser();fi_msg(&msg,&mt,cat(FVN[fV[carry].k&7]," RESCUED! HANDED TO MEDIC 1"),120);carry=-1;}}
     else if(fi_in(nx,ny)&&fH[ny][nx]>=10&&!mt)fi_msg(&msg,&mt,"TOO HOT! KNOCK IT DOWN FIRST",40);}}
   if(carry>=0&&mv==0&&fi_outside(fT[py][px])&&fT[py][px]!=FT_GRASS){fV[carry].st=2;saved++;FS.rescued++;sfx_ok();sfx_laser();fi_msg(&msg,&mt,cat(FVN[fV[carry].k&7]," RESCUED! HANDED TO MEDIC 1"),120);carry=-1;}
   // B: interact (hold)
   {int nx=px+FDX[dir],ny=py+FDY[dir];int t=fi_in(nx,ny)?fT[ny][nx]:FT_WALL;int want=0,need=0;
    if(Bh&&mv==0){int v=fi_vicat(nx,ny);
     if(v>=0&&carry<0&&!(fV[v].k&8)){fV[v].st=1;carry=v;sfx_ok();fi_msg(&msg,&mt,cat3("GOT THE ",FVN[fV[v].k&7],"! GET THEM OUT"),90);}
     else if(t==FT_LOCK){want=1;need=40;}
     else if(t==FT_HYD&&!fHyd){want=2;need=50;}
     else if(t==FT_VENT&&!fVent){want=3;need=60;}
     else if(t==FT_ENG&&fTyp==0&&air<9000){want=4;need=60;}
     else if(t==FT_WIN&&carry<0){for(int i=0;i<fNV;i++)if(fV[i].st==0&&fV[i].x==nx&&fV[i].y==ny){fr_vstop();fire_ladder(ny);fV[i].st=2;saved++;FS.rescued++;fT[ny][nx]=FT_WALL;fi_msg(&msg,&mt,"LADDER RESCUE COMPLETE!",100);
       for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fK[y][x]=0xFFFF;m3_fill(0,0,240,18,RGB(12,12,20));m3_fill(0,154,240,6,RGB(12,12,20));for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fi_tile(x,y);fr_save_bg();fr_vstart();bgm_t(TR_FIRE);break;}}
     else if(fTyp==1&&t==FT_GRASS&&!fH[ny][nx]){want=5;need=24;}}
    if(want&&want==act){actT++;if(actT%8==0)sfx_shot(0);if(actT>=need){
      if(act==1){int bx=nx+FDX[dir],by=ny+FDY[dir];fT[ny][nx]=FT_OPEN;fFu[ny][nx]=0;int r=fi_in(bx,by)?fR[by][bx]:0;int sm=0,hn=0,cn=0;
       for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)if(r&&fR[y][x]==r){sm+=fS[y][x];hn+=fH[y][x]>0;cn++;}
       if(cn&&sm/cn>140&&hn&&!fVent){shake(16);sfx_boom();air-=800;for(int k=0;k<3;k++){int qx=nx-FDX[dir]*k,qy=ny-FDY[dir]*k;if(fi_in(qx,qy)&&fFu[qy][qx])fH[qy][qx]=6;}
        int bx2=px-FDX[dir],by2=py-FDY[dir];if(fi_walk(bx2,by2)){px=bx2;py=by2;}fi_msg(&msg,&mt,"BACKDRAFT! VENT THE ROOF BEFORE FORCING DOORS",150);
        for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)if(fR[y][x]==r)fS[y][x]/=2;}
       else fi_msg(&msg,&mt,"DOOR FORCED WITH THE HALLIGAN",60);}
      else if(act==2){fHyd=1;fi_msg(&msg,&mt,"SUPPLY LINE CHARGED - HYDRANT WATER!",90);sfx_ok();}
      else if(act==3){fVent=1;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fS[y][x]/=2;fi_msg(&msg,&mt,"ROOF VENTED - SMOKE IS LIFTING",90);sfx_ok();}
      else if(act==4){air=10000;fi_msg(&msg,&mt,"FRESH SCBA BOTTLE",60);sfx_ok();}
      else if(act==5){fT[ny][nx]=FT_BREAK;fFu[ny][nx]=0;}
      act=0;actT=0;}}
    else{act=want;actT=0;}
    if(act==1&&actT==1)fi_msg(&msg,&mt,"FORCING THE DOOR...",need);if(act==2&&actT==1)fi_msg(&msg,&mt,"HOOKING UP TO THE HYDRANT...",need);if(act==3&&actT==1)fi_msg(&msg,&mt,"LADDER CREW VENTING THE ROOF...",need);if(act==4&&actT==1)fi_msg(&msg,&mt,"SWAPPING SCBA BOTTLE...",need);}
   // A: spray
   int spr=0,shx=px,shy=py,shd=0;
   if(A&&mv==0&&(fTank>0||fHyd)&&carry<0){spr=1;int reach=1+fPsi;int k=fi_ray(px,py,dir,reach);int lat=0;if(fPsi==3&&(fr&63)<12){lat=(fr&64)?1:-1;if(!kickT){kickT=90;fi_msg(&msg,&mt,"HOSE KICK! LOWER THE PSI WITH L",60);}}
    shd=k?k:reach;if(fr%4==0){if(!fHyd){fTank-=fTyp==1?1:fPsi;if(fTank<0)fTank=0;}
     if(k){int hx=px+FDX[dir]*k+(dir&1?0:lat),hy=py+FDY[dir]*k+(dir&1?lat:0);if(fi_in(hx,hy)){int h=fH[hy][hx]-(3+fPsi);fH[hy][hx]=h<0?0:h;fS[hy][hx]=fS[hy][hx]>20?fS[hy][hx]-20:0;}
      for(int s=-1;s<=1;s+=2){int qx=px+FDX[dir]*k+(dir&1?0:s),qy=py+FDY[dir]*k+(dir&1?s:0);if(fi_in(qx,qy)&&fH[qy][qx])fH[qy][qx]--;}}}
    if(fr%10==0)sfx1(0,0x4,1,0,300+rr(80),4);}
   if(kickT)kickT--;
   if(!fHyd&&fTank==0&&A&&!mt)fi_msg(&msg,&mt,fTyp==1?"TANK EMPTY - DIG A FIREBREAK WITH B":"TANK EMPTY - HOOK UP TO THE HYDRANT!",80);
   // sim + hazards
   if(fr%(fTyp==1?10:8)==0)fi_sim();
   for(int i=0;i<fNV;i++)if(fV[i].st==0&&!(fV[i].k&8)){fH[fV[i].y][fV[i].x]=0;}
   if(fTyp==0){int inside=fR[py][px]||fT[py][px]==FT_OPEN||fT[py][px]==FT_DOOR;if(inside||fS[py][px]>40){air-=carry>=0?2:1;if(fH[py][px]>3)air-=2;}
    if(air<3300){if(!lowAir){lowAir=1;fi_msg(&msg,&mt,"LOW AIR ALARM - HEAD FOR THE ENGINE",120);}if(fr%40==0)sfx1(0,0xA,1,2,1800,6);}else lowAir=0;
    if(air<=0){end=-1;R.why=1;endT=fr;}}
   if(fTyp==0&&fr%30==0){int anyw=0;for(int r=1;r<=fNR;r++){int sh=0,ss=0,cn=0;for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)if(fR[y][x]==r){sh+=fH[y][x];ss+=fS[y][x];cn++;}if(!cn)continue;
     if(sh/cn>=5&&ss/cn>=140){if(fFo[r]<255)fFo[r]++;}else if(fFo[r])fFo[r]--;
     if(fFo[r]>=4)anyw=1;
     if(fFo[r]>=10){fFo[r]=0;flashes++;shake(20);sfx_boom();for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)if(fR[y][x]==r&&fFu[y][x]){fH[y][x]=14;fS[y][x]=255;}
      if(fR[py][px]==r||fH[py][px]>=10){
       for(int y2=0;y2<FH;y2++)for(int x2=0;x2<FW;x2++)if(fT[y2][x2]==FT_EXIT){int nx=x2,ny=y2+1;if(fi_in(nx,ny)&&fi_walk(nx,ny)){px=nx;py=ny;y2=FH;break;}}
       air-=1500;if(carry>=0){fV[carry].st=2;saved++;carry=-1;}fTrN=1;fi_msg(&msg,&mt,"FLASHOVER! YOU BAILED OUT THE NEAREST EXIT",150);}
      else fi_msg(&msg,&mt,"FLASHOVER! THE ROOM IS FULLY INVOLVED",120);}}
    if(anyw&&!warn){warn=1;fi_msg(&msg,&mt,"FLASHOVER WARNING! COOL THE ROOM - GET LOW",120);}if(!anyw)warn=0;if(anyw&&fr%60==0)sfx1(0,0xB,1,2,1600,10);}
   // win / time
   if(fr%30==0){int nb=fi_burning();int left=0;for(int i=0;i<fNV;i++)left+=fV[i].st!=2;if(nb==0&&left==0&&carry<0){end=1;endT=fr;}}
   if(secs>limit&&!end){end=-1;R.why=2;endT=fr;}
   if(end){if(end>0)music_victory();else music_defeat();}}
  else if(fr-endT>150)break;
  next_frame();fr_vrestore();
  // draw dynamic layer
  for(int y=0;y<FH;y++)for(int x=0;x<FW;x++)fi_tile(x,y);
  for(int i=1;i<fTrN;i++)lf(fTr[0][i]*8+3,18+fTr[1][i]*8+3,2,2,RGB(240,220,80));
  for(int i=0;i<fNV;i++){FVic*v=&fV[i];if(v->st!=0)continue;int vis=(v->k&8)||fTherm||fS[v->y][v->x]<110||(iabs(v->x-px)<=1&&iabs(v->y-py)<=1);
   if(v->k&8){int w=(fr>>4)&1;lf(v->x*8+2,18+v->y*8+1,4,4,RGB(240,200,160));lf(v->x*8+(w?0:6),18+v->y*8,2,3,RGB(240,200,160));}
   else if(vis){if(fTherm){lf(v->x*8+1,18+v->y*8+1,6,6,RGB(255,255,255));}else fi_vic(v->x*8,18+v->y*8,v->k&7,fr);}}
  {int sx=px*8-(px-ox)*mv*8/(steps?steps:1),sy=18+py*8-(py-oy)*mv*8/(steps?steps:1);fi_ff(sx,sy,dir,carry>=0?fV[carry].k&7:-1,fr);
   }
  // HUD
  ltx(2,1,fHyd?"WATER: HYDRANT":cat3("WATER ",itoa(fTank)," GAL"),fHyd?C_GREEN:fTank<100?C_RED:C_WHITE);ltx(104,1,cat("PSI ",itoa(50+fPsi*50)),C_WHITE);
  {int s=secs;char tb[12];char*o=sp_int(tb,s/60);*o++=':';*o++='0'+(s%60)/10;*o++='0'+s%10;*o=0;ltx(150,1,tb,secs>M->par?C_RED:C_WHITE);}
  if(total){char sb[16];char*o=sp_put(sb,"SAVED ");o=sp_int(o,saved);*o++='/';sp_int(o,total);ltx(180,1,sb,saved==total?C_GREEN:C_YEL);}
  if(fTyp==0){lf(2,11,62,5,RGB(40,40,50));lf(3,12,air*60/10000>0?air*60/10000:0,3,air<3300?((fr&8)?RGB(255,60,60):RGB(120,20,20)):RGB(80,200,255));ltx(68,10,"SCBA AIR",C_GRAY);}
  else if(fTyp==1)ltx(2,10,fWind?"WIND: STRONG":"",C_GRAY);
  ltx(124,10,cat("FIRE ",itoa(fi_burning())),C_GRAY);if(fTherm)ltx(170,10,"THERMAL",RGB(255,150,40));if(fVent)ltx(214,10,"VENT",C_GREEN);
  if(mt>0){mt--;ltc(156-8,msg,C_YEL);}
  if(end)ltc(70,end>0?"FIRE UNDER CONTROL!":R.why==1?"MAYDAY! YOUR CREW BRINGS YOU OUT SAFE":R.why==2?"COMMAND: DEFENSIVE MODE":"LEFT THE SCENE",end>0?C_GREEN:C_RED);
  if(au)ltx(214,148,bad?"FAIL":"AUTO",C_BLUE);
  fsp_run();
  if(fiFull){fiFull=0;dma32(fb,FRV,240*160/2);}}
 fr_vstop();R.ok=end>0;R.secs=(frame-f0)/60;R.saved=saved;R.total=total;R.prop=fi_prop();(void)flashes;return R;}
// ---------------------------------------------------- dedication, station hub, gear locker, tips, records
static void fi_helmet(int x,int y,int s){ // classic fire helmet, front view, generic shield
 m3_fill(x+2*s,y+6*s,20*s,4*s,RGB(20,20,22));m3_fill(x+5*s,y,14*s,8*s,RGB(20,20,22));m3_fill(x+0,y+9*s,24*s,2*s,RGB(20,20,22));
 m3_fill(x+9*s,y+1*s,6*s,7*s,RGB(236,196,92));m3_fill(x+10*s,y+2*s,4*s,4*s,RGB(200,30,30));m3_fill(x+11*s,y+3*s,2*s,2*s,RGB(236,196,92));}
static void fire_dedication(void){int au=FR_AUTO;
 for(int t=0;;t++){BEAT->screen=SCR_FDED;if(t==0){for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(6+y/16,4,4));fi_helmet(96,18,2);
   m3_ctext(54,"FIREFIGHTER",RGB(255,140,40),2);m3_ctext(80,"DEDICATED TO MY DAD,",C_WHITE,1);m3_ctext(92,"A REAL HERO.",C_YEL,1);m3_ctext(108,"- LT",C_WHITE,1);m3_ctext(140,"A: CONTINUE",C_GRAY,1);m3_present();music_jingle(TR_CLEAR);}
  next_frame();if((t>30&&(pressed&(K_A|K_START|K_B)))||(au&&t>90)||t>600){pressed=0;return;}}}
static void fi_truck(int x,int y,int k){ // side views for the station: 0 engine, 1 ladder truck, 2 ambulance
 if(k==2){m3_fill(x,y+6,48,22,RGB(240,240,240));m3_fill(x+36,y+10,12,8,RGB(140,190,230));m3_fill(x,y+16,48,3,RGB(220,30,30));m3_fill(x+14,y+8,8,8,RGB(220,30,30));m3_fill(x+16,y+6,4,12,RGB(220,30,30));m3_fill(x+12,y+10,12,4,RGB(220,30,30));}
 else{m3_fill(x,y+8,52,20,RGB(200,24,24));m3_fill(x+38,y+2,14,10,RGB(200,24,24));m3_fill(x+41,y+4,9,6,RGB(140,190,230));m3_fill(x,y+18,52,2,RGB(250,250,250));m3_fill(x+40,y,10,2,RGB(255,60,60));
  if(k==1){m3_fill(x-6,y+4,44,2,RGB(210,210,220));m3_fill(x-6,y+8,44,1,RGB(210,210,220));for(int i=0;i<44;i+=4)m3_fill(x-6+i,y+4,1,5,RGB(210,210,220));}else{for(int i=0;i<3;i++)m3_fill(x+4+i*11,y+10,9,6,RGB(170,20,20));}}
 m3_fill(x+6,y+26,9,6,RGB(20,20,20));m3_fill(x+36,y+26,9,6,RGB(20,20,20));}
static void fi_fighter(int x,int y,int s){ // firefighter in full gear (front view)
 m3_fill(x+3*s,y,6*s,2*s,RGB(20,20,22));m3_fill(x+1*s,y+2*s,10*s,1*s,RGB(20,20,22));m3_fill(x+4*s,y+3*s,4*s,3*s,RGB(240,200,160));m3_fill(x+4*s,y+4*s,4*s,2*s,RGB(90,90,100));
 m3_fill(x+2*s,y+6*s,8*s,9*s,RGB(196,170,96));m3_fill(x+2*s,y+11*s,8*s,1*s,RGB(230,240,120));m3_fill(x+2*s,y+12*s,8*s,1*s,RGB(200,200,200));m3_fill(x+5*s,y+6*s,2*s,9*s,RGB(170,140,70));
 m3_fill(x,y+6*s,2*s,7*s,RGB(196,170,96));m3_fill(x+10*s,y+6*s,2*s,7*s,RGB(196,170,96));m3_fill(x,y+13*s,2*s,2*s,RGB(40,40,40));m3_fill(x+10*s,y+13*s,2*s,2*s,RGB(40,40,40));
 m3_fill(x+2*s,y+15*s,3*s,6*s,RGB(196,170,96));m3_fill(x+7*s,y+15*s,3*s,6*s,RGB(196,170,96));m3_fill(x+2*s,y+18*s,8*s,1*s,RGB(230,240,120));m3_fill(x+2*s,y+21*s,3*s,2*s,RGB(20,20,22));m3_fill(x+7*s,y+21*s,3*s,2*s,RGB(20,20,22));}
static void fire_station_bg(void){for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(60+y/4,110+y/6,170));m3_fill(0,20,240,80,RGB(150,60,48));for(int y=24;y<100;y+=6)for(int x=(y/6&1)*8;x<240;x+=16)m3_fill(x,y,1,5,RGB(120,46,36));
 for(int y=20;y<100;y+=6)m3_fill(0,y,240,1,RGB(120,46,36));m3_fill(60,8,120,14,RGB(236,226,200));m3_ctext(11,"FIRE STATION 1",RGB(160,20,20),1);
 static const char*const LB[3]={"ENGINE 1","LADDER 1","MEDIC 1"};for(int i=0;i<3;i++){int x=10+i*76;m3_fill(x,40,68,60,RGB(30,30,34));m3_fill(x,40,68,6,RGB(160,160,166));fi_truck(x+8,62,i);m3_text(x+10,30,LB[i],C_WHITE,1);}
 m3_fill(0,100,240,60,RGB(120,120,124));m3_fill(0,100,240,2,RGB(90,90,94));}
static void fire_gear(void){for(int t=0;;t++){BEAT->screen=SCR_FGEAR;if(t==0){fr_bgspace();fr_panel(6,6,228,148);m3_ctext(10,"GEAR LOCKER",C_YEL,2);fi_fighter(16,34,4);
   static const char*const G[10]={"HELMET WITH FACE SHIELD","FLASH HOOD (FIRE RESISTANT)","TURNOUT COAT + PANTS","REFLECTIVE TRIM STRIPES","GLOVES AND RUBBER BOOTS","SCBA: AIR BOTTLE + MASK","PASS ALARM (MOTION SENSOR)","HALLIGAN BAR + FLAT AXE","THERMAL IMAGING CAMERA","RADIO ON THE SHOULDER"};
   for(int i=0;i<10;i++)m3_text(76,34+i*10,G[i],C_WHITE,1);m3_text(76,138,"A FULL SET WEIGHS ABOUT 45+ LB",C_GRAY,1);m3_present();}
  next_frame();if(t>20&&(pressed&(K_A|K_B)))return;if(FR_AUTO&&t>60)return;}}
static void fire_tip(int k,int auto_t){for(int t=0;;t++){BEAT->screen=SCR_FTIP;BEAT->menu=k;
  if(t==0){for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(40+y/10,16,12));fr_panel(14,16,212,128);m3_ctext(22,"FIRE SAFETY TIP",RGB(255,140,40),1);m3_ctext(38,FTIP[k][0],C_YEL,1);
   for(int i=1;i<5;i++)m3_ctext(56+i*12-12,FTIP[k][i],C_WHITE,1);fi_fighter(206,108,1);m3_ctext(122,"IN AN EMERGENCY CALL 911.",RGB(255,140,40),1);m3_ctext(134,auto_t?"A: CONTINUE":"LEFT/RIGHT: MORE TIPS  B: BACK",C_GRAY,1);m3_present();}
  next_frame();if(auto_t&&((t>30&&(pressed&(K_A|K_B)))||(FR_AUTO&&t>auto_t)))return;
  if(!auto_t){if(pressed&K_RIGHT){k=(k+1)%10;t=-1;sfx_menu();}if(pressed&K_LEFT){k=(k+9)%10;t=-1;sfx_menu();}if(pressed&K_B)return;if(FR_AUTO&&t>60)return;}}}
static void rec_fire(const char**L){fs_load();static char b[5][40] EWRAM;int st=0;for(int i=0;i<10;i++)st+=FS.stars[i];
 char*o=sp_put(b[0],"STARS ");o=sp_int(o,st);o=sp_put(o,"/30  RESCUES ");sp_int(o,FS.rescued);L[0]=b[0];
 for(int i=0;i<3;i++){o=sp_put(b[i+1],FS.t[i].n);o=sp_put(o,"  ");sp_int(o,FS.t[i].s);L[i+1]=b[i+1];}L[4]="";}
static int fire_run(int m){const FiMis*M=&FIM[m];int au=FR_AUTO;
 // dispatch card
 for(int t=0;;t++){BEAT->screen=SCR_FDRV;BEAT->menu=m;if(t==0){for(int y=0;y<160;y++)m3_fill(0,y,240,1,RGB(30,8,8));fr_panel(10,14,220,130);m3_ctext(20,"DISPATCH - ALL HANDS",RGB(255,80,60),1);
   {char cb[48];char*o=sp_put(cb,"CALL ");o=sp_int(o,m+1);o=sp_put(o,": ");sp_put(o,M->name);m3_ctext(36,cb,C_YEL,1);}m3_ctext(56,M->call,C_WHITE,1);m3_ctext(68,M->addr,C_WHITE,1);m3_ctext(88,M->hint,RGB(110,200,230),1);
   m3_ctext(104,M->typ==1?"BRUSH TRUCK + HAND LINE":M->typ==3?"RESCUE TOOLS: JAWS OF LIFE":"ENGINE 1, LADDER 1, MEDIC 1",C_GRAY,1);m3_ctext(132,"A: RESPOND",C_GRAY,1);m3_present();
   for(int i=0;i<3;i++)sfx1(0,0xC,2,2,1200+i*200,10);}
  next_frame();if((t>20&&(pressed&(K_A|K_START)))||(au&&t>60)){pressed=0;break;}if(pressed&K_B){pressed=0;return -1;}}
 fire_drive(m);
 FRes R;memset(&R,0,sizeof R);
 if(M->typ==3){int s=fire_extric();R.ok=s>=0;R.secs=s<0?999:s;R.saved=R.ok;R.total=1;R.prop=100;}
 else R=fire_scene(m);
 int st=0;if(R.ok){st=(R.saved==R.total)+(R.secs<=M->par)+(R.prop>=M->prop);if(st==0)st=1;}
 int sc=R.ok?(R.saved*1000+R.prop*30+hmx(0,M->par-R.secs)*20+st*500):R.saved*300;
 FS.runs++;if(st>FS.stars[m])FS.stars[m]=st;if(R.ok&&(FS.best[m]==0||R.secs<FS.best[m]))FS.best[m]=R.secs;
 int rocks=mr_add(R.ok?20+st*15+m*3:5);
 int rk=hi_rank(FS.t,sc);if(rk>=0&&sc>0){char nm[4]="LTV";hi_initials(nm,cat("FIRE RECORD ",itoa(sc)));hi_put(FS.t,rk,sc,nm);}else rk=-1;
 fs_save();
 static char l1[40] EWRAM,l2[40] EWRAM;char*o=l1;if(R.total){o=sp_put(o,"SAVED ");o=sp_int(o,R.saved);o=sp_put(o,"/");o=sp_int(o,R.total);o=sp_put(o,"  ");}
 o=sp_put(o,"PROPERTY ");o=sp_int(o,R.prop);o=sp_put(o,"%  ");o=sp_int(o,R.secs);sp_put(o,"S");
 o=l2;o=sp_put(o,"STARS ");for(int i=0;i<3;i++)*o++=i<st?'*':'-';o=sp_put(o,"   +");o=sp_int(o,rocks);sp_put(o," MARS ROCKS");
 fr_result(R.ok?"FIRE UNDER CONTROL":"MISSION FAILED",l1,l2,R.ok,sc,rk==0);
 BEAT->scoreA=sc;BEAT->scoreB=st;BEAT->kills=R.saved;
 fire_tip(M->tip,120);return st;}
static void fire_mode(void){fs_load();fire_dedication();static int sel=0;int msel=0;
 for(;;){int redraw=1,go=-1;bgm_t(TR_FIRE);
  for(int t=0;;t++){BEAT->screen=SCR_FHUB;BEAT->menu=sel;
   {u32 j=FR_JUMP;if((j>>16)==0x4649){FR_JUMP=0;go=j&15;break;}}
   if(redraw){redraw=0;fire_station_bg();fi_fighter(206,104,2);static const char*const IT[5]={"RESPOND TO A CALL","GEAR LOCKER","FIRE SAFETY TIPS","RECORDS","BACK TO MENU"};
    fr_panel(8,104,190,52);for(int i=0;i<5;i++){m3_text(14,106+i*8,i==sel?">":" ",C_YEL,1);m3_text(22,106+i*8,IT[i],i==sel?C_YEL:C_WHITE,1);}
    int stt=0;for(int i=0;i<10;i++)stt+=FS.stars[i];m3_text(22,149,cat3("STARS ",itoa(stt),"/30"),C_YEL,1);mr_load();m3_text(100,149,cat("MARS ROCKS ",itoa(MR.bal)),RGB(230,120,80),1);m3_present();}
   next_frame();
   if(pressed&K_UP){sel=(sel+4)%5;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%5;redraw=1;sfx_menu();}if(pressed&K_B){pressed=0;return;}
   if(pressed&(K_A|K_START)){pressed=0;sfx_ok();if(sel==4)return;if(sel==1){fire_gear();redraw=1;}else if(sel==2){fire_tip(0,0);redraw=1;}
    else if(sel==3){for(int t2=0;;t2++){if(t2==0){fr_bgspace();fr_panel(6,6,228,148);m3_ctext(10,"FIRE RECORDS",C_YEL,2);for(int i=0;i<5;i++){m3_text(30,34+i*11,FS.t[i].n,C_WHITE,1);m3_text(60,34+i*11,itoa(FS.t[i].s),C_WHITE,1);}
       for(int i=0;i<10;i++){char b[24];char*o=sp_put(b,itoa(i+1));o=sp_put(o," ");for(int k=0;k<3;k++)*o++=k<FS.stars[i]?'*':'-';*o=0;m3_text(140,34+i*10,b,FS.stars[i]?C_YEL:C_GRAY,1);}
       m3_text(30,96,cat("RESCUES ",itoa(FS.rescued)),C_GREEN,1);m3_text(30,106,cat("CALLS RUN ",itoa(FS.runs)),C_WHITE,1);m3_present();}next_frame();if(t2>20&&(pressed&(K_A|K_B)))break;if(FR_AUTO&&t2>60)break;}redraw=1;}
    else{static char nb[10][34] EWRAM;const char*IN[10];for(int i=0;i<10;i++){char*o=sp_put(nb[i],itoa(i+1));o=sp_put(o,". ");o=sp_put(o,FIM[i].name);IN[i]=nb[i];}
     u8 dn[10];for(int i=0;i<10;i++)dn[i]=FS.stars[i]>0;int r=fr_menu("CALLS","STARS: PEOPLE SAVED, TIME, PROPERTY",IN,10,dn,&msel,SCR_FIRE+20);if(r>=0){go=r;break;}redraw=1;}}}
  if(go>=0){fire_run(go);if(FR_AUTO)continue;}}}
