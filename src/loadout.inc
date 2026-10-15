static const char*const TEAMN[5][2]={{"ALLIES","AXIS"},{"NATO","PACT"},{"BLUFOR","OPFOR"},{"UNION","SYNDIC"},{"TERRAN","VOID"}};
// ===================================================================== LOADOUT / CUSTOMIZE (+ SRAM save)
typedef struct{const char*n;u8 dmg,rate,spd16,spread,pellets,life,mag,reload,move;}Wpn;
typedef struct{int dmg,rate,spd16,spread,pellets,life,mag,reload,move,kind,fl;const char*n;}Wstat;
#define MAXPRIM 35
#define MAXSEC 8
static const u8 NPRIM[5]={35,5,30,4,4};
static const u8 NPRIM_V4[5]={17,5,5,4,4},NSEC_V4[5]={5,2,2,2,2},NL_V4[5]={3,2,2,2,2},NTK_V4[5]={55,6,6,2,2}; // v1.2-pre layouts (save migration)
#define PRIM_EXP0 15 // WW2 primaries 15..16 are tagged EXPERIMENTAL
#define PRIM_ISEXP(i) ((i)>=PRIM_EXP0&&(i)<17)
#define PRIM_FLAME 34 // WW2 M2 flamethrower
#define PRIM_DELISLE 29
static const u8 NSEC[5]={8,2,6,2,2};
#define SEC_WELROD 7
static const Wpn PRIM[5][MAXPRIM]={
 {{"M1 GARAND",50,20,84,2,1,42,8,90,100},{"THOMPSON M1A1",20,6,72,7,1,30,30,110,104},{"BAR M1918",32,8,84,5,1,42,20,130,92},{"KAR98K",95,40,100,1,1,50,5,120,98},
  {"STG 44",26,7,84,5,1,38,30,110,100},{"M1 CARBINE",30,10,80,3,1,36,15,85,104},{"MP40",19,7,72,6,1,30,32,100,105},{"MG42",24,3,84,10,1,40,50,255,84},
  {"PPSH-41",16,3,72,9,1,28,71,150,103},{"MOSIN-NAGANT",92,42,100,1,1,50,5,125,98},{"LEE-ENFIELD NO4",70,24,96,2,1,46,10,110,99},{"STEN MK II",18,6,70,8,1,28,32,95,106},
  {"M3 GREASE GUN",22,8,70,6,1,28,30,105,104},{"M1903 SCOPED",105,46,110,0,1,58,5,130,95},{"M97 TRENCH GUN",16,34,76,14,6,18,6,170,100},
  {"STG 44 VAMPIR",28,7,96,1,1,52,30,125,94},{"STG 44 KRUMMLAUF",24,7,64,9,1,22,30,110,100},
  {"M1A1 CARBINE FOLD",29,10,80,3,1,34,15,80,107},{"JOHNSON M1941",48,18,86,2,1,44,10,95,99},{"M1919A6 BROWNING",30,4,84,9,1,42,100,250,82},{"GEWEHR 43",48,16,88,2,1,46,10,95,98},{"FG 42",32,5,86,6,1,42,20,105,99},{"MG34",25,3,84,9,1,40,50,240,85},{"BERETTA M38",19,6,74,6,1,30,40,100,105},{"TYPE 99 ARISAKA",92,40,100,1,1,50,5,120,98},{"TYPE 100 SMG",17,6,70,7,1,28,30,100,105},{"TYPE 96 LMG",27,5,84,7,1,42,30,180,88},{"BREN MK II",31,6,86,5,1,44,30,150,90},{"OWEN GUN",19,6,72,6,1,30,33,100,105},{"DE LISLE CARBINE",60,26,84,2,1,40,8,110,100},{"SVT-40",46,16,88,3,1,44,10,95,99},{"DP-28",28,5,84,7,1,42,47,200,88},{"PPS-43",17,4,72,8,1,28,35,95,106},{"BOYS AT RIFLE",130,55,130,1,1,56,5,160,86},{"M2 FLAMETHROWER",7,2,44,14,2,11,60,200,90}},
 {{"AK-47",30,7,88,6,1,40,30,110,98},{"M16A1",26,6,96,4,1,40,20,100,102},{"M60",26,5,84,8,1,40,100,240,86},{"DRAGUNOV SVD",75,24,112,1,1,52,10,120,96},{"UZI",18,4,76,8,1,28,32,90,108}},
 {{"M4A1",26,5,96,4,1,40,30,95,102},{"HK416",28,5,96,3,1,42,30,100,100},{"AK-12",30,6,96,4,1,42,30,100,99},{"SCAR-H",40,8,100,4,1,46,20,110,96},{"BARRETT M82",120,45,140,1,1,56,10,160,88},
  {"M27 IAR",27,4,96,4,1,42,30,110,98},{"M7 (XM7 NGSW)",36,6,104,3,1,48,20,105,97},{"M250 (XM250)",32,4,100,7,1,46,100,230,85},{"MK 18 CQBR",25,5,90,5,1,36,30,90,104},{"REMINGTON ACR",27,5,96,4,1,42,30,100,101},{"TAVOR X95",26,5,92,4,1,40,30,95,104},{"GALIL ACE",29,6,94,4,1,42,35,105,99},{"FN F2000",26,5,94,4,1,42,30,100,102},{"FAMAS",24,4,92,5,1,40,25,100,102},{"HK433",27,5,96,4,1,42,30,100,101},{"QBZ-191",27,5,96,4,1,42,30,100,101},{"AK-15",32,6,96,4,1,44,30,105,98},{"AK-308",40,8,100,4,1,46,20,110,96},{"AN-94",30,4,100,2,1,44,30,110,98},{"SR-25",62,16,112,1,1,52,20,115,96},{"M2010 ESR",100,40,130,0,1,56,5,130,92},{"CHEYTAC M200",125,48,145,0,1,58,7,150,88},{"AWM",110,44,135,0,1,56,5,140,90},{"AI AX50",130,50,140,1,1,58,5,160,87},{"KS-23",24,36,72,15,6,18,4,160,98},{"BENELLI M4",16,14,78,12,6,18,7,140,100},{"AA-12",14,7,76,13,5,18,20,170,96},{"FN P90",19,3,84,6,1,30,50,110,107},{"HK UMP45",23,5,80,6,1,32,25,95,106},{"PP-19 BIZON",18,4,78,7,1,30,64,120,105}},
 {{"SMART RIFLE",30,6,104,1,1,44,30,100,100},{"RAILGUN",110,42,170,0,1,46,5,150,92},{"LASER CARBINE",22,3,140,2,1,36,60,140,100},{"PULSE SMG",18,3,96,6,1,30,45,90,108}},
 {{"PLASMA RIFLE",32,7,90,4,1,40,30,110,100},{"COILGUN",85,30,150,1,1,52,8,130,94},{"GAUSS SMG",18,3,100,6,1,30,50,95,108},{"ION REPEATER",22,4,96,7,1,40,80,200,90}}};
static const u8 AXIS_PRIM[11]={3,4,6,7,20,21,22,23,24,25,26}; // WW2 bots on the Axis side carry these
static const u8 ALLY_PRIM[21]={0,1,2,5,8,9,10,11,12,13,14,17,18,19,27,28,30,31,32,33,29};
static const Wpn SEC[5][MAXSEC]={
 {{"M1911",34,12,72,4,1,26,7,70,106},{"LUGER P08",30,10,72,3,1,26,8,70,106},{"WALTHER P38",31,11,72,3,1,26,8,70,106},{"WEBLEY MK VI",42,16,70,4,1,24,6,95,106},{"TOKAREV TT-33",29,9,80,4,1,26,8,70,106},{"NAGANT M1895",32,14,70,4,1,24,7,140,106},{"NAMBU TYPE 14",26,11,70,4,1,24,8,75,106},{"WELROD MK I",40,30,64,3,1,22,6,110,106}},
 {{"MAKAROV PM",28,10,72,4,1,26,8,65,106},{"M1911A1",34,12,72,4,1,26,7,70,106}},
 {{"GLOCK 17",25,8,80,4,1,26,17,60,106},{"M870 SHOTGUN",18,30,80,12,5,18,8,150,100},{"SIG M17",27,8,82,3,1,26,17,62,106},{"SIG M18",26,8,82,3,1,26,17,58,107},{"FN FIVE-SEVEN",24,7,90,3,1,28,20,62,106},{"HK USP",30,9,80,3,1,26,12,62,106}},
 {{"SMART PISTOL",26,9,90,1,1,28,12,60,106},{"ARC PISTOL",40,18,100,3,1,24,6,70,106}},
 {{"NEEDLER",16,4,90,5,1,26,24,70,106},{"ARC SCATTER",22,30,90,10,4,22,6,100,100}}};
// launchers: dmg, rate(frames between shots), spd16, spread, pellets, life, ammo, reload(unused), move
static const u8 NL[5]={4,2,6,2,2}; // launchers per era (excluding NONE)
#define LAUN_EXP0 3 // WW2 launcher index 3 is EXPERIMENTAL
static const Wpn LAUN[5][7]={
 {{"NONE",0,0,0,0,0,0,0,0,100},{"BAZOOKA M1A1",90,90,56,2,1,70,3,0,96},{"PANZERSCHRECK",110,110,52,2,1,70,2,0,95},{"FLIEGERFAUST",34,100,64,9,4,46,2,0,97},{"PIAT",100,110,48,3,1,66,2,0,94}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"M72 LAW",90,60,60,2,1,64,2,0,98},{"RPG-7",100,100,58,3,1,70,3,0,95}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"AT4",110,80,64,1,1,64,2,0,97},{"CARL GUSTAF",100,90,62,1,1,72,3,0,94},{"CARL GUSTAF M4",105,80,64,1,1,74,4,0,96},{"NLAW",115,95,60,1,1,64,1,0,96},{"FIM-92 STINGER",80,100,90,0,1,80,2,0,95},{"RPG-29",120,110,58,2,1,70,2,0,94}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"SMART LAUNCHER",90,70,70,0,1,64,3,0,96},{"PLASMA LANCE",120,110,80,0,1,60,2,0,95}},
 {{"NONE",0,0,0,0,0,0,0,0,100},{"GRAV LANCE",100,80,72,1,1,64,3,0,96},{"ION MORTAR",90,70,60,3,1,70,4,0,95}}};
#define MAXLE 10
#define MAXTA 11
static const char*const LETG_N[15]={"MK2 FRAG","STIELHANDGRANATE","GAMMON BOMB","FRAG","SEMTEX","CLUSTER","M18 CLAYMORE","C4 CHARGE","THERMITE","MOLOTOV","THROWING KNIFE","TOMAHAWK","SATCHEL CHARGE","IMPACT FRAG","NO.36 MILLS BOMB"};
static const u8 LETG_SUB[15]={0,3,4,0,1,2,5,6,7,8,9,9,6,4,0};
static const u8 NLE[5]={7,8,10,6,6};static const u8 LEL[5][MAXLE]={{0,1,2,14,12,9,10},{3,4,5,6,7,9,11,13},{3,4,5,6,7,8,9,10,11,13},{3,4,5,7,8,13},{3,4,5,7,8,13}};
static const char*const LETH_D[10]={"4 SEC FUSE, BIG BLAST","SHORT FUSE, STICKS TO WALLS","3 SMALLER BLASTS","LONG THROW (STICK), FRAG BLAST","IMPACT FUSE, HUGE BLAST","PLACED MINE - TRIGGERS ON PROXIMITY","STICKY, 1.5 SEC, VERY BIG BLAST","STICKY, BURNS AN AREA 2 SEC","IMPACT, FIRE POOL 2.5 SEC","SILENT FAST THROW, 1 TARGET"};
#define LETH_N_(e,i) LETG_N[LEL[e][i]]
#define LETH_SUB_(e,i) LETG_SUB[LEL[e][i]]
static const char*const TACG_N[17]={"FLASHBANG","SMOKE","STIM","CS GAS","DECOY","EMP GRENADE","MED KIT","AMMO CRATE","DEPLOY COVER","TROPHY SYSTEM","RECON DRONE","M15 SMOKE","MORPHINE","STUN GRENADE","SANDBAG COVER","AMMO CACHE","FIELD MED KIT"};
static const u8 TACG_FX[17]={0,1,2,3,4,5,6,7,8,9,10,1,2,0,8,7,6};
static const u8 NTACT[5]={5,7,11,8,8};static const u8 TAL[5][MAXTA]={{11,12,16,15,14},{13,1,2,3,6,7,8},{0,1,2,3,4,5,6,7,8,9,10},{0,1,2,4,5,8,9,10},{0,1,2,4,5,8,9,10}};
static const char*const TACD[11]={"STUNS ENEMIES 2.5 SEC","BLOCKS ENEMY LINE OF SIGHT","INSTANT +60 HP","GAS CLOUD DAMAGES OVER TIME","FAKE GUNFIRE DISTRACTS BOTS","STUNS ALL ENEMIES IN RADIUS","FULL HEAL","REFILLS AMMO, +1 LETHAL","BULLET-PROOF BARRIER 10 SEC","SHOOTS DOWN ENEMY GRENADES/ROCKETS","MARKS ENEMIES: +25% DAMAGE 10 SEC"};
#define TACT_N_(e,i) TACG_N[TAL[e][i]]
#define TACT_FX_(e,i) TACG_FX[TAL[e][i]]
static const char*const PERKN[5]={"SPRINTER","FLAK JACKET","SCAVENGER","SLEIGHT OF HAND","JUGGERNAUT"};
static const char*const PERKD[5]={"MOVE SPEED +12%","EXPLOSIVE DAMAGE -50%","KILLS REFILL AMMO + LETHAL","RELOAD -40%","130 HP, MOVE -6%"};
#include "vehdb.inc"
typedef struct{u8 prim,sec,opt,muz,grip,mag,leth,tact,perk,laun;}Loadout;
static Loadout LO[5];static u8 tankSel[5],airSel[5];
static const char save_tag[] __attribute__((used,aligned(4)))="SRAM_V113";
#define SRAM ((volatile u8*)0x0E000000)
// v1.2 attachments: slots 0 optic 1 muzzle 2 underbarrel 3 magazine 4 barrel 5 laser/light 6 stock 7 ammo 8 armor
typedef struct{const char*n,*d;s8 dmg,rate,spr,life,mag,rel,mv,hp;u8 fx;}Att;
static const Att ATTG[56]={{"IRON SIGHTS","NO CHANGE",0,0,0,0,0,0,0,0,0},{"RED DOT","RECOIL -1",0,0,-1,0,0,0,0,0,0},{"4X SCOPE","RECOIL -2 RANGE +15% MOB -3",0,0,-2,15,0,0,-3,0,0},{"REFLEX SIGHT","RECOIL -1, MOB +1",0,0,-1,0,0,0,1,0,0},{"HOLOGRAPHIC","RECOIL -1, RANGE +5%",0,0,-1,5,0,0,0,0,0},{"LPVO 1-6X","RECOIL -2, RANGE +10%, MOB -2",0,0,-2,10,0,0,-2,0,0},{"ACOG 4X","RECOIL -2, RANGE +12%, MOB -2",0,0,-2,12,0,0,-2,0,0},{"THERMAL SIGHT","DAMAGE +10% (SPOTTING), MOB -3",10,0,0,0,0,0,-3,0,0},{"NIGHT VISION","DAMAGE +5%, RECOIL -1, MOB -2",5,0,-1,0,0,0,-2,0,0},{"HYBRID 1X/3X","RECOIL -2, RANGE +12%, MOB -1",0,0,-2,12,0,0,-1,0,0},{"SCOPE MOUNT 4X","RECOIL -2 RANGE +20% ROF -10% MOB -4",0,10,-2,20,0,0,-4,0,0},{"STANDARD","NO CHANGE",0,0,0,0,0,0,0,0,0},{"SUPPRESSOR","BOTS SPOT YOU LATER, RANGE -15%",0,0,0,-15,0,0,0,0,1},{"COMPENSATOR","RECOIL -2, FIRE RATE -10%",0,10,-2,0,0,0,0,0,0},{"MUZZLE BRAKE","RECOIL -2, MOB -2",0,0,-2,0,0,0,-2,0,0},{"FLASH HIDER","NO FLASH, BOTS SPOT YOU LATE",0,0,0,0,0,0,0,0,2},{"HEAVY SUPPR.","STEALTHIEST, RANGE -20%, DMG -10%",-10,0,0,-20,0,0,-1,0,3},{"COMPACT SUPPR","STEALTH, RANGE -8%, MOB -1",0,0,0,-8,0,0,-1,0,1},{"BAYONET","POINT-BLANK STAB +45 DAMAGE",0,0,0,0,0,0,-1,0,4},{"RIFLE GRENADE","+2 LETHALS, THROWN 2X FARTHER",0,0,0,0,0,0,-2,0,5},{"NONE","NO CHANGE",0,0,0,0,0,0,0,0,0},{"VERTICAL GRIP","RECOIL -1",0,0,-1,0,0,0,0,0,0},{"ANGLED GRIP","RELOAD -15%",0,0,0,0,0,-15,0,0,0},{"BIPOD","RECOIL -3, MOB -6",0,0,-3,0,0,0,-6,0,0},{"M203 GL","+2 LETHALS, MOB -3",0,0,0,0,0,0,-3,0,6},{"GP-25 GL","+2 LETHALS, MOB -3",0,0,0,0,0,0,-3,0,6},{"MASTERKEY SG","L: UNDERBARREL SHOTGUN IF NO LAUNCHER",0,0,0,0,0,0,-3,0,7},{"HANDSTOP","RECOIL -1, MOB +1",0,0,-1,0,0,0,1,0,0},{"STANDARD MAG","NO CHANGE",0,0,0,0,0,0,0,0,0},{"EXTENDED MAG","MAG +50%, RELOAD +15%, MOB -3",0,0,0,0,50,15,-3,0,0},{"FAST MAG","RELOAD -35%",0,0,0,0,0,-35,0,0,0},{"DRUM MAG","MAG +100%, RELOAD +35%, MOB -5",0,0,0,0,100,35,-5,0,0},{"STD BARREL","NO CHANGE",0,0,0,0,0,0,0,0,0},{"SHORT BARREL","RANGE -15%, RECOIL +1, MOB +3",0,0,1,-15,0,0,3,0,0},{"LONG BARREL","RANGE +15%, DMG +5%, MOB -2",5,0,0,15,0,0,-2,0,0},{"HEAVY BARREL","DMG +8%, RECOIL -1, MOB -4",8,0,-1,0,0,0,-4,0,0},{"NONE","NO CHANGE",0,0,0,0,0,0,0,0,0},{"LASER SIGHT","RECOIL -1, MOB +1",0,0,-1,0,0,0,1,0,0},{"FLASHLIGHT","BLINDS (STUNS) CLOSE ENEMIES",0,0,0,0,0,0,0,0,8},{"IR LASER","RECOIL -1",0,0,-1,0,0,0,0,0,0},{"LASER+LIGHT","RECOIL -1, BLINDS CLOSE ENEMIES, MOB -1",0,0,-1,0,0,0,-1,0,8},{"STD STOCK","NO CHANGE",0,0,0,0,0,0,0,0,0},{"FOLDING STOCK","MOB +4, RECOIL +1",0,0,1,0,0,0,4,0,0},{"HEAVY STOCK","RECOIL -1, MOB -3",0,0,-1,0,0,0,-3,0,0},{"NO STOCK","MOB +7, RECOIL +2, RELOAD -10%",0,0,2,0,0,-10,7,0,0},{"FMJ ROUNDS","STANDARD BALL AMMO",0,0,0,0,0,0,0,0,0},{"ARMOR-PIERCING","DMG +6%, RANGE +5%",6,0,0,5,0,0,0,0,0},{"HOLLOW POINT","DMG +18%, RANGE -10%",18,0,0,-10,0,0,0,0,0},{"INCENDIARY","25% OF HITS IGNITE, DMG -5%",-5,0,0,0,0,0,0,0,9},{"NO ARMOR","NO CHANGE",0,0,0,0,0,0,0,0,0},{"PLATES LVL I","+15 HP, MOB -2",0,0,0,0,0,0,-2,15,0},{"PLATES LVL II","+30 HP, MOB -4",0,0,0,0,0,0,-4,30,0},{"PLATES LVL III","+50 HP, MOB -7",0,0,0,0,0,0,-7,50,0},{"GHILLIE SUIT","BOTS SPOT YOU ONLY UP CLOSE, MOB -3",0,0,0,0,0,0,-3,0,12},{"NVG","MARKS TARGETS: +10% DMG, RECOIL -1",0,0,-1,0,0,0,-1,0,13},{"BALLIST SHLD","FRONTAL DAMAGE -60%, MOB -8",0,0,0,0,0,0,-8,0,14}};
#define MAXATL 10
static const u8 NATL[9][5]={{4,6,10,6,6},{6,7,8,5,5},{4,6,8,5,5},{4,4,4,4,4},{4,4,4,4,4},{1,2,5,4,4},{4,4,4,4,4},{3,4,4,4,4},{2,4,7,6,6}};
static const u8 ATL[9][5][MAXATL]={{{0,1,2,10},{0,1,2,10,6,8},{0,1,2,3,4,5,6,7,8,9},{0,1,2,4,7,9},{0,1,2,4,7,9}},{{11,12,13,18,19,15},{11,12,13,14,15,18,19},{11,12,13,14,15,16,17,18},{11,12,13,14,15},{11,12,13,14,15}},{{20,21,22,23},{20,21,22,23,24,25},{20,21,22,23,24,25,26,27},{20,21,22,23,27},{20,21,22,23,27}},{{28,29,30,31},{28,29,30,31},{28,29,30,31},{28,29,30,31},{28,29,30,31}},{{32,33,34,35},{32,33,34,35},{32,33,34,35},{32,33,34,35},{32,33,34,35}},{{36},{36,38},{36,37,38,39,40},{36,37,38,40},{36,37,38,40}},{{41,42,43,44},{41,42,43,44},{41,42,43,44},{41,42,43,44},{41,42,43,44}},{{45,46,48},{45,46,47,48},{45,46,47,48},{45,46,47,48},{45,46,47,48}},{{49,53},{49,50,53,54},{49,50,51,52,53,54,55},{49,50,51,52,54,55},{49,50,51,52,54,55}}};
enum{AF_SUP=1,AF_HIDER,AF_HSUP,AF_BAYO,AF_RGREN,AF_UGL,AF_MKEY,AF_LIGHT,AF_INC,AF_GHIL=12,AF_NVG,AF_SHIELD};
typedef struct{u8 bar,las,stk,amo,arm;}LoX;static LoX LX[5];
static u8*att_fld(int e,int s){Loadout*l=&LO[e];LoX*x=&LX[e];switch(s){case 0:return &l->opt;case 1:return &l->muz;case 2:return &l->grip;case 3:return &l->mag;case 4:return &x->bar;case 5:return &x->las;case 6:return &x->stk;case 7:return &x->amo;}return &x->arm;}
static const Att*att_of(int e,const Loadout*l,int s){int v=s==0?l->opt:s==1?l->muz:s==2?l->grip:s==3?l->mag:((u8*)&LX[e])[s-4];if(v>=NATL[s][e])v=0;return &ATTG[ATL[s][e][v]];}
static u32 att_fx(int e,const Loadout*l){u32 m=0;for(int s=0;s<9;s++){int f=att_of(e,l,s)->fx;if(f)m|=1u<<f;}return m;}
#define LX_OFF 0x680
static void lx_write(void){u8 sum=0x33;SRAM[LX_OFF]='A';SRAM[LX_OFF+1]='O';SRAM[LX_OFF+2]='E';SRAM[LX_OFF+3]='X';u8*b=(u8*)LX;for(int i=0;i<(int)sizeof LX;i++){SRAM[LX_OFF+4+i]=b[i];sum+=b[i]*(i+3);}SRAM[LX_OFF+4+sizeof LX]=sum;}
static void lx_read(void){memset(LX,0,sizeof LX);if(SRAM[LX_OFF]!='A'||SRAM[LX_OFF+1]!='O'||SRAM[LX_OFF+2]!='E'||SRAM[LX_OFF+3]!='X')return;u8 sum=0x33;u8 b[sizeof LX];
 for(int i=0;i<(int)sizeof LX;i++){b[i]=SRAM[LX_OFF+4+i];sum+=b[i]*(i+3);}if(sum!=SRAM[LX_OFF+4+sizeof LX])return;memcpy(LX,b,sizeof LX);
 for(int e=0;e<5;e++)for(int s=4;s<9;s++)if(*att_fld(e,s)>=NATL[s][e])*att_fld(e,s)=0;}
#define LO_F 10
// ---------------------------------------------------------------- 6v6 ARSENAL: one pool across all eras + factions
typedef struct{u8 pI,sI,lI,leth,tact,tkI,aI,fac0,fac1,shI;}G6;
static G6 g6={0,0,0,0,0,0,0,0,6,0};
static const u8 N2_V3[5]={2,2,2,2,2};static const u8 NAIR_V3[5]={13,2,2,2,2}; // v3 save index layouts (migration)
static const char*const ERA_TAG[5]={"WW2","COLD WAR","MODERN","FUTURE","SPACE"};
static int fsum(const u8*n){int s=0;for(int e=0;e<5;e++)s+=n[e];return s;}
static int fdec(const u8*n,int idx,int*o){for(int e=0;e<5;e++){if(idx<n[e]){*o=idx;return e;}idx-=n[e];}*o=0;return 0;}
static int fenc(const u8*n,int e,int i){int s=0;for(int k=0;k<e;k++)s+=n[k];return s+i;}
#define NLETH6 14
#define NTACT6 12
static const char*const LETH6_N[NLETH6]={"MK2 FRAG","STIELHANDGRANATE","GAMMON BOMB","FRAG","SEMTEX","CLUSTER","M18 CLAYMORE","C4 CHARGE","THERMITE","MOLOTOV","THROWING KNIFE","TOMAHAWK","SATCHEL CHARGE","IMPACT FRAG"};
static const u8 LETH6_SUB[NLETH6]={0,3,4,0,1,2,5,6,7,8,9,9,6,4};
static const char*const TACT6_N[NTACT6]={"SMOKE","MORPHINE","FLASHBANG","STIM","CS GAS","DECOY","EMP GRENADE","MED KIT","AMMO CRATE","DEPLOY COVER","TROPHY SYSTEM","RECON DRONE"};
static const u8 TACT6_FX[NTACT6]={1,2,0,2,3,4,5,6,7,8,9,10};
typedef struct{const char*n;u8 m[3],h[3],t[3];u8 axis;}Fac;
#define NFAC 11
static const Fac FAC[NFAC]={{"USA",{92,104,58},{80,92,52},{70,150,255},0},{"UK",{124,104,66},{100,90,60},{90,200,255},0},{"USSR",{132,128,76},{90,96,60},{230,60,50},0},
 {"GERMANY",{96,102,100},{80,86,88},{210,210,210},1},{"JAPAN",{150,132,80},{120,110,70},{240,140,60},1},{"CHINA",{80,110,70},{70,96,64},{255,90,90},0},
 {"RUSSIA",{104,110,84},{84,92,70},{220,70,70},0},{"NATO",{82,98,64},{70,80,56},{80,140,255},0},{"COALITION",{170,148,104},{150,134,96},{90,220,160},0},
 {"FEDERATION",{190,196,206},{170,176,190},{110,200,255},0},{"SYNDICATE",{80,64,92},{70,56,80},{255,80,200},0}};
static u16 fac_c(const u8*c,int num,int den){return RGB(clampi(c[0]*num/den,0,255),clampi(c[1]*num/den,0,255),clampi(c[2]*num/den,0,255));}
static void fac_pal(int bank,int f,int player){volatile u16*p=PAL_OBJ+bank*16;const Fac*F=&FAC[f];p[2]=fac_c(F->m,3,4);p[3]=fac_c(F->m,1,1);p[4]=fac_c(F->m,5,4);p[5]=fac_c(F->h,1,1);p[9]=player?RGB(255,220,40):fac_c(F->t,1,1);}
#define SAVE_N2 (5*LO_F+10+6+4+2)
#define SAVE_N3 (SAVE_N2+9+20)
#define SAVE_N (SAVE_N2+10+20+1)
#define SAVE_V 5
static u32 campUnlock=1;static u8 droneSel,droneCall;
static void lo_defaults(void){campUnlock=1;droneSel=0;droneCall=0;{G6 d={0,0,0,0,0,0,0,0,6,0};g6=d;}SET.bot=1;{Chr c={"SOLDIER",1,0,0,0,0,1,1,0,1};CH=c;}memset(LO,0,sizeof LO);memset(tankSel,0,5);memset(airSel,0,5);}
static int lo_valid(void){for(int e=0;e<5;e++){Loadout*l=&LO[e];if(l->prim>=NPRIM[e]||l->sec>=NSEC[e]||l->opt>=NATL[0][e]||l->muz>=NATL[1][e]||l->grip>=NATL[2][e]||l->mag>=NATL[3][e]||l->leth>=NLE[e]||l->tact>=NTACT[e]||l->perk>4||l->laun>NL[e]||tankSel[e]>=NTK[e]||airSel[e]>=NAIR[e])return 0;}return 1;}
static void save_write(void){u8 b[SAVE_N];for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<LO_F;k++)b[e*LO_F+k]=p[k];b[5*LO_F+e]=tankSel[e];b[5*LO_F+5+e]=airSel[e];}
 {u8*q=&SET.track;for(int k=0;k<6;k++)b[5*LO_F+10+k]=q[k];for(int k=0;k<4;k++)b[5*LO_F+16+k]=campUnlock>>(8*k);b[5*LO_F+20]=droneSel;b[5*LO_F+21]=droneCall;}
 {u8*q=&g6.pI;for(int k=0;k<10;k++)b[SAVE_N2+k]=q[k];u8*c=(u8*)&CH;for(int k=0;k<20;k++)b[SAVE_N2+10+k]=c[k];b[SAVE_N2+30]=SET.bot;}
 u8 sum=0x5A;SRAM[0]='A';SRAM[1]='O';SRAM[2]='E';SRAM[3]='L';SRAM[4]=SAVE_V;
 for(int i=0;i<SAVE_N;i++){SRAM[5+i]=b[i];sum+=b[i]*(i+1);}SRAM[5+SAVE_N]=sum;}
static int saveLoaded; // 0 none/defaults, 1 v4, 2 migrated from v1, 3 migrated from v2, 4 migrated from v3
static int save_load(void){lo_defaults();saveLoaded=0;if(SRAM[0]!='A'||SRAM[1]!='O'||SRAM[2]!='E'||SRAM[3]!='L')return 0;
 int v=SRAM[4];int nf=v==1?9:LO_F;int n=v==1?5*nf+10:v==2?SAVE_N2:v==3?SAVE_N3:SAVE_N;if(v<1||v>5)return 0;
 u8 b[SAVE_N];u8 sum=0x5A;for(int i=0;i<n;i++){b[i]=SRAM[5+i];sum+=b[i]*(i+1);}if(sum!=SRAM[5+n])return 0;
 for(int e=0;e<5;e++){u8*p=&LO[e].prim;for(int k=0;k<nf;k++)p[k]=b[e*nf+k];tankSel[e]=b[5*nf+e];airSel[e]=b[5*nf+5+e];}
 if(v>=2){u8*q=&SET.track;for(int k=0;k<6;k++)q[k]=b[5*LO_F+10+k];campUnlock=0;for(int k=0;k<4;k++)campUnlock|=(u32)b[5*LO_F+16+k]<<(8*k);droneSel=b[5*LO_F+20]%9;droneCall=b[5*LO_F+21]%3;
  if(SET.track>NTRK||SET.musVol>10||SET.sfxVol>10||SET.shake>1||SET.diff>2||SET.hints>1){Settings d={0,8,10,1,1,1,1};SET=d;}}
 if(v>=3){int nb=v==3?9:10;u8*q=&g6.pI;for(int k=0;k<nb;k++)q[k]=b[SAVE_N2+k];u8*c=(u8*)&CH;for(int k=0;k<20;k++)c[k]=b[SAVE_N2+nb+k];CH.name[10]=0;
  if(v==3){int i,e; // v1.1 -> v1.2: arsenal pools grew; re-encode flat indices by (era,item)
   int op=g6.pI;e=fdec(NPRIM,op,&i);(void)e;
   if(g6.lI){int oe=(g6.lI-1)/2,ok=(g6.lI-1)%2;g6.lI=1+fenc(NL,oe,ok);}
   e=fdec(N2_V3,g6.tkI,&i);g6.tkI=fenc(NTK,e,i);e=fdec(NAIR_V3,g6.aI,&i);g6.aI=fenc(NAIR,e,i);g6.shI=0;
   {int pe=fdec((const u8[]){15,5,5,4,4},op,&i);g6.pI=fenc(NPRIM,pe,i);}e=fdec(NSEC_V4,g6.sI,&i);g6.sI=fenc(NSEC,e,i);SET.bot=1;}
  else SET.bot=b[SAVE_N2+30]>3?1:b[SAVE_N2+30];
  if(v==4){int i,e; // v1.2-pre -> v1.2: weapon pools grew at era ends; vehicles grew too
   e=fdec(NPRIM_V4,g6.pI,&i);g6.pI=fenc(NPRIM,e,i);e=fdec(NSEC_V4,g6.sI,&i);g6.sI=fenc(NSEC,e,i);
   if(g6.lI){e=fdec(NL_V4,g6.lI-1,&i);g6.lI=1+fenc(NL,e,i);}e=fdec(NTK_V4,g6.tkI,&i);g6.tkI=fenc(NTK,e,i);}
  if(g6.pI>=fsum(NPRIM)||g6.sI>=fsum(NSEC)||g6.lI>fsum(NL)||g6.leth>=NLETH6||g6.tact>=NTACT6||g6.tkI>=fsum(NTK)||g6.aI>=fsum(NAIR)||g6.shI>=fsum(NSH)||g6.fac0>=NFAC||g6.fac1>=NFAC||g6.fac0==g6.fac1){G6 d={0,0,0,0,0,0,0,0,6,0};g6=d;}
  if(CH.skin>=CH_NSKIN||CH.hair>=CH_NHAIR||CH.head>=CH_NHEAD||CH.hat>=CH_NHAT||CH.uni>=CH_NUNI||CH.camo>=CH_NCAMO||CH.vest>=CH_NVEST||CH.body>1||CH.use>1||!CH.name[0]){Chr c={"SOLDIER",1,0,0,0,0,1,1,0,1};CH=c;}}
 if(v==1){ // v1.0 -> v1.1 migration: WW2 tactical list changed (flash/smoke/stim -> smoke/morphine)
  for(int e=0;e<5;e++)LO[e].laun=0;LO[0].tact=LO[0].tact==2?1:0;}
 if(!lo_valid()){lo_defaults();return 0;}
 saveLoaded=v>=4?1:v==3?4:v==2?3:2;if(v<5)save_write();lx_read();return saveLoaded;}
static Wstat wcalc(int era,const Loadout*l,int slot){int secondary=slot!=0;const Wpn*w=slot==2?&LAUN[era][l->laun]:slot==1?&SEC[era][l->sec]:&PRIM[era][l->prim];Wstat s;s.kind=slot==2?K_ROCKET:K_BUL;
 s.n=w->n;s.dmg=w->dmg;s.rate=w->rate;s.spd16=w->spd16;s.spread=w->spread;s.pellets=w->pellets;s.life=w->life;s.mag=w->mag;s.reload=w->reload;s.move=w->move;
 s.fl=(era==0&&slot==0&&l->prim==PRIM_FLAME)?1:(era==0&&((slot==0&&l->prim==PRIM_DELISLE)||(slot==1&&l->sec==SEC_WELROD)))?2:0;
 if(!secondary){for(int k=0;k<8;k++){const Att*a=att_of(era,l,k);if(k==7&&s.fl==1)continue;
   s.dmg=s.dmg*(100+a->dmg)/100;s.rate=s.rate*(100+a->rate)/100+(a->rate>0);s.spread+=a->spr;s.life=s.life*(100+a->life)/100;s.mag=s.mag*(100+a->mag)/100;s.reload=s.reload*(100+a->rel)/100;s.move+=a->mv;}
  if(LX[era].las==3&&(l->opt==7||l->opt==8))s.spread--;}
 {const Att*a=att_of(era,l,8);s.move+=a->mv;if(!secondary)s.spread+=a->spr;}
 if(l->perk==3)s.reload=s.reload*60/100;if(l->perk==0)s.move=s.move*112/100;if(l->perk==4)s.move=s.move*94/100;
 if(l->laun)s.move-=3;if(slot==2){s.reload=s.rate;}if(s.spread<0)s.spread=0;return s;}
// ---------------------------------------------------------------- menu
enum{R_ERA,R_PRIM,R_SEC,R_LAUN,R_OPT,R_MUZ,R_GRIP,R_MAG,R_LETH,R_TACT,R_PERK,R_TANK,R_AIR,R_SHIP,R_BAR,R_LAS,R_STK,R_AMO,R_ARM,R_DONE};
static int rslot(int r){return r>=R_OPT&&r<=R_MAG?r-R_OPT:r>=R_BAR&&r<=R_ARM?r-R_BAR+4:-1;}
#define ATT_RN(r,e) (&ATTG[ATL[rslot(r)][e][*att_fld(e,rslot(r))]])
static const char*const RLAB[20]={"ERA","PRIMARY","SIDEARM","LAUNCHER","OPTIC","MUZZLE","UNDERBRL","MAGAZINE","LETHAL","TACTICAL","PERK","VEHICLE","AIRCRAFT","SHIP","BARREL","LASER","STOCK","AMMO","ARMOR",""};
static int rslot(int r);
static char wnb[14];static const char*trunc12(const char*s){int k=0;while(s[k]&&k<12){wnb[k]=s[k];k++;}wnb[k]=0;return wnb;}
static const char*const ERA_NAME[5]={"WORLD WAR II","COLD WAR","MODERN","FUTURISTIC","SPACE"};
static void m3_fill(int x0,int y0,int w,int h,u16 c){for(int y=y0;y<y0+h;y++){u16*p=&fb[y*240+x0];if(!(x0&1)&&!(w&1)&&w>=8)fill32(p,c|(c<<16),w/2);else for(int x=0;x<w;x++)p[x]=c;}}
static void m3_bar(int x,int y,const char*lab,int v,int max,u16 col){m3_text(x,y,lab,C_GRAY,1);int w=70;int f=max>0?v*w/max:0;f=clampi(f,1,w);
 m3_fill(x,y+8,w,4,RGB(40,46,70));m3_fill(x,y+8,f,4,col);}
static int lo_optcount(int r,int e){if(rslot(r)>=0)return NATL[rslot(r)][e];switch(r){case R_LETH:return NLE[e];case R_ERA:return 5;case R_PRIM:return NPRIM[e];case R_SEC:return NSEC[e];case R_TANK:return NTK[e];case R_AIR:return NAIR[e];case R_LAUN:return NL[e]+1;case R_TACT:return NTACT[e];case R_PERK:return 5;case R_DONE:return 1;}return 3;}
static u8*lo_field(int r,int e){Loadout*l=&LO[e];if(rslot(r)>=0)return att_fld(e,rslot(r));switch(r){case R_PRIM:return &l->prim;case R_SEC:return &l->sec;case R_LAUN:return &l->laun;case R_OPT:return &l->opt;case R_MUZ:return &l->muz;case R_GRIP:return &l->grip;
 case R_MAG:return &l->mag;case R_LETH:return &l->leth;case R_TACT:return &l->tact;case R_PERK:return &l->perk;case R_TANK:return &tankSel[e];case R_AIR:return &airSel[e];}return 0;}
static const char*lo_value(int r,int e){Loadout*l=&LO[e];if(rslot(r)>=0)return ATT_RN(r,e)->n;switch(r){case R_ERA:return ERA_NAME[e];case R_PRIM:return PRIM[e][l->prim].n;case R_SEC:return SEC[e][l->sec].n;case R_LAUN:return LAUN[e][l->laun].n;
 case R_LETH:return LETH_N_(e,l->leth);case R_TACT:return TACT_N_(e,l->tact);case R_PERK:return PERKN[l->perk];
 case R_TANK:return TANKV[e][tankSel[e]].n;case R_AIR:return AIRV[e][airSel[e]].n;}return "";}
static const char*lo_desc(int r,int e){Loadout*l=&LO[e];if(rslot(r)>=0)return ATT_RN(r,e)->d;switch(r){
 case R_LETH:return LETH_D[LETH_SUB_(e,l->leth)];case R_TACT:return TACD[TACT_FX_(e,l->tact)];case R_LAUN:return l->laun?"L CYCLES TO IT. SPLASH DAMAGE":"NO LAUNCHER (NO MOBILITY PENALTY)";case R_PERK:return PERKD[l->perk];case R_ERA:return "L/R OR LEFT/RIGHT: CHANGE ERA";
 case R_TANK:return VCLASS_N[TANKV[e][tankSel[e]].cls];case R_AIR:return e==0?(AIRV[e][airSel[e]].side?"AXIS SIDE - BOTS FLY AXIS TYPES":"ALLIED SIDE - BOTS FLY ALLIED TYPES"):(AIRV[e][airSel[e]].side?"EAST SIDE - BOTS FLY EAST TYPES":"WEST SIDE - BOTS FLY WEST TYPES");case R_DONE:return "SAVED TO CARTRIDGE SRAM";}return "LEFT/RIGHT: CHANGE   A: NEXT";}
// ctx: 0 full (main menu), 1 infantry, 2 campaign, 3 tank, 4 air.  returns 1 = deploy/done, 0 = back
static int loadout_menu(int ctx,int era){
 u8 rows[22];int nr=0;
 static const u8 RF[19]={R_ERA,R_PRIM,R_SEC,R_LAUN,R_OPT,R_MUZ,R_GRIP,R_MAG,R_BAR,R_LAS,R_STK,R_AMO,R_LETH,R_TACT,R_PERK,R_ARM,R_TANK,R_AIR,R_DONE};
 if(ctx==0)for(int k=0;k<19;k++)rows[nr++]=RF[k];
 else if(ctx<=2){for(int k=1;k<16;k++)rows[nr++]=RF[k];rows[nr++]=R_DONE;}
 else{rows[nr++]=ctx==3?R_TANK:R_AIR;rows[nr++]=R_DONE;}
 int sel=0,e=ctx==2?0:era,redraw=2;
 for(;;){int r=rows[sel];BEAT->screen=SCR_LOADOUT;BEAT->era=e;BEAT->menu=r;BEAT->extra=LO[e].prim|(LO[e].opt<<4)|(LO[e].perk<<8)|(tankSel[e]<<12)|(airSel[e]<<13);
  if(redraw){if(redraw==2)m3_title(1);redraw=0;
   m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);
   m3_ctext(8,ctx==2?"LOADOUT - DOG GREEN 1944":ctx>=3?(ctx==3?"SELECT TANK":"SELECT AIRCRAFT"):cat("LOADOUT - ",ERA_NAME[e]),C_YEL,1);
   int y0=ctx>=3?40:20;
   int VIS=ctx==0?14:13,top=nr>VIS?clampi(sel-VIS/2,0,nr-VIS):0;if(top>0)m3_text(152,y0,"^",C_GRAY,1);if(top+VIS<nr)m3_text(152,y0+(VIS-1)*(ctx==0?8:9),"v",C_GRAY,1);
   for(int k=top;k<nr&&k<top+VIS;k++){int rr_=rows[k];int y=y0+(k-top)*(ctx>=3?14:(ctx==0?8:9))+(rr_==R_DONE?3:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(rr_==R_DONE){m3_text(10,y,k==sel?"> ":"  ",C_YEL,1);m3_text(22,y,ctx==0?"SAVE & BACK":"DEPLOY",c,1);continue;}
    m3_text(7,y,RLAB[rr_],k==sel?C_YEL:C_GRAY,1);
    const char*v=lo_value(rr_,e);m3_text(63,y,v,c,1);
    if(k==sel&&lo_optcount(rr_,e)>1){m3_text(57,y,"<",C_YEL,1);m3_text(63+slen(v)*6+1,y,">",C_YEL,1);}}
   int sx=162,sy=22;
   if(r==R_TANK||ctx==3){const TankS*t=&TANKV[e][tankSel[e]];m3_text(sx,sy-1,trunc12(t->n),C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"ARMOR",t->hp,260,C_GREEN);m3_bar(sx,sy+16,"FIREPOWER",t->dmg,110,C_RED);m3_bar(sx,sy+32,"SPEED",t->spd,440,C_BLUE);
    m3_bar(sx,sy+48,"RELOAD",330-t->reload,330,C_YEL);m3_bar(sx,sy+64,"TURN",t->turn,500,C_WHITE);}
   else if(r==R_AIR||ctx==4){const AirS*a=&AIRV[e][airSel[e]];m3_text(sx,sy-1,trunc12(a->n),C_BLUE,1);sy+=12;
    m3_text(sx,sy-3,a->side?TEAMN[e][1]:TEAMN[e][0],a->side?C_RED:C_BLUE,1);sy+=8;m3_bar(sx,sy,"SPEED",a->spd,650,C_BLUE);m3_bar(sx,sy+16,"TURN",a->turn,720,C_WHITE);m3_bar(sx,sy+32,"GUNS",a->dmg*60/a->rate,180,C_RED);
    m3_bar(sx,sy+48,"ARMOR",a->hp,150,C_GREEN);m3_bar(sx,sy+64,cat(a->sec==2?"BOMBS ":a->mturn?"MISSILES ":"ROCKETS ",itoa(a->ammo)),a->ammo,12,C_YEL);
    if(a->gunner)m3_text(sx,sy+82,"+REAR GUNNER",C_GRAY,1);else if(a->mturn)m3_text(sx,sy+82,cat("TRACKING ",itoa(a->mturn/72)),C_GRAY,1);else m3_text(sx,sy+82,"UNGUIDED",C_GRAY,1);}
   else{int secd=r==R_SEC?1:(r==R_LAUN&&LO[e].laun)?2:0;Wstat w=wcalc(e,&LO[e],secd);m3_text(sx,sy-1,trunc12(w.n),C_BLUE,1);sy+=12;
    m3_bar(sx,sy,"DAMAGE",w.dmg*w.pellets,120,C_RED);m3_bar(sx,sy+16,"FIRE RATE",600/w.rate,200,C_YEL);
    m3_bar(sx,sy+32,"RANGE",w.spd16*w.life/16,420,C_BLUE);m3_bar(sx,sy+48,"RECOIL",w.spread+1,13,RGB(255,140,60));
    m3_bar(sx,sy+64,"MOBILITY",w.move-70,45,C_GREEN);
    if(secd==2){m3_text(sx,sy+82,cat("ROCKETS ",itoa(w.mag)),C_GRAY,1);m3_text(sx,sy+92,"SPLASH DMG",C_GRAY,1);}else{m3_text(sx,sy+82,cat("MAG ",itoa(w.mag)),C_GRAY,1);char rb[12]="RLD 0.0S";int t10=w.reload/6;rb[4]='0'+t10/10%10;rb[6]='0'+t10%10;m3_text(sx,sy+92,rb,C_GRAY,1);}}
   m3_ctext(146,lo_desc(r,e),C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+nr-1)%nr;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%nr;redraw=1;sfx_menu();}
  int dir=0;if(pressed&K_LEFT)dir=-1;if(pressed&K_RIGHT)dir=1;if((pressed&K_A)&&r!=R_DONE)dir=1;
  if(ctx==0&&(pressed&(K_L|K_R))){e=(e+((pressed&K_R)?1:4))%5;redraw=1;sfx_menu();}
  if(dir&&r!=R_DONE){int n=lo_optcount(r,e);if(r==R_ERA)e=(e+n+dir)%n;else{u8*f=lo_field(r,e);*f=(*f+n+dir)%n;}redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(r==R_DONE||(pressed&K_START))){save_write();lx_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();lx_write();sfx_menu();return 0;}}}
// ---------------------------------------------------------------- 6v6 ARSENAL menu (all eras in one pool)
// item lists: kind 0 prim 1 sec 2 launcher(non-none) 3 vehicle 4 aircraft 5 ship
static const u8*a6_cnt(int k){return k==0?NPRIM:k==1?NSEC:k==2?NL:k==3?NTK:k==4?NAIR:NSH;}
static int a6_exp(int k,int e,int i){if(e!=0)return 0;switch(k){case 0:return PRIM_ISEXP(i);case 2:return i+1==LAUN_EXP0;case 3:return TANKV[0][i].exp;case 4:return AIRV[0][i].exp;}return 0;}
static const char*a6_tag(int k,int e,int i){return a6_exp(k,e,i)?"WW2 EXPERIMENTAL":ERA_TAG[e];}
static int a6_tab(int k,int idx,int dir){ // jump to the next/previous era tab; WW2 experimental items form their own tab
 const u8*n=a6_cnt(k);int N=fsum(n);u8 st[8];int ns=0;
 for(int e=0;e<5;e++){if(!n[e])continue;st[ns++]=fenc(n,e,0);if(e==0)for(int i=1;i<n[0];i++)if(a6_exp(k,0,i)&&!a6_exp(k,0,i-1)){st[ns++]=i;break;}}
 int cur=0;for(int j=0;j<ns;j++)if(st[j]<=idx)cur=j;(void)N;return st[(cur+(dir>0?1:ns-1))%ns];}
static const char*a6_value(int r,int*era){int i,e;Loadout*l;
 switch(r){
  case R_PRIM:e=fdec(NPRIM,g6.pI,&i);*era=e;return PRIM[e][i].n;
  case R_SEC:e=fdec(NSEC,g6.sI,&i);*era=e;return SEC[e][i].n;
  case R_LAUN:if(!g6.lI){*era=-1;return "NONE";}e=fdec(NL,g6.lI-1,&i);*era=e;return LAUN[e][i+1].n;
  case R_LETH:*era=-1;return LETH6_N[g6.leth];case R_TACT:*era=-1;return TACT6_N[g6.tact];
  case R_TANK:e=fdec(NTK,g6.tkI,&i);*era=e;return TANKV[e][i].n;
  case R_AIR:e=fdec(NAIR,g6.aI,&i);*era=e;return AIRV[e][i].n;
  case R_SHIP:e=fdec(NSH,g6.shI,&i);*era=e;return SHIPV[e][i].n;}
 e=fdec(NPRIM,g6.pI,&i);l=&LO[e];*era=-1;
 if(rslot(r)>=0)return ATT_RN(r,e)->n;if(r==R_PERK)return PERKN[l->perk];return "";}
static void a6_step(int r,int dir,int tab){int i,e;u8*f=0;int k=-1;
 switch(r){case R_PRIM:f=&g6.pI;k=0;break;case R_SEC:f=&g6.sI;k=1;break;case R_TANK:f=&g6.tkI;k=3;break;case R_AIR:f=&g6.aI;k=4;break;case R_SHIP:f=&g6.shI;k=5;break;
  case R_LAUN:{int N=fsum(NL);if(tab){int cur=g6.lI?g6.lI-1:0;g6.lI=1+a6_tab(2,cur,dir);}else g6.lI=(g6.lI+N+1+dir)%(N+1);return;}
  case R_LETH:g6.leth=(g6.leth+NLETH6+dir)%NLETH6;return;case R_TACT:g6.tact=(g6.tact+NTACT6+dir)%NTACT6;return;}
 if(f){int n=fsum(a6_cnt(k));if(tab)*f=a6_tab(k,*f,dir);else *f=(*f+n+dir)%n;return;}
 e=fdec(NPRIM,g6.pI,&i);Loadout*l=&LO[e];
 if(rslot(r)>=0){int n=NATL[rslot(r)][e];u8*f=att_fld(e,rslot(r));*f=(*f+n+dir)%n;}
 if(r==R_PERK)l->perk=(l->perk+5+dir)%5;}
static void a6_build(Loadout*lp,Wstat*w,int slot){int pi,si,li=0;int pe=fdec(NPRIM,g6.pI,&pi),se=fdec(NSEC,g6.sI,&si);*lp=LO[pe];lp->prim=pi;
 int le=pe,laun=0;if(g6.lI){le=fdec(NL,g6.lI-1,&li);laun=li+1;}lp->laun=laun;
 if(slot==0)*w=wcalc(pe,lp,0);else if(slot==1){Loadout ls=LO[se];ls.sec=si;ls.perk=lp->perk;ls.laun=laun;*w=wcalc(se,&ls,1);}else{Loadout ll=LO[le];ll.laun=laun;ll.perk=lp->perk;*w=wcalc(le,&ll,2);}}
// draw a 32x32 4bpp OBJ sprite from ROM into the mode-3 framebuffer (faction-coloured preview)
static void m3_spr(const u32*t,int x0,int y0,int sc,int fac){u16 p[16];for(int i=0;i<16;i++)p[i]=obj_team_pal[i];const Fac*F=&FAC[fac];
 p[2]=fac_c(F->m,3,4);p[3]=fac_c(F->m,1,1);p[4]=fac_c(F->m,5,4);p[5]=fac_c(F->h,1,1);p[9]=fac_c(F->t,1,1);
 for(int ty=0;ty<4;ty++)for(int tx=0;tx<4;tx++)for(int y=0;y<8;y++){u32 w=t[(ty*4+tx)*8+y];for(int x=0;x<8;x++){int c=(w>>(x*4))&15;if(!c)continue;
   int X=x0+(tx*8+x)*sc,Y=y0+(ty*8+y)*sc;for(int a=0;a<sc;a++)for(int b=0;b<sc;b++)if(X+b<240&&Y+a<160)fb[(Y+a)*240+X+b]=p[c];}}}
static int arsenal_menu(int mode){u8 rows[18];int nr=0;
 if(mode==1){static const u8 RI[16]={R_PRIM,R_SEC,R_LAUN,R_OPT,R_MUZ,R_GRIP,R_MAG,R_BAR,R_LAS,R_STK,R_AMO,R_LETH,R_TACT,R_PERK,R_ARM,R_DONE};for(int k=0;k<16;k++)rows[nr++]=RI[k];}
 else{rows[nr++]=mode==2?R_TANK:mode==3?R_AIR:R_SHIP;rows[nr++]=R_DONE;}
 int sel=0,redraw=2;
 for(;;){int r=rows[sel];BEAT->screen=SCR_LOADOUT;BEAT->menu=r;BEAT->mode=mode;BEAT->extra=g6.pI|(g6.sI<<7)|(g6.lI<<11)|(g6.tkI<<15)|(g6.aI<<22);BEAT->phase=g6.shI;
  if(redraw){if(redraw==2)m3_title(1);redraw=0;m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);
   m3_ctext(8,mode==1?"6V6 ARSENAL - ALL ERAS":mode==2?"6V6 ARSENAL - ANY VEHICLE":mode==3?"6V6 ARSENAL - ANY AIRCRAFT":"6V6 ARSENAL - ANY WARSHIP",C_YEL,1);
   int y0=mode==1?20:24;
   int VIS=mode==1?11:nr,top=nr>VIS?clampi(sel-VIS/2,0,nr-VIS):0;if(top>0)m3_text(152,y0,"^",C_GRAY,1);if(top+VIS<nr)m3_text(152,y0+(VIS-1)*11,"v",C_GRAY,1);
   for(int k=top;k<nr&&k<top+VIS;k++){int q=rows[k];int y=y0+(k-top)*(mode==1?11:14)+(q==R_DONE?2:0);u16 c=k==sel?C_YEL:C_WHITE;
    if(q==R_DONE){m3_text(10,y,k==sel?"> ":"  ",C_YEL,1);m3_text(22,y,"DEPLOY",c,1);continue;}
    int e;const char*v=a6_value(q,&e);m3_text(7,y,RLAB[q],k==sel?C_YEL:C_GRAY,1);m3_text(63,y,trunc12(v),c,1);
    if(k==sel){m3_text(57,y,"<",C_YEL,1);m3_text(63+slen(trunc12(v))*6+1,y,">",C_YEL,1);}}
   int sx=162,sy=22,i;
   if(mode==2){int te=fdec(NTK,g6.tkI,&i);const TankS*t=&TANKV[te][i];int ex=a6_exp(3,te,i);
    m3_text(sx,sy-1,trunc12(t->n),C_BLUE,1);m3_text(sx,sy+8,a6_tag(3,te,i),ex?RGB(255,140,60):C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"ARMOR",t->hp,260,C_GREEN);m3_bar(sx,sy+16,"FIREPOWER",t->dmg,110,C_RED);m3_bar(sx,sy+32,"SPEED",t->spd,440,C_BLUE);m3_bar(sx,sy+48,"RELOAD",330-t->reload,330,C_YEL);m3_bar(sx,sy+64,"TURN",t->turn,500,C_WHITE);
    m3_text(10,58,VCLASS_N[t->cls],C_WHITE,1);m3_text(10,68,te==0?(t->axis?"AXIS-SIDE BOT POOL":"ALLIED-SIDE BOT POOL"):(t->axis?"EAST-SIDE BOT POOL":"WEST-SIDE BOT POOL"),C_GRAY,1);
    const char*cd=t->cls==VC_TDC||t->cls==VC_SPG?"FIXED GUN: AIM WITH HULL":t->cls==VC_IFV?"AUTOCANNON + B: ATGM, SEL: SQUAD":t->cls==VC_APC?(t->ammo2?"HMG, SEL: DISMOUNT SQUAD":"HMG, HUGE ARMOR, RAMS"):t->cls==VC_JEEP?"FAST, PINTLE MG":t->cls==VC_MINE?"A: DETONATE (SUICIDE CHARGE)":t->cls==VC_ARTY?"B: ARTILLERY CALL-IN":t->cls==VC_MLRS?"A: ROCKET  B: RIPPLE":t->cls==VC_SPAA?"TWIN CANNON + B: SAM":t->cls==VC_MRAP?(t->ammo2?"MG + B: TOW MISSILE":"MG, MINE-PROOF HULL"):t->ammo2?"B: ROCKET SALVO":t->dmg>=65?"HE SHELLS: SPLASH DAMAGE":"TURRET: L/R TRAVERSE";
    m3_text(10,78,cd,C_GRAY,1);
    m3_fill(40,92,72,52,RGB(70,120,60));m3_box(40,92,72,52,0);m3_spr(tank_spr+t->gs*256,44,86+10-8,2,g6.fac0);m3_spr(tank_spr+t->gs*256+128,44,86+10-8,2,g6.fac0);}
   else if(mode==3){int ae=fdec(NAIR,g6.aI,&i);const AirS*a=&AIRV[ae][i];int ex=a6_exp(4,ae,i);
    m3_text(sx,sy-1,trunc12(a->n),C_BLUE,1);m3_text(sx,sy+8,a6_tag(4,ae,i),ex?RGB(255,140,60):C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"SPEED",a->spd,760,C_BLUE);m3_bar(sx,sy+16,"TURN",a->turn,780,C_WHITE);m3_bar(sx,sy+32,"GUNS",a->dmg*60/a->rate,250,C_RED);m3_bar(sx,sy+48,"ARMOR",a->hp,360,C_GREEN);
    m3_bar(sx,sy+64,cat(a->sec==2?"BOMBS ":a->sec==3?"TORPEDO ":a->mturn?"MISSILES ":"ROCKETS ",itoa(a->ammo)),a->ammo,24,C_YEL);
    m3_text(10,58,ACLASS_N[a->cls],C_WHITE,1);m3_text(10,68,ae==0?(a->side?"AXIS-SIDE BOT POOL":"ALLIED-SIDE BOT POOL"):(a->side?"EAST-SIDE BOT POOL":"WEST-SIDE BOT POOL"),C_GRAY,1);
    m3_text(10,78,a->fuel?cat3("ROCKET BURN ",itoa(a->fuel),"S THEN GLIDE"):a->gunner==2?"DEFENSIVE TURRETS ALL ROUND":a->gunner?"REAR GUNNER":a->mn,C_GRAY,1);
    m3_fill(40,92,72,52,RGB(70,130,190));m3_box(40,92,72,52,0);m3_spr(plane_spr+a->gs*128,44,86+10-8,2,g6.fac0);}
   else if(mode==4){int se=fdec(NSH,g6.shI,&i);const ShipS*S=&SHIPV[se][i];
    m3_text(sx,sy-1,trunc12(S->n),C_BLUE,1);m3_text(sx,sy+8,ERA_TAG[se],C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"ARMOR",S->hp,700,C_GREEN);m3_bar(sx,sy+16,"MAIN GUNS",S->dmg,90,C_RED);m3_bar(sx,sy+32,"SPEED",S->spd,270,C_BLUE);m3_bar(sx,sy+48,"RANGE",S->sspd*S->life/256,700,C_YEL);m3_bar(sx,sy+64,"TURN",S->turn,270,C_WHITE);
    m3_text(10,58,SCLASS_N[S->cls],C_WHITE,1);m3_text(10,68,S->axis?"AXIS / EASTERN BOT POOL":"ALLIED / WESTERN BOT POOL",C_GRAY,1);
    m3_text(10,78,S->w2==0?"B: SECONDARY BATTERY":S->w2==1?cat3("B: TORPEDOES x",itoa(S->ammo2),""):S->w2==2?cat3("B: ANTI-SHIP MISSILES x",itoa(S->ammo2),""):cat("B: LAUNCH ",AIRV[S->we][S->wi].n),C_GRAY,1);
    m3_fill(40,92,72,52,RGB(50,110,170));m3_box(40,92,72,52,0);m3_spr(ship_spr+S->gs*128,60,86,1,g6.fac0);m3_spr(ship_spr+S->gs*128,44+4,94,2,g6.fac0);}
   else{int slot=r==R_SEC?1:(r==R_LAUN&&g6.lI)?2:0;Loadout lp;Wstat w;a6_build(&lp,&w,slot);int we,wi=0,kk=slot==1?1:slot==2?2:0;
    if(slot==1)we=fdec(NSEC,g6.sI,&wi);else if(slot==2)we=fdec(NL,g6.lI-1,&wi);else we=fdec(NPRIM,g6.pI,&wi);
    m3_text(sx,sy-1,trunc12(w.n),C_BLUE,1);m3_text(sx,sy+8,a6_tag(kk,we,wi),a6_exp(kk,we,wi)?RGB(255,140,60):C_GRAY,1);sy+=20;
    m3_bar(sx,sy,"DAMAGE",w.dmg*w.pellets,120,C_RED);m3_bar(sx,sy+16,"FIRE RATE",600/w.rate,200,C_YEL);m3_bar(sx,sy+32,"RANGE",w.spd16*w.life/16,420,C_BLUE);
    m3_bar(sx,sy+48,"RECOIL",w.spread+1,13,RGB(255,140,60));m3_bar(sx,sy+64,"MOBILITY",w.move-70,45,C_GREEN);}
   const char*d=r==R_PRIM?cat3("L/R ERA TAB  <> ",itoa(fsum(NPRIM))," PRIMARIES"):r==R_SEC?cat3("L/R ERA TAB  <> ",itoa(fsum(NSEC))," SIDEARMS"):r==R_LAUN?cat3("L/R ERA TAB  <> ",itoa(fsum(NL))," LAUNCHERS + NONE"):
    r==R_TANK?cat3("L/R ERA TAB  <> ",itoa(fsum(NTK))," VEHICLES"):r==R_AIR?cat3("L/R ERA TAB  <> ",itoa(fsum(NAIR))," AIRCRAFT"):r==R_SHIP?cat3("L/R ERA TAB  <> ",itoa(fsum(NSH))," WARSHIPS"):r==R_DONE?"SAVED TO SRAM - BOTS GET MIRRORED KITS":"ATTACHMENTS APPLY TO THE PRIMARY";
   m3_ctext(146,d,C_GRAY,1);m3_present();}
  next_frame();
  if(pressed&K_UP){sel=(sel+nr-1)%nr;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%nr;redraw=1;sfx_menu();}
  int dir=0;if(pressed&K_LEFT)dir=-1;if(pressed&K_RIGHT)dir=1;if((pressed&K_A)&&r!=R_DONE)dir=1;
  if(r!=R_DONE&&(pressed&(K_L|K_R))){a6_step(r,(pressed&K_R)?1:-1,1);redraw=1;sfx_menu();}
  else if(dir&&r!=R_DONE){a6_step(r,dir,0);redraw=1;sfx_menu();}
  if((pressed&(K_A|K_START))&&(r==R_DONE||(pressed&K_START))){save_write();sfx_ok();return 1;}
  if(pressed&K_B){save_write();sfx_menu();return 0;}}}
