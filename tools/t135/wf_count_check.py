#!/usr/bin/env python3
# Verifies the World Flight roster counts from a wf_count.txt run log: python3 wf_count_check.py <log>
import re,sys
L=open(sys.argv[1]).read().splitlines()
def beat(tag):
    for l in L:
        if l.startswith('['+tag+']'):return {k:int(v) for k,v in re.findall(r'(\w+)=(-?\d+)',l)}
    raise SystemExit('missing '+tag)
exp={'ww2':(33,[19,2,2,3,7]),'cw':(20,[10,1,4,5,0]),'mod':(60,[30,10,12,8,0]),'all':(113,[59,13,18,16,7])}
cats=['planes','drones','helos','exper','wunder'];ok=True
for e,(tot,cs) in exp.items():
    b=beat('era_'+e);n=(b['extra']>>8)&255;print(f'{e:4} total {n:3} (want {tot})');ok&=n==tot
    for c,w in zip(cats,cs):
        n=(beat(f'{e}_{c}')['extra']>>8)&255;print(f'      {c:7} {n:3} (want {w})');ok&=n==w
seen=set();
for l in L[-113:]:
    m=re.search(r'scr=112 .*?\bA=113\b',l)
    mm=re.search(r'\] t=\d+ frame=\d+ scr=(\d+)',l)
menus=[]
for l in L:
    if l.startswith('[] ') and 'scr=112' in l:pass
# browse beats: the last 113 hangar beats carry menu index via B= (cat|era<<4) and x/y unused -> use extra pos field
pos=[(int(re.search(r'extra=(\d+)',l).group(1))>>16) for l in L if l.startswith('[]') and 'scr=112' in l][-120:]
print('browse: visited',len(set(pos)),'distinct of 113 in',len(pos),'taps');ok&=set(pos)==set(range(113))
print('PASS' if ok else 'FAIL');sys.exit(0 if ok else 1)
