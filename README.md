# Arsenal of Eras — GBA demake (personal build)

A Game Boy Advance homebrew demake of LT Vasquez's *Arsenal of Eras*: a top-down 240x160 pixel-art shooter.
It's a personal build, made to be played in an emulator such as Delta on iPhone.

**ROM:** `ArsenalOfEras.gba` (header: ARSENALERAS / AOEG, 32 KB SRAM save).
Download it from the GitHub release, then use *Open in… → Delta* (or import it from Files).

## Modes
* **CAMPAIGN – "Dog Green"** (Omaha Beach, 6 June 1944). You play Cpl. Walter Hale (a fictional character).
  You start on a landing craft at the bottom of the beach. Advance north past Czech hedgehogs while the bunker MG fires on you;
  hedgehogs, the seawall and sandbags stop bullets. At the wire, stand on the arrow and press **A** to place the Bangalore.
  Then grenade the MG bunker (3 hits). Briefing and radio messages appear in text boxes, and the mission ends on a results screen.
* **6V6 INFANTRY**: team deathmatch, you plus 5 ally bots against 6 enemy bots. First to 30 kills or 5:00 wins, with respawns.
* **6V6 TANKS**: the hull and turret rotate separately, shells splash, each tank has HP, and coax MG.
* **6V6 DOGFIGHT**: a wrap-around 1024x1024 sky with guns and limited missiles or rockets. Planes cast shadows and fly under parallax clouds.
* **Eras** for every 6v6 mode: WW2 / Cold War / Modern / Futuristic / Space. Each era changes the tileset, palette, sprites, weapons and vehicles.
* **LOADOUT** (from the main menu, and before every match):
  * Per era you pick a Primary (4–5 per era), a Sidearm (2 per era) and four attachment slots: Optic, Muzzle, Grip and Magazine.
  * You also pick a Lethal grenade (Frag/Semtex/Cluster), a Tactical (Flashbang/Smoke/Stim) and a Perk (Sprinter/Flak Jacket/Scavenger/Sleight of Hand/Juggernaut).
  * Live stat bars show damage, fire rate, range, recoil and mobility.
  * You also pick a tank and an aircraft per era. WW2 aircraft: P-51D, Spitfire IX and **F6F Hellcat** (its own sprite; 140 HP, heavy guns, 6 HVAR rockets).
  * The loadout is saved to cartridge SRAM.

## Controls
| | Infantry / Campaign | Tanks | Dogfight |
|---|---|---|---|
| D-pad | move (aim follows) | Up/Down throttle, Left/Right hull | Left/Right turn, Up/Down throttle |
| A | fire (campaign: plant Bangalore at arrow) | main gun | guns |
| B | lethal grenade | coax MG | missile / rockets |
| L | swap primary/sidearm | turret left | barrel roll (dodge) |
| R | hold = strafe (lock aim) | turret right | barrel roll |
| SELECT | tactical (flash/smoke/stim) | re-centre turret | hold: scoreboard |
| START | pause (Resume / Quit) | pause | pause |

Menus: D-pad + A, B back. In the loadout screen, L/R switch era.
Debug and practice combo: **L+R+SELECT**. In the campaign it warps to the next objective; in 6v6 it ends the match.

## Build / test
* Needs `gcc-arm-none-eabi`, `make` and `python3-pil`. The project is bare metal (own crt0, linker script and register header), so it needs no devkitPro.
* `make` runs `tools/gen_assets.py`, which draws every tile, sprite, palette and the title art procedurally. Then it compiles and runs gbafix.
* Test harness: `make tools/harness` (needs `libmgba-dev`). Then run `tools/harness ArsenalOfEras.gba tools/shots.txt <dir>` and `python3 tools/topng.py <dir>`.
* Screenshots are in `shots/` (scaled 3x).
