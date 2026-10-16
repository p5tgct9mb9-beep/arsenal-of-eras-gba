#!/bin/bash
# v1.3.3 regression: regress133.sh <rom> <outdir> [scriptdir=tools]  -> <outdir>/summary.txt (end beat per script)
cd /workspace/aoe-gba; export ROM=$1; O=$2; D=${3:-tools}; mkdir -p $O; export O
run(){ n=$(basename $1 .txt); sav=""; [ "$n" = v11_migrate ] && sav=tools/v1_save.sav; [ "$n" = v13_migrate ] && sav=tools/v12p.sav; [ "$n" = v13_migrate_camp ] && sav=tools/v12c.sav; timeout 600 ./tools/run1.sh $1 $O/$n $sav > $O/$n.log 2>&1; echo "$n: $(grep -E '^\[end\]' $O/$n.log | tail -1 | cut -c1-90)"; }
export -f run
ls $D/*.txt | grep -v "_s.txt" | xargs -P 8 -I{} bash -c 'run {}' | sort > $O/summary.txt
echo DONE >> $O/summary.txt
