import sys,glob,os
from PIL import Image
fs=sorted(glob.glob(sys.argv[1]));out=sys.argv[2];cols=int(sys.argv[3]) if len(sys.argv)>3 else 3
ims=[Image.open(f).resize((240,160)) for f in fs]
rows=(len(ims)+cols-1)//cols
s=Image.new('RGB',(cols*242,rows*172),(255,0,255))
from PIL import ImageDraw
d=ImageDraw.Draw(s)
for i,(f,im) in enumerate(zip(fs,ims)):
    x=(i%cols)*242;y=(i//cols)*172;s.paste(im,(x,y+10));d.text((x+2,y),os.path.basename(f)[:40],fill=(255,255,255))
s.save(out)
