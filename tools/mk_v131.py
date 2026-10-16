#!/usr/bin/env python3
# v1.3.1 test scripts: menu path into a category list item, then poke AUTO + the mission-jump mailbox, shots every N frames
import sys,os
D=os.path.join(os.path.dirname(os.path.abspath(__file__)))
def script(name,cat,item,scr,mis,shots=10,gap=600,extra=""):
    L=["wait 100","tap START","wait 40"]
    for _ in range(cat):L+=["tap DOWN","wait 4"]
    L+=["tap A","wait 30"]
    for _ in range(item):L+=["tap DOWN","wait 4"]
    L+=["tap A","wait 6","poke32 0x0203FFD0 4F545541","poke32 0x0203FFD8 %08X"%(0x46520000+(scr<<8)+mis),"tap A","wait 30"]
    if extra:L+=extra.strip().split("\n")
    for k in range(shots):L+=["wait %d"%gap,"shot %s_%02d"%(name,k)]
    L+=["end"]
    open(os.path.join(D,name+".txt"),"w").write("\n".join(L)+"\n")
EXP=4;SPC=3
for m in range(4):script("v131_odef_%d"%m,EXP,6,73,m,shots=10,gap=500)
for t in range(6):script("v131_rb_%d"%t,EXP,7,74,t,shots=14,gap=600,extra="wait 20\nshot v131_rb_%d_design\ntap A\nwait 300\nhold SELECT 20"%t)
for m in range(5):script("v131_moon_%d"%m,EXP,4,71,m,shots=12,gap=500)
for m in range(5):script("v131_mars_%d"%m,EXP,5,72,m,shots=16 if m==4 else 12,gap=500)
for m in range(5):script("v131_ast_%d"%m,EXP,8,75,m,shots=14,gap=600)
