// ---------------------------------------------------------------- v1.1 campaign: mission select + data-driven missions
// Fictional protagonists inside real historical battles (story bible: /workspace/bible/campaign.py).
enum{OB_NONE,OB_KILL,OB_DESTROY,OB_GATHER,OB_REACH,OB_SURVIVE,OB_ESCORT,OB_SNIPER,OB_TREACH};
enum{ST_AA,ST_PILL,ST_CACHE,ST_CARRIER,ST_RADIO,ST_BLOCK};
#define MF_MORTAR 1
#define MF_ROE 2      // civilians present: harming 2 = mission failed
#define MF_CIVESC 4   // the ESCORT objective is civilians (else convoy vehicles)
#define MM_DOGGREEN 9
typedef struct{const char*name,*place,*date,*pov;u8 grp,mode,sEra,tEra,layout,theme,nAlly,nEnemy,flags,skind,ncv;
 u8 ob[3];s16 arg[3];const char*od[3];const char*b[4];}CMis;
#define NCM 27
enum{SH_FLETCHER=6,SH_TEXAS=10,SH_SBOOT=11}; // SHIPV[0] indices (tools/gen_vehicles.py)
static const CMis CM[NCM]={
 {"POINT LUCK","OFF MIDWAY ATOLL, PACIFIC","4 JUNE 1942","LT.(JG) FRANK DOYLE - SBD PILOT",0,MM_AIR,0,0,1,0,3,6,0,ST_CARRIER,0,
  {OB_KILL,OB_DESTROY,0},{4,2,0},{"BREAK THE FIGHTER SCREEN","DIVE-BOMB THE CARRIERS",""},
  {"YOUR SBD DAUNTLESS SQUADRON HAS FOUND","THE ENEMY CARRIER GROUP. ESCORTS WILL","TIE UP THE ZEROS - PUSH THROUGH AND","PUT YOUR BOMBS ON THE FLIGHT DECKS."}},
 {"STEEL WINTER","FACTORY DISTRICT, STALINGRAD","WINTER 1942","SGT. NADIA VOLKOVA - RED ARMY SNIPER",0,MM_INF,0,0,LY_TOWN,TH_SNOW,3,8,0,0,0,
  {OB_KILL,OB_SNIPER,OB_REACH},{6,1,3},{"CLEAR THE FACTORY BLOCK","FIND THE ENEMY SNIPER","REACH THE GRAIN ELEVATOR"},
  {"THE CITY IS RUBBLE AND ICE. AN ENEMY","MARKSMAN HAS PINNED YOUR SECTION FOR","TWO DAYS. CLEAR THE BLOCK, FIND HIM,","THEN PUSH ON TO THE GRAIN ELEVATOR."}},
 {"BLACK SKY","HEDGEROWS INLAND OF NORMANDY","6 JUNE 1944 - 01:30","PVT. THOMAS HALE - US PARATROOPER",0,MM_INF,0,0,LY_FIELD,TH_NIGHT,0,8,0,ST_AA,0,
  {OB_GATHER,OB_DESTROY,OB_KILL},{3,2,5},{"REGROUP SCATTERED TROOPERS","DESTROY THE FLAK GUNS","CLEAR THE CROSSROADS"},
  {"YOUR STICK JUMPED WIDE OF THE DROP","ZONE. FIND YOUR SCATTERED MEN IN THE","DARK, SILENCE THE FLAK GUNS, AND HOLD","THE CROSSROADS BEFORE THE BEACHES."}},
 {"DOG GREEN","OMAHA BEACH, NORMANDY","6 JUNE 1944 - 06:30","CPL. WALTER HALE - US ARMY INFANTRY",0,MM_DOGGREEN,0,0,0,0,0,0,0,0,0,
  {OB_NONE,0,0},{0,0,0},{"CROSS THE BEACH","BREACH THE WIRE","DESTROY THE BUNKER"},
  {"THE RAMP DROPS INTO MACHINE-GUN FIRE.","MOVE FROM OBSTACLE TO OBSTACLE, BLOW","THE WIRE WITH A BANGALORE AND TAKE","THE STRONGPOINT COVERING THE DRAW."}},
 {"FROZEN FOREST","CROSSROADS NEAR BASTOGNE","DECEMBER 1944","LT. THOMAS HALE - SHERMAN COMMANDER",0,MM_TANK,0,0,LY_FOREST,TH_SNOW,3,6,0,0,0,
  {OB_SURVIVE,OB_KILL,0},{75,6,0},{"HOLD THE CROSSROADS","BREAK THE ARMOURED PUSH",""},
  {"THE TOWN IS SURROUNDED AND THE FOG","GROUNDS OUR AIRCRAFT. ENEMY ARMOUR IS","COMING THROUGH THE PINES IN WAVES.","HOLD THE CROSSROADS AT ALL COSTS."}},
 {"SURIBACHI","IWO JIMA, VOLCANO ISLANDS","FEBRUARY 1945","PVT. DANIEL HALE - US MARINES",0,MM_INF,0,0,LY_FIELD,TH_DUSK,4,8,0,ST_PILL,0,
  {OB_REACH,OB_DESTROY,OB_SURVIVE},{20,3,45},{"GET OFF THE ASH TERRACES","KNOCK OUT THE PILLBOXES","HOLD THE LINE OVERNIGHT"},
  {"BLACK VOLCANIC SAND SWALLOWS EVERY","STEP. GET YOUR SQUAD OFF THE BEACH,","SILENCE THE PILLBOXES ABOVE YOU AND","DIG IN AGAINST NIGHT INFILTRATION."}},
 {"TIDAL WALL","INCHON HARBOUR, KOREA","15 SEPTEMBER 1950","SGT. DANIEL HALE - US MARINES",1,MM_INF,0,1,LY_TOWN,TH_DUSK,4,8,0,ST_RADIO,0,
  {OB_TREACH,OB_KILL,OB_DESTROY},{90,6,1},{"BEAT THE TIDE TO THE WALL","CLEAR THE WAREHOUSES","TAKE THE OBSERVATION POST"},
  {"THE EVENING TIDE GIVES US MINUTES,","NOT HOURS. GET OVER THE SEAWALL","BEFORE THE WATER DROPS, CLEAR THE","WAREHOUSES AND TAKE THE OP RADIO."}},
 {"FROZEN RESERVOIR","MOUNTAIN ROAD, CHOSIN","NOVEMBER 1950","CAPT. THOMAS HALE - US ARMY",1,MM_INF,0,1,LY_FOREST,TH_SNOW,3,9,0,0,0,
  {OB_ESCORT,OB_SURVIVE,0},{2,40,0},{"ESCORT THE CONVOY","HOLD THE PASS",""},
  {"IT IS THIRTY BELOW. THE TRUCKS CARRY","OUR WOUNDED. KEEP CLOSE - THE COLUMN","ONLY MOVES WHEN YOU ARE NEAR IT. AT","LEAST TWO TRUCKS MUST GET THROUGH."}},
 {"AIR CAVALRY","IA DRANG VALLEY, VIETNAM","NOVEMBER 1965","LT. MICHAEL HALE - US AIR CAVALRY",1,MM_INF,1,0,LY_FOREST,0,4,10,0,0,0,
  {OB_SURVIVE,OB_GATHER,OB_KILL},{75,2,8},{"DEFEND THE LANDING ZONE","REACH THE CUT-OFF SQUAD","PUSH BACK THE ASSAULT"},
  {"THE HELICOPTERS DROPPED US INTO TALL","ELEPHANT GRASS UNDER THE MOUNTAIN.","HOLD THE LZ, BRING BACK THE CUT-OFF","SQUAD AND BREAK THE ASSAULT."}},
 {"PERFUME RIVER","HUE CITY, VIETNAM","FEBRUARY 1968","LT. TRAN VAN MINH - ARVN OFFICER",1,MM_INF,1,0,LY_TOWN,TH_NIGHT,4,8,MF_ROE|MF_CIVESC,0,5,
  {OB_KILL,OB_ESCORT,OB_REACH},{6,4,3},{"CLEAR THE CITY BLOCK","LEAD CIVILIANS TO SAFETY","REACH THE CITADEL GATE"},
  {"RAIN AND SMOKE FILL THE STREETS. ","FAMILIES ARE TRAPPED IN THE FIGHTING.","LEAD THEM BACK TO OUR LINES - DO NOT","ENDANGER THEM - THEN TAKE THE GATE."}},
 {"LEFT HOOK","73 EASTING, SOUTHERN DESERT","26 FEBRUARY 1991","LT. ELENA HALE - ABRAMS PLATOON LEADER",2,MM_TANK,2,2,LY_FIELD,TH_DESERT,3,6,0,0,0,
  {OB_KILL,OB_REACH,0},{8,3,0},{"DESTROY THE DUG-IN ARMOUR","CREST THE RIDGE LINE",""},
  {"A SANDSTORM HIDES THE HORIZON. TRUST","YOUR THERMAL SIGHTS. AN ARMOURED","DEFENCE WAITS BEHIND THE LOW RISE -","BREAK IT AND TAKE THE RIDGE."}},
 {"RELIEF COLUMN","CITY STREETS, MOGADISHU","3-4 OCTOBER 1993","CAPT. ELENA HALE - UN CONVOY LIAISON",2,MM_INF,2,2,LY_TOWN,TH_NIGHT,3,8,MF_ROE,ST_BLOCK,4,
  {OB_DESTROY,OB_ESCORT,OB_SURVIVE},{2,2,40},{"CLEAR THE ROADBLOCKS","GUIDE THE RELIEF COLUMN","SECURE THE CRASH SITE"},
  {"SOLDIERS ARE CUT OFF AT A CRASH SITE.","THE MULTINATIONAL UN COLUMN NEEDS A","PATH. CLEAR THE ROADBLOCKS, PROTECT","CIVILIANS, AND GET THE COLUMN THROUGH."}},
 {"SHAH-I-KOT","SHAH-I-KOT VALLEY","MARCH 2002","CAPT. ELENA HALE - SPECIAL OPS LIAISON",2,MM_INF,2,2,LY_FOREST,TH_SNOW,3,9,MF_MORTAR,0,0,
  {OB_SURVIVE,OB_REACH,OB_SURVIVE},{50,3,35},{"HOLD THE BLOCKING POSITION","CLIMB TO THE CRASH SITE","HOLD UNTIL EXTRACTION"},
  {"THIN AIR, SNOW AND ROCK. MORTARS ARE","ZEROED ON THE VALLEY FLOOR - KEEP","MOVING. HOLD THE BLOCKING POSITION,","THEN CLIMB TO THE DOWNED CREW."}},
 {"PHANTOM FURY","CITY DISTRICT, FALLUJAH","NOVEMBER 2004","CAPT. ELENA HALE - ARMOUR COMMANDER",2,MM_INF,2,2,LY_TOWN,TH_NIGHT,3,9,MF_ROE|MF_CIVESC,ST_CACHE,3,
  {OB_KILL,OB_ESCORT,OB_DESTROY},{8,3,1},{"CLEAR THE DISTRICT","EVACUATE THE FAMILY","SEIZE THE WEAPONS CACHE"},
  {"STACK, BREACH, CLEAR, MARK, MOVE ON.","A FAMILY IS SHELTERING IN A CELLAR.","FIND THEM AND WALK THEM OUT SAFELY.","RULES OF ENGAGEMENT ARE STRICT."}},
 {"FULDA GAP (WHAT-IF)","FULDA GAP, WEST GERMANY","1985 - FICTIONAL","LT. ANNA HALE - M60A3 PLATOON LEADER",3,MM_TANK,1,1,LY_FOREST,TH_DUSK,3,6,0,0,0,
  {OB_SURVIVE,OB_KILL,0},{70,6,0},{"HOLD THE GAP","STOP THE FIRST ECHELON",""},
  {"A FICTIONAL WAR THAT NEVER HAPPENED.","WARSAW PACT ARMOUR IS POURING INTO","THE GAP. HOLD THE TREELINE UNTIL THE","REINFORCEMENTS CROSS THE RHINE."}},
 {"GUNS OF TEXAS","OMAHA BEACH / POINTE DU HOC, NORMANDY","6 JUNE 1944","LT.CDR RUTH HALE - USS TEXAS GUNNERY",0,MM_SEA,0,0,4,TH_DUSK,2,4,0,ST_PILL,0,
  {OB_DESTROY,OB_KILL,0},{4,3,0},{"SILENCE SHORE GUNS","DRIVE OFF S-BOATS",""},
  {"D-DAY. THE RANGERS ARE CLIMBING","POINTE DU HOC AND OMAHA IS PINNED.","CLOSE THE BEACH AND PUT TEN 14-INCH","GUNS ON THE BLUFFS. CLEAR THE SKY."}},
 {"TORA BORA","WHITE MOUNTAINS, AFGHANISTAN","DECEMBER 2001","SGT. RAFAEL ORTIZ - COALITION ADVISOR",2,MM_INF,2,2,LY_FOREST,TH_SNOW,3,9,MF_MORTAR,ST_CACHE,0,
  {OB_REACH,OB_DESTROY,OB_KILL},{3,2,7},{"CLIMB TO THE CAVE LINE","DESTROY THE SUPPLY CACHES","CLEAR THE RIDGE"},
  {"THE CAVES SIT HIGH IN FROZEN ROCK.","MORTAR CREWS WATCH EVERY GULLY.","CLIMB FAST, BURN THE SUPPLY CACHES","AND CLEAR THE RIDGE BEFORE DARK."}},
 {"MEDUSA","PANJWAYI DISTRICT, KANDAHAR","SEPTEMBER 2006","CPL. MAYA LEROUX - NATO INFANTRY",2,MM_INF,2,2,LY_RIVER,TH_DESERT,4,9,0,ST_PILL,0,
  {OB_REACH,OB_DESTROY,OB_SURVIVE},{3,3,40},{"CROSS THE DRY RIVERBED","KNOCK OUT THE STRONGPOINTS","HOLD THE WHITE SCHOOL"},
  {"GRAPE FIELDS AND MUD WALLS HIDE A","FORTIFIED LINE. CROSS THE RIVERBED,","SILENCE THE STRONGPOINTS AND HOLD","THE SCHOOL COMPOUND UNTIL RELIEVED."}},
 {"MOSHTARAK","MARJAH, HELMAND PROVINCE","FEBRUARY 2010","LCPL. OWEN PRICE - US MARINES",2,MM_INF,2,2,LY_FIELD,TH_NONE,4,8,MF_ROE,ST_BLOCK,3,
  {OB_DESTROY,OB_KILL,OB_REACH},{2,7,3},{"CLEAR THE IED ROADBLOCKS","DRIVE OUT THE AMBUSHERS","REACH THE BAZAAR"},
  {"HELICOPTERS PUT US DOWN AMID THE","CANALS AT NIGHT. FARMERS ARE STILL","IN THE FIELDS - WATCH YOUR FIRE.","CLEAR THE ROAD AND SECURE THE BAZAAR."}},
 {"HOSTOMEL","ANTONOV AIRPORT, KYIV OBLAST","24 FEBRUARY 2022","SGT. OKSANA HRYN - NATIONAL GUARD",2,MM_INF,2,2,LY_FIELD,TH_DUSK,4,10,0,ST_AA,0,
  {OB_SURVIVE,OB_DESTROY,OB_KILL},{60,2,8},{"HOLD THE RUNWAY EDGE","DESTROY THE LANDING GEAR","RETAKE THE HANGARS"},
  {"HELICOPTERS ARE COMING LOW OVER THE","RESERVOIR. HOLD THE RUNWAY EDGE,","WRECK THEIR EQUIPMENT ON THE APRON","AND DENY THEM THE AIRFIELD."}},
 {"IRPIN BRIDGE","IRPIN RIVER CROSSING","MARCH 2022","PVT. TARAS BILYK - TERRITORIAL DEFENSE",2,MM_INF,2,2,LY_RIVER,TH_SNOW,4,8,MF_ROE|MF_CIVESC,0,5,
  {OB_SURVIVE,OB_ESCORT,OB_KILL},{45,4,6},{"HOLD THE CROSSING","GUIDE FAMILIES ACROSS","PUSH BACK THE ASSAULT"},
  {"THE BRIDGE IS DOWN. FAMILIES ARE","CROSSING ON PLANKS UNDER THE SPAN.","HOLD THE BANK, GUIDE THEM TO THE","BUSES - EVERY LIFE COUNTS TODAY."}},
 {"OLD CITY","WEST MOSUL, IRAQ","JUNE 2017","CAPT. LAYLA HASSAN - IRAQI CTS",2,MM_INF,2,2,LY_TOWN,TH_DESERT,4,9,MF_ROE|MF_CIVESC,ST_CACHE,4,
  {OB_KILL,OB_ESCORT,OB_DESTROY},{8,3,2},{"CLEAR THE ALLEYS","EVACUATE THE TRAPPED","SEIZE THE WEAPON CACHES"},
  {"THE ALLEYS ARE TOO NARROW FOR ARMOUR.","CIVILIANS SHELTER IN EVERY BASEMENT.","CLEAR HOUSE BY HOUSE, LEAD THE","FAMILIES OUT AND SEIZE THE CACHES."}},
 {"LAKE CITY","MARAWI, MINDANAO","JULY 2017","SGT. DANILO REYES - PHILIPPINE MARINES",2,MM_INF,2,2,LY_TOWN,TH_NONE,4,9,MF_ROE,ST_PILL,3,
  {OB_KILL,OB_DESTROY,OB_SURVIVE},{8,3,40},{"CLEAR THE MARKET BLOCK","KNOCK OUT THE SNIPER NESTS","HOLD THE BRIDGE"},
  {"MONSOON RAIN AND RUINED STREETS.","SNIPERS HOLD THE TALL BUILDINGS BY","THE LAKE. CLEAR THE BLOCK, SILENCE","THE NESTS AND HOLD THE BRIDGE."}},
 {"KHARKIV DASH","BALAKLIIA AXIS, KHARKIV OBLAST","SEPTEMBER 2022","LT. ROMAN KOVAL - TANK PLATOON",2,MM_TANK,2,2,LY_FIELD,TH_NONE,3,7,0,0,0,
  {OB_KILL,OB_REACH,0},{8,3,0},{"BREAK THE FORWARD LINE","REACH THE OSKIL RIVER",""},
  {"SUNFLOWER FIELDS, DRY ROADS AND A","THIN ENEMY SCREEN. HIT IT HARD,","DO NOT STOP, AND RACE FOR THE RIVER","BEFORE THEIR RESERVES ARRIVE."}},
 {"LAST FLIGHTS","HAMID KARZAI AIRPORT, KABUL","AUGUST 2021","SSGT. NORA BLAKE - EVACUATION TEAM",2,MM_INF,2,2,LY_TOWN,TH_DUSK,5,6,MF_ROE|MF_CIVESC,0,6,
  {OB_ESCORT,OB_SURVIVE,OB_REACH},{5,45,3},{"ESCORT EVACUEES TO GATE","HOLD THE PERIMETER","REACH THE LAST AIRCRAFT"},
  {"THE AIRPORT IS THE LAST WAY OUT. THE","CROWDS ARE DESPERATE, NOT HOSTILE -","WATCH YOUR FIRE. GET THE FAMILIES","TO THE AIRCRAFT AND HOLD THE GATE."}},
 {"BLACK SEA SKIES","OVER THE NORTHWEST BLACK SEA","SPRING 2022","MAJ. ILKO SHEVCHUK - FIGHTER PILOT",2,MM_AIR,2,2,1,0,3,6,0,0,0,
  {OB_SURVIVE,OB_KILL,0},{50,5,0},{"PROTECT THE CITY","BREAK THE STRIKE PACKAGE",""},
  {"CRUISE MISSILES AND JETS ARE INBOUND","FROM THE SEA. YOU ARE OUTNUMBERED","AND LOW ON FUEL. FLY LOW, STRIKE","FAST AND KEEP THE CITY SKIES OPEN."}},
 {"IRON CURTAIN 2035 (WHAT-IF)","SUWALKI GAP - FICTIONAL","2035 - FICTIONAL","CAPT. ANNA HALE - NATO DRONE ARMOUR",3,MM_TANK,3,3,LY_FOREST,TH_NIGHT,3,7,0,0,0,
  {OB_SURVIVE,OB_KILL,0},{70,7,0},{"HOLD THE CORRIDOR","STOP THE ROBOT VANGUARD",""},
  {"A FICTIONAL FUTURE CONFLICT. DRONE","SWARMS AND ROBOT TANKS ARE COMING","THROUGH THE CORRIDOR IN THE DARK.","HOLD UNTIL THE ALLIES ARRIVE."}}};
static const char*const GRPN[4]={"WORLD WAR II","COLD WAR","MODERN","WHAT-IF"};
static const CMis*cm;static int msGath,cmIdx,msObi,msNob,msT,msObT,msCount,msCivLost,msCivPlayer,msSaved,msE0,msX0,msXN,msCPx,msCPy,msGx,msGy,msReady,msMortT;
static MapDef msMapDef;static u8 msAlt;
static int ms_obj(void){return msObi<msNob?cm->ob[msObi]:OB_NONE;}
static int ms_resp_ok(void){int o=ms_obj();return o==OB_KILL||o==OB_SURVIVE||o==OB_SNIPER||mpMode==MM_AIR;}
static int ms_free_pos(Unit*u,int y0,int y1){int W=worldPx();for(int k=0;k<60;k++){u->x=(32+rr(W-64))<<8;u->y=(y0+rr(y1-y0))<<8;if(mpMode==MM_AIR||box_free(u->x,u->y,8))return 1;}return 0;}
static void ms_goal(void){int o=ms_obj();int W=worldPx(),Hp=mapH*16;msGx=W/2;msGy=3*16;
 if(o==OB_REACH)msGy=cm->arg[msObi]*16+8;
 if(o==OB_TREACH)msGy=(mapH/2)*16;
 if(o==OB_ESCORT&&(cm->flags&MF_CIVESC)){msGy=Hp-3*16;}
 // make sure the goal is reachable ground
 for(int k=0;k<40&&!box_free(msGx<<8,msGy<<8,6);k++)msGx+=(k&1)?-(k*8):(k*8);}
static void ms_begin_obj(void){msObT=0;msCount=0;msSaved=0;ms_goal();
 if(ms_resp_ok())for(int i=msE0;i<msX0;i++)if(!U[i].alive&&U[i].resp>120)U[i].resp=60+rr(60);
 if(ms_obj()==OB_SURVIVE||ms_obj()==OB_TREACH)msObT=cm->arg[msObi]*60;}
static void ms_place_extras(void){int W=worldPx(),Hp=mapH*16;(void)W;
 for(int i=msX0;i<msXN;i++){Unit*u=&U[i];u->alive=1;u->inv=0;u->resp=30000;u->tgt=-1;u->cd=30;
  switch(u->role){
   case R_STATIC:u->team=1;u->hp=u->maxhp=u->kind==ST_CARRIER?210:u->kind==ST_PILL?200:u->kind==ST_BLOCK?140:110;
    if(mpMode==MM_AIR){u->x=((W/2-220+(i-msX0)*440+rr(60))&(W-1))<<8;u->y=(160+rr(80))<<8;u->ang=0x4000*(rr(2)?1:3);u->vel=0;}
    else if(mpMode==MM_SEA){for(int k=0;k<80;k++){int tx=4+rr(56);if(k<40&&i>msX0&&iabs(tx-((U[i-1].x>>8)>>4))<6)continue;int ty=44;while(ty>12&&map[ty*MAXW+tx]<2)ty--;if(ty>14&&ty<40){u->x=(tx*16+8)<<8;u->y=((ty-1)*16+8)<<8;break;}}u->tur=0x8000;}
    else ms_free_pos(u,4*16,Hp/2);break;
   case R_PARA:u->team=0;u->hp=u->maxhp=100;ms_free_pos(u,Hp/4,Hp-6*16);u->wpn=ALLY_PRIM[rr(21)];break;
   case R_SNIPER:u->team=1;u->hp=u->maxhp=60;ms_free_pos(u,4*16,Hp/3);break;
   case R_CIV:u->team=2;u->hp=u->maxhp=60;u->mode=0;
    if(cm->flags&MF_CIVESC){if(i==msX0||!ms_free_pos(u,(U[msX0].y>>8)-24,(U[msX0].y>>8)+24))ms_free_pos(u,Hp/4,Hp/2);}
    else ms_free_pos(u,Hp/4,Hp-6*16);break;
   case R_ESCORT:u->team=0;u->hp=u->maxhp=260;u->x=(W/2-40+(i-msX0)*40)<<8;u->y=(Hp-3*16)<<8;u->ang=0;u->mode=0;
    for(int k=0;k<20&&!box_free(u->x,u->y,8);k++)u->y-=8<<8;break;}}}
static void ms_init_fn(void){Unit*P=&U[0];int ne=cm->nEnemy,na=cm->nAlly;
 msE0=1+na;msX0=msE0+ne;int n=msX0;
 for(int i=1;i<msX0;i++){Unit*u=&U[i];u->team=i<msE0?0:1;u->role=0;u->think=i;
  if(mpMode==MM_TANK)u->veh=(u->team^msAlt)?1:0;
  else if(mpMode==MM_AIR)u->veh=msAlt?(u->team?((i&1)?13:2):11):(u->team?11:2);
  else if(mpEra==0&&msAlt)u->wpn=u->team?ALLY_PRIM[rr(21)]:AXIS_PRIM[rr(11)];
  else if(mpEra==0)u->wpn=u->team?AXIS_PRIM[rr(11)]:ALLY_PRIM[rr(21)];
  else u->wpn=mpEra==0?(u->team?AXIS_PRIM[rr(11)]:ALLY_PRIM[rr(21)]):rr(NPRIM_BOT[mpEra]);
  if(mpMode==MM_SEA){u->ve=0;u->veh=u->team?SH_SBOOT:SH_FLETCHER;}
  spawn(u,i);}
 for(int i=msX0;i<MAXU;i++)memset(&U[i],0,sizeof(Unit));
 for(int k=0;k<msNob;k++){int o=cm->ob[k],a=cm->arg[k];
  if(o==OB_DESTROY)for(int j=0;j<a&&n<MAXU;j++){U[n].role=R_STATIC;U[n].kind=cm->skind;n++;}
  if(o==OB_GATHER)for(int j=0;j<a&&n<MAXU;j++){U[n].role=R_PARA;n++;}
  if(o==OB_SNIPER&&n<MAXU){U[n].role=R_SNIPER;U[n].wpn=0;n++;}
  if(o==OB_ESCORT&&!(cm->flags&MF_CIVESC))for(int j=0;j<3&&n<MAXU;j++){U[n].role=R_ESCORT;n++;}}
 for(int j=0;j<cm->ncv&&n<MAXU;j++){U[n].role=R_CIV;n++;}
 msXN=n;nU=n;ms_place_extras();
 if(mpMode==MM_TANK){P->veh=msAlt?1:tankSel[mpEra];P->hp=P->maxhp=TV(P)->hp;}
 if(mpMode==MM_AIR){P->veh=msAlt?11:13;P->hp=P->maxhp=AV(P)->hp;P->ammo=AV(P)->ammo;P->vel=AV(P)->spd;}
 if(mpMode==MM_SEA){P->ve=0;P->veh=SH_TEXAS;P->hp=P->maxhp=SV(P)->hp;P->ammo=SV(P)->ammo2;P->y=(44*16)<<8;for(int i=1;i<msE0;i++){U[i].y=P->y+((10+i*30)<<8);U[i].x=P->x+(((i&1)?-60:60)<<8);}for(int i=msE0;i<msX0;i++){U[i].y=(60*16)<<8;U[i].ang=0;}vsNext=VS_BASE;vsN=0;for(int i=0;i<msX0;i++)U[i].vt=vslot(2,SV(&U[i])->gs);}
 if(msAlt){for(int c=2;c<=5;c++){u16 t=PAL_OBJ[c];PAL_OBJ[c]=PAL_OBJ[16+c];PAL_OBJ[16+c]=t;PAL_OBJ[32+c]=PAL_OBJ[c];}}
 // civilian palette (bank 5): desaturated warm clothing
 for(int c=1;c<16;c++){u16 v=PAL_OBJ[c];int r=v&31,g=(v>>5)&31,b=(v>>10)&31,l=(r*3+g*5+b*2)/10;PAL_OBJ[5*16+c]=clampi(l+5,0,31)|(clampi(l+1,0,31)<<5)|(clampi(l-4,0,31)<<10);}
 msObi=0;msT=0;msCivLost=0;msCivPlayer=0;msGath=0;msMortT=200;msCPx=P->x;msCPy=P->y;msReady=1;ms_begin_obj();
 feed(cat3(cm->name," - ",cm->od[0]),HB_YEL);}
static void ms_spawn_fn(Unit*u,int i){if(!msReady)return;
 if(i==0){u->x=msCPx;u->y=msCPy;if(mpMode==MM_AIR){u->veh=msAlt?11:13;u->hp=u->maxhp=AV(u)->hp;u->ammo=AV(u)->ammo;u->vel=AV(u)->spd;}}}
static void ms_death_fn(int vi,int ai){Unit*v=&U[vi];
 if(v->role==R_CIV){msCivLost++;if(ai==0){msCivPlayer++;feed("CIVILIAN HIT - HOLD YOUR FIRE!",HB_RED);}else feed("A CIVILIAN WAS LOST",HB_RED);v->resp=30000;return;}
 if(v->role==R_ESCORT){feed("CONVOY VEHICLE LOST",HB_RED);v->resp=30000;return;}
 if(v->role==R_STATIC){v->resp=30000;if(ms_obj()==OB_DESTROY)feed(v->kind==ST_CARRIER?"DIRECT HIT - CARRIER SINKING":"TARGET DESTROYED",HB_GREEN);return;}
 if(v->role==R_SNIPER){v->resp=30000;feed("ENEMY SNIPER DOWN",HB_GREEN);msCount++;return;}
 if(v->role==R_PARA){v->resp=30000;return;}
 if(v->team==1){if(ms_obj()==OB_KILL)msCount++;v->resp=ms_resp_ok()?150+rr(90):30000;}
 else if(vi!=0)v->resp=360;}
static void ms_walk(Unit*u,int tx,int ty,int sp){int dx=tx-(u->x>>8),dy=ty-(u->y>>8);if(idist(dx,dy)<4)return;int a=atan2i(dx,dy);int b=steer(u,a,12,6);
 if(!move_unit(u,SIN(b)*sp>>8,-COS(b)*sp>>8,6)){u->stuck++;if(u->stuck>40){u->x+=SIN(a)*sp>>8;u->y-=COS(a)*sp>>8;}}else u->stuck=0;u->ang=u->tur=b<<8;u->anim++;}
static int ms_role_fn(int i){Unit*u=&U[i];Unit*P=&U[0];int dp=P->alive?idist(dxw(P->x,u->x),dyw(P->y,u->y)):9999;
 switch(u->role){
 case R_STATIC:{if(u->kind==ST_CACHE||u->kind==ST_RADIO||u->kind==ST_BLOCK)return 1;
  if(u->kind==ST_AA)u->tur+=0x180;
  int rng=u->kind==ST_CARRIER?150:u->kind==ST_AA?110:150;
  if(u->cd==0){int t=-1,bd=rng;for(int k=0;k<msX0;k++){Unit*e=&U[k];if(!e->alive||e->team!=0)continue;int d=idist(dxw(e->x,u->x),dyw(e->y,u->y));if(d<bd&&(mpMode==MM_AIR||los(u->x>>8,u->y>>8,e->x>>8,e->y>>8))){bd=d;t=k;}}
   if(t>=0){int aim=atan2i(dxw(U[t].x,u->x),dyw(U[t].y,u->y));if(u->kind==ST_PILL)u->tur=aim<<8;
    fire_bullet(i,aim+rr(11)-5,mpMode==MM_AIR?1100:1150,mpMode==MM_AIR?4:6,30,K_BUL,0);u->cd=u->kind==ST_PILL?9:16;if(dp<160)sfx_shot(0);}}
  return 1;}
 case R_PARA:u->tur+=(frame&63)==0?0x2000:0;if(dp<30){u->role=0;u->tgt=-1;u->think=0;msGath++;feed(cat3("TROOPER JOINED (",itoa(msGath),")"),HB_GREEN);sfx_ok();}return 1;
 case R_SNIPER:{if(u->cd==0&&P->alive&&dp<230&&los(u->x>>8,u->y>>8,P->x>>8,P->y>>8)){int aim=atan2i(dxw(P->x,u->x),dyw(P->y,u->y));u->tur=aim<<8;
   fire_bullet(i,aim+rr(3)-1,2600,40,28,K_BUL,0);u->cd=120;u->hurt=0;u->mode=40;sfx_shot(1);fx_add((u->x>>8)+(SIN(aim)*11>>8),(u->y>>8)-(COS(aim)*11>>8),FXK_FLASH);if(rr(2))feed("SNIPER FIRE! LOOK FOR THE MUZZLE FLASH",HB_RED);}
  if(u->mode>0)u->mode--;return 1;}
 case R_CIV:{int esc=ms_obj()==OB_ESCORT&&(cm->flags&MF_CIVESC);
  if(esc&&dp<44)u->mode=2;
  if(u->mode==2&&esc){if(dp>22&&P->alive)ms_walk(u,P->x>>8,P->y>>8,230);
   if(idist(msGx-(u->x>>8),msGy-(u->y>>8))<30){u->role=R_SAVED;u->alive=0;u->resp=30000;msSaved++;feed(cat3("CIVILIAN SAFE (",itoa(msSaved),")"),HB_GREEN);sfx_ok();}}
  else if((frame+i*7)%90<20)u->tur+=0x800;
  return 1;}
 case R_ESCORT:{if(ms_obj()!=OB_ESCORT)return 1;
  if(dp<160){int sp=150;int dx=msGx-(u->x>>8)+(i-msX0-1)*24,dy=msGy-(u->y>>8);if(idist(dx,dy)>6){int a=atan2i(dx,dy);int b=steer(u,a,16,9);
     if(!move_unit(u,SIN(b)*sp>>8,-COS(b)*sp>>8,9)){u->x+=SIN(a)*sp>>8;u->y-=COS(a)*sp>>8;}turn_to(&u->ang,b,0x300);u->anim++;}}
  if(idist(msGx-(u->x>>8),msGy-(u->y>>8))<40){u->role=R_SAVED;u->alive=0;u->resp=30000;msSaved++;feed(cat3("VEHICLE THROUGH (",itoa(msSaved),")"),HB_GREEN);sfx_ok();}
  return 1;}}
 return 0;}
static int ms_draw_fn(int k,int x,int y){Unit*u=&U[k];int act=0;
 switch(u->role){
 case R_STATIC:act=ms_obj()==OB_DESTROY;
  if(u->kind==ST_CARRIER){spr_aff(x,y,OT_ERA+ES_SHIP0,2,u->hurt?3:1,u->ang>>8,0x80,1,0);}
  else if(u->kind==ST_PILL){spr_aff(x,y,OT_ERA+ES_TUR,2,1,u->tur>>8,0x100,1,0);spr_aff(x,y,OT_ERA+ES_HULL,2,1,0,0xC0,1,0);}
  else if(u->kind==ST_AA){spr_aff(x,y,OT_ERA+ES_TUR,2,1,u->tur>>8,0x100,1,0);spr(x-8,y-8,OT_FX+FX_CRATE,0,1,3,1);}
  else{spr(x-8,y-8,OT_FX+FX_CRATE,0,1,3,1);if(u->kind==ST_BLOCK){spr(x-18,y-6,OT_FX+FX_CRATE,0,1,3,1);spr(x+2,y-6,OT_FX+FX_CRATE,0,1,3,1);}}
  if(act&&(frame&16))spr(x-8,y-8-(u->kind==ST_CARRIER?0:0),OT_FX+FX_BRK_E,0,1,3,0);return 1;
 case R_PARA:spr_aff(x,y,OT_ERA+ES_SOLDIER,1,0,u->tur>>8,0x100,1,0);if(frame&16)spr(x-8,y-20,OT_FX+FX_IFF,0,1,3,0);return 1;
 case R_CIV:spr_aff(x,y,OT_ERA+ES_SOLDIER+((u->anim>>3)&1)*4,1,5,u->tur>>8,0x100,1,0);if(ms_obj()==OB_ESCORT&&(cm->flags&MF_CIVESC)&&u->mode!=2&&(frame&16))spr(x-8,y-20,OT_FX+FX_BRK_C,0,1,3,0);return 1;
 case R_ESCORT:spr_aff(x,y,OT_ERA+ES_HULL,2,0,u->ang>>8,0x100,1,0);if(ms_obj()==OB_ESCORT&&(frame&16))spr(x-8,y-24,OT_FX+FX_IFF,0,1,3,0);return 1;
 case R_SNIPER:{int dp=idist(dxw(U[0].x,u->x),dyw(U[0].y,u->y));if(dp<70||u->mode>0)spr_aff(x,y,OT_ERA+ES_SOLDIER,1,1,u->tur>>8,0x100,1,0);
  if(u->mode>0&&(frame&4))spr(x-8,y-8,OT_FX+FX_BRK_E,0,1,3,0);return 1;}}
 return 0;}
static void ms_skip_fn(void){msObi=msNob;}
static int ms_left(void){int o=ms_obj(),a=cm->arg[msObi],n=0;
 switch(o){
  case OB_KILL:return a-msCount;
  case OB_DESTROY:for(int i=msX0;i<msXN;i++)if(U[i].role==R_STATIC&&U[i].alive)n++;return n;
  case OB_GATHER:return a-msGath;
  case OB_SNIPER:for(int i=msX0;i<msXN;i++)if(U[i].role==R_SNIPER&&U[i].alive)n++;return n;
  case OB_ESCORT:return a-msSaved;
  case OB_SURVIVE:return msObT>0?1:0;
  case OB_REACH:case OB_TREACH:{Unit*P=&U[0];return P->alive&&idist(msGx-(P->x>>8),msGy-(P->y>>8))<28?0:1;}}
 return 0;}
static int ms_tick_fn(void){msT++;BEAT->phase=100+msObi;BEAT->menu=msCount|(msObT/60<<8);
 if(msObi>=msNob)return 1;
 int o=ms_obj();
 if((o==OB_SURVIVE||o==OB_TREACH)&&msObT>0)msObT--;
 if(o==OB_TREACH&&msObT==0){feed("THE TIDE WENT OUT",HB_RED);return 2;}
 if((cm->flags&MF_ROE)&&msCivPlayer>=2)return 2;
 if(o==OB_ESCORT){int alive=0;for(int i=msX0;i<msXN;i++)if(U[i].role==((cm->flags&MF_CIVESC)?R_CIV:R_ESCORT)&&U[i].alive)alive++;if(alive+msSaved<cm->arg[msObi])return 2;}
 if((cm->flags&MF_MORTAR)&&(o==OB_SURVIVE)&&U[0].alive&&--msMortT<=0){msMortT=90+rr(60);Shot*s=shot_new();
  if(s){s->x=U[0].x+((rr(97)-48)<<8);s->y=U[0].y+((rr(97)-48)<<8);s->vx=s->vy=0;s->life=55;s->kind=K_GREN;s->sub=0;s->team=1;s->owner=msE0;feed("INCOMING MORTARS - MOVE!",HB_RED);}}
 if(ms_left()<=0){msObi++;Unit*P=&U[0];if(P->alive){msCPx=P->x;msCPy=P->y;}
  if(msObi>=msNob)return 1;
  sfx_ok();feed(cat3("OBJECTIVE COMPLETE - ",cm->od[msObi],""),HB_GREEN);ms_begin_obj();}
 return 0;}
static void ms_hud_fn(void){int o=ms_obj();
 if(msObi>=msNob){hctxt(0,"ALL OBJECTIVES COMPLETE",HB_GREEN,0);return;}
 const char*od=cm->od[msObi];htxt(0,0,cat(itoa(msObi+1),"/"),HB_YEL);htxt(2,0,itoa(msNob),HB_YEL);
 const char*pr="";int a=cm->arg[msObi];
 if(o==OB_KILL)pr=cat3(itoa(msCount),"/",itoa(a));else if(o==OB_GATHER)pr=cat3(itoa(msGath),"/",itoa(a));else if(o==OB_ESCORT)pr=cat3(itoa(msSaved),"/",itoa(a));
 else if(o==OB_DESTROY)pr=cat("LEFT ",itoa(ms_left()));else if(o==OB_SURVIVE||o==OB_TREACH)pr=mmss(msObT);
 htxt(4,0,od,HB_WHITE);
 if(slen(od)+4+slen(pr)+1>30)htxt(30-slen(pr),1,pr,o==OB_TREACH?HB_RED:HB_YEL);else htxt(30-slen(pr),0,pr,o==OB_TREACH?HB_RED:HB_YEL);
 if(cm->flags&MF_ROE)htxt(0,1,cat("CIV LOST ",itoa(msCivLost)),msCivPlayer?HB_RED:HB_GRAY);
 // objective waypoint marker + arrow
 if(o==OB_REACH||o==OB_TREACH||(o==OB_ESCORT)){int x=scrx(msGx<<8),y=scry(msGy<<8);if(frame&16)spr(x-8,y-8,OT_FX+FX_BRK_C,0,1,3,0);
  Unit*P=&U[0];int dx=dxw(msGx<<8,P->x),dy=dyw(msGy<<8,P->y);if(idist(dx,dy)>60&&P->alive){int a=atan2i(dx,dy);spr_aff(scrx(P->x)+(SIN(a)*26>>8),scry(P->y)-(COS(a)*26>>8),OT_FX+FX_ARROW,1,3,a,0x100,0,0);}
  int W=worldPx(),Hp=mapH*16;if(frame&8)spr(200+1+msGx*37/W,112+1+msGy*37/Hp,OT_FX+FX_RADAR+2,0,0,3,0);}
 for(int i=msX0;i<msXN;i++){Unit*u=&U[i];if(!u->alive)continue;int show=(u->role==R_STATIC&&o==OB_DESTROY)||(u->role==R_PARA)||(u->role==R_CIV&&o==OB_ESCORT&&(cm->flags&MF_CIVESC))||(u->role==R_ESCORT);
  if(show&&(frame&8)){int W=worldPx(),Hp=mapH*16;spr(200+1+(u->x>>8)*37/W,112+1+(u->y>>8)*37/Hp,OT_FX+FX_RADAR+(u->role==R_STATIC?1:2),0,0,3,0);}}}
static int ms_stars(void){Unit*P=&U[0];int s=1;if(P->deaths<=2)s++;if(P->deaths==0||msT<60*60*4)s++;if(msCivLost)s--;return clampi(s,1,3);}
static void ms_unlock(int i){if(i+1<NCM)campUnlock|=1u<<(i+1);campUnlock|=1u<<i;save_write();}
static void ms_result_fn(int win){Unit*P=&U[0];int was=(campUnlock>>(cmIdx+1))&1;if(win)ms_unlock(cmIdx);campWon=win;
 if(!win)music_stop();
 for(int i=0;;i++){BEAT->screen=win?SCR_DONE:SCR_RESULT;BEAT->menu=cmIdx;hud_clear();hpanel(2,2,26,16);
  hctxt(3,win?"MISSION COMPLETE":"MISSION FAILED",win?HB_GREEN:HB_RED,1);hctxt(5,cm->name,HB_WHITE,1);hctxt(6,cm->date,HB_GRAY,1);
  int r=8;hptxt(4,r,"MISSION TIME",HB_WHITE);hptxt(21,r++,mmss(msT),HB_WHITE);
  hptxt(4,r,"OBJECTIVES",HB_WHITE);hptxt(21,r++,cat3(itoa(msObi>msNob?msNob:msObi),"/",itoa(msNob)),HB_WHITE);
  hptxt(4,r,"ENEMIES DOWNED",HB_WHITE);hptxt(21,r++,itoa(P->kills),HB_WHITE);
  hptxt(4,r,"TIMES DOWNED",HB_WHITE);hptxt(21,r++,itoa(P->deaths),HB_WHITE);
  if(cm->ncv){hptxt(4,r,"CIVILIANS LOST",HB_WHITE);hptxt(21,r++,itoa(msCivLost),msCivLost?HB_RED:HB_GREEN);}
  if(win){int st=ms_stars();hctxt(r+1,st==3?"* * *":st==2?"* *":"*",HB_YEL,1);
   if(!was&&cmIdx+1<NCM)hctxt(r+2,cat("UNLOCKED: ",CM[cmIdx+1].name),HB_GREEN,1);else if(cmIdx+1>=NCM)hctxt(r+2,"CAMPAIGN COMPLETE",HB_GREEN,1);}
  else hctxt(r+1,(cm->flags&MF_ROE)&&msCivPlayer>=2?"RULES OF ENGAGEMENT BROKEN":"TRY AGAIN, SOLDIER",HB_GRAY,1);
  if(i>40)hctxt(16,"A: CONTINUE",HB_YEL,1);
  oam_begin();present(0,0,0);if(i>40&&(pressed&(K_A|K_START)))break;}
 music_stop();}
// ---- alternate side (mirrored objectives, fictional characters, generic markings, no civilians involved)
typedef struct{u8 mi;const char*side0,*side1,*pov;u8 ob[3];s16 arg[3];const char*od[3];const char*b[4];}CAlt;
#define NALT 10
static const CAlt CALT[NALT]={
 {0,"US NAVY","IMPERIAL JAPANESE NAVY","FPO KENJI SATO - CARRIER FIGHTER PILOT",{OB_SURVIVE,OB_KILL,0},{50,5,0},{"INTERCEPT THE STRIKE","PROTECT THE FLEET",""},
  {"AMERICAN DIVE-BOMBERS ARE INBOUND AND","THE FLIGHT DECKS ARE FULL. CLIMB HARD,","BREAK UP THEIR FORMATION AND KEEP","THEM AWAY FROM THE CARRIERS."}},
 {1,"RED ARMY","AXIS (6TH ARMY)","CPL. ERICH BRANDT - ENCIRCLED INFANTRY",{OB_SURVIVE,OB_KILL,0},{60,6,0},{"HOLD THE TRACTOR WORKS","BREAK OUT OF THE RUINS",""},
  {"WINTER HAS CLOSED THE POCKET. FOOD AND","AMMUNITION ARE ALMOST GONE. HOLD THE","FACTORY WITH WHAT IS LEFT OF YOUR","SECTION AND KEEP YOUR MEN ALIVE."}},
 {2,"US PARATROOPERS","AXIS GARRISON","GEFR. LUKAS HOFER - COASTAL GARRISON",{OB_KILL,OB_SURVIVE,0},{6,60,0},{"STOP THE PARATROOPERS","HOLD THE CROSSROADS",""},
  {"SHAPES ARE FALLING FROM THE NIGHT","SKY ALL OVER THE FIELDS. NOBODY KNOWS","WHERE THE MAIN LANDING IS. HOLD THE","CROSSROADS UNTIL DAWN."}},
 {4,"US ARMY","AXIS PANZER CREW","FW. KARL WEBER - TANK COMMANDER",{OB_KILL,OB_REACH,0},{6,3,0},{"BREAK THE DEFENSIVE LINE","REACH THE CROSSROADS",""},
  {"FUEL IS SHORT AND THE FOG WILL NOT","LAST. THE ADVANCE MUST TAKE THE","CROSSROADS BEFORE THE SKIES CLEAR AND","ENEMY AIRCRAFT RETURN."}},
 {5,"US MARINES","IMPERIAL JAPANESE ARMY","PVT. HIROSHI TANAKA - ISLAND DEFENDER",{OB_SURVIVE,OB_KILL,0},{70,8,0},{"HOLD THE BUNKER LINE","REPEL THE LANDING",""},
  {"THE ISLAND IS SHAKING UNDER THE","BOMBARDMENT. THE TUNNELS ARE YOUR","ONLY SHELTER. HOLD THE BUNKER LINE","ABOVE THE BLACK SAND BEACHES."}},
 {6,"US MARINES","KOREAN PEOPLE'S ARMY","LT. PARK JIN-HO - HARBOUR DEFENDER",{OB_SURVIVE,OB_KILL,0},{75,8,0},{"HOLD THE SEAWALL","REPEL THE LANDING",""},
  {"NOBODY EXPECTED A LANDING HERE - THE","TIDES ARE TOO EXTREME. NOW THE SHIPS","ARE IN THE CHANNEL. HOLD THE SEAWALL","WITH YOUR UNDERSTRENGTH COMPANY."}},
 {7,"UN / US ARMY","CHINESE PEOPLE'S VOLUNTEERS","SGT. WANG LEI - VOLUNTEER INFANTRY",{OB_KILL,OB_SURVIVE,0},{8,45,0},{"CUT THE MOUNTAIN ROAD","SURVIVE THE COUNTER-ATTACK",""},
  {"THIRTY BELOW AND YOUR THIN COAT IS","FROZEN STIFF. THE ENEMY COLUMN IS","ON THE ROAD BELOW. CUT THE ROAD AND","HOLD THE RIDGE THROUGH THE NIGHT."}},
 {8,"US AIR CAVALRY","PEOPLE'S ARMY OF VIETNAM","SGT. NGUYEN VAN AN - PAVN INFANTRY",{OB_KILL,OB_SURVIVE,0},{10,50,0},{"ASSAULT THE LANDING ZONE","HOLD UNDER ARTILLERY",""},
  {"HELICOPTERS HAVE LANDED IN THE VALLEY","BELOW THE MOUNTAIN. MOVE CLOSE - SO","CLOSE THEIR ARTILLERY CANNOT REACH","YOU - AND PRESS THE ASSAULT."}},
 {10,"US ARMY","DEFENDING ARMOUR","CAPT. SAMIR HADDAD - TANK COMPANY",{OB_SURVIVE,OB_KILL,0},{60,6,0},{"HOLD THE RIDGE IN THE STORM","STOP THE ARMOURED ADVANCE",""},
  {"THE SANDSTORM BLINDS YOUR SIGHTS BUT","NOT THEIRS. ORDERS ARE TO HOLD THE","RIDGE. SHOOT FAST, MOVE BETWEEN","FIGHTING POSITIONS, BRING YOUR CREW HOME."}},
 {14,"NATO","WARSAW PACT","STARSHINA ILYA DOROV - T-72 COMMANDER",{OB_KILL,OB_REACH,0},{6,3,0},{"BREAK THE NATO SCREEN","REACH THE RIVER CROSSING",""},
  {"A FICTIONAL WAR THAT NEVER HAPPENED.","THE FIRST ECHELON ROLLS AT DAWN. YOUR","COMPANY MUST PUNCH THROUGH THE SCREEN","AND SEIZE THE CROSSING BY NOON."}}};
static int alt_of(int mi){for(int k=0;k<NALT;k++)if(CALT[k].mi==mi)return k;return -1;}
static CMis cmAlt;
static int side_select(int mi,int*side){int a=alt_of(mi);if(a<0){*side=0;return 1;}const CAlt*A=&CALT[a];int s=*side,redraw=1;
 for(;;){BEAT->screen=SCR_MBRIEF;BEAT->menu=mi;BEAT->phase=10+s;
  if(redraw){m3_title(1);m3_box(2,4,236,152,1);m3_ctext(10,CM[mi].name,C_YEL,2);m3_ctext(32,"CHOOSE YOUR SIDE",C_WHITE,1);
   for(int k=0;k<2;k++){int y=50+k*40;u16 c=k==s?C_YEL:C_WHITE;m3_fill(10,y-4,220,34,k==s?RGB(40,44,24):RGB(20,24,44));
    m3_text(16,y,cat(k==s?"> ":"  ",k?A->side1:A->side0),c,1);m3_text(16,y+11,k?A->pov:CM[mi].pov,k?C_RED:C_BLUE,1);
    m3_text(16,y+21,cat(k?"ROLE: ":"ROLE: ",k?A->od[0]:CM[mi].od[0]),C_GRAY,1);}
   m3_text(8,130,"FICTIONAL CHARACTERS. GENERIC MARKINGS.",C_GRAY,1);
   m3_ctext(144,"UP/DOWN: SIDE   A: BRIEFING   B: BACK",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if(pressed&(K_UP|K_DOWN)){s^=1;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){sfx_ok();*side=s;return 1;}
  if(pressed&K_B){sfx_menu();return 0;}}}
static const CMis*mis_for(int i,int side){if(!side)return &CM[i];const CAlt*A=&CALT[alt_of(i)];cmAlt=CM[i];cmAlt.pov=A->pov;cmAlt.flags&=~(MF_ROE|MF_CIVESC);cmAlt.ncv=0;
 for(int k=0;k<3;k++){cmAlt.ob[k]=A->ob[k];cmAlt.arg[k]=A->arg[k];cmAlt.od[k]=A->od[k];}for(int k=0;k<4;k++)cmAlt.b[k]=A->b[k];if(i==8)cmAlt.flags|=MF_MORTAR;return &cmAlt;}
static int brief_card(int i,const CMis*m){
 for(int page=0;page<2;){int redraw=1;
  for(;;){BEAT->screen=SCR_MBRIEF;BEAT->menu=i;BEAT->phase=page;
   if(redraw){m3_title(1);m3_box(2,4,236,152,1);
    m3_ctext(10,m->name,C_YEL,2);m3_ctext(30,m->place,C_WHITE,1);m3_ctext(40,m->date,C_WHITE,1);m3_ctext(50,m->pov,C_BLUE,1);
    if(page==0){for(int k=0;k<4;k++)m3_text(8,66+k*11,m->b[k],C_WHITE,1);
     m3_text(8,116,m->mode==MM_AIR?"MODE: DIVE-BOMBER (FLIGHT)":m->mode==MM_TANK?"MODE: ARMOUR":m->mode==MM_SEA?"MODE: NAVAL - SHORE BOMBARDMENT":"MODE: INFANTRY",C_GRAY,1);
     m3_text(8,126,"FICTIONAL CHARACTERS IN A REAL BATTLE",C_GRAY,1);}
    else{m3_text(8,66,"OBJECTIVES",C_YEL,1);int n=0;for(int k=0;k<3;k++){if(m->mode!=MM_DOGGREEN&&!m->ob[k])continue;m3_text(8,80+n*11,cat3(itoa(n+1),". ",m->od[k]),C_WHITE,1);n++;}
     if(m->flags&MF_ROE)m3_text(8,126,"ROE: DO NOT HARM CIVILIANS",C_RED,1);if(m->flags&MF_MORTAR)m3_text(8,126,"WARNING: ENEMY MORTARS ACTIVE",C_RED,1);
     if(m->ob[0]==OB_TREACH)m3_text(8,126,cat3("TIDE WINDOW: ",itoa(m->arg[0])," SECONDS"),C_RED,1);}
    m3_ctext(140,page==0?"A: NEXT   B: BACK":"A: BEGIN MISSION   B: BACK",C_GRAY,1);m3_present();redraw=0;}
   next_frame();
   if(pressed&(K_A|K_START)){sfx_ok();page++;break;}
   if(pressed&K_B){sfx_menu();if(page==0)return 0;page--;break;}}}
 return 1;}
static void mission_run(int i,int side){cmIdx=i;cm=mis_for(i,side);msAlt=side;campWon=0;
 if(cm->mode==MM_DOGGREEN){if(!loadout_menu(2,0))return;loading("DEPLOYING TO DOG GREEN...");msOn=0;nU=12;campaign();if(campWon)ms_unlock(i);return;}
 if(cm->mode==MM_INF){if(!loadout_menu(2,cm->sEra))return;}else if(cm->mode==MM_TANK){if(!loadout_menu(3,cm->sEra))return;}
 loading(cat("DEPLOYING: ",cm->name));
 msMapDef.n=cm->name;msMapDef.d=cm->place;msMapDef.layout=cm->layout;msMapDef.theme=cm->theme;msMapDef.seed=100+i;msMap=&msMapDef;msTEra=cm->tEra;
 msNob=0;while(msNob<3&&cm->ob[msNob])msNob++;msReady=0;
 ms_role=ms_role_fn;ms_draw=ms_draw_fn;ms_death=ms_death_fn;ms_init=ms_init_fn;ms_tick=ms_tick_fn;ms_hud=ms_hud_fn;ms_spawn=ms_spawn_fn;ms_result=ms_result_fn;ms_skip=ms_skip_fn;
 msOn=1;nU=MAXU;mp_run(cm->mode,cm->sEra,0);msOn=0;msReady=0;nU=12;}
static int msel_jump=-1;
static void mission_select(void){static int sel=0;if(msel_jump>=0){sel=msel_jump;msel_jump=-1;}campUnlock|=(1u<<14)|(1u<<15); // v1.3 what-if bonus mission is always open
int redraw=2,top=0;bgm(1,0);
 for(;;){BEAT->screen=SCR_MSEL;BEAT->menu=sel;BEAT->extra=campUnlock;
  if(sel<top)top=sel;if(sel>top+7)top=sel-7;
  if(redraw){m3_title(1);m3_fill(4,4,232,152,C_PANEL);m3_box(4,4,232,152,2);m3_ctext(8,"CAMPAIGN - MISSION SELECT",C_YEL,1);
   int y=22;for(int k=top;k<NCM&&k<top+8;k++){int un=(campUnlock>>k)&1;
    if(k==top||CM[k].grp!=CM[k-1].grp){m3_text(150,y,GRPN[CM[k].grp],C_BLUE,1);}
    m3_text(10,y,k==sel?">":" ",C_YEL,1);m3_text(18,y,cat3(itoa(k+1),". ",un?CM[k].name:"LOCKED"),k==sel?C_YEL:un?C_WHITE:C_GRAY,1);y+=11;}
   const CMis*m=&CM[sel];int un=(campUnlock>>sel)&1;m3_fill(8,111,224,1,RGB(60,70,110));
   m3_text(8,114,cat3(m->date,"  ",m->mode==MM_AIR?"FLIGHT":m->mode==MM_TANK?"ARMOUR":m->mode==MM_SEA?"NAVAL":"INFANTRY"),un?C_WHITE:C_GRAY,1);m3_text(8,124,m->place,un?C_WHITE:C_GRAY,1);
   m3_text(8,134,un?m->pov:"FINISH THE PREVIOUS MISSION TO UNLOCK",un?C_BLUE:C_GRAY,1);
   m3_ctext(146,"A:BRIEF  B:BACK  L+R+SEL:UNLOCK ALL",C_GRAY,1);m3_present();redraw=0;}
  next_frame();
  if((keys&(K_L|K_R))==(K_L|K_R)&&(pressed&K_SEL)){campUnlock=(1u<<NCM)-1;save_write();sfx_ok();redraw=1;}
  if(pressed&K_UP){sel=(sel+NCM-1)%NCM;redraw=1;sfx_menu();}
  if(pressed&K_DOWN){sel=(sel+1)%NCM;redraw=1;sfx_menu();}
  if(pressed&(K_A|K_START)){if(!((campUnlock>>sel)&1)){sfx_hurt();continue;}sfx_ok();{static int side=0;if(side_select(sel,&side)&&brief_card(sel,mis_for(sel,side))){mission_run(sel,side);bgm(1,0);}}redraw=1;}
  if(pressed&K_B){sfx_menu();return;}}}
