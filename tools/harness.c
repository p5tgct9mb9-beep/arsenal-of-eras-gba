// Headless mGBA test harness: runs an input script, dumps screenshots (PPM), soak tests.
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba-util/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void nolog(struct mLogger*l,int c,enum mLogLevel lv,const char*f,va_list a){(void)l;(void)c;(void)lv;(void)f;(void)a;}
static struct mLogger logger={.log=nolog};
static struct mCore*core;static color_t*buf;static unsigned W,H;static long total=0;static unsigned lastBeat=0;static int stall=0,maxstall=0;
static const char*names[]={"A","B","SELECT","START","RIGHT","LEFT","UP","DOWN","R","L"};
static int parsekeys(const char*s){int k=0;char t[128];strncpy(t,s,127);t[127]=0;for(char*p=strtok(t,"+");p;p=strtok(NULL,"+")){for(int i=0;i<10;i++)if(!strcmp(p,names[i]))k|=1<<i;}return k;}
static unsigned rd(unsigned a){return core->busRead32(core,a);}
static void step(int keys){core->setKeys(core,keys);core->runFrame(core);total++;unsigned f=rd(0x0203FF04);if(f==lastBeat){stall++;if(stall>maxstall)maxstall=stall;}else stall=0;lastBeat=f;}
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
 core->loadSave(core,VFileMemChunk(NULL,0));
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
  else if(!strcmp(c,"peek")){unsigned ad=strtoul(a,0,16);int n=atoi(b);printf("[peek %08x]",ad);for(int i=0;i<n;i++)printf(" %04x",core->busRead16(core,ad+2*i));printf("\n");}
  else if(!strcmp(c,"corereset")){core->reset(core);}
  else if(!strcmp(c,"sram")){printf("[sram]");for(int i=0;i<16;i++)printf(" %02x",core->busRead8(core,0x0E000000+i));printf("\n");}
  else if(!strcmp(c,"runright")){int N=atoi(a);int k=0;
   // simple bot: run right, jump periodically, shoot
   for(int i=0;i<N;i++){k=16;if((i%40)<14)k|=1;if(i%10==0)k|=2;step(k);if(rd(0x0203FF1C))break;}}
 }
 printf("frames=%ld maxstall=%d\n",total,maxstall);
 core->deinit(core);return 0;}
