# test helper: insert the first-boot KC tutorial skip (START) after the title-screen press
import sys,re
t=open(sys.argv[1]).read()
if '#kc' not in t and 'KCSKIP' not in t:
    m=re.search(r'^tap (START|A)\n',t,re.M)
    if m:t=t[:m.end()]+"# KCSKIP\nwait 10\ntap START\n"+("" if '#guide' in t else "poke32 0x0203FFE8 4447534E\n")+t[m.end():]
elif '#guide' not in t:
    m=re.search(r'^tap (START|A)\n',t,re.M)
    if m:t=t[:m.end()]+"poke32 0x0203FFE8 4447534E\n"+t[m.end():]
sys.stdout.write(t)
