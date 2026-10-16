#define RGB(r,g,b) (((r)>>3)|(((g)>>3)<<5)|(((b)>>3)<<10))
#define C_PANEL RGB(30,46,96)
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
enum{SCR_TITLE=1,SCR_MENU,SCR_ERA,SCR_CAMP,SCR_INF,SCR_TANK,SCR_AIR,SCR_RESULT,SCR_PAUSE,SCR_DONE,SCR_LOADOUT,SCR_SETTINGS,SCR_MAPSEL,SCR_MSEL,SCR_MISSION,SCR_MBRIEF,SCR_DRONE,SCR_HANGAR,SCR_SEA,SCR_LAUNCH,SCR_WMENU,SCR_WORLD,SCR_WMAP,SCR_WLOG};
static u16 keys,prevk,pressed;static u32 frame;
static void poll(void){prevk=keys;keys=~REG_KEYINPUT&0x3FF;pressed=keys&~prevk;}
static void vsync(void){while(REG_VCOUNT>=160);while(REG_VCOUNT<160);}
static void dma32(const void*src,volatile void*dst,u32 words){REG_DMA3CNT=0;REG_DMA3SAD=(u32)src;REG_DMA3DAD=(u32)dst;REG_DMA3CNT=words|0x84000000;while(REG_DMA3CNT&0x80000000);}
static void fill32(volatile void*dst,u32 val,u32 words){static volatile u32 v;v=val;REG_DMA3CNT=0;REG_DMA3SAD=(u32)&v;REG_DMA3DAD=(u32)dst;REG_DMA3CNT=words|0x85000000;while(REG_DMA3CNT&0x80000000);}
static u32 rs=0x1234567;
static u32 rnd(void){rs^=rs<<13;rs^=rs>>17;rs^=rs<<5;return rs;}
static int rr(int n){return (int)((rnd()>>8)%(u32)n);}
static int atan2i(int x,int y){ // 0=north(-y), clockwise, 256 units
 int ux=x,uy=-y;if(!ux&&!uy)return 0;int ax=iabs(ux),ay=iabs(uy),t;
 if(ax<=ay)t=atan_lut[ax*64/ay];else t=64-atan_lut[ay*64/ax];
 if(ux>=0&&uy>=0)return t; if(ux>=0)return 128-t; if(uy<0)return 128+t; return (256-t)&255;}
static int idist(int dx,int dy){dx=iabs(dx);dy=iabs(dy);return dx>dy?dx+dy*3/8:dy+dx*3/8;}

// ---------------------------------------------------------------- sound
// v1.1: VBlank-IRQ driven 4-voice PSG sequencer (ch2 lead, ch1 harmony, ch3 wave bass, ch4 noise drums).
// SFX share ch1/ch2/ch4 and "lock" the channel for a few frames so effects always win.
typedef struct{u8 track,musVol,sfxVol,shake,diff,hints,bot;}Settings;
static Settings SET={0,8,10,1,1,1,1};
// v1.2 bot difficulty: aim jitter, extra fire delay, think interval, engage range %, HP %, AA strength %
typedef struct{s8 jit,cd,think;u8 eng,hp,aa;}BotD;
static const BotD BOTD[4]={{9,12,16,75,75,60},{4,4,8,100,100,100},{2,0,6,115,115,125},{1,-3,4,130,135,150}};
static const char*const BOTN[4]={"RECRUIT","REGULAR","VETERAN","ELITE"};
#define BD(u) (&BOTD[(u)->team==1?SET.bot:1])  // enemy bots follow the setting, allied bots stay Regular
#define REG_SND3SEL REG16(0x4000070)
#define REG_SND3CNT REG16(0x4000072)
#define REG_SND3FREQ REG16(0x4000074)
#define WAVE_RAM ((volatile u16*)0x4000090)
#define REG_IE REG16(0x4000200)
#define REG_IF REG16(0x4000202)
#define REG_IME REG16(0x4000208)
#define REG_DISPSTAT REG16(0x4000004)
#define MUSDBG ((volatile u16*)0x0203FFE0)
static volatile int musOn=0,musTrk=-1,musStep,musCnt;static const u8*musD;static const u8*musM;
static volatile u8 lock1,lock2,lock4;static int shakeT;
static int sv(int v){v=v*SET.sfxVol/10;return v>15?15:v;}
static int mv(int v){v=v*SET.musVol/8;return v>15?15:v;}
static void wave_load(int w){REG_SND3SEL=0x40;const u8*p=wave_shapes+w*16;for(int i=0;i<8;i++)WAVE_RAM[i]=p[2*i]|(p[2*i+1]<<8);REG_SND3SEL=0x80;}
static void snd_init(void){REG_SOUNDCNT_X=0x80;REG_SOUNDCNT_L=0xFF77;REG_SOUNDCNT_H=2;wave_load(0);REG_SND3CNT=0;}
static void sfx_shot(int heavy){if(!SET.sfxVol)return;lock4=heavy?14:6;REG_SND4CNT=(sv(heavy?0xF:0xA)<<12)|((heavy?3:1)<<8);REG_SND4FREQ=0x8000|(heavy?(5<<4)|4:(2<<4)|1);}
static void sfx1(int sweep,int vol,int env,int duty,int fr,int lk){if(!SET.sfxVol)return;lock1=lk;REG_SND1SWEEP=sweep;REG_SND1CNT=(sv(vol)<<12)|(env<<8)|(duty<<6);REG_SND1FREQ=0x8000|fr;}
static void sfx_laser(void){sfx1((1<<4)|(1<<3)|3,0xB,1,2,1900,10);}
static void shake(int n){if(SET.shake&&shakeT<n)shakeT=n;}
static void sfx_boom(void){if(!SET.sfxVol)return;lock4=40;REG_SND4CNT=(sv(0xF)<<12)|(5<<8);REG_SND4FREQ=0x8000|(6<<4)|6;}
static void sfx_hurt(void){shake(5);sfx1((3<<4)|(1<<3)|2,0xC,2,2,1200,16);}
static void sfx_whoosh(void){sfx1((2<<4)|2,0x9,3,1,1300,22);}
static void sfx_menu(void){sfx1(0x08,0x8,1,1,1750,8);}
static void sfx_ok(void){sfx1((2<<4)|3,0xA,2,2,1600,16);}
static void mus_silence(void){if(!lock2){REG_SND2CNT=0;REG_SND2FREQ=0x8000;}if(!lock1){REG_SND1CNT=0;REG_SND1FREQ=0x8000;}REG_SND3CNT=0;}
static void music_stop(void){musOn=0;musTrk=-1;mus_silence();}
static void music_play(int t){if(t<0||t>=NTRK){music_stop();return;}if(musOn&&musTrk==t)return;
 musOn=0;musD=trk_data[t];musM=trk_meta+t*12;musStep=0;musCnt=0;wave_load(musM[7]);musTrk=t;musOn=1;}
static const u8 B3VOL[11]={0,3,3,3,2,2,2,4,4,1,1}; // ch3 volume code by music volume (25/50/75/100%)
static void mus_step(void){const u8*m=musM;const u8*d=musD+musStep*4;
 int ld=d[0],bs=d[1],hm=d[2],dr=d[3];
 if(!lock2){if(ld>=2){REG_SND2CNT=(mv(m[8])<<12)|(m[4]<<8)|(m[3]<<6);REG_SND2FREQ=0x8000|sq_freq[ld];}else if(ld==1){REG_SND2CNT=0;REG_SND2FREQ=0x8000;}}
 if(!lock1){if(hm>=2){REG_SND1SWEEP=0x08;REG_SND1CNT=(mv(m[9])<<12)|(m[6]<<8)|(m[5]<<6);REG_SND1FREQ=0x8000|sq_freq[hm];}else if(hm==1){REG_SND1CNT=0;REG_SND1FREQ=0x8000;}}
 if(bs>=2){int vc=SET.musVol>10?1:B3VOL[SET.musVol];if(m[10]==3&&vc==1)vc=4;REG_SND3CNT=vc==4?0x8000:(vc<<13);REG_SND3FREQ=0x8000|wv_freq[bs];}else if(bs==1)REG_SND3CNT=0;
 if(dr&&!lock4&&SET.musVol){int v;switch(dr){
   case 1:v=mv(12);REG_SND4CNT=(v<<12)|(1<<8);REG_SND4FREQ=0x8000|(6<<4)|3;break;          // kick
   case 2:case 4:v=mv(dr==2?10:7);REG_SND4CNT=(v<<12)|(1<<8);REG_SND4FREQ=0x8000|(2<<4)|2;break; // snare / roll
   case 3:v=mv(5);REG_SND4CNT=(v<<12)|(1<<8)|40;REG_SND4FREQ=0xC000|(1<<4)|8|0;break;          // hat (short, metallic)
   case 5:v=mv(10);REG_SND4CNT=(v<<12)|(2<<8);REG_SND4FREQ=0x8000|(5<<4)|5;break;           // tom
   default:v=mv(4);REG_SND4CNT=(v<<12)|(3<<8);REG_SND4FREQ=0x8000|(1<<4)|8|1;}}             // cymbal shimmer
}
static volatile u32 vblN; // vblank counter (for fixed-rate sim catch-up)
void mus_isr(void){ // called from the ARM IRQ stub on every VBlank
 u16 f=REG_IF;if(f&1)vblN++;
 if(lock1)lock1--;if(lock2)lock2--;if(lock4)lock4--;
 if(musOn){if(musCnt==0){mus_step();musCnt=musM[1];
    if(++musStep>=musM[0]){if(musM[2])musStep=0;else{musOn=0;mus_silence();}}}
  else{musCnt--;if(musD[((musStep+musM[0]-1)%musM[0])*4+3]==4&&musCnt==(musM[1]>>1)&&!lock4){REG_SND4CNT=(mv(7)<<12)|(1<<8);REG_SND4FREQ=0x8000|(2<<4)|2;}}}
 MUSDBG[0]=musOn;MUSDBG[1]=musTrk;MUSDBG[2]=musStep;
 REG_IF=f;*(volatile u16*)0x03007FF8|=f;}
extern void isr_stub(void);
static void irq_init(void){*(volatile u32*)0x03007FFC=(u32)isr_stub;REG_DISPSTAT|=0x08;REG_IE=1;REG_IME=1;}
static const u8 DIFF_PCT[3]={65,100,135};static int diffd(int d){return d*DIFF_PCT[SET.diff]/100;}
// era/mode default tracks
enum{TR_BUGLE,TR_OMAHA,TR_MARCH,TR_SWING,TR_COLD,TR_MODERN,TR_FUTURE,TR_SPACE,TR_THEME,TR_VICTORY,
 TR_DMZ,TR_FROST,TR_DUNE,TR_JUNGLE,TR_ISLAND,TR_SAND,TR_PADDY,TR_OLDC,TR_XENO,TR_BOSS,TR_FORT,TR_MAP,TR_ASCENT,TR_CRUISE,TR_NAVAL,TR_LOITER,TR_KC,TR_REVOL,TR_MEDIEV,TR_TRENCH,TR_DUEL,TR_AA,TR_BRAD,TR_SNIPE,TR_EOD,TR_LRRP,TR_FPS,TR_DEFEAT,TR_CLEAR};
static const char*const TRK_DESC[NTRK]={"WW2 BUGLE CALL - MENUS","WW2 SOMBER - OMAHA CAMPAIGN","WW2 1940S MILITARY MARCH","WW2 BIG-BAND SWING - DOGFIGHT","COLD WAR SPY SYNTH","MODERN DRIVING ACTION","FUTURISTIC SYNTH ARPS","SPACE AMBIENT DRIFT","TITLE THEME","VICTORY FANFARE","SPEC OPS W3 THE DMZ","SPEC OPS W5 ARCTIC CITY","SPEC OPS W2 DESERT","SPEC OPS W1 BORDER JUNGLE","SPEC OPS W6 ISLAND FORTRESS","SPEC OPS W7 DESERT AIRBASE","SPEC OPS W8 VIETNAM / COLD WAR","SPEC OPS W9 WW2 EUROPE","SPEC OPS W10 ALIEN WORLDS","BOSS BATTLE THEME","AIRSHIP / FORTRESS COURSES","SPEC OPS WORLD MAP","X-15 / ROCKET LAUNCH ASCENT","WORLD FLIGHT CRUISING","6V6 NAVAL BATTLE","DRONE OPS - TENSE","KC TUTORIAL - CHEERFUL","AMERICAN REVOLUTION FIFE","MEDIEVAL ESTAMPIE","WW1 TRENCH MARCH","SWORD FIGHT DUEL","ANTI-AIRCRAFT RAID","BRADLEY ARMOR ROCK","SNIPER - SLOW HEARTBEAT TENSION","EOD - TICKING CLOCK","LRRP - JUNGLE PATROL","FPS - FAST METAL","DEFEAT JINGLE","MISSION CLEAR JINGLE"};
static const char*const TRK_NAME[NTRK]={"BUGLE CALL","DOG GREEN","HEDGEROW MARCH","SWING SQUADRON","IRON CURTAIN","TASK FORCE","NEON PROTOCOL","VOID DRIFT","ARSENAL THEME","VICTORY",
 "DMZ PATROL","PERMAFROST","DUNE RAIDERS","JUNGLE RIVER","ISLAND FORTRESS","SAND AIRSTRIP","PADDY FIELDS","OLD CONTINENT","XENO SIGNAL",
 "BOSS BATTLE","FORTRESS","WORLD MAP","ASCENT","CRUISE","BROADSIDE","LOITER","KC'S BOOT CAMP","LIBERTY FIFE","ESTAMPIE","NO MAN'S LAND","STEEL & HONOR","SKYWATCH","IRON CAVALRY","LONG SHOT","RED WIRE","JUNGLE PATROL","CORRIDORS","DEFEAT","MISSION CLEAR"};
// ctx: 0 title, 1 menus, 2 ground battle, 3 air battle, 4 campaign/sombre
static void bgm(int ctx,int era){if(SET.track==NTRK-1+1){music_stop();return;} // last option = OFF
 int t;if(SET.track>0)t=SET.track-1;
 else if(ctx==0)t=TR_THEME;else if(ctx==1)t=TR_BUGLE;else if(ctx==4)t=TR_OMAHA;
 else{static const u8 G[5]={TR_MARCH,TR_COLD,TR_MODERN,TR_FUTURE,TR_SPACE};t=(ctx==3&&era==0)?TR_SWING:G[era];}
 music_play(t);}
static void bgm_t(int t){if(SET.track==NTRK){music_stop();return;}music_play(SET.track>0?SET.track-1:t);}
static void music_jingle(int t){if(SET.musVol&&SET.track!=NTRK){musOn=0;musTrk=-1;music_play(t);}}
static void music_defeat(void){music_jingle(TR_DEFEAT);}
static void music_victory(void){if(SET.musVol){musOn=0;musTrk=-1;music_play(TR_VICTORY);}}
// ---------------------------------------------------------------- frame
static void beat_common(int scr){BEAT->magic=0xA0E5;BEAT->screen=scr;}
static int shimmer; // v1.2 animated water: 1 = arena water bank rotate, 2 = open-sea swap
static void next_frame(void){vsync();frame++;BEAT->frame=frame;poll();
 if(shimmer==1&&(frame&15)==0){u16 t=PAL_BG[18];PAL_BG[18]=PAL_BG[19];PAL_BG[19]=PAL_BG[20];PAL_BG[20]=t;}
 else if(shimmer==2&&(frame%24)==0){u16 t=PAL_BG[1];PAL_BG[1]=PAL_BG[2];PAL_BG[2]=t;}}

// ---------------------------------------------------------------- mode 3 drawing
static u16 fb[240*160] EWRAM;
#define M3 fb
static void m3_present(void){vsync();dma32(fb,(void*)0x06000000,240*160/2);REG_DISPCNT=3|(1<<10);}
static u16 dimc(u16 c){return (c>>1)&0x3DEF;}
static void m3_title(int dim){dma32(dim?title_dim:title_bmp,fb,240*160/2);}
static void m3_px(int x,int y,u16 c){if((unsigned)x<240&&(unsigned)y<160)M3[y*240+x]=c;}
static void m3_box(int x0,int y0,int w,int h,int mode){ // v1.2 adventure-style frame: dark rim, gold double line, rounded gem corners
 if(mode<2)for(int y=y0;y<y0+h&&y<160;y++)for(int x=x0;x<x0+w&&x<240;x++){u16*p=&M3[y*240+x];*p=mode?C_PANEL:dimc(dimc(*p));}
 static const u16 RING[4]={RGB(28,20,36),RGB(236,196,92),RGB(255,236,160),RGB(150,100,40)};
 for(int r=0;r<4;r++){u16 c=RING[r];int a=x0+r,b=y0+r,ww=w-2*r,hh=h-2*r;if(ww<=2||hh<=2)break;
  for(int x=a+2;x<a+ww-2;x++){m3_px(x,b,r==2?RING[1]:c);m3_px(x,b+hh-1,c);}
  for(int y=b+2;y<b+hh-2;y++){m3_px(a,y,r==2?RING[1]:c);m3_px(a+ww-1,y,c);}
  m3_px(a+1,b+1,c);m3_px(a+ww-2,b+1,c);m3_px(a+1,b+hh-2,c);m3_px(a+ww-2,b+hh-2,c);}
 if(y0>0&&x0>0)m3_px(x0-1+1,y0,0);
 static const int CX[4]={0,1,0,1},CY[4]={0,0,1,1};
 for(int k=0;k<4;k++){int cx=CX[k]?x0+w-5:x0+4,cy=CY[k]?y0+h-5:y0+4;m3_px(cx,cy,RGB(120,220,255));m3_px(cx-1,cy,RGB(40,90,200));m3_px(cx+1,cy,RGB(40,90,200));m3_px(cx,cy-1,RGB(200,250,255));m3_px(cx,cy+1,RGB(40,90,200));}}
static int slen(const char*s){int n=0;while(s[n])n++;return n;}
static void m3_text(int x,int y,const char*s,u16 col,int sc){
 for(;*s;s++,x+=6*sc){int c=*s;if(c>='a'&&c<='z')c-=32;if(c<32||c>127)c='?';const u8*g=&font_bits[(c-32)*7];
  for(int pass=0;pass<2;pass++)for(int gy=0;gy<7;gy++)for(int gx=0;gx<5;gx++)if(g[gy]&(1<<gx))
   for(int oy=0;oy<sc;oy++)for(int ox=0;ox<sc;ox++){int X=x+gx*sc+ox+(pass?0:1),Y=y+gy*sc+oy+(pass?0:1);if(X>=0&&X<240&&Y>=0&&Y<160)M3[Y*240+X]=pass?col:0x0421;}}}
static void m3_ctext(int y,const char*s,u16 col,int sc){m3_text(120-(slen(s)*6*sc-sc)/2,y,s,col,sc);}
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
static void hearts(int x,int y,int n,int v,int max){int h=max>0?(v*n*2+max-1)/max:0;if(v<=0)h=0;for(int i=0;i<n;i++){int q=h-i*2;hput(x+i,y,HT_HEART+(q>=2?0:q==1?1:2),15);}}
static void hbar(int x,int y,int cells,int v,int max){int f=max>0?v*cells*8/max:0;if(v>0&&f==0)f=1;for(int i=0;i<cells;i++){int n=f-i*8;n=n<0?0:n>8?8:n;hput(x+i,y,HT_BAR+n,15);}}
static char nb[4][12];static int nbi;
static const char*itoa(int v){char*o=nb[nbi++&3];char t[12];int n=0,neg=v<0;if(neg)v=-v;do{t[n++]='0'+v%10;v/=10;}while(v);int k=0;if(neg)o[k++]='-';while(n)o[k++]=t[--n];o[k]=0;return o;}
static const char*mmss(int frames){char*o=nb[nbi++&3];int s=frames/60;if(s<0)s=0;int m=s/60;s%=60;int k=0;if(m>=10)o[k++]='0'+m/10;o[k++]='0'+m%10;o[k++]=':';o[k++]='0'+s/10;o[k++]='0'+s%10;o[k]=0;return o;}
static char sbuf[64];
static char sbuf3[64];static const char*cat3(const char*a,const char*b,const char*c){int k=0;while(*a&&k<62)sbuf3[k++]=*a++;while(*b&&k<62)sbuf3[k++]=*b++;while(*c&&k<62)sbuf3[k++]=*c++;sbuf3[k]=0;return sbuf3;}
static const char*cat(const char*a,const char*b){int k=0;while(*a&&k<62)sbuf[k++]=*a++;while(*b&&k<62)sbuf[k++]=*b++;sbuf[k]=0;return sbuf;}

// ---------------------------------------------------------------- OAM + affine
static u16 oam[128*4] EWRAM;static int noam,naff;static u16 affkey[32];
static void oam_begin(void){for(int i=0;i<128;i++){oam[i*4]=0x0200;oam[i*4+1]=0;oam[i*4+2]=0;}noam=0;naff=0;}
static void spr(int x,int y,int tile,int shape,int size,int pal,int prio){
 if(noam>=128||x<=-64||x>=240||y<=-64||y>=160)return;u16*o=&oam[noam*4];
 o[0]=(y&255)|(shape<<14);o[1]=(x&511)|(size<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}
static int aff_get(int ang,int sx){ // sx: x-scale 8.8 (roll); 0x100 normal; 0x80 double
 u16 key=(ang&255)|((sx==0x81?63:(sx>>3))<<8);for(int i=0;i<naff;i++)if(affkey[i]==key)return i;
 if(naff>=32){int best=0,bd=999;for(int i=0;i<32;i++){int d=iabs(angd(affkey[i]&255,ang));if((affkey[i]>>8)==(key>>8)&&d<bd){bd=d;best=i;}}return best;}
 int k=naff++;affkey[k]=key;int c=COS(ang),s=SIN(ang);int kx=sx>=0x100?256:(256*256/(sx<16?16:sx));
 if(sx==0x81){oam[(k*4+0)*4+3]=(u16)(s16)(c/2);oam[(k*4+1)*4+3]=(u16)(s16)(s/2);oam[(k*4+2)*4+3]=(u16)(s16)(-s/2);oam[(k*4+3)*4+3]=(u16)(s16)(c/2);return k;}
 if(sx==0x80){c*=2;s*=2;kx=256;}  // 2x enlarge (double-size)
 if(sx==0x80){oam[(k*4+0)*4+3]=(u16)(c/2>0?128:128);oam[(k*4+1)*4+3]=0;oam[(k*4+2)*4+3]=0;oam[(k*4+3)*4+3]=128;return k;}
 oam[(k*4+0)*4+3]=(u16)(s16)(c*kx/256);oam[(k*4+1)*4+3]=(u16)(s16)(s*kx/256);oam[(k*4+2)*4+3]=(u16)(s16)(-s);oam[(k*4+3)*4+3]=(u16)(s16)c;return k;}
// affine sprite centred at (cx,cy); sz 0=8,1=16,2=32
static void spr_aff(int cx,int cy,int tile,int sz,int pal,int ang,int sx,int prio,int semi){
 int w=8<<sz;if(noam>=128||cx<-w||cx>240+w||cy<-w||cy>160+w)return;int m=aff_get(ang,sx);int dbl=sx==0x80||sx==0x81;
 int x=cx-(dbl?w:w/2),y=cy-(dbl?w:w/2);u16*o=&oam[noam*4];
 o[0]=(y&255)|(1<<8)|(dbl<<9)|(semi?1<<10:0);o[1]=(x&511)|(m<<9)|(sz<<14);o[2]=tile|(prio<<10)|(pal<<12);noam++;}

// ---------------------------------------------------------------- tile world (BG1 SB30 cb1, clouds BG2 SB29)
#define MAXW 64
static u8 map[64*64] EWRAM;static u8 cmap[64*64] EWRAM;static u16 bgw[1024] EWRAM,bgc[1024] EWRAM;
static int mapW,mapH,mapWrap,camX,camY,borderTile;static u8 tflag[32];static u8 mtBank[32];
enum{TF_SOLID=1,TF_SHOT=2,TF_SLOW=4};
static inline int cell(int cx,int cy){if(mapWrap){cx&=mapW-1;cy&=mapH-1;}else if(cx<0||cy<0||cx>=mapW||cy>=mapH)return borderTile;return map[cy*MAXW+cx];}
static inline int flag_at(int px,int py){return tflag[cell(px>>4,py>>4)];}
static void draw_layer(const u8*m,u16*buf,int cx0,int cy0,int bank,int wrap){
 int tx0=cx0>>4,ty0=cy0>>4;
 for(int cy=ty0;cy<ty0+11;cy++)for(int cx=tx0;cx<tx0+16;cx++){int t;
  if(wrap)t=m[(cy&(mapH-1))*MAXW+(cx&(mapW-1))];else t=(cx<0||cy<0||cx>=mapW||cy>=mapH)?borderTile:m[cy*MAXW+cx];
  u16 b=(t*4)|((bank+mtBank[t&31])<<12);int X=(cx*2)&31,Y=(cy*2)&31;
  buf[Y*32+X]=b;buf[Y*32+X+1]=b+1;buf[(Y+1)*32+X]=b+2;buf[(Y+1)*32+X+1]=b+3;}}
// map themes: 0 normal, 1 snow, 2 desert, 3 night, 4 dusk
static u16 tint_c(u16 c,int th){if(!th)return c;int r=c&31,g=(c>>5)&31,b=(c>>10)&31;
  if(th==1){int l=(r+g+b)/3;r=(l+r)/2+12;g=(l+g)/2+12;b=(l+b)/2+14;}else if(th==2){r=(r*3+26)/4;g=(g*3+21)/4;b=(b*3+13)/4;}
  else if(th==3){r=r*9/20;g=g*9/20;b=b*9/20+3;}else{r=(r*3+28)/4;g=g*2/3+3;b=b*2/3+4;}
  if(r>31)r=31;if(g>31)g=31;if(b>31)b=31;return r|(g<<5)|(b<<10);}
static void tint_bg(int th,int banks){if(!th)return;for(int i=0;i<16*banks;i++){if((i&15)==0&&i)continue;PAL_BG[i]=tint_c(PAL_BG[i],th);}}
static void present(int clouds,int ccx,int ccy){int kx=0,ky=0;
 if(shakeT>0){shakeT--;if(SET.shake){kx=((frame*7)&3)-1;ky=((frame*5)&3)-1;if(shakeT>4){kx*=2;ky*=2;}
  for(int i=0;i<noam;i++){u16*o=&oam[i*4];if((o[0]&0x300)==0x200)continue;o[0]=(o[0]&0xFF00)|((o[0]-ky)&0xFF);o[1]=(o[1]&0xFE00)|((o[1]-kx)&0x1FF);}}}
 vsync();
 dma32(oam,OAM,256);dma32(hud,SCREENBLOCK(28),512);dma32(bgw,SCREENBLOCK(30),512);
 REG_BGHOFS(1)=camX+kx;REG_BGVOFS(1)=camY+ky;REG_BGHOFS(0)=kx;REG_BGVOFS(0)=ky;
 if(clouds){dma32(bgc,SCREENBLOCK(29),512);REG_BGHOFS(2)=ccx+kx;REG_BGVOFS(2)=ccy+ky;}
 frame++;BEAT->frame=frame;poll();}
#define VS_BASE 384  // v1.2: per-match vehicle sprite slots 384..895 (allocated on demand)
#define CH_TILE 896
// ---------------------------------------------------------------- character creator data (sprite built at runtime)
typedef struct{char name[11];u8 skin,hair,head,hat,uni,camo,vest,body,use;}Chr;
static Chr CH={"SOLDIER",1,0,0,0,0,1,1,0,1};
#define CH_NSKIN 6
#define CH_NHAIR 5
#define CH_NHEAD 3
#define CH_NHAT 5
#define CH_NUNI 8
#define CH_NCAMO 5
#define CH_NVEST 4
static const u16 CH_SKIN[CH_NSKIN]={RGB(246,214,182),RGB(226,182,140),RGB(196,146,100),RGB(160,108,68),RGB(118,78,48),RGB(84,56,36)};
static const u16 CH_HAIRC[CH_NHAIR]={RGB(60,40,24),RGB(24,22,22),RGB(214,180,96),RGB(160,72,32),RGB(150,150,150)};
static const u8 CH_UNIC[CH_NUNI][3]={{60,72,40},{120,100,64},{84,90,90},{40,52,84},{36,36,40},{170,146,100},{200,204,210},{96,100,110}};
static u8 chpx[2][16][16];
static void ch_e(int f,int x0,int y0,int x1,int y1,int c){int cx2=x0+x1,cy2=y0+y1,rx=x1-x0+1,ry=y1-y0+1;for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++){int dx=2*x-cx2,dy=2*y-cy2;if(dx*dx*ry*ry+dy*dy*rx*rx<=rx*rx*ry*ry)chpx[f][y][x]=c;}}
static void ch_r(int f,int x0,int y0,int x1,int y1,int c){for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++)chpx[f][y][x]=c;}
// palette (bank 6): 1 outline 2 uni dark 3 uni mid 4 uni light 5 hat 6 skin 7 black 8 vest 9 player marker 10 hat dark 11 hair 12 skin shade 13 vest dark 14 metal 15 hi
static void ch_build(void){
 for(int f=0;f<2;f++){memset(chpx[f],0,256);int st=f?2:-2;
  ch_e(f,5,9+st/2,7,12+st/2,7);ch_e(f,9,9-st/2,11,12-st/2,7);
  if(CH.vest==3)ch_r(f,5,10,10,13,13);else ch_r(f,5,10,10,12,2);
  if(CH.body)ch_e(f,4,5,11,11,3);else ch_e(f,3,5,12,11,3);
  if(CH.camo)for(int y=5;y<12;y++)for(int x=3;x<13;x++)if(chpx[f][y][x]==3){int h=(x*7+y*13+(CH.camo*5))%(CH.camo==3?3:5);
    if(CH.camo==4){if(((x+y*2)&3)==0)chpx[f][y][x]=1;}else if(h==0)chpx[f][y][x]=2;else if(h==1&&CH.camo!=3)chpx[f][y][x]=4;}
  for(int x=4;x<12;x++)if(chpx[f][10][x])chpx[f][10][x]=2;
  if(CH.vest==1){ch_r(f,5,6,5,10,13);ch_r(f,10,6,10,10,13);ch_r(f,5,8,10,8,13);}
  if(CH.vest==2){ch_r(f,5,6,10,9,8);ch_r(f,6,7,9,7,13);}
  chpx[f][8][CH.body?4:3]=9;chpx[f][8][CH.body?11:12]=9;
  for(int y=0;y<=7;y++)chpx[f][y][9]=7;chpx[f][3][9]=14;chpx[f][4][9]=14;chpx[f][6][9]=6;chpx[f][5][7]=6;
  // head
  int hx0=4,hx1=11;ch_e(f,hx0,2,hx1,11,11);if(CH.head==1)ch_e(f,5,3,10,10,12);
  chpx[f][4][7]=6;chpx[f][4][8]=6;if(CH.head==2){chpx[f][3][7]=11;chpx[f][3][8]=11;chpx[f][4][6]=11;chpx[f][4][9]=11;}
  switch(CH.hat){
   case 0:ch_e(f,3,1,12,11,5);chpx[f][3][5]=15;chpx[f][3][6]=15;chpx[f][4][5]=15;chpx[f][10][7]=10;chpx[f][10][8]=10;break;
   case 1:ch_e(f,4,2,11,10,5);ch_e(f,3,4,7,9,5);chpx[f][4][10]=10;break;
   case 2:ch_e(f,4,2,11,11,5);ch_r(f,6,0,9,2,10);break;
   case 3:ch_e(f,2,1,13,12,10);ch_e(f,4,3,11,10,5);break;}
  // v1.2: 3-tone cel shade + dark outline (adventure style)
  static u8 t[16][16];memcpy(t,chpx[f],256);
  for(int y=0;y<16;y++)for(int x=0;x<16;x++){int v=t[y][x];int up=y==0||!t[y-1][x],lf=x==0||!t[y][x-1],dn=y==15||!t[y+1][x],rt=x==15||!t[y][x+1];
   if(v==3){if(up||lf)chpx[f][y][x]=4;else if(dn||rt)chpx[f][y][x]=2;}
   else if(!v&&((y>0&&t[y-1][x])||(y<15&&t[y+1][x])||(x>0&&t[y][x-1])||(x<15&&t[y][x+1])))chpx[f][y][x]=1;}}}
static void ch_palette(int bank){u16*p=(u16*)PAL_OBJ+bank*16;const u8*u=CH_UNIC[CH.uni];
 p[1]=RGB(30,20,40);p[2]=RGB(u[0]*3/4,u[1]*3/4,u[2]*3/4);p[3]=RGB(u[0],u[1],u[2]);p[4]=RGB(clampi(u[0]*5/4,0,255),clampi(u[1]*5/4,0,255),clampi(u[2]*5/4,0,255));
 static const u8 HAT[5][3]={{70,82,48},{150,30,40},{60,70,44},{100,104,64},{0,0,0}};p[5]=CH.hat==0?RGB(u[0]*9/10,u[1]*9/10+8,u[2]*9/10):RGB(HAT[CH.hat][0],HAT[CH.hat][1],HAT[CH.hat][2]);
 p[10]=RGB(40,40,36);if(CH.hat==3)p[10]=RGB(u[0]*4/5,u[1]*4/5,u[2]*4/5);
 p[6]=CH_SKIN[CH.skin];{u16 s=CH_SKIN[CH.skin];p[12]=((s&31)*3/4)|((((s>>5)&31)*3/4)<<5)|((((s>>10)&31)*3/4)<<10);}
 p[7]=RGB(30,30,32);p[8]=RGB(84,90,70);p[13]=RGB(54,58,46);p[9]=RGB(255,220,40);p[11]=CH_HAIRC[CH.hair];p[14]=RGB(140,142,146);p[15]=RGB(220,220,200);}
static void char_upload(void){ch_build();
 for(int f=0;f<2;f++)for(int t=0;t<4;t++){u32*d=(u32*)OBJ_TILES+(CH_TILE+f*4+t)*8;int ox=(t&1)*8,oy=(t>>1)*8;
  for(int y=0;y<8;y++){u32 w=0;for(int x=0;x<8;x++)w|=(u32)(chpx[f][oy+y][ox+x]&15)<<(x*4);d[y]=w;}}
 ch_palette(6);}
static void setup_mode0(const u32*tiles,int ntilewords,const u16*pal,int era,int clouds){shimmer=0;
 vsync();REG_DISPCNT=0x80; // forced blank while loading
 dma32(hud_tiles,CHARBLOCK(0),sizeof(hud_tiles)/4);
 for(int b=0;b<6;b++)dma32(hud_pal+b*16,PAL_BG+(15-b)*16,8);
 dma32(tiles,CHARBLOCK(1),ntilewords);for(int i=0;i<16;i++)PAL_BG[i]=pal[i];
 dma32(fx_tiles,OBJ_TILES,sizeof(fx_tiles)/4);
 const u32*es=era==0?era_spr_0:era==1?era_spr_1:era==2?era_spr_2:era==3?era_spr_3:era_spr_4;
 dma32(es,OBJ_TILES+64*8,(era==0?ES_N0:era==2?ES_N2:ES_N1)*8);
 char_upload();
 for(int i=0;i<48;i++)PAL_OBJ[i]=obj_team_pal[era*48+i];for(int i=0;i<16;i++)PAL_OBJ[48+i]=fx_pal[i];
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
 s16 cd,cd2,resp,inv,hurt,anim,roll,rollcd,ammo,gren,kills,deaths,think,tgt,stuck,wander,wang,evade,regen,mode,mag[3],rl,stun,tact,veh;s32 lx,ly,gx,gy;u8 role,kind,ve;u16 vt;s16 fuel;}Unit;
#define MAXU 24
static Unit U[MAXU];static int nU=12;
#define NU nU
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
