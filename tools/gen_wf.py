# v1.3.5 World Flight: 113-aircraft roster -> src/wfdb.inc (exec'd from gen_assets.py; uses tiles4/rgb15 from there)
import math as _m2
_ns={}
_src=open(os.path.join(ROOT,'tools','gen_bigart.py')).read().split("if __name__=='__main__':")[0]
exec(_src,_ns)
exec(open(os.path.join(ROOT,'tools','wf_roster.py')).read(),_ns)
WR=_ns['R'];BPAL=_ns['PAL'];_side=_ns['side'];_bgf=_ns['background']
assert len(WR)==113
_cnt=[sum(1 for r in WR if r[3]==e) for e in range(3)];assert _cnt==[33,20,60],_cnt
# ---- top-down 32x32 OBJ sprite (nose up), per-aircraft 16-colour palette
_RMP={'gray':[(62,65,70),(90,94,100),(120,124,130),(152,156,162),(188,191,195)],'olive':[(56,62,36),(78,86,50),(102,110,66),(130,138,88),(162,168,118)],
 'khaki':[(86,76,52),(116,104,72),(146,134,96),(176,164,124),(206,196,158)],'navy':[(38,48,66),(56,68,90),(78,92,116),(104,120,142),(140,154,172)],
 'silver':[(104,108,114),(138,142,148),(170,174,180),(200,204,208),(232,234,236)],'black':[(28,28,34),(40,40,48),(54,54,62),(70,70,80),(96,96,108)],
 'red':[(110,30,24),(150,44,32),(186,62,44),(214,96,70),(236,140,110)],'yel':[(140,110,20),(190,150,30),(220,184,50),(240,210,90),(250,232,150)],
 'orange':[(160,64,16),(204,92,24),(232,124,40),(246,160,80),(252,200,140)],'metal':[(52,48,46),(78,72,68),(108,100,94),(140,132,124),(176,170,162)]}
def _tpal(sp):
    r=_RMP.get(sp.get('paint','gray'),_RMP['gray'])
    return [(0,0,0),(14,16,14)]+r+[(30,34,36),(140,138,118),(78,72,68),(16,16,20),(236,236,236),(186,62,44),(56,68,90),(240,210,90),(150,150,156)]
def topdown(sp,cat):
    g=[[0]*32 for _ in range(32)];kind=sp.get('kind','jet');L0=sp.get('L',150)
    Lt=max(15,min(30,int(round(L0/190*30))));y0=16-Lt//2;cx=15.5
    bw=max(1.5,sp.get('H',12)*1.45*Lt/L0*0.55)
    def put(x,y,c):
        x=int(round(x));y=int(round(y))
        if 0<=x<32 and 0<=y<32:g[y][x]=c
    def fillpoly(pts,c):
        ys=[p[1] for p in pts]
        for y in range(max(0,int(min(ys))),min(32,int(max(ys))+1)):
            xs=[]
            for i in range(len(pts)):
                (xa,ya),(xb,yb)=pts[i],pts[(i+1)%len(pts)]
                if (ya<=y+0.5<yb) or (yb<=y+0.5<ya):xs.append(xa+(y+0.5-ya)*(xb-xa)/(yb-ya))
            xs.sort()
            for k in range(0,len(xs)-1,2):
                for x in range(int(round(xs[k])),int(round(xs[k+1]))):
                    if 0<=x<32:g[y][x]=c if not callable(c) else c(x,y)
    shade=lambda x,y:2+max(0,min(4,int(4.5-(x-cx+bw)/(2*bw+0.01)*4)))
    if kind=='wing':
        sp_=min(31,int(Lt*1.9));fillpoly([(cx,y0),(cx+sp_/2,y0+Lt*0.7),(cx+sp_/2-2,y0+Lt*0.8),(cx,y0+Lt*0.62),(cx-sp_/2+2,y0+Lt*0.8),(cx-sp_/2,y0+Lt*0.7)],lambda x,y:3 if x<cx else 5)
        fillpoly([(cx-1.5,y0+Lt*0.18),(cx+1.5,y0+Lt*0.18),(cx+1.5,y0+Lt*0.32),(cx-1.5,y0+Lt*0.32)],7)
    else:
        wg=sp.get('wing')
        if kind=='heli':
            pass
        elif wg:
            wp,wc,ws,wh=wg
            span={'jet':0.78,'prop':1.05,'drone':1.9,'rocket':0.6}.get(kind,0.9)*Lt
            if cat=='P' and L0>=160:span=Lt*1.45
            if sp.get('span'):span=sp['span']*Lt
            span=min(31,span);rc=max(2,wc*Lt*1.3);sw=ws/12*span*0.35
            yr=y0+wp*Lt;tc=rc*(0.25 if ws>=16 else 0.45)
            fillpoly([(cx,yr),(cx+span/2,yr+sw),(cx+span/2,yr+sw+tc),(cx,yr+rc)],lambda x,y:4 if y<yr+rc*0.4+abs(x-cx)*sw/(span/2+.1) else 3)
            fillpoly([(cx,yr),(cx-span/2,yr+sw),(cx-span/2,yr+sw+tc),(cx,yr+rc)],lambda x,y:5 if y<yr+rc*0.4+abs(x-cx)*sw/(span/2+.1) else 4)
            for e in sp.get('eng',[]):
                for s_ in (-1,1):
                    for k2 in ((0.22,0.4) if len(sp.get('eng',[]))>1 or L0>=160 else (0.3,)):
                        ex=cx+s_*span*k2;fillpoly([(ex-1,yr-2),(ex+1,yr-2),(ex+1,yr+rc*0.6),(ex-1,yr+rc*0.6)],9)
                break
        # tail plane
        if sp.get('hstab',1) and sp.get('tail','single') not in('none',):
            hs=min(31,(span if (kind!='heli' and wg) else Lt)*0.38+2);yt=y0+Lt*0.86
            fillpoly([(cx,yt-1),(cx+hs/2,yt+1),(cx+hs/2,yt+3),(cx,yt+2),(cx-hs/2,yt+3),(cx-hs/2,yt+1)],lambda x,y:4 if x<cx else 3)
        if sp.get('canard'):fillpoly([(cx,y0+Lt*0.25),(cx+Lt*0.2,y0+Lt*0.3),(cx+Lt*0.2,y0+Lt*0.34),(cx-Lt*0.2,y0+Lt*0.34),(cx-Lt*0.2,y0+Lt*0.3)],4)
        # fuselage
        nl=sp.get('nl',0.22);tl=sp.get('tl',0.3);bm=sp.get('boom',0.28 if kind=='heli' and sp.get('rotor')!='tandem' else 0)
        for yy in range(Lt):
            t=(yy+0.5)/Lt;f=_ns['_prof'](t,nl,tl,sp.get('nose','point'),0,1,bm)
            hw=bw*max(0.25,f)
            for x in range(32):
                if abs(x+0.5-cx-0.5)<=hw+0.01:g[y0+yy][x]=shade(x,y0+yy)
        if sp.get('tail')=='twin':
            for s_ in (-1,1):
                for yy in range(int(y0+Lt*0.78),int(y0+Lt*0.96)):put(cx+s_*bw*0.8,yy,6)
        cp=sp.get('canopy')
        if cp:
            a,l=cp[0],max(0.08,cp[1]);ys_=y0+Lt*(a+0.02);ye=y0+Lt*(a+l)
            for yy in range(int(ys_),int(ye)+1):
                hw=max(0.6,bw*0.45)
                for x in range(32):
                    if abs(x-cx)<=hw and g[yy][x]:g[yy][x]=8 if x<cx and yy<=ys_+1 else 7
        if kind=='heli':
            r=sp.get('rotor','main');R_=min(15.5,(sp.get('rl',0.95))*Lt*0.55)
            cs=[(cx,y0+Lt*0.18),(cx,y0+Lt*0.8)] if r=='tandem' else [(cx-Lt*0.32,y0+Lt*0.35),(cx+Lt*0.32,y0+Lt*0.35)] if r=='tilt' else [(cx,y0+Lt*0.3)]
            for (rx,ry) in cs:
                for k in range(4 if r!='coax' else 6):
                    a=k*_m2.pi*2/(4 if r!='coax' else 6)+0.4
                    for d in range(1,int(R_)):put(rx+_m2.cos(a)*d,ry+_m2.sin(a)*d,15 if d%3 else 9)
                put(rx,ry,10)
        if sp.get('prop'):
            for d in range(-4,5):put(cx+d,y0-0 if True else 0,15)
        if sp.get('pusher'):
            for d in range(-3,4):put(cx+d,y0+Lt-1,15)
        mk=sp.get('mark')
        if mk and kind not in('heli',) and wg:
            for s_ in (-1,1):
                mx=int(round(cx+s_*span*0.32));my=int(round(y0+wg[0]*Lt+wg[1]*Lt*0.6+sw*0.6))
                if mk=='cross':
                    for d in (-1,0,1):
                        for (ax,ay) in ((mx+d,my),(mx,my+d)):
                            if 0<=ay<32 and 0<=ax<32 and g[ay][ax]:g[ay][ax]=10
                elif mk=='hino':
                    if g[my][mx]:g[my][mx]=12;
                    for (ax,ay) in ((mx+1,my),(mx,my+1),(mx+1,my+1)):
                        if g[ay][ax]:g[ay][ax]=12
                else:
                    if g[my][mx]:g[my][mx]=13 if mk=='star' else 12
                    if mk=='roundel':
                        for (ax,ay) in ((mx-1,my),(mx+1,my),(mx,my-1),(mx,my+1)):
                            if g[ay][ax]:g[ay][ax]=13
    # outline
    o=[r[:] for r in g]
    for y in range(32):
        for x in range(32):
            if not g[y][x] and any(0<=y+dy<32 and 0<=x+dx<32 and g[y+dy][x+dx] not in(0,15) for dx,dy in((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=1
    return o
# ---- big side art: crop + RLE (run<=255, colour) over the bounding box
def _rle(g):
    out=[];prev=None;n=0
    for row in g:
        for c in row:
            if c==prev and n<255:n+=1
            else:
                if prev is not None:out+=[n,prev]
                prev=c;n=1
    out+=[n,prev];return out
WBIG=[];WOFF=[];WTI=[];WPL=[];WBX=[]
for r in WR:
    sp=r[12];a=_side(sp);g=a.idx
    ys=[y for y in range(len(g)) if any(g[y])];xs=[x for x in range(len(g[0])) if any(g[y][x] for y in range(len(g)))]
    x0,x1,y0_,y1=min(xs),max(xs)+1,min(ys),max(ys)+1
    cg=[row[x0:x1] for row in g[y0_:y1]]
    WOFF.append(len(WBIG));WBX.append((x1-x0,y1-y0_,y0_));WBIG+=_rle(cg)
    WTI+=tiles4(topdown(sp,r[2]),32,32);WPL+=[rgb15(c) for c in _tpal(sp)]
WBIG+=[0]*((-len(WBIG))%4)
emit("wf_big",WBIG,"unsigned char");emit("wf_bpal",[rgb15(c) for c in BPAL]);emit("wf_tiles",WTI,"unsigned int");emit("wf_pal",WPL)
_bg=_bgf(240,72,3);emit("wf_band",[c for row in _bg for c in row],"unsigned char")
print("wf: big",len(WBIG),"bytes; tiles",len(WTI)*4)
# ---- C table
def _q(s):return '"'+s.replace('\\','\\\\').replace('"','\\"')+'"'
CATN={'P':0,'D':1,'H':2,'X':3,'W':4}
L_=["// generated by tools/gen_wf.py - do not edit","#define NWAC %d"%len(WR),
"typedef struct{const char*n,*s,*cty,*f1,*f2;s32 fuel,ceil;s16 thrq,stall,vmax;u8 rocket,turnk,cat,era,agil,bw,bh,by;u32 off;}WAc;",
"static const WAc WAC[NWAC]={"]
for i,r in enumerate(WR):
    n,s_,cat,era,cty,mach,ceil,agi,end,rk,f1,f2,sp=r
    vmax=int(mach*10000);heli=cat=='H'
    if heli:alt=2000
    elif sp.get('kind')=='prop' or (cat=='D' and mach<0.5):alt=0.6*ceil
    else:alt=0.75*ceil
    if rk:alt=min(alt,0.3*ceil if rk==1 else 0.6*ceil)
    d=2**(-alt/20000.0);thr=vmax*vmax*d/2.75e6
    if rk:thr*=1.6
    thrq=max(2,int(round(thr*(4 if rk else 8))))
    stall=0 if heli else int(max(700,min(2400,vmax*(0.2 if rk else 0.25))))
    fuel=int(end*(2700 if rk else 3600))
    turnk=50+agi*10
    w,h,by=WBX[i]
    L_.append(" {%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%du},"%(_q(n),_q(s_),_q(cty),_q(f1),_q(f2),fuel,ceil,thrq,stall,vmax,rk,turnk,CATN[cat],era,agi,w,h,by,WOFF[i]))
L_.append("};")
# old (v1.3.4) indices 0..3 = X-15, SR-71, Concorde, P-51D
_names=[r[0] for r in WR]
_old=[next(i for i,n in enumerate(_names) if 'X-15' in n),next(i for i,n in enumerate(_names) if 'SR-71' in n),next(i for i,n in enumerate(_names) if 'CONCORDE' in n),next(i for i,n in enumerate(_names) if 'P-51D' in n)]
L_.append("static const u8 WAC_OLD[4]={%s};"%",".join(map(str,_old)))
open(os.path.join(ROOT,'src','wfdb.inc'),'w').write("\n".join(L_)+"\n")
