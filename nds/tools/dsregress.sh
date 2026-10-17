#!/bin/bash
# dsregress.sh OUTDIR [SCRIPTDIR=tools] [GBA_BASELINE=build/R135]
# Runs every GBA regression script on the DS build (8 parallel) and compares the end state
# (scr + mode of the last [end] beat) with the GBA v1.3.5 baseline summary.
cd /workspace/aoe-gba; O=$1; D=${2:-tools}; BASE=${3:-build/R135}; mkdir -p $O; export O; export NOPNG=${NOPNG:-1}; export DSH_CTL=${DSH_CTL:-classic} # GBA scripts use the GBA layout
run(){ n=$(basename $1 .txt); sav=""; [ "$n" = v11_migrate ] && sav=tools/v1_save.sav; [ "$n" = v13_migrate ] && sav=tools/v12p.sav; [ "$n" = v13_migrate_camp ] && sav=tools/v12c.sav
 nds/tools/dsrun1.sh $1 $O/$n $sav > $O/$n.log 2>&1; echo "$n: $(grep -E '^\[end\]' $O/$n.log | tail -1 | cut -c1-90)"; }
export -f run
ls $D/*.txt | grep -v "_s.txt" | xargs -P ${JOBS:-8} -I{} bash -c 'run {}' | sort > $O/summary.txt
python3 - "$O/summary.txt" "$BASE/summary.txt" > $O/compare.txt <<'PY'
import sys,re
def P(f):
    d={}
    for l in open(f):
        m=re.match(r'(\S+): (.*)',l.strip())
        if not m: continue
        b=re.search(r'scr=(\d+) mode=(\d+)',m.group(2)); d[m.group(1)]=(b.group(1),b.group(2)) if b else None
    return d
ds,gb=P(sys.argv[1]),P(sys.argv[2])
same=[k for k in gb if k in ds and ds[k]==gb[k]]; diff=[k for k in gb if k in ds and ds[k]!=gb[k]]; miss=[k for k in gb if k not in ds]
print('scripts %d  same end scr/mode %d  different %d  missing %d'%(len(gb),len(same),len(diff),len(miss)))
for k in diff: print('DIFF %-28s gba=%s ds=%s'%(k,gb[k],ds[k]))
for k in miss: print('MISS',k)
PY
cat $O/compare.txt | head -60
python3 nds/tools/dsbeatcmp.py $O build/R135 | tee $O/beatcmp.txt | head -1
