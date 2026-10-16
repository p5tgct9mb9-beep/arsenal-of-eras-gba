#!/usr/bin/env python3
# v1.3.3: translate a v1.3.2-menu harness script (13-row main menu -> category list -> options) into the new tile menu.
# Simulates the old main menu + category screen up to the first action that leaves them, then emits the equivalent
# tile / submenu taps and copies the rest of the script verbatim. Prints nothing and exits 1 when it can't translate.
import sys,re
MD="CAMP INF TANK AIR SEA SWORD SPEC BRAD SNIPE EOD AA DRONE LAUNCH FLIGHT LRRP FPS SPACE ASTRO MOON MARS ODEF RBUILD ASTER WHATIF HIST OVER GP TT FREE BUILD SAND INVADE VBM FIRE BOOT LOADOUT SETTINGS CREDITS CODES".split()
M={n:i for i,n in enumerate(MD)}
OLD=[(4,['CAMP']),(7,'INF TANK AIR SEA SWORD FPS SAND'.split()),(2,['SPEC','BRAD']),(6,'SNIPE EOD LRRP AA DRONE OVER'.split()),
     (12,'SPACE ASTRO LAUNCH FLIGHT MOON MARS ODEF RBUILD ASTER WHATIF HIST VBM'.split()),(0,['LOADOUT']),(0,['SETTINGS']),(0,['BOOT']),
     (3,['GP','TT','FREE']),(1,['BUILD']),(1,['INVADE']),(1,['FIRE']),(0,['CREDITS'])]
NEW=[['CAMP'],'INF TANK AIR SEA SWORD FPS'.split(),'FIRE SPEC BRAD SNIPE EOD LRRP'.split(),'AIR AA DRONE FLIGHT OVER'.split(),
     'LAUNCH SPACE ASTRO VBM MOON MARS ODEF ASTER'.split(),'INVADE GP TT FREE'.split(),'BOOT HIST WHATIF'.split(),'BUILD RBUILD SAND LOADOUT'.split(),['STORE'],'CODES CREDITS SETTINGS'.split()]
def ops(md):
    i=M[md]
    if i>=M['MOON']:return ['PLAY','REC','HOW']
    o=['PLAY']
    if md in('INF','TANK','AIR','SEA'):o+=['MAP','SIDE']
    if md not in('LAUNCH','FLIGHT','CAMP','SPACE','ASTRO'):o.append('BOTS')
    if md not in('CAMP','SPACE','ASTRO','FPS'):o.append('LOAD')
    return o+['REC','HOW']
def tile_of(md):
    for t,l in enumerate(NEW):
        if md in l:return t,l.index(md)
def nav_tiles(a,b):
    out=[]
    while a!=b:
        if (a^1)==b:out.append('RIGHT' if a%2==0 else 'LEFT');a^=1
        elif (b-a)%10 in (8,9):out.append('UP');a=(a+8)%10
        else:out.append('DOWN');a=(a+2)%10
    return out
def xlate(text):
    if 'corereset' in text or '#newmenu' in text:return None  # multi-boot scripts keep the legacy menu flag
    L=text.split('\n')
    # skip to the title press
    i=0;out=[]
    while i<len(L) and not re.match(r'^tap (START|A)\s*$',L[i]):out.append(L[i]);i+=1
    if i>=len(L):return None
    out.append(L[i]);i+=1
    while i<len(L) and (L[i].startswith('wait') or L[i].startswith('poke32') or L[i].startswith('#')):out.append(L[i]);i+=1
    sel=0;st='main';c=0;cs=0;foc=0;os_=0;pending=[]
    while i<len(L):
        ln=L[i].strip();i+=1
        m=re.match(r'^tap (\w+)$',ln)
        if ln.startswith('wait') or ln.startswith('beat') or ln.startswith('shot') or ln.startswith('poke32') or ln.startswith('#') or ln=='':
            if not (ln.startswith('wait')):pending.append(ln)
            continue
        if not m:return None
        k=m.group(1)
        if st=='main':
            if k=='UP':sel=(sel+12)%13
            elif k=='DOWN':sel=(sel+1)%13
            elif k in('LEFT','RIGHT'):sel=(sel+4)%8 if sel<8 else (0 if k=='LEFT' else 12)
            elif k in('A','START'):
                n,l=OLD[sel]
                if n==0:  # direct screens -> tile + submenu entry (pseudo: A opens directly)
                    md=l[0];break
                st='cat';c=sel;cs=0;foc=0;os_=0
            elif k=='B':return None
            else:return None
        else:
            n,l=OLD[c];md=l[cs]
            if not foc:
                if k=='UP':cs=(cs+n-1)%n
                elif k=='DOWN':cs=(cs+1)%n
                elif k in('A','RIGHT','START'):foc=1;os_=0
                elif k=='B':st='main'
                else:return None
            else:
                o=ops(md)
                if k=='UP':os_=(os_+len(o)-1)%len(o)
                elif k=='DOWN':os_=(os_+1)%len(o)
                elif k in('B','LEFT'):foc=0
                elif k in('A','START'):
                    if o[os_]=='BOTS':return None  # cycles in place; rare - keep legacy
                    md=l[cs] if c!=0 else 'CAMP';break
                elif k=='RIGHT' and o[os_]=='BOTS':return None
                else:return None
    else:return None
    # emit new navigation
    t,idx=tile_of(md)
    em=[]
    for k in nav_tiles(0,t):em+=['tap '+k,'wait 6']
    em+=['wait 10','tap A','wait 50']
    if md=='CAMP':idx=cs  # era row is the same
    for _ in range(idx):em+=['tap DOWN','wait 6']
    if md in('LOADOUT','SETTINGS','BOOT','CREDITS'):em+=['tap A']
    else:
        em+=['tap A','wait 12']
        for _ in range(os_):em+=['tap DOWN','wait 6']
        em+=['tap A']
    return "\n".join(['#newmenu','# translated from the v1.3.2 menu by tools/menu133_xlate.py']+out+pending+em+L[i:])
if __name__=='__main__':
    r=xlate(open(sys.argv[1]).read())
    if r is None:sys.exit(1)
    sys.stdout.write(r)
