# v1.2: KC, the tutorial guide (the user's dog): black shepherd mix, huge upright ears, grey/white senior muzzle,
# brown eyes, small white chest patch. Olive collar with a gold dog tag. Original procedural pixel art.
from PIL import Image
KCP=[(0,0,0),(12,10,16),(34,32,42),(58,54,68),(92,88,104),(146,144,152),(206,204,212),(244,244,248),(96,58,34),(8,8,10),(120,78,92),(84,112,60),(250,206,70),(232,112,132),(255,255,255),(150,120,40)]
def KG(w,h):return [[0]*w for _ in range(h)]
def KP(g,x,y,c):
    if 0<=y<len(g) and 0<=x<len(g[0]):g[y][x]=c
def KR(g,x0,y0,x1,y1,c):
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):KP(g,x,y,c)
def KE(g,x0,y0,x1,y1,c):
    cx=(x0+x1)/2;cy=(y0+y1)/2;rx=(x1-x0)/2+.5;ry=(y1-y0)/2+.5
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if ((x-cx)/rx)**2+((y-cy)/ry)**2<=1.0:KP(g,x,y,c)
def KL(g,x0,y0,x1,y1,c):
    n=max(abs(x1-x0),abs(y1-y0),1)
    for i in range(n+1):KP(g,round(x0+(x1-x0)*i/n),round(y0+(y1-y0)*i/n),c)
def Kout(g):
    h=len(g);w=len(g[0]);o=[r[:] for r in g]
    for y in range(h):
        for x in range(w):
            if not g[y][x] and any(0<=x+a<w and 0<=y+b<h and g[y+b][x+a] not in (0,1) for a,b in ((1,0),(-1,0),(0,1),(0,-1))):o[y][x]=1
    return o
globals().update(KG=KG,KP=KP,KR=KR,KE=KE,KL=KL,Kout=Kout,KCP=KCP)
def side(f):
    g=KG(16,16)
    sit=f>=2
    if not sit:
        KE(g,2,7,11,11,2);
        a=1 if f==0 else -1
        for lx,ph in ((3,a),(5,-a),(9,a),(11,-a)):KL(g,lx,11,lx+ph,15,2);KP(g,lx+ph,15,1)
        KL(g,2,8,0,5+(f&1),2);KP(g,0,4+(f&1),3)   # tail
        KE(g,9,3,14,8,2);KR(g,13,6,15,8,5);KP(g,15,6,9);KP(g,14,8,6);KP(g,12,5,8)
        KR(g,11,0,12,0,2);KR(g,10,1,13,3,2);KP(g,11,1,10);KP(g,12,1,10);KP(g,11,2,10);KP(g,13,0,3)
        KR(g,10,8,12,8,11);KP(g,11,9,12);KP(g,12,10,7)
        KL(g,3,7,9,7,3);KL(g,10,4,12,4,3)
    else:
        KE(g,3,9,9,15,2);KR(g,4,13,9,15,2);KR(g,8,6,11,14,2);KR(g,10,12,13,15,2);KP(g,13,15,1)   # haunch, chest, front paws
        KL(g,4,10,8,9,3)
        wag=f==4
        KL(g,3,13,0,11-(2 if wag else 0),2);KP(g,0,10-(2 if wag else 0),3)
        KE(g,8,3,13,8,2);KR(g,12,6,15,8,5);KP(g,15,6,9);KP(g,13,8,6);KP(g,11,5,8);KL(g,9,4,11,4,3)
        KR(g,10,0,11,0,2);KR(g,9,1,12,2,2);KP(g,10,1,10);KP(g,11,1,10);KP(g,10,2,10);KP(g,12,0,3)
        KR(g,9,9,12,9,11);KP(g,11,10,12);KP(g,11,11,7);KP(g,10,12,7)
        if f==3:KR(g,13,8,15,8,1);KP(g,14,9,13);KP(g,15,9,13)
    return g
spr=[];sheet=Image.new('RGB',(5*20,20),(110,170,90))
for f in range(5):
    g=Kout(side(f));spr+=tiles4(g,16,16)
    for y in range(16):
        for x in range(16):
            if g[y][x]:sheet.putpixel((f*20+2+x,2+y),KCP[g[y][x]])
emit("kc_spr",spr,"unsigned int");emit("kc_pal",pal16(KCP))
globals().update(side=side)
def portrait(open_):
    g=KG(48,48)
    def tri(p,c):
        (x0,y0),(x1,y1),(x2,y2)=p
        for y in range(48):
            for x in range(48):
                d=(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0)
                a=((x1-x)*(y2-y)-(x2-x)*(y1-y))/d;b=((x2-x)*(y0-y)-(x0-x)*(y2-y))/d;cc=1-a-b
                if a>=0 and b>=0 and cc>=0:g[y][x]=c
    tri(((4,26),(8,0),(22,14)),2);tri(((26,14),(40,0),(44,26)),2)
    tri(((8,20),(9,5),(18,15)),10);tri(((30,15),(39,5),(40,20)),10)
    KL(g,8,1,6,14,3);KL(g,40,1,42,14,3)
    KE(g,7,11,41,41,2)
    for y in range(12,22):
        for x in range(14,35):
            if g[y][x]==2 and H(x//2,y//2,77)<.35:g[y][x]=3
    KE(g,17,12,31,17,3);KL(g,20,12,28,12,4)
    KE(g,15,26,33,43,5);KE(g,18,30,30,43,6);KE(g,21,36,27,43,7)
    for i in range(10):KP(g,16+int(H(i,3,9)*16),28+int(H(i,4,9)*12),6)
    KE(g,12,19,19,26,8);KE(g,29,19,36,26,8);KE(g,14,20,18,25,9);KE(g,31,20,35,25,9);KR(g,14,20,15,21,14);KR(g,31,20,32,21,14);KP(g,17,24,4);KP(g,34,24,4)
    KL(g,13,19,18,18,5);KL(g,30,18,35,19,5)
    KE(g,20,26,28,32,9);KP(g,22,27,4);KP(g,23,27,14)
    KL(g,24,32,24,34,1)
    if open_:KE(g,19,35,29,41,1);KE(g,21,38,27,42,13)
    else:KL(g,20,35,24,36,1);KL(g,24,36,28,35,1)
    KR(g,10,42,38,47,2);KR(g,21,44,27,47,7)
    KR(g,11,41,37,42,11);KE(g,22,43,26,47,12);KP(g,23,44,14)
    return Kout(g)
pt=[];big=Image.new('RGB',(2*52,52),(30,46,96))
for fr in (0,1):
    g=portrait(fr)
    for y in range(48):
        for x in range(48):
            c=g[y][x];pt.append(rgb15(KCP[c]) if c else 0x8000)
            if c:big.putpixel((fr*52+2+x,2+y),KCP[c])
emit("kc_portrait",pt)
big.resize((big.width*4,big.height*4),Image.NEAREST).save(os.path.join(ROOT,'shots','v12_kc_portrait.png'))
sheet.resize((sheet.width*6,sheet.height*6),Image.NEAREST).save(os.path.join(ROOT,'shots','v12_kc_sprites.png'))
print("kc: 5 sprite frames, 2 portrait frames")
