#!/bin/bash
# run all 28 Bradley autopilot playtests (8 parallel) -> build/BT/NN, summary build/BT/summary.txt
cd /workspace/aoe-gba; python3 tools/mk_brt.py; mkdir -p build/BT
ls tools/brt/brt_*.txt | xargs -P 8 -I{} bash -c 'n=$(basename {} .txt|cut -c5-); tools/run1.sh {} build/BT/$n >/dev/null 2>&1'
: > build/BT/summary.txt
for i in $(seq -w 0 27); do l=$(grep -E '^\[' build/BT/$i/log | grep 'scr=58\|phase=4' | grep -m1 'phase=4\|phase=5'); [ -z "$l" ] && l="NOT DONE: $(grep -E '^\[' build/BT/$i/log | tail -1)"; echo "$i $l" | cut -c1-170 >> build/BT/summary.txt; done
cat build/BT/summary.txt
