# Arsenal of Eras — Nintendo DS edition (nds/)

The DS port reuses the GBA game source in `../src` unchanged. `tools/port.py` copies it into `gen/`,
rewrites the few hard-coded GBA hardware addresses to the compatibility layer in `source/gba_ds.h`,
and moves large assets (16 KB and up) into NitroFS (`nitrofiles/assets.bin`), which is paged in on demand.

* `source/platform.c`: per-frame push of the shadowed GBA display registers and OAM to the DS main engine. The 240x160 picture is centred on the 256x192 top screen with a window border, and GBA bitmap modes are converted. Also handles VBlank music and NitroFS asset paging.
* `source/sound.c`: translates the GBA PSG registers to DS PSG, PCM and noise channels, with software envelope, length and sweep.
* `source/save.c`: the GBA SRAM image lives in RAM and is saved to the DS save chip as RLE-compressed ping-pong 4 KB slots, which fit the 8 KB EEPROM melonDS and Delta give this cart. GBA saves can be imported from Slot-2 or `fat:/ArsenalOfEras.sav`, or converted offline with `tools/gbasav2ds.py`.
* `source/touchui.c`: the bottom-screen touch UI (title, main-menu tiles, category lists and mode options, and a virtual pad).

Build: `./build.sh` (needs the BlocksDS toolchain under /opt/wonderful).
Test: `tools/dsrun1.sh SCRIPT OUTDIR [gba.sav]` runs one script. `tools/dsregress.sh OUTDIR` runs all 220 GBA regression scripts on melonDS (libretro core, headless `tools/dsharness`) and compares end states with the GBA v1.3.5 baseline.
The header uses game code ZAOE and a DS-only unit code. With these, melonDS treats the ROM as a retail cart and gives it an 8 KB EEPROM. Homebrew carts with code `####` get no save memory in melonDS or Delta.
