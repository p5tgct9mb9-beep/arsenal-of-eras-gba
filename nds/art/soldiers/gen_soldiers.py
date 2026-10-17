#!/usr/bin/env python3
"""v1.3.6 soldiers: 3/4 top-down infantry sprites (32x32, split 32x16 upper/lower) and 72px creator portraits.
Rendered with tools/art/sold.py (shaded polygon painter). Materials are encoded with code colours so the GBA can
recolour skin / hair / uniform / camo / helmet / gear / boots at runtime. Output: src/soldierdb.inc"""
import sys,os,math
sys.path.insert(0,os.path.join(os.path.dirname(os.path.abspath(__file__)),'art'))
import sold
from sold import spec,sprite,mix,mul
from PIL import Image
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TRUE=dict(SKINS=list(sold.SKINS),hr=sold.hr,cloth=sold.cloth,R=sold.R,helm_ramp=sold.helm_ramp,LEATHER=sold.LEATHER,BLKB=sold.BLKB,TANB=sold.TANB,WEB=sold.WEB,WEBT=sold.WEBT,WEBB=sold.WEBB)
M_SKIN,M_HAIR,M_UNI,M_HELM,M_GEAR,M_BOOT,M_BERET=1,2,3,4,5,6,7
def code(m,n): return [(250,m*8+k,3) for k in range(n)]
def set_code(s):
    base=sold.UNIF[s['unif']]; plate=mix(base,(80,82,62),0.45); beret=s.get('beretcol') or (110,30,36)
    trueR=TRUE['R']
    def R(b,lo=0.38,hi=1.3):
        b=tuple(b)
        if b==tuple(base): return code(M_UNI,7)
        if b==tuple(plate): return code(M_GEAR,7)
        if b==tuple(beret): return code(M_BERET,7)
        return trueR(b,lo,hi)
    sold.SKINS=[code(M_SKIN,6)]*6; sold.hr=lambda b:code(M_HAIR,6); sold.cloth=lambda b,k:dict(ramp=code(M_UNI,7))
    sold.R=R; sold.helm_ramp=lambda s:code(M_HELM,7)
    for k in ('LEATHER','BLKB','TANB'): setattr(sold,k,code(M_BOOT,7))
    for k in ('WEB','WEBT','WEBB'): setattr(sold,k,code(M_GEAR,7))
def set_true():
    for k,v in TRUE.items(): setattr(sold,k,v)
def true_ramps(s):
    """material -> true ramp for reference spec s (used to snap derived feature colours)"""
    set_true(); base=sold.UNIF[s['unif']]
    boots=TANB if 0 else (sold.TANB if s['unif'] in ('DESERT TAN','KHAKI') else sold.BLKB if s['unif'] in ('BLACK','NAVY','URBAN GREY','SNOW WHITE') else sold.LEATHER)
    web={'DESERT TAN':sold.WEBT,'KHAKI':sold.WEBT,'BLACK':sold.WEBB,'NAVY':sold.WEBB}.get(s['unif'],sold.WEB)
    if s['gear']=='PLATE': web=sold.R(mix(base,(80,82,62),0.45))
    return {M_SKIN:sold.SKINS[s['skin']-1],M_HAIR:sold.hr(sold.HAIR[s['hair']]),M_UNI:sold.R(base),M_HELM:sold.helm_ramp(s),M_GEAR:web,M_BOOT:boots,M_BERET:sold.R(s.get('beretcol') or (110,30,36))}
TANB=None
FIXED=[]  # global fixed colour list (index 64+)
def fixed_idx(c):
    if c in FIXED: return 64+FIXED.index(c)
    FIXED.append(c); return 64+len(FIXED)-1
def render_idx(s,sc,view,warp,flip=False):
    """returns (W,H,ax,ay, idx[y][x]) ; 0 transparent, mat*8+shade, 64+fixed"""
    set_code(s); ci,(ax,ay),mask=sprite(s,sc,view,warp=warp,flip=flip)
    set_true(); ti,_,_=sprite(s,sc,view,warp=warp,flip=flip)
    tr=true_ramps(s); W,H=ci.size; C=ci.load(); T=ti.load(); M=mask.load(); out=[[0]*W for _ in range(H)]
    for y in range(H):
        for x in range(W):
            if not M[x,y]: continue
            c=C[x,y][:3]
            if c[0]==250 and c[2]==3: out[y][x]=c[1]; continue
            t=T[x,y][:3]; best=None
            for m,rp in tr.items():
                for k,rc in enumerate(rp):
                    d=sum((a-b)**2 for a,b in zip(t,rc))
                    if best is None or d<best[0]: best=(d,m*8+k)
            # snap only colours that came from mixing a material (face shading etc.)
            out[y][x]=best[1] if best[0]<300 and c!=t else fixed_idx(t)
    return W,H,ax,ay,out
# ------------------------------------------------ in-game 4bpp slot mapping
# 1 ink 2/3/4 uniform d/m/l 5/6 camo blotch 7/8 skin shade/light 9 hair 10/11 hat d/l 12 gear 13 boots 14 metal 15 wood
SLOTREF={1:(10,10,8),14:(62,63,68),15:(128,82,44),7:(150,100,70),8:(210,160,116),12:(112,108,72),13:(70,50,34)}
def slot_of(i):
    if i==0: return 0
    if i<64:
        m,k=i>>3,i&7
        if m==M_UNI: return 2 if k<=1 else 3 if k<=3 else 4
        if m==M_SKIN: return 7 if k<=2 else 8
        if m==M_HAIR: return 9
        if m in (M_HELM,M_BERET): return 10 if k<=2 else 11
        if m==M_GEAR: return 12
        if m==M_BOOT: return 13
    c=FIXED[i-64]; lum=sum(c)/3
    if lum<34: return 1
    if c[0]>c[2]+40 and c[0]>c[1]+20: return 15 if lum<170 else 8
    if lum>190: return 8
    return 14
VIEWS='FSB'
def window(view): return (-16 if view!='S' else -11),-29   # x0,y0 rel. to feet anchor
def game_frames(s):
    """15 frames of 32x16 (upper aim/fire per view, lower stand/w1/w2 per view) -> list of 32x16 slot grids"""
    ups=[];lows=[]
    for v in VIEWS:
        W,H,ax,ay,ix=render_idx(s,0.95,v,True)
        ox,oy=window(v); g=[[0]*32 for _ in range(32)]
        for y in range(32):
            for x in range(32):
                X,Y=ax+ox+x,ay+oy+y
                if 0<=X<W and 0<=Y<H: g[y][x]=slot_of(ix[Y][X])
        # ground shadow (dithered ink ellipse under the feet)
        fx=-ox
        for y in range(28,32):
            for x in range(32):
                e=((x-fx)/8.5)**2+((y-29.5)/2.0)**2
                if e<1 and g[y][x]==0 and (x+y)&1: g[y][x]=1
        up=[r[:] for r in g[:16]]
        fire=[[0]*32 for _ in range(16)]
        dx,dy={'F':(0,-1),'S':(-1,0),'B':(0,1)}[v]
        for y in range(16):
            for x in range(32):
                X,Y=x-dx,y-dy
                if 0<=X<32 and 0<=Y<16: fire[y][x]=up[Y][X]
        ups+= [up,fire]
        lo=[r[:] for r in g[16:]]
        def legs(f):
            o=[[0]*32 for _ in range(16)]
            for y in range(16):
                for x in range(32):
                    if y<6: o[y][x]=lo[y][x]; continue
                    X,Y=f(x,y)
                    o[y][x]=lo[Y][X] if 0<=X<32 and 0<=Y<16 else 0
            return o
        if v=='S':
            w1=legs(lambda x,y:(x+1,y) if x<fx else (x-1,y))
            w2=legs(lambda x,y:(x,y+1) if x<fx else (x,y))
        else:
            w1=legs(lambda x,y:(x,y+1) if x<fx else (x,y))
            w2=legs(lambda x,y:(x,y+1) if x>=fx else (x,y))
        lows+=[lo,w1,w2]
    return ups+lows
def tiles4(grid):  # 32x16 -> 8 tiles 1D order (4 wide, 2 tall)
    b=bytearray()
    for ty in range(2):
        for tx in range(4):
            for y in range(8):
                for x in range(0,8,2):
                    a=grid[ty*8+y][tx*8+x]; c=grid[ty*8+y][tx*8+x+1]; b.append(a|(c<<4))
    return b
def rle(b):
    o=bytearray(); i=0
    while i<len(b):
        if b[i]==0:
            n=0
            while i<len(b) and b[i]==0 and n<255: i+=1; n+=1
            o+=bytes([0,n])
        else: o.append(b[i]); i+=1
    return o
HATS=['HELMET','BERET','CAP','BOONIE','NONE']; FACES=['CLEAN','STUBBLE','LONG']; GEARS=['NONE','WEBBING','PLATE','RADIO']; BUILDS=['STANDARD','SLIM']
RIFLES=['M4','GARAND']
# bot looks (geometry); colours come from the faction at runtime
LOOKS=[dict(head='M1',gear='WEBBING',rifle='GARAND'),        # 0 WW2 allied
       dict(head='STAHL',gear='WEBBING',rifle='BOLT'),       # 1 WW2 axis (generic, no insignia)
       dict(head='M1',gear='WEBBING',rifle='M16'),           # 2 cold war west
       dict(head='HELMET',gear='WEBBING',rifle='AK'),        # 3 cold war east
       dict(head='HELMET',gear='PLATE',rifle='M4',nvg=True), # 4 modern west
       dict(head='HELMET',gear='PLATE',rifle='AK'),          # 5 modern east
       dict(head='HELMET',gear='PLATE',rifle='SOF',nvg=True,ears=True), # 6 future / space
       dict(head='BRODIE',gear='WEBBING',rifle='BOLT'),      # 7 WW1
       dict(head='CAP',gear='NONE',rifle='BOLT'),            # 8 1776 / medieval
       dict(head='HELMET',gear='WEBBING',rifle='BOLT')]      # 9 WW2 pacific axis
REF=dict(skin=3,hair='BROWN',unif='OLIVE DRAB',camo='SOLID')
def main():
    sets=[];offs=[]
    blob=bytearray()
    def addset(s):
        offs.append(len(blob)); fr=game_frames(s); raw=b''.join(tiles4(g) for g in fr); assert len(raw)==15*256; blob.extend(rle(raw))
    for h in HATS:
        for f in FACES:
            for g in GEARS:
                for b in BUILDS:
                    for r in RIFLES: addset(spec(head=h,face=f,gear=g,build=b,rifle=r,**REF))
    for L in LOOKS: addset(spec(face='CLEAN',build='STANDARD',**dict(REF,**L)))
    offs.append(len(blob))
    # portraits (72px, FRONT/SIDE/BACK) per geometry combo
    pblob=bytearray(); poffs=[]; PW={'F':48,'S':64,'B':48}; PH=82; pax={'F':24,'S':22,'B':24}
    prev=[]
    for h in HATS:
        for f in FACES:
            for g in GEARS:
                for b in BUILDS:
                    s=spec(head=h,face=f,gear=g,build=b,rifle='M4',**REF)
                    for v in VIEWS:
                        W,H,ax,ay,ix=render_idx(s,1.4,v,False)
                        raw=bytearray()
                        for y in range(PH):
                            for x in range(PW[v]):
                                X,Y=ax-pax[v]+x,ay-PH+2+y
                                raw.append(ix[Y][X] if 0<=X<W and 0<=Y<H else 0)
                        poffs.append(len(pblob)); pblob.extend(rle(raw))
                        if h=='HELMET' and f=='CLEAN' and b=='STANDARD' and g=='WEBBING': prev.append((v,raw))
    poffs.append(len(pblob))
    assert len(FIXED)<=192, len(FIXED)
    # ---- colour tables
    set_true()
    FAC=[(92,104,58,80,92,52),(124,104,66,100,90,60),(132,128,76,90,96,60),(96,102,100,80,86,88),(150,132,80,120,110,70),(80,110,70,70,96,64),
         (104,110,84,84,92,70),(82,98,64,70,80,56),(170,148,104,150,134,96),(190,196,206,170,176,190),(80,64,92,70,56,80),(118,112,78,96,94,66)]
    UN=list(sold.UNIF.keys())  # 8 creator uniforms, then 12 factions
    unis=[sold.UNIF[k] for k in UN]+[f[:3] for f in FAC]; helms=[]; gears=[]; boots=[]; camo=[]
    for i,base in enumerate(unis):
        nm=UN[i] if i<8 else None
        camo.append(sold.camo_ramps(base))
        helms.append(sold.R(mix(base,(70,76,50),0.5)) if i<8 else sold.R(FAC[i-8][3:]))
        web={'DESERT TAN':sold.WEBT,'KHAKI':sold.WEBT,'BLACK':sold.WEBB,'NAVY':sold.WEBB}.get(nm,sold.WEB)
        gears.append((web,sold.R(mix(base,(80,82,62),0.45))))
        boots.append(sold.TANB if nm in ('DESERT TAN','KHAKI') else sold.BLKB if nm in ('BLACK','NAVY','URBAN GREY','SNOW WHITE') else sold.LEATHER)
    c5=lambda c:(c[0]>>3)|((c[1]>>3)<<5)|((c[2]>>3)<<10)
    L=[]; A=L.append
    A('// generated by tools/gen_soldiers.py - do not edit')
    A('#define SD_NSET %d'%(len(offs)-1)); A('#define SD_LOOK0 240')
    def arr(name,typ,vals,per=24):
        A('static const %s %s[%d]={'%(typ,name,len(vals)))
        for i in range(0,len(vals),per): A(','.join(str(v) for v in vals[i:i+per])+',')
        A('};')
    arr('sd_off','u32',offs); arr('sd_rle','u8',list(blob),48)
    arr('sp_off','u32',poffs); arr('sp_rle','u8',list(pblob),48)
    arr('sd_fixed','u16',[c5(c) for c in FIXED]+[0]*(192-len(FIXED)))
    arr('sd_uni','u16',[c5(c) for u in camo for l in u for c in l])        # [20][4][7]
    arr('sd_helm','u16',[c5(c) for h in helms for c in h])                 # [20][7]
    arr('sd_gear','u16',[c5(c) for g in gears for gg in g for c in gg])    # [20][2][7]
    arr('sd_boot','u16',[c5(c) for b in boots for c in b])                 # [20][7]
    arr('sd_skin','u16',[c5(c) for s in sold.SKINS for c in s])            # [6][6]
    arr('sd_hair','u16',[c5(c) for h in sold.HAIR.values() for c in sold.hr(h)]) # [5][6]
    arr('sd_beret','u16',[c5(c) for c in sold.R((110,30,36))])
    # camo layer maps: in-game 32x32 and portrait 64x82, per camo kind 1..4, 2 bits packed as bytes (one per px)
    kinds=['WOODLAND','SPLINTER','DIGITAL','TIGER STRIPE']
    arr('sd_camo_g','u8',[sold.camo_layer(k,x*1.05,y*1.05) for k in kinds for y in range(32) for x in range(32)],64)
    arr('sd_camo_p','u8',[sold.camo_layer(k,x/1.4*1.0,y/1.4) for k in kinds for y in range(82) for x in range(64)],64)
    FONT="\n0 ### #.# #.# #.# ###\n1 .#. ##. .#. .#. ###\n2 ##. ..# .#. #.. ###\n3 ##. ..# .#. ..# ##.\n4 #.# #.# ### ..# ..#\n5 ### #.. ##. ..# ##.\n6 .## #.. ### #.# ###\n7 ### ..# .#. .#. .#.\n8 ### #.# ### #.# ###\n9 ### #.# ### ..# ##.\nA .#. #.# ### #.# #.#\nB ##. #.# ##. #.# ##.\nC .## #.. #.. #.. .##\nD ##. #.# #.# #.# ##.\nE ### #.. ##. #.. ###\nF ### #.. ##. #.. #..\nG .## #.. #.# #.# .##\nH #.# #.# ### #.# #.#\nI ### .#. .#. .#. ###\nJ ..# ..# ..# #.# .#.\nK #.# #.# ##. #.# #.#\nL #.. #.. #.. #.. ###\nM #.# ### ### #.# #.#\nN ##. #.# #.# #.# #.#\nO .#. #.# #.# #.# .#.\nP ##. #.# ##. #.. #..\nQ .#. #.# #.# ##. .##\nR ##. #.# ##. #.# #.#\nS .## #.. .#. ..# ##.\nT ### .#. .#. .#. .#.\nU #.# #.# #.# #.# ###\nV #.# #.# #.# #.# .#.\nW #.# #.# ### ### #.#\nX #.# #.# .#. #.# #.#\nY #.# #.# .#. .#. .#.\nZ ### ..# .#. #.. ###\n_ ... ... ... ... ...\n- ... ... ### ... ...\n. ... ... ... ... .#.\n' ... ... ... ... ...\n"
    fv=[]
    for ln in FONT.strip().split('\n'):
        rows=ln.split()[1:]; v=0
        for r,row in enumerate(rows):
            for c,ch in enumerate(row):
                if ch=='#': v|=1<<(14-r*3-c)
        fv.append(v)
    arr('sd_font','u16',fv)
    open(os.path.join(ROOT,'src','soldierdb.inc'),'w').write('\n'.join(L)+'\n')
    print('sets',len(offs)-1,'game bytes',len(blob),'portrait bytes',len(pblob),'fixed',len(FIXED))
if __name__=='__main__': main()
