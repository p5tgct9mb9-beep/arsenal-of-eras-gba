import sys; from dslib import *
import importlib
def make(name,mod,label):
    m=importlib.import_module(mod)
    t,b=m.top(),m.bottom()
    t.save(f'n_{name}_top.png'); b.save(f'n_{name}_bot.png')
    im=pair(t,b,label=label); im.save(f'n_{name}_pair.png')
    up(im,3).save(f'/workspace/ds_mockup_{name}.png'); return im
if __name__=='__main__':
    for a in sys.argv[1:]:
        n,m,l=a.split(':'); make(n,m,l)
