# v1.2 SPEC OPS: course builder. Assembles each course from parametric hand-designed segments,
# validates it with a conservative reachability model (BFS over standing cells), RLE-compresses it and emits pf_lv_* arrays.
import random,os
TID={'.':0,'#':1,'B':3,'?':4,'S':6,'-':7,'L':8,'R':9,'~':10,'w':11,'^':12,'H':13,'D':15,'G':16,'b':17,'s':18,'I':20,'C':21,'<':22,'>':23,'K':24,'[':25,']':26,'{':27,'}':28,'V':29,'v':30,'T':31,'X':32}
SOLID=set('#B?SDIK[]{}<>')
STAND=SOLID|set('-CT')
HAZ=set('^Vv')
WATER=set('~w')
CLIMB=set('LR')
E=dict(RIFLE=1,MG=2,RPG=3,SNIPER=4,DRONE=5,LIGHT=6,HEAVY=7,INTEL=10,TAG=11,AMMO=12,MED=13,ARMOR=14,RAPID=15,JET=16,ONEUP=17,SPACEW=18,
       CHECK=20,FLAG=21,SECRET=22,FIREBAR=24,MPH=25,MPV=26,CRUSHER=27,CANNON=28,PSWITCH=29,WARP=30,WARPDST=31,WIND=32,BOSS=34,RAFT=35)
H_=12
class Lv:
    def __init__(s):s.g=[[] for _ in range(H_)];s.ents=[];s.h=9;s.virt=set();s.intel=0
    @property
    def W(s):return len(s.g[0])
    def add(s,n,h=None):
        if h is None:h=s.h
        x0=s.W
        for i in range(n):
            for y in range(H_):s.g[y].append('#' if y>=h and h<H_ else '.')
        return x0
    def set(s,x,y,c):
        if 0<=y<H_ and 0<=x<s.W:s.g[y][x]=c
    def get(s,x,y):
        if y<0:return '.'
        if y>=H_:return '.'
        if x<0 or x>=s.W:return 'S'
        return s.g[y][x]
    def ent(s,t,x,y,p=0):s.ents.append((E[t],p,x,y))
    def intel_at(s,x,y):
        if s.intel<3:s.ent('INTEL',x,y,s.intel);s.intel+=1
def clampH(h):return max(5,min(10,h))
def enemies(L,r,x0,x1,d,cf):
    n=0
    for x in range(x0+1,x1):
        if r.random()<0.07+d*0.035 and n<2+d//2:
            k=r.choice(cf['etypes'][:2+min(d,3)])
            if k in('LIGHT','HEAVY') and x1-x0<7:k='RIFLE'
            L.ent(k,x,L.h-1 if k!='DRONE' else max(1,L.h-4));n+=1
def s_flat(L,r,d,cf):
    n=r.randint(4,8);x=L.add(n);enemies(L,r,x,x+n,d,cf)
    if r.random()<.4:
        for i in range(min(4,n-2)):L.ent('TAG',x+1+i,L.h-3-(1 if 0<i<3 else 0))
    if r.random()<.3:L.set(x+r.randrange(n),L.h-1,'b' if r.random()<.6 else 's')
    if d>=3 and r.random()<.25 and n>=7:L.ent(r.choice(['LIGHT','HEAVY'] if d>=4 else ['LIGHT']),x+n//2,L.h-1)
def s_gap(L,r,d,cf):
    L.add(1);w=r.randint(2,min(cf['gmax'],1+d));L.add(w,H_);L.h=clampH(L.h+r.choice([-1,0,0,1]));L.add(3)
def s_steps(L,r,d,cf):
    k=r.randint(1,min(3,1+d//2));dirn=r.choice([-1,1])
    for i in range(k):L.h=clampH(L.h+dirn);L.add(2)
    L.add(2)
def s_plats(L,r,d,cf):
    L.add(1);n=r.randint(6,8+d);x0=L.add(n,H_);y=L.h-1
    for x in range(x0+1,x0+n-1,3):
        y=max(3,min(L.h,y+r.choice([-1,0,1])))
        L.set(x,y,'-');L.set(x+1,y,'-')
        if r.random()<.3:L.ent('TAG',x,y-2)
    L.add(3)
def s_moving(L,r,d,cf):
    L.add(2);n=r.randint(6,9);x0=L.add(n,H_);y=L.h
    L.ent('MPH',x0,y,n-2)
    for x in range(x0,x0+n):L.virt.add((x,y))
    L.add(3)
def s_vmoving(L,r,d,cf):
    L.add(2);x0=L.add(1);L.add(4,H_);hi=max(5,L.h-4);L.ent('MPV',x0+2,L.h,L.h-hi)
    for y in range(hi,L.h+1):L.virt.add((x0+2,y));L.virt.add((x0+3,y))
    L.h=hi;L.add(4)
def s_crumble(L,r,d,cf):
    L.add(2);n=r.randint(4,6+d);x0=L.add(n,H_)
    for x in range(x0,x0+n):L.set(x,L.h,'C')
    L.add(3)
def s_bricks(L,r,d,cf):
    n=8;x=L.add(n);y=L.h-4;pat=r.choice(["B?B?B","?BHB?","BB?BB","?H?","B?H?B"])
    for i,c in enumerate(pat):
        L.set(x+2+i,y,c)
        if c in '?H':
            it=r.choice(['TAG','TAG','AMMO','MED','ARMOR','RAPID'] if c=='?' else ['ONEUP','ARMOR','RAPID','MED'])
            if cf.get('space') and r.random()<.4:it='SPACEW'
            L.ent(it,x+2+i,y)
    enemies(L,r,x,x+n,d,cf)
def s_pipe(L,r,d,cf):
    x=L.add(7);hh=r.randint(2,3);px=x+3
    for y in range(L.h-hh,L.h):
        top=y==L.h-hh;L.set(px,y,'[' if top else '{');L.set(px+1,y,']' if top else '}')
    if not cf['warped'] and r.random()<.7:
        cf['warped']=1;L.ent('WARP',px,L.h-hh-1);cf['warp_back']=(px+3,L.h-1)
def s_cannon(L,r,d,cf):
    x=L.add(7);k=x+3+r.randint(0,2);L.set(k,L.h-1,'K')
    if d>=3 and r.random()<.6:L.set(k,L.h-2,'K');L.ent('CANNON',k,L.h-2,0)
    else:L.ent('CANNON',k,L.h-1,0)
def s_firebar(L,r,d,cf):
    x=L.add(8);fy=L.h-3 if r.random()<.6 else L.h-4;L.set(x+4,fy,'S');L.ent('FIREBAR',x+4,fy,(2+min(d,4))|((1 if r.random()<.5 else 0)<<4))
def s_crusher(L,r,d,cf):
    n=r.randint(7,9+d);x=L.add(n);cy=L.h-6
    if cy<0:return
    for xx in range(x,x+n):
        for y in range(0,cy+1):L.set(xx,y,'S')
    for xx in range(x+2,x+n-1,3 if d>=4 else 4):L.ent('CRUSHER',xx,cy+1,0)
def s_conveyor(L,r,d,cf):
    n=r.randint(6,9);x=L.add(n);c=r.choice('<>')
    for xx in range(x,x+n):L.set(xx,L.h,c)
    enemies(L,r,x,x+n,d,cf)
def s_spikes(L,r,d,cf):
    L.add(1);w=r.randint(2,min(cf['gmax'],2+d//2));x0=L.add(w,H_)
    for xx in range(x0,x0+w):L.set(xx,11,'^')
    L.add(3)
def s_wire(L,r,d,cf):
    x=L.add(8)
    for i in range(r.randint(1,2)):L.set(x+2+i*3,L.h-1,'^')
def s_lava(L,r,d,cf):
    L.add(1);w=r.randint(2,min(cf['gmax'],2+d//2));x0=L.add(w,H_)
    for xx in range(x0,x0+w):L.set(xx,10,'V');L.set(xx,11,'v')
    L.add(3)
def s_spring(L,r,d,cf):
    x=L.add(3);L.set(x+2,L.h,'T')
    wall=r.randint(4,5);top=max(3,L.h-wall)
    x1=L.add(3,top);L.ent('TAG',x1+1,top-2)
    L.add(3)
def s_water(L,r,d,cf):
    L.add(1);n=r.randint(6,10);x0=L.add(n,H_)
    for xx in range(x0,x0+n):
        L.set(xx,L.h,'~')
        for y in range(L.h+1,11):L.set(xx,y,'w')
        L.set(xx,11,'#')
    if r.random()<.5:L.ent('TAG',x0+n//2,L.h+1)
    L.add(3)
def s_tower(L,r,d,cf):
    x=L.add(6);top=max(2,L.h-6)
    for y in range(top,L.h):L.set(x+2,y,'L')
    for xx in (x+3,x+4):L.set(xx,top,'-')
    L.intel_at(x+4,top-1)
def s_rope(L,r,d,cf):
    x=L.add(7);top=max(2,L.h-6)
    for xx in range(x+1,x+6):L.set(xx,top,'S')
    for y in range(top+1,L.h):L.set(x+1,y,'R')
    L.set(x+1,top,'R');L.ent('TAG',x+3,top-1);L.ent('TAG',x+4,top-1)
    if L.intel<3 and r.random()<.5:L.intel_at(x+5,top-1)
def s_vault(L,r,d,cf):
    x=L.add(9);h=L.h
    for y in range(h-2,h):L.set(x+3,y,'S');L.set(x+7,y,'S')
    for xx in range(x+3,x+8):L.set(xx,h-3,'S')
    L.set(x+3,h-1,'D');L.intel_at(x+5,h-1);L.ent('TAG',x+6,h-1)
def s_switch(L,r,d,cf):
    x=L.add(10);L.ent('PSWITCH',x+1,L.h-1);y=max(2,L.h-5)
    for xx in range(x+3,x+9):L.set(xx,y,'X');L.ent('TAG',xx,y-1)
def s_grapple(L,r,d,cf):
    x=L.add(7);top=max(2,L.h-7)
    for xx in range(x+3,x+6):L.set(xx,top,'-')
    L.set(x+2,max(0,top-2),'G');L.ent('ONEUP' if r.random()<.3 else 'TAG',x+4,top-1)
def s_jet(L,r,d,cf):
    x=L.add(3);L.ent('JET',x+1,L.h-1);n=r.randint(10,14);x0=L.add(n,H_)
    for xx in range(x0,x0+n):L.virt.add((xx,L.h))
    L.add(4)
def s_hidden(L,r,d,cf):
    x=L.add(6)
    for xx in range(x+1,x+4):L.set(xx,L.h-4,'H')
    L.ent('ONEUP',x+2,L.h-4)
SEGS=dict(flat=s_flat,gap=s_gap,steps=s_steps,plats=s_plats,moving=s_moving,vmoving=s_vmoving,crumble=s_crumble,bricks=s_bricks,pipe=s_pipe,cannon=s_cannon,
          firebar=s_firebar,crusher=s_crusher,conveyor=s_conveyor,spikes=s_spikes,wire=s_wire,lava=s_lava,spring=s_spring,water=s_water,tower=s_tower,
          rope=s_rope,vault=s_vault,switch=s_switch,grapple=s_grapple,jet=s_jet,hidden=s_hidden)
WPOOL=[
 ['plats','moving','spring','crumble','pipe','bricks','rope'],
 ['crumble','cannon','firebar','switch','plats','pipe','spring'],
 ['spikes','conveyor','cannon','moving','crusher','vmoving','plats'],
 ['water','moving','firebar','pipe','plats','vmoving','crumble'],
 ['conveyor','crusher','firebar','switch','plats','hidden','cannon'],
 ['crumble','cannon','firebar','lava','crusher','spring','vmoving'],
 ['conveyor','crusher','cannon','lava','spikes','moving','firebar'],
 ['water','spikes','wire','cannon','crumble','plats','rope'],
 ['cannon','crusher','crumble','firebar','lava','wire','moving'],
 ['plats','moving','firebar','conveyor','crusher','lava','switch','vmoving'],
 ['wire','spikes','cannon','crumble','plats','hidden','moving'],      # 11 WW1 trenches: wire, shellfire
 ['rope','plats','water','crumble','cannon','spring','moving'],       # 12 colonial frontier
 ['firebar','crusher','switch','hidden','vmoving','plats','spikes'],  # 13 castle traps
]
ETYPES=[['RIFLE','RIFLE','MG','DRONE','RPG','SNIPER']]*7+[['RIFLE','RIFLE','MG','RPG','SNIPER','RIFLE']]*2+[['RIFLE','DRONE','MG','RPG','SNIPER','DRONE']]+[['RIFLE','RIFLE','MG','RPG','SNIPER','RIFLE']]+[['RIFLE','RIFLE','RIFLE','SNIPER','RIFLE','MG']]*2
def validate(L,goal,lowg=False):
    W=L.W
    def free(x,y):return L.get(x,y) not in SOLID
    def standable(x,y):
        if not(0<=x<W and 0<=y<H_):return False
        c=L.get(x,y)
        if c in SOLID or c in HAZ:return False
        if c in CLIMB or c in WATER:return True
        b=L.get(x,y+1)
        return (b in STAND) or ((x,y+1) in L.virt)
    J=5 if lowg else 3
    start=None
    for y in range(H_):
        if standable(1,y):start=(1,y);break
    if not start:return False,0
    seen={start};q=[start];best=0
    while q:
        x,y=q.pop();best=max(best,x)
        if abs(x-goal[0])<=1 and abs(y-goal[1])<=1:return True,x
        cand=[];c=L.get(x,y)
        if c in CLIMB or c in WATER:
            for dx,dy in ((0,-1),(0,1),(1,0),(-1,0),(1,-1),(-1,-1)):cand.append((x+dx,y+dy))
            if c in WATER:
                for dx in (-1,1):
                    for up in (1,2):cand.append((x+dx,y-up))
        onground=L.get(x,y+1) in STAND or (x,y+1) in L.virt or c in WATER or c in CLIMB
        if not onground:continue
        jr=J+(3 if L.get(x,y+1)=='T' else 0)
        for dx in range(-5,6):
            for dy in range(-jr,10):
                tx,ty=x+dx,y+dy
                if (tx,ty) in seen or not standable(tx,ty):continue
                up=-dy;adx=abs(dx)
                lim=(2 if up>=3 else 3 if up==2 else 4) if up>0 else 5
                if lowg:lim+=2
                if adx>lim:continue
                apex=min(y,ty)-(1 if (up>0 or adx>1) else 0)
                ok=all(free(x,yy) for yy in range(max(0,apex),y))
                if ok and adx>0:
                    sgn=1 if dx>0 else -1
                    for cx in range(x+sgn,tx+sgn,sgn):
                        if not free(cx,max(0,apex)) or not free(cx,min(y,ty)):ok=False;break
                if ok:cand.append((tx,ty))
        for t in cand:
            if t not in seen and standable(*t):seen.add(t);q.append(t)
    return False,best
def build(w,s,stars,flags,ai,seed):
    r=random.Random(seed);L=Lv();d=stars
    cf=dict(etypes=ETYPES[w],gmax=3 if 'n' in flags else 4,warped=0,wind='n' in flags,space=w==9)
    L.add(6)
    if 'b' in flags or 'a' in flags:return build_auto(w,stars,flags,r,L,cf)
    nseg=(8+3*d+r.randint(0,3)) if not ai else 6+d
    if 'k' in flags:nseg=7+d
    pool=WPOOL[w][:3+d]+['flat','gap','steps','bricks','pipe','flat']
    if 'w' in flags:pool+=['water','water']
    if 's' in flags:pool+=['spikes','wire']
    if 'v' in flags:pool+=['vault']
    if 'j' in flags:pool+=['jet','jet']
    if 'k' in flags:pool+=['bricks','hidden','switch','flat']
    if 't' in flags:pool=[p for p in pool if p not in('tower','rope','grapple','spring','jet','vmoving','switch','crusher','plats')]
    special=['vault','hidden'] if 't' in flags else ['tower','vault',r.choice(['rope','grapple','tower'])]
    order=[]
    for i in range(nseg):
        k=r.choice(pool)
        if order and k==order[-1]:k='flat'
        order.append(k)
    if 'x' in flags and 'pipe' not in order:order.insert(2,'pipe')
    for sp,pos in zip(special,(nseg//4,nseg//2+1,3*nseg//4)):order.insert(pos,sp)
    mid=len(order)//2
    for i,k in enumerate(order):
        if i==mid:x=L.add(3);L.ent('CHECK',x+1,L.h-1)
        if cf['wind'] and i==2:L.ent('WIND',L.W,0,1)
        if cf['wind'] and i==len(order)-3:L.ent('WIND',L.W,0,0)
        if k=='pipe' and 'x' in flags and not cf['warped']:
            x=L.add(7);hh=2;px=x+3
            for y in range(L.h-hh,L.h):L.set(px,y,'[' if y==L.h-hh else '{');L.set(px+1,y,']' if y==L.h-hh else '}')
            cf['warped']=1;L.ent('WARP',px,L.h-hh-1);cf['warp_back']=(px+3,L.h-1);continue
        SEGS[k](L,r,d,cf)
    if ai:
        L.h=9;x0=L.add(2);L.ent('BOSS',x0+1,L.h-1);L.add(22)
        for y in range(0,12):L.set(L.W-1,y,'S')
        goal=(x0+1,L.h-1)
    else:
        x=L.add(7);L.ent('FLAG',x+4,L.h-1);goal=(x+4,L.h-1)
        L.set(x+6,L.h-1,'S')
    return finish(L,r,cf,flags,goal)
def build_auto(w,stars,flags,r,L,cf):
    d=stars;boat='b' in flags;n=16+d*4
    if boat:
        for i in range(n):
            if r.random()<.35:x=L.add(r.randint(4,6));enemies(L,r,x,L.W,d,cf)
            else:
                x0=L.add(r.randint(6,10),H_)
                for xx in range(x0,L.W):
                    L.set(xx,9,'~')
                    for y in range(10,12):L.set(xx,y,'w')
                if r.random()<.4:L.ent('DRONE' if w==3 else 'RIFLE',x0+2,6)
                if r.random()<.3:L.ent('TAG',x0+3,8)
            if i==n//2:L.ent('CHECK',L.W-2,L.h-1)
        L.ents.insert(0,(E['RAFT'],0,3,8));L.h=9
    else:
        L.h=8
        for i in range(n):
            k=r.random();nh=clampH(L.h+r.choice([-1,0,1]));gw=r.randint(2,min(3 if nh<L.h else 4,1+d));L.add(gw,H_)
            L.h=nh;seg=r.randint(4,8);x1=L.add(seg)
            for xx in range(x1,x1+seg):
                for y in range(L.h,12):L.set(xx,y,'.' if y>L.h+2 else 'S')
            if k<.3:L.set(x1+seg//2,L.h-1,'K');L.ent('CANNON',x1+seg//2,L.h-1,0)
            elif k<.45 and d>=3:L.set(x1+seg//2,L.h-3,'S');L.ent('FIREBAR',x1+seg//2,L.h-3,3|((i&1)<<4))
            elif k<.75:enemies(L,r,x1,x1+seg,d,cf)
            else:L.ent('TAG',x1+1,L.h-2);L.ent('TAG',x1+2,L.h-2)
            if i==n//2:L.ent('CHECK',x1+1,L.h-1)
        L.intel_at(L.W-4,L.h-3)
    x=L.add(7);L.ent('FLAG',x+4,L.h-1);goal=(x+4,L.h-1)
    return finish(L,r,cf,flags,goal)
def finish(L,r,cf,flags,goal):
    if 't' in flags:
        for x in range(L.W):
            for y in range(0,2):
                if L.get(x,y)=='.':L.set(x,y,'#')
    if cf['warped']:
        bx=L.add(1,0)
        for y in range(H_):L.set(bx,y,'S')
        rx=L.add(18,10)
        for xx in range(rx,rx+18):L.set(xx,0,'S');L.set(xx,1,'S')
        for y in range(H_):L.set(L.W-1,y,'S')
        L.ents.append((E['WARPDST'],0,rx+1,9))
        for i in range(10):L.ent('TAG',rx+3+i,6+(i%2))
        L.intel_at(rx+5,9)
        if 'x' in flags:L.ent('SECRET',rx+15,9)
        px=rx+9;L.set(px,9,'[');L.set(px+1,9,']');L.ent('WARP',px,8,1);bx2,by2=cf['warp_back'];L.ents.append((E['WARPDST'],1,bx2,by2))
    tries=0
    while L.intel<3 and tries<200:
        tries+=1;x=r.randrange(8,max(9,L.W-30))
        for y in range(1,10):
            if L.get(x,y)=='.' and L.get(x,y+1) in STAND:L.intel_at(x,y);break
    return L,goal
def rle(L):
    out=[]
    for y in range(H_):
        row=[TID[c] for c in L.g[y]];i=0
        while i<len(row):
            j=i
            while j<len(row) and row[j]==row[i] and j-i<255:j+=1
            out+=[row[i],j-i];i=j
    return out
stats={1:0,2:0,3:0,4:0,5:0};byw=[];tot_raw=tot_c=fails=0;info=[]
for w in range(len(ST)):
    cnt={1:0,2:0,3:0,4:0,5:0}
    for s,stg in enumerate(ST[w]):
        n,th,es,ep,ai,ba,bb,bp,fl=stg;stars=STARS[w][s]
        for attempt in range(80):
            L,goal=build(w,s,stars,fl,ai,w*1000+s*37+attempt*7919)
            ok,reach=validate(L,goal,lowg=('l' in fl or 'z' in fl))
            if 'b' in fl:ok=True
            if ok and 'x' in fl and not any(e[0]==E['SECRET'] for e in L.ents):ok=False
            if ok:break
        if not ok:fails+=1;print("COURSE NOT VALIDATED",w+1,s+1,n,reach,L.W)
        os.makedirs(os.path.join(ROOT,'build','pf_ascii'),exist_ok=True);open(os.path.join(ROOT,'build','pf_ascii','%d_%d.txt'%(w+1,s+1)),'w').write('\n'.join(''.join(r) for r in L.g)+'\n'+repr(L.ents))
        data=rle(L);tot_raw+=L.W*H_;tot_c+=len(data)
        ents=[]
        for t,p,x,y in L.ents:ents+=[t|(p<<8),x,y]
        emit("pf_lv_%d_%d"%(w,s),data,"unsigned char");emit("pf_le_%d_%d"%(w,s),ents if ents else [0,0,0])
        info.append((w,s,L.W,len(L.ents),attempt,ok));cnt[stars]+=1;stats[stars]+=1
        if s in (0,4,len(ST[w])-1):
            from PIL import Image
            col={'.':(150,200,250),'#':(110,80,40),'B':(200,100,60),'?':(250,200,60),'S':(120,120,130),'-':(200,150,90),'L':(150,110,50),'R':(150,110,50),'~':(60,140,230),'w':(40,100,200),'^':(230,230,230),'H':(220,220,255),'D':(90,90,100),'G':(250,200,0),'b':(60,160,60),'s':(200,180,120),'C':(210,160,100),'<':(80,80,90),'>':(80,80,90),'K':(40,40,50),'[':(60,170,90),']':(60,170,90),'{':(50,140,70),'}':(50,140,70),'V':(250,90,20),'v':(220,60,20),'T':(250,60,60),'X':(110,110,250),'I':(0,0,0)}
            im=Image.new('RGB',(L.W,H_))
            for y in range(H_):
                for x in range(L.W):im.putpixel((x,y),col[L.g[y][x]])
            for t,p,x,y in L.ents:
                if 0<=x<L.W and 0<=y<H_:im.putpixel((x,y),(255,0,0) if t<10 else (255,255,0) if t<20 else (255,0,255))
            im.resize((L.W*4,H_*4),Image.NEAREST).save(os.path.join(ROOT,'build','pf_course_%d_%d.png'%(w+1,s+1)))
    byw.append(cnt)
hh=["// generated by tools/gen_levels.py","typedef struct{const u8*t;const u16*e;u16 w,ne;}PfLv;"]
rows=[]
for w in range(len(ST)):
    ent=["{pf_lv_%d_%d,pf_le_%d_%d,%d,%d}"%(w,s,w,s,W_,ne) for (ww,s,W_,ne,a,ok) in info if ww==w]
    ent+=["{0,0,0,0}"]*(10-len(ent));rows.append("{"+",".join(ent)+"}")
hh.append("static const PfLv PF_LV[PF_NW][PF_MS]={"+",".join(rows)+"};")
hh.append("#define PF_MAXW %d"%max(i[2] for i in info))
open(os.path.join(ROOT,'src','pflevels.inc'),'w').write("\n".join(hh)+"\n")
print("courses: %d total, by stars %s, rle %d->%d bytes, %d unvalidated, max width %d"%(sum(stats.values()),stats,tot_raw,tot_c,fails,max(i[2] for i in info)))
for w in range(len(ST)):print("  world %d: %d courses %s"%(w+1,sum(byw[w].values()),{k:v for k,v in byw[w].items() if v}))
open(os.path.join(ROOT,'build','pf_course_stats.txt'),'w').write(repr((stats,byw,info)))
