// controls.c - native DSi button layout.  The shared GBA game code only knows the ten GBA keys
// (REG_KEYINPUT), so the DS build translates the physical DS buttons (A B X Y L R START SELECT,
// D-pad) into the GBA key bits each mode expects.  Preset 'DS' (default) gives every mode the full
// A/B/X/Y/L/R layout; 'CLASSIC (GBA)' is the untranslated v1.3.5 layout (X/Y unused).
// Two extra bits, K_DSX (1<<10) and K_DSY (1<<11), reach the DS-patched game code (tools/dscontrols.py)
// for actions that have no GBA key at all (manual reload, quick weapon swap, smoke, flares).
#include "platform.h"
#define GA 1
#define GB 2
#define GSEL 4
#define GST 8
#define GR 256
#define GL 512
#define GU 64
#define GD 128
#define DSX (1<<10)
#define DSY (1<<11)
int ds_ctl_preset;              // 0 = DS (default), 1 = CLASSIC (GBA)
extern volatile u32 ds_dbg[16];
enum{C_MENU,C_INF,C_CAMP,C_TUT,C_TANK,C_AIR,C_HOVER,C_SEA,C_FPS,C_OTHER,C_PAUSE,C_ARC,C_CAB,C_N};
int ds_arc_on; // 2 = arcade cabinet screen, 1 = a cabinet game is running
// physical button order: A B X Y L R SELECT START
typedef struct{u16 out[8];const char*lab[8];const char*name;}CMap;
static const CMap MAP[C_N]={
 {{GA,GB,GSEL,GST,GL,GR,GSEL,GST},{"OK","BACK","OPTION","START","TAB <","TAB >","OPTION","START"},"MENU"},
 {{GA,GSEL,DSX,DSY,GR,GB,GSEL,GST},{"FIRE","GEAR","RELOAD","SWAP","AIM","GRENADE","GEAR","PAUSE"},"INFANTRY"},
 {{GA,GSEL,GL,GL,GR,GB,GSEL,GST},{"FIRE","GEAR","SWAP","SWAP","AIM","GRENADE","GEAR","PAUSE"},"CAMPAIGN"},
 {{GA,GSEL,GSEL,GL,GR,GB,GSEL,GST},{"FIRE","USE","USE","SWAP","COVER","GRENADE","USE","SKIP"},"BOOT CAMP"},
 {{GA,GB,GSEL,DSY,GL,GR,GSEL,GST},{"MAINGUN","COAX","SQUAD","SMOKE","TURRET<","TURRET>","SQUAD","PAUSE"},"ARMOR"},
 {{GA,GB,GSEL,DSY,GL,GR,GSEL,GST},{"GUNS","MISSILE","SCORES","FLARES","ROLL <","ROLL >","SCORES","PAUSE"},"AIR"},
 {{GA,GB,GSEL|GU,GSEL|GD,GL,GR,GSEL,GST},{"GUNS","MISSILE","CLIMB","DESCEND","STRAFE<","STRAFE>","ALT","PAUSE"},"HOVER"},
 {{GA,GB,GSEL,DSY,GL,GR,GSEL,GST},{"MAIN","SECOND","CENTER","SMOKE","TURRET<","TURRET>","CENTER","PAUSE"},"NAVAL"},
 {{GA,GB,GB,GSEL,GL,GR,GSEL,GST},{"FIRE","TURN","RELOAD","SWAP","STRAFE<","STRAFE>","SWAP","QUIT"},"FPS"},
 {{GA,GB,GSEL,GSEL,GL,GR,GSEL,GST},{"A","B","ACTION","ACTION","L","R","ACTION","START"},"MODE"},
 {{GA,GB,0,0,GL,GR,GSEL,GST},{"A","B","-","-","L","R","QUIT","RESUME"},"PAUSE"},
 {{GA,GB,GA,GB,GL,GR,GSEL,GST},{"A","B","SAME AS A","SAME AS B","L","R","-","PAUSE"},"ARCADE"},
 {{GA,GB,GA,GB,GL,GR,GSEL,GST},{"PLAY","BACK","PLAY","BACK","PREV","NEXT","-","PLAY"},"CABINET"},
};
static const char*const CLASSIC_LAB[8]={"A","B","SELECT","-","L","R","SELECT","START"};
extern int ds_ctl_hover(void);
static int cat_of(int scr){
 if(ds_arc_on)return ds_arc_on==2&&scr==107?C_CAB:scr==108?C_ARC:scr==109||scr==101?C_MENU:C_ARC;
 switch(scr){case 2:case 3:case 11:case 12:case 13:case 14:case 15:case 16:case 18:case 53:case 54:case 55:case 56:case 57:case 105:return C_MENU;
  case 5:return C_INF;case 4:return C_CAMP;case 44:return C_TUT;case 6:return C_TANK;case 19:return C_SEA;case 84:return C_FPS;case 9:return C_PAUSE;
  case 7:return ds_ctl_hover()?C_HOVER:C_AIR;}
 return C_OTHER;}
int ds_ctl_testmode(void){return ds_dbg[12]==0xC1A55100u;} // harness: DSH_CTL=classic (GBA regression scripts)
static int classic(void){return ds_ctl_preset||ds_ctl_testmode();}
static u32 nX,nY;static u16 lastXY;
u16 ds_ctl_map(u16 hw,u16 xy){ // hw: GBA-layout pressed bits (active high); xy: bit0 X, bit1 Y
 int scr=DSBEAT->screen;
 if(ds_ctl_testmode()){ds_dbg[13]=hw;ds_dbg[15]=2;return hw;}
 if(ds_ctl_preset){u16 o=hw|((xy&1)?GSEL:0);ds_dbg[13]=o;ds_dbg[15]=1;return o;} // CLASSIC: GBA layout, X doubles as SELECT
 const CMap*M=&MAP[cat_of(scr)];u16 o=hw&0xF0;
 static const u16 PB[8]={GA,GB,0,0,GL,GR,GSEL,GST};
 for(int i=0;i<8;i++){int on=i==2?(xy&1):i==3?(xy&2):(hw&PB[i]);if(on)o|=M->out[i];}
 if((xy&1)&&!(lastXY&1)){nX++;}if((xy&2)&&!(lastXY&2)){nY++;}lastXY=xy;
 ds_dbg[13]=o|(cat_of(scr)<<12);ds_dbg[14]=(nX&0xFFFF)|(nY<<16);ds_dbg[15]=0;return o;}
// bottom-screen legend: n lines "BTN  ACTION"
int ds_ctl_legend(int scr,const char*btn[8],const char*lab[8],const char**name){
 static const char*const BN[8]={"A","B","X","Y","L","R","SEL","START"};
 for(int i=0;i<8;i++)btn[i]=BN[i];
 if(classic()){for(int i=0;i<8;i++)lab[i]=CLASSIC_LAB[i];*name="CLASSIC (GBA)";return 8;}
 const CMap*M=&MAP[cat_of(scr)];for(int i=0;i<8;i++)lab[i]=M->lab[i];*name=M->name;return 8;}
const char*ds_ctl_name(void){return ds_ctl_preset?"CLASSIC (GBA)":"DS  A/B/X/Y";}
// preset persistence: a 4-byte DS-only record at the very end of the SRAM image (unused by the game)
#define CTL_OFF 0xFFF0
void ds_ctl_load(void){u8*p=gba_sram+CTL_OFF;ds_ctl_preset=(p[0]=='D'&&p[1]=='C'&&p[2]=='T'&&p[3]<2)?p[3]:0;}
void ds_ctl_set(int v){ds_ctl_preset=v&1;u8*p=gba_sram+CTL_OFF;p[0]='D';p[1]='C';p[2]='T';p[3]=ds_ctl_preset;DC_FlushRange(p,32);}
