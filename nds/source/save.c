// save.c - persistence of the GBA SRAM image on the DS.
// The game keeps using its GBA SRAM layout (checksummed blocks from 0x2200 etc.) in gba_sram[].
// Storage: the DS cartridge save chip (EEPROM/FLASH via libnds). melonDS (and Delta) give an
// unknown retail-style cart an 8 KB EEPROM, so the 64 KB image is RLE-compressed into one of two
// 4 KB slots (ping-pong, sequence number + checksum, header written last = power-safe).
// Writes are spread over frames (one page per frame). Other save types are autodetected: EEPROM 8-128 KB
// (slot = half the chip, up to 32 KB), FLASH 256 KB+ (one 64 KB sector per slot, erased before writing),
// and with no usable chip (none / 512-byte EEPROM / unknown) a file on the SD card via DLDI:
// fat:/ArsenalOfEras.dssav (written whole, through a temp file).
// GBA save import: (1) a GBA "Arsenal of Eras" cartridge/ROM+save in Slot-2 (DS/DS Lite, or
// melonDS/Delta "GBA slot") is read at boot when the DS save is empty or SELECT is held;
// (2) on flashcarts with DLDI, fat:/ArsenalOfEras.sav (the GBA .sav) is imported the same way.
#include "platform.h"
#include <fat.h>
#include <stdio.h>
#include <string.h>
#define SLOT 0x10000   /* largest slot (FLASH sector) */
static int slotSz=0x1000,pageSz=32;
#define HDR 16
#define FATSAV "fat:/ArsenalOfEras.dssav"
static u8 saved[0x10000];static u8 sbuf[SLOT] __attribute__((aligned(4))); // shared scratch for chip/SD reads
static u8 job[SLOT+16] __attribute__((aligned(4)));static int jobLen,jobPos=-1,jobSlot,jobAddr;
int ds_save_dbg_type,ds_save_dbg_size;u32 ds_save_verified;
static u32 seq;static int curSlot=-1;static int eeType=-1;static u32 eeSize;
int ds_save_kind;const char* ds_save_status="NO SAVE CHIP";
u32 ds_save_writes,ds_save_imported,ds_save_complen;
static u32 sum32(const u8*p,int n){u32 s=0x41454F44;for(int i=0;i<n;i++)s=(s<<5|s>>27)^p[i];return s;}
static int rle_pack(const u8*in,int n,u8*out,int cap){int o=0,i=0;
 while(i<n){int r=1;while(i+r<n&&r<130&&in[i+r]==in[i])r++;
  if(r>=3){if(o+2>cap)return -1;out[o++]=0x80|(r-3);out[o++]=in[i];i+=r;continue;}
  int s=i,l=0;while(i<n&&l<128){int rr=1;while(i+rr<n&&rr<3&&in[i+rr]==in[i])rr++;if(rr>=3)break;i++;l++;}
  if(o+1+l>cap)return -1;out[o++]=l-1;memcpy(out+o,in+s,l);o+=l;}
 return o;}
static int rle_unpack(const u8*in,int n,u8*out,int cap){int o=0,i=0;
 while(i<n){u8 c=in[i++];if(c&0x80){int r=(c&0x7F)+3;if(o+r>cap||i>=n)return -1;memset(out+o,in[i++],r);o+=r;}
  else{int l=c+1;if(o+l>cap||i+l>n)return -1;memcpy(out+o,in+i,l);o+=l;i+=l;}}
 return o;}
static int read_slot(int s,u32*sq){u8 h[HDR];cardReadEeprom(s*slotSz,h,HDR,eeType);
 if(memcmp(h,"AOD1",4))return 0;u32 q,len,cs;memcpy(&q,h+4,4);memcpy(&len,h+8,4);memcpy(&cs,h+12,4);
 if(len==0||len>(u32)(slotSz-HDR))return 0;u8*b=sbuf;cardReadEeprom(s*slotSz+HDR,b,len,eeType);
 if(sum32(b,len)!=cs)return 0;*sq=q;return (int)len;}
static int load_slot(int s){u32 q;int len=read_slot(s,&q);if(!len)return 0;u8*b=sbuf;cardReadEeprom(s*slotSz+HDR,b,len,eeType);
 memset(gba_sram,0xFF,sizeof gba_sram);return rle_unpack(b,len,gba_sram,sizeof gba_sram)>0;}
static int slot2_import(void){
 sysSetCartOwner(BUS_OWNER_ARM9);
 const volatile u8*hdr=(const volatile u8*)0x080000AC;
 if(!(hdr[0]=='A'&&hdr[1]=='O'&&hdr[2]=='E'&&hdr[3]=='G'))return 0;
 const volatile u8*sr=(const volatile u8*)0x0A000000;int any=0;
 for(int i=0;i<0x8000;i++){u8 v=sr[i];gba_sram[i]=v;if(v!=0xFF&&v!=0)any=1;}
 return any;}
static int fatOk=-1;static int fat_up(void){if(fatOk<0)fatOk=fatInitDefault()?1:0;return fatOk;}
static int fat_import(void){
 if(!fat_up())return 0;
 FILE*f=fopen("fat:/ArsenalOfEras.sav","rb");if(!f)f=fopen("fat:/saves/ArsenalOfEras.sav","rb");if(!f)return 0;
 int n=fread(gba_sram,1,sizeof gba_sram,f);fclose(f);return n>=0x4000;}
void save_init(void){
 memset(gba_sram,0xFF,sizeof gba_sram);
 eeType=cardEepromGetType();eeSize=cardEepromGetSize();ds_save_dbg_type=eeType;ds_save_dbg_size=eeSize;
 int have=0;
 if(eeType==2&&eeSize>=0x2000){slotSz=eeSize/2;if(slotSz>0x8000)slotSz=0x8000;pageSz=32;}
 else if(eeType==3&&eeSize>=0x20000){slotSz=0x10000;pageSz=256;}
 else eeType=-eeType-10; // unusable here (none, 512-byte EEPROM, unknown): fall back to the SD card
 if(eeType>0){ds_save_kind=1;
  u32 qa=0,qb=0;int la=read_slot(0,&qa),lb=read_slot(1,&qb);
  if(la&&(!lb||qa>=qb)){if(load_slot(0)){curSlot=0;seq=qa;have=1;}}
  else if(lb){if(load_slot(1)){curSlot=1;seq=qb;have=1;}}
  ds_save_status=have?"DS SAVE LOADED":"NEW DS SAVE";}
 else if(fat_up()){ds_save_kind=2;FILE*f=fopen(FATSAV,"rb");
  if(f){u8*b=sbuf;int n=fread(b,1,sizeof b,f);fclose(f);u32 q,len,cs;
   if(n>HDR&&!memcmp(b,"AOD1",4)){memcpy(&q,b+4,4);memcpy(&len,b+8,4);memcpy(&cs,b+12,4);
    if(len<=(u32)(n-HDR)&&sum32(b+HDR,len)==cs&&rle_unpack(b+HDR,len,gba_sram,sizeof gba_sram)>0){seq=q;have=1;}
    else memset(gba_sram,0xFF,sizeof gba_sram);}}
  ds_save_status=have?"SD SAVE LOADED":"NEW SD SAVE";}
 scanKeys();int force=keysHeld()&KEY_SELECT;
 if(!have||force){static u8 keep[0x10000];if(have)memcpy(keep,gba_sram,sizeof keep);
  if(slot2_import()){ds_save_imported=1;ds_save_status="IMPORTED GBA SAVE (SLOT-2)";}
  else if(fat_import()){ds_save_imported=2;ds_save_status="IMPORTED GBA SAVE (SD)";}
  else if(have)memcpy(gba_sram,keep,sizeof keep);}
 // an imported save is written out on the first save_tick; otherwise remember what is on the chip
 if(ds_save_imported)memset(saved,0x5A,sizeof saved);else memcpy(saved,gba_sram,sizeof saved);
 if(!ds_save_kind&&!ds_save_imported)ds_save_status="NO SAVE CHIP - PROGRESS NOT KEPT";}
static void fat_write(void){
 FILE*f=fopen("fat:/ArsenalOfEras.tmp","wb");if(!f){ds_save_status="SD SAVE FAILED";return;}
 int ok=fwrite(job,1,jobLen+HDR,f)==(size_t)(jobLen+HDR);ok&=fclose(f)==0;
 if(ok){remove(FATSAV);ok=rename("fat:/ArsenalOfEras.tmp",FATSAV)==0;}
 if(ok){ds_save_writes++;ds_save_verified++;ds_save_status="SAVED TO SD CARD";}else{ds_save_status="SD SAVE FAILED";memset(saved,0x5A,sizeof saved);}}
void save_tick(void){
 if(ds_save_kind!=1&&ds_save_kind!=2)return;
 if(jobPos>=0&&ds_save_kind==1){ // write one page per frame; data first, header last
  int end=jobLen+HDR;int p=jobPos;
  if(p<end-HDR){int n=end-HDR-p;if(n>pageSz)n=pageSz;cardWriteEeprom(jobAddr+HDR+p,job+HDR+p,n,eeType);jobPos+=n;}
  else{cardWriteEeprom(jobAddr,job,HDR,eeType);curSlot=jobSlot;jobPos=-1;ds_save_writes++;
   // read the slot back from the chip and check it decodes to exactly what we meant to save
   u32 q=0;static u8 chk[0x10000];int ok=read_slot(jobSlot,&q)==jobLen&&q==seq;
   if(ok){u8*b=sbuf;cardReadEeprom(jobAddr+HDR,b,jobLen,eeType);ok=rle_unpack(b,jobLen,chk,sizeof chk)==(int)sizeof chk&&!memcmp(chk,saved,sizeof chk);}
   if(ok){ds_save_verified++;ds_save_status="SAVED TO DS SAVE CHIP";}else{ds_save_status="SAVE VERIFY FAILED";memset(saved,0x5A,sizeof saved);}}
  return;}
 if(ds_frame%30)return;
 if(!memcmp(saved,gba_sram,sizeof saved))return;
 memcpy(saved,gba_sram,sizeof saved);
 int n=rle_pack(gba_sram,sizeof gba_sram,job+HDR,slotSz-HDR);
 if(n<0){ds_save_status="SAVE TOO BIG FOR CHIP!";return;}
 ds_save_complen=n;seq++;jobSlot=curSlot<0?0:curSlot^1;jobAddr=jobSlot*slotSz;jobLen=n;
 u32 cs=sum32(job+HDR,n),len=n;memcpy(job,"AOD1",4);memcpy(job+4,&seq,4);memcpy(job+8,&len,4);memcpy(job+12,&cs,4);
 if(ds_save_kind==2){fat_write();return;}
 if(eeType==3)cardEepromSectorErase(jobAddr); // FLASH: erase the slot's 64 KB sector first
 jobPos=0;ds_save_status="SAVING...";}
