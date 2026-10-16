#!/bin/bash
# mig132.sh OUT : make a realistic v1.3.2 save. Each v1.3.2 mode script runs on a fresh v1.3.2 boot (the scripts expect the
# first-boot flow); their SRAM sections are then merged into one save image (every section carries its own magic+checksum).
cd /workspace/aoe-gba; O=${1:-/tmp/mig}; rm -rf $O; mkdir -p $O
for n in v132_inv0g v132_fire0 v132_race_gp0 v132_race_tt5 v132_hangar v132_sand_ui v132_vbm_0w; do
  cp tools/t134/mig/$n.txt $O/$n.txt; echo "savedump $O/$n.sav" >> $O/$n.txt; echo end >> $O/$n.txt
  ROM=build/v132.gba tools/run1.sh $O/$n.txt $O/r_$n > $O/$n.out 2>&1 &
done; wait
python3 - $O <<'P'
import sys,glob
O=sys.argv[1];img=bytearray(b'\xff'*32768)
B=[0x400,0x500,0x600,0x680,0x700,0x880,0x980,0xA00,0xA80,0xB00,0xB80,0xC00,0xC80,0xD00,0x1000,0x2000,0x2200,0x2300,0x2480,0x2500,0x2600,0x26C0,0x2680,0x2700,0x2800,0x3400,0x8000]
B=sorted(set(B))
for f in sorted(glob.glob(O+'/v132_*.sav')):
  d=open(f,'rb').read()
  for a,b in zip(B,B[1:]):
    if d[a:a+3]==b'AOE' and img[a:a+3]!=b'AOE':img[a:b]=d[a:b];print(f.split('/')[-1],'section',hex(a),chr(d[a+3]))
open(O+'/v132_final.sav','wb').write(img)
P
