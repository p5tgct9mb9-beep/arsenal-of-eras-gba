#!/usr/bin/env python3
# v1.3.1: generate per-course Bradley autopilot playtest scripts tools/brt/brt_XX.txt
import sys,os
TIER=os.environ.get("TIER")
for i in range(28):
    s="wait 100\ntap START\nwait 15\ntap DOWN\ntap DOWN\nwait 4\ntap A\nwait 25\ntap DOWN\nwait 4\ntap A\nwait 6\nhold A+R 4\nwait 30\n"
    if TIER is not None:s+="poke32 0x0203FFDC %08X\n"%(0x54495200|int(TIER))
    s+="poke32 0x0203FFD0 4F545541\npoke32 0x0203FFD8 %08X\n"%(0x42524300|i)
    for k in range(14):
        s+="wait 700\nshot brt%02d_%02d\n"%(i,k)
    s+="beat end\n"
    open(("tools/brt/brt_%02d.txt"%i) if TIER is None else ("/tmp/brtT/brt_%02d.txt"%i),"w").write(s)
