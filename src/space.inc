// v1.3 SPACE PROGRAM: real flight profiles, satellite deploy, ISS docking, spacewalk (EVA)
#define SCR_SPSEL 71
#define SCR_SPFLY 72
#define SCR_SPORB 73
#define SCR_SPDEP 74
#define SCR_SPRDV 75
#define SCR_SPDOCK 76
#define SCR_SPEVA 77
#define SP_OFF 0xC00
// ---------------------------------------------------------------- small float math (no libm)
static float fsqr(float x){if(x<=0)return 0;union{float f;u32 i;}u;u.f=x;u.i=(u.i>>1)+0x1FC00000;float r=u.f;for(int i=0;i<4;i++)r=0.5f*(r+x/r);return r;}
#define FPI 3.14159265f
static float fwrap(float a){while(a>FPI)a-=2*FPI;while(a<-FPI)a+=2*FPI;return a;}
static float fsin(float a){a=fwrap(a);int neg=0;if(a<0){a=-a;neg=1;}if(a>FPI/2)a=FPI-a;float a2=a*a;float r=a*(1-a2/6*(1-a2/20*(1-a2/42*(1-a2/72))));return neg?-r:r;}
static float fcos(float a){return fsin(a+FPI/2);}
static float fatan2(float y,float x){float ax=x<0?-x:x,ay=y<0?-y:y;float mx=ax>ay?ax:ay,mn=ax>ay?ay:ax;if(mx<1e-9f)return 0;float t=mn/mx,t2=t*t;float r=t*(0.99997726f+t2*(-0.33262347f+t2*(0.19354346f+t2*(-0.11643287f+t2*(0.05265332f-0.01172120f*t2)))));if(ay>ax)r=FPI/2-r;if(x<0)r=FPI-r;if(y<0)r=-r;return r;}
static int fabsi(int v){return v<0?-v:v;}
static float ffabs(float v){return v<0?-v:v;}
// number formatting into caller buffer (avoid cat/itoa static clashes)
static char*sp_put(char*o,const char*s){while(*s)*o++=*s++;*o=0;return o;}
static char*sp_int(char*o,int v){char t[12];int n=0;int ng=v<0;if(ng)v=-v;do{t[n++]='0'+v%10;v/=10;}while(v);if(ng)*o++='-';while(n)*o++=t[--n];*o=0;return o;}
static char*sp_intc(char*o,int v){ // with thousands commas
 char t[16];int n=0,ng=v<0;if(ng)v=-v;int g=0;do{if(g==3){t[n++]=',';g=0;}t[n++]='0'+v%10;v/=10;g++;}while(v);if(ng)*o++='-';while(n)*o++=t[--n];*o=0;return o;}
static char*sp_dec1(char*o,int v10){if(v10<0){*o++='-';v10=-v10;}o=sp_int(o,v10/10);*o++='.';*o++='0'+v10%10;*o=0;return o;}
static char*sp_tm(char*o,int s){if(s<0){*o++='-';s=-s;}else*o++='+';int h=s/3600;if(h){o=sp_int(o,h);*o++=':';}int m=(s/60)%60;*o++='0'+m/10;*o++='0'+m%10;*o++=':';*o++='0'+(s%60)/10;*o++='0'+s%10;*o=0;return o;}
static void sp_bar(int x,int y,int w,int h,int v,int mx,u16 c){lf(x,y,w,h,RGB(30,32,40));int f=mx>0?w*clampi(v,0,mx)/mx:0;if(f>0)lf(x,y,f,h,c);}
static u16 sp_mix(u16 a,u16 b,int t){int r=((a&31)*(256-t)+(b&31)*t)>>8,g=(((a>>5)&31)*(256-t)+((b>>5)&31)*t)>>8,bb=(((a>>10)&31)*(256-t)+((b>>10)&31)*t)>>8;return r|(g<<5)|(bb<<10);}
static void sp_blend(int x,int y,int w,int h,u16 c,int t){u16*F=lfb?lfb:fb;if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>240)w=240-x;if(y+h>160)h=160-y;for(int j=0;j<h;j++){u16*p=F+(y+j)*240+x;for(int i=0;i<w;i++)p[i]=sp_mix(p[i],c,t);}}
static void sp_px(int x,int y,u16 c){if((unsigned)x<240&&(unsigned)y<160)(lfb?lfb:fb)[y*240+x]=c;}
static void sp_line(int x0,int y0,int x1,int y1,u16 c){int dx=fabsi(x1-x0),dy=-fabsi(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,e=dx+dy;for(int n=0;n<600;n++){sp_px(x0,y0,c);if(x0==x1&&y0==y1)break;int e2=2*e;if(e2>=dy){e+=dy;x0+=sx;}if(e2<=dx){e+=dx;y0+=sy;}}}
static void sp_circ(int cx,int cy,int r,u16 c){for(int a=0;a<64;a++){int x=cx+(COS(a*4)*r>>8),y=cy+(SIN(a*4)*r>>8);sp_px(x,y,c);}}
static void sp_disc(int cx,int cy,int r,u16 c){for(int j=-r;j<=r;j++){int w=isq(r*r-j*j);lf(cx-w,cy+j,2*w+1,1,c);}}
// ---------------------------------------------------------------- vehicles
enum{SV_SATV,SV_STS,SV_F9,SV_FH,SV_SS,SV_SOY,SV_SLS,SV_A5,SV_CZ5,SV_H2A,SV_ATL,SV_N};
enum{PL_RP1,PL_H2,PL_SRB,PL_CH4,PL_HYP};
#define C_WH RGB(236,236,230)
#define C_BK RGB(28,28,32)
#define C_ETO RGB(196,104,44)
#define C_SS RGB(178,182,190)
static void sp_body(int x,int y,int w,int h,u16 c){if(w<=0||h<=0)return;lf(x,y,w,h,c);lf(x,y,1,h,lit(c));if(w>3)lf(x+w-1,y,1,h,dimc(c));if(w>5)lf(x+w-2,y,1,h,sp_mix(c,dimc(c),128));}
static void sp_nose(int cx,int ybot,int w,int h,u16 c){for(int j=0;j<h;j++){int ww=w*(j+1)/h;if(j<h/3)ww=w*(j+1)*(j+1)/((h/3+1)*h)+1;if(ww<1)ww=1;if(ww>w)ww=w;lf(cx-ww/2,ybot-h+j,ww,1,c);if(ww>2)sp_px(cx-ww/2,ybot-h+j,lit(c));}}
static void sp_flag(int x,int y){lf(x,y,4,3,RGB(200,40,40));lf(x,y+1,4,1,C_WH);lf(x,y,2,2,RGB(40,60,160));}
// geometry (scale ~0.8 px per metre, widths x1.6) - returns height drawn; stage = number of stages dropped
// flags: bit0 white ET (STS-1/2), bit1 crew capsule on F9 (Dragon), bit2 fairing gone, bit3 LES/tower gone, bit4 grid fins out
static int sp_draw(int v,int cx,int by,int st,int fl){int y=by;
 switch(v){
 case SV_SATV:{ // S-IC 33 / S-II 20 / S-IVB 14 / IU 1 / SLA 7 / SM 5 / CM 3 / LES 8
  if(st<1){int h=33,w=13,x=cx-w/2;sp_body(x,y-h,w,h,C_WH);
    for(int j=6;j<h-8;j++){lf(x+w/4,y-j,w/4,1,C_BK);lf(x+w-2,y-j,2,1,C_BK);}           // roll pattern stripes (lower)
    for(int j=h-8;j<h-4;j++)for(int i=0;i<w;i++)if(((i*4/w)+(j>h-6))&1)sp_px(x+i,y-j,C_BK); // intertank checker
    lf(x-1,y-5,w+2,5,C_BK);lf(x-3,y-6,2,6,C_BK);lf(x+w+1,y-6,2,6,C_BK);                    // thrust skirt + fins
    for(int k=0;k<5;k++)lf(x+1+k*2+(k>2),y,2,2,RGB(70,70,76));                              // F-1 bells
    lf(x+w/2-1,y-h+4,1,9,RGB(30,40,120));sp_flag(x+2,y-h+10);y-=h;}
  if(st<2){int h=20,w=13,x=cx-w/2;sp_body(x,y-h,w,h,C_WH);lf(x,y-3,w,3,sp_mix(C_WH,C_BK,60));for(int i=0;i<w;i+=3)lf(x+i,y-h,1,2,C_BK);if(st==1)for(int k=0;k<3;k++)lf(x+2+k*3,y,2,2,RGB(80,80,86));y-=h;}
  {int h=14,w=9,x=cx-w/2;lf(cx-6,y-2,13,2,C_WH);sp_body(x,y-h,w,h,C_WH);lf(x,y-4,w/2,4,C_BK);lf(x+w/2,y-8,w-w/2,4,C_BK);if(st==2)lf(cx-1,y,3,2,RGB(80,80,86));y-=h;
   lf(x,y-1,w,1,RGB(150,150,150));y-=1;for(int j=0;j<7;j++){int ww=9-j*3/7*1-(j>3);lf(cx-ww/2,y-j-1,ww,1,j<1?C_WH:RGB(214,214,206));}y-=7;
   sp_body(cx-3,y-5,6,5,RGB(200,204,210));y-=5;sp_nose(cx,y,6,3,RGB(220,220,224));y-=3;
   if(!(fl&8)){lf(cx,y-6,1,6,RGB(140,40,30));lf(cx-1,y-8,3,3,C_WH);sp_px(cx,y-9,RGB(200,40,30));y-=9;}}
  break;}
 case SV_STS:{int et=fl&1?C_WH:C_ETO;int ex=cx-5;
  if(st<2){ // ET (37 tall, 11 wide) + ogive
   sp_body(ex,y-30-8,11,30,et);for(int j=4;j<30;j+=6)lf(ex,y-8-j,11,1,dimc(et));sp_nose(cx,y-38,11,9,et);lf(ex+5,y-47,1,2,RGB(150,150,150));
   if(st<1){ // SRBs: one in front-left, one peeking right
    int sx=ex-6;sp_body(sx,y-36,6,32,C_WH);sp_nose(sx+3,y-36,6,5,C_WH);lf(sx,y-4,6,4,RGB(60,60,64));lf(sx+1,y,4,2,RGB(70,70,76));lf(sx,y-24,6,1,RGB(150,150,150));
    lf(ex+11,y-36,2,32,sp_mix(C_WH,C_BK,70));sp_nose(ex+12,y-36,3,4,sp_mix(C_WH,C_BK,70));}}
  // orbiter on the right side of the ET: fuselage 30 tall
  {int ox=cx+6-(st>=2?8:0),oy=y-4;sp_body(ox,oy-28,6,28,C_WH);lf(ox,oy-28,2,28,C_BK);sp_nose(ox+3,oy-28,6,5,C_WH);lf(ox+1,oy-31,4,2,C_BK);
   lf(ox+6,oy-10,4,10,C_WH);lf(ox+9,oy-10,1,10,C_BK);lf(ox+6,oy-16,2,6,C_WH);             // wing (black leading edge)
   lf(ox+3,oy-8,4,4,RGB(230,230,226));lf(ox+3,oy-5,4,1,C_BK);                               // OMS pods
   lf(ox+6,oy-6,2,7,C_WH);lf(ox+7,oy-8,2,2,C_WH);                                           // tail fin
   for(int k=0;k<3;k++)lf(ox+1+k*2,oy,2,2,RGB(70,70,76));lf(ox+2,oy-24,2,2,RGB(40,48,70));}
  break;}
 case SV_F9:case SV_FH:{int w=6,x=cx-w/2;
  if(st<1){if(v==SV_FH){for(int s=-1;s<=1;s+=2){int bx=cx+s*7-w/2;sp_body(bx,y-33,w,33,C_WH);sp_nose(bx+w/2,y-33,w,6,C_WH);lf(bx,y-8,1,8,RGB(60,60,66));lf(bx+w-1,y-8,1,8,RGB(60,60,66));lf(bx+1,y,4,2,RGB(70,70,76));}}
   sp_body(x,y-33,w,33,C_WH);lf(x,y-8,1,8,RGB(60,60,66));lf(x+w-1,y-8,1,8,RGB(60,60,66));lf(x+1,y,4,2,RGB(70,70,76));y-=33;}
  else if(st==1&&v==SV_FH){sp_body(x,y-33,w,33,C_WH);lf(x+1,y,4,2,RGB(70,70,76));y-=33;}
  if(st<1||(v==SV_FH&&st<2)){lf(x,y-4,w,4,C_BK);if(fl&16){lf(x-2,y-4,2,2,RGB(90,90,96));lf(x+w,y-4,2,2,RGB(90,90,96));}else{lf(x-1,y-4,1,2,RGB(90,90,96));lf(x+w,y-4,1,2,RGB(90,90,96));}y-=4;}
  sp_body(x,y-10,w,10,C_WH);if((v==SV_FH?st==2:st==1))lf(x+2,y,2,2,RGB(70,70,76));y-=10;
  if(fl&2){lf(x,y-5,w,5,C_WH);lf(x,y-5,w/2,5,RGB(30,32,44));y-=5;sp_nose(cx,y,w,6,C_WH);lf(cx-1,y-4,1,1,RGB(40,50,80));y-=6;}
  else if(!(fl&4)){sp_body(x-1,y-8,w+2,8,C_WH);sp_nose(cx,y-8,w+2,6,C_WH);y-=14;}
  else{lf(x+1,y-4,w-2,4,RGB(170,170,176));y-=4;}
  break;}
 case SV_SS:{int w=12,x=cx-w/2;
  if(st<1){sp_body(x,y-57,w,57,C_SS);for(int j=6;j<57;j+=7)lf(x,y-j,w,1,sp_mix(C_SS,C_BK,40));lf(x-1,y-5,w+2,5,RGB(150,154,162));
   for(int k=0;k<5;k++)lf(x+1+k*2,y,2,2,RGB(80,80,90));lf(x-2,y-54,2,3,RGB(90,92,100));lf(x+w,y-54,2,3,RGB(90,92,100));y-=57;
   lf(x,y-3,w,3,RGB(60,62,68));for(int i=1;i<w;i+=2)lf(x+i,y-3,1,3,RGB(150,80,40));y-=3;}
  sp_body(x,y-34,w,34,C_SS);lf(x+w/2,y-34,w/2,34,RGB(30,30,34));for(int j=5;j<34;j+=6)lf(x,y-j,w/2,1,sp_mix(C_SS,C_BK,40));
  lf(x-2,y-8,2,6,RGB(30,30,34));lf(x+w,y-8,2,6,RGB(30,30,34));y-=34;sp_nose(cx,y,w,8,C_SS);lf(cx,y-8,w/2,8,0);for(int j=0;j<8;j++){int ww=w*(j+1)/8;lf(cx,y-8+j,ww/2,1,RGB(30,30,34));}
  lf(x-2,y-6,2,4,RGB(30,30,34));lf(x+w,y-6,2,4,RGB(30,30,34));y-=8;
  break;}
 case SV_SOY:{u16 G=RGB(172,180,170);int w=5,x=cx-w/2;
  if(st<1){for(int s=-1;s<=1;s+=2){int bx=cx+s*4;for(int j=0;j<16;j++){int ww=1+(16-j)*3/16;int px=s<0?bx-ww:bx;lf(px,y-j-1,ww,1,G);if((j&3)==0)sp_px(px,y-j-1,dimc(G));}lf(bx+(s<0?-3:1),y,2,2,RGB(70,70,76));lf(s<0?bx-1:bx,y-17,1,2,RGB(200,80,50));}}
  if(st<2){for(int j=0;j<22;j++){int ww=j<14?5-(j>10):4;lf(cx-ww/2,y-j-1,ww,1,G);}lf(x+1,y,3,2,RGB(70,70,76));if(st<1)lf(x,y-14,w,1,RGB(120,40,30));y-=22;
   for(int i=0;i<w;i+=2)lf(x+i,y-2,1,2,RGB(60,60,60));y-=2;}
  sp_body(x,y-7,w,7,G);if(st==2)lf(x+1,y,3,2,RGB(70,70,76));y-=7;
  if(!(fl&4)){sp_body(cx-3,y-8,7,8,C_WH);sp_nose(cx,y-8,7,4,C_WH);y-=12;if(!(fl&8)){lf(cx-1,y-2,1,2,RGB(90,90,90));lf(cx-2,y-3,2,1,RGB(90,90,90));lf(cx+1,y-3,2,1,RGB(90,90,90));lf(cx,y-7,1,7,RGB(200,200,200));y-=7;}}
  else{lf(cx-2,y-3,5,3,RGB(140,150,120));lf(cx-1,y-5,3,2,RGB(140,150,120));y-=5;}
  break;}
 case SV_SLS:{int w=11,x=cx-w/2;
  if(st<1){for(int s=-1;s<=1;s+=2){int bx=s<0?x-6:x+w;sp_body(bx,y-43,6,43,C_WH);sp_nose(bx+3,y-43,6,5,C_WH);lf(bx,y-4,6,4,RGB(60,60,64));lf(bx+1,y,4,2,RGB(70,70,76));for(int j=8;j<40;j+=9)lf(bx,y-j,6,1,RGB(40,40,46));}}
  if(st<2){sp_body(x,y-52,w,52,C_ETO);lf(x,y-6,w,6,C_WH);for(int k=0;k<4;k++)lf(x+1+k*2+(k>1),y,2,2,RGB(70,70,76));lf(x,y-52,w,2,C_WH);y-=52;}
  for(int j=0;j<5;j++){int ww=w-j;lf(cx-ww/2,y-j-1,ww,1,C_WH);}y-=5;sp_body(cx-2,y-6,5,6,C_WH);if(st>=2)lf(cx-1,y,3,2,RGB(70,70,76));y-=6;
  sp_body(cx-3,y-5,7,5,C_WH);y-=5;sp_nose(cx,y,7,4,C_WH);y-=4;if(!(fl&8)){lf(cx,y-6,1,6,RGB(220,220,220));lf(cx-1,y-2,3,1,C_BK);y-=6;}
  break;}
 case SV_A5:{int w=7,x=cx-w/2;
  if(st<1){for(int s=-1;s<=1;s+=2){int bx=s<0?x-4:x+w;sp_body(bx,y-25,4,25,C_WH);sp_nose(bx+2,y-25,4,4,C_WH);lf(bx,y-3,4,3,RGB(60,60,64));}}
  if(st<2){sp_body(x,y-24,w,24,C_WH);lf(x+2,y,3,2,RGB(70,70,76));lf(x+1,y-20,1,8,RGB(30,40,120));y-=24;}
  lf(x,y-3,w,3,RGB(60,60,66));y-=3;if(!(fl&4)){sp_body(x-1,y-11,w+2,11,C_WH);sp_nose(cx,y-11,w+2,6,C_WH);y-=17;}else{lf(cx-3,y-6,7,6,RGB(210,170,90));lf(cx-3,y-8,7,2,RGB(160,160,170));y-=8;}
  break;}
 case SV_CZ5:{int w=7,x=cx-w/2;
  if(st<1){for(int s=-1;s<=1;s+=2){int bx=s<0?x-5:x+w;sp_body(bx,y-22,5,22,C_WH);sp_nose(bx+2,y-22,5,6,C_WH);lf(bx+1,y,3,2,RGB(70,70,76));}}
  if(st<2){sp_body(x,y-33,w,33,C_WH);lf(x+1,y-26,2,4,RGB(200,40,40));lf(x+2,y,3,2,RGB(70,70,76));y-=33;}
  if(!(fl&4)){sp_body(x-1,y-14,w+2,14,C_WH);sp_nose(cx,y-14,w+2,6,C_WH);y-=20;}else{lf(cx-2,y-8,5,8,RGB(200,190,170));y-=8;}
  break;}
 case SV_H2A:case SV_ATL:{int w=6,x=cx-w/2;u16 c=v==SV_ATL?RGB(214,144,74):C_WH;
  if(st<1){for(int s=-1;s<=1;s+=2){int bx=s<0?x-3:x+w;sp_body(bx,y-14,3,14,C_WH);sp_nose(bx+1,y-14,3,3,C_WH);}}
  if(st<2){sp_body(x,y-26,w,26,c);if(v==SV_H2A)lf(x+1,y-20,1,6,RGB(30,40,140));lf(x+1,y,4,2,RGB(70,70,76));y-=26;}
  sp_body(x,y-9,w,9,C_WH);if(st>=2)lf(x+2,y,2,2,RGB(70,70,76));y-=9;if(!(fl&4)){sp_body(x-1,y-9,w+2,9,C_WH);sp_nose(cx,y-9,w+2,5,C_WH);y-=14;}else{lf(cx-2,y-5,5,5,RGB(210,180,90));y-=5;}
  break;}}
 return by-y;}
// engine plume: type, nozzle x, nozzle y, width, length, altitude km (expansion)
static void sp_plume(int t,int x,int y,int w,int len,int altk){int ex=altk>25?(altk-25)/6:0;if(ex>14)ex=14;
 for(int j=0;j<len;j++){int ww=w+(j*(2+ex))/12;int k=j*256/len;u16 c;int tr=0;
  switch(t){
  case PL_RP1:c=j<2?RGB(255,250,220):sp_mix(RGB(255,200,90),RGB(220,90,30),k);if(j>len*2/3)c=sp_mix(c,RGB(70,50,40),(k-170)*3);tr=altk>40?140:0;break;
  case PL_H2:c=RGB(200,220,255);tr=200;if((j%6)<2&&j<len*3/4){int dw=(j%6)?ww/2:ww/3;lf(x-dw/2,y+j,dw+1,1,RGB(255,236,200));continue;}break;
  case PL_SRB:c=j<len/2?RGB(255,252,236):sp_mix(RGB(255,240,200),RGB(255,180,90),(k-128)*2);break;
  case PL_CH4:c=sp_mix(RGB(150,170,255),RGB(190,120,255),k);tr=150;if((j%7)<2&&j<len*2/3){lf(x-ww/4,y+j,ww/2+1,1,RGB(240,230,255));continue;}break;
  default:c=sp_mix(RGB(255,220,140),RGB(240,140,60),k);tr=60;}
  ww+=(frame+j)&1;if(tr)sp_blend(x-ww/2,y+j,ww,1,c,256-tr);else lf(x-ww/2,y+j,ww,1,c);}}
// ---------------------------------------------------------------- missions (real profiles, approximate public data)
typedef struct{s16 t;u16 alt;u16 vel;u16 dr;u8 g;}SPp;           // t s, alt 0.1 km, vel km/h, downrange km, g x10
enum{EV_IGN=1,EV_LIFT,EV_ROLL,EV_THRD,EV_THRU,EV_MAXQ,EV_M1,EV_STAGE,EV_JETT,EV_CUT,EV_IGN2,EV_INFO,EV_BBACK,EV_ENTRY,EV_LAND,EV_ORBIT,EV_ROLLE};
typedef struct{s16 t;u8 ev,arg;const char*s;}SPe;
typedef struct{const char*n,*veh,*pad,*date,*orb;u8 v,fl,ns;s8 thrB;s16 cd;const SPp*p;u8 np;const SPe*e;u8 ne;u8 pl[3];u8 land;}SPm;
static const SPp P_A11[]={{0,0,0,0,12},{12,1,80,0,12},{30,16,380,0,13},{66,77,1100,2,17},{83,134,1850,5,19},{135,440,6600,50,39},{161,670,8600,93,39},{165,700,8650,100,8},{197,920,9200,135,9},{300,1250,13000,380,12},{460,1600,19500,1100,18},{548,1767,23500,1480,19},{552,1770,23600,1500,6},{699,1910,28000,2750,7}};
static const SPe E_A11[]={{-9,EV_IGN,0,"IGNITION SEQUENCE START"},{0,EV_LIFT,0,"LIFTOFF! WE HAVE LIFTOFF"},{12,EV_INFO,0,"TOWER CLEARED"},{13,EV_ROLL,72,"ROLL PROGRAM - AZIMUTH 72"},{31,EV_ROLLE,0,"ROLL COMPLETE - PITCH PROGRAM"},{66,EV_M1,0,"MACH 1"},{83,EV_MAXQ,0,"MAX-Q"},{135,EV_INFO,0,"S-IC CENTER ENGINE CUTOFF"},{161,EV_STAGE,1,"S-IC OUTBOARD CUTOFF - STAGING"},{165,EV_IGN2,0,"S-II IGNITION - 5 J-2"},{197,EV_JETT,8,"JETTISON LES TOWER"},{460,EV_INFO,0,"S-II CENTER ENGINE CUTOFF"},{548,EV_STAGE,2,"S-II CUTOFF - STAGING"},{552,EV_IGN2,0,"S-IVB IGNITION"},{699,EV_ORBIT,0,"S-IVB CUTOFF - ORBIT 191 KM"}};
static const SPp P_STS[]={{0,0,0,0,15},{7,1,90,0,15},{20,10,560,1,16},{26,25,700,1,15},{52,110,1550,6,14},{65,160,2000,10,17},{124,450,4900,50,25},{126,460,4950,52,9},{240,1000,7400,330,10},{360,1080,12500,700,15},{460,1110,22500,1150,30},{510,1130,27000,1450,30},{528,1140,27100,1520,0}};
static const SPe E_STS1[]={{-7,EV_IGN,0,"MAIN ENGINE START (T-6.6)"},{0,EV_LIFT,0,"SRB IGNITION - LIFTOFF"},{7,EV_ROLL,120,"ROLL PROGRAM - HEADS DOWN"},{20,EV_ROLLE,0,"ROLL COMPLETE"},{26,EV_THRD,65,"THROTTLE DOWN 65% (BUCKET)"},{52,EV_MAXQ,0,"MAX-Q"},{65,EV_THRU,100,"THROTTLE UP 100%"},{124,EV_STAGE,1,"SRB SEPARATION"},{240,EV_INFO,0,"NEGATIVE RETURN"},{460,EV_THRD,67,"3G THROTTLE LIMIT"},{510,EV_CUT,0,"MECO - MAIN ENGINE CUTOFF"},{528,EV_ORBIT,2,"ET SEPARATION - OMS-1 NEXT"}};
static const SPe E_STS31[]={{-7,EV_IGN,0,"MAIN ENGINE START (T-6.6)"},{0,EV_LIFT,0,"SRB IGNITION - LIFTOFF"},{7,EV_ROLL,120,"ROLL PROGRAM - HEADS DOWN"},{20,EV_ROLLE,0,"ROLL COMPLETE"},{26,EV_THRD,65,"THROTTLE DOWN 65% (BUCKET)"},{52,EV_MAXQ,0,"MAX-Q"},{65,EV_THRU,104,"THROTTLE UP 104%"},{124,EV_STAGE,1,"SRB SEPARATION"},{240,EV_INFO,0,"NEGATIVE RETURN"},{460,EV_THRD,67,"3G THROTTLE LIMIT"},{511,EV_CUT,0,"MECO - MAIN ENGINE CUTOFF"},{529,EV_ORBIT,2,"ET SEP - OMS-2 TO 618 KM"}};
static const SPp P_F9[]={{0,0,0,0,13},{12,3,180,0,13},{45,55,900,2,15},{62,110,1500,5,15},{100,280,3300,25,22},{150,660,7700,85,33},{154,690,7750,90,0},{162,740,7800,100,9},{300,1400,13000,500,13},{400,1700,18500,900,18},{527,2050,27300,1600,30}};
static const SPe E_DM2[]={{-3,EV_IGN,0,"ENGINE IGNITION - 9 MERLINS"},{0,EV_LIFT,0,"LIFTOFF - DEMO-2"},{45,EV_THRD,80,"THROTTLE DOWN (MAX-Q)"},{62,EV_MAXQ,0,"MAX-Q"},{75,EV_THRU,100,"THROTTLE UP"},{150,EV_CUT,0,"MECO"},{154,EV_STAGE,1,"STAGE SEPARATION"},{162,EV_IGN2,0,"SECOND ENGINE START"},{406,EV_ENTRY,0,"BOOSTER ENTRY BURN"},{511,EV_LAND,0,"LANDING BURN - OCISLY"},{527,EV_ORBIT,0,"SECO - DRAGON IN ORBIT"}};
static const SPe E_SL[]={{-3,EV_IGN,0,"ENGINE IGNITION - 9 MERLINS"},{0,EV_LIFT,0,"LIFTOFF - STARLINK"},{45,EV_THRD,80,"THROTTLE DOWN (MAX-Q)"},{62,EV_MAXQ,0,"MAX-Q"},{75,EV_THRU,100,"THROTTLE UP"},{147,EV_CUT,0,"MECO"},{151,EV_STAGE,1,"STAGE SEPARATION"},{158,EV_IGN2,0,"SECOND ENGINE START"},{166,EV_JETT,4,"FAIRING SEPARATION"},{374,EV_ENTRY,0,"BOOSTER ENTRY BURN"},{481,EV_LAND,0,"LANDING BURN - DRONESHIP"},{522,EV_ORBIT,0,"SECO - PARKING ORBIT"}};
static const SPp P_FH[]={{0,0,0,0,13},{45,55,900,2,15},{66,120,1600,5,15},{149,700,7900,85,30},{153,720,7950,90,15},{184,1000,9700,140,25},{187,1020,9750,145,0},{195,1060,9800,155,9},{330,1500,15000,600,14},{511,2100,27500,1650,28}};
static const SPe E_FH[]={{-3,EV_IGN,0,"ENGINE IGNITION - 27 MERLINS"},{0,EV_LIFT,0,"LIFTOFF - FALCON HEAVY"},{50,EV_THRD,80,"THROTTLE DOWN (MAX-Q)"},{66,EV_MAXQ,0,"MAX-Q"},{80,EV_THRU,100,"THROTTLE UP"},{149,EV_CUT,0,"BECO - SIDE BOOSTERS"},{153,EV_STAGE,1,"SIDE BOOSTER SEPARATION"},{184,EV_CUT,0,"CENTER CORE MECO"},{187,EV_STAGE,2,"STAGE SEPARATION"},{195,EV_IGN2,0,"SECOND ENGINE START"},{204,EV_JETT,4,"FAIRING SEPARATION"},{478,EV_LAND,0,"SIDE BOOSTERS LAND LZ-1/LZ-2"},{511,EV_ORBIT,0,"SECO - STARMAN IN ORBIT"}};
static const SPp P_SS[]={{0,0,0,0,14},{30,20,500,1,14},{60,110,1500,6,16},{120,450,5200,50,25},{162,650,6000,90,32},{165,670,6050,95,10},{300,1300,12000,450,13},{420,1450,20000,1000,20},{503,1500,26500,1600,26}};
static const SPe E_SS[]={{-3,EV_IGN,0,"RAPTOR IGNITION - 33 ENGINES"},{0,EV_LIFT,0,"LIFTOFF - STARSHIP FLIGHT 4"},{55,EV_THRD,75,"THROTTLE DOWN (MAX-Q)"},{62,EV_MAXQ,0,"MAX-Q"},{75,EV_THRU,100,"THROTTLE UP"},{162,EV_CUT,0,"BOOSTER MECO"},{165,EV_STAGE,1,"HOT STAGING"},{171,EV_BBACK,0,"BOOSTBACK BURN"},{424,EV_LAND,0,"BOOSTER LANDING BURN - GULF"},{503,EV_ORBIT,0,"SHIP SECO - NOMINAL"}};
static const SPp P_SOY[]={{0,0,0,0,14},{30,20,450,1,16},{70,110,1500,8,20},{118,420,5400,45,38},{122,440,5450,48,10},{287,1700,13000,520,36},{292,1720,13100,530,12},{525,2000,27000,1500,35}};
static const SPe E_SOY[]={{-20,EV_IGN,0,"ZAZHIGANIYE - IGNITION"},{0,EV_LIFT,0,"LIFTOFF - SOYUZ MS-22"},{114,EV_JETT,8,"ESCAPE TOWER JETTISON"},{118,EV_STAGE,1,"STRAP-ON SEP - KOROLEV CROSS"},{158,EV_JETT,4,"FAIRING JETTISON"},{287,EV_STAGE,2,"CORE STAGE SEPARATION"},{525,EV_CUT,0,"THIRD STAGE CUTOFF"},{528,EV_ORBIT,0,"SPACECRAFT SEPARATION"}};
static const SPp P_SLS[]={{0,0,0,0,15},{30,22,600,1,15},{70,140,1700,8,16},{132,480,5500,55,25},{134,490,5520,57,9},{300,1200,11500,500,12},{484,1620,28100,1800,30},{496,1620,28100,1850,0}};
static const SPe E_SLS[]={{-7,EV_IGN,0,"RS-25 ENGINE START (T-6.36)"},{0,EV_LIFT,0,"SRB IGNITION - LIFTOFF ARTEMIS I"},{7,EV_ROLL,90,"ROLL AND PITCH PROGRAM"},{20,EV_ROLLE,0,"ROLL COMPLETE"},{50,EV_THRD,85,"THROTTLE DOWN (MAX-Q)"},{70,EV_MAXQ,0,"MAX-Q"},{80,EV_THRU,109,"THROTTLE UP 109%"},{132,EV_STAGE,1,"BOOSTER SEPARATION"},{198,EV_JETT,8,"LAUNCH ABORT SYSTEM JETTISON"},{484,EV_CUT,0,"CORE STAGE MECO"},{496,EV_ORBIT,2,"CORE STAGE SEPARATION"}};
static const SPp P_A5[]={{0,0,0,0,13},{60,90,1200,4,15},{142,640,7200,60,32},{144,650,7210,62,8},{206,1100,7900,200,10},{526,1800,25000,1700,38},{540,1820,25100,1800,5},{1497,14000,36000,9000,25},{1627,14500,35000,10000,0}};
static const SPe E_A5[]={{-7,EV_IGN,0,"VULCAIN 2 IGNITION"},{0,EV_LIFT,0,"EAP IGNITION - LIFTOFF"},{142,EV_STAGE,1,"EAP BOOSTER SEPARATION"},{206,EV_JETT,4,"FAIRING JETTISON"},{526,EV_CUT,0,"EPC CUTOFF"},{532,EV_STAGE,2,"EPC SEPARATION"},{540,EV_IGN2,0,"ESC-D IGNITION"},{1497,EV_CUT,0,"ESC-D CUTOFF"},{1627,EV_ORBIT,0,"JWST SEPARATION!"}};
static const SPp P_CZ5[]={{0,0,0,0,13},{60,90,1200,4,15},{173,620,6800,70,30},{176,640,6850,72,10},{230,950,8200,170,12},{480,1800,26000,1500,30},{494,1810,26500,1600,0}};
static const SPe E_CZ5[]={{-3,EV_IGN,0,"YF-77 + YF-100 IGNITION"},{0,EV_LIFT,0,"LIFTOFF - TIANHE CORE MODULE"},{173,EV_STAGE,1,"BOOSTER SEPARATION"},{227,EV_JETT,4,"FAIRING JETTISON"},{480,EV_CUT,0,"CORE STAGE CUTOFF"},{494,EV_ORBIT,0,"TIANHE SEPARATION"}};
static const SPp P_H2A[]={{0,0,0,0,13},{60,90,1200,4,15},{108,420,4400,40,25},{110,430,4420,42,9},{250,1300,9500,350,13},{396,1650,19000,1100,27},{404,1660,19050,1150,5},{780,2500,27500,3500,10}};
static const SPe E_H2A[]={{-4,EV_IGN,0,"LE-7A IGNITION"},{0,EV_LIFT,0,"SRB-A IGNITION - LIFTOFF"},{108,EV_STAGE,1,"SRB-A SEPARATION"},{250,EV_JETT,4,"FAIRING JETTISON"},{396,EV_CUT,0,"MECO"},{404,EV_STAGE,2,"STAGE SEPARATION"},{412,EV_IGN2,0,"SECOND STAGE IGNITION"},{780,EV_ORBIT,0,"SECO-1 - PARKING ORBIT"}};
static const SPp P_ATL[]={{0,0,0,0,13},{60,90,1200,4,15},{110,350,4000,40,20},{240,1100,12000,400,30},{250,1110,12050,420,3},{620,1850,27000,2200,8}};
static const SPe E_ATL[]={{-3,EV_IGN,0,"RD-180 IGNITION"},{0,EV_LIFT,0,"LIFTOFF - LANDSAT 9"},{60,EV_MAXQ,0,"MAX-Q"},{210,EV_JETT,4,"FAIRING JETTISON"},{240,EV_CUT,0,"BECO"},{246,EV_STAGE,2,"ATLAS/CENTAUR SEPARATION"},{256,EV_IGN2,0,"CENTAUR MES-1"},{620,EV_ORBIT,0,"CENTAUR MECO-1"}};
#define NE(a) (sizeof(a)/sizeof(a[0]))
static const SPm SPM[]={
 {"APOLLO 11","SATURN V","LC-39A KENNEDY","16 JUL 1969","191 X 184 KM PARKING",SV_SATV,0,3,0,-15,P_A11,NE(P_A11),E_A11,NE(E_A11),{PL_RP1,PL_H2,PL_H2},0},
 {"STS-1","SHUTTLE COLUMBIA","LC-39A KENNEDY","12 APR 1981","246 X 240 KM (OMS)",SV_STS,1,2,65,-12,P_STS,NE(P_STS),E_STS1,NE(E_STS1),{PL_SRB,PL_H2,PL_H2},0},
 {"STS-31 HUBBLE","SHUTTLE DISCOVERY","LC-39B KENNEDY","24 APR 1990","618 KM - HUBBLE ORBIT",SV_STS,0,2,65,-12,P_STS,NE(P_STS),E_STS31,NE(E_STS31),{PL_SRB,PL_H2,PL_H2},0},
 {"CREW DEMO-2","FALCON 9 + CREW DRAGON","LC-39A KENNEDY","30 MAY 2020","200 KM - ISS CHASE",SV_F9,2,2,80,-10,P_F9,NE(P_F9),E_DM2,NE(E_DM2),{PL_RP1,PL_RP1,0},1},
 {"STARLINK","FALCON 9 BLOCK 5","SLC-40 CAPE CANAVERAL","2024","PARKING ORBIT",SV_F9,0,2,80,-10,P_F9,NE(P_F9),E_SL,NE(E_SL),{PL_RP1,PL_RP1,0},1},
 {"FALCON HEAVY DEMO","FALCON HEAVY","LC-39A KENNEDY","06 FEB 2018","HELIOCENTRIC (STARMAN)",SV_FH,0,3,80,-10,P_FH,NE(P_FH),E_FH,NE(E_FH),{PL_RP1,PL_RP1,PL_RP1},2},
 {"STARSHIP FLIGHT 4","STARSHIP + SUPER HEAVY","STARBASE TEXAS","06 JUN 2024","NEAR-ORBITAL",SV_SS,0,2,75,-10,P_SS,NE(P_SS),E_SS,NE(E_SS),{PL_CH4,PL_CH4,0},3},
 {"SOYUZ MS-22","SOYUZ-2.1A","BAIKONUR SITE 31","21 SEP 2022","200 KM - ISS FAST TRACK",SV_SOY,0,3,0,-25,P_SOY,NE(P_SOY),E_SOY,NE(E_SOY),{PL_RP1,PL_RP1,PL_RP1},0},
 {"ARTEMIS I","SLS BLOCK 1 + ORION","LC-39B KENNEDY","16 NOV 2022","TO THE MOON",SV_SLS,0,2,85,-12,P_SLS,NE(P_SLS),E_SLS,NE(E_SLS),{PL_SRB,PL_H2,0},0},
 {"JWST","ARIANE 5 ECA","KOUROU ELA-3","25 DEC 2021","L2 TRANSFER",SV_A5,0,3,0,-12,P_A5,NE(P_A5),E_A5,NE(E_A5),{PL_SRB,PL_H2,PL_H2},0},
 {"TIANHE","LONG MARCH 5B","WENCHANG LC-101","29 APR 2021","LEO - SPACE STATION",SV_CZ5,0,2,0,-10,P_CZ5,NE(P_CZ5),E_CZ5,NE(E_CZ5),{PL_RP1,PL_H2,0},0},
 {"XRISM + SLIM","H-IIA F47","TANEGASHIMA LA-Y1","07 SEP 2023","PARKING ORBIT",SV_H2A,0,3,0,-10,P_H2A,NE(P_H2A),E_H2A,NE(E_H2A),{PL_SRB,PL_H2,PL_H2},0},
 {"LANDSAT 9","ATLAS V 401","VANDENBERG SLC-3E","27 SEP 2021","705 KM SUN-SYNC",SV_ATL,0,3,0,-10,P_ATL,NE(P_ATL),E_ATL,NE(E_ATL),{PL_RP1,PL_RP1,PL_H2},0},
};
#define SPM_N 12   // flyable in the profile list (Landsat 9 used by satellite deploy)
// ---------------------------------------------------------------- SRAM records
typedef struct{u16 fly[SPM_N];u8 flyS[SPM_N];u16 sat[8];u8 satS[8];u16 iss[8];u8 issS[8];u16 eva[6];u8 evaS[6];u8 last,lastSat,lastIss,lastEva;}SPRec;
static SPRec SPR EWRAM;
static void spr_write(void){const u8*b=(const u8*)&SPR;u8 s=0x53;SRAM[SP_OFF]='A';SRAM[SP_OFF+1]='O';SRAM[SP_OFF+2]='E';SRAM[SP_OFF+3]='P';SRAM[SP_OFF+4]=1;for(int i=0;i<(int)sizeof SPR;i++){SRAM[SP_OFF+5+i]=b[i];s+=b[i]*(i+1);}SRAM[SP_OFF+5+sizeof SPR]=s;}
static void spr_read(void){memset(&SPR,0,sizeof SPR);if(SRAM[SP_OFF]!='A'||SRAM[SP_OFF+1]!='O'||SRAM[SP_OFF+2]!='E'||SRAM[SP_OFF+3]!='P'||SRAM[SP_OFF+4]!=1)return;u8 b[sizeof SPR];u8 s=0x53;for(int i=0;i<(int)sizeof SPR;i++){b[i]=SRAM[SP_OFF+5+i];s+=b[i]*(i+1);}if(s==SRAM[SP_OFF+5+sizeof SPR])memcpy(&SPR,b,sizeof SPR);}
static int sp_isauto(void){return *(volatile u32*)0x0203FFD0==0x4F545541u;}
// ---------------------------------------------------------------- flight state
typedef struct{s16 x,y,vx,vy,life,r;u8 k;}SPpart;   // x,y in 1/16 px (y world = altitude px)
typedef struct{s16 x,y,vx,vy,life,w,h;u16 c;u8 k;}SPdeb;
typedef struct{int tq,warp,ei,st,fl,roll,pitch,thr,thrT,score,guid,gerr,gn,maxq,auto_,iT,ign,lastEv,msgT,inset,insY,insV,landT,qwarn,done;const char*msg;u8 gr[20];s8 dt[20];}SPF;
static SPF F EWRAM;static SPpart SPP[56] EWRAM;static SPdeb SPD[8] EWRAM;
static void spf_msg(const char*s){F.msg=s;F.msgT=150;}
// interpolate profile at time tq (1/60 s): alt m, vel km/h, dr km, g x10
static void sp_prof(const SPm*m,int tq,int*alt,int*vel,int*dr,int*g){const SPp*p=m->p;int n=m->np;int t=tq;if(t<0)t=0;
 int i=0;while(i<n-2&&p[i+1].t*60<=t)i++;const SPp*a=&p[i],*b=&p[i+1];int span=(b->t-a->t)*60;int f=span>0?((t-a->t*60)<<10)/span:1024;if(f>1024)f=1024;if(f<0)f=0;
 *alt=(a->alt*100)+((b->alt-a->alt)*100*f>>10);*vel=a->vel+((b->vel-a->vel)*f>>10);*dr=a->dr+((b->dr-a->dr)*f>>10);*g=a->g+((b->g-a->g)*f>>10);
 if(tq<=0){*alt=0;*vel=0;*dr=0;*g=10;}}
// guidance pitch reference (deg from vertical) derived from the profile: atan2(horizontal, vertical speed)
static int sp_pitchref(const SPm*m,int tq){if(tq<10*60)return 0;int a0,v0,d0,g0,a1,v1,d1,g1;sp_prof(m,tq-60,&a0,&v0,&d0,&g0);sp_prof(m,tq+60,&a1,&v1,&d1,&g1);
 int vz=(a1-a0)/2;int v=(v0+v1)/2*10/36;if(v<1)v=1;int vh2=v*v-vz*vz;int vh=vh2>0?isq(vh2):0;int ang=atan2i(vh,vz<1?1:vz);int deg=ang*360/256;if(deg>90)deg=90;if(deg<0)deg=0;return deg;}
static int sp_stage_of_event(const SPm*m,int ei){int s=0;for(int i=0;i<ei&&i<m->ne;i++)if(m->e[i].ev==EV_STAGE)s=m->e[i].arg;return s;}
// fuel % for stage k at time t: burns from its ignition to its cutoff (from event table)
static int sp_fuel(const SPm*m,int k,int tq){int t0=0,t1=-1;int cur=0;for(int i=0;i<m->ne;i++){const SPe*e=&m->e[i];if(e->ev==EV_STAGE){if(cur==k){t1=e->t;break;}cur=e->arg;if(cur==k)t0=e->t;}}
 if(t1<0){t1=m->e[m->ne-1].t;}int t=tq/60;if(k>cur&&t<t0)return 100;if(t<=t0)return 100;if(t>=t1)return 0;return 100-(t-t0)*100/(t1-t0);}
static void sp_puff(int x,int y,int vx,int vy,int life,int r,int k){int b=0,bl=9999;for(int i=0;i<56;i++){if(SPP[i].life<=0){b=i;break;}if(SPP[i].life<bl){bl=SPP[i].life;b=i;}}SPpart*p=&SPP[b];p->x=x;p->y=y;p->vx=vx;p->vy=vy;p->life=life;p->r=r;p->k=k;}
static void sp_debris(int x,int y,int vx,int vy,int w,int h,u16 c,int k){for(int i=0;i<8;i++)if(SPD[i].life<=0){SPdeb*d=&SPD[i];d->x=x<<4;d->y=y<<4;d->vx=vx;d->vy=vy;d->w=w;d->h=h;d->c=c;d->k=k;d->life=240;return;}}
// pad scenery
static void sp_pad(const SPm*m,int gy,int cx,int rh){if(gy>=160)return;u16 sand=RGB(150,140,110),con=RGB(150,150,146);
 int water=m->pad[0]=='L'||m->pad[0]=='S'||m->pad[1]=='E';lf(0,gy,240,160-gy,water?RGB(140,150,110):RGB(170,150,110));if(water&&gy+14<160)lf(0,gy+14,240,160-gy-14,RGB(40,90,140));
 lf(cx-30,gy,60,4,con);lf(cx-8,gy-3,16,3,RGB(110,110,108));                                       // pad + flame trench top
 int tx=cx-24,th=rh+10;
 switch(m->v){
  case SV_SATV:for(int j=0;j<th;j+=4){lf(tx,gy-j-4,8,1,RGB(190,60,40));lf(tx,gy-j-4,1,4,RGB(190,60,40));lf(tx+7,gy-j-4,1,4,RGB(190,60,40));sp_line(tx,gy-j,tx+7,gy-j-4,RGB(160,50,34));}lf(tx-2,gy-th-6,12,2,RGB(190,60,40));lf(tx-4,gy,16,3,RGB(90,90,90));for(int k=0;k<9;k++)lf(tx+8,gy-12-k*10,cx-tx-14,1,RGB(180,60,40));break;
  case SV_STS:case SV_SLS:for(int j=0;j<th;j+=4){lf(tx,gy-j-4,8,1,RGB(150,150,154));lf(tx,gy-j-4,1,4,RGB(150,150,154));lf(tx+7,gy-j-4,1,4,RGB(150,150,154));}lf(tx+8,gy-th+6,cx-tx-14,2,RGB(130,130,134));lf(tx+3,gy-th-8,2,8,RGB(200,200,200));break;
  case SV_F9:case SV_FH:if(m->pad[0]=='L'){for(int j=0;j<th;j+=5)lf(tx,gy-j-5,7,4,RGB(60,62,66));lf(tx+7,gy-th+12,cx-tx-12,2,RGB(70,72,76));lf(tx+2,gy-th-10,2,10,RGB(190,190,190));}else{for(int k=0;k<4;k++)lf(tx-30+k*36+(k>1?60:0),gy-60,1,60,RGB(170,170,170));lf(cx-8,gy-rh+8,4,rh-8,RGB(100,100,104));}break;
  case SV_SS:lf(tx-6,gy-th-10,8,th+10,RGB(120,124,130));for(int j=0;j<th;j+=6)lf(tx-6,gy-j,8,1,RGB(90,92,98));lf(tx+2,gy-th+20,cx-tx-6,2,RGB(160,164,170));lf(tx+2,gy-th+26,cx-tx-6,2,RGB(160,164,170));lf(cx-14,gy-12,28,12,RGB(130,132,138));break;
  case SV_SOY:{int op=F.tq>0?clampi(F.tq/6,0,10):0;for(int s=-1;s<=1;s+=2){int bx=cx+s*(10+op);sp_line(bx,gy,bx+s*(-2+op/2),gy-30,RGB(150,150,150));lf(bx-1,gy-30-op/2,3,2,RGB(150,150,150));}break;}
  case SV_CZ5:for(int j=0;j<th;j+=4)lf(tx,gy-j-4,7,3,RGB(200,200,204));lf(tx+7,gy-th+14,cx-tx-12,2,RGB(170,170,174));break;
  default:lf(cx-20,gy-6,40,6,RGB(110,110,110));}
 ltx(4,gy+2<150?gy+2:150,m->pad,RGB(255,255,200));}
static void sp_sky(int altm){int a=altm/1000;u16 top,bot;
 if(a<12){top=sp_mix(RGB(70,120,200),RGB(20,40,110),a*20);bot=sp_mix(RGB(150,190,235),RGB(60,100,180),a*20);}
 else if(a<60){int t=(a-12)*256/48;top=sp_mix(RGB(20,40,110),RGB(2,2,8),t);bot=sp_mix(RGB(60,100,180),RGB(6,8,24),t);}
 else{top=RGB(2,2,8);bot=RGB(6,8,24);}
 for(int y=0;y<160;y+=4){u16 c=sp_mix(top,bot,y*256/160);lf(0,y,240,4,c);}
 if(a>30){int n=a>70?40:(a-30);stars(0,n);}if(a>90){int e=(a-90)/3;if(e>46)e=46;earth_limb(e);}}
// rocket nozzle layout per vehicle/stage: fills xs[] (offsets), types; returns count
static int sp_noz(const SPm*m,int st,s8*xo,u8*ty,u8*wd){int n=0;
#define NZ(o,t,w) {xo[n]=o;ty[n]=t;wd[n]=w;n++;}
 switch(m->v){
 case SV_SATV:NZ(0,m->pl[st],st==0?12:st==1?10:3);break;
 case SV_STS:if(st==0){NZ(-8,PL_SRB,5);}NZ(9-(st>=2?8:0),PL_H2,5);break;
 case SV_F9:NZ(0,st==0?PL_RP1:PL_RP1,st==0?6:3);break;
 case SV_FH:if(st==0){NZ(-7,PL_RP1,6);NZ(7,PL_RP1,6);}NZ(0,PL_RP1,st<2?6:3);break;
 case SV_SS:NZ(0,PL_CH4,st==0?12:8);break;
 case SV_SOY:if(st==0){NZ(-6,PL_RP1,3);NZ(6,PL_RP1,3);}NZ(0,PL_RP1,st<2?4:3);break;
 case SV_SLS:if(st==0){NZ(-8,PL_SRB,5);NZ(8,PL_SRB,5);}if(st<2)NZ(0,PL_H2,9)else NZ(0,PL_H2,3);break;
 case SV_A5:if(st==0){NZ(-6,PL_SRB,4);NZ(6,PL_SRB,4);}NZ(0,PL_H2,st<2?5:3);break;
 case SV_CZ5:if(st==0){NZ(-7,PL_RP1,4);NZ(7,PL_RP1,4);}NZ(0,PL_H2,5);break;
 default:if(st==0&&m->v==SV_H2A){NZ(-5,PL_SRB,3);NZ(5,PL_SRB,3);}NZ(0,st<2?(m->v==SV_ATL?PL_RP1:PL_H2):PL_H2,st<2?5:3);}
#undef NZ
 return n;}
static int sp_evkey(int ev){switch(ev){case EV_IGN:case EV_STAGE:case EV_LAND:return K_A;case EV_JETT:return K_SEL;case EV_THRD:return K_L;case EV_THRU:return K_R;}return 0;}
static const char*sp_evhelp(int ev){switch(ev){case EV_IGN:return "A: COMMAND IGNITION";case EV_STAGE:return "A: STAGE";case EV_LAND:return "A: LANDING BURN";case EV_JETT:return "SELECT: JETTISON";case EV_THRD:return "L: THROTTLE DOWN";case EV_THRU:return "R: THROTTLE UP";case EV_ROLL:return "HOLD RIGHT: ROLL TO TARGET";}return "";}
static void sp_grade(int i,int dtq){int a=dtq<0?-dtq:dtq;F.dt[i]=clampi(dtq/6,-99,99);int g=a<=30?3:a<=90?2:1;F.gr[i]=g;F.score+=g==3?300:g==2?150:50;spf_msg(g==3?"PERFECT TIMING +300":g==2?"GOOD +150":"LATE +50");}
static void sp_do_event(const SPm*m,int i,int auto_){const SPe*e=&m->e[i];
 switch(e->ev){
 case EV_IGN:F.ign=1;sfx_boom();break;
 case EV_STAGE:{int ns=e->arg;int cx=120,by=118;
   if(m->v==SV_SOY&&ns==1){for(int k=0;k<4;k++){int s=k&1?1:-1;sp_debris(cx+s*6,by-8,s*(k<2?14:6),8,3,15,RGB(172,180,170),k<2?1:2);}spf_msg("KOROLEV CROSS!");}
   else if(ns==1&&(m->v==SV_STS||m->v==SV_SLS||m->v==SV_A5||m->v==SV_CZ5||m->v==SV_H2A)){for(int s=-1;s<=1;s+=2)sp_debris(cx+s*9,by-14,s*10,10,5,24,C_WH,1);}
   else if(ns==1&&m->v==SV_FH){for(int s=-1;s<=1;s+=2)sp_debris(cx+s*8,by-16,s*8,6,6,33,C_WH,1);F.inset=1;F.insY=0;F.insV=0;}
   else{sp_debris(cx,by-10,0,14,m->v==SV_SATV?13:m->v==SV_SS?12:6,ns==1&&m->v==SV_SATV?33:20,m->v==SV_SS?C_SS:m->v==SV_SLS?C_ETO:C_WH,0);if(m->land&&ns==1){F.inset=1;F.insY=0;F.insV=0;}}
   F.st=ns;sfx_whoosh();break;}
 case EV_JETT:if(e->arg==4){for(int s=-1;s<=1;s+=2)sp_debris(120+s*4,80,s*12,-2,4,10,C_WH,3);}else{sp_debris(120,40,6,-20,2,9,RGB(200,200,200),4);}F.fl|=e->arg;sfx_whoosh();break;
 case EV_THRD:F.thrT=e->arg;break;case EV_THRU:F.thrT=e->arg;break;
 case EV_MAXQ:if(m->thrB&&F.thrT>m->thrB+5){F.qwarn=1;spf_msg("HIGH DYNAMIC PRESSURE! THROTTLE DOWN");F.score-=200;}break;
 case EV_LAND:F.landT=1;break;
 case EV_ORBIT:if(e->arg==2&&m->v==SV_STS){F.st=2;sp_debris(116,100,-4,10,11,40,m->fl&1?C_WH:C_ETO,0);}else if(m->v==SV_SLS&&e->arg==2){F.st=2;sp_debris(120,100,0,10,11,40,C_ETO,0);}F.done=1;break;}
 if(!F.msgT||e->ev!=EV_STAGE||m->v!=SV_SOY)spf_msg(e->s);}
static void sp_hud(const SPm*m,int alt,int vel,int dr,int g){char b[48],*o;
 lf(0,0,240,10,RGB(8,10,20));o=sp_put(b,"T");sp_tm(o,F.tq/60);ltx(2,1,b,F.tq<0?RGB(255,220,120):RGB(140,255,160));ltx(56,1,m->n,RGB(255,220,120));
 o=sp_put(b,"X");o=sp_int(o,F.warp);ltx(222,1,b,F.warp>1?RGB(255,160,80):RGB(150,150,170));
 lf(176,12,62,82,RGB(8,10,20));
 o=sp_put(b,"ALT ");o=sp_dec1(o,alt/100);ltx(178,14,b,C_WHITE);ltx(178,22,"KM",RGB(130,130,150));
 o=sp_put(b,"VEL ");o=sp_intc(o,vel);ltx(178,30,b,C_WHITE);ltx(178,38,"KM/H",RGB(130,130,150));
 o=sp_put(b,"DR ");o=sp_intc(o,dr);o=sp_put(o," KM");ltx(178,46,b,C_WHITE);
 o=sp_put(b,"THR ");o=sp_int(o,F.thr);o=sp_put(o,"%");ltx(178,54,b,F.thr<100?RGB(255,220,120):C_WHITE);
 o=sp_put(b,"G ");o=sp_dec1(o,g);ltx(178,62,b,g>=30?RGB(255,160,80):C_WHITE);
 for(int k=0;k<m->ns;k++){int f=sp_fuel(m,k,F.tq);if(k<F.st)f=0;b[0]='S';b[1]='1'+k;b[2]=0;ltx(178,71+k*7,b,RGB(150,150,170));sp_bar(192,72+k*7,42,4,f,100,f>20?RGB(90,200,120):RGB(230,90,60));}
 // ADI: guidance chevron vs vehicle attitude
 lf(2,12,40,40,RGB(8,10,20));int ref=sp_pitchref(m,F.tq);{int a=deg2a(F.pitch);int x1=22+(SIN(a)*16>>8),y1=32-(COS(a)*16>>8);sp_line(22,32,x1,y1,C_WHITE);}
 {int a=deg2a(ref);int x1=22+(SIN(a)*18>>8),y1=32-(COS(a)*18>>8);sp_px(x1,y1,RGB(255,80,200));sp_px(x1+1,y1,RGB(255,80,200));sp_px(x1,y1+1,RGB(255,80,200));}
 o=sp_put(b,"P");o=sp_int(o,F.pitch);ltx(3,44,b,fabsi(F.pitch-ref)>4?RGB(255,160,80):RGB(140,255,160));
 if(F.roll>=0){o=sp_put(b,"ROLL ");o=sp_int(o,F.roll);ltx(2,54,b,C_WHITE);}
 if(F.msgT>0){ltc(140,F.msg,RGB(255,240,160));}
 // next input cue
 if(F.ei<m->ne){const SPe*e=&m->e[F.ei];int k=sp_evkey(e->ev);int dt=e->t*60-F.tq;if((k||e->ev==EV_ROLL)&&dt<240){lf(0,150,240,10,RGB(30,20,10));ltc(151,sp_evhelp(e->ev),dt<=30?RGB(255,255,120):RGB(255,180,90));
   char c[8];o=sp_put(c,"");sp_int(o,dt>0?dt/60:0);if(dt>0)ltx(4,151,c,RGB(255,220,120));}}}
static void sp_inset(const SPm*m){ // booster recovery camera
 if(!F.inset)return;int x0=2,y0=96,w=64,h=50;lf(x0,y0,w,h,RGB(20,30,50));lf(x0,y0,w,1,RGB(120,140,170));lf(x0,y0+h-1,w,1,RGB(120,140,170));ltx(x0+2,y0+2,m->v==SV_SS?"SUPER HEAVY":"BOOSTER CAM",RGB(150,200,255));
 int land=m->land==3?0:1;lf(x0+1,y0+h-8,w-2,7,land?RGB(30,60,110):RGB(30,70,120));if(m->land==1)lf(x0+18,y0+h-10,28,3,RGB(70,70,74));if(m->land==2){lf(x0+6,y0+h-9,20,2,RGB(150,150,140));lf(x0+38,y0+h-9,20,2,RGB(150,150,140));}
 int by=y0+12+F.insY/64;if(by>y0+h-10)by=y0+h-10;int bx=x0+w/2;
 if(m->land==2){for(int s=-1;s<=1;s+=2){int xx=bx+s*16;lf(xx-2,by-24,4,24,C_WH);lf(xx-2,by-24,4,3,C_BK);if(F.landT||F.insY>2400){lf(xx-4,by-2,2,2,C_BK);lf(xx+2,by-2,2,2,C_BK);}if(F.landT&&F.insY<2600)sp_plume(PL_RP1,xx,by,3,8,0);}}
 else{u16 c=m->v==SV_SS?C_SS:C_WH;int bw=m->v==SV_SS?6:4;lf(bx-bw/2,by-26,bw,26,c);lf(bx-bw/2,by-26,bw,3,m->v==SV_SS?RGB(90,92,100):C_BK);lf(bx-bw/2-2,by-25,2,2,RGB(90,90,96));lf(bx+bw/2,by-25,2,2,RGB(90,90,96));
  if(F.landT||F.insY>2400){lf(bx-bw/2-2,by-3,2,3,C_BK);lf(bx+bw/2,by-3,2,3,C_BK);}
  if((F.landT&&F.insY<2600)||F.inset==2)sp_plume(m->v==SV_SS?PL_CH4:PL_RP1,bx,by,3,10,0);}
 if(F.insY>=2600)ltx(x0+4,y0+h-18,m->land==3?"SOFT SPLASH":"LANDED!",RGB(140,255,160));}
static void sp_fly_draw(const SPm*m,int alt,int vel,int g,int dAlt){
 int by=118,altpx=alt*4/5;if(F.tq<=0||alt<27){by=140-alt*4/5;if(by<118)by=118;}
 sp_sky(alt);
 // clouds (world)
 for(int k=0;k<4;k++){int ca=1800+k*1900,cy=by-(ca*4/5-altpx)/8;int cx=((k*83+F.tq/40)%300)-30;if(cy>-20&&cy<170){u16 c=RGB(236,236,240);lf(cx,cy,40,6,c);lf(cx+8,cy-4,22,4,c);lf(cx+4,cy+6,30,3,RGB(210,210,220));}}
 int gy=by+altpx;int rh=0;
 // steam/smoke particles
 for(int i=0;i<56;i++){SPpart*p=&SPP[i];if(p->life<=0)continue;p->life--;p->x+=p->vx;p->y+=p->vy+dAlt*16/5*4;if(p->k==0&&p->r<14&&(p->life&7)==0)p->r++;int x=p->x>>4,y=p->y>>4;if(y>170||y<-20){p->life=0;continue;}
  u16 c=p->k==1?RGB(250,250,250):sp_mix(RGB(200,200,200),RGB(120,120,120),clampi(255-p->life,0,255));if(p->k==2)c=RGB(255,240,200);sp_disc(x,y,p->r,c);}
 sp_pad(m,gy,120,rh?rh:90);
 // debris
 for(int i=0;i<8;i++){SPdeb*d=&SPD[i];if(d->life<=0)continue;d->life--;d->x+=d->vx;d->y+=d->vy;d->vy+=1;int x=d->x>>4,y=d->y>>4;if(y>170){d->life=0;continue;}
  int w=d->w,h=d->h;if(d->k==2){w=w*2/3+1;h=h*2/3+1;}if(d->k==3){int s=d->vx<0?-1:1;for(int j=0;j<h;j++){int ww=j<4?j+1:w;lf(s<0?x-ww:x,y+j,ww,1,d->c);}continue;}
  if(d->k==1&&(F.tq&8))sp_body(x-w/2,y,w,h,d->c);else sp_body(x-w/2,y,w,h,d->c);lf(x-w/2,y+h-2,w,2,dimc(d->c));}
 int lit=F.ign&&!F.done;rh=sp_draw(m->v,120,by,F.st,F.fl);
 if(lit){s8 xo[4];u8 ty[4],wd[4];int n=sp_noz(m,F.st,xo,ty,wd);int ak=alt/1000;
  for(int k=0;k<n;k++){int L=(F.tq<0?6:14)+F.thr/8+(ak>20?ak/4:0);if(L>60)L=60;if(F.tq<0&&ty[k]==PL_SRB)continue;sp_plume(ty[k],120+xo[k],by+2,wd[k],L,ak);
   if(ak<25&&(F.tq&1)&&(ty[k]==PL_SRB||ty[k]==PL_RP1)&&F.tq>0)sp_puff((120+xo[k]+rr(5)-2)<<4,(by+L)<<4,rr(9)-4,8,140,ty[k]==PL_SRB?5:3,0);}
  if(F.tq>-60&&alt<30&&(F.tq&1)){for(int s=-1;s<=1;s+=2)sp_puff((120+s*20)<<4,(gy-4)<<4,s*(16+rr(16)),-rr(6),200,4,1);}}
 sp_inset(m);}
static int sp_fly(int mi){const SPm*m=&SPM[mi];memset(&F,0,sizeof F);memset(SPP,0,sizeof SPP);memset(SPD,0,sizeof SPD);F.tq=m->cd*60;F.warp=1;F.roll=-1;F.thrT=0;F.auto_=sp_isauto();bgm_t(TR_ASCENT);
 int prevAlt=0,endT=0,ri=-1,rollT=-1,rollG=0;
 for(;;){next_frame();int alt,vel,dr,g;
  // input
  u16 pr=pressed,ky=keys;if(F.auto_){pr=0;ky=0;if(F.ei<m->ne){const SPe*e=&m->e[F.ei];int k=sp_evkey(e->ev);int dt=e->t*60-F.tq;if(k&&dt<=6&&dt>-6)pr|=k;if(e->ev==EV_ROLLE&&F.roll>=0&&F.roll<rollG)ky|=K_RIGHT;}
   if(F.ei<m->ne){int dt=m->e[F.ei].t*60-F.tq;F.warp=dt>400?16:1;}int ref=sp_pitchref(m,F.tq);if(F.pitch<ref)ky|=K_UP;else if(F.pitch>ref)ky|=K_DOWN;}
  if(pr&K_B){F.warp=F.warp==1?4:F.warp==4?16:1;sfx_menu();}
  if(pressed&K_START){if(endT==0)F.auto_^=0;}
  // keyed-event grading
  if(F.ei<m->ne){const SPe*e=&m->e[F.ei];int k=sp_evkey(e->ev);int dt=e->t*60-F.tq;
   if(k&&F.warp>1&&dt<240)F.warp=1;
   if(k&&(pr&k)&&dt<=180){int gi=F.ei;sp_grade(gi,-dt);sp_do_event(m,F.ei,0);F.ei++;}
   else if(e->ev==EV_ROLLE&&F.tq>=e->t*60){int er=fabsi(F.roll-rollG);int gg=er<=3?3:er<=10?2:1;F.gr[F.ei]=gg;F.score+=gg==3?300:gg==2?150:50;spf_msg(gg==3?"ROLL ON TARGET +300":"ROLL COMPLETE");sp_do_event(m,F.ei,1);F.roll=-1;F.ei++;}
   else if(F.tq>=e->t*60+(k?180:0)){if(k){F.gr[F.ei]=0;spf_msg("AUTO SEQUENCER");}if(e->ev==EV_ROLL){F.roll=0;rollG=e->arg;rollT=e->t;}sp_do_event(m,F.ei,1);F.ei++;}}
  // roll/pitch/throttle
  if(F.roll>=0&&(ky&K_RIGHT)&&(F.tq&1))F.roll++;if(F.roll>=0&&(ky&K_LEFT)&&(F.tq&1)&&F.roll>0)F.roll--;
  if(F.tq>8*60&&!F.done){if((ky&K_UP)&&(F.tq%4==0))F.pitch++;if((ky&K_DOWN)&&(F.tq%4==0))F.pitch--;if(F.warp>1)F.pitch=sp_pitchref(m,F.tq);
   if(F.warp==1&&(F.tq%90)==0&&!F.auto_)F.pitch+=rr(3)-1;F.pitch=clampi(F.pitch,0,95);if((F.tq%60)==0&&F.warp==1){F.gerr+=fabsi(F.pitch-sp_pitchref(m,F.tq));F.gn++;}}
  if(F.ign&&F.thrT==0)F.thrT=100;if(F.done)F.thrT=0;if(F.thr<F.thrT&&(F.tq&1))F.thr++;if(F.thr>F.thrT&&(F.tq&1))F.thr--;if(F.thr<F.thrT-30)F.thr+=3;
  // advance time
  if(!F.done)F.tq+=F.warp;else endT++;
  sp_prof(m,F.tq,&alt,&vel,&dr,&g);int dA=(alt-prevAlt)*4/5;if(dA<0)dA=0;if(dA>200)dA=200;prevAlt=alt;
  if(F.inset){if(F.landT){F.insY+=F.insY<2600?(2600-F.insY)/30+4:0;if(F.insY>2600)F.insY=2600;}else F.insY+=F.insY<1800?6:0;}
  if(F.msgT>0)F.msgT--;
  BEAT->screen=SCR_SPFLY;BEAT->mode=mi;BEAT->timer=F.tq<0?0:F.tq/60;BEAT->scoreA=F.score<0?0:F.score;BEAT->phase=F.done?2:F.tq>0?1:0;BEAT->x=alt/100;BEAT->y=vel;BEAT->extra=F.st|(F.ei<<4)|(F.thr<<12)|(g<<20);
  lfb=0;sp_fly_draw(m,alt,vel,g,F.done?0:dA*F.warp/ (F.warp?F.warp:1));sp_hud(m,alt,vel,dr,g);
  if(F.done){lf(30,64,180,26,RGB(8,10,20));ltc(68,"ORBIT ACHIEVED",RGB(140,255,160));ltc(78,m->orb,C_WHITE);}
  m3_present();
  if(F.done&&endT>150)break;
  if(!F.auto_&&(pressed&K_START)&&F.tq<0){return -1;}}
 // results
 int gs=F.gn?clampi(1000-F.gerr*100/F.gn,0,1000):1000;F.score+=gs;int mx=1000;for(int i=0;i<m->ne;i++)if(sp_evkey(m->e[i].ev)||m->e[i].ev==EV_ROLLE)mx+=300;
 int sc=F.score<0?0:F.score;int st=1+(sc*100>=mx*60)+(sc*100>=mx*85);int best=sc>SPR.fly[mi];if(best)SPR.fly[mi]=sc;if(st>SPR.flyS[mi])SPR.flyS[mi]=st;SPR.last=mi;spr_write();
 music_victory();
 for(int t=0;;t++){next_frame();BEAT->screen=SCR_SPFLY;BEAT->phase=3;BEAT->scoreA=sc;BEAT->scoreB=st;
  if(t==0){char b[48],*o;chrome(CAT_EXPLORE,0);m3_box(8,8,224,144,1);m3_ctext(14,m->n,C_YEL,2);m3_ctext(32,cat3(m->veh," - ",m->pad),C_WHITE,1);
   int y=44,n=0;for(int i=0;i<m->ne&&n<9;i++){int k=sp_evkey(m->e[i].ev);if(!k&&m->e[i].ev!=EV_ROLLE)continue;o=sp_put(b,"T");o=sp_tm(o,m->e[i].t);o=sp_put(o," ");int j=0;while(m->e[i].s[j]&&j<22)*o++=m->e[i].s[j++];*o=0;m3_text(16,y,b,C_GRAY,1);
    static const char*const GN[4]={"AUTO","LATE","GOOD","PERFECT"};m3_text(180,y,GN[F.gr[i]],F.gr[i]==3?C_GREEN:F.gr[i]?C_YEL:C_RED,1);y+=9;n++;}
   o=sp_put(b,"GUIDANCE ");o=sp_int(o,gs/10);o=sp_put(o,"%   SCORE ");o=sp_int(o,sc);if(best)o=sp_put(o," BEST!");m3_text(16,128,b,best?C_GREEN:C_WHITE,1);
   for(int s=0;s<3;s++)m3_fill(176+s*14,126,10,10,s<st?C_YEL:RGB(50,50,60));m3_ctext(141,"A: CONTINUE",C_GRAY,1);m3_present();}
  if(sp_isauto()&&t>200)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;break;}}
 bgCached=-1;return sc;}
static int sp_list(const char*title,const char*sub,int n,const char*(*nm)(int),const char*(*rt)(int,char*),int sel,int scr){int redraw=1;
 for(;;){BEAT->screen=scr;BEAT->x=sel;
  if(redraw){chrome(CAT_EXPLORE,0);banner(title,sub,C_YEL);m3_box(4,38,232,118,1);int top=sel>8?sel-8:0;
   for(int i=top;i<n&&i<top+9;i++){int y=44+(i-top)*11;char b[24];m3_text(12,y,i==sel?">":" ",C_YEL,1);m3_text(20,y,nm(i),i==sel?C_YEL:C_WHITE,1);const char*r=rt(i,b);m3_text(150,y,r,C_GRAY,1);}
   m3_ctext(146,"A: SELECT   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();if(pressed&K_UP){sel=(sel+n-1)%n;redraw=1;sfx_menu();}if(pressed&K_DOWN){sel=(sel+1)%n;redraw=1;sfx_menu();}
  if(pressed&K_B){pressed=0;return -1;}if(pressed&K_A){pressed=0;sfx_ok();return sel;}}}
static const char*spm_nm(int i){return SPM[i].n;}
static const char*spm_rt(int i,char*b){char*o=sp_put(b,"");for(int s=0;s<3;s++)*o++=s<SPR.flyS[i]?'*':'-';*o++=' ';sp_int(o,SPR.fly[i]);return b;}
static void sp_brief(const char*n,const char*l1,const char*l2,const char*l3,const char*l4,const char*help){chrome(CAT_EXPLORE,0);banner(n,l1,C_YEL);m3_box(4,38,232,118,1);m3_text(12,46,l2,C_WHITE,1);m3_text(12,58,l3,C_GRAY,1);m3_text(12,70,l4,C_GRAY,1);
 int y=86;const char*p=help;char b[40];while(*p&&y<136){int k=0;while(p[k]&&p[k]!='|'&&k<38){b[k]=p[k];k++;}b[k]=0;m3_text(12,y,b,RGB(170,200,255),1);y+=10;p+=k;if(*p=='|')p++;}m3_ctext(146,"A: GO   B: BACK",C_GRAY,1);m3_present();}
static int sp_wait_ab(int scr){for(int t=0;;t++){next_frame();BEAT->screen=scr;if(sp_isauto()&&t>30)return 1;if(pressed&K_A){pressed=0;return 1;}if(pressed&K_B){pressed=0;return 0;}}}
static void sp_profiles(void){int sel=SPR.last<SPM_N?SPR.last:0;
 for(;;){int i=sp_list("MISSION PROFILES","REAL LAUNCHES - REAL TIMELINES",SPM_N,spm_nm,spm_rt,sel,SCR_SPSEL+0);if(i<0)return;sel=i;const SPm*m=&SPM[i];
  char l2[48],*o=sp_put(l2,m->veh);(void)o;char l3[48];o=sp_put(l3,m->pad);o=sp_put(o," - ");sp_put(o,m->date);char l4[48];o=sp_put(l4,"TARGET: ");sp_put(o,m->orb);
  sp_brief(m->n,"FLY THE REAL FLIGHT PROFILE",l2,l3,l4,"A/SELECT/L/R ON THE CUES. UP/DOWN: HOLD|THE PINK GUIDANCE MARK. B: TIME X1/4/16|TELEMETRY ~ REAL VALUES (APPROX).");BEAT->screen=SCR_SPSEL+0;BEAT->mode=i;
  if(sp_wait_ab(SCR_SPSEL))sp_fly(i);bgm_t(TR_SPACE);}}
// ---------------------------------------------------------------- hub
static void sp_satellites(void);static void sp_iss(void);static void sp_eva(void);
static const char*const SPHN[4]={"MISSION PROFILES","SATELLITE DEPLOY","ISS DOCKING","SPACEWALK (EVA)"};
static const char*sph_nm(int i){return SPHN[i];}
static const char*sph_rt(int i,char*b){int n=0,s=0;if(i==0){for(int k=0;k<SPM_N;k++){n+=SPR.fly[k]>0;s+=SPR.flyS[k];}}else if(i==1){for(int k=0;k<8;k++){n+=SPR.sat[k]>0;s+=SPR.satS[k];}}else if(i==2){for(int k=0;k<8;k++){n+=SPR.iss[k]>0;s+=SPR.issS[k];}}else{for(int k=0;k<6;k++){n+=SPR.eva[k]>0;s+=SPR.evaS[k];}}
 char*o=sp_int(b,n);o=sp_put(o," DONE ");o=sp_int(o,s);sp_put(o,"*");return b;}
static void space_program(void){spr_read();bgm_t(TR_SPACE);int sel=0;
 for(;;){int i=sp_list("SPACE PROGRAM","REAL MISSIONS - NO SHORTCUTS",4,sph_nm,sph_rt,sel,SCR_SPSEL);if(i<0){bgCached=-1;return;}sel=i;
  if(i==0)sp_profiles();else if(i==1)sp_satellites();else if(i==2)sp_iss();else sp_eva();bgm_t(TR_SPACE);}}
static void rec_space(const char**L){spr_read();static char b[4][40];for(int i=0;i<4;i++){char t[24];char*o=sp_put(b[i],SPHN[i]);o=sp_put(o,": ");sp_put(o,sph_rt(i,t));L[i]=b[i];}L[4]="REAL PROFILES, SATS, ISS, EVA";}
static void sp_satellites(void){}static void sp_iss(void){}static void sp_eva(void){}
