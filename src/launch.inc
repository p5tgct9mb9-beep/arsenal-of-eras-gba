// ===================================================================== v1.3 SPACE LAUNCH
// Real launch vehicles (generic agency colour schemes, no logos). Countdown (TR_ASCENT) -> ascent with throttle, pitch,
// stage separation and max-Q stress -> optional booster landing / tower catch / lunar landing -> orbital debris + docking.
// Failures are always non-destructive aborts (escape / RTLS / safe deorbit). Best scores in SRAM 0x700 'AOES'.
#define SCR_LSEL 45
#define SCR_LAUNCH 46
#define LS_OFF 0x700
typedef struct{const char*n,*f;u8 ag,ns,nb,top,craft,land,kind;u8 w[3],h[3];u16 c[3];u16 dv[3],bt[3];u8 bw,bh,pad;u16 bc;}Rk;
#include "rocketdb.inc"
enum{LK_ORBIT,LK_SUB,LK_HOP,LK_LUNAR};enum{LL_NONE,LL_SHIP,LL_SIDE,LL_TOWER,LL_HOP};
static u16 lbest[NRK];static u8 lbonus[NRK];
static void ls_load(void){if(!(SRAM[LS_OFF]=='A'&&SRAM[LS_OFF+1]=='O'&&SRAM[LS_OFF+2]=='E'&&SRAM[LS_OFF+3]=='S'&&SRAM[LS_OFF+4]==1)){for(int i=0;i<NRK;i++)lbest[i]=lbonus[i]=0;return;}
 for(int i=0;i<NRK;i++){lbest[i]=SRAM[LS_OFF+8+i*2]|(SRAM[LS_OFF+9+i*2]<<8);lbonus[i]=SRAM[LS_OFF+8+NRK*2+i];}}
static void ls_save(void){SRAM[LS_OFF]='A';SRAM[LS_OFF+1]='O';SRAM[LS_OFF+2]='E';SRAM[LS_OFF+3]='S';SRAM[LS_OFF+4]=1;
 for(int i=0;i<NRK;i++){SRAM[LS_OFF+8+i*2]=lbest[i]&255;SRAM[LS_OFF+9+i*2]=lbest[i]>>8;SRAM[LS_OFF+8+NRK*2+i]=lbonus[i];}}
// ---------------------------------------------------------------- drawing helpers (clipped)
static void lf(int x,int y,int w,int h,u16 c){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;if(w<=0||h<=0)return;
 if(x==0&&w==240){fill32(&fb[y*240],c|(c<<16),120*h);return;}
 for(int j=0;j<h;j++){u16*p=&fb[(y+j)*240+x];int n=w;if(((u32)p)&2){*p++=c;n--;}if(n>=16){fill32(p,c|(c<<16),n>>1);p+=n&~1;n&=1;}while(n-->0)*p++=c;}}
static void ltx(int x,int y,const char*s,u16 col){ // fast 1x text with drop shadow (only touches set pixels)
 if(y<0||y>151)return;for(;*s;s++,x+=6){int c=*s;if(c>='a'&&c<='z')c-=32;if(c<=32||c>127)continue;if(x<0||x>233)continue;const u8*g=&font_bits[(c-32)*7];
  u16*row=&fb[y*240+x];for(int gy=0;gy<7;gy++,row+=240){u32 b=g[gy];int gx=0;while(b){if(b&1){row[gx+241]=0x0421;row[gx]=col;}b>>=1;gx++;}}
  row=&fb[y*240+x];for(int gy=0;gy<7;gy++,row+=240){u32 b=g[gy];int gx=0;while(b){if(b&1)row[gx]=col;b>>=1;gx++;}}}}
static void ltc(int y,const char*s,u16 col){ltx(120-(slen(s)*6-1)/2,y,s,col);}
static u16 lit(u16 c){int r=c&31,g=(c>>5)&31,b=(c>>10)&31;r=r+(31-r)/3;g=g+(31-g)/3;b=b+(31-b)/3;return r|(g<<5)|(b<<10);}
static void body(int x,int y,int w,int h,u16 c){if(w<=0||h<=0)return;lf(x,y,w,h,c);lf(x,y,1,h,lit(c));if(w>3)lf(x+w-2,y,2,h,dimc(c));}
static void cone(int cx,int ybot,int w,int h,u16 c){for(int j=0;j<h;j++){int ww=w*(j+1)/h;if(ww<1)ww=1;lf(cx-ww/2,ybot-h+j,ww,1,j==h-1?c:(j&1?c:lit(c)));}}
static void flame(int cx,int y,int w,int len){if(len<=0)return;for(int j=0;j<len;j++){int ww=w-(w*j)/(len+1)+((frame+j)&1);lf(cx-ww/2,y+j,ww,1,j<len/3?RGB(255,250,200):j<len*2/3?RGB(255,200,80):RGB(240,110,40));}}
static void bell(int cx,int y,int w){lf(cx-w/2+1,y,w-2,2,RGB(70,70,76));lf(cx-w/2,y+2,w,1,RGB(50,50,56));}
static int rk_hgt(const Rk*r,int cur){int h=0;for(int s=cur;s<r->ns;s++)h+=r->h[s];return h;}
static int rk_cw(const Rk*r){for(int s=0;s<r->ns;s++)if(r->w[s])return r->w[s];return 8;}
// draws the stack with its bottom at by; cur = first live stage; thr = 0..100 (0 = no flame)
static void draw_rk(const Rk*r,int cx,int by,int cur,int thr){
 int y=by,cw=0;int bot=1;
 for(int s=cur;s<r->ns;s++){int w=r->w[s],h=r->h[s];if(!h)continue;if(!cw)cw=w;
  if(r->top==4&&s==0)body(cx-w/2,y-h,w,h,r->c[s]);else body(cx-w/2,y-h,w,h,r->c[s]);
  if(s+1<r->ns)lf(cx-w/2,y-h,w,1,RGB(60,60,66));
  if(r->top==3&&s==1){} // shuttle ET: plain
  if(bot){bell(cx,y,w>10?w-4:w);if(thr)flame(cx,y+3,w-2,6+thr*(10+w)/100);bot=0;}
  if(r->top==4)for(int k=0;k<2;k++){lf(cx-w/2-2,y-h+2+k*(h-8),2,4,RGB(80,84,90));lf(cx+w/2,y-h+2+k*(h-8),2,4,RGB(80,84,90));}
  y-=h;}
 if(!cw)cw=rk_cw(r);int tw=r->w[r->ns-1]?r->w[r->ns-1]:cw;
 if(cur==0&&r->nb){int n=r->nb;for(int k=0;k<n;k++){int side=k&1?1:-1,lay=k>>1;int bx=cx+side*(cw/2+r->bw/2+lay*(r->bw-2));int bt=by-r->bh;
   u16 bc=lay?dimc(r->bc):r->bc;body(bx-r->bw/2,bt,r->bw,r->bh,bc);cone(bx,bt,r->bw,4,bc);bell(bx,by,r->bw);if(thr)flame(bx,by+3,r->bw,8+thr*12/100);}}
 switch(r->top){
  case 0:cone(cx,y,tw,tw+2,r->c[r->ns-1]==RGB(44,44,48)?RGB(236,236,232):lit(r->c[r->ns-1]));break;
  case 1:body(cx-tw/2-2,y-12,tw+4,12,RGB(236,236,232));cone(cx,y-12,tw+4,8,RGB(236,236,232));break;
  case 2:cone(cx,y,tw-2>7?7:tw-2,7,RGB(150,154,162));lf(cx,y-16,1,9,RGB(200,90,60));lf(cx-1,y-17,3,2,RGB(200,90,60));break;
  case 3:case 7:{u16 et=r->top==3?RGB(206,120,60):RGB(190,120,70);cone(cx,y,tw,8,et);int ox=cx+tw/2,oy=y+6;
   lf(ox,oy,7,34,RGB(236,236,232));lf(ox+5,oy+2,2,30,RGB(44,44,48));cone(ox+3,oy,7,5,RGB(44,44,48));
   for(int j=0;j<10;j++)lf(ox+7,oy+24+j,j,1,RGB(236,236,232));lf(ox+1,oy+36,5,2,RGB(70,70,76));lf(ox+1,oy+28,1,6,RGB(44,44,48));
   if(thr&&r->top==3)flame(ox+3,oy+38,5,5+thr*8/100);}break;
  case 4:cone(cx,y,tw,12,RGB(192,198,206));lf(cx-tw/2-2,y-10,2,4,RGB(80,84,90));lf(cx+tw/2,y-10,2,4,RGB(80,84,90));break;
  case 5:cone(cx,y,tw,8,r->craft==2?RGB(44,44,48):RGB(236,236,232));break;
  case 6:body(cx-tw/2-1,y-14,tw+2,14,RGB(150,156,136));cone(cx,y-14,tw+2,6,RGB(150,156,136));lf(cx,y-26,1,6,RGB(200,200,200));break;
  case 8:body(cx-tw/2,y-6,tw,6,RGB(236,236,232));cone(cx,y-6,tw,7,RGB(236,236,232));lf(cx,y-23,1,10,RGB(200,200,200));lf(cx-1,y-14,3,2,RGB(200,200,200));break;
  case 9:cone(cx,y,tw,4,RGB(200,200,206));for(int k=-1;k<=1;k+=2)lf(cx+k*(tw/2+1),by-6,1,6,RGB(60,60,66));break;}}
// craft for the orbit phase, nose to the right, centred on (x,y)
static void draw_craft(int k,int x,int y){u16 W=RGB(236,236,232),B=RGB(44,44,48),G=RGB(220,180,60),P=RGB(60,90,170),S=RGB(150,156,136);
 switch(k){
  case 1:for(int j=0;j<6;j++)lf(x-3+j,y-3+j/2,1,7-j,B);lf(x-5,y-1,2,3,G);break;
  case 2:lf(x-8,y-3,6,7,W);for(int j=0;j<6;j++)lf(x-2+j,y-3+j/2,1,7-j,B);break;
  case 3:lf(x-10,y-3,10,7,RGB(190,196,204));lf(x-12,y-2,2,5,B);for(int j=0;j<6;j++)lf(x+j,y-3+j/2,1,7-j,RGB(210,210,214));break;
  case 4:case 10:lf(x-14,y-2,22,5,W);lf(x-14,y+2,22,2,B);for(int j=0;j<5;j++)lf(x+8+j,y-2+j/2,1,5-j,B);for(int j=0;j<8;j++)lf(x-12+j,y+4,1,1+j/2,W);lf(x-14,y-6,3,4,W);
   if(k==4)lf(x-6,y-1,10,1,RGB(150,150,150));break;
  case 5:case 11:lf(x-9,y-4,6,9,k==5?B:W);lf(x-9,y-6,6,2,P);lf(x-9,y+5,6,2,P);for(int j=0;j<7;j++)lf(x-3+j,y-4+j/2,1,9-j,W);break;
  case 6:case 7:lf(x-12,y-3,8,7,S);lf(x-16,y-1,4,3,P);lf(x-16,y-6,4,4,P);lf(x-16,y+3,4,4,P);for(int j=0;j<5;j++)lf(x-4+j,y-3+j/3,1,7-j/2,S);lf(x+1,y-2,4,5,k==7?RGB(200,200,190):S);break;
  case 8:lf(x-10,y-3,8,7,W);lf(x-12,y-7,4,14,P);for(int j=0;j<7;j++)lf(x-2+j,y-4+j/2,1,9-j,RGB(200,200,206));break;
  case 9:lf(x-18,y-3,30,7,RGB(192,198,206));for(int j=0;j<6;j++)lf(x+12+j,y-3+j/2,1,7-j,RGB(192,198,206));lf(x-18,y-5,3,2,B);lf(x-18,y+4,3,2,B);lf(x+8,y-5,2,2,B);lf(x+8,y+4,2,2,B);lf(x-10,y-1,16,1,RGB(150,156,166));break;
  default:lf(x-6,y-3,9,7,G);lf(x-14,y-1,8,3,P);lf(x+3,y-1,4,3,RGB(170,170,176));lf(x+7,y,2,1,W);break;}}
static void stars(int sc,int n){for(int i=0;i<n;i++){int x=((i*97+sc*(1+(i&3)))%240+240)%240,y=(i*53)%150+4;fb[y*240+x]=(i&7)?RGB(150,150,170):RGB(255,255,255);}}
static void earth_limb(int e){if(e<=0)return;for(int x=0;x<240;x++){int dx=x-120;int t=160-e+dx*dx/(1600+e*20);if(t>=160)continue;lf(x,t,1,160-t,RGB(30,80,170));fb[t*240+x]=RGB(140,200,255);if(t+1<160)fb[(t+1)*240+x]=RGB(80,150,230);}}
// ---------------------------------------------------------------- physics helpers
static int isq(int v){if(v<=0)return 0;int r=0,b=1<<30;while(b>v)b>>=2;while(b){if(v>=r+b){v-=r+b;r=(r>>1)+b;}else r>>=1;b>>=2;}return r;}
static int deg2a(int d){return d*32/45;}
static const u16 RHO[41]={4096,3052,2275,1695,1263,941,701,523,389,290,216,161,120,89,67,50,37,28,21,15,11,9,6,5,4,3,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0}; // 4096*e^(-h/8.5km), 2.5 km steps
static int rho4k(int altm){int k=altm/2500;if(k<0)k=0;if(k>=40)return 0;int a=RHO[k],b=RHO[k+1];return a+(b-a)*(altm-k*2500)/2500;}
static int qdk(int v,int altm){if(v>10000)v=10000;return ((v*v>>8)*rho4k(altm)>>8)*98/1000;} // dynamic pressure, deci-kPa
static int lGp;
static int acos_deg(int c256){if(c256>=256)return 0;if(c256<=-51)return 101;for(int d=0;d<=101;d++)if(COS(deg2a(d))<=c256)return d;return 101;}
static void lnum1(char*o,int v){ // v in tenths -> "12.3"
 const char*s=itoa(v/10);int k=0;while(*s)o[k++]=*s++;o[k++]='.';o[k++]='0'+v%10;o[k]=0;}
static int lAuto,lScore,lIgn,lStage,lStress,lHull;static const char*lMsg;static int lMsgT;
static void lmsg(const char*s,int t){lMsg=s;lMsgT=t;}
typedef struct{s16 x,y,vx,vy,w,h,t;u16 c;u8 on,k;}Spent;static Spent spn[6];
static void spawn_spent(int x,int y,int w,int h,u16 c,int vx){for(int i=0;i<6;i++)if(!spn[i].on){Spent*s=&spn[i];s->on=1;s->x=x<<4;s->y=y<<4;s->w=w;s->h=h;s->c=c;s->vx=vx;s->vy=8;s->t=0;return;}}
// ---------------------------------------------------------------- landing minigames: 0 = fail, 1 = success
static int l_land(const Rk*r,int ri,int mode){ // mode: LL_SHIP/LL_SIDE/LL_TOWER/LL_HOP or 9 = lunar module
 int moon=mode==9,tower=mode==LL_TOWER,hop=mode==LL_HOP;
 int g=moon?104:627;int T=moon?380:hop?1150:1900;   // m/s^2 * 64
 int alt=(hop?0:moon?1500:tower?1500:1600)<<6,x=(hop?0:moon?-70:-60)<<6,vy=(hop?0:moon?-30:-130)<<6,vx=(hop?0:moon?10:9)<<6;
 int fuel=(moon?75:hop?40:tower?15:16)*30,fuel0=fuel,dtq=moon?4:2; // 30 frames/sim-sec, lunar runs at 2x
 int padw=moon?26:tower?6:12,armH=140,res=-1,endT=0,peak=0,thr=0,scale=moon?11:tower?11:hop?3:12;
 static const char*const TT[10]={"","BOOSTER RETURN: DRONE SHIP","SIDE BOOSTERS: LANDING ZONES","SUPER HEAVY: TOWER CATCH","GRASSHOPPER HOP: CLIMB 250 M","","","","","LUNAR MODULE: LAND ON THE MOON"};
 lmsg(TT[mode],150);bgm_t(TR_ASCENT);
 for(int fr=0;;fr++){BEAT->screen=SCR_LAUNCH;BEAT->phase=2;BEAT->x=alt>>6;BEAT->y=vy>>6;BEAT->extra=ri|(mode<<8);BEAT->scoreA=lScore;
  next_frame();
  for(int sub=0;sub<dtq;sub++){
   if(res<0){int up=(keys&(K_A|K_UP))!=0,lr=((keys&K_RIGHT)?1:0)-((keys&K_LEFT)?1:0);
    if(lAuto||(keys&K_SEL)){lAuto=1; // simple autopilot: aim for a gentle descent profile and drift to the pad
     int a=alt>>6,tv=hop&&peak<(262<<6)?60:hop?-(a/7+2):-(isq(24*a)+1);if(tower)tv=a>armH?-(isq(24*(a-armH))+1):1;if(moon)tv=-(a/12+1);
     up=(vy>>6)<tv;{int tvx=clampi(-(x>>6)/6,-8,8)<<6;lr=vx<tvx-48?1:vx>tvx+48?-1:0;}}
    if(fuel<=0)up=lr=0;thr=up;
    vy+=((up?T:0)-g)/30;vx+=lr*(moon?110:150)/30;if(up)fuel--;if(lr&&!(fr&3))fuel--;
    alt+=vy/30;x+=vx/30;if(alt>peak)peak=alt;
    int a=alt>>6;
    if(tower&&a>=armH-12&&a<=armH+12&&iabs(x>>6)<=padw&&iabs(vy>>6)<=4&&iabs(vx>>6)<=3){res=1;alt=armH<<6;vy=vx=0;}
    if(alt<=0&&(!hop||peak>(20<<6))){alt=0;
     if(tower)res=0;else res=(iabs(vy>>6)<=(moon?3:5)&&iabs(vx>>6)<=3&&iabs(x>>6)<=padw&&(!hop||peak>=(250<<6)))?1:0;vy=vx=0;}
    if(alt<0)alt=0,vy=vy<0?0:vy;
    if(res>=0){endT=fr;if(res)sfx_ok();else sfx_hurt();}}}
  if(res>=0&&fr-endT>120)break;
  // ---- draw
  int gy=148,sy=gy-(alt>>6)/scale,sx=120+(x>>6)/(hop?1:1)*(moon?1:1);
  if(moon){lf(0,0,240,160,RGB(0,0,6));stars(0,40);lf(0,gy,240,12,RGB(120,120,124));for(int i=0;i<9;i++){int cx=(i*61)%240;lf(cx,gy+3+(i%3)*2,8+(i%4)*3,2,RGB(90,90,96));}
   lf(120-padw,gy,padw*2,1,RGB(200,200,180));}
  else{for(int y=0;y<gy;y+=8){int k=y*12/gy;lf(0,y,240,8,RGB(80+k*6,140+k*5,220));}
   if(mode==LL_SHIP){lf(0,gy,240,12,RGB(30,70,140));for(int i=0;i<12;i++)lf((i*41+fr)%240,gy+2+(i&3)*2,6,1,RGB(90,140,200));lf(120-padw-6,gy-3,padw*2+12,5,RGB(110,110,118));lf(120-4,gy-3,8,1,RGB(240,240,240));}
   else{lf(0,gy,240,12,RGB(170,150,110));lf(120-padw,gy-1,padw*2,2,RGB(200,200,200));if(mode==LL_SIDE)lf(60-padw,gy-1,padw*2,2,RGB(200,200,200));}
   if(tower){int ty=gy-armH/scale-34;lf(150,ty,8,gy-ty,RGB(90,90,96));for(int j=ty;j<gy;j+=4)lf(150,j,8,1,RGB(130,130,136));int ay=gy-armH/scale-8;lf(126,ay,24,2,RGB(150,150,156));lf(126,ay+5,24,2,RGB(150,150,156));}
   if(hop){int my=gy-250/scale;for(int x2=0;x2<240;x2+=6)lf(x2,my,3,1,peak>(250<<6)?C_GREEN:C_YEL);ltx(4,my-9,"250 M",C_YEL);}}
  if(moon){lf(sx-5,sy-8,10,6,RGB(220,180,60));lf(sx-3,sy-12,6,4,RGB(190,190,196));lf(sx-7,sy-2,2,2,RGB(150,150,150));lf(sx+5,sy-2,2,2,RGB(150,150,150));if(thr&&res<0)flame(sx,sy,4,4+(frame&3));}
  else{int bw=mode==LL_TOWER?10:6,bh=mode==LL_TOWER?40:mode==LL_HOP?26:34;
   for(int k=0;k<(mode==LL_SIDE?2:1);k++){int bx=k?sx-60:sx;body(bx-bw/2,sy-bh,bw,bh,mode==LL_TOWER?RGB(192,198,206):hop?RGB(150,152,158):RGB(236,236,232));
    lf(bx-bw/2-2,sy-bh+1,2,2,RGB(70,70,76));lf(bx+bw/2,sy-bh+1,2,2,RGB(70,70,76));
    if((alt>>6)<300&&!tower){lf(bx-bw/2-3,sy-4,2,4,RGB(44,44,48));lf(bx+bw/2+1,sy-4,2,4,RGB(44,44,48));}
    if(thr&&res<0)flame(bx,sy,bw-1,6+(frame&3));}}
  if(res==0&&!moon&&!tower){for(int i=0;i<8;i++)lf(sx-12+i*3,gy-2-((fr+i*3)&7),2,2,mode==LL_SHIP?RGB(200,230,255):RGB(200,180,140));}
  {char b[16];ltx(4,4,cat("ALT ",itoa(alt>>6)),C_WHITE);ltx(4,13,cat("V/S ",itoa(vy>>6)),iabs(vy>>6)<=5?C_GREEN:C_WHITE);ltx(4,22,cat("DRIFT ",itoa(vx>>6)),iabs(vx>>6)<=3?C_GREEN:C_WHITE);
   ltx(160,4,"FUEL",C_GRAY);lf(186,5,50,5,RGB(30,30,40));lf(186,5,50*fuel/fuel0,5,fuel*4<fuel0?C_RED:C_GREEN);(void)b;}
  if(lMsgT>0){lMsgT-=2;ltc(36,lMsg,C_YEL);}
  if(res<0&&fr<240)ltc(140,"A/UP: ENGINE  L/R: DRIFT  SEL: AUTO",C_GRAY);
  if(res>=0)ltc(60,res?(moon?"THE EAGLE HAS LANDED!":tower?"CAUGHT BY THE TOWER!":mode==LL_SIDE?"BOTH BOOSTERS DOWN!":hop?"HOP COMPLETE!":"BOOSTER LANDED!"):(moon?"ABORT TO ORBIT - TRY AGAIN":tower?"MISSED THE CATCH - SAFE SPLASHDOWN":"BOOSTER LOST - SAFE AREA"),res?C_GREEN:C_RED);
  m3_present();}
 return res==1?1+fuel*100/fuel0:0;}
// ---------------------------------------------------------------- orbit: debris field + docking. returns 0 abort, 1 docked
typedef struct{s16 x,y,vx,vy;u8 on,k;}Deb;static Deb deb[10];
static int l_orbit(const Rk*r,int ri){int px=50<<8,py=80<<8,vx=0,vy=0,rcs=900,inv=0,dist=0,len=1300,stx=300<<8,res=-1,endT=0,sc=0;
 int lunar=r->kind==LK_LUNAR;for(int i=0;i<10;i++)deb[i].on=0;
 lmsg(lunar?"TRANSPOSITION + DOCKING: FETCH THE LM":r->craft==9?"DEBRIS FIELD - THEN THE ORBITAL DEPOT":"DEBRIS FIELD - THEN DOCK AT THE STATION",180);bgm_t(TR_CRUISE);
 int cw=r->craft==4||r->craft==10?8:r->craft==9?18:6;
 for(int fr=0;;fr++){BEAT->screen=SCR_LAUNCH;BEAT->phase=3;BEAT->x=px>>8;BEAT->y=py>>8;BEAT->hp=lHull;BEAT->extra=ri|(dist<<8);BEAT->scoreA=lScore;
  next_frame();
  for(int sub=0;sub<3;sub++){
   if(res<0){int dx=((keys&K_RIGHT)?1:0)-((keys&K_LEFT)?1:0),dy=((keys&K_DOWN)?1:0)-((keys&K_UP)?1:0);
    int portx=(stx>>8)-24,porty=lunar?84:76;
    if(lAuto||(keys&K_SEL)){lAuto=1;int ty=80;for(int i=0;i<10;i++)if(deb[i].on&&deb[i].x>px&&deb[i].x-px<(70<<8)&&iabs(deb[i].y-py)<(14<<8))ty=deb[i].y<py?(py>>8)+20:(py>>8)-20;
     if(dist>=len){ty=porty;}dy=(py>>8)<ty-1?((vy<0x80)?1:0):(py>>8)>ty+1?((vy>-0x80)?-1:0):(vy>0x20?-1:vy<-0x20?1:0);
     int tx=dist>=len?portx-cw:60,want=dist>=len?((tx-(px>>8))>20?0x100:0x60):0;if((px>>8)>=tx-2)want=0x50;dx=vx<want-0x20?1:vx>want+0x20?-1:0;}
    if(rcs<=0)dx=dy=0;if(dx||dy)rcs--;
    vx+=dx*0x10;vy+=dy*0x10;vx=clampi(vx,-0x280,0x280);vy=clampi(vy,-0x280,0x280);px+=vx;py+=vy;
    px=clampi(px,12<<8,226<<8);py=clampi(py,14<<8,146<<8);if((px>>8)==12||(px>>8)==226)vx=0;if((py>>8)==14||(py>>8)==146)vy=0;
    if(dist<len){dist++;if(!(fr&1)&&rr(100)<(sub?0:6+dist/120)){for(int i=0;i<10;i++)if(!deb[i].on){Deb*d=&deb[i];d->on=1;d->k=rr(10)<3?2:rr(2);d->x=244<<8;d->y=(16+rr(128))<<8;
       d->vx=-(d->k==2?0x500+rr(0x200):0x160+rr(0x180));d->vy=rr(0x60)-0x30;break;}}}
    else{int tgt=170<<8;if(stx>tgt)stx-=0x120;}
    if(inv>0)inv--;
    for(int i=0;i<10;i++)if(deb[i].on){Deb*d=&deb[i];d->x+=d->vx;d->y+=d->vy;if(d->x<-(20<<8)){d->on=0;continue;}
     int hw=d->k==0?10:d->k==1?9:2,hh=d->k==0?4:d->k==1?4:1;
     if(!inv&&iabs((d->x>>8)-(px>>8))<hw+cw&&iabs((d->y>>8)-(py>>8))<hh+3){lHull--;inv=60;d->on=0;sfx_hurt();lmsg(d->k==2?"MICROMETEOROID HIT!":d->k==0?"SATELLITE GLANCING BLOW!":"SPENT STAGE STRIKE!",60);
      if(lHull<=0){res=0;endT=fr;}}}
    if(dist>=len&&stx<=(172<<8)&&(px>>8)+cw>=portx){int off=iabs((py>>8)-porty);
     if(off<=3&&vx<=0xC0){res=1;endT=fr;vx=vy=0;px=(portx-cw)<<8;sfx_ok();}
     else{vx=-0x180;px-=0x400;if(!inv){lHull--;inv=60;sfx_hurt();lmsg(off>3?"MISALIGNED - BACK OFF":"TOO FAST - CLOSE BELOW 0.75",90);if(lHull<=0){res=0;endT=fr;}}}}
    if(rcs<=0&&res<0&&dist>=len&&vx<=0){res=0;endT=fr;lmsg("RCS DRY - SAFE DEORBIT",120);}}}
  if(res>=0&&fr-endT>100)break;
  sc+=1;
  // ---- draw
  lf(0,0,240,160,RGB(0,0,8));stars(-sc,48);earth_limb(26);
  for(int i=0;i<10;i++)if(deb[i].on){Deb*d=&deb[i];int x=d->x>>8,y=d->y>>8;
   if(d->k==0){lf(x-3,y-3,6,6,RGB(220,180,60));lf(x-11,y-1,7,3,RGB(60,90,170));lf(x+4,y-1,7,3,RGB(60,90,170));}
   else if(d->k==1){body(x-9,y-4,18,8,RGB(170,170,176));lf(x+9,y-3,2,6,RGB(70,70,76));}
   else lf(x-2,y,5,1,RGB(255,240,200));}
  if(dist>=len||stx<(260<<8)){int x=stx>>8,y=lunar?84:76;
   if(lunar){body(x-24,y-8,40,16,RGB(236,236,232));lf(x-24,y-6,4,12,RGB(220,180,60));lf(x-28,y-5,4,10,RGB(190,190,196));}
   else{lf(x-20,y-2,58,4,RGB(150,150,156));body(x-24,y-6,22,12,RGB(220,220,214));body(x-2,y-8,26,16,RGB(200,200,196));lf(x-25,y-1,2,3,RGB(60,60,66));
    for(int k=0;k<2;k++){lf(x+28,y-36+k*44,8,28,RGB(60,90,170));lf(x+38,y-36+k*44,8,28,RGB(60,90,170));lf(x-12,y-30+k*40,10,20,RGB(60,90,170));}}}
  if(!(inv&2))draw_craft(r->craft,px>>8,py>>8);
  ltx(4,4,"HULL",C_GRAY);for(int k=0;k<3;k++)lf(32+k*8,5,6,5,k<lHull?C_GREEN:RGB(60,60,70));
  ltx(64,4,"RCS",C_GRAY);lf(84,5,50,5,RGB(30,30,40));lf(84,5,50*rcs/900,5,rcs<200?C_RED:C_BLUE);
  if(dist<len){ltx(142,4,cat("RANGE ",itoa((len-dist)/13)),C_WHITE);}
  else{char b[8];lnum1(b,vx*10/256);ltx(142,4,cat("CLOSE ",b),vx<=0xC0?C_GREEN:C_RED);ltx(142,13,cat("OFFSET ",itoa((py>>8)-(lunar?84:76))),C_WHITE);}
  if(lMsgT>0){lMsgT-=2;ltc(24,lMsg,C_YEL);}
  if(fr<200&&res<0)ltc(150,"D-PAD: RCS  SELECT: AUTOPILOT",C_GRAY);
  if(res>=0)ltc(64,res?(lunar?"CSM DOCKED WITH THE LM!":"HARD DOCK - CAPTURE CONFIRMED!"):"MISSION ABORT - CREW/CRAFT SAFE",res?C_GREEN:C_RED);
  m3_present();}
 if(res==1)lScore+=3000+lHull*500+rcs*2;return res==1;}
// ---------------------------------------------------------------- countdown + ascent. returns 0 abort, 1 orbit/goal, 2 low/suborbit end
static int l_ascent(const Rk*r,int ri){
 int alt=0,vv=0,vh=0,pitch=0,thr=100,st=0,fuel=r->bt[0]*400,burn=0,cd=1200,ign=0,res=-1,endT=0,sepWait=-1,clean=0,early=0,apo=0;
 int clX[6],clY[6];for(int i=0;i<6;i++){clX[i]=rr(240);clY[i]=-rr(400);}for(int i=0;i<6;i++)spn[i].on=0;
 lStress=0;bgm_t(TR_ASCENT);int suborb=r->kind==LK_SUB;int shake_=0;
 for(int fr=0;;fr++){
  next_frame();
  for(int sub=0;sub<3;sub++){u16 pr=sub?0:pressed;
   if(cd>0){if(pr&K_START)cd=cd>360?360:cd;cd--;
    if(pr&K_A){if(cd<=40){ign=1;lIgn=cd<=24;cd=0;}}
    if(cd==0&&!ign)ign=1;if(cd==0){sfx_boom();lmsg(lIgn?"PERFECT IGNITION! +500":"IGNITION - LIFTOFF!",120);}continue;}
   if(res>=0)continue;
   int up=(keys&K_UP)!=0,dn=(keys&K_DOWN)!=0,lt=(keys&K_LEFT)!=0,rt=(keys&K_RIGHT)!=0;
   // guide (closed-loop above 90 km)
   int am=alt>>4,g64=627-(int)((vh>>6)*(vh>>6)/101109);
   int f=st<r->ns?(r->bt[st]*400-fuel)*256/(r->bt[st]*400):0;
   int A=st<r->ns&&fuel>0?(int)r->dv[st]*640/r->bt[st]*(154+f*205/256)/256:0; // full-throttle accel, m/s^2*64
   int gp=suborb?0:am<90000?85*isq(am)/316:0;if(gp>85)gp=85;
   if(!suborb&&am>=90000){int vt=(175000-am)/40;int need=g64+((vt<<6)-vv)/20;gp=A>0?acos_deg(need*256/A):90;gp=clampi(gp,30,100);}
   lGp=gp;
   if(lAuto||(keys&K_SEL)){lAuto=1;lt=pitch>gp;rt=pitch<gp;int q=qdk(isq((vv>>6)*(vv>>6)+(vh>>6)*(vh>>6)),am);up=q<275;dn=q>=285;
    if(st<r->ns&&fuel<=0)pr|=K_A;}
   if(up)thr+=2;if(dn)thr-=2;thr=clampi(thr,40,100);
   if(rt&&pitch<100)pitch++;if(lt&&pitch>0)pitch--;
   // stage separation
   if((pr&K_A)&&st<r->ns-1+(r->nb&&st==0?0:0)){if(st<r->ns-1){
     if(fuel>0)early++;else if(sepWait>=0&&sepWait<=60)clean++;
     int y=128-0;int cw=rk_cw(r);
     if(r->h[st])spawn_spent(120-r->w[st]/2,132-r->h[st],r->w[st],r->h[st],r->c[st],0);
     if(st==0&&r->nb){spawn_spent(120-cw/2-r->bw-2,132-r->bh,r->bw,r->bh,r->bc,-10);spawn_spent(120+cw/2+2,132-r->bh,r->bw,r->bh,r->bc,10);}
     (void)y;st++;fuel=r->bt[st]*400;sepWait=-1;sfx_whoosh();lmsg(st==1&&r->nb?"BOOSTER SEPARATION":"STAGE SEPARATION",80);}}
   int on=st<r->ns&&fuel>0;
   if(st<r->ns&&fuel<=0&&sepWait<0){sepWait=0;if(st<r->ns-1)lmsg("BURNOUT - PRESS A: STAGE!",120);}
   if(sepWait>=0)sepWait++;
   int a=on?A*thr/100:0;if(on){fuel-=thr;burn++;}
   int ca=COS(deg2a(pitch)),sa=SIN(deg2a(pitch));
   vv+=((a*ca>>8)-g64)/4;vh+=(a*sa>>8)/4;if(vh<0)vh=0;alt+=vv/16;
   am=alt>>4;if(am>apo)apo=am;
   int v=isq((vv>>6)*(vv>>6)+(vh>>6)*(vh>>6));int q=qdk(v,am);
   if(q>300){lStress+=(q-300)/10;shake_=4;}
   BEAT->scoreB=q;
   if(lStress>=1600){res=0;endT=fr;lmsg(r->top==3?"RTLS ABORT - ORBITER SAFE":r->top==2||r->top==6||r->top==8?"LAUNCH ESCAPE - CREW SAFE":"RANGE SAFETY ABORT",200);}
   if(!suborb&&am>=150000&&(vh>>6)>=7700){res=1;endT=fr;lmsg(r->ns>1?"ORBIT INSERTION - ENGINE CUTOFF":"ORBIT!",150);sfx_ok();}
   if(suborb&&st>=r->ns-1&&fuel<=0&&vv<0&&am>1000){res=apo>=187000?1:2;endT=fr;lmsg(apo>=187000?"APOGEE 187 KM - SPLASHDOWN!":"SUBORBITAL HOP - SPLASHDOWN",160);}
   if(!suborb&&st==r->ns-1&&fuel<=0&&res<0){if(am>100000&&(vh>>6)>6800){res=1;lmsg("LOW ORBIT - TRIM WITH RCS",150);}else{res=2;lmsg(r->top==2||r->top==5||r->top==6||r->top==8?"SUBORBITAL - CAPSULE SPLASHDOWN, CREW SAFE":"SUBORBITAL - SAFE REENTRY",200);}endT=fr;}
   if(fr>30&&alt<0){alt=0;vv=0;if(ign&&burn>40){res=0;endT=fr;lmsg("ABORT - CREW/VEHICLE SAFE",200);}}
   if(alt<0)alt=0,vv=vv<0?0:vv;}
  BEAT->screen=SCR_LAUNCH;BEAT->phase=cd>0?0:1;BEAT->x=alt>>4;BEAT->y=vh>>6;BEAT->extra=ri|(st<<8);BEAT->hp=lStress;BEAT->timer=cd;BEAT->kills=thr;BEAT->deaths=pitch;
  if(res>=0&&fr-endT>90)break;
  // ---- draw (30 fps)
  int am=alt>>4,km=am/1000;
  {int k=clampi(km,0,70);int R0,G0,B0;if(k<20){R0=100-k*3;G0=160-k*4;B0=235-k*3;}else if(k<45){R0=40-(k-20)*30/25;G0=80-(k-20)*64/25;B0=175-(k-20)*125/25;}else{R0=10-(k-45)*10/25;G0=16-(k-45)*16/25;B0=50-(k-45)*42/25;}
   lf(0,0,240,160,RGB(clampi(R0,0,255),clampi(G0,0,255),clampi(B0,0,255)));}
  if(km>22)stars(0,km>45?48:20);
  if(km>50)earth_limb(clampi((km-50)/3,0,36));
  int sk=shake_>0?((frame>>1)&1?1:-1):0;if(shake_>0)shake_--;
  int gy=134+am/10;if(gy<170){lf(0,gy,240,160-gy,RGB(90,130,70));lf(150,gy,90,160-gy,RGB(40,90,170));lf(90,gy-2,60,3,RGB(150,150,150));
   lf(96,gy-80,6,78,RGB(140,90,60));for(int j=gy-80;j<gy-2;j+=6)lf(96,j,10,1,RGB(170,110,70));if(cd>0){lf(102,gy-60,10,2,RGB(140,90,60));lf(102,gy-30,10,2,RGB(140,90,60));}}
  if(km<14&&cd==0){for(int i=0;i<6;i++){clY[i]+=(vv>>6)/40+1;if(clY[i]>170){clY[i]=-20-rr(200);clX[i]=rr(240);}lf(clX[i],clY[i],26,6,RGB(236,240,250));lf(clX[i]+5,clY[i]-3,14,4,RGB(236,240,250));}}
  for(int i=0;i<6;i++)if(spn[i].on){Spent*s=&spn[i];s->vy+=1;s->y+=s->vy;s->x+=s->vx;s->t++;if((s->y>>4)>170||s->t>200){s->on=0;continue;}body((s->x>>4)+sk,s->y>>4,s->w,s->h,s->c);}
  int by=132;
  draw_rk(r,120+sk,by,st,cd>0||res>=0||!(st<r->ns&&fuel>0)?0:thr);
  if(cd>0&&cd<240){for(int i=0;i<10;i++)lf(100+rr(40),by-2+rr(6),6,3,RGB(220,220,220));}
  // HUD
  {char b[12];lnum1(b,am/100);ltx(4,4,cat3("ALT ",b," KM"),C_WHITE);ltx(4,13,cat3("VEL ",itoa(isq((vv>>6)*(vv>>6)+(vh>>6)*(vh>>6)))," M/S"),C_WHITE);
   ltx(4,22,cat3("STAGE ",itoa(st+1<=r->ns?st+1:r->ns),cat("/",itoa(r->ns))),C_WHITE);
   ltx(4,140,"THR",C_GRAY);lf(26,141,40,5,RGB(30,30,40));lf(26,141,40*thr/100,5,C_YEL);
   ltx(4,150,"FUEL",C_GRAY);lf(32,151,34,5,RGB(30,30,40));if(st<r->ns)lf(32,151,34*fuel/(r->bt[st]*400),5,fuel*5<r->bt[st]*400?C_RED:C_GREEN);
   int q=BEAT->scoreB;ltx(172,140,"MAX-Q",q>300?C_RED:C_GRAY);lf(204,141,32,5,RGB(30,30,40));lf(204,141,32*clampi(q,0,450)/450,5,q>300?C_RED:C_BLUE);
   ltx(172,150,"STRESS",C_GRAY);lf(210,151,26,5,RGB(30,30,40));lf(210,151,26*clampi(lStress,0,1600)/1600,5,lStress>1000?C_RED:C_YEL);
   // attitude dial
   int ox=214,oy=34;for(int k=0;k<32;k++){int aa=k*8;m3_px(ox+(SIN(aa)*14>>8),oy-(COS(aa)*14>>8),RGB(120,130,150));}
   for(int k=0;k<14;k++){m3_px(ox+(SIN(deg2a(pitch))*k>>8),oy-(COS(deg2a(pitch))*k>>8),C_WHITE);}
   if(!suborb){int gp=lGp;m3_px(ox+(SIN(deg2a(gp))*16>>8),oy-(COS(deg2a(gp))*16>>8),C_YEL);m3_px(ox+(SIN(deg2a(gp))*17>>8),oy-(COS(deg2a(gp))*17>>8),C_YEL);}
   ltx(196,52,cat("P ",itoa(pitch)),C_WHITE);
   if(cd>0){m3_ctext(40,cat("T-",itoa((cd+119)/120)),C_YEL,2);ltc(60,cd<=40?"A: IGNITION!":"A AT T-0: IGNITION  START: SKIP",cd<=20?C_GREEN:C_GRAY);}
   else if(fr<300&&res<0)ltc(150-0,"",C_GRAY);
   if(cd==0&&fr<420&&res<0){ltc(30,"UP/DN THR  L/R PITCH  A STAGE",C_GRAY);ltc(39,"FOLLOW THE YELLOW GUIDE TICK",C_GRAY);}
   if(sepWait>=0&&st<r->ns-1&&(frame&8))ltc(70,"PRESS A: STAGE SEPARATION",C_RED);
   if(BEAT->scoreB>300&&(frame&8))ltc(80,"MAX-Q! THROTTLE DOWN",C_RED);
   (void)b;}
  if(lMsgT>0){lMsgT-=2;ltc(100,lMsg,C_YEL);}
  if(lAuto)ltx(90,13,"AUTOPILOT",C_BLUE);
  m3_present();
  }
 // ascent score
 if(res>=1){lScore+=(res==1?3000:1200)+(lIgn?500:0)+clean*250-early*150-lStress/4;
  if(suborb)lScore+=apo/50;else if(st<r->ns)lScore+=fuel/r->bt[st]/4;}
 else lScore+=apo/200;
 return res<0?0:res;}
static void l_results(const Rk*r,int ri,int ok,int bonus){int s=lScore<0?0:lScore;if(lAuto)s/=2;int rec=s/10>lbest[ri];
 if(rec)lbest[ri]=s/10>65535?65535:s/10;if(bonus)lbonus[ri]|=bonus;ls_save();if(ok)music_victory();
 for(int fr=0;;fr++){BEAT->screen=SCR_LAUNCH;BEAT->phase=4;BEAT->scoreA=s;BEAT->extra=ri|(bonus<<8);
  if(fr==0){m3_title(1);m3_fill(12,14,216,132,C_PANEL);m3_box(12,14,216,132,2);m3_ctext(22,r->n,C_YEL,1);m3_ctext(34,AGN[r->ag],C_GRAY,1);
   m3_ctext(52,ok?"MISSION COMPLETE":"MISSION ENDED - ALL SAFE",ok?C_GREEN:C_WHITE,1);
   m3_ctext(68,cat("SCORE ",itoa(s)),C_WHITE,2);if(lAuto)m3_ctext(88,"AUTOPILOT: HALF SCORE",C_BLUE,1);
   m3_ctext(100,cat("BEST ",itoa(lbest[ri]*10)),rec?C_YEL:C_GRAY,1);if(rec&&s)m3_ctext(110,"NEW RECORD!",C_YEL,1);
   if(bonus&1)m3_ctext(120,r->land==LL_TOWER?"BONUS: TOWER CATCH":r->land==LL_HOP?"BONUS: HOP + LANDING":r->kind==LK_LUNAR?"BONUS: LUNAR LANDING":"BONUS: BOOSTER LANDING",C_GREEN,1);
   m3_ctext(134,"A: CONTINUE",C_GRAY,1);m3_present();}
  next_frame();if(fr>20&&(pressed&(K_A|K_B|K_START))){sfx_ok();return;}}}
static void launch_run(int ri){const Rk*r=&RK[ri];lScore=0;lAuto=0;lIgn=0;lHull=3;lMsgT=0;int bonus=0,ok=0;
 if(r->kind==LK_HOP){int k=l_land(r,ri,LL_HOP);if(k){lScore+=5000+k*20;bonus|=1;ok=1;}else lScore+=500;l_results(r,ri,ok,bonus);return;}
 int a=l_ascent(r,ri);
 if(a==1&&r->land){int m=r->land;int k=l_land(r,ri,m);if(k){lScore+=(m==LL_TOWER?6000:m==LL_SIDE?5000:4000)+k*20;bonus|=1;}}
 if(a==1&&r->kind!=LK_SUB){ok=l_orbit(r,ri);
  if(ok&&r->kind==LK_LUNAR){int k=l_land(r,ri,9);if(k){lScore+=6000+k*30;bonus|=1;}}}
 else ok=a==1;
 l_results(r,ri,ok,bonus);}
// ---------------------------------------------------------------- select screen
static void space_launch(void){ls_load();static int ag=0,sel=0;int redraw=1;
 bgm_t(TR_ASCENT);
 for(;;){int idx[20],n=0;for(int i=0;i<NRK;i++)if(RK[i].ag==ag)idx[n++]=i;if(sel>=n)sel=n-1;int ri=idx[sel];const Rk*r=&RK[ri];
  BEAT->screen=SCR_LSEL;BEAT->menu=sel;BEAT->era=ag;BEAT->extra=ri;
  if(redraw){redraw=0;lf(0,0,240,160,RGB(6,8,22));stars(0,40);m3_box(2,2,236,156,0);
   m3_text(8,7,"SPACE LAUNCH",C_YEL,1);m3_text(88,7,cat3("< ",AGN[ag]," >"),C_BLUE,1);
   for(int k=0;k<n;k++){int y=17+k*8;m3_text(8,y,k==sel?">":" ",C_YEL,1);const char*nm=RK[idx[k]].n;m3_text(14,y,nm,k==sel?C_YEL:C_WHITE,1);if(lbonus[idx[k]]&1)m3_px(126,y+3,C_GREEN);}
   lf(130,18,104,108,RGB(10,14,34));lf(130,120,104,6,RGB(70,74,80));
   
   m3_text(132,128,cat("STAGES ",itoa(r->ns-(r->nb&&r->h[0]==0?1:0))),C_WHITE,1);if(r->nb)m3_text(186,128,cat("+",cat(itoa(r->nb)," BSTR")),C_GRAY,1);
   {const char*f=r->f;int L=slen(f),cut=L;if(L>17){cut=17;while(cut>0&&f[cut]!=' ')cut--;if(cut<6)cut=17;}char a[24],b[24];int i;for(i=0;i<cut&&i<23;i++)a[i]=f[i];a[i]=0;int j=0;for(i=cut+(f[cut]==' ');f[i]&&j<23;i++)b[j++]=f[i];b[j]=0;
    m3_text(132,137,a,C_GRAY,1);m3_text(132,145,b,C_GRAY,1);}
   m3_text(132,20,cat("HI ",itoa(lbest[ri]*10)),lbest[ri]?C_YEL:C_GRAY,1);
   if(r->land||r->kind==LK_LUNAR)m3_text(132,29,r->land==LL_TOWER?"CATCH":r->land==LL_HOP?"HOP+LAND":r->land==LL_SIDE?"2X LAND":r->kind==LK_LUNAR?"LUNAR LM":"LANDING",C_GREEN,1);
   draw_rk(r,206,120,0,0);
   m3_text(8,147,"L/R:AGENCY A:GO",C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+n-1)%n;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&(K_L|K_LEFT)){ag=(ag+NAGY-1)%NAGY;sel=0;redraw=1;sfx_menu();}
  if(pressed&(K_R|K_RIGHT)){ag=(ag+1)%NAGY;sel=0;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();launch_run(ri);bgm_t(TR_ASCENT);redraw=1;}
  if(pressed&K_B){sfx_menu();return;}}}
