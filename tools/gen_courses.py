# v1.2 SPEC OPS: 90-course table (10 worlds). Each entry: name, stars(1-5), flags, boss ai, boss sprite A, boss sprite B, [theme, enemy set, enemy pal]
# flags: c paradrop j jetpack b boat w water d dark t tunnel l low-g z zero-g s spikes v vaults i ice n wind r rising tide a autoscroll airship x secret exit k bonus course
WDEF=[(6,7,14,12),(2,8,15,13),(0,9,16,14),(5,10,17,15),(1,11,18,16),(11,12,19,17),(6,1,6,6),(7,2,7,7),(9,4,10,9),(12,6,13,11)]  # theme, eset, epal, bosspal
J=(3,7,14);D=(6,8,15);CY_=(2,8,15);K=(5,9,16);CT=(4,11,18);M=(5,12,19)
def T(t):return [None,None,None]+list(t)
PF_COURSES=[
 [("BORDER CROSSING",1,""),("DRY RIVERBED",1,"w",*T(J)),("CANTINA ROW",2,"x"),("JUNGLE AIRSTRIP",2,"w",*T(J)),("STASH HOUSE",1,"k"),("HACIENDA WALLS",3,"v"),
  ("RIDGE LOOKOUT",2,"c",*T(J)),("SMUGGLER PLANE",3,"a",*T(J)),("ARMORED TRUCK",2,"","tank","armtruck")],
 [("DROP ZONE",1,"c"),("WADI CAVES",2,"t"),("DUST STORM",2,"nx",*T(D)),("OASIS CACHE",2,"kw",*T(D)),("COMPOUND WALLS",3,"v",*T(D)),("SLOT CANYON",3,"s"),
  ("DESALINATION DOCK",3,"w",*T(D)),("CARGO PLANE",4,"a"),("TECHNICAL CONVOY",3,"","tank","technical","",*D)],
 [("DMZ TREELINE",1,"s"),("INFILTRATION TUNNEL",2,"td"),("MINEFIELD",2,"sx"),("OBSERVATION POST",2,"k"),("DEEP TUNNEL",3,"tv"),("BORDER RIVER",3,"w"),
  ("ARTILLERY RIDGE",4,"",*T(K)),("BOMBER RUN",4,"a"),("KPA ARMOR",3,"","tank","kpatank")],
 [("VALLEY ROAD",1,""),("CABLE CAR",2,"c"),("GALE RIDGE",2,"nx"),("EAGLE'S NEST",2,"k"),("ICE FALLS",3,"i"),("FORTRESS WALLS",3,"v"),
  ("CANNON ROW",4,""),("TRANSPORT RUN",4,"a"),("HIND HUNTER",3,"","heli","hind")],
 [("FROZEN HARBOR",2,"i"),("ICE CAVES",2,"ti"),("METRO TUNNELS",3,"tx",*T(CT)),("SUPPLY CACHE",2,"ki"),("BLIZZARD PASS",3,"n"),("ROOFTOPS",4,"j",*T(CT)),
  ("BLACKOUT",4,"d",*T(CT)),("ICEBREAKER RUN",4,"a"),("T-90 PLAZA",4,"","tank","t90","",*CT)],
 [("BEACH LANDING",2,"w"),("REEF CROSSING",2,"b"),("JUNGLE BATTERY",3,"x"),("HIDDEN COVE",3,"kw"),("RISING TIDE",3,"r"),("FORTRESS WALLS",4,"v",*T(M)),
  ("TYPE 99 BEACHHEAD",4,"","tank","type99"),("FLYING FORTRESS",5,"a"),("CARRIER FINALE",4,"w","heli","z10")],
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
