// v1.3.2 shared helpers: checksummed SRAM blobs (magic 'A','O','E',tag, ver, size, sum)
static void sv_put(int off,char tag,int ver,const void*p,int n){const u8*b=(const u8*)p;u8 sum=0x5A;SRAM[off]='A';SRAM[off+1]='O';SRAM[off+2]='E';SRAM[off+3]=tag;SRAM[off+4]=ver;SRAM[off+5]=n&255;SRAM[off+6]=n>>8;
 for(int i=0;i<n;i++){SRAM[off+8+i]=b[i];sum+=b[i]*(i+3);}SRAM[off+7]=sum;}
static int sv_get(int off,char tag,int ver,void*p,int n){if(SRAM[off]!='A'||SRAM[off+1]!='O'||SRAM[off+2]!='E'||SRAM[off+3]!=tag||SRAM[off+4]!=ver)return 0;if((SRAM[off+5]|(SRAM[off+6]<<8))!=n)return 0;
 u8 sum=0x5A;u8 tmp[512];if(n>512)return 0;for(int i=0;i<n;i++){tmp[i]=SRAM[off+8+i];sum+=tmp[i]*(i+3);}if(sum!=SRAM[off+7])return 0;memcpy(p,tmp,n);return 1;}
#define SCR_HANGAR 94
#define SCR_HBUILD 95
#define SCR_HNAME 96
