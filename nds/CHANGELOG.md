# Arsenal of Eras DS: changelog

## ds-v0.2.6 (Oct 17, 2026): rebuilt aircraft sprites (ds-v0.3.0 part 2)

Second checkpoint of ds-v0.3.0. The top-down aircraft sprites used in Dogfight, Campaign air missions and the hangar are rebuilt from real dimensions (`art/veh_air.py`).

* **147 aircraft are redrawn from a spec** of real length and span, wing planform, wing position, root and tip chord, sweep, engine layout, tail type and canopy position.
  * **Wing planforms:** tapered, elliptical (Spitfire, He 111, Tempest, P-47), gull (Corsair, Stuka), swept (MiG-17 to Su-27), delta (MiG-21, Mirage, A-4), cranked delta with LERX (F-16, F/A-18, MiG-29), forward-swept (Ju 287), flying wing (B-2, Ho 229, XP-79), and biplane and triplane wings for WWI aircraft.
  * **Engines:** nose propellers drawn as see-through blur discs, twin and four-engine wing nacelles with spinners, push-pull (Do 335), pushers (XP-55, Gotha), wing-pod jets (Me 262, Ar 234), rear pods (A-10), single and twin jet nozzles with an afterburner glow, and the He 162's dorsal jet.
  * **Tails:** single fin, twin fins (B-25, Lancaster, F-15, Su-27), canted fins (F-35, F/A-18, Su-57), twin booms (P-38, P-61), T-tails (F-104), cruciform tails (Do 335), tailless deltas with a fin (Mirage, Gripen, J-10), and canards (Gripen, J-10, Su-30, J-15).
  * **Details:** tinted canopies with a sky glint and frame, radial cowlings, belly scoops, intakes, conformal fuel tanks (F-15E/EX), tip tanks, floats, gun pods, wing roundels in team colours, and helicopter rotors drawn as thin dashed blur (Cobra, Hind, Apache, Ka-52 coaxial).
  * Fuselages are drawn 40% fatter than true scale so they read at 32 px. The larger of span or length keeps the GBA sprite size.
* **Not rebuilt yet:** balloons, the Da Vinci machines, the Triebflügel, BV 141, XF5U and the sci-fi craft. These keep the re-shaded GBA art.

## ds-v0.2.5 (Oct 17, 2026): rebuilt tank sprites (ds-v0.3.0 part 1)

First checkpoint of ds-v0.3.0. The top-down armour sprites are rebuilt from real vehicle dimensions. Controls and game rules are the same as in ds-v0.2.4.

* **91 tanks, tank destroyers, SPGs, IFVs and armoured cars are redrawn** (`art/veh_hd.py`). Each vehicle has a spec with its real hull length and width, track width, turret shape, size and position, gun length and calibre, and its fittings. The spec is drawn as a layered 4x model with a height field, lit from the front-left like the approved side-view art, and reduced to the 32x32 hull and turret sprites.
  * **Proportions:** each vehicle keeps its GBA hull length, so gameplay size is unchanged. The width comes from the real length-to-width ratio, widened 25% so it still reads at 32 px. A Sherman is now long and narrow, and a Tiger or Abrams is wide and flat.
  * **Turrets:** cast domes (Sherman, T-34/85, IS-2, Pershing), Soviet saucers (T-55 through T-90M), welded boxes (Tiger, Panzer IV, KV-1, Cromwell), flat composite turrets (Abrams, Challenger, K2, Leclerc), arrowhead wedges (Leopard 2A7/2A8, Merkava, Type 99A), and open-top rings (M10, M18, M36). Each has its mantlet, cupola with vision block, loader hatch, bustle rack, smoke dischargers, ERA bricks, sights and roof weapons where the real vehicle has them.
  * **Hulls:** track links on the pixel grid, side skirts with plate seams, fenders and stowage bins, a sloped glacis with a lit roof edge, driver hatches, engine-deck grilles, exhausts or fuel drums, spare track links, headlights, tools, casemates with their guns (Jagdpanther, StuG III, Elefant, SU-100), and rocket pods (M270, TOS-1A).
  * Colours stay on the team palette ramps, so team colours, the hangar and the loadout screens still work.
* **Not rebuilt yet:** soft-skins, halftracks, WWI vehicles, the Goliath, and the fantasy, sci-fi and medieval vehicles. These keep the re-shaded GBA art for now.
* **Build:** `AOE_GBA_VEH=1 ./build.sh` builds with the original GBA tank sprites, for comparisons.
* **Tests:** regression results are the same as ds-v0.2.4: 110 of 116 match, and the same 6 differ by timing only. Arcade: 12/12 clean.

## ds-v0.2.4 (Oct 17, 2026): Arcade fixes

Fixes found while checking the ds-v0.2.3 release. Controls are the same as in ds-v0.2.3.

* **Tank Duel tanks moved 55-110 pixels per frame.** One frame of UP/DOWN made a tank jump across the arena, and the CPU tank could reach the player and win round 1 about a second after the round started. The cause was a fixed-point mistake in the shared arcade code: `SIN*speed/2` was used where `SIN*speed>>7` was meant. The DS build now patches it (`tools/dscontrols.py`). Your tank moves 1.7 px/frame forward and 1.1 px/frame in reverse. The CPU tank moves 1.4 px/frame. Wall collision now works at these speeds.
* **Cargo Stack:** the bottom-screen arcade page showed "LIVES 0", but the game has no lives. It now shows only the level.
* **Correction to the ds-v0.2.3 notes:** Chopper Rescue draws at about 48 fps in the harness, not 54-58. Its game logic still runs at the full 60 Hz. A faster hill-drawing routine was tried, but it changed the code layout and cost 2-3% of frames in infantry missions under melonDS, so it was left out.
* **Tests:** 116 regression scripts: 110 match the ds-v0.2.3 run. The 6 that differ are timing-only (v11_m_aircav, v11_m_blacksky, v11_m_suribachi, v13_brad_boss0, v13_brad_boss3, v11_camp), and none of that code changed. Arcade: 12/12 cabinet checks clean.

## ds-v0.2.3 (Oct 16, 2026): native DSi button layout and the Arcade bug pass

* **Native DSi buttons** (`source/controls.c`, `tools/dscontrols.py`). The game now uses A, B, X, Y, L, R, START, SELECT, the D-pad and touch in every mode. Before this, it used the GBA layout.
  * **How it works:** X and Y are read through libnds (`keysHeld`, which comes from the ARM7 `REG_KEYXY`). Each physical button is translated into the GBA keys the current mode expects. Two DS-only key bits handle actions the GBA has no key for.
  * **Infantry:** A fire, B gear (claymore/sensor/strobe), X reload (or take the weapon at your feet), Y switch weapon, L aim/strafe-lock, R grenade.
  * **Armor:** A main gun, B coax/ATGM, X dismount squad or centre the turret, Y **smoke screen**, L/R turret.
  * **Dogfight:** A guns, B missile, X scoreboard, Y **flares**, L/R barrel roll. Helicopters and drones: X climbs and Y descends. This replaces the GBA SELECT+UP/DOWN chord.
  * **Naval:** A main guns, B secondary/torpedoes, X centres the turrets, Y smoke screen, L/R turrets.
  * **Smoke and flares** break incoming missile locks. Smoke reloads in 12 s and flares in 5 s.
  * **FPS:** A fire, X reload/door, Y switch weapon, B fast turn, L/R strafe.
  * **Campaign and Boot Camp** use a similar layout. See the cheat sheet.
  * **Menus:** A confirm, B back, X option, Y start/loadout, L/R tabs.
  * **Other modes:** X does the old SELECT action.
  * **Settings > Controls:** **DS** (default) or **CLASSIC (GBA)**. CLASSIC is the v1.3.5 layout, with X as SELECT. The setting is saved in a DS-only record at SRAM 0xFFF0. It replaces the GBA "Rumble N/A" row.
  * **On-screen text:** the control hints, HOW TO PLAY, the loadout notes and the Boot Camp tutorial text now name the A/B/X/Y/L/R buttons.
  * **Bottom-screen legend:** every battle page and the virtual pad page show a per-mode button legend.
  * **Cheat sheet:** `art/mkcheatsheet.py` makes the controls cheat sheet.
* **Arcade Cabinet bug pass** covering all 12 games (`tests/ds_arc_NN.txt`, `tools/arccheck.py`). Each game is opened from the cabinet and played by hand with every button for about 25 s. The test then pauses, resumes, quits, enters a score, plays again, quits, returns to the cabinet, plays a full autopilot game (up to 4 min), and leaves to the menu. Result: 12/12 clean. Bugs fixed:
  1. X and Y did nothing in the cabinet and its games. Now X = A and Y = B, and on the cabinet X plays, Y goes back and L/R choose.
  2. During arcade play the bottom screen showed the generic virtual pad with a debug "SCREEN 108 MODE n" line. It now shows an arcade page with the game card, live score, lives and level, the HI score, the controls and touch PAUSE/B buttons. The cabinet page has PREV/PLAY/NEXT, and the high-score entry page has help text.
  3. Frame drops. Each arcade frame was copied twice: frame buffer to the GBA bitmap shadow, then to VRAM. Chopper Rescue drew at 30 fps and Space Pinball at about 41 fps. Frames now go straight from the frame buffer to VRAM, and every game draws at 54-58 fps. Game logic stays at a fixed 60 Hz.
  4. The bottom-screen arcade page redrew every 8 frames, which cost frames (51 fps). It now redraws only when something changes, at most every 12 frames, and the fill routine is faster.
  5. The cabinet attract preview ran at half speed. It now catches up by VBlank.
  6. Starship Hopper fell into the floor about 1.3 s after start, before the player could react. It now waits for A ("PRESS A TO FIRE THE ENGINE") and starts by itself after 5 s. The PLAY AGAIN press no longer fires the engine.
  7. The Starship Hopper HUD showed 3 lives, but one crash ends the run. It now shows 1.
  8. Cargo Stack has no lives but showed 3. The lives display is now hidden.
  * **Checked with no problem found:** launch, exit and return to the cabinet; pause, resume and quit; game over, PLAY AGAIN and back to the cabinet; stalls (at most 3 frames); logic rate (1.00); high-score tables and play counts in SRAM 0x3400; palette or leftover-tile corruption when switching games (none seen in 108 screenshots).
* **Harness:** `DSH_CTL=classic` makes the GBA regression scripts run with the GBA layout (`dsregress.sh` sets it). Added the `ctlstat TAG` command (translated key mask, control category, X/Y counts) and `chipdump FILE`. New tests: `tests/ds_ctl_*.txt` (made by `tests/mkctl.py`) and `tests/ds_arc_*.txt` (made by `tests/mkarc.py`).

## ds-v0.2.2 (Oct 16, 2026): official cover art, wakes and afterburners

* **Official cover art** (`nds/art/cover_art.png`, also `docs/cover.png` in the README header):
  * **Title screen:** the top screen shows a 256x192 crop of the cover (Earthrise over the lunar horizon), with the title and the tagline "FROM THE TRENCHES TO THE STARS" re-set as crisp spaced text. The bottom screen shows TOUCH TO START / OR PRESS START over the lunar surface. Made by `nds/art/mkcover.py`.
  * **DS menu icon:** Earth over the Moon horizon (32x32, 16 colours).
  * **Banner text:** "Arsenal of Eras / From the trenches to the stars / DS v0.2.2 - LT Vasquez".
  * **Credits:** the opener now carries the tagline.
* **Effects:**
  * Ships leave white foam wakes on both sides of the stern. The foam uses its own sprite palette.
  * Jets (Cold War through Space) show afterburner glow at the tail.
  * The effect pool grew from 40 to 96 entries.
* **Not done yet** (still planned for ds-v0.3.0): tanks, aircraft and ships rebuilt from the detailed art, the top-screen HUD and main menu to match the mockups, and Modern+ Dogfight/Naval terrain.

## ds-v0.2.1 (Oct 16, 2026): graphics upgrade, part 1 (soldiers, terrain, effects) and save autodetect

A checkpoint on the way to ds-v0.3.0. Infantry and Dogfight look clearly better. Armor and Naval are only somewhat better, because the vehicles and ships are still the GBA sprites.

* **Infantry soldiers:**
  * 3/4 top-down 32x32 soldiers (from `sold.py`), ported from the unreleased GBA v1.3.6 work (`gba-wip 5efcaac`) through a new `nds/overlay/`.
  * Walk, aim and fire frames, with uniform, skin, hair and camo recolouring.
  * Team chevrons and nameplates near the player.
  * The 1.1 MB soldier and portrait tables stream from NitroFS one set at a time (`ast_window`), so they cost no main RAM.
* **Painted terrain** (`nds/art/terrain_ds.py`, `nitro:/terrain.bin`):
  * **Arenas** (Infantry and Armor, WW2, Cold War, Modern, Future and Space):
    * Each of the 14 map tiles has 4 painted variants: grass with tufts and flowers, rutted dirt roads, brick walls in 3/4 view, plank crates, stacked sandbags, shaded trees with drop shadows, craters, rock borders, floor slabs, bridges and water.
    * Each material has its own 15-colour palette in BG banks 1-9.
    * Variants are picked per cell, so large areas no longer repeat.
  * **Aerial and sea sets** (WW2 and Cold War Dogfight and Naval): sea with moving wave glints, beach or ice shore, farmland, field furrows and hedgerows, forest, farmhouses.
  * Collision, line of sight and AI still use the original tile ids. Only the picture changed.
  * The three bonus eras (WW1, Medieval and Revolutionary) keep their original art.
  * The bottom-screen TACTICAL map reads its colours from the new tiles.
* **Effects** (`nds/art/fx_ds.py`): repainted shared FX sheet:
  * Glowing tracers and shells, star-shaped muzzle flashes and sparks.
  * Shaded smoke puffs.
  * A 4-frame explosion: flash, fireball, break-up with embers, then smoke.
* **Saves:**
  * The save chip is autodetected: EEPROM at several sizes (half the chip, up to 32 KB) or FLASH (64 KB sectors).
  * If no usable chip is found, the game saves to `fat:/ArsenalOfEras.dssav` on the SD card. This is meant to stop "NO SAVE CHIP" on flashcarts and Delta.
  * Every chip write is read back and verified.
* **Tests:**
  * New scripts: `nds/tests/ds_inf_soldiers.txt`, `ds_armor.txt` and `ds_naval.txt`.
  * `nds/tools/beforeafter.py` builds the before/after sheets.
* **Not done yet** (planned for ds-v0.3.0):
  * Tanks, aircraft and ships are still the GBA sprites.
  * The top-screen HUD and the main menu are unchanged.
  * No new wakes or afterburners yet.

## ds-v0.2.0 (Oct 16, 2026): second-screen gameplay and the Museum (INSPECT)

* **Bottom screen during gameplay (Phase B):**
  * **TACTICAL page** for infantry, armor, air, naval and campaign battles:
    * A live map of the whole arena, coloured from the real map tiles.
    * Friendly and enemy blips, your heading, and the top-screen view box.
    * HP bar, weapon, kills/downs, score or bunker HP, grenades or fuel, and the timer.
    * Four hold-type action buttons per mode, e.g. FIRE / GRENADE / SWAP / STRAFE or GUN / MG-ATGM / TURRET L / R.
  * **GRAND PRIX page:**
    * Full track map with every car.
    * Position, lap and the top-4 standings.
    * THROTTLE / BRAKE / BOOST / DRIFT buttons.
  * The PAD button still switches to the virtual pad at any time.
* **Museum / INSPECT (Phase C start):**
  * 36 entries: Hangar 16 aircraft, Motor Pool 14 land vehicles, Armory 6 weapons.
  * Each entry has a full-screen top view plus three touch tabs (OVERVIEW / ARMAMENT or FITTINGS / HISTORY; the M1 Garand has OVERVIEW / HISTORY / ATTACHMENTS) with rounded, documented facts.
  * The F-4E and M1 Garand use the approved high-detail art.
  * Open it from MUSEUM on the main-menu bottom screen, or from INSPECT on the Loadout and Hangar screens.
  * Controls: L/R or the footer for previous/next, D-pad or touch to browse, B to back out. The game stays paused underneath.
  * The screens are generated by `nds/art/mkinspect.py` with the shared art toolkit (`nds/art/`).
* The title and menu read DS EDITION V0.2.0.

## ds-v0.1.0 (Oct 16, 2026): first playable DS build

The first playable DS build. It runs all v1.3.5 GBA modes on the DS top screen. The bottom screen has the touch UI.

* **Engine (Phase A):**
  * Display: the GBA 240x160 picture is centred on the 256x192 top screen. Tiled, affine and bitmap modes, windows, blending, sprites and affine sprites are supported.
  * Grand Prix: the per-scanline (HBlank DMA) mode-7 floor is translated to DS HBlank DMA.
  * Assets: large assets (1.9 MB) are paged in from NitroFS through an LRU cache.
  * Sound: the GBA PSG registers drive DS PSG, PCM and noise channels. Pitches match GBA captures.
* **Saves:**
  * The 64 KB game save is RLE-compressed into two ping-pong 4 KB slots on the 8 KB save EEPROM.
  * Writes are spread over frames, and each write is read back from the chip and checked.
  * GBA saves are imported automatically on first boot, or while SELECT is held, from a GBA cart in Slot-2 or `fat:/ArsenalOfEras.sav`.
  * `nds/tools/gbasav2ds.py` converts a GBA .sav to a DS save file offline.
* **Bottom screen:**
  * Tap-to-start title.
  * The ten main-menu tiles as touch buttons.
  * Category lists with their mode options (PLAY, HIGH SCORES, ...).
  * A virtual pad for everything else.
  * A BACK / SELECT / START / PAD bar.
* **Title:** reads "DS FIELD EDITION". The main menu reads "DS EDITION V0.1.0". There is a new DS menu icon.
* **Tests:**
  * The 220 GBA regression scripts run headless on melonDS (`nds/tools/dsregress.sh`), with screenshots of both screens.
  * `dsbeatcmp.py` compares every checkpoint with the GBA v1.3.5 baseline.

Known limits:
* Tested only in melonDS. It has not been run on real hardware or flashcarts.
* The save code expects an EEPROM save chip. Loaders that give the cart FLASH or no save will fall back to "NO SAVE CHIP" or may not persist.
* During gameplay the bottom screen is still the generic virtual pad. Maps, radar and HUD come in ds-v0.2.0.
