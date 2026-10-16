# test helper: insert the first-boot KC tutorial skip (START) after the title-screen press
import sys,re
t=open(sys.argv[1]).read()
if '#kc' not in t and 'KCSKIP' not in t:
    m=re.search(r'^tap (START|A)\n',t,re.M)
    if m:t=t[:m.end()]+"# KCSKIP\nwait 10\ntap START\n"+("" if '#guide' in t else "poke32 0x0203FFE8 4447534E\n")+t[m.end():]
elif '#guide' not in t:
    m=re.search(r'^tap (START|A)\n',t,re.M)
    if m:t=t[:m.end()]+"poke32 0x0203FFE8 4447534E\n"+t[m.end():]
# v1.3.3: scripts written for the v1.3.2 13-row main menu run with the legacy-menu test flag unless tagged #newmenu
if '#newmenu' not in t:
    parts=re.split(r'(?m)^(?=corereset\s*$)',t);o=[]
    for p in parts:
        m=re.search(r'^tap (START|A)\n',p,re.M)
        if m:p=p[:m.end()]+"poke32 0x0203FFC8 5943474C\n"+p[m.end():]
        o.append(p)
    t="".join(o)
sys.stdout.write(t)
