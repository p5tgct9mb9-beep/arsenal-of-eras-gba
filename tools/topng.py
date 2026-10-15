import sys,glob,os
from PIL import Image
d=sys.argv[1];out=sys.argv[2] if len(sys.argv)>2 else d
for p in glob.glob(os.path.join(d,'*.ppm')):
    im=Image.open(p);im=im.resize((im.width*3,im.height*3),Image.NEAREST)
    im.save(os.path.join(out,os.path.basename(p)[:-4]+'.png'));os.remove(p)
