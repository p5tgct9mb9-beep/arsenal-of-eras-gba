CC=arm-none-eabi-gcc
CFLAGS=-mcpu=arm7tdmi -mthumb -mthumb-interwork -O2 -Wall -Wno-unused -Wno-misleading-indentation -ffreestanding -fno-tree-loop-distribute-patterns -fno-strict-aliasing
ROM=ArsenalOfEras.gba
all: $(ROM)
src/assets.c src/assets.h src/vehdb.inc src/worlddb.inc src/platdb.inc src/pflevels.inc: tools/gen_assets.py tools/gen_vehicles.py tools/gen_world.py tools/gen_platform.py tools/gen_courses.py tools/gen_levels.py tools/gen_kc.py
	@mkdir -p build
	python3 tools/gen_assets.py
src/rocketdb.inc: tools/gen_rockets.py
	python3 tools/gen_rockets.py
build/game.elf: src/crt0.s src/main.c src/core.inc src/camp.inc src/mp.inc src/camp2.inc src/drone.inc src/loadout.inc src/char.inc src/world.inc src/worlddb.inc src/plat.inc src/kc.inc src/launch.inc src/duel.inc src/aa.inc src/menu.inc src/sniperdb.inc src/brad.inc src/braddb.inc src/sniper.inc src/eod.inc src/lrrp.inc src/space.inc src/explorer.inc src/fps.inc src/fpslut.inc src/aadb.inc src/rocketdb.inc src/platdb.inc src/pflevels.inc src/vehdb.inc src/assets.c src/assets.h src/gba.h src/gba.ld
	@mkdir -p build
	$(CC) $(CFLAGS) -nostartfiles -nostdlib -T src/gba.ld -o $@ src/crt0.s src/main.c src/assets.c -lgcc
$(ROM): build/game.elf tools/gbafix
	arm-none-eabi-objcopy -O binary $< $@
	./tools/gbafix $@ -t"ARSENALERAS" -cAOEG -m01 -r0
	@ls -l $@
tools/gbafix: tools/gbafix.c
	gcc -O2 -o $@ $<
tools/harness: tools/harness.c
	gcc -O2 -o $@ $< -lmgba
clean:
	rm -f build/game.elf $(ROM)

src/aadb.inc: tools/gen_aa.py
	python3 tools/gen_aa.py

src/sniperdb.inc: tools/gen_sniper.py
	python3 tools/gen_sniper.py
