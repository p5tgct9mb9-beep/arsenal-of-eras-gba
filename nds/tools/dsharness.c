// Headless DS test harness: hosts a libretro DS core (melonDS), runs an input script, dumps both screens (PPM).
// usage: dsharness core.so rom.nds script.txt outdir [save.sav]
// script (GBA harness compatible): wait N | hold KEYS N | press KEYS | release | tap KEYS | shot NAME | beat TAG | random N SEED | reset | corereset
//   | poke32 GBAADDR VAL | peek GBAADDR N | sram64 [OFF] | sram | savedump FILE | audio NAME N | runright N
//   (GBA addresses 0x0203FFxx = BEAT block, 0x0E00xxxx = SRAM image)  DS extras: touch X Y N | ttap X Y | peekds DSADDR N | audio FILE|stop
// old: audio FILE|stop (raw s16 stereo 32768 Hz) | wait N | hold KEYS N | press KEYS | release | tap KEYS | touch X Y N (bottom screen px) | ttap X Y | shot NAME | beat TAG | peek ADDR N | savedump FILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>
#include "libretro.h"
static void*(*p_get_memory_data)(unsigned);static size_t(*p_get_memory_size)(unsigned);
static unsigned W=256,H=384,pitch;static unsigned char fbuf[1024*1024*4];static int fmt=RETRO_PIXEL_FORMAT_0RGB1555;
static int keys=0,tx=-1,ty=-1;static long total=0;static const char*sysdir="/tmp/dsh_sys";
static void logcb(enum retro_log_level l,const char*f,...){if(getenv("DSH_LOG")){va_list a;va_start(a,f);vfprintf(stderr,f,a);va_end(a);}}
static const char*optval(const char*k){
 if(!strcmp(k,"melonds_console_mode"))return "DS";if(!strcmp(k,"melonds_boot_directly"))return "enabled";
 if(!strcmp(k,"melonds_screen_layout"))return "Top/Bottom";if(!strcmp(k,"melonds_touch_mode"))return "Touch";
 if(!strcmp(k,"melonds_use_fw_settings"))return "disabled";if(!strcmp(k,"melonds_jit_enable"))return "disabled";
 if(!strcmp(k,"melonds_threaded_renderer"))return "disabled";if(!strcmp(k,"melonds_opengl_renderer"))return "disabled";
 if(!strcmp(k,"melonds_screen_gap"))return "0";return NULL;}
static bool env(unsigned cmd,void*d){switch(cmd){
 case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:((struct retro_log_callback*)d)->log=logcb;return true;
 case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:fmt=*(int*)d;return true;
 case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:*(const char**)d=sysdir;return true;
 case RETRO_ENVIRONMENT_GET_VARIABLE:{struct retro_variable*v=d;v->value=optval(v->key);return v->value!=NULL;}
 case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:*(bool*)d=false;return true;
 case RETRO_ENVIRONMENT_GET_CAN_DUPE:*(bool*)d=true;return true;
 case RETRO_ENVIRONMENT_SET_GEOMETRY:return true;
 default:return false;}}
static void video(const void*data,unsigned w,unsigned h,size_t p){if(!data)return;W=w;H=h;pitch=p;for(unsigned y=0;y<h;y++)memcpy(fbuf+y*w*4,(const char*)data+y*p,fmt==RETRO_PIXEL_FORMAT_XRGB8888?w*4:w*2);}
static FILE*aud;static long audN;
static void audio(int16_t l,int16_t r){if(aud){int16_t b[2]={l,r};fwrite(b,4,1,aud);audN++;}}
static size_t audiob(const int16_t*d,size_t f){if(aud){fwrite(d,4,f,aud);audN+=f;}return f;}
static void ipoll(void){}
static const char*names[]={"B","Y","SELECT","START","UP","DOWN","LEFT","RIGHT","A","X","L","R"};
static int16_t istate(unsigned port,unsigned dev,unsigned idx,unsigned id){
 if(port)return 0;
 if(dev==RETRO_DEVICE_JOYPAD){if(id==RETRO_DEVICE_ID_JOYPAD_MASK)return keys;return (keys>>id)&1;}
 if(dev==RETRO_DEVICE_POINTER){if(tx<0)return 0;
  if(id==RETRO_DEVICE_ID_POINTER_PRESSED)return 1;
  if(id==RETRO_DEVICE_ID_POINTER_X)return (int16_t)((tx*2+1)*0x7fff/(int)W-0x7fff);
  if(id==RETRO_DEVICE_ID_POINTER_Y)return (int16_t)(((ty+192)*2+1)*0x7fff/(int)H-0x7fff);}
 return 0;}
static int parsekeys(const char*s){int k=0;char t[128];strncpy(t,s,127);t[127]=0;for(char*p=strtok(t,"+");p;p=strtok(NULL,"+"))for(int i=0;i<12;i++)if(!strcmp(p,names[i]))k|=1<<i;return k;}
static void(*p_run)(void);static void(*p_reset)(void);
// GBA key bit order (as used by the GBA harness 'random' bot) -> libretro joypad ids
static const int GBA2RETRO[10]={8,0,2,3,7,6,4,5,11,10};
static int gba2keys(int g){int k=0;for(int i=0;i<10;i++)if(g&(1<<i))k|=1<<GBA2RETRO[i];return k;}
// DS debug block (platform.c ds_dbg): magic, magic2, &beat, &gba_sram, &gba_io, &gba_bmp, &ds_frame
static unsigned char*ram;static size_t ramsz;static long dbgOff=-1,beatOff=-1,sramOff=-1;
static unsigned rd32(long o){unsigned v;if(o<0||(size_t)o+4>ramsz)return 0;memcpy(&v,ram+o,4);return v;}
static void wr32(long o,unsigned v){if(o<0||(size_t)o+4>ramsz)return;memcpy(ram+o,&v,4);}
static void finddbg(void){if(dbgOff>=0||!ram)return;for(size_t o=0;o+64<ramsz;o+=4)if(rd32(o)==0x5EA7DB60u&&rd32(o+4)==0xDB60DB60u){dbgOff=o;beatOff=(rd32(o+8)&0x3FFFFF)-4;sramOff=rd32(o+12)&0x3FFFFF;break;}}
// map a GBA-side address used by the old scripts onto DS main RAM
static long mapaddr(unsigned a){finddbg();
 if(a>=0x0203FF00u&&a<0x02040000u)return beatOff<0?-1:beatOff+4+(a-0x0203FF00u);
 if(a>=0x0E000000u&&a<0x0E010000u)return sramOff<0?-1:sramOff+(a-0x0E000000u);
 return a&0x3FFFFF;}
static unsigned lastBeat;static int stall,maxstall;
static void step(void){p_run();total++;finddbg();if(beatOff>=0){unsigned f=rd32(beatOff+8);if(f==lastBeat){stall++;if(stall>maxstall)maxstall=stall;}else stall=0;lastBeat=f;}}
static void stepk(int k){int o=keys;keys=k;step();keys=o;}
static void shot(const char*dir,const char*name){char p[512];snprintf(p,512,"%s/%s.ppm",dir,name);FILE*f=fopen(p,"wb");if(!f)return;fprintf(f,"P6 %u %u 255\n",W,H);
 for(unsigned y=0;y<H;y++)for(unsigned x=0;x<W;x++){unsigned char px[3];
  if(fmt==RETRO_PIXEL_FORMAT_XRGB8888){unsigned c=((unsigned*)fbuf)[y*W+x];px[0]=c>>16;px[1]=c>>8;px[2]=c;}
  else{unsigned c=((unsigned short*)fbuf)[y*W+x];if(fmt==RETRO_PIXEL_FORMAT_RGB565){px[0]=(c>>11)<<3;px[1]=((c>>5)&63)<<2;px[2]=(c&31)<<3;}else{px[0]=((c>>10)&31)<<3;px[1]=((c>>5)&31)<<3;px[2]=(c&31)<<3;}}
  fwrite(px,1,3,f);}fclose(f);}
static void beat(const char*tag){finddbg();
 if(beatOff<0||rd32(beatOff+4)!=0xA0E5u){printf("[%s] t=%ld nobeat\n",tag,total);fflush(stdout);return;}long b=beatOff;
 printf("[%s] t=%ld frame=%u scr=%u mode=%u era=%u hp=%d A=%u B=%u timer=%u phase=%u k=%u d=%u x=%u y=%u extra=%u maxstall=%d\n",tag,total,rd32(b+8),rd32(b+12),rd32(b+16),rd32(b+20),(int)rd32(b+24),rd32(b+28),rd32(b+32),rd32(b+36),rd32(b+40),rd32(b+44),rd32(b+48),rd32(b+56),rd32(b+60),rd32(b+64),maxstall);fflush(stdout);}
int main(int argc,char**argv){
 if(argc<5){fprintf(stderr,"usage: dsharness core.so rom.nds script outdir [save]\n");return 1;}
 void*h=dlopen(argv[1],RTLD_NOW);if(!h){fprintf(stderr,"dlopen: %s\n",dlerror());return 1;}
#define S(n) *(void**)(&p_##n)=dlsym(h,"retro_" #n)
 void(*p_set_environment)(retro_environment_t);void(*p_set_video_refresh)(retro_video_refresh_t);void(*p_set_audio_sample)(retro_audio_sample_t);void(*p_set_audio_sample_batch)(retro_audio_sample_batch_t);
 void(*p_set_input_poll)(retro_input_poll_t);void(*p_set_input_state)(retro_input_state_t);void(*p_init)(void);bool(*p_load_game)(const struct retro_game_info*);void(*p_set_controller_port_device)(unsigned,unsigned);
 S(set_environment);S(set_video_refresh);S(set_audio_sample);S(set_audio_sample_batch);S(set_input_poll);S(set_input_state);S(init);S(load_game);S(run);S(get_memory_data);S(get_memory_size);S(set_controller_port_device);S(reset);
 if(getenv("DSH_SYS"))sysdir=getenv("DSH_SYS");
 char cmd[600];snprintf(cmd,600,"mkdir -p %s",sysdir);system(cmd);
 if(argc>5){char p[600];const char*b=strrchr(argv[2],'/');b=b?b+1:argv[2];char nm[256];snprintf(nm,256,"%s",b);char*dot=strrchr(nm,'.');if(dot)*dot=0;
  snprintf(cmd,600,"mkdir -p %s/melonDS && cp %s %s/%s.sav && cp %s %s/melonDS/%s.sav",sysdir,argv[5],sysdir,nm,argv[5],sysdir,nm);system(cmd);}
 else{const char*b=strrchr(argv[2],'/');b=b?b+1:argv[2];char nm[256];snprintf(nm,256,"%s",b);char*dot=strrchr(nm,'.');if(dot)*dot=0;snprintf(cmd,600,"rm -f %s/%s.sav %s/melonDS/%s.sav",sysdir,nm,sysdir,nm);system(cmd);}
 p_set_environment(env);p_init();p_set_video_refresh(video);p_set_audio_sample(audio);p_set_audio_sample_batch(audiob);p_set_input_poll(ipoll);p_set_input_state(istate);
 FILE*rf=fopen(argv[2],"rb");if(!rf){fprintf(stderr,"no rom\n");return 1;}fseek(rf,0,SEEK_END);long rs=ftell(rf);fseek(rf,0,SEEK_SET);void*rb=malloc(rs);fread(rb,1,rs,rf);fclose(rf);
 struct retro_game_info gi={argv[2],rb,(size_t)rs,NULL};if(!p_load_game(&gi)){fprintf(stderr,"load_game failed\n");return 1;}
 if(p_set_controller_port_device)p_set_controller_port_device(0,RETRO_DEVICE_JOYPAD);
 ram=p_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);ramsz=p_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
 FILE*sf=fopen(argv[3],"r");char line[256];
 int held=0;
 while(fgets(line,256,sf)){char c[64]="",a[128]="",b[64]="",d[64]="";int n=sscanf(line,"%63s %127s %63s %63s",c,a,b,d);if(n<1||c[0]=='#')continue;
  if(!strcmp(c,"wait")){int N=atoi(a);for(int i=0;i<N;i++)stepk(held);}
  else if(!strcmp(c,"hold")){int k=parsekeys(a),N=atoi(b);for(int i=0;i<N;i++)stepk(k);}
  else if(!strcmp(c,"press"))held=parsekeys(a);
  else if(!strcmp(c,"release"))held=0;
  else if(!strcmp(c,"tap")){int k=parsekeys(a);for(int i=0;i<4;i++)stepk(k);for(int i=0;i<6;i++)stepk(0);}
  else if(!strcmp(c,"touch")){tx=atoi(a);ty=atoi(b);int N=atoi(d);for(int i=0;i<N;i++)stepk(held);tx=ty=-1;}
  else if(!strcmp(c,"ttap")){tx=atoi(a);ty=atoi(b);for(int i=0;i<5;i++)stepk(held);tx=ty=-1;for(int i=0;i<8;i++)stepk(held);}
  else if(!strcmp(c,"shot")){shot(argv[4],a);beat(a);}
  else if(!strcmp(c,"beat"))beat(a);
  else if(!strcmp(c,"random")){long N=atol(a);srand(atoi(b));int k=0;
   for(long i=0;i<N;i++){if(i%8==0){k=rand()&0x3FF;if(rand()%3)k&=~(4|8);if(rand()%2)k|=16;k&=~(32*((k&16)?1:0));}stepk(gba2keys(k));if(i%20000==0)beat("soak");}
   beat("soak-end");}
  else if(!strcmp(c,"reset")){maxstall=0;stall=0;}
  else if(!strcmp(c,"corereset")){if(p_reset)p_reset();}
  else if(!strcmp(c,"poke32")){long o=mapaddr(strtoul(a,0,16));wr32(o,strtoul(b,0,16));}
  else if(!strcmp(c,"peek")){unsigned ad=strtoul(a,0,16);int N=atoi(b);long o=mapaddr(ad);printf("[peek %08x]",ad);for(int i=0;i<N;i++){unsigned v=rd32(o+2*i);printf(" %04x",v&0xFFFF);}printf("\n");}
  else if(!strcmp(c,"peekds")){long o=strtoul(a,0,16)&0x3FFFFF;int N=atoi(b);printf("[peekds %08lx]",o);for(int i=0;i<N;i++)printf(" %08x",rd32(o+4*i));printf("\n");}
  else if(!strcmp(c,"sram64")){unsigned so=a[0]?strtoul(a,0,16):0;finddbg();printf("[sram64]");for(int i=0;i<64;i++)printf(" %02x",sramOff<0?0:ram[sramOff+so+i]);printf("\n");}
  else if(!strcmp(c,"sram")){finddbg();printf("[sram]");for(int i=0;i<16;i++)printf(" %02x",sramOff<0?0:ram[sramOff+i]);printf("\n");}
  else if(!strcmp(c,"savedump")){finddbg();FILE*o=fopen(a,"wb");if(o&&sramOff>=0)fwrite(ram+sramOff,1,32768,o);if(o)fclose(o);printf("[savedump %s]\n",a);}
  else if(!strcmp(c,"audio")){
   if(b[0]){char p[512];snprintf(p,512,"%s/%s.wav",argv[4],a);FILE*w=fopen(p,"wb");char hd[44]={0};fwrite(hd,1,44,w);FILE*tmp=tmpfile();aud=tmp;audN=0;int N=atoi(b);for(int i=0;i<N;i++)stepk(held);aud=0;
    rewind(tmp);int16_t fr[2];long cnt=0;while(fread(fr,4,1,tmp)==1){fwrite(&fr[0],2,1,w);cnt++;}fclose(tmp);
    unsigned sr=32768,bytes=cnt*2;unsigned char H_[44]="RIFF";
    #define P32(o,v) {H_[o]=(v)&255;H_[o+1]=((v)>>8)&255;H_[o+2]=((v)>>16)&255;H_[o+3]=((v)>>24)&255;}
    P32(4,36+bytes);memcpy(H_+8,"WAVEfmt ",8);P32(16,16);H_[20]=1;H_[21]=0;H_[22]=1;H_[23]=0;P32(24,sr);P32(28,sr*2);H_[32]=2;H_[33]=0;H_[34]=16;H_[35]=0;memcpy(H_+36,"data",4);P32(40,bytes);
    fseek(w,0,SEEK_SET);fwrite(H_,1,44,w);fclose(w);printf("[audio %s] samples=%ld\n",a,cnt);}
   else{if(aud){fclose(aud);aud=0;printf("[audio %ld frames]\n",audN);}if(strcmp(a,"stop")){aud=fopen(a,"wb");audN=0;}}}
  else if(!strcmp(c,"runright")){int N=atoi(a);for(int i=0;i<N;i++){int k=16;if((i%40)<14)k|=1;if(i%10==0)k|=2;stepk(gba2keys(k));if(rd32(mapaddr(0x0203FF1C)))break;}}
 }
 printf("frames=%ld maxstall=%d\n",total,maxstall);fflush(stdout);
 {void(*p_unload)(void)=(void(*)(void))dlsym(h,"retro_unload_game");void(*p_deinit)(void)=(void(*)(void))dlsym(h,"retro_deinit");if(p_unload)p_unload();if(p_deinit)p_deinit();}
 if(getenv("DSH_SAVEOUT")){const char*b=strrchr(argv[2],'/');b=b?b+1:argv[2];char nm[256];snprintf(nm,256,"%s",b);char*dot=strrchr(nm,'.');if(dot)*dot=0;
  snprintf(cmd,600,"f=$(ls -t %s/%s.sav %s/melonDS/%s.sav %s/*/%s.sav 2>/dev/null|head -1); [ -n \"$f\" ] && cp \"$f\" %s && echo \"[saveout $f]\"",sysdir,nm,sysdir,nm,sysdir,nm,getenv("DSH_SAVEOUT"));system(cmd);}
 return 0;}
