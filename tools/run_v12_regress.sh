#!/bin/bash
cd /workspace/aoe-gba
run(){ n=$1; mkdir -p build/T/scr; python3 tools/kcskip.py $2 > build/T/scr/$n.txt; f=build/T/scr/$n.txt; sav=$3; rm -rf build/T/$n; mkdir -p build/T/$n; if [ -n "$sav" ]; then cp $sav build/T/$n.sav; timeout 300 ./tools/harness ArsenalOfEras.gba $f build/T/$n build/T/$n.sav > build/T/$n.log 2>&1; else timeout 300 ./tools/harness ArsenalOfEras.gba $f build/T/$n > build/T/$n.log 2>&1; fi; python3 tools/topng.py build/T/$n >/dev/null; echo "$n: $(grep -E '^\[(end|back)\]' build/T/$n.log | tail -1 | cut -c1-60)"; }
mkdir -p build/T
for f in tools/v11_m_*.txt; do n=$(basename $f .txt); [ "$n" = v11_m_natural ] && continue; run $n $f & done; wait
run v11_camp tools/v11_camp.txt & run v11_loadout tools/v11_loadout.txt & run v11_migrate tools/v11_migrate.txt tools/v1_save.sav & run v11_settings tools/v12_settings.txt & run v11_char tools/v11_char.txt & run v11_6v6 tools/v11_6v6_inf.txt & wait
for f in tools/v11_dogfight_*.txt build/v11_inf_*.txt build/v11_drone_*.txt tools/v11_maps.txt tools/v11_m_natural.txt; do run $(basename $f .txt) $f & done; wait
