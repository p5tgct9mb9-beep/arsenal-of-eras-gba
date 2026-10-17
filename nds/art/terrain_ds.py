#!/usr/bin/env python3
"""terrain_ds.py - DS terrain tiles for the Infantry/Armor arenas (ds-v0.3.0 graphics upgrade).
Each of the 14 arena metatiles gets 4 painted 16x16 variants (rendered in RGB with value noise, soft shading
and 3/4-view highlights), quantised to 15 colours per DS palette bank (banks 1..9).  The game logic still uses
the original metatile ids; only the picture changes.  Output: nitro:/terrain.bin
  header 'TER1', n_eras ; per era: 9 banks x 16 u16 palette, 14 x u8 bank, 224 tiles x 32 bytes (4bpp)
Usage: terrain_ds.py out.bin [preview.png]"""
import sys, struct, numpy as np
from PIL import Image
R = np.random.default_rng
AM = ['G0','G1','G2','PATH','WALL','CRATE','BAGS','TREE','CRATER','BORDER','SPAWN','WATER','BRIDGE','FLOOR']
BANK = {'WATER':1,'BRIDGE':1,'G0':2,'G1':2,'G2':2,'SPAWN':2,'PATH':3,'CRATER':3,'WALL':4,'CRATE':5,'BAGS':6,'TREE':7,'BORDER':8,'FLOOR':9}
# era looks: ground, ground2 (variation), blade/dark, dirt, dirt dark, stone, wood, foliage, water, accent
ERAS = [
 dict(g=(70,110,48),g2=(92,128,58),gd=(40,72,32),d=(132,100,62),dd=(96,70,44),st=(150,90,70),wood=(150,108,62),fo=(52,104,44),wa=(40,96,150),ac=(230,210,90),fl=(120,112,100),bag=(176,156,108)),
 dict(g=(222,228,238),g2=(200,212,228),gd=(160,176,200),d=(140,130,124),dd=(100,94,92),st=(128,132,140),wood=(120,96,70),fo=(40,82,66),wa=(60,110,160),ac=(255,255,255),fl=(150,150,158),bag=(190,190,176)),
 dict(g=(206,170,112),g2=(190,150,96),gd=(150,118,72),d=(170,128,82),dd=(126,92,58),st=(196,170,130),wood=(140,104,64),fo=(110,130,64),wa=(48,120,160),ac=(240,220,170),fl=(176,160,130),bag=(196,176,128)),
 dict(g=(58,64,96),g2=(70,78,112),gd=(40,44,72),d=(84,88,120),dd=(50,52,80),st=(100,104,150),wood=(90,90,130),fo=(40,140,140),wa=(40,140,190),ac=(90,240,230),fl=(72,76,110),bag=(110,110,140)),
 dict(g=(128,128,136),g2=(112,112,122),gd=(84,84,96),d=(150,146,140),dd=(96,94,96),st=(150,150,170),wood=(120,120,136),fo=(90,100,110),wa=(70,90,140),ac=(255,180,80),fl=(110,114,130),bag=(150,150,150)),
]
def noise(rng, s, oct=(4,8,16), amp=(0.5,0.3,0.2)):
    out = np.zeros((s,s))
    for o,a in zip(oct,amp):
        g = rng.random((o+1,o+1)); g[o,:] = g[0,:]; g[:,o] = g[:,0]  # tileable
        ys = np.linspace(0,o,s,endpoint=False); i = ys.astype(int); f = ys-i; f = f*f*(3-2*f)
        a0 = g[i][:,i]*(1-f)[None,:]+g[i][:,i+1]*f[None,:]; a1 = g[i+1][:,i]*(1-f)[None,:]+g[i+1][:,i+1]*f[None,:]
        out += a*(a0*(1-f)[:,None]+a1*f[:,None])
    return out
def mix(a,b,t): t=np.clip(t,0,1)[...,None]; return np.array(a)*(1-t)+np.array(b)*t
def shade(c,k): return tuple(min(255,max(0,int(v*k))) for v in c)
def ground(E, rng, kind, era=0):
    n = noise(rng,16,(2,4,8),(0.5,0.3,0.2)); im = mix(E['g'],E['g2'],n*1.6-0.3)
    m = noise(rng,16,(8,16),(0.5,0.5))
    im[m<0.28] = mix(im[m<0.28], shade(E['gd'],1.0), np.full((m<0.28).sum(),0.55))
    im[m>0.76] = mix(im[m>0.76], shade(E['g2'],1.15), np.full((m>0.76).sum(),0.6))
    def put(x,y,c):
        if 0<=x<16 and 0<=y<16: im[y,x] = c
    nd = {'G0':2,'G1':3,'G2':4,'SPAWN':1}[kind]
    for i in range(nd):
        x,y = (int(v) for v in rng.integers(2,13,2))
        if era==0:   # grass tuft: dark blades with a lit tip
            for dx,dy in ((0,0),(1,-1),(2,0),(-1,0),(1,0)): put(x+dx,y+dy,shade(E['gd'],0.8))
            put(x+1,y-2,shade(E['g2'],1.3)); put(x-1,y-1,shade(E['g2'],1.2))
            if kind=='G1' and i==0: put(x+4,y+2,E['ac']); put(x+5,y+2,(240,240,230)); put(x+4,y+3,shade(E['gd'],0.7))
        elif era==1: # snow: rock with snow cap / ice glint
            if i%2==0: put(x,y,shade(E['st'],0.8)); put(x+1,y,shade(E['st'],0.6)); put(x,y-1,(250,252,255)); put(x+1,y-1,(240,244,250)); put(x,y+1,shade(E['gd'],0.85))
            else: put(x,y,(255,255,255)); put(x+1,y+1,shade(E['gd'],0.9))
        elif era==2: # desert scrub bush and pebbles
            if i%2==0 or kind=='G2':
                for dx,dy in ((0,0),(1,0),(-1,0),(0,-1),(1,-1),(2,0)): put(x+dx,y+dy,shade(E['fo'],0.75))
                put(x,y-1,shade(E['fo'],1.15)); put(x-1,y,shade(E['fo'],1.0)); put(x+1,y+1,shade(E['gd'],0.7)); put(x+2,y+1,shade(E['gd'],0.75))
            else: put(x,y,shade(E['st'],1.1)); put(x+1,y,shade(E['st'],0.8)); put(x,y+1,shade(E['gd'],0.7))
        elif era==3: # tech panel seam with a lit node
            for dx in range(-2,3): put(x+dx,y,shade(E['gd'],0.8))
            put(x,y-1,E['ac'])
        else:        # regolith micro-crater
            put(x,y,shade(E['gd'],0.8)); put(x+1,y,shade(E['gd'],0.9)); put(x,y-1,shade(E['g2'],1.25)); put(x+1,y+1,shade(E['g2'],1.15))
    return im
def dirt(E, rng):
    n = noise(rng,16,(4,8,16)); im = mix(E['d'],E['dd'],n*1.2-0.15)
    for y in (4,11):  # wheel ruts
        im[y,:] = mix(im[y,:], shade(E['dd'],0.85), np.full(16,0.6)); im[y+1,:] = mix(im[y+1,:],shade(E['d'],1.15),np.full(16,0.5))
    for _ in range(4):
        x,y = rng.integers(0,15,2); im[y,x] = shade(E['st'],1.2); im[y+1,x] = shade(E['dd'],0.7)
    return im
def paint(E, t, v, era):
    rng = R(1000*era+37*AM.index(t)+v)
    gr = ground(E, R(era*7+v), 'G0', era)
    if t in ('G0','G1','G2'): return ground(E, rng, t, era)
    if t=='SPAWN':
        im = ground(E, rng, 'SPAWN', era)
        for i in range(16):
            if i%4<2: im[0,i]=im[15,i]=im[i,0]=im[i,15]=E['ac']
        return im
    if t=='PATH': return dirt(E, rng)
    if t=='CRATER':
        im = dirt(E, rng); yy,xx = np.mgrid[0:16,0:16]; r = np.hypot(xx-7.5,yy-7.5)
        im[r<6.5] = mix(shade(E['dd'],0.55), shade(E['dd'],0.9), (yy[r<6.5]-2)/12)
        rim = (r>=5.5)&(r<7.5); im[rim&(yy<8)] = shade(E['d'],1.25); im[rim&(yy>=8)] = shade(E['dd'],0.8)
        im[(r<2.5)] = shade(E['dd'],0.4); return im
    if t=='WALL':  # 3/4 brick wall: top cap + brick face
        im = np.zeros((16,16,3)); im[:] = shade(E['st'],0.8)
        im[0:5] = mix(shade(E['st'],1.25), shade(E['st'],1.05), np.tile(np.linspace(0,1,5)[:,None],(1,16)))
        im[0,:] = shade(E['st'],1.45)
        for row,y in enumerate(range(5,15,3)):
            im[y,:] = shade(E['st'],0.5); off = 0 if row%2==0 else 4
            for x in range(off,16,8): im[y:y+3,x] = shade(E['st'],0.5)
            n = rng.random(16); im[y+1,:] = mix(im[y+1,:], shade(E['st'],1.0), n*0.6)
        im[15,:] = shade(E['st'],0.35); return im
    if t=='CRATE':
        im = gr.copy(); im[13:16,2:16] = mix(im[13:16,2:16], (0,0,0), np.full((3,14),0.45))  # shadow
        w = E['wood']; im[1:14,1:14] = shade(w,0.95)
        for y in range(1,14,4): im[y,1:14] = shade(w,0.6)
        im[1:14,1] = shade(w,1.25); im[1,1:14] = shade(w,1.3); im[13,1:14] = shade(w,0.5); im[1:14,13] = shade(w,0.6)
        for i in range(2,13): im[i,i] = shade(w,0.65); im[i,i+1 if i<12 else i] = shade(w,1.15)
        return im
    if t=='BAGS':
        im = gr.copy(); b = E['bag']
        for row,(y0,offs) in enumerate(((7,0),(2,3))):
            for x0 in range(-6+offs,16,6):
                for y in range(5):
                    for x in range(6):
                        X=x0+x; Y=y0+y
                        if 0<=X<16 and 0<=Y<16 and ((x-2.5)/3)**2+((y-2)/2.6)**2<1:
                            im[Y,X] = shade(b, 1.2-0.12*y-0.03*x)
                im[min(15,y0+4),max(0,x0+1):max(0,x0+5)] = shade(b,0.55)
        im[13:16,:] = mix(im[13:16,:], (0,0,0), np.full((3,16),0.3)); return im
    if t=='TREE':
        im = gr.copy(); yy,xx = np.mgrid[0:16,0:16]
        sh = np.hypot(xx-9,(yy-10)*1.4)<6.5; im[sh] = mix(im[sh],(0,0,0),np.full(sh.sum(),0.45))
        n = noise(rng,16,(8,16),(0.6,0.4)); r = np.hypot(xx-7.5,yy-7)
        c = r<6.8+n*1.2
        lit = np.clip(((7.5-xx)+(7-yy))/12+0.5+n*0.4-0.2,0,1)
        im[c] = mix(shade(E['fo'],0.55), shade(E['fo'],1.35), lit[c])
        im[(r<6.8+n*1.2)&(r>5.6+n*1.2)&(xx+yy>15)] = shade(E['fo'],0.45)
        if era==1: im[c&(lit>0.9)&(n>0.5)] = (235,240,248)
        return im
    if t=='BORDER':
        im = np.zeros((16,16,3)); n = noise(rng,16,(4,8),(0.6,0.4))
        im[:] = mix(shade(E['st'],0.45), shade(E['st'],0.85), n)
        for _ in range(4):
            x,y = rng.integers(1,13,2); im[y:y+3,x:x+3] = shade(E['st'],1.05); im[y,x:x+3] = shade(E['st'],1.3); im[y+3 if y<13 else y,x:x+3] = shade(E['st'],0.35)
        return im
    if t=='FLOOR':
        im = np.zeros((16,16,3)); n = noise(rng,16,(8,16),(0.5,0.5)); im[:] = mix(shade(E['fl'],0.92), shade(E['fl'],1.08), n)
        im[7,:] = im[:,7] = shade(E['fl'],0.7); im[8,:] = im[:,8] = shade(E['fl'],1.15)
        im[0,:] = im[:,0] = shade(E['fl'],0.78); return im
    if t=='WATER':  # indices: deep, shimmer A/B/C (rotated by the game), light
        im = np.zeros((16,16,3)); n = noise(rng,16,(4,8),(0.6,0.4)); im[:] = mix(shade(E['wa'],0.8), shade(E['wa'],1.0), n)
        return im
    if t=='BRIDGE':
        im = np.zeros((16,16,3)); w = E['wood']
        for y in range(16): im[y,:] = shade(w, 1.05 if y%4 else 0.6)
        im[:,0:2] = shade(w,0.5); im[:,14:16] = shade(w,0.5); im[:,2] = shade(w,1.3)
        n = rng.random((16,16)); im[n<0.06] = shade(w,0.75); return im
    raise ValueError(t)
def quant(imgs, k, fixed=()):
    px = np.concatenate([i.reshape(-1,3) for i in imgs]).astype(float)
    c = list(fixed); rng = R(5)
    free = k-len(c); cent = px[rng.choice(len(px), free, replace=len(px)<free)]
    for _ in range(18):
        allc = np.array(c+list(cent)) if c else cent
        d = ((px[:,None,:]-allc[None])**2).sum(-1); a = d.argmin(1)
        for j in range(free):
            m = a==len(c)+j
            if m.any(): cent[j] = px[m].mean(0)
    allc = np.array(c+list(cent))
    return [((i.reshape(-1,3)[:,None]-allc[None])**2).sum(-1).argmin(1).reshape(16,16)+1 for i in imgs], allc
def rgb15(c): r,g,b = (int(round(v))>>3 for v in c); return r|(g<<5)|(b<<10)
def era_set(era):
    E = ERAS[era]; pics = {(t,v): paint(E,t,v,era) for t in AM for v in range(4)}
    pal = np.zeros((10,16,3)); idx = {}
    # water: 1 deep, 2..4 shimmer (rotated in game), 5 foam ; plus bridge colours 6..15
    wa = E['wa']; wc = [shade(wa,0.7), shade(wa,0.95), shade(wa,1.12), shade(wa,1.3), (200,225,240)]
    for v in range(4):
        rng = R(900+era*4+v); im = pics[('WATER',v)]; q = np.ones((16,16),int)
        n = noise(rng,16,(4,8),(0.6,0.4)); q[n>0.45] = 1
        for y in range(16):
            for x in range(16):
                ph = (x + 2*np.sin((y+v*3)*0.8)) % 6
                q[y,x] = 1 if n[y,x]<0.35 else (2+int(ph)//2 if ph<6 else 1)
        for _ in range(3):
            x,y = rng.integers(1,13,2); q[y,x:x+3] = 5
        idx[('WATER',v)] = q
    bq, bc = quant([pics[('BRIDGE',v)] for v in range(4)], 10)
    for v in range(4): idx[('BRIDGE',v)] = bq[v]+5
    pal[1,1:6] = wc; pal[1,6:16] = bc
    grass_imgs = [pics[(t,v)] for t in ('G0','G1','G2','SPAWN') for v in range(4)]
    gq, gc = quant(grass_imgs, 15); pal[2,1:16] = gc
    for i,(t,v) in enumerate((t,v) for t in ('G0','G1','G2','SPAWN') for v in range(4)): idx[(t,v)] = gq[i]
    keepg = [tuple(x) for x in gc[np.argsort([-(gq[0]==j+1).sum()-(gq[1]==j+1).sum() for j in range(15)])[:5]]]
    for t,b in (('PATH',3),('WALL',4),('CRATE',5),('BAGS',6),('TREE',7),('BORDER',8),('FLOOR',9)):
        ts = [t] + (['CRATER'] if t=='PATH' else [])
        ims = [pics[(u,v)] for u in ts for v in range(4)]
        q, c = quant(ims, 15, keepg if t in ('CRATE','BAGS','TREE') else ())
        pal[b,1:16] = c
        for i,(u,v) in enumerate((u,v) for u in ts for v in range(4)): idx[(u,v)] = q[i]
    return pal, idx
def tiles4(q):
    out = b''
    for ty in (0,8):
        for tx in (0,8):
            for y in range(8):
                w = 0
                for x in range(8): w |= int(q[ty+y,tx+x]) << (4*x)
                out += struct.pack('<I', w)
    return out
SKY = ['SEA','WAVES','SHORE','LAND','LAND2','FOREST','TOWN','BOAT']
SKYBANK = {'SEA':0,'WAVES':0,'BOAT':0,'SHORE':2,'LAND':2,'LAND2':3,'FOREST':4,'TOWN':5}
SKYE = [dict(sea=(36,84,140),sand=(206,186,132),land=(84,128,56),land2=(120,140,64),fo=(40,84,40),roof=(170,80,60),wall=(200,190,170)),
        dict(sea=(40,76,120),sand=(200,214,228),land=(226,232,240),land2=(196,206,220),fo=(36,70,60),roof=(110,110,120),wall=(170,170,176))]
def sky_set(era):
    E = SKYE[era]; pal = np.zeros((10,16,3)); idx = {}
    sea = E['sea']; pal[0,1:7] = [shade(sea,1.08), shade(sea,1.24), shade(sea,0.82), shade(sea,0.94), shade(sea,1.5), (220,232,240)]
    for v in range(4):
        rng = R(300+era*10+v); n = noise(rng,16,(2,4,8),(0.5,0.3,0.2))
        for t in ('SEA','WAVES','BOAT'):
            q = np.where(n<0.5,3,4)
            for k in range(4 if t=='SEA' else 7):  # short wave dashes; colours 1/2 swap in game = moving glints
                x,y = (int(c) for c in rng.integers(0,16,2)); L = int(rng.integers(2,5))
                for i in range(L): q[y,(x+i)%16] = 1 if (k+i)%2 else 2
                if t=='WAVES' and k%3==0: q[y,x%16] = 5; q[(y+1)%16,(x+1)%16] = 6
            if t=='BOAT':
                for i in range(3,13): q[9,i] = 6 if i<6 else q[9,i]
                q[7:9,6:12] = 7; q[7,6:12] = 8; q[6:8,9:11] = 9; q[9,6:12] = 6
            idx[(t,v)] = q
    pal[0,7:10] = [(120,90,60),(170,140,100),(220,220,210)]
    pics = {}
    for v in range(4):
        rng = R(500+era*10+v); n = noise(rng,16,(2,4,8))
        sh = mix(shade(E['sand'],0.92), shade(E['sand'],1.06), n)
        for _ in range(5): x,y = (int(c) for c in rng.integers(0,16,2)); sh[y,x] = shade(E['sand'],0.8)
        pics[('SHORE',v)] = sh
        ld = mix(shade(E['land'],0.9), shade(E['land'],1.08), n)
        for _ in range(3): x,y = (int(c) for c in rng.integers(0,15,2)); ld[y,x] = shade(E['fo'],1.0); ld[y,x+1] = shade(E['fo'],0.8)
        pics[('LAND',v)] = ld
        l2 = mix(shade(E['land2'],0.9), shade(E['land2'],1.06), n)
        for y in range(0,16,3): l2[y,:] = shade(E['land2'],0.78)   # field furrows
        if v%2: l2 = l2.transpose(1,0,2).copy()
        l2[:, (v*5)%16] = shade(E['land'],0.7)                       # hedgerow
        pics[('LAND2',v)] = l2
        fo = mix(shade(E['land'],0.7), shade(E['land'],0.85), n)
        for _ in range(9):
            x,y = (int(c) for c in rng.integers(0,14,2))
            fo[y:y+3,x:x+3] = shade(E['fo'],0.95); fo[y,x:x+2] = shade(E['fo'],1.3 if era==0 else 1.6); fo[min(15,y+3),x+1:x+3] = shade(E['fo'],0.5)
        pics[('FOREST',v)] = fo
        tw = ld.copy()   # farmland feature: one or two small farmhouses with shadow, a lane
        tw[:, 3+v*3] = mix(tw[:, 3+v*3], shade(E['sand'],0.8), np.full(16,0.7))
        for k in range(1 + v%2):
            bx,by = (int(c) for c in rng.integers(1,10,2)); rc = shade(E['roof'], 0.85+0.3*rng.random())
            tw[by+1:by+5,bx+1:bx+6] = (30,36,28)                 # shadow
            tw[by:by+4,bx:bx+5] = rc; tw[by:by+2,bx:bx+5] = shade(rc,1.25); tw[by+3,bx:bx+5] = E['wall']
        pics[('TOWN',v)] = tw
    for t,b in (('SHORE',2),('LAND2',3),('FOREST',4),('TOWN',5)):
        ts = [t] + (['LAND'] if t=='SHORE' else [])
        q, c = quant([pics[(u,v)] for u in ts for v in range(4)], 15); pal[b,1:16] = c
        for i,(u,v) in enumerate((u,v) for u in ts for v in range(4)): idx[(u,v)] = q[i]
    return pal, idx
def pack_set(types, bank, pal, idx, base):
    used = sorted(set(bank[t] for t in types)); mask = sum(1<<b for b in used)
    out = struct.pack('<BBHH', len(types), 0, base, mask) + b'\0\0'
    out += b''.join(struct.pack('<H', 0 if j==0 else rgb15(pal[b,j])) for b in range(10) for j in range(16))
    out += bytes(bank[t] for t in types) + bytes((-len(types))%4)
    for t in types:
        for v in range(4): out += tiles4(idx[(t,v)])
    return out
def build(out, preview=None):
    sets = []; sheets = []
    for era in range(len(ERAS)):
        pal, idx = era_set(era); sets.append(pack_set(AM, BANK, pal, idx, 0)); sheets.append((AM, BANK, pal, idx))
    for era in range(len(SKYE)):
        pal, idx = sky_set(era); sets.append(pack_set(SKY, SKYBANK, pal, idx, 128)); sheets.append((SKY, SKYBANK, pal, idx))
    hdr = b'TER2' + struct.pack('<I', len(sets)); off = 8 + 4*len(sets); tab = b''
    for st in sets: tab += struct.pack('<I', off); off += len(st)
    open(out,'wb').write(hdr + tab + b''.join(sets))
    if preview:
        sh = np.zeros((len(sheets)*72,14*18,3),np.uint8)
        for k,(types,bank,pal,idx) in enumerate(sheets):
            for i,t in enumerate(types):
                for v in range(4): sh[k*72+v*18:k*72+v*18+16,i*18:i*18+16] = pal[bank[t]][idx[(t,v)]].astype(np.uint8)
        Image.fromarray(sh).resize((14*18*3,len(sheets)*72*3),Image.NEAREST).save(preview)
if __name__ == '__main__': build(sys.argv[1], sys.argv[2] if len(sys.argv)>2 else None)
