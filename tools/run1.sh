#!/bin/bash
# run1.sh script.txt outdir [sav]  -- kcskip + harness + png
cd /workspace/aoe-gba; s=$1; o=$2; rm -rf $o; mkdir -p $o; python3 tools/kcskip.py $s > $o/_s.txt
if [ -n "$3" ]; then cp $3 $o/_.sav; timeout 300 ./tools/harness ${ROM:-ArsenalOfEras.gba} $o/_s.txt $o $o/_.sav > $o/log 2>&1; else timeout 300 ./tools/harness ${ROM:-ArsenalOfEras.gba} $o/_s.txt $o > $o/log 2>&1; fi
python3 tools/topng.py $o >/dev/null; grep -E '^\[' $o/log | cut -c1-150
