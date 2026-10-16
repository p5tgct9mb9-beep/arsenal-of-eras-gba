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
  case PL_H2:c=RGB(200,220,255);tr=altk>30?225:200;if(altk<30&&(j%6)<2&&j<len*3/4){int dw=(j%6)?ww/2:ww/3;lf(x-dw/2,y+j,dw+1,1,RGB(255,236,200));continue;}break;
  case PL_SRB:c=j<len/2?RGB(255,252,236):sp_mix(RGB(255,240,200),RGB(255,180,90),(k-128)*2);break;
  case PL_CH4:c=sp_mix(RGB(150,170,255),RGB(190,120,255),k);tr=altk>30?200:150;if(altk<30&&(j%7)<2&&j<len*2/3){lf(x-ww/4,y+j,ww/2+1,1,RGB(240,230,255));continue;}break;
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
typedef struct{u32 lv;int loop,lsec,fc,fv,fps,tq,warp,ei,st,fl,roll,pitch,thr,thrT,score,guid,gerr,gn,maxq,auto_,iT,ign,lastEv,msgT,inset,insY,insV,landT,qwarn,done;const char*msg;u8 gr[20];s8 dt[20];}SPF;
static SPF F EWRAM;static SPpart SPP[56] EWRAM;static SPdeb SPD[8] EWRAM;
static void spf_msg(const char*s){F.msg=s;F.msgT=150;}
// interpolate profile at time tq (1/60 s): alt m, vel km/h, dr km, g x10
static void sp_prof(const SPm*m,int tq,int*alt,int*vel,int*dr,int*g){const SPp*p=m->p;int n=m->np;int t=tq;if(t<0)t=0;
 int i=0;while(i<n-2&&p[i+1].t*60<=t)i++;const SPp*a=&p[i],*b=&p[i+1];int span=(b->t-a->t)*60;int f=span>0?((t-a->t*60)<<10)/span:1024;if(f>1024)f=1024;if(f<0)f=0;
 *alt=(a->alt*100)+((b->alt-a->alt)*100*f>>10);*vel=a->vel+((b->vel-a->vel)*f>>10);*dr=a->dr+((b->dr-a->dr)*f>>10);*g=a->g+((b->g-a->g)*f>>10);
 if(tq<=0){*alt=0;*vel=0;*dr=0;*g=10;}}
// guidance pitch reference (deg from vertical) derived from the profile: atan2(horizontal, vertical speed)
static int sp_pitchref(const SPm*m,int tq){if(tq<10*60)return 0;int a0,v0,d0,g0,a1,v1,d1,g1;sp_prof(m,tq-60,&a0,&v0,&d0,&g0);sp_prof(m,tq+60,&a1,&v1,&d1,&g1);
 int vz=(a1-a0)/2;int v=(v0+v1)/2*10/36;if(v<1)v=1;int vh2=v*v-vz*vz;int vh=vh2>0?isq(vh2):0;int ang=atan2i(vh,-(vz<1?1:vz));int deg=ang*360/256;if(deg>90)deg=90;if(deg<0)deg=0;return deg;}
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
 ltx(46,12,m->pad,RGB(255,255,200));}
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
 o=sp_put(b,"DR ");o=sp_int(o,dr);o=sp_put(o," KM");ltx(178,46,b,C_WHITE);
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
   if(ak<25&&(F.loop&1)&&(ty[k]==PL_SRB||ty[k]==PL_RP1)&&F.tq>0)sp_puff((120+xo[k]+rr(5)-2)<<4,(by+L)<<4,rr(9)-4,8,140,ty[k]==PL_SRB?5:3,0);}
  if(F.tq>-60&&alt<30&&(F.loop&1)){for(int s=-1;s<=1;s+=2)sp_puff((120+s*20)<<4,(gy-4)<<4,s*(16+rr(16)),-rr(6),200,4,1);}}
 sp_inset(m);}
static int sp_fly(int mi){const SPm*m=&SPM[mi];memset(&F,0,sizeof F);F.lv=vblN;memset(SPP,0,sizeof SPP);memset(SPD,0,sizeof SPD);F.tq=m->cd*60;F.warp=1;F.roll=-1;F.thrT=0;F.auto_=sp_isauto();bgm_t(TR_ASCENT);
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
  if(F.roll>=0&&(ky&K_RIGHT))F.roll++;if(F.roll>=0&&(ky&K_LEFT)&&F.roll>0)F.roll--;
  if(F.tq>8*60&&!F.done){if((ky&K_UP)&&(F.loop&1))F.pitch++;if((ky&K_DOWN)&&(F.loop&1))F.pitch--;if(F.warp>1)F.pitch=sp_pitchref(m,F.tq);
   int sec=F.tq/60;int nsec=sec!=F.lsec;F.lsec=sec;if(F.warp==1&&nsec&&(sec%2)==0&&!F.auto_)F.pitch+=rr(3)-1;F.pitch=clampi(F.pitch,0,95);if(nsec&&F.warp==1){F.gerr+=fabsi(F.pitch-sp_pitchref(m,F.tq));F.gn++;}}
  if(F.ign&&F.thrT==0)F.thrT=100;if(F.done)F.thrT=0;F.loop++;if(F.thr<F.thrT)F.thr++;if(F.thr>F.thrT)F.thr--;if(F.thr<F.thrT-30)F.thr+=3;
  // advance time
  {int el=(int)(vblN-F.lv);F.lv=vblN;if(el<1)el=1;if(el>4)el=4;F.fc++;F.fv+=el;if(F.fv>=60){F.fps=F.fc;F.fc=0;F.fv=0;}if(!F.done)F.tq+=F.warp*el;else endT+=el;}
  sp_prof(m,F.tq,&alt,&vel,&dr,&g);int dA=(alt-prevAlt)*4/5;if(dA<0)dA=0;if(dA>200)dA=200;prevAlt=alt;
  if(F.inset){if(F.landT){F.insY+=F.insY<2600?(2600-F.insY)/30+4:0;if(F.insY>2600)F.insY=2600;}else F.insY+=F.insY<1800?6:0;}
  if(F.msgT>0)F.msgT--;
  BEAT->screen=SCR_SPFLY;BEAT->mode=mi;BEAT->timer=F.tq<0?0:F.tq/60;BEAT->kills=F.fps;BEAT->scoreA=F.score<0?0:F.score;BEAT->phase=F.done?2:F.tq>0?1:0;BEAT->x=alt/100;BEAT->y=vel;BEAT->extra=F.st|(F.ei<<4)|(F.thr<<12)|(g<<20);
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
   int y=44,n=0;for(int i=0;i<m->ne&&n<9;i++){int k=sp_evkey(m->e[i].ev);if(!k&&m->e[i].ev!=EV_ROLLE)continue;o=sp_put(b,"T");o=sp_tm(o,m->e[i].t);o=sp_put(o," ");int j=0;while(m->e[i].s[j]&&j<16)*o++=m->e[i].s[j++];*o=0;m3_text(14,y,b,C_GRAY,1);
    static const char*const GN[4]={"AUTO","LATE","GOOD","PERFECT"};m3_text(178,y,GN[F.gr[i]],F.gr[i]==3?C_GREEN:F.gr[i]?C_YEL:C_RED,1);y+=9;n++;}
   o=sp_put(b,"GUIDANCE ");o=sp_int(o,gs/10);o=sp_put(o,"%   SCORE ");o=sp_int(o,sc);if(best)o=sp_put(o," BEST!");m3_text(16,128,b,best?C_GREEN:C_WHITE,1);
   for(int s=0;s<3;s++)m3_fill(186+s*14,114,10,10,s<st?C_YEL:RGB(50,50,60));m3_ctext(141,"A: CONTINUE",C_GRAY,1);m3_present();}
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
static int sp_wait_ab(int scr){for(int t=0;;t++){next_frame();BEAT->screen=scr;if(sp_isauto()&&t>30)return 1;if(pressed&K_A){pressed=0;return 1;}if(pressed&K_SEL){pressed=0;return 2;}if(pressed&K_B){pressed=0;return 0;}}}
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
// ---------------------------------------------------------------- orbital mechanics (2D Kepler, km / s)
#define MU 398600.4f
#define RE 6371.0f
typedef struct{float a,e,w,M0,t0,n;float t;}Orb;
static void orb_from(Orb*o,float rx,float ry,float vx,float vy,float t){float r=fsqr(rx*rx+ry*ry),v2=vx*vx+vy*vy;float en=v2/2-MU/r;o->a=-MU/(2*en);
 float rv=rx*vx+ry*vy;float ex=((v2-MU/r)*rx-rv*vx)/MU,ey=((v2-MU/r)*ry-rv*vy)/MU;o->e=fsqr(ex*ex+ey*ey);if(o->e>0.995f)o->e=0.995f;if(o->a<0)o->a=4e6f;
 o->w=o->e>1e-5f?fatan2(ey,ex):0;float nu=fatan2(ry,rx)-o->w;float h=rx*vy-ry*vx;if(h<0)h=-h;
 float E=2*fatan2(fsqr(1-o->e)*fsin(nu/2),fsqr(1+o->e)*fcos(nu/2));o->M0=E-o->e*fsin(E);o->t0=t;o->t=t;o->n=fsqr(MU/(o->a*o->a*o->a));}
static float orb_E(const Orb*o,float t){float M=fwrap(o->M0+o->n*(t-o->t0));float E=o->e>0.8f?(M<0?-FPI*0.9f:FPI*0.9f)*0+M+o->e*fsin(M):M;for(int i=0;i<8;i++){float f=E-o->e*fsin(E)-M,d=1-o->e*fcos(E);E-=f/d;}return E;}
static void orb_state(const Orb*o,float t,float*rx,float*ry,float*vx,float*vy,float*nuo){float E=orb_E(o,t);float e=o->e;float nu=2*fatan2(fsqr(1+e)*fsin(E/2),fsqr(1-e)*fcos(E/2));float r=o->a*(1-e*fcos(E));
 float th=nu+o->w;float c=fcos(th),sn=fsin(th);*rx=r*c;*ry=r*sn;float p=o->a*(1-e*e);float h=fsqr(MU*p);float vr=MU/h*e*fsin(nu),vt=MU/h*(1+e*fcos(nu));*vx=vr*c-vt*sn;*vy=vr*sn+vt*c;if(nuo)*nuo=fwrap(nu);}
static int orb_pe(const Orb*o){return (int)(o->a*(1-o->e)-RE);}
static int orb_ap(const Orb*o){return (int)(o->a*(1+o->e)-RE);}
// ---------------------------------------------------------------- satellite deploy
typedef struct{const char*n,*sub,*veh;u8 prof;s16 pe0;s32 ap0;s32 tpe,tap,tol;u16 dv;u8 kind;const char*fact;}SPsat;
enum{PK_STARLINK,PK_GPS,PK_HST,PK_JWST,PK_TDRS,PK_COMM,PK_WX,PK_LAND};
static const SPsat SPS[8]={
 {"STARLINK","60-SAT STACK - LEO","FALCON 9 / SLC-40",4,195,212,280,290,12,250,PK_STARLINK,"FLAT-PACKED, RELEASED ALL AT ONCE"},
 {"GPS III SV05","NAVIGATION - MEO 55 DEG","FALCON 9 / SLC-40",3,185,185,20180,20180,300,4300,PK_GPS,"31 SATS GIVE YOUR PHONE ITS FIX"},
 {"HUBBLE (STS-31)","SPACE TELESCOPE - 618 KM","DISCOVERY / LC-39B",2,260,620,612,622,10,250,PK_HST,"CANADARM LIFTS IT OUT OF THE BAY"},
 {"JWST","L2 TRANSFER - 1.5M KM","ARIANE 5 / KOUROU",9,180,1400,180,1500000,250000,3600,PK_JWST,"ARIANE WAS SO PRECISE IT SAVED FUEL"},
 {"TDRS-C (STS-26)","RELAY SAT - GEO VIA IUS","DISCOVERY / LC-39B",2,300,300,35786,35786,250,4600,PK_TDRS,"IUS SOLID MOTOR: 2 BURNS TO GEO"},
 {"SES-8 COMMS","GEO COMMS - GTO DROP-OFF","FALCON 9 / SLC-40",4,185,185,250,35786,400,2800,PK_COMM,"SAT USES ITS OWN MOTOR AFTERWARDS"},
 {"HIMAWARI-9 WEATHER","GEO WEATHER - 140.7 E","H-IIA / TANEGASHIMA",11,250,35786,35786,35786,250,1800,PK_WX,"CIRCULARISE AT APOGEE (APOGEE BURN)"},
 {"LANDSAT 9","SUN-SYNC 705 KM 98.2 DEG","ATLAS V / VANDENBERG",12,185,705,700,710,8,400,PK_LAND,"SSO: SAME SUN ANGLE EVERY PASS"}};
static const char*sps_nm(int i){return SPS[i].n;}
static const char*sps_rt(int i,char*b){char*o=b;for(int s=0;s<3;s++)*o++=s<SPR.satS[i]?'*':'-';*o++=' ';sp_int(o,SPR.sat[i]);return b;}
typedef struct{Orb o;float dv;int warp,burn,dep,acc,auto_,t;}SPO;
static SPO SO EWRAM;
static void sp_orbit_draw(const SPsat*S,float rx,float ry){char b[48],*o;float apr=SO.o.a*(1+SO.o.e);float tr=(float)S->tap+RE;float mx=apr>tr?apr:tr;if(mx<RE*1.6f)mx=RE*1.6f;float sc=66.0f/mx;int cx=110,cy=84;
 lf(0,0,240,160,RGB(2,3,10));stars(0,30);int er=(int)(RE*sc);if(er<1)er=1;sp_disc(cx,cy,er,RGB(40,90,170));if(er>4){sp_disc(cx-er/4,cy-er/4,er/3,RGB(60,140,80));sp_circ(cx,cy,er,RGB(140,190,255));}
 // target orbit (dashed, same orientation)
 {float ta=((float)S->tpe+(float)S->tap)/2+RE,te=((float)S->tap-(float)S->tpe)/(2*ta);float p=ta*(1-te*te);for(int k=0;k<96;k+=2){float nu=k*2*FPI/96;float r=p/(1+te*fcos(nu));float th=nu+SO.o.w;sp_px(cx+(int)(r*fcos(th)*sc),cy-(int)(r*fsin(th)*sc),RGB(80,220,120));}}
 {float p=SO.o.a*(1-SO.o.e*SO.o.e);int px=0,py=0;for(int k=0;k<=96;k++){float nu=k*2*FPI/96;float r=p/(1+SO.o.e*fcos(nu));float th=nu+SO.o.w;int x=cx+(int)(r*fcos(th)*sc),y=cy-(int)(r*fsin(th)*sc);if(k)sp_line(px,py,x,y,RGB(230,230,240));px=x;py=y;}
  float ra=SO.o.a*(1+SO.o.e),rp=SO.o.a*(1-SO.o.e);int ax=cx-(int)(ra*fcos(SO.o.w)*sc),ay=cy+(int)(ra*fsin(SO.o.w)*sc);ltx(clampi(ax-3,0,200),clampi(ay-4,10,150),"AP",RGB(255,200,120));
  int qx=cx+(int)(rp*fcos(SO.o.w)*sc),qy=cy-(int)(rp*fsin(SO.o.w)*sc);ltx(clampi(qx-3,0,200),clampi(qy+2,10,150),"PE",RGB(150,200,255));}
 int sx=cx+(int)(rx*sc),sy=cy-(int)(ry*sc);lf(sx-1,sy-1,3,3,SO.burn?RGB(255,160,60):RGB(255,255,120));if(SO.burn)sp_line(sx,sy,sx-(SO.burn>0?3:-3),sy,RGB(255,120,40));
 lf(0,0,240,10,RGB(8,10,20));ltx(2,1,S->n,RGB(255,220,120));o=sp_put(b,"X");sp_int(o,SO.warp==0?1:SO.warp==1?4:16);ltx(222,1,b,RGB(150,150,170));
 lf(172,12,66,74,RGB(8,10,20));int pe=orb_pe(&SO.o),ap=orb_ap(&SO.o);
 ltx(174,14,"PE KM",RGB(150,200,255));o=sp_intc(b,pe);ltx(174,22,b,fabsi(pe-S->tpe)<=S->tol?C_GREEN:C_WHITE);
 ltx(174,31,"AP KM",RGB(255,200,120));o=sp_intc(b,ap);ltx(174,39,b,fabsi(ap-S->tap)<=S->tol?C_GREEN:C_WHITE);
 ltx(174,48,"TARGET",RGB(80,220,120));o=sp_intc(b,(int)S->tpe);o=sp_put(o,"X");ltx(174,56,b,RGB(80,220,120));o=sp_intc(b,(int)S->tap);ltx(174,64,b,RGB(80,220,120));
 o=sp_put(b,"DV ");o=sp_int(o,(int)(SO.dv*1000));sp_put(o," M/S");ltx(174,74,b,SO.dv<0.1f?RGB(255,120,80):C_WHITE);
 lf(0,150,240,10,RGB(8,10,20));int ok=fabsi(pe-S->tpe)<=S->tol&&fabsi(ap-S->tap)<=S->tol;ltc(151,ok?"IN TARGET ORBIT - SELECT: DEPLOY":"A PROGRADE  L RETRO  R FINE  B WARP",ok?RGB(140,255,160):RGB(170,200,255));
 o=sp_put(b,"T");sp_tm(o,(int)SO.o.t);ltx(2,140,b,RGB(140,255,160));}
static int sp_orbit(int si){const SPsat*S=&SPS[si];memset(&SO,0,sizeof SO);SO.auto_=sp_isauto();float rp=RE+S->pe0,ra=RE+S->ap0;float a=(rp+ra)/2;float vp=fsqr(MU*(2/rp-1/a));orb_from(&SO.o,rp,0,0,vp,0);SO.dv=S->dv/1000.0f;bgm_t(TR_SPACE);
 for(SO.t=0;;SO.t++){next_frame();float rx,ry,vx,vy,nu;orb_state(&SO.o,SO.o.t,&rx,&ry,&vx,&vy,&nu);int pe=orb_pe(&SO.o),ap=orb_ap(&SO.o);int ok=fabsi(pe-S->tpe)<=S->tol&&fabsi(ap-S->tap)<=S->tol;
  u16 ky=keys,pr=pressed;
  float adv=0;if(SO.auto_){ky=0;pr=0;float an=nu<0?-nu:nu;int nearP=an<0.06f,nearA=an>FPI-0.06f;float r=fsqr(rx*rx+ry*ry),v=fsqr(vx*vx+vy*vy);
   if(ok)pr|=K_SEL;else if(nearP||nearA){float other=nearP?(float)S->tap+RE:(float)S->tpe+RE;int need=nearP?fabsi(ap-S->tap)>S->tol/4:fabsi(pe-S->tpe)>S->tol/4;
    if(nearA&&!need&&fabsi(ap-S->tap)>S->tol/4){other=(float)S->tap+RE;need=0;}
    if(need){float an2=(r+other)/2;float vr=fsqr(MU*(2/r-1/an2));adv=vr-v;if(ffabs(adv)<0.0005f)adv=0;}}
   SO.warp=(nearP||nearA)?0:2;}
  if(pr&K_B){SO.warp=(SO.warp+1)%3;sfx_menu();}
  SO.burn=0;if(((ky&(K_A|K_L))||adv!=0)&&SO.dv>0){float dvs=(ky&K_R)?0.002f:0.02f;float sg=(ky&K_A)?1:-1;if(adv!=0){sg=adv>0?1:-1;dvs=ffabs(adv);if(dvs>0.02f)dvs=0.02f;}if(dvs>SO.dv)dvs=SO.dv;float v=fsqr(vx*vx+vy*vy);vx+=sg*dvs*vx/v;vy+=sg*dvs*vy/v;SO.dv-=dvs;SO.o.t+=2;orb_from(&SO.o,rx,ry,vx,vy,SO.o.t);SO.burn=(int)sg;if((SO.t&7)==0)sfx_whoosh();}
  else{static const s16 WR[3]={10,60,300};float step=WR[SO.warp];float per=2*FPI/SO.o.n;if(step>per/60)step=per/60;SO.o.t+=step;}
  BEAT->screen=SCR_SPORB;BEAT->mode=si;BEAT->x=pe;BEAT->y=ap;BEAT->scoreA=(int)(SO.dv*1000);BEAT->phase=ok;BEAT->timer=(int)SO.o.t;
  lfb=0;sp_orbit_draw(S,rx,ry);m3_present();
  if((pr&K_SEL)&&ok){int acc=1000-(fabsi(pe-S->tpe)+fabsi(ap-S->tap))*500/S->tol;SO.acc=clampi(acc,0,1000);return 1;}
  if(pr&K_START)return 0;if(SO.t>36000)return 0;}}
// payload sprites
static void sp_payload(int k,int x,int y,int ph){u16 G=RGB(210,170,70),Pn=RGB(40,60,120),W=C_WH;
 switch(k){
 case PK_STARLINK:for(int i=0;i<10;i++){int dx=ph?(i-5)*ph/6:0,dy=ph?(i-5)*ph/12:0;lf(x-8+dx,y-10+i*2+dy,16,1,RGB(150,150,160));lf(x-8+dx,y-10+i*2+dy,16,1,i&1?RGB(110,110,120):RGB(160,160,170));}break;
 case PK_GPS:lf(x-4,y-5,8,10,G);lf(x-4,y-5,8,1,lit(G));if(ph){lf(x-4-ph,y-2,ph,4,Pn);lf(x+4,y-2,ph,4,Pn);}lf(x-1,y-8,3,3,W);break;
 case PK_HST:lf(x-12,y-3,22,7,RGB(200,200,206));lf(x-12,y-3,22,1,C_WH);lf(x+10,y-2,3,5,RGB(80,80,90));if(ph){lf(x-6,y-12,4,9,Pn);lf(x-6,y+4,4,9,Pn);}break;
 case PK_JWST:if(ph<6){lf(x-4,y-8,8,16,G);lf(x-6,y+6,12,3,RGB(180,120,170));}else{for(int r=0;r<5;r++)lf(x-5+(r&1)*2,y-9+r*3,8,3,G);int sw=clampi(ph-6,0,20);lf(x-sw,y+8,sw*2,3,RGB(190,130,180));lf(x-sw/2,y+11,sw,2,RGB(160,110,160));}if(ph)lf(x+5,y+2,6,3,Pn);break;
 case PK_TDRS:lf(x-4,y-4,8,8,G);if(ph){sp_circ(x-8,y,5,C_WH);sp_circ(x+8,y,5,C_WH);lf(x-2,y-6-ph/2,4,ph/2,Pn);}break;
 default:lf(x-4,y-5,8,10,k==PK_LAND?RGB(200,200,206):G);if(ph){lf(x+4,y-2,ph+2,4,Pn);if(k!=PK_LAND)lf(x-4-ph-2,y-2,ph+2,4,Pn);}if(k==PK_WX)lf(x-2,y-8,4,3,C_WH);}}
static int sp_canadarm(void){ // STS-31: grapple Hubble in the bay, lift, deploy arrays, release
 int ex=40,ey=60,st=0,t=0,score=1000,au=sp_isauto();int hx=130,hy=110;int gx=hx-4,gy=hy-8,rx=150,ry=60,dep=0;
 for(;;t++){next_frame();u16 ky=keys,pr=pressed;int tx=st==0?gx:rx,ty=st==0?gy:ry;if(au){ky=0;pr=0;if(ex<tx)ky|=K_RIGHT;if(ex>tx)ky|=K_LEFT;if(ey<ty)ky|=K_DOWN;if(ey>ty)ky|=K_UP;if(ex==tx&&ey==ty&&(t&15)==0&&(st==0||dep>=60))pr|=K_A;}
  if((t&1)==0){if(ky&K_LEFT)ex--;if(ky&K_RIGHT)ex++;if(ky&K_UP)ey--;if(ky&K_DOWN)ey++;}ex=clampi(ex,10,230);ey=clampi(ey,20,140);
  if(st==1){hx=ex+4;hy=ey+8;if(idist(ex-rx,ey-ry)<3&&dep<60)dep++;}
  if(pr&K_A){if(st==0&&idist(ex-gx,ey-gy)<3){st=1;sfx_ok();}else if(st==1&&dep>=60){st=2;sfx_ok();}else score-=50;}
  if(st==2){hy--;if(t%2)hx++;}
  BEAT->screen=SCR_SPDEP;BEAT->phase=st;BEAT->x=ex;BEAT->y=ey;
  lfb=0;lf(0,0,240,160,RGB(2,3,10));stars(0,30);earth_limb(30);
  lf(20,118,200,20,RGB(220,220,220));lf(20,118,200,2,RGB(150,150,150));lf(20,124,200,14,RGB(190,190,194));for(int i=0;i<10;i++)lf(26+i*20,120,1,4,RGB(160,160,160));lf(14,100,8,38,C_WH);ltx(24,140,"DISCOVERY PAYLOAD BAY",RGB(150,200,255));
  {int sx=30,sy=116;int mx=(sx+ex)/2-10,my=(sy+ey)/2-14;sp_line(sx,sy,mx,my,RGB(240,240,240));sp_line(sx+1,sy,mx+1,my,RGB(200,200,200));sp_line(mx,my,ex,ey,RGB(240,240,240));sp_line(mx,my+1,ex,ey+1,RGB(200,200,200));lf(mx-1,my-1,3,3,RGB(60,60,60));lf(ex-1,ey-1,3,3,RGB(60,60,60));}
  sp_payload(PK_HST,hx,hy,st>=1?dep/6:0);if(st==0)lf(gx-1,gy-1,3,3,RGB(255,80,80));if(st==1&&dep<60){lf(rx-2,ry,5,1,C_GREEN);lf(rx,ry-2,1,5,C_GREEN);}
  lf(0,0,240,10,RGB(8,10,20));ltx(2,1,"CANADARM - HUBBLE RELEASE",RGB(255,220,120));
  ltc(151,st==0?"D-PAD: ARM TO RED GRAPPLE PIN, A: GRAB":st==1?(dep<60?"LIFT TO THE GREEN MARK - ARRAYS UNFURL":"A: RELEASE HUBBLE"):"HUBBLE IS FREE - 25 APR 1990",RGB(170,200,255));m3_present();
  if(st==2&&hy<40)return clampi(score-t/4,200,1000);if(t>5000)return 0;}}
static int sp_deploy(const SPsat*S){int t=0,res=0,au=sp_isauto();int sepT=300,ph=0,sx=120,sy=80;
 for(;;t++){next_frame();u16 pr=pressed;if(au)pr=(t==sepT)?K_A:0;
  if(!res&&(pr&K_A)&&t>=sepT-120){int d=t-sepT;if(d<0)d=-d;res=d<=15?3:d<=45?2:1;sfx_ok();ph=1;}
  if(!res&&t>sepT+120){res=1;ph=1;}
  if(ph&&ph<40&&(t&3)==0)ph++;
  BEAT->screen=SCR_SPDEP;BEAT->phase=res;BEAT->timer=t;
  lfb=0;lf(0,0,240,160,RGB(2,3,10));stars(0,40);earth_limb(40);
  int stx=sx-(ph?ph:0),sty=sy+12;lf(stx-5,sty,10,18,C_WH);lf(stx-3,sty+18,6,3,RGB(80,80,86));
  sp_payload(S->kind,sx+(ph?ph/2:0),sy-(ph?ph/3:0),ph>6?ph-6:0);
  lf(0,0,240,10,RGB(8,10,20));ltx(2,1,S->n,RGB(255,220,120));
  char b[40],*o;if(!res){int c=(sepT-t)/60;o=sp_put(b,"SEPARATION IN ");sp_int(o,c<0?0:c);ltc(130,b,RGB(255,240,160));ltc(151,"A: SEPARATE AT ZERO",RGB(170,200,255));}
  else{static const char*const GN[4]={"","LATE SEP","GOOD SEP","PERFECT SEP"};ltc(130,GN[res],res==3?C_GREEN:C_YEL);ltc(140,S->fact,C_WHITE);}
  m3_present();if(res&&t>sepT+260)return res;}}
static void sp_satellites(void){int sel=SPR.lastSat<8?SPR.lastSat:0;
 for(;;){int i=sp_list("SATELLITE DEPLOY","REACH THE ORBIT, THEN DEPLOY",8,sps_nm,sps_rt,sel,SCR_SPSEL+0);if(i<0)return;sel=i;const SPsat*S=&SPS[i];
  char l3[48],*o=sp_put(l3,"TARGET ");o=sp_intc(o,(int)S->tpe);o=sp_put(o," X ");o=sp_intc(o,(int)S->tap);sp_put(o," KM");char l4[48];o=sp_put(l4,"START ");o=sp_intc(o,S->pe0);o=sp_put(o," X ");o=sp_intc(o,(int)S->ap0);o=sp_put(o," KM  DV ");sp_int(o,S->dv);
  sp_brief(S->n,S->sub,S->veh,l3,l4,"BURN PROGRADE (A) AT PERIGEE TO RAISE|APOGEE; AT APOGEE TO RAISE PERIGEE.|L = RETRO.  SELECT: FLY THE ASCENT|FIRST. ACCURACY + DV LEFT = SCORE.");
  BEAT->mode=i;int w=sp_wait_ab(SCR_SPSEL);if(!w)continue;if(w==2)sp_fly(S->prof);
  if(sp_orbit(i)){int sep=S->kind==PK_HST?3:0;int arm=0;if(S->kind==PK_HST)arm=sp_canadarm();else sep=sp_deploy(S);
   int sc=SO.acc+(int)(SO.dv*1000*500/S->dv)/1+sep*100+arm/2;int st=1+(SO.acc>=600)+(SO.acc>=900);int best=sc>SPR.sat[i];if(best)SPR.sat[i]=sc;if(st>SPR.satS[i])SPR.satS[i]=st;SPR.lastSat=i;spr_write();music_victory();
   for(int t=0;;t++){next_frame();BEAT->screen=SCR_SPORB;BEAT->phase=3;BEAT->scoreA=sc;BEAT->scoreB=st;if(t==0){char b[48];chrome(CAT_EXPLORE,0);m3_box(16,24,208,112,1);m3_ctext(32,"PAYLOAD DEPLOYED",C_YEL,2);m3_ctext(52,S->n,C_WHITE,1);
     o=sp_put(b,"ORBIT ");o=sp_intc(o,orb_pe(&SO.o));o=sp_put(o," X ");o=sp_intc(o,orb_ap(&SO.o));sp_put(o," KM");m3_ctext(66,b,C_WHITE,1);
     o=sp_put(b,"ACCURACY ");o=sp_int(o,SO.acc/10);o=sp_put(o,"%  DV LEFT ");o=sp_int(o,(int)(SO.dv*1000));m3_ctext(78,b,C_GRAY,1);
     o=sp_put(b,"SCORE ");o=sp_int(o,sc);if(best)sp_put(o,"  BEST!");m3_ctext(94,b,best?C_GREEN:C_WHITE,1);for(int s=0;s<3;s++)m3_fill(99+s*14,106,10,10,s<st?C_YEL:RGB(50,50,60));m3_ctext(124,"A: CONTINUE",C_GRAY,1);m3_present();}
    if(sp_isauto()&&t>200)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;break;}}}
  bgm_t(TR_SPACE);}}
// ---------------------------------------------------------------- ISS
// front view of the ISS (looking aft along -V-bar at Harmony forward). sc = px per 10 m (x16)
static void iss_draw(int cx,int cy,int sc,int view){ // view 0 front, 1 side (plot icon)
 int L=(sc*110)>>4;if(L<4)L=4;int tw=(sc*3)>>4;if(tw<1)tw=1;u16 T=RGB(170,170,176),A=RGB(160,118,58),AG=RGB(110,80,40),R=C_WH,M=RGB(220,220,214);
 if(view==1){lf(cx-L/2,cy,L,tw,T);for(int k=-1;k<=1;k+=2)for(int j=0;j<2;j++){int ax=cx+k*(L/2-j*L/7)-((sc*2)>>4);int ah=(sc*35)>>4;lf(ax,cy-ah,(sc*4)>>4|1,ah*2+tw,A);}lf(cx-((sc*3)>>4),cy-((sc*8)>>4),(sc*6)>>4|1,(sc*16)>>4|1,M);return;}
 lf(cx-L/2,cy-tw/2,L,tw,T);for(int i=-L/2;i<L/2;i+=4)lf(cx+i,cy-tw/2,1,tw,RGB(120,120,126));                // integrated truss
 for(int k=-1;k<=1;k+=2)for(int j=0;j<2;j++){int ax=cx+k*(L/2-((sc*6)>>4)-j*((sc*14)>>4));int aw=(sc*12)>>4,ah=(sc*35)>>4;if(aw<1)aw=1;           // 8 solar array wings (+iROSA)
  for(int s2=-1;s2<=1;s2+=2){int y0=s2<0?cy-tw/2-ah:cy+tw/2;lf(ax-aw/2,y0,aw,ah,A);for(int q=0;q<ah;q+=3)lf(ax-aw/2,y0+q,aw,1,AG);lf(ax,y0,1,ah,AG);
   if(sc>=8){int iw=aw*2/3,ih=ah/2;lf(ax-iw/2,s2<0?y0:y0+ah-ih,iw,ih,RGB(60,70,110));}}}
 for(int k=-1;k<=1;k+=2){int rx=cx+k*((sc*20)>>4);lf(rx-((sc*2)>>4),cy+tw/2,(sc*4)>>4|1,(sc*16)>>4,R);}                          // radiators
 int mw=(sc*5)>>4|1;lf(cx-mw/2,cy-((sc*12)>>4),mw,(sc*24)>>4|1,M);                                                    // module stack
 if(sc>=6){sp_disc(cx,cy,mw,RGB(200,200,196));sp_circ(cx,cy,mw,RGB(140,140,140));}}
typedef struct{const char*n,*port,*how;u8 app,arm,shut;}SPiv;   // app: 0 V-bar fwd (+x), 1 R-bar zenith (+z), 2 R-bar nadir (-z)
static const SPiv SPI[8]={
 {"CREW DRAGON","HARMONY FWD - IDA-2/PMA-2","AUTONOMOUS, CREW MONITORS",0,0,0},
 {"CREW DRAGON (ZEN)","HARMONY ZENITH - IDA-3/PMA-3","CREW-2 STYLE ZENITH DOCK",1,0,0},
 {"SOYUZ MS","RASSVET (MRM-1) NADIR","KURS RADAR, TORU BACKUP",2,0,0},
 {"STARLINER","HARMONY FWD - IDA-2/PMA-2","CFT: CREW TAKES MANUAL",0,0,0},
 {"CARGO DRAGON","HARMONY ZENITH - IDA-3","CRS-2: DOCKS ITSELF",1,0,0},
 {"CYGNUS","UNITY NADIR (CANADARM2)","HOLD AT 10 M, ARM CAPTURE",2,1,0},
 {"HTV KOUNOTORI","HARMONY NADIR (CANADARM2)","R-BAR APPROACH, ARM CAPTURE",2,1,0},
 {"SPACE SHUTTLE","PMA-2 ON HARMONY FWD","R-BAR PITCH MANEUVER FIRST",0,0,1}};
static const char*spi_nm(int i){return SPI[i].n;}
static const char*spi_rt(int i,char*b){char*o=b;for(int s=0;s<3;s++)*o++=s<SPR.issS[i]?'*':'-';*o++=' ';sp_int(o,SPR.iss[i]);return b;}
typedef struct{float x,z,vx,vz,dv;int t,warp,kos,ae,viol;float hx,hz;}SPR1;
static SPR1 RV EWRAM;
#define CWN 0.00113f
static int sp_rendezvous(int vi){const SPiv*V=&SPI[vi];memset(&RV,0,sizeof RV);RV.x=-9000;RV.z=-1500;RV.vx=1.5f*CWN*1500;RV.vz=0;int au=sp_isauto();
 RV.hx=V->app==0?250:0;RV.hz=V->app==1?250:V->app==2?-250:0;int held=0;
 for(;;RV.t++){next_frame();u16 ky=keys,pr=pressed;float r=fsqr(RV.x*RV.x+RV.z*RV.z),rate=(RV.x*RV.vx+RV.z*RV.vz)/(r>1?r:1);
  float dx=RV.hx-RV.x,dz=RV.hz-RV.z,dh=fsqr(dx*dx+dz*dz);
  if(au){ky=0;pr=0;}
  if(pr&K_B){RV.warp=(RV.warp+1)%3;sfx_menu();}
  static const u8 WS[3]={1,5,20};int ss=WS[RV.warp];int th=0;u16 ky0=ky;for(int i=0;i<ss;i++){ky=ky0;
   if(au){ky=0;r=fsqr(RV.x*RV.x+RV.z*RV.z);dx=RV.hx-RV.x;dz=RV.hz-RV.z;dh=fsqr(dx*dx+dz*dz);float vmax=dh>3000?8:dh>800?3:dh>200?1.0f:0.3f;float k=vmax/(dh>1?dh:1);if(k>0.004f)k=0.004f;float tvx=dx*k*(dh>1?1:0),tvz=dz*k;if(dh<vmax*250){tvx=dx*0.004f;tvz=dz*0.004f;}
   // route around the keep-out sphere: stay outside 260 m until on the approach axis
   if(r<235&&dh>40){float ox=RV.x/r,oz=RV.z/r;tvx+=ox*0.4f;tvz+=oz*0.4f;}
   float ex=tvx-RV.vx,ez=tvz-RV.vz;if(ex>0.03f)ky|=K_RIGHT;if(ex<-0.03f)ky|=K_LEFT;if(ez>0.03f)ky|=K_UP;if(ez<-0.03f)ky|=K_DOWN;if(i==0)RV.warp=dh>1500?2:dh>300?1:0;}
   float a=0.05f;if(ky&K_RIGHT){RV.vx+=a;RV.dv+=a;th=1;}if(ky&K_LEFT){RV.vx-=a;RV.dv+=a;th=1;}if(ky&K_UP){RV.vz+=a;RV.dv+=a;th=1;}if(ky&K_DOWN){RV.vz-=a;RV.dv+=a;th=1;}
   float ax=-2*CWN*RV.vz,az=2*CWN*RV.vx+3*CWN*CWN*RV.z;RV.vx+=ax;RV.vz+=az;RV.x+=RV.vx;RV.z+=RV.vz;}
  r=fsqr(RV.x*RV.x+RV.z*RV.z);float ae=(RV.x*RV.x)/(2000.0f*2000)+(RV.z*RV.z)/(1000.0f*1000);
  if(ae<1&&!RV.ae){RV.ae=1;spf_msg("ENTERED APPROACH ELLIPSOID");}
  if(r<200&&!RV.kos){RV.kos=1;}if(r<190&&dh>40){RV.viol++;if((RV.viol&63)==1)spf_msg("KEEP-OUT SPHERE: NOT ON AXIS!");}
  float sp=fsqr(RV.vx*RV.vx+RV.vz*RV.vz);if(dh<15&&sp<0.12f)held++;else held=0;
  BEAT->screen=SCR_SPRDV;BEAT->mode=vi;BEAT->x=(int)RV.x+20000;BEAT->y=(int)RV.z+20000;BEAT->timer=RV.t;BEAT->scoreA=(int)(RV.dv*10);BEAT->phase=RV.ae+RV.kos*2;
  // draw
  lfb=0;lf(0,0,240,160,RGB(4,6,16));float span=r>4000?12000:r>1200?4000:r>400?1200:500;float sc=200.0f/span;int cx=120,cy=84;
  for(int i=-3;i<=3;i++){lf(cx+(int)(i*span/6*sc),12,1,138,RGB(14,18,34));}lf(0,cy,240,1,RGB(30,40,70));lf(cx,12,1,138,RGB(30,40,70));
  ltx(2,cy-9,"-V",RGB(90,110,150));ltx(226,cy-9,"+V",RGB(90,110,150));ltx(cx+2,13,"+R",RGB(90,110,150));
  for(int k=0;k<64;k+=2){int px=cx+(int)(2000*COS(k*4)/256*sc),py=cy-(int)(1000*SIN(k*4)/256*sc);sp_px(px,py,RGB(80,160,255));}
  {int kr=(int)(200*sc);if(kr>=2)sp_circ(cx,cy,kr,RGB(255,90,90));}
  iss_draw(cx,cy,(int)(sc*16*10)<2?2:(int)(sc*16*10)>14?14:(int)(sc*16*10),1);
  int hx=cx+(int)(RV.hx*sc),hy=cy-(int)(RV.hz*sc);lf(hx-2,hy,5,1,C_GREEN);lf(hx,hy-2,1,5,C_GREEN);
  int px=cx+(int)(RV.x*sc),py=cy-(int)(RV.z*sc);if(px>=0&&px<240&&py>=10&&py<150){lf(px-1,py-1,3,3,th?RGB(255,160,60):RGB(255,255,120));sp_line(px,py,px+(int)(RV.vx*8),py-(int)(RV.vz*8),RGB(255,220,120));}
  // predicted path (ballistic, 40 steps of 30 s)
  {static s16 PX[24],PY[24];if((RV.t&15)==0){float qx=RV.x,qz=RV.z,ux=RV.vx,uz=RV.vz;for(int k=0;k<24;k++){for(int j=0;j<5;j++){float ax=-2*CWN*uz*10,az=(2*CWN*ux+3*CWN*CWN*qz)*10;ux+=ax;uz+=az;qx+=ux*10;qz+=uz*10;}PX[k]=cx+(int)(qx*sc);PY[k]=cy-(int)(qz*sc);}}for(int k=0;k<24;k++)sp_px(PX[k],PY[k],RGB(120,120,160));}
  char b[48],*o;lf(0,0,240,10,RGB(8,10,20));o=sp_put(b,V->n);sp_put(o," RENDEZVOUS");ltx(2,1,b,RGB(255,220,120));o=sp_put(b,"X");sp_int(o,WS[RV.warp]);ltx(222,1,b,RGB(150,150,170));
  lf(0,150,240,10,RGB(8,10,20));o=sp_put(b,"RNG ");o=sp_int(o,(int)r);o=sp_put(o,"M  RATE ");o=sp_dec1(o,(int)(rate*10));o=sp_put(o,"  DV ");o=sp_dec1(o,(int)(RV.dv*10));ltx(2,151,b,C_WHITE);
  if(F.msgT>0){F.msgT--;ltc(136,F.msg,RGB(255,240,160));}else ltc(136,dh<300?"NULL YOUR RATES AT THE GREEN HOLD POINT":"D-PAD: THRUST  B: TIME WARP",RGB(150,170,210));
  m3_present();
  if(held>60)return 1;if(pr&K_START)return 0;if(RV.t>20000)return 0;}}
typedef struct{float x,y,z,vx,vy,vz,ro,pi,ya,vro,vpi,vya;int t,ff,soft,cont;}SPD1;
static SPD1 DK EWRAM;
static int sp_dock(int vi){const SPiv*V=&SPI[vi];memset(&DK,0,sizeof DK);DK.x=V->arm?30:200;DK.y=3.5f;DK.z=-2.0f;DK.ro=4;DK.pi=-3;DK.ya=5;int au=sp_isauto();int capT=0;
 for(;;DK.t++){next_frame();u16 ky=keys,pr=pressed;
  if(au){ky=0;pr=0;DK.ff=DK.x>12?1:0;int ph=DK.t%12;float tx=V->arm?10:0;float vt=DK.x>40?-0.8f:DK.x>12?-0.3f:-0.1f;if(V->arm&&DK.x<tx+0.3f)vt=0;
   float tr=0;if(ph==0){if(DK.vy> -DK.y*0.02f+0.01f)pr|=K_LEFT;else if(DK.vy< -DK.y*0.02f-0.01f)pr|=K_RIGHT;}if(ph==3){if(DK.vz> -DK.z*0.02f+0.01f)pr|=K_DOWN;else if(DK.vz< -DK.z*0.02f-0.01f)pr|=K_UP;}
   if(ph==6){if(-DK.vx< -vt-0.02f)pr|=K_L;else if(-DK.vx> -vt+0.02f)pr|=K_R;}
   if(ph==9){ky|=K_B;float*a[3]={&DK.ro,&DK.pi,&DK.ya};float*v[3]={&DK.vro,&DK.vpi,&DK.vya};int w=(DK.t/12)%3;float dsr=-*a[w]*0.03f;if(*v[w]>dsr+0.02f)pr|=w==0?K_L:w==1?K_DOWN:K_LEFT;else if(*v[w]<dsr-0.02f)pr|=w==0?K_R:w==1?K_UP:K_RIGHT;}(void)tr;
   if(V->arm&&DK.x<tx+0.5f&&fabsi((int)(DK.y*10))<3&&fabsi((int)(DK.z*10))<3)capT++;}
  if(pr&K_START)DK.ff^=1;
  int rot=(ky&K_B)!=0;float tp=0.02f,rp=0.1f;
  if(!rot){if(pr&K_LEFT)DK.vy-=tp;if(pr&K_RIGHT)DK.vy+=tp;if(pr&K_UP)DK.vz+=tp;if(pr&K_DOWN)DK.vz-=tp;if(pr&K_L)DK.vx-=0.05f;if(pr&K_R)DK.vx+=0.05f;}
  else{if(pr&K_LEFT)DK.vya-=rp;if(pr&K_RIGHT)DK.vya+=rp;if(pr&K_UP)DK.vpi+=rp;if(pr&K_DOWN)DK.vpi-=rp;if(pr&K_L)DK.vro-=rp;if(pr&K_R)DK.vro+=rp;}
  if(rot&&!au){}
  int ss=DK.ff?10:1;for(int i=0;i<ss;i++){float dt=1.0f/60*2;DK.x+=DK.vx*dt*-1*-1;DK.y+=DK.vy*dt;DK.z+=DK.vz*dt;DK.ro+=DK.vro*dt;DK.pi+=DK.vpi*dt;DK.ya+=DK.vya*dt;}
  // L moves toward the port (closing), R backs away: vx is closing speed sign-reversed
  float rate=-DK.vx;
  if(DK.x<=0.2f&&!V->arm){int ok=ffabs(DK.y)<0.3f&&ffabs(DK.z)<0.3f&&ffabs(DK.ro)<1&&ffabs(DK.pi)<1&&ffabs(DK.ya)<1&&rate<0.25f;
   if(ok){DK.cont=1;break;}DK.soft++;DK.x=10;DK.vx=0;spf_msg("NO SOFT CAPTURE - BACKED OUT TO 10 M");}
  if(V->arm&&DK.x<10.6f&&capT>60){DK.cont=1;break;}
  if(DK.x>400){return 0;}
  BEAT->screen=SCR_SPDOCK;BEAT->mode=vi;BEAT->x=(int)(DK.x*100);BEAT->y=(int)(rate*1000+1000);BEAT->extra=(int)(ffabs(DK.y)*100)|((int)(ffabs(DK.z)*100)<<10)|(DK.soft<<20);BEAT->timer=DK.t;
  // draw first-person view
  lfb=0;lf(0,0,240,160,RGB(2,3,8));stars(DK.t/50,30);
  float rr_=DK.x<1?1:DK.x;int sc=(int)(16*400/(rr_*10+40));if(sc>40)sc=40;if(sc<2)sc=2;
  int tx=120+(int)((-DK.y/rr_)*400)+(int)(DK.ya*4),ty=80+(int)((DK.z/rr_)*400)-(int)(DK.pi*4);
  iss_draw(tx,ty-(V->app==0?0:sc/2),sc>14?14:sc,0);
  int pr2=(int)(60.0f/(rr_+2))+2;if(pr2>70)pr2=70;sp_disc(tx,ty,pr2,RGB(190,190,186));sp_circ(tx,ty,pr2,RGB(120,120,120));sp_circ(tx,ty,pr2*2/3,RGB(90,90,90));
  {int cl=pr2/2;float ca=DK.ro*FPI/180;int ox=(int)(fcos(ca)*cl),oy=(int)(fsin(ca)*cl);sp_line(tx-ox,ty-oy,tx+ox,ty+oy,RGB(30,30,30));sp_line(tx+oy,ty-ox,tx-oy,ty+ox,RGB(30,30,30));}
  // reticle
  u16 RC=RGB(80,255,120);sp_circ(120,80,20,RC);lf(100,80,12,1,RC);lf(128,80,12,1,RC);lf(120,60,1,12,RC);lf(120,88,1,12,RC);
  char b[40],*o;lf(0,0,240,10,RGB(8,10,20));o=sp_put(b,V->n);o=sp_put(o," > ");sp_put(o,V->port);ltx(2,1,b,RGB(255,220,120));
  lf(0,12,56,58,RGB(8,10,20));o=sp_put(b,"X ");o=sp_dec1(o,(int)(DK.x*10));ltx(2,14,b,C_WHITE);o=sp_put(b,"Y ");o=sp_dec1(o,(int)(DK.y*10));ltx(2,24,b,ffabs(DK.y)<0.3f?C_GREEN:C_WHITE);
  o=sp_put(b,"Z ");o=sp_dec1(o,(int)(DK.z*10));ltx(2,34,b,ffabs(DK.z)<0.3f?C_GREEN:C_WHITE);o=sp_put(b,"RATE");ltx(2,46,b,RGB(150,150,170));o=sp_dec1(b,(int)(rate*100));o=sp_put(o," CM");ltx(2,56,b,rate>0.25f&&DK.x<10?RGB(255,120,80):C_WHITE);
  lf(184,12,56,58,RGB(8,10,20));const char*AN[3]={"ROLL ","PITCH","YAW  "};float av[3]={DK.ro,DK.pi,DK.ya};for(int k=0;k<3;k++){o=sp_put(b,AN[k]);ltx(186,14+k*18,b,RGB(150,150,170));o=sp_dec1(b,(int)(av[k]*10));ltx(196,22+k*18,b,ffabs(av[k])<1?C_GREEN:C_WHITE);}
  lf(0,150,240,10,RGB(8,10,20));if(F.msgT>0){F.msgT--;ltc(151,F.msg,RGB(255,240,160));}else ltc(151,V->arm?"HOLD AT 10 M CAPTURE BOX FOR THE ARM":"DPAD:Y/Z  L/R:FWD/BACK  B+:ROT  ST:X10",RGB(150,170,210));
  if(DK.ff)ltx(222,1,"X10",RGB(255,160,80));m3_present();if(DK.t>30000)return 0;}
 // contact / capture vignette
 for(int t=0;t<200;t++){next_frame();BEAT->screen=SCR_SPDOCK;BEAT->phase=1;lfb=0;lf(0,0,240,160,RGB(2,3,8));stars(0,30);iss_draw(120,70,14,0);
  if(V->arm){int ax=60+t/4,ay=40+t/8;sp_line(30,40,ax,ay,C_WH);sp_line(ax,ay,120,100-t/10,C_WH);lf(110,100-t/10,20,14,V->n[0]=='C'?RGB(200,190,150):RGB(210,170,70));ltc(140,t<100?"CANADARM2 CAPTURE!":V->port,C_GREEN);}
  else{sp_disc(120,80,40,RGB(190,190,186));ltc(140,t<100?"SOFT CAPTURE... HARD CAPTURE":V->port,C_GREEN);}m3_present();}
 return 1;}
static void sp_rbar_pitch(void){for(int t=0;t<240;t++){next_frame();BEAT->screen=SCR_SPRDV;BEAT->phase=9;lfb=0;lf(0,0,240,160,RGB(2,3,8));stars(0,30);earth_limb(30);iss_draw(60,40,6,1);
  int a=t*256/240;int L=26;int x1=150+(SIN(a)*L>>8),y1=80-(COS(a)*L>>8),x2=150-(SIN(a)*L>>8),y2=80+(COS(a)*L>>8);for(int w=-3;w<=3;w++)sp_line(x1+w,y1,x2+w,y2,w<0?C_BK:C_WH);
  ltc(130,"R-BAR PITCH MANEUVER (180 M)",RGB(255,220,120));ltc(140,"ISS CREW PHOTOGRAPHS THE HEAT SHIELD",C_WHITE);m3_present();}}
static void sp_iss(void){int sel=SPR.lastIss<8?SPR.lastIss:0;
 for(;;){int i=sp_list("ISS DOCKING","RENDEZVOUS + PROXIMITY OPS",8,spi_nm,spi_rt,sel,SCR_SPSEL+0);if(i<0)return;sel=i;const SPiv*V=&SPI[i];
  sp_brief(V->n,"ISS 420 KM - 92.7 MIN ORBIT",V->port,V->how,V->app==0?"APPROACH ALONG +V-BAR":V->app==1?"APPROACH ALONG +R-BAR (ABOVE)":"APPROACH ALONG -R-BAR (BELOW)",
   "1 RENDEZVOUS: CW ORBIT MECHANICS - THRUST|  PRO/RETRO CHANGES YOUR ALTITUDE TOO.|2 DOCK: <0.25 M/S, ALIGN Y/Z <0.3 M,|  ANGLES <1 DEG. KEEP-OUT SPHERE 200 M.");
  BEAT->mode=i;if(!sp_wait_ab(SCR_SPSEL))continue;
  int ok=sp_rendezvous(i);if(ok&&V->shut)sp_rbar_pitch();if(ok)ok=sp_dock(i);
  if(ok){int sc=3000-(int)(RV.dv*40)-DK.soft*400-RV.viol*2-DK.t/40;sc=clampi(sc,300,3000);int st=1+(sc>=1800)+(sc>=2400&&!DK.soft&&!RV.viol);int best=sc>SPR.iss[i];if(best)SPR.iss[i]=sc;if(st>SPR.issS[i])SPR.issS[i]=st;SPR.lastIss=i;spr_write();music_victory();
   for(int t=0;;t++){next_frame();BEAT->screen=SCR_SPDOCK;BEAT->phase=3;BEAT->scoreA=sc;BEAT->scoreB=st;if(t==0){char b[48],*o;chrome(CAT_EXPLORE,0);m3_box(16,24,208,112,1);m3_ctext(32,V->arm?"BERTHED!":"DOCKED!",C_YEL,2);m3_ctext(52,V->port,C_WHITE,1);
     o=sp_put(b,"DV ");o=sp_dec1(o,(int)(RV.dv*10));o=sp_put(o," M/S  RETRIES ");sp_int(o,DK.soft);m3_ctext(66,b,C_GRAY,1);o=sp_put(b,"KOS VIOLATIONS ");sp_int(o,RV.viol/60);m3_ctext(78,b,C_GRAY,1);
     o=sp_put(b,"SCORE ");o=sp_int(o,sc);if(best)sp_put(o,"  BEST!");m3_ctext(94,b,best?C_GREEN:C_WHITE,1);for(int s=0;s<3;s++)m3_fill(99+s*14,106,10,10,s<st?C_YEL:RGB(50,50,60));m3_ctext(124,"A: CONTINUE",C_GRAY,1);m3_present();}
    if(sp_isauto()&&t>200)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;break;}}}
  bgm_t(TR_SPACE);}}
// ---------------------------------------------------------------- SPACEWALK (EVA)
typedef struct{const char*s;u16 key;u8 hold;}EvStep;
typedef struct{const char*n,*sub,*air;u8 suit,scene,np;s16 px[12],py[12];u8 ws[3],nws,task[3];}EvMis;
enum{ES_ISS,ES_RUS,ES_HST,ES_MMU};
static const EvStep ETK[6][6]={
 {{"OPEN BOX LATCH - A",K_A,0},{"PGT: DRIVE BOLT - HOLD R",K_R,50},{"PULL OLD BATTERY - DOWN",K_DOWN,0},{"INSERT LI-ION ORU - UP",K_UP,0},{"PGT: TORQUE - HOLD R",K_R,50},{"CLOSE LATCH - A",K_A,0}},
 {{"RELEASE IROSA ROLL - A",K_A,0},{"ALIGN ON MAST - LEFT",K_LEFT,0},{"BOLT 1 - HOLD R",K_R,40},{"BOLT 2 - HOLD R",K_R,40},{"CONNECT CABLES - A",K_A,0},{"UNFURL ARRAY - UP",K_UP,0}},
 {{"FIND FLAKES: CLOSE VALVE - A",K_A,0},{"DEMATE QD FLUID LINE - DOWN",K_DOWN,0},{"PGT: PUMP BOLTS - HOLD R",K_R,60},{"SWAP PUMP MODULE - UP",K_UP,0},{"MATE QD - A",K_A,0},{"OPEN VALVE - A",K_A,0}},
 {{"OPEN BAY DOORS - A",K_A,0},{"PULL OLD RSU GYRO - DOWN",K_DOWN,0},{"INSERT NEW RSU - UP",K_UP,0},{"PGT: TORQUE - HOLD R",K_R,40},{"REMOVE WFPC2 - LEFT",K_LEFT,0},{"INSTALL WFC3 - RIGHT",K_RIGHT,0}},
 {{"FOOT RESTRAINT: LOCK - A",K_A,0},{"OPEN CAMERA BAG - A",K_A,0},{"MOUNT CAMERA - UP",K_UP,0},{"TORQUE - HOLD R",K_R,40},{"ROUTE CABLE - RIGHT",K_RIGHT,0},{"VERIFY - A",K_A,0}},
 {{"UNSTOW KIT - A",K_A,0},{"STRELA BOOM: SWING - RIGHT",K_RIGHT,0},{"MOUNT EXPERIMENT - UP",K_UP,0},{"HAND BOLT - HOLD R",K_R,40},{"CONNECT - A",K_A,0},{"PHOTO DOC - A",K_A,0}}};
static const EvMis EVM[7]={
 {"BATTERY SWAP","P4 TRUSS - LI-ION ORU","QUEST AIRLOCK",0,ES_ISS,7,{40,90,90,200,320,420,470},{110,110,90,90,90,90,70},{6},1,{0}},
 {"IROSA INSTALL","2B MAST - ROLL-OUT ARRAY","QUEST AIRLOCK",0,ES_ISS,7,{40,90,90,240,380,520,560},{110,110,90,90,90,90,50},{6},1,{1}},
 {"AMMONIA LEAK","P6 PUMP MODULE - FLAKES!","QUEST AIRLOCK",0,ES_ISS,6,{40,90,90,250,400,450},{110,110,90,90,90,120},{5},1,{2}},
 {"HUBBLE SM4","ATLANTIS 2009 - GYROS + WFC3","SHUTTLE AIRLOCK",0,ES_HST,6,{60,120,200,260,300,330},{120,120,120,100,80,60},{5},1,{3}},
 {"CANADARM2 RIDE","FOOT RESTRAINT ON THE ARM","QUEST AIRLOCK",0,ES_ISS,4,{40,90,90,140},{110,110,90,90},{3},1,{4}},
 {"ORLAN: ZVEZDA","RUSSIAN SEGMENT - STRELA","POISK AIRLOCK",1,ES_RUS,6,{40,80,80,200,300,360},{110,110,90,90,90,70},{5},1,{5}},
 {"MMU 1984 BONUS","STS-41B: FREE FLIGHT 98 M","CHALLENGER BAY",0,ES_MMU,2,{60,60},{110,110},{0},0,{0}}};
static const char*eva_nm(int i){return EVM[i].n;}
static const char*eva_rt(int i,char*b){char*o=b;for(int s=0;s<3;s++)*o++=s<SPR.evaS[i<6?i:5]?'*':'-';*o++=' ';sp_int(o,SPR.eva[i<6?i:5]);return b;}
typedef struct{int s,len,clip,clipS,hand,t,gm,o2,bat,slip,drift,dx,dy,safer,gas,task,step,hold,cam,done,wsi,score,mmu,mx,my,mvx,mvy,arm;s16 cum[12];}EVS;
static EVS EV EWRAM;
static void eva_pos(const EvMis*M,int s,int*x,int*y){for(int i=0;i<M->np-1;i++){int a=EV.cum[i],b=EV.cum[i+1];if(s<=b||i==M->np-2){int L=b-a;int f=L?((s-a)<<8)/L:0;f=clampi(f,0,256);*x=M->px[i]+((M->px[i+1]-M->px[i])*f>>8);*y=M->py[i]+((M->py[i+1]-M->py[i])*f>>8);return;}}*x=M->px[0];*y=M->py[0];}
static void eva_astro(int x,int y,int z,int suit,int fr,int lit){u16 W=C_WH,Rd=suit?RGB(60,90,200):RGB(200,40,40),V=lit?RGB(250,210,90):RGB(200,160,60),P=RGB(200,200,196);
 #define AF(a,b,c,d,e) lf(x+(a)*z,y+(b)*z,(c)*z,(d)*z,e)
 AF(-3,-12,6,5,W);AF(-2,-11,4,3,V);AF(-5,-7,10,7,W);AF(-6,-8,3,8,P);AF(-4,0,3,6,W);AF(1,0,3,6,W);AF(-4,3,3,1,Rd);AF(1,3,3,1,Rd);
 int ar=fr&1;AF(4,-7+ar*2,2,5,W);AF(4,-8+ar*2,2,1,RGB(150,150,150));AF(-7,-6-ar*2,2,4,W);if(suit)AF(-1,-6,2,4,Rd);
 if(lit){AF(-3,-13,1,1,RGB(255,255,200));AF(2,-13,1,1,RGB(255,255,200));}
 #undef AF
}
static void eva_scene(const EvMis*M,int cam,int z,int night){int ox=cam;
 #define WX(v) (((v)-ox)*z)
 u16 sky=night?RGB(1,1,4):RGB(3,4,12);lf(0,0,240,160,sky);stars(cam/8,night?60:30);
 // Earth below with day/night terminator
 {u16 c=night?RGB(4,8,22):RGB(40,90,170);int y0=night?132:126;lf(0,y0,240,160-y0,c);if(!night){for(int i=0;i<6;i++)lf(((i*61-cam/3)%280+280)%280-20,y0+4+i*3,30,2,RGB(230,230,240));lf(0,y0,240,1,RGB(150,200,255));}else for(int i=0;i<14;i++)sp_px(((i*37-cam/3)%240+240)%240,y0+6+(i*7)%20,RGB(255,200,90));}
 if(M->scene==ES_ISS||M->scene==ES_RUS){u16 T=RGB(170,170,176),A=RGB(160,118,58);
  for(int x=0;x<620;x+=8){int sx=WX(x);if(sx<-8*z||sx>240)continue;lf(sx,(90-ox*0+2-0)*0+((90-0)*z-((z-1)*60)),1,0,T);}
  int ty=z==1?88:88*2-120;for(int x=60;x<620;x+=10){int sx=WX(x);if(sx<-10*z||sx>240)continue;lf(sx,ty,10*z,4*z,T);sp_line(sx,ty,sx+10*z,ty+4*z,RGB(120,120,126));}
  for(int k=0;k<4;k++){int ax=WX(420+k*40);if(ax>-20*z&&ax<240){lf(ax,ty-60*z,8*z,60*z,A);for(int q=0;q<60*z;q+=3)lf(ax,ty-60*z+q,8*z,1,RGB(110,80,40));lf(ax,ty+4*z,8*z,30*z,A);}}
  int my=z==1?96:96*2-120;lf(WX(0),my,100*z,24*z,RGB(214,214,206));lf(WX(0),my,100*z,2*z,C_WH);lf(WX(30),my+4*z,1,16*z,RGB(160,160,160));
  if(M->scene==ES_RUS){lf(WX(100),my-2*z,110*z,28*z,RGB(214,214,200));ltx(clampi(WX(120),0,200),my+8,"ZVEZDA",RGB(120,120,120));}else ltx(clampi(WX(10),0,200),my+8*z,"QUEST",RGB(120,120,120));}
 else if(M->scene==ES_HST||M->scene==ES_MMU){int by=z==1?124:124*2-120;lf(WX(20),by,240*z,16*z,RGB(220,220,220));lf(WX(20),by,240*z,2*z,RGB(150,150,150));lf(WX(20),by+2*z,240*z,3*z,C_BK);lf(WX(250),by-20*z,20*z,36*z,C_WH);
  if(M->scene==ES_HST){int hx=WX(250),hy=by-90*z;lf(hx,hy,24*z,80*z,RGB(200,200,206));lf(hx,hy,24*z,2*z,C_WH);lf(hx-26*z,hy+30*z,24*z,8*z,RGB(40,60,120));lf(hx+26*z,hy+30*z,24*z,8*z,RGB(40,60,120));lf(hx,hy+80*z,24*z,8*z,RGB(120,120,126));}
  ltx(clampi(WX(30),0,200),by+6,M->scene==ES_HST?"ATLANTIS":"CHALLENGER",RGB(80,80,80));}
 #undef WX
}
static int sp_eva_run(int mi){const EvMis*M=&EVM[mi];memset(&EV,0,sizeof EV);int au=sp_isauto();
 EV.cum[0]=0;for(int i=1;i<M->np;i++)EV.cum[i]=EV.cum[i-1]+idist(M->px[i]-M->px[i-1],M->py[i]-M->py[i-1]);EV.len=EV.cum[M->np-1];EV.o2=420*60;EV.bat=480*60;EV.gas=600;EV.clip=0;EV.clipS=-1;EV.mmu=M->scene==ES_MMU;EV.mx=60<<4;EV.my=110<<4;EV.task=-1;
 int out=0,tgt=0,lastHand=-1;
 for(;;EV.t++){next_frame();u16 ky=keys,pr=pressed;int ax,ay;
  int wsS=M->nws?EV.cum[M->ws[0]]:0;int goal=EV.done?0:wsS;
  if(au){ky=0;pr=0;
   if(EV.mmu){int tx=out?60:300,ty=out?110:70;int x=EV.mx>>4,y=EV.my>>4;if(x<tx-2&&EV.mvx<12)ky|=K_RIGHT;if(x>tx+2&&EV.mvx>-12)ky|=K_LEFT;if(y<ty-2&&EV.mvy<8)ky|=K_DOWN;if(y>ty+2&&EV.mvy>-8)ky|=K_UP;if(idist(x-tx,y-ty)<20){if(EV.mvx>0)ky|=K_LEFT;if(EV.mvx<0)ky|=K_RIGHT;}}
   else if(EV.drift){pr|=(EV.t&1)?0:(K_L|K_R);ky|=K_L|K_R;ky|=EV.dx>0?K_LEFT:K_RIGHT;}
   else if(EV.task>=0){const EvStep*S2=&ETK[M->task[0]][EV.step];if(S2->hold)ky|=S2->key;else if((EV.t&15)==0)pr|=S2->key;}
   else{int fw=goal>EV.s;int dist=fw?goal-EV.s:EV.s-goal;
    // anchors every 60 px; clip at the anchor nearest when tether nearly taut
    int an=((EV.s+30)/60)*60;if(EV.clipS<0||(fabsi(EV.s-EV.clipS)>40&&fabsi(EV.s-an)<3&&an!=EV.clipS))pr|=K_SEL;
    else if(dist>2){ky|=fw?K_RIGHT:K_LEFT;if((EV.t&7)==0)pr|=(EV.hand&1)?K_L:K_R;}
    else if(!EV.done&&EV.task<0&&(EV.t&15)==0)pr|=K_A;}}
  if(pr&K_B){EV.cam^=1;sfx_menu();}
  if(!EV.mmu){
   if(EV.drift){EV.dx+=EV.dx>0?1:-1;if(EV.safer){if(ky&K_LEFT)EV.dx-=3;if(ky&K_RIGHT)EV.dx+=3;EV.gas-=2;if(fabsi(EV.dx)<20){EV.drift=0;EV.dx=0;spf_msg("SAFER: BACK ON THE HANDRAIL");}}
    else if((ky&(K_L|K_R))==(K_L|K_R)){EV.safer=1;spf_msg("SAFER JETPACK ACTIVATED");}
    if(EV.gas<=0||fabsi(EV.dx)>900)return -1;}
   else if(EV.task>=0){const EvStep*S2=&ETK[M->task[0]][EV.step];int ok=0;if(S2->hold){if(ky&S2->key){if(++EV.hold>=S2->hold)ok=1;}else EV.hold=0;}else if(pr&S2->key)ok=1;else if(pr&(K_A|K_UP|K_DOWN|K_LEFT|K_RIGHT)){EV.score-=20;}
    if(ok){sfx_ok();EV.hold=0;EV.score+=150;if(++EV.step>=6){EV.task=-1;EV.done=1;spf_msg("TASK COMPLETE - RETURN TO THE AIRLOCK");}}}
   else{
    if(pr&K_SEL){int an=((EV.s+30)/60)*60;if(fabsi(EV.s-an)<6){EV.clipS=an;EV.clip=1;spf_msg("TETHER CLIPPED (MAKE BEFORE BREAK)");sfx_menu();}else if(EV.clip){EV.clip=0;EV.clipS=-1;spf_msg("TETHER UNCLIPPED!");}}
    int dir=(ky&(K_RIGHT|K_UP))?1:(ky&(K_LEFT|K_DOWN))?-1:0;int hnd=(pr&K_L)?0:(pr&K_R)?1:-1;
    if(dir&&hnd>=0){if(hnd==lastHand){spf_msg("ALTERNATE HANDS (L/R)");}else{int ns=EV.s+dir*6;ns=clampi(ns,0,EV.len);
      if(EV.clip&&fabsi(ns-EV.clipS)>70){spf_msg("TETHER TAUT - CLIP AT THE NEXT RING");}
      else{EV.s=ns;EV.hand++;lastHand=hnd;if(!EV.clip&&rr(100)<12){EV.drift=1;EV.dx=dir*30;EV.safer=0;spf_msg("LOST GRIP! L+R: SAFER");sfx_hurt();}}}}
    if(pr&K_A){if(!EV.done&&M->nws&&fabsi(EV.s-wsS)<6){EV.task=0;EV.step=0;spf_msg("WORKSITE - FOLLOW THE PROCEDURE");}}
    if(EV.done&&EV.s<3){break;}}
  }else{ // MMU
   int x=EV.mx>>4,y=EV.my>>4;if(ky&K_RIGHT){EV.mvx++;EV.gas--;}if(ky&K_LEFT){EV.mvx--;EV.gas--;}if(ky&K_UP){EV.mvy--;EV.gas--;}if(ky&K_DOWN){EV.mvy++;EV.gas--;}EV.mvx=clampi(EV.mvx,-16,16);EV.mvy=clampi(EV.mvy,-12,12);EV.mx+=EV.mvx;EV.my+=EV.mvy;
   if(!out&&idist(x-300,y-70)<12){out=1;spf_msg("98 M FROM CHALLENGER - FIRST UNTETHERED EVA!");EV.score+=600;}
   if(out&&idist(x-60,y-110)<10&&fabsi(EV.mvx)<4&&fabsi(EV.mvy)<4){EV.done=1;break;}if(EV.gas<=0)return -1;}
  EV.gm=EV.t/60;EV.o2-=1;EV.bat-=1;if(EV.o2<=0)return -1;
  int night=((EV.gm+10)%90)>=45;
  if(EV.mmu){ax=EV.mx>>4;ay=EV.my>>4;}else eva_pos(M,EV.s,&ax,&ay);if(EV.drift){ay-=fabsi(EV.dx)/16;ax+=EV.dx/10;}
  int z=EV.cam?2:1;int cam=clampi(ax-(z==1?120:60),0,640);
  BEAT->screen=SCR_SPEVA;BEAT->mode=mi;BEAT->x=EV.mmu?ax:EV.s;BEAT->y=EV.done|(EV.clip<<1)|(EV.drift<<2)|(night<<3)|(EV.cam<<4);BEAT->timer=EV.gm;BEAT->scoreA=EV.score;BEAT->phase=EV.task>=0?1:EV.done?2:0;
  lfb=0;eva_scene(M,cam,z,night);
  if(!EV.mmu){// handrail + anchor rings + worksite + tether line
   for(int i=0;i<M->np-1;i++){int x0=(M->px[i]-cam)*z,y0=z==1?M->py[i]:M->py[i]*2-120,x1=(M->px[i+1]-cam)*z,y1=z==1?M->py[i+1]:M->py[i+1]*2-120;sp_line(x0,y0+2,x1,y1+2,RGB(230,190,60));sp_line(x0,y0+3,x1,y1+3,RGB(150,120,40));}
   for(int a=0;a<=EV.len;a+=60){int x,y;eva_pos(M,a,&x,&y);int sx=(x-cam)*z,sy=z==1?y:y*2-120;sp_circ(sx,sy+2,2*z,RGB(255,230,80));}
   if(M->nws&&!EV.done){int x,y;eva_pos(M,wsS,&x,&y);int sx=(x-cam)*z,sy=z==1?y:y*2-120;lf(sx-4*z,sy-16*z,8*z,10*z,RGB(80,220,120));lf(sx-3*z,sy-15*z,6*z,8*z,RGB(20,60,30));}
   if(EV.clip){int x,y;eva_pos(M,EV.clipS,&x,&y);sp_line((x-cam)*z,(z==1?y:y*2-120)+2,(ax-cam)*z,(z==1?ay:ay*2-120)-4*z,RGB(255,255,255));}}
  else{int bx=(300-cam)*z,by=70;sp_circ(bx,by,6,RGB(80,220,120));ltx(clampi(bx-10,0,200),by+8,"98 M",RGB(80,220,120));}
  {int sx=(ax-cam)*z,sy=z==1?ay:ay*2-120;eva_astro(sx,sy,z,M->suit,EV.hand,night);if(EV.mmu){lf(sx-8*z,sy-9*z,3*z,9*z,RGB(200,200,196));lf(sx+5*z,sy-9*z,3*z,9*z,RGB(200,200,196));}if(EV.safer&&EV.drift)lf(sx-8,sy-4,3,2,RGB(200,220,255));
   if(night){for(int j=0;j<20;j++)sp_blend(sx+6*z+j*2,sy-12*z-j/2,2,1+j/3,RGB(255,250,200),60);}}
  if(EV.cam){for(int y=0;y<160;y++){int w=isq(clampi(120*120-(y-80)*(y-80)*2,0,99999));lf(0,y,120-w,1,RGB(10,10,12));lf(120+w,y,120-w,1,RGB(10,10,12));}ltx(80,140,"SUIT CAM - HELMET",RGB(140,255,160));}
  char b[48],*o;lf(0,0,240,10,RGB(8,10,20));o=sp_put(b,M->suit?"ORLAN ":"EMU ");sp_put(o,M->n);ltx(2,1,b,RGB(255,220,120));
  o=sp_put(b,"GET ");o=sp_int(o,EV.gm/60);*o++=':';*o++='0'+(EV.gm%60)/10;*o++='0'+EV.gm%10;*o=0;ltx(150,1,b,RGB(140,255,160));ltx(204,1,night?"NIGHT":"DAY",night?RGB(120,140,255):RGB(255,230,120));
  lf(0,150,240,10,RGB(8,10,20));o=sp_put(b,"O2 ");ltx(2,151,b,RGB(150,150,170));sp_bar(20,152,40,5,EV.o2,420*60,RGB(90,200,255));ltx(64,151,"BAT",RGB(150,150,170));sp_bar(84,152,30,5,EV.bat,480*60,RGB(255,220,90));
  ltx(118,151,EV.mmu?"N2":EV.clip?"TETHER OK":"NO TETHER",EV.mmu?RGB(150,150,170):EV.clip?C_GREEN:RGB(255,100,80));if(EV.mmu||EV.safer)sp_bar(196,152,40,5,EV.gas,600,RGB(200,200,255));
  if(EV.task>=0){const EvStep*S2=&ETK[M->task[0]][EV.step];lf(20,20,200,22,RGB(8,10,20));o=sp_put(b,"STEP ");o=sp_int(o,EV.step+1);sp_put(o,"/6");ltx(24,22,b,RGB(255,220,120));ltx(24,31,S2->s,C_WHITE);if(S2->hold)sp_bar(150,23,60,5,EV.hold,S2->hold,C_GREEN);}
  else if(F.msgT>0){F.msgT--;ltc(138,F.msg,RGB(255,240,160));}else ltc(138,EV.mmu?"D-PAD: MMU JETS - FLY OUT, COME BACK SLOW":EV.done?"RETURN TO THE AIRLOCK":"HOLD DIR + ALTERNATE L/R  SEL: CLIP",RGB(150,170,210));
  m3_present();if(EV.t>60*600)return -1;}
 return 1;}
static void sp_eva(void){int sel=SPR.lastEva<7?SPR.lastEva:0;
 for(;;){int i=sp_list("SPACEWALK (EVA)","EMU / ORLAN - HAND OVER HAND",7,eva_nm,eva_rt,sel,SCR_SPSEL+0);if(i<0)return;sel=i;const EvMis*M=&EVM[i];
  sp_brief(M->n,M->sub,cat(M->suit?"ORLAN-MKS SUIT - ":"EMU SUIT - ",M->air),"O2 7 H, DAY/NIGHT EVERY 45 MIN","B: SUIT CAM  SELECT: CLIP TETHER",EVM[i].scene==ES_MMU?"1984: BRUCE MCCANDLESS FLEW THE MMU|98 M FROM CHALLENGER. D-PAD = JETS.|FLY TO THE GREEN RING AND BACK.|LOW SPEED AT THE BAY.":"HOLD A DIRECTION + PRESS L, R, L, R|(ONE HAND AT A TIME). CLIP AT EACH|YELLOW RING. NO TETHER + SLIP = DRIFT:|HOLD L+R FOR SAFER, D-PAD BACK.");
  BEAT->mode=i;if(!sp_wait_ab(SCR_SPSEL))continue;bgm_t(TR_SPACE);int r=sp_eva_run(i);int ri=i<6?i:5;
  int sc=r>0?clampi(EV.score+1000+EV.o2/60*2-(EV.drift?0:0),0,9999):0;int st=r>0?1+(sc>=1600)+(sc>=2000&&!EV.safer):0;int best=sc>SPR.eva[ri];if(best)SPR.eva[ri]=sc;if(st>SPR.evaS[ri])SPR.evaS[ri]=st;SPR.lastEva=i;spr_write();if(r>0)music_victory();
  for(int t=0;;t++){next_frame();BEAT->screen=SCR_SPEVA;BEAT->phase=r>0?3:4;BEAT->scoreA=sc;BEAT->scoreB=st;if(t==0){char b[48],*o;chrome(CAT_EXPLORE,0);m3_box(16,24,208,112,1);m3_ctext(32,r>0?"EVA COMPLETE":"EVA TERMINATED",r>0?C_YEL:C_RED,2);m3_ctext(52,M->n,C_WHITE,1);
    o=sp_put(b,"DURATION ");o=sp_int(o,EV.gm/60);o=sp_put(o," H ");o=sp_int(o,EV.gm%60);sp_put(o," MIN");m3_ctext(66,b,C_GRAY,1);m3_ctext(78,r>0?(EV.safer?"SAFER USED - CREW SAFE":"TETHER DISCIPLINE OK"):"BACK INSIDE - SAFE. TRY AGAIN.",C_GRAY,1);
    o=sp_put(b,"SCORE ");o=sp_int(o,sc);if(best)sp_put(o,"  BEST!");m3_ctext(94,b,best?C_GREEN:C_WHITE,1);for(int s=0;s<3;s++)m3_fill(99+s*14,106,10,10,s<st?C_YEL:RGB(50,50,60));m3_ctext(124,"A: CONTINUE",C_GRAY,1);m3_present();}
   if(sp_isauto()&&t>200)pressed=K_A;if(t>20&&(pressed&(K_A|K_B))){pressed=0;break;}}
  bgm_t(TR_SPACE);}}
