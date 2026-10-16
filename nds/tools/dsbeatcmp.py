#!/usr/bin/env python3
"""dsbeatcmp.py DSDIR GBADIR - compare every beat checkpoint (tag -> scr/mode) of each DS regression log
with the GBA v1.3.5 baseline log of the same script. Reports per-script checkpoint agreement."""
import sys,re,os,glob
R=re.compile(r'^\[([^\]\s]+)\] t=\d+ frame=\d+ scr=(\d+) mode=(\d+)')
def beats(f):
    d=[]
    for l in open(f,errors='replace'):
        m=R.match(l)
        if m: d.append((m.group(1),m.group(2),m.group(3)))
    return d
ds_dir,gb_dir=sys.argv[1],sys.argv[2]
tot=agree=0;bad=[];crash=[]
for f in sorted(glob.glob(ds_dir+'/*.log')):
    n=os.path.basename(f)[:-4];g=os.path.join(gb_dir,n+'.log')
    if not os.path.exists(g): continue
    a,b=beats(f),beats(g)
    txt=open(f,errors='replace').read()
    if 'frames=' not in txt or 'FATAL' in txt: crash.append(n)
    gm={};[gm.setdefault(t,(s,m)) for t,s,m in b]
    k=s=0;first=None
    seen=set()
    for t,sc,md in a:
        if t in seen or t not in gm: continue
        seen.add(t);k+=1
        if gm[t]==(sc,md): s+=1
        elif first is None: first=(t,gm[t],(sc,md))
    tot+=k;agree+=s
    if k and s<k: bad.append((n,s,k,first))
print('checkpoints %d, matching GBA %d (%.1f%%); scripts with any mismatch %d; did not finish %d'%(tot,agree,100*agree/max(tot,1),len(bad),len(crash)))
for n,s,k,fi in bad: print('  %-28s %3d/%-3d first diff at [%s] gba=%s ds=%s'%(n,s,k,fi[0],fi[1],fi[2]))
for n in crash: print('  NOT FINISHED',n)
