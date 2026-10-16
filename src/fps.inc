#include "fpslut.inc"
// v1.3 FPS VS BOTS - mode 4 raycaster (120 columns x 2 px, 136-row view), ARM column loop in IWRAM
#define SCR_FPS 84
#define SCR_FPSEL 85
#define FP_OFF 0xB80
#define FVH 136
static const char FMAP_COMPOUND[24][25]={"########################","#P.....#........#......#","#......#..E.....D..E...#","#..BB..#........#......#","#..BB..###D######...H..#","#......................#","###D####..SS..SS..######","#......#..........#....#","#..E...#....E.....D..E.#","#......#..........#....#","#..A...####D#######....#","#......................#","#####..MMMMMM..MMMMMD###","#...#..M....M..M.......#","#.E.D..M..E.D..M...X...#","#...#..M....M..M.......#","#####..MMMMMM..MMMMMMMM#","#......................#","#..SS....E.......SS..W.#","#......................#","#..BBB......BBB.....E..#","#..B.........B.........#","#....H.................#","########################"};
static const char FMAP_BUNKER[24][25]={"MMMMMMMMMMMMMMMMMMMMMMMM","MP...M.....E....M.....XM","M....M..........M......M","M....D....MMMM..D...E..M","M....M....M..M..M......M","MMDMMM....M.HM..MMMMDMMM","M.........MDMM.........M","M..E...............E...M","M......MMMMMMMMMM......M","MMMMD.MM........MM.DMMMM","M.....M..E....A..M.....M","M..A..D..........D..E..M","M.....M..........M.....M","MMMMD.MM...E....MM.DMMMM","M......MMMMMMMMMM......M","M..E...................M","M........#####.........M","MMMDMM...#...#....MMDMMM","M....M...D.W.#....M....M","M.E..M...#...#....M..E.M","M....D...#####....D....M","M....M.......H....M....M","M....M............M....M","MMMMMMMMMMMMMMMMMMMMMMMM"};
static const char FMAP_VILLAGE[24][25]={"SSSSSSSSSSSSSSSSSSSSSSSS","S......................S","S.BBBB.....E.....BBBB..S","S.B..D...........D..B..S","S.B..B....SS.....B.EB..S","S.BBBB...........BBBB..S","S......................S","S...E.......H......E...S","S......BBBDBBB.........S","S......B.....B....SS...S","S..SS..B..X..B.........S","S......B.....B.........S","S......BBBBBBB...E.....S","S......................S","S.E........W...........S","S.....BBDBB.......BBDB.S","S.....B...B..A....B..B.S","S.....B.E.B.......B.EB.S","S.....BBBBB.......BBBB.S","S......................S","S..P.......E...........S","S..................H...S","S......................S","SSSSSSSSSSSSSSSSSSSSSSSS"};
static const char(*const FMAPS[3])[25]={FMAP_COMPOUND,FMAP_BUNKER,FMAP_VILLAGE};
typedef struct{const char*n,*sub,*l1,*l2;u8 map,dm,outdoor,need,night,exfil,surv;}FMis;
#define FM_N 8
static const FMis FMI[FM_N]={
 {"COMPOUND RAID","CLEAR THE COMPOUND, SEIZE THE INTEL","ELIMINATE ALL HOSTILES","THEN GRAB THE INTEL (X)",0,0,1,9,0,0,0},
 {"BUNKER SWEEP","UNDERGROUND COMMAND POST","SWEEP EVERY ROOM, GRAB THE INTEL","DOORS: B TO OPEN",1,0,0,10,0,0,0},
 {"VILLAGE CLEAR","HOUSE-TO-HOUSE","CLEAR ALL HOUSES, GRAB THE INTEL","WATCH THE WINDOWS",2,0,1,9,0,0,0},
 {"NIGHT RAID","COMPOUND AFTER DARK","SAME COMPOUND - NO MOON","CLEAR IT AND TAKE THE INTEL",0,0,1,9,1,0,0},
 {"HOSTAGE RESCUE","BUNKER - ONE HOSTAGE","FIND THE HOSTAGE (X)","THEN EXFIL TO YOUR START POINT",1,0,0,0,0,1,0},
 {"HOLDOUT","VILLAGE - SURVIVE 3 MINUTES","ENEMIES KEEP COMING","STAY ALIVE UNTIL EXFIL",2,1,1,0,0,0,1},
 {"DEATHMATCH VS BOTS","VILLAGE - FIRST TO 15 KILLS","BOTS RESPAWN, YOU RESPAWN","PICK UP ENEMY WEAPONS",2,1,1,15,0,0,0},
 {"BUNKER DEATHMATCH","CLOSE QUARTERS - FIRST TO 15","BOTS RESPAWN, YOU RESPAWN","SHOTGUN COUNTRY",1,1,0,15,0,0,0}};
typedef struct{const char*n;u8 mag,rate,dmg,pel,spread,auto_;u16 res;}FWep;
static const FWep FWP[5]={{"M9 PISTOL",15,14,26,1,0,0,60},{"M4 CARBINE",30,6,34,1,1,1,150},{"M870 SHOTGUN",6,40,14,8,10,0,30},{"AK-47 (PICKUP)",30,7,38,1,2,1,90},{"PKM (PICKUP)",100,5,36,1,3,1,100}};
typedef struct{s32 x,y;s16 hp,cd,deadT,ang,alert;u8 alive,type,fr,shoot;s32 lkx,lky;u8 seen;}FBot;
typedef struct{s32 x,y;u8 t,on;}FPk;
typedef struct{s32 x,y;int a,hp,wep,have,mag[5],res[5],cd,reload,kills,deaths,flash,hurt,intel,t,mi,shots,hits,fpsv,fcnt;u32 v0;int bob,win,lose,respawn;}FPl;
static FPl FP EWRAM;static int nFB EWRAM,nFK EWRAM;
// play-time arrays live in the KC yard cache (kc_bg, regenerated on demand via kc_bgok=0)
#define FSCR ((u8*)kc_bg)
#define FM ((u8(*)[24])(FSCR+0))
#define FD (FSCR+576)
#define FTEX ((u8(*)[256])(FSCR+1152))
#define FCEIL ((u16*)(FSCR+2432))
#define FFLR ((u16*)(FSCR+2720))
#define FZ ((u16*)(FSCR+3008))
#define FPALB ((u16*)(FSCR+3264))
#define FQ ((u16*)(FSCR+3776))
#define FB ((FBot*)(FSCR+5120))
#define FK ((FPk*)(FSCR+6400))
#define FDEP ((int*)(FSCR+7168))
#define FORD (FSCR+7360)
#define FSPC (FSCR+7424)
static u16*fbk EWRAM;
#define FW(c) ((c)=='#'||(c)=='B'||(c)=='M'||(c)=='S'||(c)=='D')
static int fwt(int c){return c=='#'?0:c=='B'?1:c=='M'?2:c=='S'?3:4;}
static int fsn(int a){a&=1023;int i=a>>2,f=a&3;int s0=SIN(i&255),s1=SIN((i+1)&255);return s0+((s1-s0)*f>>2);}
static int fcs(int a){return fsn(a+256);}
// ---------------------------------------------------------------- palette + textures
static u16 f_rgb(int r,int g,int b){return RGB(clampi(r,0,255),clampi(g,0,255),clampi(b,0,255));}
static int fNight EWRAM;
static void f_palette(int outdoor){ // v1.3: per wall type 4 texel colours x 4 light levels (distance/side)
 static const u8 WC[5][4][3]={{{150,148,138},{112,110,104},{178,176,166},{86,84,80}},      // '#' concrete: panel, seam, highlight, stain
  {{168,72,48},{196,190,176},{124,50,36},{204,106,72}},                                   // 'B' brick: brick, mortar, dark brick, lit edge
  {{104,118,132},{60,66,76},{168,182,196},{140,100,60}},                                  // 'M' metal: plate, seam, sheen, rust
  {{176,156,104},{124,106,70},{206,190,140},{96,84,56}},                                  // 'S' sandbags: bag, crease, highlight, shadow
  {{132,86,44},{86,54,28},{220,190,60},{164,116,64}}};                                    // 'D' door: plank, gap, brass/sign, grain
 static const u8 LK[4]={240,184,132,88}; // v1.3 Doom-gritty: darker falloff
 for(int t=0;t<5;t++)for(int k=0;k<4;k++)for(int l=0;l<4;l++){int m=LK[l];PAL_BG[16+t*16+k*4+l]=f_rgb(WC[t][k][0]*m/256,WC[t][k][1]*m/256,WC[t][k][2]*m/256);}
 for(int i=0;i<16;i++){PAL_BG[96+i]=outdoor?f_rgb(60+i*8,110+i*6,190+i*3):f_rgb(20+i,20+i,24+i);PAL_BG[112+i]=outdoor?f_rgb(90+i*5,80+i*4,60+i*3):f_rgb(40+i*3,40+i*3,38+i*3);}
 static const u8 SC[16][3]={{0,0,0},{200,160,120},{70,90,50},{45,60,35},{30,30,30},{90,90,96},{60,70,40},{255,230,120},{150,40,40},{230,230,220},{60,60,180},{180,140,60},{40,120,40},{200,60,60},{120,90,60},{250,250,250}};
 for(int s=0;s<3;s++)for(int i=0;i<16;i++){int k=256-s*70;PAL_BG[128+s*16+i]=f_rgb(SC[i][0]*k/256,SC[i][1]*k/256,SC[i][2]*k/256);}
 if(fNight)for(int i=16;i<176;i++){u16 c=PAL_BG[i];int r=(c&31)*2/5,g=((c>>5)&31)*2/5,b=((c>>10)&31)*3/5;if(i>=96&&i<112){r=1+(i-96)/8;g=1+(i-96)/8;b=3+(i-96)/5;}PAL_BG[i]=r|(g<<5)|(b<<10);}
 PAL_BG[0]=0;PAL_BG[1]=RGB(248,248,248);PAL_BG[2]=RGB(255,220,80);PAL_BG[3]=RGB(240,60,50);PAL_BG[4]=RGB(80,220,80);PAL_BG[5]=RGB(20,22,30);PAL_BG[6]=RGB(70,74,90);PAL_BG[7]=RGB(140,140,150);PAL_BG[8]=RGB(255,250,200);PAL_BG[9]=RGB(40,40,46);PAL_BG[10]=RGB(90,60,40);PAL_BG[11]=RGB(255,140,40);
 PAL_BG[12]=RGB(214,168,124);PAL_BG[13]=RGB(166,120,84);PAL_BG[14]=RGB(104,70,48);PAL_BG[15]=RGB(84,104,56);PAL_BG[176]=RGB(54,70,36);PAL_BG[177]=RGB(140,30,30);} // v1.3 HUD face colours (never night-dimmed)
static void f_textures(void){for(int t=0;t<5;t++)for(int y=0;y<16;y++)for(int x=0;x<16;x++){u32 h=xh(t*977+y*16+x);int v=0;
  if(t==0){v=(x==0||y==0||y==8)?1:(x==1||y==1||y==9)?2:((h&7)==0||(y>11&&(h&3)==0&&x>4&&x<11)||(x==4+(y>>2)&&y>3&&y<11))?3:0;}                       // concrete panels + water stains
  else if(t==1){int off=(y/4)&1?4:0;if((y&3)==3||((x+off)&7)==7)v=1;else if((y&3)==0||((x+off)&7)==0)v=3;else v=(h%4==0)?2:(h%11==0)?1:0;} // running-bond brick
  else if(t==2){v=(x==0||x==8||y==0)?1:(x==1||x==9||y==1)?2:((x==3||x==12)&&(y==3||y==12))?2:(y>10&&(h&3)!=0)?3:((h&31)==0)?3:0;}        // riveted plates, rust at foot
  else if(t==3){int yy=y%5,o=((y/5)&1)*4;v=yy==4?1:yy==0?2:(((x+o)%8)==0)?1:(yy==3&&(h&1))?3:0;}                              // stacked sandbags
  else{v=(x==0||x==15)?1:(x%5==0)?1:(x==12&&(y==7||y==8))?2:(y==3&&x>3&&x<12)?2:((h&7)==0)?3:0;}                              // plank door, brass handle, sign
  FTEX[t][y*16+x]=v;}}
// ---------------------------------------------------------------- mode 4 primitives
IWCODE static void f_rect(int x,int y,int w,int h,int c){u16 v=c|(c<<8);if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w<=0||h<=0)return;
 for(int j=y;j<y+h;j++){u16*p=fbk+j*120;int a=x,b=x+w;if(a&1){p[a>>1]=(p[a>>1]&0xFF)|(c<<8);a++;}if(b&1){p[b>>1]=(p[b>>1]&0xFF00)|c;b--;}for(int k=a>>1;k<(b+1)>>1;k++)p[k]=v;}}
static void f_px(int x,int y,int c){if((unsigned)x>=240||(unsigned)y>=160)return;u16*p=fbk+y*120+(x>>1);*p=(x&1)?((*p&0xFF)|(c<<8)):((*p&0xFF00)|c);}
static void f_text(int x,int y,const char*s,int c){for(;*s;s++,x+=6){int ch=*s;if(ch>='a'&&ch<='z')ch-=32;if(ch<32||ch>127)ch='?';const u8*g=&font_bits[(ch-32)*7];for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<gx)){f_px(x+gx+1,y+gy+1,5);f_px(x+gx,y+gy,c);}}}
static void f_ctext(int y,const char*s,int c){f_text(120-(slen(s)*6-1)/2,y,s,c);}
IWCODE static void f_col(u16*p,int top,int h,const u8*tex,const u16*lut,const u16*ceil,const u16*flr,int st){int y=0,y0=top<0?0:top,y1=top+h;if(y1>FVH)y1=FVH;
 for(;y<y0;y++){*p=ceil[y];p+=120;}int ty=(y0-top)*st;for(;y<y1;y++){*p=lut[tex[(ty>>16)<<4]];ty+=st;p+=120;}for(;y<FVH;y++){*p=flr[y];p+=120;}}
// ---------------------------------------------------------------- render
static s16 FCX[120] EWRAM;
#define FABS(v) ((v)<0?-(v):(v))
IWCODE static void f_cast_a(int dx,int dy,int px,int py,int bob,u16*fb){int plx=-dy*169>>8,ply=dx*169>>8;
 for(int c=0;c<120;c++){int cx=FCX[c];int rx=dx+(plx*cx>>8),ry=dy+(ply*cx>>8);int mx=px>>8,my=py>>8;int arx=FABS(rx),ary=FABS(ry);
  int ddx=arx?(int)(FRCP[arx<2047?arx:2047]>>8):1<<24,ddy=ary?(int)(FRCP[ary<2047?ary:2047]>>8):1<<24;int sx,sy,sdx,sdy;
  if(rx<0){sx=-1;sdx=(px&255)*ddx>>8;}else{sx=1;sdx=(256-(px&255))*ddx>>8;}if(ry<0){sy=-1;sdy=(py&255)*ddy>>8;}else{sy=1;sdy=(256-(py&255))*ddy>>8;}
  int side=0,cell='#';for(int n=0;n<48;n++){if(sdx<sdy){sdx+=ddx;mx+=sx;side=0;}else{sdy+=ddy;my+=sy;side=1;}if((unsigned)mx>=24||(unsigned)my>=24)break;cell=FM[my][mx];if(FW(cell))break;}
  int perp=side==0?sdx-ddx:sdy-ddy;if(perp<24)perp=24;if(perp>65535)perp=65535;FZ[c]=perp;
  int lh=perp<2048?(int)(((unsigned long long)(FVH*256)*FRCP[perp])>>24):(int)(((unsigned long long)(FVH*8)*FRCP[perp>>5])>>24);if(lh<1)lh=1;
  int top=FVH/2-lh/2+bob;int wx=side==0?py+(perp*ry>>8):px+(perp*rx>>8);int tx=(wx&255)>>4;if(side==0&&rx>0)tx=15-tx;if(side==1&&ry<0)tx=15-tx;
  int wt=cell=='#'?0:cell=='B'?1:cell=='M'?2:cell=='S'?3:4;int lv=(perp>>9)+side;if(lv>3)lv=3;u16 lut[4];for(int k=0;k<4;k++){int idx=16+wt*16+k*4+lv;lut[k]=idx|(idx<<8);}
  f_col(fb+c,top,lh,&FTEX[wt][tx],lut,FCEIL,FFLR,lh<2047?(int)(FRCP[lh]>>4):0);}}
static void f_cast(void){f_cast_a(fcs(FP.a),fsn(FP.a),FP.x,FP.y,FP.bob,fbk);}
// sprite art (16 wide): . clear, k skin, g uniform, d dark, b boots, w weapon, h helmet, f flash, r blood, c cap, 1-3 pickups
static const char*const FSA[4][24]={
{"......hhhh......",".....hhhhhh.....",".....hkkkkh.....","......kkkk......","......kkkk......",".......kk.......","....gggggggg....","...gggggggggg...","...gg.gggg.gg...","...gg.gggg.gg...","...gg.gggg.gg...","...kk.gggg.kk...","......dddd......","......dddd......","......dddd......",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....bb..bb.....","....bbb..bbb...."},
{"......hhhh......",".....hhhhhh.....",".....hkkkkh.....","......kkkk......","......kkkk......",".......kk.......","....gggggggg....","...gggggggggg...","...gg.gggg.gg...","...gg.gggg.gg...","...kk.gggg.gg...","......gggg.kk...","......dddd......","......dddd......","......dddd......",".....dd...dd....","....dd....dd....","....dd.....dd...","...dd......dd...","...dd......dd...","...dd.......dd..","..bb........bb..","..bbb.......bbb.","................"},
{"......hhhh......",".....hhhhhh.....",".....hkkkkh.....","......kkkk......","......kkkk......",".......kk.......","....gggggggg....","...gggwwwwggg...","...ggkwwwwkgg...","......wwww......","......wwww......",".....ffwwff.....","....ff.ff.ff....","......dddd......","......dddd......",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....dd..dd.....",".....bb..bb.....","....bbb..bbb...."},
{"................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","..........w.....","hh.ggggggdddbb..","hkkggggggdddbbb.","hh.ggggggdddbb..","..rrrr.....rr..."}};
static const char*const FPA[7][8]={
{"..1111..",".133331.","13311331","13311331","13333331",".133331.","..1111..","........"},
{"........","..2222..",".255552.",".255552.",".255552.",".222222.","........","........"},
{"........","........","wwwwwwww","wwbbwww.",".bb.....","........","........","........"},
{"........","........","wwwwwwww","dd.wwbb.","...dd...","........","........","........"},
{"........","..ccc...","wwwwwwww","wwwbbww.",".bb..b..","........","........","........"},
{"........","..9999..",".9eeee9.",".9e99e9.",".9eeee9.","..9999..","........","........"},
{"..kkkk..","..kkkk..",".gggggg.","kggggggk",".gggggg.","..dd.dd.","..dd.dd.","..bb.bb."}};
static u8 f_spc(int ch){switch(ch){case 'k':return 1;case 'g':return 2;case 'd':return 3;case 'b':return 4;case 'w':return 5;case 'h':return 6;case 'f':return 7;case 'r':return 8;case 'c':return 11;case '1':return 9;case '3':return 13;case '2':return 14;case '5':return 12;case '9':return 9;case 'e':return 10;}return 0;}
static void f_billboard(s32 wx,s32 wy,const char*const*art,int aw,int ah,int hfrac){int dx=fcs(FP.a),dy=fsn(FP.a);int sx=wx-FP.x,sy=wy-FP.y;int d=(sx*dx+sy*dy)>>8;if(d<40)return;int u=(sx*-dy+sy*dx)>>8;
 int col=60+(u*60*256)/(d*169);int full=FVH*256/d;int hs=full*hfrac>>8;int bot=FVH/2+full/2+FP.bob;int top=bot-hs;int wc=hs*aw/ah/2;if(wc<1)wc=1;int c0=col-wc/2;
 int shade=d>1200?2:d>600?1:0;if(c0+wc<0||c0>=120)return;
 for(int c=c0;c<c0+wc;c++){if(c<0||c>=120||FZ[c]<d)continue;int tx=(c-c0)*aw/wc;u16*p=fbk+c;int y0=top<0?0:top,y1=bot>FVH?FVH:bot;
  int st=(ah<<16)/(hs?hs:1),ty=(y0-top)*st;for(int y=y0;y<y1;y++,ty+=st){char ch=art[ty>>16][tx];if(ch=='.')continue;int idx=128+shade*16+FSPC[(u8)ch];p[y*120]=idx*0x101;}}}
// ---------------------------------------------------------------- world
static int f_pass(int cx,int cy){if((unsigned)cx>=24||(unsigned)cy>=24)return 0;return !FW(FM[cy][cx]);}
static int f_free(s32 x,s32 y,int r){return f_pass((x-r)>>8,(y-r)>>8)&&f_pass((x+r)>>8,(y-r)>>8)&&f_pass((x-r)>>8,(y+r)>>8)&&f_pass((x+r)>>8,(y+r)>>8);}
static void f_move(s32*x,s32*y,int dx,int dy,int r){if(f_free(*x+dx,*y,r))*x+=dx;if(f_free(*x,*y+dy,r))*y+=dy;}
static int f_los(s32 x0,s32 y0,s32 x1,s32 y1){int dx=x1-x0,dy=y1-y0;int n=idist(dx,dy)/48+1;int sx=(dx<<8)/n,sy=(dy<<8)/n;s32 x=x0<<8,y=y0<<8;for(int i=1;i<n;i++){x+=sx;y+=sy;if(FW(FM[y>>16][x>>16]))return 0;}return 1;}
static void f_bfs(void){u16*Q=FQ;memset(FD,255,576);int h=0,t=0;int s=(FP.y>>8)*24+(FP.x>>8);FD[s]=0;Q[t++]=s;
 while(h<t){int c=Q[h++];int cx=c%24,cy=c/24;int d=FD[c];static const s8 NX[4]={1,-1,0,0},NY[4]={0,0,1,-1};for(int k=0;k<4;k++){int nx=cx+NX[k],ny=cy+NY[k];if((unsigned)nx>=24||(unsigned)ny>=24)continue;int n=ny*24+nx;if(FD[n]!=255)continue;int ch=FM[ny][nx];if(ch=='#'||ch=='B'||ch=='M'||ch=='S')continue;FD[n]=d+1<254?d+1:254;Q[t++]=n;}}}
static int f_ang(int dx,int dy){return (atan2i(dx,dy)*4+768)&1023;}
static int f_adiff(int a,int b){int d=(a-b)&1023;return d>512?d-1024:d;}
static FPk*f_pk(int t,s32 x,s32 y){if(nFK>=24){for(int i=0;i<24;i++)if(!FK[i].on){nFK=i;break;}if(nFK>=24)return 0;}FPk*k=&FK[nFK++];k->x=x;k->y=y;k->t=t;k->on=1;return k;}
static void f_msg(const char*m){F.msg=m;F.msgT=120;}
static void f_load(int mi){const FMis*M=&FMI[mi];const char(*m)[25]=FMAPS[M->map];memset(&FP,0,sizeof FP);nFB=0;nFK=0;memset(FB,0,sizeof(FBot)*12);memset(FK,0,sizeof(FPk)*24);FP.mi=mi;
 for(int y=0;y<24;y++)for(int x=0;x<24;x++){char c=m[y][x];FM[y][x]=FW(c)?c:'.';s32 wx=(x<<8)+128,wy=(y<<8)+128;
  if(c=='P'){FP.x=wx;FP.y=wy;}else if(c=='E'&&nFB<12){FBot*b=&FB[nFB++];b->x=wx;b->y=wy;b->hp=100;b->alive=1;b->type=nFB%4==0?1:nFB%5==0?2:0;b->ang=rr(1024);b->cd=60+rr(60);}
  else if(c=='H')f_pk(0,wx,wy);else if(c=='A')f_pk(1,wx,wy);else if(c=='W')f_pk(2,wx,wy);else if(c=='X'&&!M->dm)f_pk(M->exfil?6:5,wx,wy);}
 FP.hp=100;FP.wep=1;FP.have=3;for(int i=0;i<5;i++){FP.mag[i]=FWP[i].mag;FP.res[i]=0;}FP.res[0]=FWP[0].res;FP.res[1]=FWP[1].res;
 {int best=0,bd=0;for(int i=0;i<nFB;i++){int d=f_ang(FB[i].x-FP.x,FB[i].y-FP.y);(void)d;}(void)best;(void)bd;}
 FP.a=0;{int dirs[4]={0,256,512,768};int bl=0;for(int k=0;k<4;k++){int ok=0;s32 x=FP.x,y=FP.y;for(int s=0;s<6;s++){x+=fcs(dirs[k]);y+=fsn(dirs[k]);if(!f_pass(x>>8,y>>8))break;ok++;}if(ok>bl){bl=ok;FP.a=dirs[k];}}}
 f_bfs();}
static int fLastHurt EWRAM;
static void f_hurt(int d){FP.hp-=d;FP.hurt=10;fLastHurt=FP.t;sfx_hurt();}
static void f_bot_spawn(FBot*b){for(int tries=0;tries<200;tries++){int x=1+rr(22),y=1+rr(22);if(!f_pass(x,y))continue;s32 wx=(x<<8)+128,wy=(y<<8)+128;if(idist(wx-FP.x,wy-FP.y)<8*256)continue;if(f_los(wx,wy,FP.x,FP.y))continue;b->x=wx;b->y=wy;b->hp=100;b->alive=1;b->alert=0;b->deadT=0;b->cd=90;return;}}
static int f_alive(void);
static u32 fV0 EWRAM; // holdout clock: real seconds from the vblank counter (game loop runs ~17 fps)
#define FSEC() ((int)((vblN-fV0)/60))
static void f_bots(void){const FMis*M=&FMI[FP.mi];
 for(int i=0;i<nFB;i++){FBot*b=&FB[i];if(b->shoot)b->shoot--;
  if(!b->alive){if(M->dm){if(++b->deadT>(M->surv?420:300)&&(!M->surv||f_alive()<3+FSEC()/45))f_bot_spawn(b);}continue;} // holdout: waves grow every 45 s
  int dx=FP.x-b->x,dy=FP.y-b->y;int dd=idist(dx,dy);int los=dd<14*256&&FP.hp>0&&f_los(b->x,b->y,FP.x,FP.y);
  if(los){if(!b->alert){b->alert=1;b->cd=50+rr(40);}b->lkx=FP.x;b->lky=FP.y;b->seen=1;}
  int sp=b->alert?11:5;int mvx=0,mvy=0;
  if(b->alert&&los&&dd<9*256){b->ang=f_ang(dx,dy);if(dd>4*256){mvx=dx*sp/dd/2;mvy=dy*sp/dd/2;}
   if(--b->cd<=0){b->shoot=8;sfx_shot(0);int ch=46+(SET.bot&3)*8-dd*4/256-((keys&(K_UP|K_DOWN|K_L|K_R))?15:0);if(rr(100)<ch)f_hurt(b->type==1?7+rr(5):5+rr(6));b->cd=b->type==1?(b->fr++%4?7:70):40+rr(45);}}
  else if(b->alert){int cx=b->x>>8,cy=b->y>>8;int bd=FD[cy*24+cx],bx=cx,by=cy;static const s8 NX[4]={1,-1,0,0},NY[4]={0,0,1,-1};for(int k=0;k<4;k++){int nx=cx+NX[k],ny=cy+NY[k];if((unsigned)nx>=24||(unsigned)ny>=24)continue;int v=FD[ny*24+nx];if(v<bd){bd=v;bx=nx;by=ny;}}
   if(FM[by][bx]=='D'){FM[by][bx]='.';}int tx=(bx<<8)+128-b->x,ty=(by<<8)+128-b->y;int td=idist(tx,ty);if(td>0){mvx=tx*sp/td;mvy=ty*sp/td;}b->ang=f_ang(tx,ty);}
  else{if(rr(90)==0)b->ang=rr(1024);mvx=fcs(b->ang)*sp>>8;mvy=fsn(b->ang)*sp>>8;s32 ox=b->x,oy=b->y;f_move(&b->x,&b->y,mvx,mvy,70);if(ox==b->x&&oy==b->y)b->ang=rr(1024);mvx=mvy=0;b->fr++;}
  if(mvx||mvy){f_move(&b->x,&b->y,mvx,mvy,70);b->fr++;}
  for(int j=0;j<nFB;j++)if(j!=i&&FB[j].alive){int ex=b->x-FB[j].x,ey=b->y-FB[j].y;if(iabs(ex)<120&&iabs(ey)<120){f_move(&b->x,&b->y,ex>0?4:-4,ey>0?4:-4,70);}}}}
static void f_alert_all(void){for(int i=0;i<nFB;i++){FBot*b=&FB[i];if(b->alive&&!b->alert){int d=FD[(b->y>>8)*24+(b->x>>8)];if(d<14){b->alert=1;b->cd=40+rr(40);}}}}
static int f_alive(void){int n=0;for(int i=0;i<nFB;i++)n+=FB[i].alive;return n;}
static void f_fire(void){const FWep*W=&FWP[FP.wep];FP.mag[FP.wep]--;FP.cd=W->rate;FP.flash=4;FP.shots++;sfx_shot(FP.wep==2||FP.wep==4);f_alert_all();int dx=fcs(FP.a),dy=fsn(FP.a);int hitAny=0;
 for(int p=0;p<W->pel;p++){int col=60+(W->spread?rr(W->spread*2+1)-W->spread:0);int best=-1,bd=FZ[col<0?0:col>119?119:col];
  for(int i=0;i<nFB;i++){FBot*b=&FB[i];if(!b->alive)continue;int sx=b->x-FP.x,sy=b->y-FP.y;int d=(sx*dx+sy*dy)>>8;if(d<40||d>=bd)continue;int u=(sx*-dy+sy*dx)>>8;int c=60+(u*60*256)/(d*169);int full=FVH*256/d;int wc=(full*230>>8)*16/24/2;if(wc<2)wc=2;if(col>=c-wc/2&&col<c+wc/2){best=i;bd=d;}}
  if(best>=0){FBot*b=&FB[best];int dmg=W->dmg;if(bd>8*256)dmg=dmg*7/10;b->hp-=dmg;b->alert=1;hitAny=1;if(b->hp<=0){b->alive=0;b->deadT=0;FP.kills++;sfx_boom();f_pk(b->type==1?4:b->type==2?1:3,b->x+40,b->y);}}}
 FP.hits+=hitAny;}
static const char*const FPKN[7]={"MEDKIT +40","AMMO","M870 SHOTGUN","AK-47 PICKED UP","PKM PICKED UP","INTEL SECURED","HOSTAGE FREED - EXFIL TO START"};
static void f_pickups(void){for(int i=0;i<nFK;i++){FPk*k=&FK[i];if(!k->on)continue;if(idist(k->x-FP.x,k->y-FP.y)>110)continue;int t=k->t;
  if(t==0){if(FP.hp>=100)continue;FP.hp=FP.hp+40>100?100:FP.hp+40;}else if(t==1){FP.res[FP.wep]+=FWP[FP.wep].mag*2;}
  else{int w=t==2?2:t==3?3:t==4?4:-1;if(w>=0){if(FP.have&(1<<w))FP.res[w]+=FWP[w].mag;else{FP.have|=1<<w;FP.mag[w]=FWP[w].mag;FP.res[w]=FWP[w].mag;FP.wep=w;}}if(t>=5)FP.intel=1;}
  k->on=0;sfx_ok();f_msg(FPKN[t]);}}
// ---------------------------------------------------------------- frame
static void f_weapon(void){int w=FP.wep;int bx=128+(FP.bob*2),by=138-(FP.cd>FWP[w].rate-3?4:0);const int G=131,S=129,K=9,M=6,L=7,W=10,O=5;
 // v1.3 Doom-style chunky gun: rect list drawn twice (dark 1px outline pass, then fill pass)
 s16 R[16][5];int n=0;
#define FR(x,y,ww,hh,c) {R[n][0]=x;R[n][1]=y;R[n][2]=ww;R[n][3]=hh;R[n][4]=c;n++;}
 if(w==0){FR(bx-7,by-32,14,15,K);FR(bx-5,by-37,10,6,M);FR(bx-2,by-40,4,4,L);FR(bx-4,by-30,3,10,M);FR(bx-9,by-19,18,12,S);FR(bx-10,by-9,20,9,G);}
 else{int len=w==2?50:w==4?54:48;int hw=w==4?10:w==2?8:9;int bc=w==3||w==2?W:K,mc=w==3||w==2?W:M;
  FR(bx-hw,by-len,hw*2,len,bc);FR(bx-hw+3,by-len,hw*2-6,len,mc);FR(bx-hw+2,by-len+4,2,len-8,L);
  FR(bx-3,by-len-10,6,11,K);FR(bx-1,by-len-12,2,3,L);
  if(w==1){FR(bx-4,by-len+12,8,6,K);FR(bx-1,by-len+13,2,4,L);}
  if(w==3){FR(bx+hw-1,by-len+20,7,15,K);}if(w==4){FR(bx-hw-5,by-len+22,5,10,K);}
  if(w==2){FR(bx-hw,by-len+26,hw*2,6,K);}
  FR(bx-hw-12,by-len+18,14,12,S);FR(bx-hw-16,by-len+28,12,len-28,G);
  FR(bx+hw-7,by-20,14,12,S);FR(bx+hw-5,by-9,17,9,G);}
#undef FR
 for(int k=0;k<n;k++)f_rect(R[k][0]-1,R[k][1]-1,R[k][2]+2,R[k][3]+2,O);
 for(int k=0;k<n;k++)f_rect(R[k][0],R[k][1],R[k][2],R[k][3],R[k][4]);
 if(FP.flash>0){int fy=by-(w==0?42:(w==4?66:w==2?62:60));f_rect(bx-9,fy-7,18,10,8);f_rect(bx-5,fy-12,10,7,2);f_rect(bx-13,fy-3,26,4,11);f_rect(bx-2,fy-15,4,4,1);}}
// v1.3 Doom-style reacting face portrait in the HUD (original pixel art, procedural)
static int f_facest(void){int hp=FP.hp<0?0:FP.hp;int band=hp>=70?0:hp>=40?1:hp>0?2:3;int look=FP.hurt>0?3:((FP.t>>6)%3);return band*4+look;}
static void f_face(int x,int y,int st){int band=st>>2,look=st&3;const int SK=12,SD=13,SX=14,K=9,WH=1,RD=3,DR=177,HG=15,HD=176;
 f_rect(x,y,20,22,5);f_rect(x+3,y+5,14,15,SK);f_rect(x+12,y+6,5,13,SD);f_rect(x+4,y+18,12,3,SD);       // face + cel shadow + jaw
 f_rect(x+2,y+1,16,6,HG);f_rect(x+1,y+5,18,2,HD);f_rect(x+12,y+1,6,5,HD);f_rect(x+4,y+2,4,1,7);            // helmet + rim + sheen
 if(band<3){int ey=y+9,dx=look==1?-1:look==2?1:0;
  if(look==3){f_rect(x+5,ey+1,4,1,K);f_rect(x+11,ey+1,4,1,K);}                                              // grimace squint
  else{f_rect(x+5,ey,4,3,WH);f_rect(x+11,ey,4,3,WH);f_rect(x+6+dx,ey,2,3,K);f_rect(x+12+dx,ey,2,3,K);}
  f_rect(x+4,ey-2,5,1,band?SX:K);f_rect(x+11,ey-2,5,1,band?SX:K);                                         // brows
  f_rect(x+9,ey+3,2,3,SD);                                                                                  // nose
  if(look==3){f_rect(x+7,y+16,6,3,K);f_rect(x+8,y+16,4,1,WH);}else f_rect(x+7,y+17,6,1,band>=2?K:SX);       // mouth
  if(band>=1){f_rect(x+14,y+12,2,2,SX);f_rect(x+4,y+14,2,1,SX);}                                            // bruises
  if(band>=2){f_rect(x+5,y+7,2,6,RD);f_rect(x+5,y+13,1,3,DR);f_rect(x+13,y+15,3,2,RD);}}                    // blood
 else{f_rect(x+5,y+9,4,1,K);f_rect(x+11,y+9,4,1,K);f_rect(x+6,y+8,2,3,K);f_rect(x+12,y+8,2,3,K);f_rect(x+7,y+16,6,2,K);f_rect(x+4,y+6,12,3,RD);}
 f_rect(x,y,20,1,6);f_rect(x,y+21,20,1,6);f_rect(x,y,1,22,6);f_rect(x+19,y,1,22,6);}
static u32*FHS EWRAM;
static void f_hud(void){const FMis*M=&FMI[FP.mi];u32 sig=(u32)(FP.hp&255)+(u32)(FMI[FP.mi].surv?FSEC():0)*0x9E3779B1u|((u32)FP.mag[FP.wep]<<8)|((u32)FP.res[FP.wep]<<16)|((u32)FP.wep<<26)|((u32)(FP.reload>0)<<29);sig^=(u32)(FP.kills*7919+f_alive()*104729+FP.intel*31+FP.fpsv*1299709+FP.deaths*15485863);int pg=fbk==(u16*)0x06000000?0:1;
 int fst=f_facest();sig^=(u32)fst*0x01000193u;
 if(FHS[pg]!=sig){FHS[pg]=sig;f_rect(0,136,240,24,5);f_rect(0,136,240,1,6);char b[48],*o;f_face(2,137,fst);
 o=sp_put(b,"HP ");sp_int(o,FP.hp<0?0:FP.hp);f_text(26,139,b,FP.hp<30?3:4);f_rect(26,149,50,4,9);f_rect(26,149,(FP.hp<0?0:FP.hp)/2,4,FP.hp<30?3:4);
 f_text(82,139,FWP[FP.wep].n,1);o=sp_int(b,FP.mag[FP.wep]);o=sp_put(o,"/");sp_int(o,FP.res[FP.wep]);f_text(82,149,FP.reload?"RELOAD":b,FP.reload?2:7);
 if(M->surv){int r=180-FSEC();o=sp_put(b,"HOLD ");o=sp_int(o,r<0?0:r);o=sp_put(o,"S K");sp_int(o,FP.kills);}else if(M->dm){o=sp_put(b,"K ");o=sp_int(o,FP.kills);o=sp_put(o,"/15 D ");sp_int(o,FP.deaths);}else{o=sp_put(b,"ENEMY ");o=sp_int(o,f_alive());if(FP.intel)sp_put(o," +INT");}f_text(130,149,b,2);
 o=sp_int(b,FP.fpsv);sp_put(o," FPS");f_text(196,139,b,7);}
 int cx=120,cy=FVH/2;f_rect(cx-6,cy,4,1,4);f_rect(cx+3,cy,4,1,4);f_rect(cx,cy-6,1,4,4);f_rect(cx,cy+3,1,4,4);
 if(FP.hurt>0){f_rect(0,0,240,3,3);f_rect(0,133,240,3,3);f_rect(0,0,3,136,3);f_rect(237,0,3,136,3);}
 if(F.msgT>0){F.msgT--;f_ctext(12,F.msg,2);}}
#define FTK() ((u32)(*(volatile u16*)0x4000108)|((u32)(*(volatile u16*)0x400010C)<<16))
static u32 fPr[4];
static void f_render(void){u32 t0=FTK();f_cast();u32 t1=FTK();fPr[0]=(t1-t0)>>10;
 // sprites back to front
 u8*ord=FORD;int*dep=FDEP;int n=0;int dx=fcs(FP.a),dy=fsn(FP.a);
 for(int i=0;i<nFB;i++){if(!FB[i].alive&&FB[i].deadT>240&&FMI[FP.mi].dm)continue;dep[n]=((FB[i].x-FP.x)*dx+(FB[i].y-FP.y)*dy)>>8;ord[n++]=i;}
 for(int i=0;i<nFK;i++)if(FK[i].on){dep[n]=((FK[i].x-FP.x)*dx+(FK[i].y-FP.y)*dy)>>8;ord[n++]=100+i;}
 for(int i=1;i<n;i++){int j=i;while(j>0&&dep[j-1]<dep[j]){int t=dep[j];dep[j]=dep[j-1];dep[j-1]=t;u8 o=ord[j];ord[j]=ord[j-1];ord[j-1]=o;j--;}}
 for(int i=0;i<n;i++){int k=ord[i];if(k>=100){FPk*p=&FK[k-100];f_billboard(p->x,p->y,FPA[p->t],8,8,70);}else{FBot*b=&FB[k];int fr=!b->alive?3:b->shoot>4?2:(b->fr>>3)&1;f_billboard(b->x,b->y,FSA[fr],16,24,230);}}
 u32 t2=FTK();fPr[1]=(t2-t1)>>10;f_weapon();f_hud();fPr[2]=(FTK()-t2)>>10;}
static void f_flip(void){volatile u16*dc=(volatile u16*)0x04000000;*dc^=0x10;fbk=(*dc&0x10)?(u16*)0x06000000:(u16*)0x0600A000;}
// ---------------------------------------------------------------- autopilot
static void f_auto(u16*ky,u16*pr){*ky=0;*pr=0;int best=-1,bd=1<<30;for(int i=0;i<nFB;i++){FBot*b=&FB[i];if(!b->alive)continue;int d=idist(b->x-FP.x,b->y-FP.y);if(d<14*256&&d<bd&&f_los(FP.x,FP.y,b->x,b->y)){bd=d;best=i;}}
 if(FP.reload||FP.hp<35)*ky|=K_DOWN|((FP.t&64)?K_L:K_R);
 if(best>=0){FBot*b=&FB[best];int a=f_ang(b->x-FP.x,b->y-FP.y);int df=f_adiff(a,FP.a);if(df>6)*ky|=K_RIGHT;else if(df<-6)*ky|=K_LEFT;if(iabs(df)<16){*ky|=K_A;if(FWP[FP.wep].auto_==0&&(FP.t&3)==0)*pr|=K_A;}if(FP.mag[FP.wep]==0&&FP.res[FP.wep]==0)*pr|=K_SEL;return;}
 // navigate: goal = nearest alive bot by path distance, else intel, else start
 int gc=-1,gd=255;for(int i=0;i<nFB;i++)if(FB[i].alive){int c=(FB[i].y>>8)*24+(FB[i].x>>8);if(FD[c]<gd){gd=FD[c];gc=c;}}
 if(gc<0){for(int i=0;i<nFK;i++)if(FK[i].on&&(FK[i].t>=5||(FK[i].t==0&&FP.hp<60))){int c=(FK[i].y>>8)*24+(FK[i].x>>8);if(FD[c]<gd){gd=FD[c];gc=c;}}}
 if(FMI[FP.mi].exfil&&FP.intel){const char(*m)[25]=FMAPS[FMI[FP.mi].map];for(int y=0;y<24;y++)for(int x=0;x<24;x++)if(m[y][x]=='P')gc=y*24+x;}
 if(FMI[FP.mi].exfil&&!FP.intel){for(int i=0;i<nFK;i++)if(FK[i].on&&FK[i].t==6)gc=(FK[i].y>>8)*24+(FK[i].x>>8);}
 if(gc<0)return;int c=gc;for(int s=0;s<80&&FD[c]>1;s++){int cx=c%24,cy=c/24;static const s8 NX[4]={1,-1,0,0},NY[4]={0,0,1,-1};int nb=c;for(int k=0;k<4;k++){int nx=cx+NX[k],ny=cy+NY[k];if((unsigned)nx>=24||(unsigned)ny>=24)continue;int n=ny*24+nx;if(FD[n]<FD[nb])nb=n;}if(nb==c)break;c=nb;}
 int tx=((c%24)<<8)+128,ty=((c/24)<<8)+128;if(FD[gc]<=1){tx=((gc%24)<<8)+128;ty=((gc/24)<<8)+128;}
 int a=f_ang(tx-FP.x,ty-FP.y);int df=f_adiff(a,FP.a);if(df>10)*ky|=K_RIGHT;else if(df<-10)*ky|=K_LEFT;if(iabs(df)<120)*ky|=K_UP;
 if(FM[c/24][c%24]=='D'&&(FP.t&15)==0)*pr|=K_B;
 static s32 lx EWRAM,ly EWRAM;static int st EWRAM;if(iabs(FP.x-lx)+iabs(FP.y-ly)<4)st++;else st=0;lx=FP.x;ly=FP.y;if(st>40){*ky|=(FP.t&64)?K_L:K_R;*ky&=~K_UP;if(st>80)st=0;}}
// ---------------------------------------------------------------- mission loop
typedef struct{u16 best[FM_N];u8 stars[FM_N];}FRec;static FRec FR EWRAM;
static void fr_write(void){const u8*b=(const u8*)&FR;u8 s=0x46;SRAM[FP_OFF]='A';SRAM[FP_OFF+1]='O';SRAM[FP_OFF+2]='E';SRAM[FP_OFF+3]='F';for(int i=0;i<(int)sizeof FR;i++){SRAM[FP_OFF+4+i]=b[i];s+=b[i]*(i+1);}SRAM[FP_OFF+4+sizeof FR]=s;}
static void fr_read(void){memset(&FR,0,sizeof FR);if(SRAM[FP_OFF]!='A'||SRAM[FP_OFF+1]!='O'||SRAM[FP_OFF+2]!='E'||SRAM[FP_OFF+3]!='F')return;u8*b=(u8*)&FR;u8 s=0x46;for(int i=0;i<(int)sizeof FR;i++){b[i]=SRAM[FP_OFF+4+i];s+=b[i]*(i+1);}if(s!=SRAM[FP_OFF+4+sizeof FR])memset(&FR,0,sizeof FR);}
static int f_play(int mi){const FMis*M=&FMI[mi];kc_bgok=0;f_load(mi);int au=sp_isauto();
 for(int i=0;i<256;i++)FPALB[i]=PAL_BG[i];fNight=M->night;f_palette(M->outdoor);f_textures();for(int c=0;c<128;c++)FSPC[c]=f_spc(c);static u32 hsig[2] EWRAM;hsig[0]=hsig[1]=0xFFFFFFFF;FHS=hsig;
 for(int c=0;c<120;c++)FCX[c]=(2*c-119)*256/120;
 for(int y=0;y<FVH;y++){int up=y<FVH/2;int f=up?y*32/(FVH/2):(y-FVH/2)*32/(FVH/2+1);if(f>31)f=31;int base=up?96:112,c0=base+f/2,c1=c0+((f&1)&&(f/2<15));u16 v=(y&1)?(c0|(c1<<8)):(c1|(c0<<8));if(up)FCEIL[y]=v;else FFLR[y]=v;}
 for(int y=0;y<FVH;y++){if(y<FVH/2)FFLR[y]=FCEIL[y];else FCEIL[y]=FFLR[y];}
 vsync();REG_DISPCNT=4|(1<<10);fbk=(u16*)0x0600A000;F.msgT=0;f_msg(M->l1);bgm_t(TR_MODERN);
 FP.v0=vblN;fV0=vblN;int res=0,endT=0;int prevA=0;*(volatile u16*)0x400010A=0;*(volatile u16*)0x400010E=0;*(volatile u16*)0x4000108=0;*(volatile u16*)0x400010C=0;*(volatile u16*)0x400010E=0x84;*(volatile u16*)0x400010A=0x80;u32 tL=0;
 for(;;FP.t++){u16 ky=keys,pr=pressed;pressed=0;if(au)f_auto(&ky,&pr);
  if(!au&&(pr&K_START)){res=2;break;}
  // player
  if(FP.hp>0&&!res){int tr=(ky&K_B)?20:14;if(ky&K_LEFT)FP.a=(FP.a-tr)&1023;if(ky&K_RIGHT)FP.a=(FP.a+tr)&1023;int dx=fcs(FP.a),dy=fsn(FP.a);int sp=18;int mx=0,my=0;
   if(ky&K_UP){mx+=dx*sp>>8;my+=dy*sp>>8;}if(ky&K_DOWN){mx-=dx*sp>>9;my-=dy*sp>>9;}if(ky&K_L){mx+=dy*sp>>9;my-=dx*sp>>9;}if(ky&K_R){mx-=dy*sp>>9;my+=dx*sp>>9;}
   if(mx||my){f_move(&FP.x,&FP.y,mx,my,60);FP.bob=(FP.t>>2)&1;}else FP.bob=0;
   if(pr&K_B){int cx=(FP.x+dx)>>8,cy=(FP.y+dy)>>8;if((unsigned)cx<24&&(unsigned)cy<24&&FM[cy][cx]=='D'){FM[cy][cx]='.';sfx_menu();f_msg("DOOR OPEN");}else if(FP.mag[FP.wep]<FWP[FP.wep].mag&&FP.res[FP.wep]>0&&!FP.reload)FP.reload=70;}
   if(pr&K_SEL){for(int k=1;k<=5;k++){int w=(FP.wep+k)%5;if(FP.have&(1<<w)){FP.wep=w;FP.reload=0;sfx_menu();break;}}}
   if(FP.cd>0)FP.cd--;if(FP.flash>0)FP.flash--;if(FP.hurt>0)FP.hurt--;
   if(FP.reload>0){if(--FP.reload==0){int need=FWP[FP.wep].mag-FP.mag[FP.wep];if(need>FP.res[FP.wep])need=FP.res[FP.wep];FP.mag[FP.wep]+=need;FP.res[FP.wep]-=need;}}
   else{int want=FWP[FP.wep].auto_?(ky&K_A):(pr&K_A)||((ky&K_A)&&!prevA);if(want&&FP.cd==0){if(FP.mag[FP.wep]>0)f_fire();else if(FP.res[FP.wep]>0)FP.reload=70;else{f_msg("EMPTY - SELECT: SWITCH");FP.cd=20;}}}
   prevA=ky&K_A;f_pickups();if(FP.hp<100&&FP.t-fLastHurt>240&&(FP.t%30)==0)FP.hp++;}
  tL=FTK();if(FP.t%20==0)f_bfs();
  f_bots();
  if(!res){if(M->surv&&FSEC()>=180&&FP.hp>0){res=1;endT=FP.t;f_msg("EXFIL HELO INBOUND - YOU HELD!");}if(M->surv&&FP.hp<=0){res=3;endT=FP.t;}
   if(M->surv){}else if(M->dm){if(FP.kills>=15){res=1;endT=FP.t;}if(FP.hp<=0){if(!FP.respawn){FP.deaths++;FP.respawn=120;f_msg("KILLED - RESPAWNING");}else if(--FP.respawn==0){const char(*m)[25]=FMAPS[M->map];for(int y=0;y<24;y++)for(int x=0;x<24;x++)if(m[y][x]=='P'){FP.x=(x<<8)+128;FP.y=(y<<8)+128;}FP.hp=100;f_bfs();}}if(FP.deaths>=15){res=3;endT=FP.t;}}
   else{if(FP.hp<=0){res=3;endT=FP.t;f_msg("YOU ARE DOWN - MISSION FAILED");}else if(M->exfil){if(FP.intel){const char(*m)[25]=FMAPS[M->map];int px=0,py=0;for(int y=0;y<24;y++)for(int x=0;x<24;x++)if(m[y][x]=='P'){px=x;py=y;}if((FP.x>>8)==px&&(FP.y>>8)==py){res=1;endT=FP.t;f_msg("HOSTAGE EXFILTRATED");}}}
    else if(!f_alive()&&FP.intel){res=1;endT=FP.t;f_msg("AREA SECURE - INTEL IN HAND");}else if(!f_alive()&&!FP.intel&&F.msgT==0)f_msg("ALL CLEAR - GRAB THE INTEL (X)");}}
  if(res&&FP.t-endT>150)break;if(FP.t>60*60*8){res=3;break;}
  fPr[3]=(FTK()-tL)>>10;f_render();
  FP.fcnt++;{u32 el=vblN-FP.v0;if(el>=60){FP.fpsv=(FP.fcnt*60+el/2)/el;FP.fcnt=0;FP.v0=vblN;}}
  BEAT->screen=SCR_FPS;BEAT->mode=mi;BEAT->hp=FP.hp;BEAT->scoreA=FP.kills;BEAT->scoreB=FP.fpsv;BEAT->kills=FP.kills;BEAT->deaths=FP.deaths;BEAT->timer=FP.t;BEAT->phase=res?(res==1?3:4):1;BEAT->x=FP.x>>4;BEAT->y=FP.y>>4;BEAT->extra=f_alive()|(FP.intel<<8)|(FP.wep<<12);
  next_frame();f_flip();}
 vsync();REG_DISPCNT=0x80;for(int i=0;i<256;i++)PAL_BG[i]=FPALB[i];
 int secs=endT/60;int acc=FP.shots?FP.hits*100/FP.shots:0;int sc=res==1?clampi(3000-secs*4+acc*10+FP.hp*5-FP.deaths*100,500,9999):0;int st=res==1?1+(FP.hp>=50||M->dm)+(acc>=40):0;
 if(res!=2){fr_read();if(sc>FR.best[mi])FR.best[mi]=sc;if(st>FR.stars[mi])FR.stars[mi]=st;fr_write();}
 if(res==1)music_victory();
 for(int t=0;;t++){next_frame();BEAT->screen=SCR_FPS;BEAT->phase=res==1?3:res==2?5:4;BEAT->scoreA=sc;BEAT->scoreB=st;
  if(t==0){char b[48],*o;chrome(CAT_MP,0);m3_box(16,24,208,112,1);m3_ctext(32,res==1?"MISSION COMPLETE":res==2?"MISSION ABORTED":"MISSION FAILED",res==1?C_YEL:C_RED,2);m3_ctext(52,M->n,C_WHITE,1);
   o=sp_put(b,"KILLS ");o=sp_int(o,FP.kills);o=sp_put(o,"  DEATHS ");o=sp_int(o,FP.deaths);o=sp_put(o,"  ACC ");o=sp_int(o,acc);sp_put(o,"%");m3_ctext(66,b,C_GRAY,1);
   o=sp_put(b,"TIME ");o=sp_int(o,secs/60);*o++=':';*o++='0'+secs%60/10;*o++='0'+secs%10;*o=0;o=sp_put(o,"  HP ");sp_int(o,FP.hp<0?0:FP.hp);m3_ctext(78,b,C_GRAY,1);
   o=sp_put(b,"SCORE ");sp_int(o,sc);m3_ctext(94,b,C_WHITE,1);o=b;for(int k=0;k<3;k++)*o++=k<st?'*':'-';*o=0;m3_ctext(106,b,C_YEL,2);m3_ctext(128,"A: CONTINUE",C_GRAY,1);m3_present();}
  if(pressed&K_A){pressed=0;break;}if(sp_isauto()&&t>240)break;}
 return res;}
static const char*fm_nm(int i){return FMI[i].n;}
static const char*fm_rt(int i,char*b){char*o=b;for(int s=0;s<3;s++)*o++=s<FR.stars[i]?'*':'-';*o++=' ';sp_int(o,FR.best[i]);return b;}
static void fps_ops(void){fr_read();spCatOv=CAT_MP+1;int sel=0;bgm_t(TR_MODERN);
 for(;;){int i=sp_list("FPS VS BOTS","RAYCAST 3D - MODERN ERA",FM_N,fm_nm,fm_rt,sel,SCR_FPSEL);if(i<0){spCatOv=0;bgCached=-1;return;}sel=i;const FMis*M=&FMI[i];
  sp_brief(M->n,M->sub,M->l1,M->l2,"","UP/DOWN: MOVE  </>: TURN  L/R: STRAFE|A: FIRE  B: DOOR/RELOAD (HOLD: FAST TURN)|SELECT: SWITCH WEAPON  START: QUIT|ENEMIES DROP THEIR WEAPONS - TAKE THEM");
  BEAT->screen=SCR_FPSEL;BEAT->mode=i;if(sp_wait_ab(SCR_FPSEL))f_play(i);bgm_t(TR_MODERN);}}
static void rec_fps(const char**L){fr_read();static char b[4][32] EWRAM;int nd=0,ns=0;for(int i=0;i<FM_N;i++){nd+=FR.best[i]>0;ns+=FR.stars[i];}for(int i=0;i<3;i++){char t[16];char*o=sp_put(b[i],FMI[i].n);o=sp_put(o,": ");sp_put(o,fm_rt(i,t));L[i]=b[i];}{char*o=sp_put(b[3],"MISSIONS DONE ");o=sp_int(o,nd);o=sp_put(o,"/8  STARS ");sp_int(o,ns);L[3]=b[3];}L[4]="RAYCAST FPS VS BOTS";}
