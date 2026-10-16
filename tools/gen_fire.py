#!/usr/bin/env python3
# FIREFIGHTER mission maps -> src/firedb.inc (30x17 tiles). Validates size and legend.
import os,sys
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LEG=set(" #.fsDLEHTPVw*Fpckg,th=CBxa_!")
# legend: ' ' outside  # wall  . floor  f furniture  s warehouse shelving  D door  L locked door  E exterior door
# H hydrant  T engine  P player start  V roof-vent ladder point  w window with person (ladder rescue)
# * fire on floor  F fire in furniture  p adult  c child  k cat  g dog (Scout)  , dry grass  t tree  h house
# = road  C car  B burning car  x burning grass  a ambulance (outside, solid)  _ dirt   ! burning shelving
M=[]
def mis(name,typ,tank,wind,par,prop,call,addr,hint,tip,rows):
    rows=[r.ljust(30)[:30] for r in rows]
    assert len(rows)==17,(name,len(rows))
    for r in rows:
        bad=set(r)-LEG
        assert not bad,(name,bad,r)
    if typ in(0,):
        assert any('P' in r for r in rows),name
    M.append((name,typ,tank,wind,par,prop,call,addr,hint,tip,rows))
# typ 0 structure, 1 wildland, 2 vehicle fire, 3 extrication
mis("KITCHEN FIRE",0,750,0,120,70,"STRUCTURE FIRE, KITCHEN. ONE","OCCUPANT AND A CAT REPORTED INSIDE.","HOOK THE HYDRANT (B), AIM AT THE BASE.",0,[
"",
"     ##############",
"     #ff...#.....f#",
"     #.....D..p..f#",
"     #ff...#......#",
"     ###D######D###",
"     #.....#..**F.#",
"     #..k..#..*...#",
"     #ff...#ff..ff#",
"     ###E##########",
"",
"",
"        P       H",
"     TTTTT",
"",
"   ==============================",
"=============================="])
mis("BEDROOM FIRE",0,750,0,150,65,"SMOKE SHOWING FROM A BEDROOM.","TWO ADULTS AND A CHILD INSIDE.","LOCKED DOOR: HOLD B. VENT FIRST!",1,[
"",
"   ####################",
"   #ff...#ff....#ff..F#",
"   #..p..#...c..L.**..#",
"   #.....#......#..*..#",
"   ##D######D####F*..f#",
"   #..................#",
"   #ff....###D###....f#",
"   #......#..p..#.....#",
"   #......#ff.ff#.....#",
"   ######E#######V#####",
"",
"         P        H",
"      TTTTT",
"",
"==============================",
""])
mis("CAR FIRE ON ROUTE 9",2,750,0,60,40,"VEHICLE FIRE ON ROUTE 9.","DRIVER IS OUT AND SAFE.","COOL IT ALL THE WAY - CARS RE-FLASH.",2,[
"",
"",
"",
"==============================",
"==============================",
"==============================",
"=========CCBBC=================",
"=========CBBBC=================",
"==============================",
"==============================",
"==============================",
"",
"        P",
"     TTTTT",
"",
"",
""])
mis("APARTMENT LADDER RESCUE",0,750,0,180,60,"APARTMENT FIRE, PEOPLE AT THE","WINDOWS ON THE UPPER FLOORS.","PEOPLE AT WINDOWS: FACE ONE, PRESS B.",3,[
"",
"  ##w#######w######w####",
"  #ff..#ff..#..ff..#ff.#",
"  #....D....#..*...D...#",
"  #..p.#....D..F*..#..F#",
"  ######....#.....*#####",
"  #ff..#....###D####ff.#",
"  #....D...............#",
"  #ff..#...........#...#",
"  ##########E###########",
"",
"          P      H",
"       TTTTT",
"",
"==============================",
"",
""])
mis("BRUSH FIRE",1,300,1,150,70,"BRUSH FIRE PUSHED BY AN EAST","WIND. THREE HOMES IN ITS PATH.","SPRAY THE EDGE. B DIGS A FIREBREAK.",4,[
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,t,,,,,,,,,,,,,,,,,,,,,,,,t,",
",,,,,,,,,,,,,,,,,,,,,,,,,,hh,,",
",,,,,,t,,,,,,,,,,,,,,,,,,,hh,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,xxxx,,,,,,,,,,,,,,,,,,",
",,,,,,,xxxxx,,,,,,t,,,,,,,hh,,",
",,,,,,,,xxx,,,,,,,,,,,,,,,hh,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,t,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,hh,,",
",,,,,,,,,,,,,,,t,,,,,,,,,,hh,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,P,,,,,,,,,,,,,,,,,,",
"=======TTTT===================",
"==============================",
""])
mis("HIGHWAY CRASH",3,750,0,90,100,"TWO-CAR CRASH, DRIVER PINNED.","EXTRICATION NEEDED.","STABILIZE, CUT, SPREAD, PACKAGE.",5,[""]*17)
mis("FIND SCOUT",0,750,0,180,60,"HOUSE FIRE, HEAVY SMOKE. A CHILD","AND THE FAMILY DOG SCOUT INSIDE.","R = THERMAL CAMERA. SEE THROUGH SMOKE.",6,[
"",
"  ##########################",
"  #ff...#.....g#ff....#ff..#",
"  #.....#......#......L..c.#",
"  #..*..D......D...F..#....#",
"  #.*F..#ff....#......####D#",
"  ###D########D###D###f....#",
"  #...................#....#",
"  #ff.....#ff..ff#....D..p.#",
"  #.......#......#....#ff..#",
"  ######E##########V########",
"",
"        P             H",
"     TTTTT",
"==============================",
"",
""])
mis("STRIP MALL FIRE",0,750,0,210,55,"FIRE IN A STRIP MALL. SHOPPERS","TRAPPED IN THREE STORES.","FLASHOVER RISK: COOL ROOMS FAST.",7,[
"",
" ############################",
" #ff..ff#f...f#..ff..#ff..ff#",
" #..p...#..*..#......#......#",
" #......#.*F*.#...c..L...p..#",
" #ff....#..*..#......#..ff..#",
" #......#.....#ff....#......#",
" ####D######D######D#####D###",
" #..........................#",
" ######E#############E#######",
"",
"     P            V      H",
"  TTTTT",
"==============================",
"",
"",
""])
mis("WILDLAND INTERFACE",1,300,3,210,60,"WIND-DRIVEN GRASS FIRE NEAR A","NEIGHBORHOOD. PROTECT THE HOMES.","CUT A FIREBREAK AHEAD OF THE FLAMES.",8,[
"hh,,hh,,,,hh,,,hh,,,,hh,,,hh,,",
"hh,,hh,,,,hh,,,hh,,,,hh,,,hh,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,t,,,,,,,,,,,,,,,,,,,,,t,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,t,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,xxxx,,,,,,,,,,,,,",
",,,,,,,t,,,,xxxxxx,,,,,,,,,t,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
",,,,,,,,,,,,,,,,P,,,,,,,,,,,,,",
"===========TTTT===============",
"=============================="])
mis("WAREHOUSE FIRE",0,750,0,300,50,"FIRST ALARM: WAREHOUSE FULLY","INVOLVED. WORKERS UNACCOUNTED FOR.","HYDRANT, VENT, SEARCH, RESCUE.",9,[
"",
" ######w##############w######",
" #ss.ss.ss.ss#..ff#f..ss.ss.#",
" #ss.ss.ss.ss#..p.L...ss.ss.#",
" #...........#....#.........#",
" #ss.ss!ss.ss####D#ss.ss.ss.#",
" #ss.ss!!s.ss#.........!s...#",
" #......!....D.......p..!...#",
" #ss.ss.ss.ss#ss.ss.ss.ss.ss#",
" #ss.ss.ss.ss#ss.ss.ss.ss.ss#",
" #.....c.....#......*.....g.#",
" #####D#######ff..ff#!......#",
" #....p.......D.....#.......#",
" #####E##########V#####E#####",
"",
"   P        H",
"TTTTT"])
TIPS=[("STOP, DROP AND ROLL","IF YOUR CLOTHES CATCH FIRE:","STOP WHERE YOU ARE, DROP TO THE","GROUND, COVER YOUR FACE AND ROLL","UNTIL THE FLAMES ARE OUT."),
("TEST YOUR SMOKE ALARMS","PUT SMOKE ALARMS ON EVERY LEVEL","AND OUTSIDE SLEEPING AREAS.","TEST THEM EVERY MONTH. A WORKING","ALARM GIVES YOU TIME TO GET OUT."),
("CAR FIRE? GET OUT","PULL OVER, TURN THE ENGINE OFF,","GET EVERYONE OUT AND MOVE FAR","AWAY. CALL 911. NEVER OPEN THE","HOOD - LET FIREFIGHTERS DO IT."),
("MAKE AN ESCAPE PLAN","KNOW TWO WAYS OUT OF EVERY ROOM","AND PICK A MEETING PLACE OUTSIDE.","PRACTICE YOUR HOME FIRE DRILL","WITH YOUR FAMILY TWICE A YEAR."),
("WILDFIRE READY","CLEAR DRY BRUSH AND LEAVES AWAY","FROM YOUR HOME. WHEN OFFICIALS","SAY EVACUATE, LEAVE EARLY. A","GO-BAG HELPS YOU MOVE FAST."),
("BUCKLE UP","SEAT BELTS AND CAR SEATS SAVE","LIVES IN CRASHES. AFTER A CRASH,","STAY CALM, CALL 911 AND WAIT FOR","HELP IF YOU ARE HURT."),
("GET LOW AND GO","SMOKE RISES, SO CLEANER AIR IS","NEAR THE FLOOR. CRAWL LOW UNDER","SMOKE TO THE EXIT. FEEL DOORS","WITH THE BACK OF YOUR HAND FIRST."),
("GET OUT, STAY OUT","ONCE YOU ARE OUT, STAY OUT.","NEVER GO BACK IN FOR PETS OR","THINGS - TELL FIREFIGHTERS WHO","IS STILL INSIDE. CALL 911."),
("KITCHEN SAFETY","STAY IN THE KITCHEN WHEN YOU","COOK. GREASE FIRE? COVER THE PAN","WITH A LID AND TURN OFF THE","HEAT. NEVER USE WATER ON GREASE."),
("SPACE HEATERS + CANDLES","KEEP ANYTHING THAT CAN BURN AT","LEAST 3 FEET FROM HEATERS. BLOW","OUT CANDLES WHEN YOU LEAVE THE","ROOM. CLOSE YOUR DOOR AT NIGHT.")]
assert len(M)==10
WALK=set(" .DLE*,=_xP")
for m in M:
    if m[1]==3:continue
    rows=m[10];W,H=30,17
    st=[(x,y) for y in range(H) for x in range(W) if rows[y][x]=='P'][0];seen={st};q=[st]
    while q:
        x,y=q.pop()
        for dx,dy in((1,0),(-1,0),(0,1),(0,-1)):
            nx,ny=x+dx,y+dy
            if 0<=nx<W and 0<=ny<H and (nx,ny) not in seen and rows[ny][nx] in WALK:seen.add((nx,ny));q.append((nx,ny))
    for y in range(H):
        for x in range(W):
            if rows[y][x] in "pckgwHV":
                ok=any((x+dx,y+dy) in seen for dx,dy in((1,0),(-1,0),(0,1),(0,-1)))
                assert ok,(m[0],"unreachable",rows[y][x],x,y)
o=["// generated by tools/gen_fire.py - do not edit","#define FW 30","#define FH 17"]
o.append("static const char FMAP[10][FH][FW+1]={")
for m in M:o.append(" {"+",".join('"%s"'%r for r in m[10])+"},")
o.append("};")
o.append("typedef struct{const char*name;u8 typ;u16 tank;u8 wind;u16 par;u8 prop;const char*call,*addr,*hint;u8 tip;}FiMis;")
o.append("static const FiMis FIM[10]={")
for m in M:o.append(' {"%s",%d,%d,%d,%d,%d,"%s","%s","%s",%d},'%m[:10])
o.append("};")
o.append("static const char*const FTIP[10][5]={")
for t in TIPS:o.append(" {"+",".join('"%s"'%x for x in t)+"},")
o.append("};")
for t in TIPS:
    for x in t:assert len(x)<=34,x
for m in M:
    for x in m[6:9]:assert len(x)<=38,(x,len(x))
open(os.path.join(ROOT,"src","firedb.inc"),"w").write("\n".join(o)+"\n")
print("firedb ok")
