# exec'd from gen_race.py: car sprites (OBJ 4bpp 32x16: rear, 3/4, side), menu side art 64x24, FX tiles
# palette slots: 1 outline 2 body 3 shade 4 highlight 5 glass 6 glass hi 7 tyre 8 rim 9 tail red 10 tail hot 11 headlamp 12 under 13 trim 14 accent 15 white
CARS=[ # name, body, shade, hi, glass, side profile, window, wheels x, rear(bw,roofy,cabw,bodytop,light), accent
 ("ROADSTER",(220,30,40),(150,16,24),(255,120,110),(40,50,80),
  [(.02,.62),(.05,.46),(.36,.40),(.50,.22),(.70,.22),(.82,.42),(.98,.52),(.99,.72),(.02,.76)],[(.40,.40),(.52,.27),(.68,.27),(.76,.41)],(.20,.80),(.94,.42,.50,.56,'bar'),(30,30,34)),
 ("MODEL S PLAID",(176,180,190),(110,114,126),(236,240,248),(30,40,64),
  [(.02,.56),(.04,.42),(.22,.35),(.40,.15),(.66,.15),(.82,.35),(.98,.45),(.99,.72),(.02,.76)],[(.26,.35),(.42,.19),(.64,.19),(.78,.35)],(.20,.80),(.88,.24,.58,.50,'pair'),(30,30,34)),
 ("MODEL 3",(236,238,244),(170,174,186),(255,255,255),(30,40,64),
  [(.04,.56),(.06,.42),(.24,.36),(.40,.16),(.64,.16),(.80,.36),(.96,.46),(.97,.72),(.04,.76)],[(.28,.36),(.43,.20),(.62,.20),(.76,.36)],(.22,.78),(.84,.26,.56,.50,'pair'),(30,30,34)),
 ("MODEL X",(44,46,56),(24,24,30),(120,126,146),(70,90,130),
  [(.02,.46),(.04,.30),(.24,.09),(.70,.07),(.84,.30),(.98,.40),(.99,.72),(.02,.76)],[(.27,.30),(.40,.12),(.68,.11),(.80,.30)],(.20,.80),(.90,.10,.70,.46,'pair'),(200,200,210)),
 ("MODEL Y",(40,90,200),(22,50,130),(120,170,250),(30,40,64),
  [(.03,.50),(.05,.36),(.24,.14),(.66,.12),(.82,.33),(.97,.42),(.98,.72),(.03,.76)],[(.28,.34),(.41,.17),(.64,.16),(.77,.34)],(.21,.79),(.88,.18,.62,.48,'pair'),(30,30,34)),
 ("CYBERTRUCK",(196,200,206),(140,144,152),(240,242,246),(50,56,70),
  [(.02,.42),(.02,.32),(.50,.05),(.98,.36),(.99,.70),(.02,.72)],[(.30,.27),(.50,.12),(.70,.24),(.62,.31)],(.18,.82),(.96,.16,.46,.50,'bar'),(40,40,44)),
 ("SEMI",(240,240,244),(170,172,180),(255,255,255),(40,50,80),
  [(.02,.52),(.02,.44),(.54,.44),(.60,.04),(.88,.04),(.99,.30),(.99,.72),(.02,.74)],[(.63,.10),(.84,.10),(.94,.30),(.62,.30)],(.14,.86),(.80,.02,.78,.40,'pair'),(30,30,34)),
 ("CYBERCAB",(214,180,96),(160,126,56),(250,226,150),(40,46,60),
  [(.10,.56),(.14,.37),(.36,.18),(.64,.18),(.86,.37),(.92,.56),(.92,.72),(.10,.76)],[(.28,.36),(.40,.22),(.60,.22),(.72,.36)],(.26,.74),(.76,.24,.60,.50,'bar'),(30,30,34)),
]
def car_pal(c):
    name,body,shade,hi,glass=c[:5];acc=c[9]
    P=[(0,0,0),(16,16,22),body,shade,hi,glass,(150,180,230),(26,26,30),(120,124,134),(200,20,30),(255,110,90),(255,250,220),(40,40,46),(170,174,184),acc,(255,255,255)]
    return [rgb15(x) for x in P]
def outline(a):
    o=a.copy();H,W=a.shape
    for y in range(H):
        for x in range(W):
            if a[y,x]==0:
                for dy,dx in ((1,0),(-1,0),(0,1),(0,-1)):
                    yy,xx=y+dy,x+dx
                    if 0<=yy<H and 0<=xx<W and a[yy,xx] not in (0,1):o[y,x]=1;break
    return o
def side(c,W,H,flip=False):
    im=Image.new("P",(W,H),0);d=ImageDraw.Draw(im)
    P=lambda pts:[(1+x*(W-3),1+y*(H-3)) for x,y in pts]
    d.polygon(P(c[5]),fill=2)
    body=P(c[5]);ys=[p[1] for p in body]
    # shade the lower third
    a=np.array(im);lo=int(1+0.62*(H-3));a[lo:,:][a[lo:,:]==2]=3;im=Image.fromarray(a);d=ImageDraw.Draw(im)
    d.line([(2,int(1+0.48*(H-3))),(W-3,int(1+0.48*(H-3)))],fill=4)
    d.polygon(P(c[6]),fill=5);w=P(c[6]);d.line([w[0],w[1]],fill=6)
    for wx in c[7]:
        cx=1+wx*(W-3);cy=1+0.80*(H-3);r=max(2,0.17*(H-3)) if W<40 else 0.18*(H-3)
        d.ellipse([cx-r,cy-r,cx+r,cy+r],fill=7);d.ellipse([cx-r/2,cy-r/2,cx+r/2,cy+r/2],fill=8)
    d.point((W-3,int(1+0.50*(H-3))),fill=11);d.point((2,int(1+0.52*(H-3))),fill=9)
    a=outline(np.array(im))
    return a[:,::-1] if flip else a
def rear(c,W,H):
    bw,roofy,cabw,btop,light=c[8];a=np.zeros((H,W),np.int32)
    x0=int(round((W-bw*W)/2));x1=W-1-x0;yt=int(btop*H);yr=int(roofy*H)+1
    for y in range(yt,H-2):
        for x in range(x0,x1+1):a[y,x]=2 if y<H-5 else 3
    a[yt,x0]=0;a[yt,x1]=0
    for y in range(yr,yt):
        f=(y-yr)/max(1,yt-yr);hw=(cabw*W/2)*(1-f)+(bw*W/2-2)*f;xa=int(round(W/2-hw));xb=int(round(W/2+hw))-1
        for x in range(xa,xb+1):a[y,x]=5 if (y>yr and x>xa and x<xb) else 2
        if y==yr+1 and xb-xa>4:a[y,xa+2]=6
    for x in range(x0+1,x1):a[yt+1,x]=4 if light!='bar' else 9
    if light=='bar':
        for x in range(x0+2,x1-1):a[yt+2,x]=10 if (x+yt)%3 else 9
    else:
        for x in list(range(x0+1,x0+5))+list(range(x1-4,x1)):a[yt+2,x]=9;
        a[yt+2,x0+2]=10;a[yt+2,x1-2]=10
    for x in range(x0+2,x1-1):a[H-5,x]=12
    for y in range(H-5,H):
        for x in list(range(x0,x0+4))+list(range(x1-3,x1+1)):a[y,x]=7
    a[H-1,:]=np.where(a[H-1,:]==7,7,a[H-1,:])
    return outline(a)
def three_q(c,W,H):
    r=rear(c,W,H);s=side(c,W,H,flip=True)  # side with front to the left (car turning left = we see its right side)
    a=np.zeros((H,W),np.int32)
    rr=np.array(Image.fromarray(r.astype(np.uint8)).resize((W*5//12,H),Image.NEAREST)).astype(np.int32)
    ss=np.array(Image.fromarray(s.astype(np.uint8)).resize((W*8//12,H),Image.NEAREST)).astype(np.int32)
    a[:,:ss.shape[1]]=ss
    m=rr>0;sub=a[:,W-rr.shape[1]:];sub[m]=rr[m];a[:,W-rr.shape[1]:]=sub
    return outline(np.where(a==1,0,a))
def tiles4(a):  # HxW indexed -> 4bpp 1D OBJ tile words
    H,W=a.shape;words=[]
    for ty in range(H//8):
        for tx in range(W//8):
            for r in range(8):
                v=0
                for k in range(8):v|=int(a[ty*8+r,tx*8+k])<<(4*k)
                words.append(v)
    return words
words=[];pals=[];arts=[]
sheet=Image.new("RGB",(32*3*4,16*4*8))
for ci,c in enumerate(CARS):
    views=[rear(c,32,16),three_q(c,32,16),side(c,32,16,flip=True)]
    for v in views:words+=tiles4(v)
    pals+=car_pal(c)
    art=side(c,64,24);arts+=[int(x) for x in art.reshape(-1)]
    pr=[(0,0,0)]+[((p&31)<<3,((p>>5)&31)<<3,((p>>10)&31)<<3) for p in car_pal(c)[1:]]
    for vi,v in enumerate(views):
        im=Image.fromarray(np.array([[pr[x] for x in row] for row in v],np.uint8)).resize((128,64),Image.NEAREST);sheet.paste(im,(vi*128,ci*64))
sheet.save("/tmp/race_cars.png")
emit("rc_cart",words,"unsigned int");emit("rc_carp",pals,"unsigned short");emit("rc_cara",arts)
OUT.append("#define RC_NCAR %d"%len(CARS))
# FX: 0 shadow 32x8 (4 tiles), then 8x8: 4 smoke, 5 gas puff, 6 spark, 7 speed line, 8 bolt
fx=[]
sh=np.zeros((8,32),np.int32)
for y in range(8):
    for x in range(32):
        if ((x-15.5)/15.5)**2+((y-3.5)/3.6)**2<=1:sh[y,x]=1
fx+=tiles4(sh)
def t8(rows):return tiles4(np.array([[int(ch,16) if ch!='.' else 0 for ch in r] for r in rows]))
fx+=t8(["..7777..",".777777.","77777777","77788777","77788777","77777777",".777777.","..7777.."])
fx+=t8(["...22...","..2332..",".233332.","23333332","23333332",".233332.","..2332..","...22..."])
fx+=t8(["...5....","...5....",".5.5.5..","..555...","5555555.","..555...",".5.5.5..","...5...."])
fx+=t8(["........","........","........","22222222","........","........","........","........"])
fx+=t8(["....55..","...55...","..55....",".555555.","....55..","...55...","..55....",".5......"])
emit("rc_fxt",fx,"unsigned int")
emit("rc_fxp",[rgb15(x) for x in [(0,0,0),(10,10,14),(255,255,255),(170,220,255),(60,120,255),(255,220,60),(255,140,40),(150,150,156),(90,90,96)]+[(0,0,0)]*7],"unsigned short")
