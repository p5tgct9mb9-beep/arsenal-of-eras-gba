CC=arm-none-eabi-gcc
CFLAGS=-mcpu=arm7tdmi -mthumb -mthumb-interwork -O2 -Wall -Wno-unused -Wno-misleading-indentation -ffreestanding -fno-tree-loop-distribute-patterns -fno-strict-aliasing
ROM=ArsenalOfEras.gba
all: $(ROM)
src/assets.c src/assets.h: tools/gen_assets.py
	@mkdir -p build
	python3 tools/gen_assets.py
build/game.elf: src/crt0.s src/main.c src/core.inc src/camp.inc src/mp.inc src/assets.c src/assets.h src/gba.h src/gba.ld
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
