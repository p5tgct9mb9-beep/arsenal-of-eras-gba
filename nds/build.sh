#!/bin/bash
# build.sh - regenerate the DS port sources from ../src and build ArsenalOfEras.nds (BlocksDS)
set -e; cd "$(dirname "$0")"
export PATH=/opt/wonderful/bin:$PATH BLOCKSDS=${BLOCKSDS:-/opt/wonderful/thirdparty/blocksds/core}
python3 tools/port.py
[ nitrofiles/inspect.bin -nt art/inspect_data.py ] && [ nitrofiles/inspect.bin -nt art/mkinspect.py ] || python3 art/mkinspect.py nitrofiles/inspect.bin
make -j${JOBS:-8} "$@"
[ -x tools/dsharness ] || gcc -O2 -o tools/dsharness tools/dsharness.c -ldl
ls -l ArsenalOfEras.nds
