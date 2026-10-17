import sys
from PIL import Image, ImageDraw
files=sys.argv[2:]; out=sys.argv[1]
cols=3; w,h=360,240
rows=(len(files)+cols-1)//cols
S=Image.new('RGB',(cols*w,rows*(h+12)),'white'); d=ImageDraw.Draw(S)
for i,f in enumerate(files):
    im=Image.open(f).convert('RGB').resize((w,h),Image.NEAREST)
    x,y=(i%cols)*w,(i//cols)*(h+12)
    S.paste(im,(x,y+12)); d.text((x+2,y),f.split('/')[-1],fill='black')
S.save(out)
