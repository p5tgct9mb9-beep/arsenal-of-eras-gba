#!/usr/bin/env python3
# v1.3.3 achievements -> src/achdb.inc  (tiers 0 bronze 1 silver 2 gold 3 platinum; hidden ones show ??? until earned)
import os
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
K="PLAY WIN TPLAY TWIN ROCKS BUY MODES TIME LB CAMP FSTAR FRESC INVW INVS SOF ACHN FLAG SPENT ARCS ARCN".split()
G={'CAMP':1,'INF':2,'TANK':2,'AIR':2,'SEA':2,'SWORD':2,'FPS':2,'SPEC':3,'BRAD':3,'SNIPE':3,'EOD':3,'LRRP':3,'FIRE':3,
   'AA':4,'DRONE':4,'FLIGHT':4,'OVER':4,'LAUNCH':5,'SPACE':5,'ASTRO':5,'MOON':5,'MARS':5,'ODEF':5,'ASTER':5,'WHATIF':5,'VBM':5,'RBUILD':8,
   'INVADE':6,'GP':6,'TT':6,'FREE':6,'HIST':7,'BUILD':8,'SAND':8}
# (mode, bronze name, bronze how, win?, silver name, gold name)
M=[('CAMP','FIRST ORDERS','PLAY A CAMPAIGN MISSION',1,'BATTLE HARDENED','WAR HERO'),
 ('INF','BOOTS ON GROUND','PLAY 6V6 INFANTRY',1,'SQUAD LEADER','PLATOON LEGEND'),
 ('TANK','TANKER','PLAY 6V6 ARMOR',1,'STEEL RAIN','ARMORED FIST'),
 ('AIR','WHEELS UP','PLAY 6V6 DOGFIGHT',1,'WINGMAN','TOP GUN'),
 ('SEA','ANCHORS AWEIGH','PLAY 6V6 NAVAL',1,'BROADSIDE','ADMIRAL'),
 ('SWORD','EN GARDE','PLAY A SWORD FIGHT',1,'BLADEMASTER',None),
 ('FPS','EYES FORWARD','PLAY THE FPS MODE',1,'POINT MAN',None),
 ('SPEC','QUIET PROFESSIONAL','PLAY SPEC OPS',1,'OPERATOR','TIER ONE'),
 ('BRAD','BRADLEY CREW','PLAY BRADLEY OPS',1,'BUSHMASTER',None),
 ('SNIPE','ONE SHOT','PLAY SNIPER',1,'MARKSMAN','GHOST'),
 ('EOD','STEADY HANDS','PLAY EOD',1,'RED WIRE',None),
 ('LRRP','RECON','PLAY LRRP',1,'LURP',None),
 ('FIRE','ENGINE ONE','RUN A FIREFIGHTER CALL',1,'SMOKE EATER','CAPTAIN'),
 ('AA','SKYWATCH','PLAY ANTI-AIRCRAFT',1,'FLAK WALL',None),
 ('DRONE','EYE IN THE SKY','PLAY DRONE OPS',1,'OVERWATCH',None),
 ('FLIGHT','AVIATOR','FLY WORLD FLIGHT',0,None,None),
 ('OVER','NEED TO KNOW','FLY AN OVERFLIGHT',1,'BLACKBIRD',None),
 ('LAUNCH','LIFTOFF','FLY A SPACE LAUNCH',1,'BOOSTER BACK','FLIGHT DIRECTOR'),
 ('SPACE','MISSION CONTROL','PLAY SPACE PROGRAM',1,'ORBITAL',None),
 ('ASTRO','EXPLORER','PLAY ASTRONAUT EXPLORER',1,None,None),
 ('MOON','MOONWALKER','PLAY MOON BASE',1,None,None),
 ('MARS','RED PLANET','PLAY MARS MISSION',1,None,None),
 ('ODEF','ORBITAL GUARD','PLAY ORBITAL DEFENSE',1,None,None),
 ('ASTER','PROSPECTOR','PLAY ASTEROID MINING',1,None,None),
 ('WHATIF','ALT HISTORY','PLAY WHAT-IF 1945',1,None,None),
 ('VBM','ROCKET TEAM','PLAY VON BRAUN MISSIONS',1,'CHIEF ENGINEER',None),
 ('RBUILD','DRAFTING TABLE','OPEN THE ROCKET BUILDER',0,None,None),
 ('INVADE','INVASION','PLAY ROCKET INVADERS',1,'DEFENDER',None),
 ('GP','LIGHTS OUT','RACE A GRAND PRIX',1,'PODIUM','CHAMPION'),
 ('TT','AGAINST THE CLOCK','RUN A TIME TRIAL',1,None,None),
 ('FREE','SUNDAY DRIVE','TRY FREE DRIVE',0,None,None),
 ('HIST','HISTORIAN','TAKE THE HISTORY TOUR',0,None,None),
 ('BUILD','DESIGNER','OPEN THE AIRCRAFT BUILDER',0,None,None),
 ('SAND','SANDBOXER','PLAY A SANDBOX BATTLE',1,None,None)]
A=[]
def add(n,how,t,k,arg,tgt,g,hid=0):A.append((n,how,t,K.index(k),arg,tgt,g,hid))
for (m,bn,bh,w,sn,gn) in M:
    add(bn,bh,0,'PLAY','MD_'+m,1,G[m])
    d=bh.split(' ',1)[1]
    if sn:add(sn,'WIN 5 TIMES: '+d,1,'WIN','MD_'+m,5,G[m])
    if gn:add(gn,'WIN 20 TIMES: '+d,2,'WIN','MD_'+m,20,G[m])
for t,(n,v) in enumerate([('ON DUTY',10),('VETERAN',50),('LIFER',200),('ARSENAL OF ERAS',600)]):add(n,'PLAY %d MATCHES/MISSIONS'%v,t,'TPLAY',0,v,10)
for t,(n,v) in enumerate([('FIRST VICTORY',1),('WINNER',25),('CONQUEROR',100),('UNSTOPPABLE',300)]):add(n,'WIN %d TIMES IN ANY MODE'%v,t,'TWIN',0,v,10)
for t,(n,v) in enumerate([('ROCK HOUND',100),('GEOLOGIST',750),('ROCK BARON',3000),('MARTIAN TYCOON',8000)]):add(n,'EARN %d MARS ROCKS'%v,t,'ROCKS',0,v,9)
for t,(n,v) in enumerate([('FIRST PURCHASE',1),('COLLECTOR',5),('QUARTERMASTER',15),('FULL ARMORY',41)]):add(n,'BUY %d STORE ITEM%s'%(v,'' if v==1 else 'S'),t,'BUY',0,v,9)
for t,(n,v) in enumerate([('TOURIST',5),('WELL TRAVELLED',15),('JACK OF ALL TRADES',25),('SEEN IT ALL',34)]):add(n,'PLAY %d DIFFERENT MODES'%v,t,'MODES',0,v,10)
for t,(n,v) in enumerate([('HALF HOUR',30),('TWO HOURS',120),('TEN HOURS',600)]):add(n,'PLAY FOR %s'%('30 MINUTES' if v==30 else '%d HOURS'%(v//60)),t,'TIME',0,v,10)
add('ON THE BOARD','SET A LEADERBOARD RECORD',0,'LB',0,1,10);add('RECORD BREAKER','SET 10 LEADERBOARD RECORDS',1,'LB',0,10,10)
for t,(n,v) in enumerate([('SECOND FRONT',4),('CAMPAIGNER',8),('ALL FRONTS',16)]):add(n,'UNLOCK %d CAMPAIGN MISSIONS'%v,t,'CAMP',0,v,1)
for t,(n,v) in enumerate([('BRONZE BUGLE',10),('SILVER BUGLE',20),('GOLD BUGLE',30)]):add(n,'EARN %d FIREFIGHTER STARS'%v,t,'FSTAR',0,v,3)
add('LIFE SAVER','RESCUE 10 PEOPLE OR PETS',0,'FRESC',0,10,3);add('GUARDIAN ANGEL','RESCUE 50 PEOPLE OR PETS',1,'FRESC',0,50,3)
for t,(n,v) in enumerate([('WAVE RIDER',5),('HOLD THE LINE',10),('LAST ROCKET STANDING',20)]):add(n,'REACH INVADERS WAVE %d'%v,t,'INVW',0,v,6)
add('HIGH SCORER','SCORE 5000 IN INVADERS',0,'INVS',0,5000,6);add('ARCADE ACE','SCORE 20000 IN INVADERS',2,'INVS',0,20000,6)
for t,(n,v) in enumerate([('NEW ALLY',1),('COALITION',8),('FIVE EYES AND MORE',16)]):add(n,'RECRUIT %d STORE SOF UNIT%s'%(v,'' if v==1 else 'S'),t,'SOF',0,v,9)
for t,(n,v) in enumerate([('BADGE HUNTER',25),('DECORATED',50),('MEDAL OF ERAS',100)]):add(n,'EARN %d BADGES'%v,t+1 if t else 1,'ACHN',0,v,10)
add('BIG SPENDER','SPEND 2000 MARS ROCKS',1,'SPENT',0,2000,9)
H=[('ROLL CREDITS','WATCH THE CREDITS TO THE END',0),('INSIDER','ENTER A SECRET CODE',1),('GOOD DOG','FINISH BOOT CAMP WITH KC',2),
   ('QUEEN SLAYER','DEFEAT THE HIVE QUEEN',3),('SHARE THE GLORY','VIEW A LEADERBOARD SHARE CODE',4),('WHO AM I','OPEN YOUR PROFILE CARD',5),
   ('HOVER HIGH','HOVER A CUSTOM HELI AT 60 M',6),('NO MERCY','WIN A RACE ON HARD',7),('DOWN TO PEBBLES','SPEND DOWN TO UNDER 10 ROCKS',8),('STEEL STARSHIP','BEAT THE HIVE QUEEN WITH STARSHIP',9)]
for n,how,b in H:add(n,how,1 if b in(3,7,9) else 0,'FLAG',b,1,10,1)
# v1.3.4 arcade cabinet: per game a bronze score badge + a silver high-score badge, plus cabinet totals (grp 6 = arcade)
ARC=[('IRON DOME','DOME KEEPER',2000,'IRON CURTAIN',8000),('DEBRIS SWEEPER','JANITOR',2000,'CLEAN ORBIT',8000),('KC CROSSING','GOOD GIRL',500,'TOP DOG',2500),
 ('BUNKER MAZE','TREAT HUNTER',2000,'BONE COLLECTOR',8000),('BUNKER BUSTER','WALL CRACKER',1500,'FORT BREAKER',6000),('CARGO STACK','LOADMASTER',1500,'TETRA-TON',8000),
 ('CONVOY','SUPPLY RUN',300,'LONG HAUL',1200),('CHOPPER RESCUE','NIGHT STALKER',600,'NO ONE LEFT',3000),('STARSHIP HOPPER','HOP HOP',10,'TOWER MASTER',40),
 ('TANK PONG','PADDLE TANK',500,'RICOCHET KING',2000),('TANK DUEL','DUELIST',500,'LAST TANK',3000),('SPACE PINBALL','TILT',5000,'MARS MULTIBALL',30000)]
for g,(gn,bn,bv,sn,sv) in enumerate(ARC):
    add(bn,'SCORE %d IN %s'%(bv,gn),0,'ARCS',g,bv,6);add(sn,'SCORE %d IN %s'%(sv,gn),1,'ARCS',g,sv,6)
add('COIN-OP',"PLAY 4 DIFFERENT CABINET GAMES",0,'ARCN',0,4,6);add('ARCADE RAT','PLAY ALL 12 CABINET GAMES',1,'ARCN',0,12,6)
add('PLATINUM ARSENAL','EARN EVERY OTHER BADGE',3,'ACHN',0,len(A),10)
o=["// generated by tools/gen_ach.py - do not edit","enum{"+",".join("AK_"+k for k in K)+"};",
 "typedef struct{const char*n,*how;u8 tier,kind;s16 arg;u16 tgt;u8 grp,hid;}Ach;","#define ACH_N %d"%len(A),"static const Ach ACH[ACH_N]={"]
for (n,how,t,k,arg,tgt,g,hid) in A:
    assert len(n)<=22 and len(how)<=38,(n,how)
    o.append(' {"%s","%s",%d,%d,%s,%d,%d,%d},'%(n,how,t,k,arg,tgt,g,hid))
o.append("};")
open(os.path.join(ROOT,'src','achdb.inc'),'w').write("\n".join(o)+"\n")
print(len(A),"achievements",sum(1 for a in A if a[2]==0),sum(1 for a in A if a[2]==1),sum(1 for a in A if a[2]==2),sum(1 for a in A if a[2]==3))
