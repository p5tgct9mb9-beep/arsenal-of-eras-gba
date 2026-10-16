// Headless mGBA test harness: runs an input script, dumps screenshots (PPM), soak tests.
#include <mgba/core/core.h>
#include <mgba/internal/arm/arm.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba-util/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mgba/core/blip_buf.h>
static void nolog(struct mLogger*l,int c,enum mLogLevel lv,const char*f,va_list a){(void)l;(void)c;(void)lv;(void)f;(void)a;}
static struct mLogger logger={.log=nolog};
static struct mCore*core;static color_t*buf;static unsigned W,H;static long total=0;static unsigned lastBeat=0;static int stall=0,maxstall=0;
static FILE*wav=0;static long wavN=0;static short abuf[4096];
static void audio_drain(void){for(int c=0;c<2;c++){struct blip_t*b=core->getAudioChannel(core,c);int n;while((n=blip_samples_avail(b))>0){if(n>4096)n=4096;blip_read_samples(b,abuf,n,0);if(c==0&&wav){fwrite(abuf,2,n,wav);wavN+=n;}}}}
static const char*names[]={"A","B","SELECT","START","RIGHT","LEFT","UP","DOWN","R","L"};
static int parsekeys(const char*s){int k=0;char t[128];strncpy(t,s,127);t[127]=0;for(char*p=strtok(t,"+");p;p=strtok(NULL,"+")){for(int i=0;i<10;i++)if(!strcmp(p,names[i]))k|=1<<i;}return k;}
static unsigned rd(unsigned a){return core->busRead32(core,a);}
static void step(int keys){core->setKeys(core,keys);core->runFrame(core);audio_drain();total++;unsigned f=rd(0x0203FF04);if(f==lastBeat){stall++;if(stall>maxstall)maxstall=stall;}else stall=0;lastBeat=f;}
static void shot(const char*dir,const char*name){char p[512];snprintf(p,512,"%s/%s.ppm",dir,name);FILE*f=fopen(p,"wb");fprintf(f,"P6 %u %u 255\n",W,H);
 for(unsigned i=0;i<W*H;i++){unsigned c=buf[i];unsigned char px[3]={c&255,(c>>8)&255,(c>>16)&255};fwrite(px,1,3,f);}fclose(f);}
static void beat(const char*tag){printf("[%s] t=%ld frame=%u scr=%u mode=%u era=%u hp=%d A=%u B=%u timer=%u phase=%u k=%u d=%u x=%u y=%u extra=%u maxstall=%d\n",tag,total,rd(0x0203FF04),rd(0x0203FF08),rd(0x0203FF0C),rd(0x0203FF10),(int)rd(0x0203FF14),rd(0x0203FF18),rd(0x0203FF1C),rd(0x0203FF20),rd(0x0203FF24),rd(0x0203FF28),rd(0x0203FF2C),rd(0x0203FF34),rd(0x0203FF38),rd(0x0203FF3C),maxstall);fflush(stdout);}
int main(int argc,char**argv){
 if(argc<4){fprintf(stderr,"usage: harness rom script shotdir\n");return 1;}
 mLogSetDefaultLogger(&logger);
 core=mCoreFind(argv[1]);if(!core){fprintf(stderr,"no core\n");return 1;}
 core->init(core);mCoreInitConfig(core,NULL);
 core->desiredVideoDimensions(core,&W,&H);buf=malloc(W*H*sizeof(color_t));core->setVideoBuffer(core,buf,W);
 if(!mCoreLoadFile(core,argv[1])){fprintf(stderr,"load fail\n");return 1;}
 if(argc>4){FILE*f=fopen(argv[4],"rb");static unsigned char sv[65536];size_t n=f?fread(sv,1,sizeof sv,f):0;if(f)fclose(f);core->loadSave(core,VFileMemChunk(sv,n));}
 else core->loadSave(core,VFileMemChunk(NULL,0));
 core->setAudioBufferSize(core,4096);for(int c=0;c<2;c++)blip_set_rates(core->getAudioChannel(core,c),core->frequency(core),32768);
 core->reset(core);
 FILE*sf=fopen(argv[2],"r");char line[256];int held=0;
 while(fgets(line,256,sf)){char c[64]="",a[128]="",b[64]="";int n=sscanf(line,"%63s %127s %63s",c,a,b);if(n<1||c[0]=='#')continue;
  if(!strcmp(c,"wait")){int N=atoi(a);for(int i=0;i<N;i++)step(held);}
  else if(!strcmp(c,"hold")){int k=parsekeys(a),N=atoi(b);for(int i=0;i<N;i++)step(k);}
  else if(!strcmp(c,"press")){held=parsekeys(a);}
  else if(!strcmp(c,"release")){held=0;}
  else if(!strcmp(c,"tap")){int k=parsekeys(a);for(int i=0;i<4;i++)step(k);for(int i=0;i<6;i++)step(0);}
  else if(!strcmp(c,"shot")){shot(argv[3],a);beat(a);}
  else if(!strcmp(c,"beat")){beat(a);}
  else if(!strcmp(c,"random")){long N=atol(a);srand(atoi(b));int k=0;
   for(long i=0;i<N;i++){if(i%8==0){k=rand()&0x3FF;if(rand()%3)k&=~(4|8);if(rand()%2)k|=16;k&=~(32*((k&16)?1:0));}step(k);
    if(i%20000==0)beat("soak");}
   beat("soak-end");}
  else if(!strcmp(c,"reset")){maxstall=0;stall=0;}
  else if(!strcmp(c,"pc")){struct ARMCore*cpu=core->cpu;printf("[pc] %08x lr %08x sp %08x r0 %08x r1 %08x r2 %08x r6 %08x\n",cpu->gprs[15],cpu->gprs[14],cpu->gprs[13],cpu->gprs[0],cpu->gprs[1],cpu->gprs[2],cpu->gprs[6]);}
  else if(!strcmp(c,"peek")){unsigned ad=strtoul(a,0,16);int n=atoi(b);printf("[peek %08x]",ad);for(int i=0;i<n;i++)printf(" %04x",core->busRead16(core,ad+2*i));printf("\n");}
  else if(!strcmp(c,"poke32")){unsigned ad=strtoul(a,0,16);unsigned v=strtoul(b,0,16);core->busWrite16(core,ad,v&0xFFFF);core->busWrite16(core,ad+2,v>>16);}
  else if(!strcmp(c,"corereset")){core->reset(core);}
  else if(!strcmp(c,"audio")){char p[512];snprintf(p,512,"%s/%s.wav",argv[3],a);wav=fopen(p,"wb");char hd[44]={0};fwrite(hd,1,44,wav);wavN=0;int N=atoi(b);for(int i=0;i<N;i++)step(held);
   unsigned sr=32768,bytes=wavN*2;unsigned char H_[44]="RIFF";fseek(wav,0,SEEK_SET);
   #define P32(o,v) {H_[o]=(v)&255;H_[o+1]=((v)>>8)&255;H_[o+2]=((v)>>16)&255;H_[o+3]=((v)>>24)&255;}
   P32(4,36+bytes);memcpy(H_+8,"WAVEfmt ",8);P32(16,16);H_[20]=1;H_[21]=0;H_[22]=1;H_[23]=0;P32(24,sr);P32(28,sr*2);H_[32]=2;H_[33]=0;H_[34]=16;H_[35]=0;memcpy(H_+36,"data",4);P32(40,bytes);
   fwrite(H_,1,44,wav);fclose(wav);wav=0;printf("[audio %s] samples=%ld\n",a,wavN);}
  else if(!strcmp(c,"sram64")){printf("[sram64]");for(int i=0;i<64;i++)printf(" %02x",core->busRead8(core,0x0E000000+i));printf("\n");}
  else if(!strcmp(c,"savedump")){FILE*o=fopen(a,"wb");for(int i=0;i<32768;i++)fputc(core->busRead8(core,0x0E000000+i),o);fclose(o);printf("[savedump %s]\n",a);}
  else if(!strcmp(c,"sram")){printf("[sram]");for(int i=0;i<16;i++)printf(" %02x",core->busRead8(core,0x0E000000+i));printf("\n");}
  else if(!strcmp(c,"runright")){int N=atoi(a);int k=0;
   // simple bot: run right, jump periodically, shoot
   for(int i=0;i<N;i++){k=16;if((i%40)<14)k|=1;if(i%10==0)k|=2;step(k);if(rd(0x0203FF1C))break;}}
 }
 printf("frames=%ld maxstall=%d\n",total,maxstall);
 core->deinit(core);return 0;}
