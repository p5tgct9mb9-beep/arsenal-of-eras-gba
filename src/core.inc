// ===================================================================== core
void *memcpy(void *d,const void *s,unsigned n){u8*a=d;const u8*b=s;while(n--)*a++=*b++;return d;}
void *memset(void *d,int v,unsigned n){u8*a=d;while(n--)*a++=(u8)v;return d;}
#define EWRAM __attribute__((section(".ewram")))
#define REG_WAITCNT REG16(0x4000204)
#define SIN(a) (sin_lut[(a)&255])
#define COS(a) (sin_lut[((a)+64)&255])
static inline int iabs(int v){return v<0?-v:v;}
static inline int clampi(int v,int a,int b){return v<a?a:v>b?b:v;}
static inline int angd(int a,int b){return ((a-b+128)&255)-128;}   // signed diff in 256-units
typedef struct{u32 magic,frame,screen,mode,era,hp,scoreA,scoreB,timer,phase,kills,deaths,menu,x,y,extra;}Beat;
#define BEAT ((volatile Beat*)0x0203FF00)
enum{SCR_TITLE=1,SCR_MENU,SCR_ERA,SCR_CAMP,SCR_INF,SCR_TANK,SCR_AIR,SCR_RESULT,SCR_PAUSE,SCR_DONE,SCR_LOADOUT};
static u16 keys,prevk,pressed;static u32 frame;
static void poll(void){prevk=keys;keys=~REG_KEYINPUT&0x3FF;pressed=keys&~prevk;}
static void vsync(void){while(REG_VCOUNT>=160);while(REG_VCOUNT<160);}
static void dma32(const void*src,volatile void*dst,u32 words){REG_DMA3CNT=0;REG_DMA3SAD=(u32)src;REG_DMA3DAD=(u32)dst;REG_DMA3CNT=words|0x84000000;}
static void fill32(volatile void*dst,u32 val,u32 words){static volatile u32 v;v=val;REG_DMA3CNT=0;REG_DMA3SAD=(u32)&v;REG_DMA3DAD=(u32)dst;REG_DMA3CNT=words|0x85000000;}
static u32 rs=0x1234567;
static u32 rnd(void){rs^=rs<<13;rs^=rs>>17;rs^=rs<<5;return rs;}
static int rr(int n){return (int)((rnd()>>8)%(u32)n);}
static int atan2i(int x,int y){ // 0=north(-y), clockwise, 256 units
 int ux=x,uy=-y;if(!ux&&!uy)return 0;int ax=iabs(ux),ay=iabs(uy),t;
 if(ax<=ay)t=atan_lut[ax*64/ay];else t=64-atan_lut[ay*64/ax];
 if(ux>=0&&uy>=0)return t; if(ux>=0)return 128-t; if(uy<0)return 128+t; return (256-t)&255;}
static int idist(int dx,int dy){dx=iabs(dx);dy=iabs(dy);return dx>dy?dx+dy*3/8:dy+dx*3/8;}

// ---------------------------------------------------------------- sound
static int musPos=-1,musT,musLen;static const u16*musData;static int musRate=8;
static void snd_init(void){REG_SOUNDCNT_X=0x80;REG_SOUNDCNT_L=0xFF77;REG_SOUNDCNT_H=2;}
static void sfx_shot(int heavy){REG_SND4CNT=((heavy?0xF:0xA)<<12)|((heavy?3:1)<<8);REG_SND4FREQ=0x8000|(heavy?(5<<4)|4:(2<<4)|1);}
static void sfx_laser(void){REG_SND1SWEEP=(1<<4)|(1<<3)|3;REG_SND1CNT=(0xB<<12)|(1<<8)|(2<<6);REG_SND1FREQ=0x8000|1900;}
static void sfx_boom(void){REG_SND4CNT=(0xF<<12)|(5<<8);REG_SND4FREQ=0x8000|(6<<4)|6;}
static void sfx_hurt(void){REG_SND1SWEEP=(3<<4)|(1<<3)|2;REG_SND1CNT=(0xC<<12)|(2<<8)|(2<<6);REG_SND1FREQ=0x8000|1200;}
static void sfx_whoosh(void){REG_SND1SWEEP=(2<<4)|2;REG_SND1CNT=(0x9<<12)|(3<<8)|(1<<6);REG_SND1FREQ=0x8000|1300;}
static void sfx_menu(void){if(musPos>=0)return;REG_SND2CNT=(0x8<<12)|(1<<8)|(1<<6)|50;REG_SND2FREQ=0xC000|1750;}
static void sfx_ok(void){REG_SND1SWEEP=(2<<4)|3;REG_SND1CNT=(0xA<<12)|(2<<8)|(2<<6);REG_SND1FREQ=0x8000|1600;}
static void music_start(const u16*d,int len,int rate){musData=d;musLen=len;musPos=0;musT=0;musRate=rate;}
static void music_stop(void){musPos=-1;REG_SND2CNT=0;}
static void music_tick(void){if(musPos<0)return;if(musT--)return;musT=musRate-1;u16 n=musData[musPos];
 if(n){REG_SND2CNT=(0x9<<12)|(3<<8)|(2<<6);REG_SND2FREQ=0x8000|n;}
 if(++musPos>=musLen){if(musData==title_song)musPos=0;else musPos=-1;}}

// ---------------------------------------------------------------- frame
static void beat_common(int scr){BEAT->magic=0xA0E5;BEAT->screen=scr;}
static void next_frame(void){vsync();frame++;BEAT->frame=frame;poll();music_tick();}

// ---------------------------------------------------------------- mode 3 drawing
static u16 fb[240*160] EWRAM;
#define M3 fb
static void m3_present(void){vsync();dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);}
static u16 dimc(u16 c){return (c>>1)&0x3DEF;}
static void m3_title(int dim){dma32(dim?title_dim:title_bmp,fb,240*160/2);}
static void m3_box(int x0,int y0,int w,int h,int mode){ // mode 0: darken, 1: solid navy
 if(mode<2)for(int y=y0;y<y0+h&&y<160;y++)for(int x=x0;x<x0+w&&x<240;x++){u16*p=&M3[y*240+x];*p=mode?0x1C42:dimc(dimc(*p));}
 for(int x=x0;x<x0+w;x++){M3[y0*240+x]=0x6B5A;M3[(y0+h-1)*240+x]=0x6B5A;}
 for(int y=y0;y<y0+h;y++){M3[y*240+x0]=0x6B5A;M3[y*240+x0+w-1]=0x6B5A;}}
static int slen(const char*s){int n=0;while(s[n])n++;return n;}
static void m3_text(int x,int y,const char*s,u16 col,int sc){
 for(;*s;s++,x+=6*sc){int c=*s;if(c>='a'&&c<='z')c-=32;if(c<32||c>127)c='?';const u8*g=&font_bits[(c-32)*7];
  for(int pass=0;pass<2;pass++)for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<gx))
   for(int oy=0;oy<sc;oy++)for(int ox=0;ox<sc;ox++){int X=x+gx*sc+ox+(pass?0:1),Y=y+gy*sc+oy+(pass?0:1);if(X>=0&&X<240&&Y>=0&&Y<160)M3[Y*240+X]=pass?col:0x0421;}}}
static void m3_ctext(int y,const char*s,u16 col,int sc){m3_text(120-(slen(s)*6*sc-sc)/2,y,s,col,sc);}
#define RGB(r,g,b) ((r>>3)|((g>>3)<<5)|((b>>3)<<10))
#define C_WHITE RGB(248,248,248)
#define C_YEL RGB(255,214,64)
#define C_GRAY RGB(160,164,176)
#define C_RED RGB(255,90,80)
#define C_BLUE RGB(110,190,255)
#define C_GREEN RGB(110,230,120)

// ---------------------------------------------------------------- HUD (BG0, SB28, charblock 0)
static u16 hud[32*32] EWRAM;
enum{HB_WHITE=15,HB_YEL=14,HB_RED=13,HB_BLUE=12,HB_GREEN=11,HB_GRAY=10};
static void hud_clear(void){memset(hud,0,sizeof hud);}
static void hput(int x,int y,int t,int b){if(x>=0&&x<30&&y>=0&&y<20)hud[y*32+x]=t|(b<<12);}
static void htxt(int x,int y,const char*s,int b){for(;*s;s++,x++){int c=*s;if(c>='a'&&c<='z')c-=32;if(c<32||c>127)c=32;if(c!=32)hput(x,y,c-32,b);}}
static void hptxt(int x,int y,const char*s,int b){for(;*s;s++,x++){int c=*s;if(c>='a'&&c<='z')c-=32;if(c<32||c>127)c=32;hput(x,y,c-32+HT_PANEL_FONT,b);}}
static void hctxt(int y,const char*s,int b,int panel){int x=(30-slen(s))/2;if(panel)hptxt(x,y,s,b);else htxt(x,y,s,b);}
static void hpanel(int x,int y,int w,int h){for(int j=0;j<h;j++)for(int i=0;i<w;i++){int t=0;
 int T=j==0,B=j==h-1,L=i==0,R=i==w-1;
 if(T&&L)t=5;else if(T&&R)t=6;else if(B&&L)t=7;else if(B&&R)t=8;else if(T)t=1;else if(B)t=2;else if(L)t=3;else if(R)t=4;
 hput(x+i,y+j,HT_PANEL+t,15);}}
static void hbar(int x,int y,int cells,int v,int max){int f=max>0?v*cells*8/max:0;if(v>0&&f==0)f=1;for(int i=0;i<cells;i++){int n=f-i*8;n=n<0?0:n>8?8:n;hput(x+i,y,HT_BAR+n,15);}}
static char nb[4][12];static int nbi;
static const char*itoa(int v){char*o=nb[nbi++&3];char t[12];int n=0,neg=v<0;if(neg)v=-v;do{t[n++]='0'+v%10;v/=10;}while(v);int k=0;if(neg)o[k++]='-';while(n)o[k++]=t[--n];o[k]=0;return o;}
static const char*mmss(int frames){char*o=nb[nbi++&3];int s=frames/60;if(s<0)s=0;int m=s/60;s%=60;int k=0;if(m>=10)o[k++]='0'+m/10;o[k++]='0'+m%10;o[k++]=':';o[k++]='0'+s/10;o[k++]='0'+s%10;o[k]=0;return o;}
static char sbuf[64];
static const char*cat(const char*a,const char*b){int k=0;while(*a&&k<62)sbuf[k++]=*a++;while(*b&&k<62)sbuf[k++]=*b++;sbuf[k]=0;return sbuf;}

// ---------------------------------------------------------------- OAM + affine
static u16 oam[128*4] EWRAM;static int noam,naff;static u16 affkey[32];
static void oam_begin(void){for(int i=0;i<128;i++){oam[i*4]=0x0200;oam[i*4+1]=0;oam[i*4+2]=0;}noam=0;naff=0;}
static void spr(int x,int y,int tile,int shape,int size,int pal,int prio){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static int aff_get(int ang,int sx){ // sx: x-scale 8.8 (roll); 0x100 normal; 0x80 double
 u16 key=(ang&255)|((sx>>3)<<8);for(int i=0;i<naff;i++)if(affkey[i]==key)return i;
 if(naff>=32){int best=0,bd=999;for(int i=0;i<32;i++){int d=iabs(angd(affkey[i]&255,ang));if((affkey[i]>>8)==(key>>8)&&d<bd){bd=d;best=i;}}return best;}
 int k=naff++;affkey[k]=key;int c=COS(ang),s=SIN(ang);int kx=sx>=0x100?256:(256*256/(sx<16?16:sx));
 if(sx==0x80){c*=2;s*=2;kx=256;}  // 2x enlarge (double-size)
 if(sx==0x80){oam[(k*4+0)*4+3]=(u16)(c/2>0?128:128);oam[(k*4+1)*4+3]=0;oam[(k*4+2)*4+3]=0;oam[(k*4+3)*4+3]=128;return k;}
 oam[(k*4+0)*4+3]=(u16)(s16)(c*kx/256);oam[(k*4+1)*4+3]=(u16)(s16)(s*kx/256);oam[(k*4+2)*4+3]=(u16)(s16)(-s);oam[(k*4+3)*4+3]=(u16)(s16)c;return k;}
// affine sprite centred at (cx,cy); sz 0=8,1=16,2=32
static void spr_aff(int cx,int cy,int tile,int sz,int pal,int ang,int sx,int prio,int semi){
 int w=8<<sz;if(noam>=128||cx<-w||cx>240+w||cy<-w||cy>160+w)return;int m=aff_get(ang,sx);int dbl=sx==0x80;
 int x=cx-(dbl?w:w/2),y=cy-(dbl?w:w/2);u16*o=&oam[noam*4];
 o[0]=(y&255)|(1<<8)|(dbl<<9)|(semi?1<<10:0);o[1]=(x&511)|(m<<9)|(sz<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}

// ---------------------------------------------------------------- tile world (BG1 SB30 cb1, clouds BG2 SB29)
#define MAXW 64
static u8 map[64*64] EWRAM;static u8 cmap[64*64] EWRAM;static u16 bgw[1024] EWRAM,bgc[1024] EWRAM;
static int mapW,mapH,mapWrap,camX,camY,borderTile;static u8 tflag[32];
enum{TF_SOLID=1,TF_SHOT=2,TF_SLOW=4};
static inline int cell(int cx,int cy){if(mapWrap){cx&=mapW-1;cy&=mapH-1;}else if(cx<0||cy<0||cx>=mapW||cy>=mapH)return borderTile;return map[cy*MAXW+cx];}
static inline int flag_at(int px,int py){return tflag[cell(px>>4,py>>4)];}
static void draw_layer(const u8*m,u16*buf,int cx0,int cy0,int bank,int wrap){
 int tx0=cx0>>4,ty0=cy0>>4;
 for(int cy=ty0;cy<ty0+11;cy++)for(int cx=tx0;cx<tx0+16;cx++){int t;
  if(wrap)t=m[(cy&(mapH-1))*MAXW+(cx&(mapW-1))];else t=(cx<0||cy<0||cx>=mapW||cy>=mapH)?borderTile:m[cy*MAXW+cx];
  u16 b=(t*4)|(bank<<12);int X=(cx*2)&31,Y=(cy*2)&31;
  buf[Y*32+X]=b;buf[Y*32+X+1]=b+1;buf[(Y+1)*32+X]=b+2;buf[(Y+1)*32+X+1]=b+3;}}
static void present(int clouds,int ccx,int ccy){
 vsync();
 dma32(oam,OAM,256);dma32(hud,SCREENBLOCK(28),512);dma32(bgw,SCREENBLOCK(30),512);
 REG_BGHOFS(1)=camX;REG_BGVOFS(1)=camY;
 if(clouds){dma32(bgc,SCREENBLOCK(29),512);REG_BGHOFS(2)=ccx;REG_BGVOFS(2)=ccy;}
 frame++;BEAT->frame=frame;poll();music_tick();}
static void setup_mode0(const u32*tiles,int ntilewords,const u16*pal,int era,int clouds){
 vsync();REG_DISPCNT=0x80; // forced blank while loading
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);
 for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(tiles,CHARBLOCK(1),ntilewords);for(int i=0;i<16;i++)PAL_BG[i]=pal[i];
 dma32(fx_tiles,OBJ_TILES,sizeof(fx_tiles)/4);
 const u32*es=era==0?era_spr_0:era==1?era_spr_1:era==2?era_spr_2:era==3?era_spr_3:era_spr_4;
 dma32(es,OBJ_TILES+64*8,ES_NTILES*8);
 dma32(obj_team_pal+era*48,PAL_OBJ,24);dma32(fx_pal,PAL_OBJ+3*16,8);
 for(int i=0;i<16;i++)PAL_OBJ[4*16+i]=RGB(8,8,16);
 REG_BGCNT(0)=0|(0<<2)|(28<<8);REG_BGCNT(1)=3|(1<<2)|(30<<8);REG_BGCNT(2)=2|(1<<2)|(29<<8);
 REG_BGHOFS(0)=0;REG_BGVOFS(0)=0;
 REG_BLDCNT=(1<<2)|(1<<6)|(1<<9)|(1<<12)|(1<<13);REG_BLDALPHA=10|(7<<8);
 if(!clouds)REG_BLDCNT=(1<<6)|(1<<9);
 hud_clear();oam_begin();memset(bgw,0,2048);memset(bgc,0,2048);
 dma32(hud,SCREENBLOCK(28),512);dma32(oam,OAM,256);
 REG_DISPCNT=0|(1<<6)|(1<<8)|(1<<9)|(clouds?1<<10:0)|(1<<12);}
#define OT_FX 0
#define OT_ERA 64

// ---------------------------------------------------------------- shared entities
typedef struct{s32 x,y;s16 vel;u16 ang,tur;s16 hp,maxhp;u8 team,alive,isp,wpn;
 s16 cd,cd2,resp,inv,hurt,anim,roll,rollcd,ammo,gren,kills,deaths,think,tgt,stuck,wander,wang,evade,regen,mode,mag[2],rl,stun,tact,veh;s32 lx,ly,gx,gy;}Unit;
#define NU 12
static Unit U[NU];
enum{K_BUL,K_LASER,K_PLASMA,K_SHELL,K_GREN,K_MISSILE,K_ROCKET};
typedef struct{s32 x,y,vx,vy;s16 life,dmg,dist;u8 team,kind,owner,sub;s8 tgt;u16 ang;}Shot;
#define NS 80
static Shot S[NS];
typedef struct{s16 x,y,t,life;u8 kind;}Fx;
#define NFX 40
static Fx FXs[NFX];
enum{FXK_SMALL,FXK_BIG,FXK_SMOKE,FXK_FLASH,FXK_SPARK};
static void fx_add(int x,int y,int kind){for(int i=0;i<NFX;i++)if(FXs[i].life<=0){Fx*f=&FXs[i];f->x=x;f->y=y;f->kind=kind;f->t=0;f->life=kind==FXK_BIG?32:kind==FXK_SMALL?20:kind==FXK_SMOKE?40:kind==FXK_FLASH?4:8;return;}}
static Shot*shot_new(void){for(int i=0;i<NS;i++)if(S[i].life<=0){Shot*s=&S[i];memset(s,0,sizeof*s);s->tgt=-1;return s;}return 0;}
static int wrapd(int d,int W){if(!mapWrap)return d;if(d>W/2)d-=W;if(d<-W/2)d+=W;return d;}
static int scrx(s32 x){return wrapd((x>>8)-camX-120,mapW*16)+120;}
static int scry(s32 y){return wrapd((y>>8)-camY-80,mapH*16)+80;}
static void fx_update_draw(void){
 for(int i=0;i<NFX;i++){Fx*f=&FXs[i];if(f->life<=0)continue;f->t++;if(f->t>=f->life){f->life=0;continue;}
  int x=wrapd(f->x-camX-120,mapW*16)+120,y=wrapd(f->y-camY-80,mapH*16)+80;
  switch(f->kind){
   case FXK_SMALL:spr(x-8,y-8,OT_FX+FX_EXPL+(f->t*4/f->life)*4,0,1,3,1);break;
   case FXK_BIG:spr_aff(x,y,OT_FX+FX_EXPL+(f->t*4/f->life)*4,1,3,0,0x80,1,0);break;
   case FXK_SMOKE:spr(x-4,y-4-f->t/4,OT_FX+(f->t<20?FX_SMOKE:FX_SMOKE2),0,0,3,1);break;
   case FXK_FLASH:spr(x-4,y-4,OT_FX+FX_FLASH,0,0,3,1);break;
   default:spr(x-4,y-4,OT_FX+FX_SPARK,0,0,3,1);}}}
static int box_free(s32 x,s32 y,int r){int px=x>>8,py=y>>8;
 return !(flag_at(px-r,py-r)&TF_SOLID)&&!(flag_at(px+r,py-r)&TF_SOLID)&&!(flag_at(px-r,py+r)&TF_SOLID)&&!(flag_at(px+r,py+r)&TF_SOLID)&&
        !(flag_at(px,py-r)&TF_SOLID)&&!(flag_at(px,py+r)&TF_SOLID)&&!(flag_at(px-r,py)&TF_SOLID)&&!(flag_at(px+r,py)&TF_SOLID);}
static int move_unit(Unit*u,int dx,int dy,int r){int ok=1;
 if(dx){if(box_free(u->x+dx,u->y,r))u->x+=dx;else ok=0;}
 if(dy){if(box_free(u->x,u->y+dy,r))u->y+=dy;else ok=0;}return ok;}
static int wrapd(int d,int W);
static s16 smX[4],smY[4],smT[4];static int nSmoke;
static void smoke_add(int x,int y){int k=0;for(int i=0;i<4;i++)if(smT[i]<smT[k])k=i;smX[k]=x;smY[k]=y;smT[k]=330;}
static int los(int x0,int y0,int x1,int y1){int dx=x1-x0,dy=y1-y0;int d=idist(dx,dy);if(d<16)return 1;int n=d/6;
 int anysm=smT[0]>0||smT[1]>0||smT[2]>0||smT[3]>0;
 for(int i=2;i<n;i++){int x=x0+dx*i/n,y=y0+dy*i/n;if(flag_at(x,y)&TF_SHOT)return 0;
  if(anysm)for(int k=0;k<4;k++)if(smT[k]>0&&idist(x-smX[k],y-smY[k])<26)return 0;}return 1;}
static void smoke_tick_draw(void){for(int k=0;k<4;k++){if(smT[k]<=0)continue;smT[k]--;
  for(int j=0;j<7;j++){int ox=(j%3-1)*14+((j*37+(frame>>4))&3),oy=(j/3-1)*12+((j*53)&5)-((frame>>3)&1);
   int x=wrapd(smX[k]+ox-camX-120,mapW*16)+120,y=wrapd(smY[k]+oy-camY-80,mapH*16)+80;spr_aff(x,y,OT_FX+FX_SMOKE,0,3,0,0x80,1,0);}}}
static int pause_menu(void){ // returns 1 to quit
 int sel=0;u16 save[32*32];memcpy(save,hud,sizeof save);
 BEAT->screen=SCR_PAUSE;
 for(;;){memcpy(hud,save,sizeof save);hpanel(8,6,14,7);hctxt(7,"PAUSED",HB_YEL,1);
  hptxt(10,9,sel==0?">RESUME":" RESUME",sel==0?HB_WHITE:HB_GRAY);hptxt(10,10,sel==1?">QUIT":" QUIT",sel==1?HB_WHITE:HB_GRAY);
  vsync();dma32(hud,SCREENBLOCK(28),512);frame++;BEAT->frame=frame;poll();
  if(pressed&(K_UP|K_DOWN)){sel^=1;sfx_menu();}
  if(pressed&K_START){sel=0;break;}
  if(pressed&K_A)break;}
 memcpy(hud,save,sizeof save);return sel;}
