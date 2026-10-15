# v1.2 SPEC OPS: 90-course table (10 worlds). Each entry: name, stars(1-5), flags, boss ai, boss sprite A, boss sprite B, [theme, enemy set, enemy pal]
# flags: c paradrop j jetpack b boat w water d dark t tunnel l low-g z zero-g s spikes v vaults i ice n wind r rising tide a autoscroll airship x secret exit k bonus course
WDEF=[(0,0,0,0),(1,0,1,1),(2,0,2,2),(3,0,3,3),(4,0,4,4),(5,0,5,5),(6,1,6,6),(7,2,7,7),(9,4,10,9),(12,6,13,11)]  # theme, eset, epal, bosspal
PF_COURSES=[
 [("TREELINE",1,""),("OLD MILL",1,"w"),("LOGGING CAMP",2,"x"),("RIVER CROSSING",2,"w"),("SUPPLY DEPOT",1,"k"),("RIDGE BUNKERS",3,"v"),("HILLTOP RADIO",2,"c"),("BOMBER RUN",3,"a"),("TANK TRAP",2,"","tank","tank")],
 [("DROP ZONE",1,"c"),("ICE CAVES",2,"ti"),("FROZEN LAKE",2,"ix"),("AURORA CACHE",2,"ki"),("BLIZZARD PASS",3,"n"),("RADAR STATION",3,"vi"),("ICEBREAKER DOCK",3,"w"),("SKY FORTRESS",4,"a"),("GUNSHIP NEST",3,"","heli","heli")],
 [("DRY WASH",1,"s"),("MESA CLIFFS",2,"c"),("DUST DEVIL",2,"nx"),("OASIS",2,"kw"),("RAIL YARD",3,"v"),("SLOT CANYON",3,"s"),("ORE CONVEYOR",4,""),("CARGO PLANE",4,"a"),("ARMORED TRAIN",3,"","train","loco","car")],
 [("RIVER BANK",1,"w"),("BOAT RUN",2,"b"),("MANGROVE",2,"wx"),("HIDDEN LAGOON",2,"kw"),("RISING TIDE",3,"r"),("SUNKEN TEMPLE",3,"wd"),("WATERFALL CLIMB",4,"w"),("SEAPLANE RAID",4,"a"),("GUNBOAT BAY",3,"w","boat","boat")],
 [("ROOFTOPS",2,"j"),("NIGHT MARKET",2,"dv"),("SUBWAY",3,"tx"),("NEON ARCADE",2,"k"),("FACTORY LINE",3,""),("SKY TOWER",4,"j"),("BLACKOUT",4,"d"),("AIRSHIP DOCK",4,"a"),("WALKER PLAZA",4,"","walker","walker")],
 [("SWITCHBACKS",2,"s"),("CABLE CAR",2,"c"),("GALE RIDGE",3,"nx"),("EAGLE'S NEST",3,"k"),("ICE FALLS",3,"i"),("FORTRESS WALLS",4,"v"),("CANNON ROW",4,""),("FLYING FORTRESS",5,"a"),("THE BIG GUN",4,"","fort","fort")],
 [("DESERT OUTPOST",2,"v"),("MINE SHAFTS",3,"tdx"),("ABANDONED PIT",3,"k"),("ORE CRUSHERS",3,""),("PROCESSING PLANT",4,"s"),("FUEL DEPOT",4,""),("MERC CONVOY",4,"","convoy"),("CARGO JET RUN",4,"a"),("MERCENARY AIRBASE",5,"","mi8","mi8","cmdr")],
 [("JUNGLE PATROL",2,"w"),("RICE PADDIES",2,"w"),("TUNNEL NETWORK",3,"tdx"),("FIREBASE CACHE",3,"k"),("AA EMPLACEMENT",3,"","fort","aagun"),("PBR RIVER RUN",3,"b"),
  ("FULDA GAP",4,"v","tank","t55","",8,3,9),("BORDER WIRE",4,"sn",None,None,None,8,3,9),("SNOW BASE",5,"i","heli","hind","",8,3,8)],
 [("NORMANDY HEDGEROWS",3,"c"),("BOMBED CITY",3,"vx",None,None,None,10,4,10),("RESISTANCE CELLAR",3,"k",None,None,None,10,4,10),("TIGER AMBUSH",4,"","tank","tiger","",10,4,10),
  ("MOUNTAIN VILLAGE",3,"s",None,None,None,5,4,11),("ARMORED TRAIN",4,"","train","loco","car",5,4,11),("PACIFIC JUNGLE",4,"w",None,None,None,11,5,12),
  ("BUNKER COMPLEX",5,"","fortfly","bunker","zero",11,5,12),("BOMBER STREAM",5,"a",None,None,None,10,4,10),("FINAL FORTRESS",5,"v","fortfly","fort","me262",10,4,10)],
 [("LUNAR BASE",3,"l"),("LOW-G CRATERS",3,"ljx"),("CRYSTAL VAULT",4,"kl"),("MARS HIVE",4,"s",None,None,None,13,6,13),("BROOD NEST",4,"l","walker","brood","",13,6,13),
  ("DERELICT STARSHIP",4,"zd",None,None,None,14,6,13),("ZERO-G CORE",5,"z",None,None,None,14,6,13),("MOTHERSHIP CORE",5,"z","mother","mother","",14,6,13)],
]
ST=[];STARS=[]
for w,lst in enumerate(PF_COURSES):
    th0,es0,ep0,bp0=WDEF[w];row=[];sr=[]
    for e in lst:
        n,st,fl=e[0],e[1],e[2];ai=e[3] if len(e)>3 and e[3] else "";ba=e[4] if len(e)>4 and e[4] else "";bb=e[5] if len(e)>5 and e[5] else ""
        th=e[6] if len(e)>6 else th0;es=e[7] if len(e)>7 else es0;ep=e[8] if len(e)>8 else ep0
        if ai=="convoy":ba=""
        row.append((n,th,es,ep,ai,ba,bb,bp0 if th==th0 or w!=7 else 8,fl));sr.append(st)
    ST.append(row);STARS.append(sr)
