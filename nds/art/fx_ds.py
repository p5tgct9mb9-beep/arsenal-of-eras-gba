#!/usr/bin/env python3
"""fx_ds.py - DS effects upgrade (ds-v0.3.0): repaints the shared FX sprite sheet (fx_tiles) with shaded
tracers, star muzzle flashes, volumetric smoke puffs and 4-frame fireballs (flash -> fireball -> breakup -> smoke),
keeping the GBA tile layout and the 16-colour FX palette so every mode picks them up unchanged.
Used by tools/port.py: fx_ds.patch(words) -> words.  Run directly to write a preview PNG."""
import math, sys
import numpy as np
# FX palette: 0 clear,1 black,2 dark,3 grey,4 light,5 white,6 yellow,7 orange,8 red,9 dark red,...
FXP=[(0,0,0),(16,16,18),(60,60,64),(110,110,116),(170,170,176),(250,250,250),(255,236,90),(255,150,40),(230,60,30),(130,30,20),(80,240,255),(60,120,255),(255,80,220),(90,230,110),(80,160,255),(255,70,60)]
FIRE=[9,8,7,6,5]; SMOKE=[1,2,3,4]
def vnoise(n, seed, f=4):
    r=np.random.default_rng(seed); g=r.random((f+2,f+2)); ys=np.linspace(0,f,n,endpoint=False); i=ys.astype(int); t=ys-i; t=t*t*(3-2*t)
    a=g[i][:,i]*(1-t)+g[i][:,i+1]*t; b=g[i+1][:,i]*(1-t)+g[i+1][:,i+1]*t; return a*(1-t)[:,None]+b*t[:,None]
def fireball(n, k, seed):
    yy,xx=np.mgrid[0:n,0:n]; c=(n-1)/2; r=np.hypot(xx-c,yy-c)/(n/2); a=np.arctan2(yy-c,xx-c)
    nz=vnoise(n,seed,4)*0.5+vnoise(n,seed+1,8)*0.5; g=np.zeros((n,n),int)
    if k==0:  # flash: white core, yellow body, orange tongues + radial spikes
        R=0.55+0.25*nz; v=1-r/R
        g[v>0]=7; g[v>0.25]=6; g[v>0.55]=5
        for i in range(8):
            ang=i*math.pi/4; L=0.98 if i%2==0 else 0.75
            for t in np.linspace(0.3,L,int(n*0.6)):
                x=int(round(c+math.cos(ang)*t*n/2)); y=int(round(c+math.sin(ang)*t*n/2))
                if 0<=x<n and 0<=y<n and g[y,x]==0: g[y,x]=6 if t<0.7 else 7
    elif k==1:  # fireball: lumpy, hot core, red rim
        R=0.80+0.22*(nz-0.5)*2; v=1-r/R
        g[v>0]=8; g[v>0.15]=7; g[v>0.42]=6; g[v>0.72]=5
        g[(v>0)&(v<0.12)&(nz<0.45)]=9
    elif k==2:  # breakup: dark smoke shell with glowing pockets
        R=0.92+0.2*(nz-0.5)*2; v=1-r/R; lit=((c-xx)+(c-yy))/n
        g[v>0]=2; g[(v>0)&(lit+nz*0.5>0.35)]=3; g[(v>0.25)&(nz>0.55)]=8; g[(v>0.35)&(nz>0.65)]=7; g[(v>0.5)&(nz>0.72)]=6
        g[(v>0)&(v<0.1)&(nz<0.5)]=2
    else:      # smoke: billowing grey puffs, embers
        g[:]=0; rng=np.random.default_rng(seed)
        for i in range(6):
            ang=i*1.05+0.3; d=0.38*n/2; px=c+math.cos(ang)*d; py=c+math.sin(ang)*d; pr=n*0.24
            rr=np.hypot(xx-px,yy-py)/pr; m=rr<1+0.25*(nz-0.5)
            lit=((px-xx)+(py-yy))/pr
            g[m]=np.where(lit[m]>0.5,4,np.where(lit[m]>-0.4,3,2))
        g[(g>0)&(nz>0.78)]=7
    return g
def puff(n, seed, small=False):
    yy,xx=np.mgrid[0:n,0:n]; c=(n-1)/2-(0.5 if not small else 0); nz=vnoise(n,seed,3)
    rad=(n*0.42 if not small else n*0.28); r=np.hypot(xx-c,yy-c-(0 if small else 0.5))/rad; g=np.zeros((n,n),int)
    m=r<1+0.3*(nz-0.5); lit=((c-xx)+(c-yy))/rad
    g[m]=np.where(lit[m]>0.6,5 if small else 4,np.where(lit[m]>-0.3,4 if small else 3,3 if small else 2)); g[m&(r>0.85)&(lit<0)]=2
    return g
def star(n, cols, arms=4, long=1.0):
    g=np.zeros((n,n),int); c=(n-1)/2
    for i in range(arms*2):
        ang=i*math.pi/arms; L=(n/2)*(long if i%2==0 else 0.55)
        for t in np.linspace(0,L,n*2):
            x=int(round(c+math.cos(ang)*t)); y=int(round(c+math.sin(ang)*t))
            if 0<=x<n and 0<=y<n: g[y,x]=max(g[y,x],0) or cols[min(len(cols)-1,int(t/L*len(cols)))]
    yy,xx=np.mgrid[0:n,0:n]; g[np.hypot(xx-c,yy-c)<n*0.2]=cols[0]; return g
def tracer(n):
    g=np.zeros((n,n),int); yy,xx=np.mgrid[0:n,0:n]; r=np.hypot(xx-3.5,yy-3.5)
    g[r<2.6]=7; g[r<1.9]=6; g[r<1.0]=5; return g
def shell(n):
    g=np.zeros((n,n),int); yy,xx=np.mgrid[0:n,0:n]; r=np.hypot(xx-3.5,yy-3.5)
    g[r<3.0]=8; g[r<2.4]=7; g[r<1.6]=6; g[r<0.8]=5; return g
def grid_words(g):
    n=g.shape[0]; out=[]
    for ty in range(n//8):
        for tx in range(n//8):
            for y in range(8):
                v=0
                for x in range(8): v|=int(g[ty*8+y,tx*8+x])<<(4*x)
                out.append(v)
    return out
def tiles():
    t={}
    t[0]=tracer(8); t[3]=shell(8); t[5]=star(8,[5,6,7],4,1.0); t[6]=puff(8,11); t[7]=puff(8,12,True); t[8]=star(8,[5,6,7],4,0.9)
    big={16+4*k: fireball(16,k,40+k) for k in range(4)}
    return t,big
def patch(words):
    w=list(words); t,big=tiles()
    for i,g in t.items(): w[i*8:i*8+8]=grid_words(g)
    for i,g in big.items(): w[i*8:i*8+32]=grid_words(g)
    return w
if __name__=='__main__':
    from PIL import Image
    t,big=tiles(); im=np.zeros((20,20*6+18*4,3),np.uint8); x=0
    for i in (0,3,5,6,7,8): g=t[i]; im[6:14,x+6:x+14]=np.array(FXP)[g]; x+=20
    for i in sorted(big): g=big[i]; im[2:18,x+1:x+17]=np.array(FXP)[g]; x+=18
    bg=np.array([60,90,50],np.uint8); m=(im.sum(-1)==0); im[m]=bg
    Image.fromarray(im).resize((im.shape[1]*6,im.shape[0]*6),Image.NEAREST).save(sys.argv[1] if len(sys.argv)>1 else '/tmp/fx_prev.png')
