import sys
from PIL import Image
src,prefix,n=sys.argv[1],sys.argv[2],int(sys.argv[3])
im=Image.open(src); cards=[]
cols=int(sys.argv[4]) if len(sys.argv)>4 else 5
for k in range(n):
    r,i=divmod(k,cols); x=5+i*161; y=28+r*128
    cards.append(im.crop((x*3,y*3,(x+156)*3,(y+112)*3)))
for j in range(0,n,3):
    g=cards[j:j+3]; out=Image.new('RGB',(468*len(g)+8*(len(g)-1),336),(0,0,0))
    for q,c in enumerate(g): out.paste(c,(q*476,0))
    out.save(f'{prefix}{j//3}.png')
