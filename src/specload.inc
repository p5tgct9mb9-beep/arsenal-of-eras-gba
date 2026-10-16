// ===================================================================== v1.3 SPEC OPS LOADOUT (per unit, SRAM 0x800 'AOEU')
// primary: 0 = unit default, 1.. = flat index into the all-era PRIM pool; secondary: 0 = none, 1.. = flat SEC pool
// melee: MEL list; two equipment slots (L / DOWN+L); attachments use the MODERN-era lists from loadout.inc
typedef struct{u8 prim,sec,mel,eq0,eq1,opt,muz,grip,mag;}SoLo;
static SoLo SOL[PF_NU] EWRAM;
typedef struct{const char*n;u8 dmg,reach,rate;}Mel;
#define NMEL 10
static const Mel MEL[NMEL]={{"COMBAT KNIFE",2,16,14},{"BAYONET",3,20,18},{"CAVALRY SABRE",3,20,14},{"TOMAHAWK",3,16,12},{"MACHETE",3,18,14},{"ENTRENCHING TOOL",3,16,16},
 {"LONGSWORD",4,22,18},{"KATANA",3,22,10},{"FLANGED MACE",5,16,22},{"WAR HAMMER",5,16,24}};
enum{EQ_NONE,EQ_FRAG,EQ_SEMTEX,EQ_MOLOTOV,EQ_C4,EQ_CLAY,EQ_KNIFE,EQ_HAWK,EQ_FLASH,EQ_SMOKE,EQ_STIM,EQ_MED,NEQ};
static const char*const EQ_N[NEQ]={"NONE","FRAG GRENADE","SEMTEX","MOLOTOV","C4 CHARGE","CLAYMORE","THROWING KNIFE","TOMAHAWK","FLASHBANG","SMOKE","STIM","MED KIT"};
static const char*const EQ_D[NEQ]={"-","4 SEC FUSE, BIG BLAST","STICKY, QUICK BLAST","FIRE BURST ON IMPACT","PLACED, HUGE BLAST","PLACED MINE, LONG FUSE","FAST, PIERCING THROW","ARCING HEAVY THROW",
 "STUNS ENEMIES NEARBY","3 SEC STEALTH CLOUD","+1 HP","+2 HP"};
static const u8 EQ_CNT[NEQ]={0,3,3,3,2,2,4,3,2,2,2,1};
typedef struct{int rate,dmg,life,spd,pel,spr,pierce,melee,sup,move,wk;const char*n;}SoW;
static SoW SW[2];static int pWsel,pEq1n;
static int so_unit_mel(int u){int wk=PF_UW[u];return wk==11?6:wk==12?7:wk==9?1:wk==13?7:0;}
static void so_defaults(int u){SoLo d={0,0,(u8)so_unit_mel(u),EQ_FRAG,EQ_NONE,0,0,0,0};SOL[u]=d;}
static int so_valid(const SoLo*l){return l->prim<=fsum(NPRIM)&&l->sec<=fsum(NSEC)&&l->mel<NMEL&&l->eq0<NEQ&&l->eq1<NEQ&&l->opt<NATL[0][2]&&l->muz<NATL[1][2]&&l->grip<NATL[2][2]&&l->mag<NATL[3][2];}
#define SO_OFF 0x1000 // v1.3: moved from 0x800 (overlapped the AA record at 0x880)
static void so_write(void){u8 sum=0x77;const u8*b=(const u8*)SOL;SRAM[SO_OFF]='A';SRAM[SO_OFF+1]='O';SRAM[SO_OFF+2]='E';SRAM[SO_OFF+3]='U';SRAM[SO_OFF+4]=1;SRAM[SO_OFF+5]=PF_NU;
 for(unsigned i=0;i<sizeof SOL;i++){SRAM[SO_OFF+6+i]=b[i];sum+=b[i]*(i+5);}SRAM[SO_OFF+6+sizeof SOL]=sum;}
static int so_read_at(int o){
 if(SRAM[o]!='A'||SRAM[o+1]!='O'||SRAM[o+2]!='E'||SRAM[o+3]!='U'||SRAM[o+4]!=1)return 0;int n=SRAM[o+5];if(n>PF_NU)n=PF_NU;
 u8 b[sizeof SOL];u8 sum=0x77;unsigned sz=n*sizeof(SoLo);for(unsigned i=0;i<sz;i++){b[i]=SRAM[o+6+i];sum+=b[i]*(i+5);}if(sum!=SRAM[o+6+sz])return 0;
 for(int u=0;u<n;u++){SoLo l;memcpy(&l,b+u*sizeof(SoLo),sizeof l);if(so_valid(&l))SOL[u]=l;}return 1;}
// v1.3: loadouts live at 0x1000; a pre-1.3 record at 0x800 (31 units) is migrated once if its checksum still holds
static void so_read(void){for(int u=0;u<PF_NU;u++)so_defaults(u);if(so_read_at(SO_OFF))return;if(so_read_at(0x800))so_write();}
// base stats of a unit's own weapon kind
static const u8 WK_RATE[14]={10,6,5,9,24,10,10,10,13,42,22,16,10,9},WK_DMG[14]={1,1,1,1,3,1,1,1,1,4,2,3,2,1};
static void so_calc(int u,int slot,SoW*s){const SoLo*l=&SOL[u];int wk=PF_UW[u];s->sup=(wk==3||wk==8);s->pel=1;s->spr=0;s->move=100;s->melee=0;s->wk=wk;
 int idx=slot?l->sec:l->prim;
 if(slot&&!idx){s->n=0;return;}
 if(!idx){s->rate=WK_RATE[wk];s->dmg=WK_DMG[wk]*100;s->life=40;s->spd=0x700;s->pierce=(wk==4||wk==9||wk==10);s->melee=(wk==11||wk==12);s->n=WPN_N[wk];
  if(wk==10){s->spd=0x600;s->life=48;}if(wk==9){s->spr=1;}}
 else{int e,i;const Wpn*w;if(slot){e=fdec(NSEC,idx-1,&i);w=&SEC[e][i];}else{e=fdec(NPRIM,idx-1,&i);w=&PRIM[e][i];}
  s->n=w->n;s->dmg=clampi((w->dmg+12)*4,100,500);s->rate=clampi(w->rate*13/10+3,4,60);s->life=clampi(w->life,18,60);s->spd=0x500+w->spd16*8;if(s->spd>0x900)s->spd=0x900;
  s->pel=w->pellets>1?(w->pellets>4?4:w->pellets):1;s->spr=w->spread/3;s->pierce=w->dmg>=70;s->move=w->move;s->wk=slot?8:(w->rate<=7?1:w->dmg>=70?4:0);if(s->sup==0)s->sup=(slot==1&&e==0&&i==SEC_WELROD);}
 if(!s->melee){const u8 sl[4]={l->opt,l->muz,l->grip,l->mag};
  for(int k=0;k<4;k++){const Att*a=&ATTG[ATL[k][2][sl[k]<NATL[k][2]?sl[k]:0]];s->dmg=s->dmg*(100+a->dmg)/100;s->rate=s->rate*(100+a->rate)/100+(a->rate>0);s->life=s->life*(100+a->life)/100;s->move+=a->mv;s->spr+=a->spr;
   if(a->fx==AF_SUP||a->fx==AF_HSUP)s->sup=1;}
  if(s->spr<0)s->spr=0;if(s->rate<3)s->rate=3;}}
// stat bars: DMG / ROF / RANGE / MOBILITY in 0..100
static void so_bars(const SoW*s,int*o){if(!s->n){o[0]=o[1]=o[2]=o[3]=0;return;}if(s->melee){o[0]=clampi(MEL[SOL[PS.unit].mel].dmg*20,0,100);o[1]=clampi(600/MEL[SOL[PS.unit].mel].rate,0,100);o[2]=10;o[3]=clampi(s->move,0,100);return;}
 o[0]=clampi(s->dmg*s->pel/6,0,100);o[1]=clampi(360/s->rate,0,100);o[2]=clampi(s->life*s->spd/0x700*100/60,0,100);o[3]=clampi(s->move-10,0,100);}
