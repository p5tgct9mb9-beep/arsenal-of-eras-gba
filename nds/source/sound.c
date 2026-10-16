// sound.c - GBA PSG -> DS sound translation.
// The game writes the GBA sound registers (shadowed in gba_io). Once per frame we look for
// restart triggers (bit 15 of the FREQ registers, cleared after we consume it), run the GBA
// envelope/length/sweep units in software and drive DS channels:
//   GBA ch1/ch2 square -> DS PSG channels 8/9, ch3 wave -> DS channel 0 (looping 8-bit PCM),
//   ch4 noise -> DS noise channel 14.
#include "platform.h"
#define IO(o) gba_io[(o)>>1]
typedef struct{int on,vol,edir,estep,eacc,len,lenon,duty,freq,raw,id,sentv,sentf,sentp,sacc;}Ch;
static Ch C[4];
static s8 wavebuf[2][64] __attribute__((aligned(32)));static int wb;
u32 ds_snd_trig[4];
static int dsvol(int c,int v,int*pan){
 u16 l=IO(0x80),h=IO(0x82);int en=(l>>(8+c))&1,el=(l>>(12+c))&1;
 int mv=(((l&7)+((l>>4)&7))>>1)+1; // 1..8
 static const int PR[4]={25,50,100,100};int pr=PR[h&3];
 if(!(IO(0x84)&0x80)||(!en&&!el))return 0;
 *pan=en&&el?64:(el?16:112);
 int r=v*90/15*mv/8*pr/100; /* ~0.7x: the DS mix is louder than the GBA at equal register values */return r>127?127:r;}
static void trig(int c){Ch*h=&C[c];ds_snd_trig[c]++;
 u16 cnt=IO(c==0?0x62:c==1?0x68:0x78),fr=IO(c==0?0x64:c==1?0x6C:0x7C);
 h->on=1;h->vol=cnt>>12;h->edir=(cnt>>11)&1;h->estep=(cnt>>8)&7;h->eacc=0;
 h->len=64-(cnt&63);h->lenon=(fr>>14)&1;h->duty=(cnt>>6)&3;h->sacc=0;
 if(c<2){h->freq=fr&0x7FF;h->raw=h->freq;}else h->freq=fr&0xFF;
 if(h->id>=0&&c!=2){soundKill(h->id);h->id=-1;}
 h->sentv=-1;h->sentf=-1;}
static void trig3(void){Ch*h=&C[2];ds_snd_trig[2]++;
 u16 sel=IO(0x70),cnt=IO(0x72),fr=IO(0x74);h->on=(sel&0x80)!=0;h->len=256-(cnt&255);h->lenon=(fr>>14)&1;h->freq=fr&0x7FF;
 const u8*w=(const u8*)&gba_io[0x90>>1];s8*b=wavebuf[wb];
 for(int i=0;i<16;i++){b[i*2]=(s8)(((w[i]>>4)&15)*17-128);b[i*2+1]=(s8)((w[i]&15)*17-128);}
 DC_FlushRange(b,64);
 if(h->id>=0){soundKill(h->id);h->id=-1;}
 h->sentv=-1;h->sentf=-1;}
static int psg_hz(int f){return 131072/(2048-f);}            // GBA square tone frequency
static void update_ch(int c){Ch*h=&C[c];int pan=64;
 if(!h->on){if(h->id>=0){soundKill(h->id);h->id=-1;}return;}
 // envelope at 64 Hz, length at 256 Hz, from a 60 Hz tick
 if(c!=2&&h->estep){h->eacc+=64;while(h->eacc>=60*h->estep){h->eacc-=60*h->estep;if(h->edir){if(h->vol<15)h->vol++;}else if(h->vol>0)h->vol--;}}
 if(h->lenon){h->len-=256/60+((ds_frame%15)==0);if(h->len<=0){h->on=0;if(h->id>=0){soundKill(h->id);h->id=-1;}return;}}
 if(c==0){u16 sw=IO(0x60);int st=(sw>>4)&7,sh=sw&7;if(st&&sh){h->sacc+=128;while(h->sacc>=60*st){h->sacc-=60*st;int d=h->freq>>sh;h->freq+=(sw&8)?-d:d;if(h->freq>2047){h->on=0;return;}if(h->freq<0)h->freq=0;}}}
 int v,f;
 if(c==2){u16 cnt=IO(0x72);int vs=(cnt>>13)&3;static const int VQ[4]={0,15,8,4};v=(cnt&0x8000)?11:VQ[vs];if(!(IO(0x70)&0x80))v=0;
  f=IO(0x74)&0x7FF;f=2097152/(2048-f);}
 else if(c==3){int r=h->freq&7,s=(h->freq>>4)&15;f=r?524288/r:1048576;f>>=s+1;v=h->vol;}
 else{int raw=IO(c==0?0x64:0x6C)&0x7FF;if(raw!=h->raw){h->raw=raw;h->freq=raw;}f=psg_hz(h->freq)*8;v=h->vol;}
 int dv=dsvol(c,v,&pan);
 if(f>65535)f=65535;if(f<1)f=1;
 if(h->id<0){if(dv==0)return;
  if(c==2)h->id=soundPlaySampleChannel(0,wavebuf[wb],SoundFormat_8Bit,32,f*1,dv,pan,true,0),wb^=1;
  else if(c==3)h->id=soundPlayNoiseChannel(14,f,dv,pan);
  else{static const DutyCycle D[4]={DutyCycle_12,DutyCycle_25,DutyCycle_50,DutyCycle_75};h->id=soundPlayPSGChannel(8+c,D[h->duty],f,dv,pan);}
  h->sentv=dv;h->sentf=f;h->sentp=pan;return;}
 if(dv!=h->sentv){soundSetVolume(h->id,dv);h->sentv=dv;}
 if(f!=h->sentf){soundSetFreq(h->id,f);h->sentf=f;}
 if(pan!=h->sentp){soundSetPan(h->id,pan);h->sentp=pan;}}
void snd_update(void){
 static int init;if(!init){init=1;for(int i=0;i<4;i++)C[i].id=-1;}
 int ime=REG_IME;REG_IME=0;
 int t0=IO(0x64)&0x8000,t1=IO(0x6C)&0x8000,t2=IO(0x74)&0x8000,t3=IO(0x7C)&0x8000;
 if(t0){trig(0);IO(0x64)&=0x7FFF;}if(t1){trig(1);IO(0x6C)&=0x7FFF;}if(t2){trig3();IO(0x74)&=0x7FFF;}if(t3){trig(3);IO(0x7C)&=0x7FFF;}
 if(!(IO(0x70)&0x80))C[2].on=0;
 REG_IME=ime;
 for(int c=0;c<4;c++)update_ch(c);}
