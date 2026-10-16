#!/usr/bin/env python3
"""Decode src/soldierdb.inc the same way the ROM does and draw a preview sheet (for review screenshots)."""
import re,sys,os
from PIL import Image
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
T=open(os.path.join(ROOT,'src/soldierdb.inc')).read()
A={m.group(1):[int(v) for v in m.group(2).replace('\n','').split(',') if v.strip()] for m in re.finditer(r'static const \w+ (\w+)\[\d+\]=\{(.*?)\};',T,re.S)}
def unrle(b,o,n):
    out=[];i=o
    while len(out)<n:
        if b[i]==0: out+= [0]*b[i+1]; i+=2
        else: out.append(b[i]); i+=1
    return out
def c8(v): return ((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3)
def pal16(uni=0,skin=2,hair=0,beret=False,plate=False):
    U=A['sd_uni']; u=lambda l,k:c8(U[uni*28+l*7+k]); hl=A['sd_beret'] if beret else A['sd_helm'][uni*7:uni*7+7]
    g=A['sd_gear'][(uni*2+(1 if plate else 0))*7:]; S=A['sd_skin'][skin*6:]; H=A['sd_hair'][hair*6:]
    return [(0,0,0),(10,10,8),u(0,1),u(0,3),u(0,5),u(1,3),u(2,3),c8(S[1]),c8(S[4]),c8(H[3]),c8(hl[2]),c8(hl[5]),c8(g[3]),c8(A['sd_boot'][uni*7+2]),(62,63,68),(128,82,44)]
def game_set(si,pal,camo=0):
    raw=unrle(A['sd_rle'],A['sd_off'][si],15*256); frames=[]
    for f in range(15):
        im=Image.new('RGBA',(32,16),(0,0,0,0)); P=im.load()
        for t in range(8):
            tx,ty=t%4,t//4
            for y in range(8):
                for x in range(8):
                    b=raw[f*256+t*32+y*4+x//2]; p=(b>>(4*(x&1)))&15
                    X,Y=tx*8+x,ty*8+y
                    if camo and p in (3,4):
                        l=A['sd_camo_g'][(camo-1)*1024+((Y+(16 if f>=6 else 0))&31)*32+X]
                        if l==1: p=5
                        elif l>=2: p=6
                    if p: P[X,Y]=pal[p]+(255,)
        frames.append(im)
    return frames
def portrait(ci,v,lut):
    W={0:48,1:64,2:48}[v]; raw=unrle(A['sp_rle'],A['sp_off'][ci*3+v],W*82)
    im=Image.new('RGBA',(W,82),(0,0,0,0)); P=im.load()
    for i,p in enumerate(raw):
        if p: P[i%W,i//W]=lut(p,i%W,i//W)+(255,)
    return im
def lut_for(uni=0,skin=2,hair=0,camo=0,plate=False):
    def f(p,x,y):
        if p>=64: return c8(A['sd_fixed'][p-64])
        m,k=p>>3,p&7
        if m==1: return c8(A['sd_skin'][skin*6+min(k,5)])
        if m==2: return c8(A['sd_hair'][hair*6+min(k,5)])
        if m==3:
            l=A['sd_camo_p'][(camo-1)*64*82+y*64+x] if camo else 0
            return c8(A['sd_uni'][uni*28+l*7+k])
        if m==4: return c8(A['sd_helm'][uni*7+k])
        if m==5: return c8(A['sd_gear'][(uni*2+(1 if plate else 0))*7+k])
        if m==6: return c8(A['sd_boot'][uni*7+k])
        if m==7: return c8(A['sd_beret'][k])
        return (255,0,255)
    return f
if __name__=='__main__':
    out=Image.new('RGB',(400,260),(76,92,52))
    for row,(si,uni,camo,sk) in enumerate([(240+0,8+0,0,2),(240+1,8+3,0,1),(240+4,8+7,0,4),(240+5,8+6,0,0),(2*2*4*3*0+ (1*2+0)*2,0,1,5)]):
        fr=game_set(si,pal16(uni,sk,0,plate=si in(244,245)),camo)
        for v in range(3):
            for j,lf in enumerate((0,1,2)):
                x=4+(v*3+j)*36; y=4+row*36
                out.paste(fr[6+v*3+lf],(x,y+16),fr[6+v*3+lf]); out.paste(fr[v*2],(x,y),fr[v*2])
    for v in range(3):
        p=portrait(1*2+0,v,lut_for(0,2,0,1)); out.paste(p,(4+v*66,178),p)
    out.resize((out.width*3,out.height*3),Image.NEAREST).save(sys.argv[1] if len(sys.argv)>1 else '/tmp/sd_prev.png')
