#!/usr/bin/env python3
# v1.3: rewrite old flat-menu navigation (after title START) to the category menu.
import sys,re
def T(k,n=1):return ["tap "+k]*n
NAV={0:T("A")+["wait 25"]+T("A")+["wait 6"]+T("A"),
 5:T("DOWN",3)+["wait 4"]+T("A")+["wait 25"]+T("DOWN",4)+["wait 4"]+T("A")+["wait 6"]+T("A"),
 6:T("RIGHT")+["wait 4"]+T("A")+["wait 25"]+T("DOWN")+["wait 4"]+T("A")+["wait 6"]+T("A"),
 7:T("DOWN",2)+["wait 4"]+T("A")+["wait 25"]+T("A")+["wait 6"]+T("A"),
 8:T("RIGHT")+T("DOWN")+["wait 4"]+T("A"),
 9:T("RIGHT")+T("DOWN",2)+["wait 4"]+T("A"),
 10:T("RIGHT")+T("DOWN",3)+["wait 4"]+T("A"),
 11:T("RIGHT")+["wait 4"]+T("A")+["wait 25"]+T("A")+["wait 6"]+T("A"),
 13:T("DOWN",3)+["wait 4"]+T("A")+["wait 25"]+T("DOWN",3)+["wait 4"]+T("A")+["wait 6"]+T("A")}
for i,b in ((1,0),(2,1),(3,2),(4,3),(12,4)):NAV[i]=T("DOWN")+["wait 4"]+T("A")+["wait 25"]+T("DOWN",b)+["wait 4"]+T("A")+["wait 6"]+T("A")
def fix(path,nmm=15,drone=False):
    L=open(path).read().split("\n");out=[];i=0;fresh=True;n=0
    while i<len(L):
        l=L[i];out.append(l)
        if l.strip()=="corereset":fresh=True
        if l.strip()=="tap START" and fresh:
            fresh=False;j=i+1;idx=0;block=[]
            while j<len(L):
                s=L[j].strip()
                m=re.match(r"tap (UP|DOWN|LEFT|RIGHT)$",s)
                if m:idx+={"DOWN":1,"UP":-1,"RIGHT":5,"LEFT":-5}[m.group(1)];j+=1;continue
                if s.split(" ")[0] in ("wait","","#","shot","peek","audio") or s.startswith("#"):block.append(L[j]);j+=1;continue
                break
            if j<len(L) and L[j].strip()=="tap A":
                idx%=nmm
                if drone and idx==4:idx=5
                if path.endswith("shots.txt") and idx==14:idx=8
                if nmm==13 and idx==12:idx=12
                out+= [b for b in block if not b.strip().startswith("shot")]+NAV[idx];n+=1;i=j+1;continue
        i+=1
    open(path,"w").write("\n".join(out));return n
for p in sys.argv[1:]:
    print(p,fix(p,13 if p.endswith("v13_duel.txt") else 15,"drone" in p))
