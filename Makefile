CC=arm-none-eabi-gcc
CFLAGS=-mcpu=arm7tdmi -mthumb -mthumb-interwork -O2 -Wall -Wno-unused -Wno-misleading-indentation -ffreestanding -fno-tree-loop-distribute-patterns -fno-strict-aliasing
ROM=ArsenalOfEras.gba
all: $(ROM)
src/assets.c src/assets.h src/vehdb.inc src/worlddb.inc src/wfdb.inc src/platdb.inc src/pflevels.inc: tools/gen_assets.py tools/gen_vehicles.py tools/gen_world.py tools/gen_platform.py tools/gen_courses.py tools/gen_levels.py tools/gen_kc.py tools/gen_wf.py tools/wf_roster.py tools/gen_bigart.py
	@mkdir -p build
	python3 tools/gen_assets.py
src/rocketdb.inc: tools/gen_rockets.py
	python3 tools/gen_rockets.py
src/racedb.inc: tools/gen_race.py tools/gen_race_cars.py
	python3 tools/gen_race.py
build/game.elf: src/crt0.s src/main.c $(wildcard src/*.inc) src/v131db.inc src/racedb.inc src/assets.c src/assets.h src/gba.h src/gba.ld
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

src/v131db.inc: tools/gen_v131.py tools/world_mask.bin
	python3 tools/gen_v131.py
