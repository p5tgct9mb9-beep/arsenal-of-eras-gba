#!/usr/bin/env python3
# v1.3 SPACE LAUNCH: rocket roster -> src/rocketdb.inc. Every orbital rocket is flown here with the same physics + guide
# the ROM uses (follow the guide marker, throttle 60% through max-Q) and must reach orbit, or the build fails.
# Generic agency colour schemes only - no logos. Dimensions are sprite pixels, dv in 10 m/s, burn in sim-seconds.
import math,os
WH=(236,236,232);BK=(44,44,48);GY=(150,152,158);ST=(192,198,206);OR=(206,120,60);GN=(150,156,136);TAN=(204,168,104)
CR=(226,214,186);BZ=(214,144,74);DB=(70,90,140);DT=(80,140,170);NC=(0,0,0)
AG=["NASA / USA","SPACEX","ESA (EUROPE)","RUSSIA / USSR","CHINA","JAPAN","EARLY 1944-49","BLUE ORIGIN","ROCKET LAB","INDIA (ISRO)","WHAT-IF 1945"]
RU=(120,40,40);DG=(96,104,92);CK=(236,236,232)
TOP={'fair':0,'wide':1,'les':2,'shuttle':3,'starship':4,'capsule':5,'soyuz':6,'buran':7,'orion':8,'cap':9,'glider':10,'ball':11,'v2':12}
CRAFT={'tug':0,'mercury':1,'gemini':2,'apollo':3,'orbiter':4,'dragon':5,'soyuz':6,'shenzhou':7,'orion':8,'starship':9,'buran':10,'cargo':11,'vostok':12,'station':13,'x20':14,'chaser':15,'starliner':16,'glider':17}
LAND={'':0,'ship':1,'side':2,'tower':3,'hop':4}
KIND={'orbit':0,'sub':1,'hop':2,'lunar':3}
R=[ # name, fact, agency, kind, top, craft, land, stages[(w,h,col,dv,bt)], boosters (n,w,h,col) or None
("MERCURY-REDSTONE","1961: FIRST US CREW TO SPACE",0,'sub','les','mercury','',[(8,44,WH,300,140)],None),
("MERCURY-ATLAS","1962: FIRST US ORBITAL CREW",0,'orbit','les','mercury','',[(10,8,ST,300,110),(10,40,ST,800,190)],None),
("TITAN II GEMINI","TWO-SEAT GEMINI PROGRAM 1965",0,'orbit','capsule','gemini','',[(10,36,WH,470,150),(10,16,WH,630,180)],None),
("SATURN IB","APOLLO 7, SKYLAB CREWS",0,'orbit','les','apollo','',[(14,26,WH,340,140),(10,22,WH,780,320)],None),
("SATURN V","APOLLO MOON PROGRAM 1967-73",0,'lunar','les','apollo','',[(14,36,WH,390,160),(14,28,WH,440,390),(10,18,WH,330,300)],None),
("SHUTTLE COLUMBIA","FIRST ORBITER - STS-1, 1981",0,'orbit','shuttle','orbiter','',[(0,0,OR,420,120),(12,46,OR,670,390)],(2,6,44,WH)),
("SHUTTLE CHALLENGER","STS-7: SALLY RIDE, 1983",0,'orbit','shuttle','orbiter','',[(0,0,OR,420,120),(12,46,OR,670,390)],(2,6,44,WH)),
("SHUTTLE DISCOVERY","DEPLOYED HUBBLE - 39 FLIGHTS",0,'orbit','shuttle','orbiter','',[(0,0,OR,420,120),(12,46,OR,670,390)],(2,6,44,WH)),
("SHUTTLE ATLANTIS","FLEW STS-135, THE FINALE",0,'orbit','shuttle','orbiter','',[(0,0,OR,420,120),(12,46,OR,670,390)],(2,6,44,WH)),
("SHUTTLE ENDEAVOUR","STS-88: FIRST ISS ASSEMBLY",0,'orbit','shuttle','orbiter','',[(0,0,OR,420,120),(12,46,OR,670,390)],(2,6,44,WH)),
("DELTA II","GPS + MARS ROVER WORKHORSE",0,'orbit','fair','tug','',[(0,0,DT,250,85),(10,36,DT,320,190),(8,14,WH,530,360)],(4,4,18,WH)),
("DELTA IV HEAVY","THREE COMMON BOOSTER CORES",0,'orbit','wide','tug','',[(0,0,OR,360,140),(12,56,OR,300,150),(10,14,WH,540,450)],(2,12,56,OR)),
("ATLAS V","CENTAUR UPPER STAGE",0,'orbit','wide','tug','',[(0,0,BZ,260,100),(12,44,BZ,340,180),(10,12,WH,580,420)],(2,5,24,WH)),
("TITAN IV","HEAVY-LIFT TITAN, 1989-2005",0,'orbit','wide','tug','',[(0,0,TAN,300,110),(12,40,TAN,380,170),(12,14,TAN,400,330)],(2,8,50,GY)),
("ANTARES","CYGNUS CARGO FROM WALLOPS",0,'orbit','fair','cargo','',[(12,36,WH,460,200),(10,14,BK,640,260)],None),
("SLS / ORION","ARTEMIS MOON ROCKET",0,'orbit','orion','orion','',[(0,0,OR,350,126),(14,54,OR,500,300),(10,10,WH,260,300)],(2,7,52,WH)),
("FALCON 1","FIRST PRIVATE LIQUID TO ORBIT",1,'orbit','fair','tug','',[(7,30,WH,500,150),(7,10,BK,560,330)],None),
("FALCON 9 BLOCK 5","LAND THE BOOSTER ON THE SHIP",1,'orbit','wide','tug','ship',[(9,48,WH,370,160),(9,14,WH,700,370)],None),
("FALCON HEAVY","TWIN SIDE-BOOSTER LANDINGS",1,'orbit','wide','tug','side',[(0,0,WH,300,120),(9,48,WH,290,90),(9,14,WH,700,370)],(2,9,48,WH)),
("STARSHIP","SUPER HEAVY: CATCH IT AT THE TOWER",1,'orbit','starship','starship','tower',[(14,40,ST,320,160),(14,34,ST,780,360)],None),
("CREW DRAGON","FALCON 9 + CREW TO THE STATION",1,'orbit','capsule','dragon','ship',[(9,48,WH,370,160),(9,14,WH,700,370)],None),
("CARGO DRAGON","FALCON 9 + STATION RESUPPLY",1,'orbit','capsule','cargo','ship',[(9,48,WH,370,160),(9,14,WH,700,370)],None),
("GRASSHOPPER","VTOL HOP TEST VEHICLE 2012-13",1,'hop','cap','tug','hop',[(9,30,GY,0,1)],None),
("ARIANE 5","EUROPE'S HEAVY LIFTER, ATV",2,'orbit','wide','cargo','',[(0,0,WH,290,130),(12,40,WH,520,320),(10,14,WH,380,300)],(2,7,40,WH)),
("ARIANE 6","MODULAR SUCCESSOR, 2024",2,'orbit','wide','tug','',[(0,0,WH,250,110),(12,40,WH,480,300),(12,14,WH,450,320)],(2,6,24,WH)),
("VEGA","SMALL-SAT SOLID LAUNCHER",2,'orbit','fair','tug','',[(8,20,WH,350,110),(7,12,WH,350,80),(6,10,WH,400,100)],None),
("SOYUZ","CREW FERRY SINCE 1966",3,'orbit','soyuz','soyuz','',[(0,0,GN,320,120),(9,36,GN,380,170),(9,12,GN,380,240)],(4,6,26,GN)),
("PROTON","HEAVY LIFT: STATION MODULES",3,'orbit','wide','cargo','',[(14,30,CR,350,120),(10,20,CR,380,210),(10,10,CR,350,240)],None),
("ENERGIA / BURAN","1988 UNCREWED ORBITAL FLIGHT",3,'orbit','buran','buran','',[(0,0,OR,380,140),(14,52,OR,700,330)],(4,6,46,WH)),
("N1","SOVIET SUPER-HEAVY MOON ROCKET",3,'orbit','les','soyuz','',[(22,30,WH,390,120),(16,24,WH,420,130),(12,18,WH,320,300)],None),
("LONG MARCH 5","CHINA'S HEAVY LIFTER",4,'orbit','wide','cargo','',[(0,0,WH,280,120),(14,40,WH,520,320),(14,14,WH,380,300)],(4,7,28,WH)),
("LONG MARCH 2F","SHENZHOU CREW LAUNCHER",4,'orbit','soyuz','shenzhou','',[(0,0,WH,300,120),(10,36,WH,400,170),(10,16,WH,380,240)],(4,6,24,WH)),
("H-IIA","WORKHORSE FROM TANEGASHIMA",5,'orbit','wide','tug','',[(0,0,BZ,220,90),(11,40,BZ,500,300),(11,12,WH,450,330)],(2,6,28,WH)),
("H3","NEXT-GEN, HTV-X CARGO",5,'orbit','wide','cargo','',[(0,0,BZ,220,90),(13,40,BZ,520,300),(11,12,WH,440,330)],(2,6,26,WH)),
("EPSILON","SOLID ROCKET FOR SCIENCE SATS",5,'orbit','fair','tug','',[(8,22,WH,350,110),(7,12,DB,380,90),(6,8,WH,420,110)],None),
("V-2 / A4 MW 18014","1944: FIRST OBJECT TO REACH SPACE",6,'sub','v2','tug','',[(8,40,CK,245,65)],None,174),
("WAC CORPORAL","1945: FIRST US SOUNDING ROCKET",6,'sub','fair','tug','',[(5,14,WH,60,8),(4,16,RU,85,25)],None,70),
("BUMPER-WAC","1949: V-2 + WAC, 393 KM RECORD",6,'sub','fair','tug','',[(8,40,CK,245,65),(4,16,RU,116,45)],None,390),
("VOSTOK 1","1961: GAGARIN, FIRST HUMAN IN ORBIT",3,'orbit','ball','vostok','',[(0,0,GN,320,120),(9,36,GN,380,170),(8,10,GN,420,250)],(4,6,26,GN)),
("VOSKHOD 2","1965: LEONOV, FIRST SPACEWALK",3,'orbit','ball','vostok','',[(0,0,GN,320,120),(9,36,GN,380,170),(9,12,GN,400,240)],(4,6,26,GN)),
("SALYUT / MIR","STATION MODULES ON PROTON",3,'orbit','wide','station','',[(14,30,CR,350,120),(10,20,CR,380,210),(10,10,CR,350,240)],None),
("SKYLAB","1973: FIRST US SPACE STATION",0,'orbit','wide','station','',[(14,36,WH,390,160),(14,28,WH,600,390)],None),
("APOLLO 15 + ROVER","1971: FIRST LUNAR ROVING VEHICLE",0,'lunar','les','apollo','',[(14,36,WH,390,160),(14,28,WH,440,390),(10,18,WH,330,300)],None),
("X-20 DYNA-SOAR","WHAT-IF: CANCELLED 1963 SPACEPLANE",0,'orbit','glider','x20','',[(0,0,TAN,300,110),(10,36,TAN,420,170),(10,10,TAN,450,300)],(2,7,40,GY)),
("VULCAN CENTAUR","ULA HEAVY LIFTER, 2024",0,'orbit','wide','tug','',[(0,0,OR,260,100),(13,46,OR,340,190),(11,12,WH,580,420)],(2,5,22,WH)),
("STARLINER","ATLAS V N22: CREW TO THE ISS",0,'orbit','capsule','starliner','',[(0,0,BZ,260,100),(12,44,BZ,340,180),(10,8,WH,600,420)],(2,5,24,WH)),
("DREAM CHASER","VULCAN: SPACEPLANE CARGO RUN",0,'orbit','fair','chaser','',[(0,0,OR,260,100),(13,46,OR,340,190),(11,12,WH,580,420)],(2,5,22,WH)),
("STARSHIP HLS","ARTEMIS LUNAR LANDER",1,'lunar','starship','starship','',[(14,40,ST,320,160),(14,34,ST,780,360)],None),
("NEW SHEPARD","SUBORBITAL TOURISM, PAD LANDING",7,'sub','capsule','tug','hop',[(9,30,WH,225,110)],None,105),
("NEW GLENN","7-ENGINE BOOSTER, SHIP LANDING",7,'orbit','wide','tug','ship',[(13,52,WH,360,170),(13,16,WH,700,380)],None),
("ELECTRON","3D-PRINTED ENGINES, SMALLSATS",8,'orbit','fair','tug','',[(6,30,BK,480,150),(6,10,BK,560,330)],None),
("NEUTRON","REUSABLE MEDIUM LIFTER",8,'orbit','wide','tug','ship',[(11,46,BK,370,160),(9,12,BK,700,370)],None),
("LONG MARCH 5B","2021: TIANHE CORE OF TIANGONG",4,'orbit','wide','station','',[(0,0,WH,280,120),(14,48,WH,800,400)],(4,7,28,WH)),
("LVM3 GAGANYAAN","INDIA'S CREW CAPSULE PROGRAM",9,'orbit','les','orion','',[(0,0,WH,300,130),(12,36,WH,420,200),(10,12,WH,420,320)],(2,8,40,GY)),
("A9/A10 AMERIKA","WHAT-IF: 2-STAGE TRANSATLANTIC",10,'sub','v2','tug','',[(14,30,DG,200,60),(8,36,DG,154,65)],None,350),
("A4B WINGED","WHAT-IF: GLIDE-WINGED A4",10,'sub','glider','tug','',[(8,38,DG,212,65)],None,120),
("SILBERVOGEL","WHAT-IF: SKIP-GLIDE BOMBER STUDY",10,'orbit','glider','glider','',[(14,40,DG,400,150),(10,20,DG,700,330)],None),
("RHEINBOTE","WHAT-IF: 4-STAGE SOLID SOUNDING",10,'sub','fair','tug','',[(6,14,DG,60,12),(5,12,DG,54,20),(4,10,DG,61,20)],None,90),
("NATTER BA 349","WHAT-IF: VERTICAL LAUNCH HOP",10,'sub','cap','tug','',[(6,16,DG,111,40)],None,30),
("ALLIED LANCER X","WHAT-IF: ALLIED ORBITAL COUNTER",10,'orbit','les','mercury','',[(12,40,WH,420,150),(10,16,WH,700,330)],None),
("SOVIET ZARYA-45","WHAT-IF: SOVIET CLUSTER ROCKET",10,'orbit','ball','vostok','',[(0,0,GN,330,120),(9,36,GN,380,170),(8,10,GN,420,250)],(4,6,26,GN)),
]
def guide(alt,vv,vh,A):
    if alt<90000:return 85*min(1,math.sqrt(max(alt,0)/100000))
    g=9.8-vh*vh/6.471e6;vt=(175000-alt)/40;need=g+(vt-vv)/20
    if A<=0:return 90
    return math.degrees(math.acos(max(-0.2,min(1,need/A))))
def fly(st,kind):
    dv=[s[3] for s in st];bt=[s[4] for s in st];alt=vv=vh=0.0;s=0;fuel=bt[0];p=0;dt=.25;stress=0;apo=0
    for t in range(6000):
        if s>=len(dv):
            if kind=='sub':
                vv-=9.8*dt;alt+=vv*dt;apo=max(apo,alt)
                if vv<0:return ('APOGEE',int(apo))
                continue
            return ('FAIL',int(alt),int(vh))
        rho=math.exp(-alt/8500);q=0.6125*(vv*vv+vh*vh)*rho/1000;thr=0.6 if q>28 else 1
        f=1-fuel/bt[s];A=dv[s]*10/bt[s]*(0.6+0.8*f)*thr
        if kind!='sub':p+=max(-1,min(1,guide(alt,vv,vh,A)-p))
        fuel-=dt*thr
        if fuel<=0:
            s+=1
            if s<len(dv):fuel=bt[s]
        g=9.8-vh*vh/6.471e6;vv+=(A*math.cos(math.radians(p))-g)*dt;vh+=A*math.sin(math.radians(p))*dt;alt+=vv*dt
        stress+=max(0,q-30)
        if alt<0 and t>4:return ('CRASH',t)
        if kind!='sub' and alt>=150000 and vh>=7700:return ('ORBIT',t,int(stress))
    return ('TIMEOUT',)
def c(rgb):r,g,b=rgb;return "RGB(%d,%d,%d)"%(r,g,b)
o=["// generated by tools/gen_rockets.py - do not edit","#define NRK %d"%len(R),"#define NAGY %d"%len(AG),
   "static const char*const AGN[NAGY]={%s};"%",".join('"%s"'%a for a in AG),"static const Rk RK[NRK]={"]
bad=0
for row in R:
    n,f,ag,kind,top,craft,land,st,bo=row[:9];tgt=row[9] if len(row)>9 else 187
    assert len(n)<=19 and len(f)<=36,(n,f)
    if kind=='sub':assert tgt<=510
    if kind in('orbit','lunar','sub'):
        res=fly(st,kind);ok=res[0]=='ORBIT' or (res[0]=='APOGEE' and res[1]>=tgt*1000)
        print("  %-20s %s"%(n,res)+("" if ok else "   <-- FAILS"));bad+=not ok
        assert all(s[3]*10/s[4]*0.6>11.5 for s in st[:1] if s[1] or not bo),("liftoff T/W too low",n)
    ns=len(st);pad=lambda L,z:L+[z]*(3-len(L))
    bn,bw,bh,bc=bo if bo else (0,0,0,NC)
    o.append(' {"%s","%s",%d,%d,%d,%d,%d,%d,%d,{%s},{%s},{%s},{%s},{%s},%d,%d,%d,%s},'%(n,f,ag,ns,bn,TOP[top],CRAFT[craft],LAND[land],KIND[kind],
      ",".join(str(x) for x in pad([s[0] for s in st],0)),",".join(str(x) for x in pad([s[1] for s in st],0)),
      ",".join(c(x) for x in pad([s[2] for s in st],NC)),",".join(str(x) for x in pad([s[3] for s in st],0)),",".join(str(x) for x in pad([s[4] for s in st],1)),bw,bh,(tgt+1)//2 if kind=='sub' else 0,c(bc)))
o.append("};")
assert not bad,"%d rockets cannot reach their goal"%bad
open(os.path.join(os.path.dirname(__file__),'..','src','rocketdb.inc'),'w').write("\n".join(o)+"\n")
print("rockets: %d (%s)"%(len(R),", ".join("%s %d"%(a,sum(1 for r in R if r[2]==i)) for i,a in enumerate(AG))))
