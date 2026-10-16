#!/bin/bash
# v1.3 regression: every tools/*.txt script -> build/R/<name>/ (+ log); prints end beat per script
cd /workspace/aoe-gba; mkdir -p build/R
run(){ n=$(basename $1 .txt); sav=""; [ "$n" = v11_migrate ] && sav=tools/v1_save.sav; timeout 600 ./tools/run1.sh $1 build/R/$n $sav > build/R/$n.log 2>&1; echo "$n: $(grep -E '^\[end\]' build/R/$n.log | tail -1 | cut -c1-90)"; }
export -f run
ls tools/*.txt | grep -v "_s.txt" | xargs -P 8 -I{} bash -c 'run {}' | sort > build/R/summary.txt
echo DONE >> build/R/summary.txt
