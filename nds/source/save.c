// save.c - persistence of the GBA SRAM image on the DS.
// The game keeps using its GBA SRAM layout (checksummed blocks from 0x2200 etc.) in gba_sram[].
// Storage: the DS cartridge save chip (EEPROM/FLASH via libnds). melonDS (and Delta) give an
// unknown retail-style cart an 8 KB EEPROM, so the 64 KB image is RLE-compressed into one of two
// 4 KB slots (ping-pong, sequence number + checksum, header written last = power-safe).
// Writes are spread over frames (one 32-byte page per frame).
// GBA save import: (1) a GBA "Arsenal of Eras" cartridge/ROM+save in Slot-2 (DS/DS Lite, or
// melonDS/Delta "GBA slot") is read at boot when the DS save is empty or SELECT is held;
// (2) on flashcarts with DLDI, fat:/ArsenalOfEras.sav (the GBA .sav) is imported the same way.
#include "platform.h"
#include <fat.h>
#include <stdio.h>
#include <string.h>
#define SLOT 0x1000
#define HDR 16
static u8 saved[0x10000];
static u8 job[SLOT] __attribute__((aligned(4)));static int jobLen,jobPos=-1,jobSlot,jobAddr;
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
static int read_slot(int s,u32*sq){u8 h[HDR];cardReadEeprom(s*SLOT,h,HDR,eeType);
 if(memcmp(h,"AOD1",4))return 0;u32 q,len,cs;memcpy(&q,h+4,4);memcpy(&len,h+8,4);memcpy(&cs,h+12,4);
 if(len==0||len>SLOT-HDR)return 0;static u8 b[SLOT];cardReadEeprom(s*SLOT+HDR,b,len,eeType);
 if(sum32(b,len)!=cs)return 0;*sq=q;return (int)len;}
static int load_slot(int s){u32 q;int len=read_slot(s,&q);if(!len)return 0;static u8 b[SLOT];cardReadEeprom(s*SLOT+HDR,b,len,eeType);
 memset(gba_sram,0xFF,sizeof gba_sram);return rle_unpack(b,len,gba_sram,sizeof gba_sram)>0;}
static int slot2_import(void){
 sysSetCartOwner(BUS_OWNER_ARM9);
 const volatile u8*hdr=(const volatile u8*)0x080000AC;
 if(!(hdr[0]=='A'&&hdr[1]=='O'&&hdr[2]=='E'&&hdr[3]=='G'))return 0;
 const volatile u8*sr=(const volatile u8*)0x0A000000;int any=0;
 for(int i=0;i<0x8000;i++){u8 v=sr[i];gba_sram[i]=v;if(v!=0xFF&&v!=0)any=1;}
 return any;}
static int fat_import(void){
 if(!fatInitDefault())return 0;
 FILE*f=fopen("fat:/ArsenalOfEras.sav","rb");if(!f)f=fopen("fat:/saves/ArsenalOfEras.sav","rb");if(!f)return 0;
 int n=fread(gba_sram,1,sizeof gba_sram,f);fclose(f);return n>=0x4000;}
void save_init(void){
 memset(gba_sram,0xFF,sizeof gba_sram);
 eeType=cardEepromGetType();eeSize=cardEepromGetSize();ds_save_dbg_type=eeType;ds_save_dbg_size=eeSize;
 int have=0;
 if(eeType>0&&eeSize>=2*SLOT){ds_save_kind=1;
  u32 qa=0,qb=0;int la=read_slot(0,&qa),lb=read_slot(1,&qb);
  if(la&&(!lb||qa>=qb)){if(load_slot(0)){curSlot=0;seq=qa;have=1;}}
  else if(lb){if(load_slot(1)){curSlot=1;seq=qb;have=1;}}
  ds_save_status=have?"DS SAVE LOADED":"NEW DS SAVE";}
 scanKeys();int force=keysHeld()&KEY_SELECT;
 if(!have||force){static u8 keep[0x10000];if(have)memcpy(keep,gba_sram,sizeof keep);
  if(slot2_import()){ds_save_imported=1;ds_save_status="IMPORTED GBA SAVE (SLOT-2)";}
  else if(fat_import()){ds_save_imported=2;ds_save_status="IMPORTED GBA SAVE (SD)";}
  else if(have)memcpy(gba_sram,keep,sizeof keep);}
 // an imported save is written out on the first save_tick; otherwise remember what is on the chip
 if(ds_save_imported)memset(saved,0x5A,sizeof saved);else memcpy(saved,gba_sram,sizeof saved);
 if(!ds_save_kind&&!ds_save_imported)ds_save_status="NO SAVE CHIP - PROGRESS NOT KEPT";}
void save_tick(void){
 if(ds_save_kind!=1)return;
 if(jobPos>=0){ // write one 32-byte page per frame; data first, header last
  int end=jobLen+HDR;int p=jobPos;
  if(p<end-HDR){int n=end-HDR-p;if(n>32)n=32;cardWriteEeprom(jobAddr+HDR+p,job+HDR+p,n,eeType);jobPos+=n;}
  else{cardWriteEeprom(jobAddr,job,HDR,eeType);curSlot=jobSlot;jobPos=-1;ds_save_writes++;
   // read the slot back from the chip and check it decodes to exactly what we meant to save
   u32 q=0;static u8 chk[0x10000];int ok=read_slot(jobSlot,&q)==jobLen&&q==seq;
   if(ok){static u8 b[SLOT];cardReadEeprom(jobAddr+HDR,b,jobLen,eeType);ok=rle_unpack(b,jobLen,chk,sizeof chk)==(int)sizeof chk&&!memcmp(chk,saved,sizeof chk);}
   if(ok){ds_save_verified++;ds_save_status="SAVED TO DS SAVE CHIP";}else{ds_save_status="SAVE VERIFY FAILED";memset(saved,0x5A,sizeof saved);}}
  return;}
 if(ds_frame%30)return;
 if(!memcmp(saved,gba_sram,sizeof saved))return;
 memcpy(saved,gba_sram,sizeof saved);
 int n=rle_pack(gba_sram,sizeof gba_sram,job+HDR,SLOT-HDR);
 if(n<0){ds_save_status="SAVE TOO BIG FOR CHIP!";return;}
 ds_save_complen=n;seq++;jobSlot=curSlot<0?0:curSlot^1;jobAddr=jobSlot*SLOT;jobLen=n;
 u32 cs=sum32(job+HDR,n),len=n;memcpy(job,"AOD1",4);memcpy(job+4,&seq,4);memcpy(job+8,&len,4);memcpy(job+12,&cs,4);
 jobPos=0;ds_save_status="SAVING...";}
