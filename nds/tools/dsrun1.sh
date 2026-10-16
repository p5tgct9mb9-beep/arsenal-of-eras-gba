#!/bin/bash
# dsrun1.sh script.txt outdir [gba.sav]  -- DS twin of tools/run1.sh: kcskip + dsharness (melonDS) + png
cd /workspace/aoe-gba; s=$1; o=$2; rm -rf $o; mkdir -p $o; python3 tools/kcskip.py $s > $o/_s.txt
export DSH_SYS=$(mktemp -d /tmp/dsh_XXXXXX)
CORE=${DSCORE:-/workspace/emu/melonds_libretro.so}; NDS=${NDS:-nds/ArsenalOfEras.nds}
if [ -n "$3" ]; then python3 nds/tools/gbasav2ds.py $3 $o/_.ds.sav >/dev/null; timeout 600 nds/tools/dsharness $CORE $NDS $o/_s.txt $o $o/_.ds.sav > $o/rawlog 2>&1
else timeout 600 nds/tools/dsharness $CORE $NDS $o/_s.txt $o > $o/rawlog 2>&1; fi
grep -a -o '^\[[a-z_A-Z0-9-]*\] .*\|frames=.*' $o/rawlog > $o/log; rm -f $o/rawlog; rm -rf $DSH_SYS
[ -z "$NOPNG" ] && python3 nds/tools/dspng.py $o >/dev/null 2>&1; cat $o/log | cut -c1-150
