// v1.3.2 shared helpers: checksummed SRAM blobs (magic 'A','O','E',tag, ver, size, sum)
static void sv_put(int off,char tag,int ver,const void*p,int n){const u8*b=(const u8*)p;u8 sum=0x5A;SRAM[off]='A';SRAM[off+1]='O';SRAM[off+2]='E';SRAM[off+3]=tag;SRAM[off+4]=ver;SRAM[off+5]=n&255;SRAM[off+6]=n>>8;
 for(int i=0;i<n;i++){SRAM[off+8+i]=b[i];sum+=b[i]*(i+3);}SRAM[off+7]=sum;}
static int sv_get(int off,char tag,int ver,void*p,int n){if(SRAM[off]!='A'||SRAM[off+1]!='O'||SRAM[off+2]!='E'||SRAM[off+3]!=tag||SRAM[off+4]!=ver)return 0;if((SRAM[off+5]|(SRAM[off+6]<<8))!=n)return 0;
 u8 sum=0x5A;u8 tmp[512];if(n>512)return 0;for(int i=0;i<n;i++){tmp[i]=SRAM[off+8+i];sum+=tmp[i]*(i+3);}if(sum!=SRAM[off+7])return 0;memcpy(p,tmp,n);return 1;}
#define SCR_HANGAR 94
#define SCR_HBUILD 95
#define SCR_HNAME 96
// ---- local high-score tables: 5 entries, 3-letter initials
typedef struct{char n[4];u32 s;}HiE;
static void hi_seed(HiE*t,const char*const*nm,const u32*sc){for(int i=0;i<5;i++){for(int k=0;k<3;k++)t[i].n[k]=nm[i][k];t[i].n[3]=0;t[i].s=sc[i];}}
static int hi_rank(const HiE*t,u32 s){for(int i=0;i<5;i++)if(s>t[i].s)return i;return -1;}
static void hi_put(HiE*t,int r,u32 s,const char*nm){for(int i=4;i>r;i--)t[i]=t[i-1];for(int k=0;k<3;k++)t[r].n[k]=nm[k];t[r].n[3]=0;t[r].s=s;}
#define SCR_INIT 101
static void hi_initials(char*nm,const char*title){static const char ICH[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";int nc=sizeof ICH-1;int cur=0,k[3]={11,19,0};for(int i=0;i<3;i++){for(int j=0;j<nc;j++)if(ICH[j]==nm[i])k[i]=j;}
 for(int t=0;;t++){BEAT->screen=SCR_INIT;BEAT->menu=cur;fr_bgspace();fr_panel(30,40,180,80);m3_ctext(48,title,C_YEL,1);m3_ctext(62,"ENTER YOUR INITIALS",C_WHITE,1);
  for(int i=0;i<3;i++){char s[2]={ICH[k[i]],0};m3_fill(90+i*22,78,18,20,i==cur?RGB(40,80,140):RGB(24,28,44));m3_text(94+i*22,82,s,C_WHITE,2);}
  m3_ctext(106,"UP/DOWN  LEFT/RIGHT  A: OK",C_GRAY,1);m3_present();next_frame();
  if(FR_AUTO&&t>40){nm[0]='K';nm[1]='C';nm[2]='B';nm[3]=0;return;}
  if(pressed&K_UP)k[cur]=(k[cur]+1)%nc;if(pressed&K_DOWN)k[cur]=(k[cur]+nc-1)%nc;if(pressed&K_LEFT&&cur>0)cur--;if(pressed&K_RIGHT&&cur<2)cur++;
  if(pressed&(K_A|K_START)){sfx_ok();if(cur<2){cur++;}else{for(int i=0;i<3;i++)nm[i]=ICH[k[i]];nm[3]=0;pressed=0;return;}}}}
// shareable record code: short checksum of mode, score and initials (letters+digits, no I/O/0/1)
static const char*hi_code(int mode,u32 s,const char*nm){static char o[10];static const char A[]="23456789ABCDEFGHJKLMNPQRSTUVWXYZ";u32 h=0x9E3779B9u^(mode*0x45D9F3Bu);h^=s*0x27D4EB2Du;for(int i=0;i<3;i++)h=(h^(u8)nm[i])*0x01000193u;h^=h>>15;
 for(int i=0;i<8;i++){o[i<4?i:i+1]=A[(h>>(i*4))&31];h=h*1103515245u+12345u;}o[4]='-';o[9]=0;return o;}
