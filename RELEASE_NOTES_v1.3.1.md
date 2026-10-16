# Arsenal of Eras GBA v1.3.1

ROM: `ArsenalOfEras.gba` (save format unchanged; v1.3 saves load as-is, new progress lives in its own SRAM block at 0x2000).

## Bradley Ops overhaul
* New Bradley/enemy sprites, parallax backdrops and **28 set-piece courses** (desert, forest, city, winter, winter night, night thermal, armoured-train boss), plus a variant/camo/upgrades garage.
* AP/HE ammo switch, wire-guided TOW (steer the missile), armour zones, multi-phase bosses.
* Autopilot playtest: 28/28 courses cleared on Regular. Before/after in `shots/v131/v131_bradley_*`.

## Main menu redesign
* Orbital main menu: Earth from orbit (sphere LUT, scrolling continents and clouds, night lights, sun glint, limb glow), parallax stars, ISS pass, rotating launch vignettes, space-HUD panel and icons, KC as an astronaut.

## Space expansion (EXPLORE)
* **Space Guide:** Wernher von Braun (1912–1977) as a historical guide, with an honest history tour: the V-2 and the forced labour at Mittelwerk, Operation Paperclip, Explorer 1, Saturn V. Dialogue is paraphrased game text; not endorsed by his estate, NASA or any agency.
* **Space Launch:** 35 → **61 craft** (V-2/A4 MW 18014, WAC Corporal, Bumper, Vostok … Gaganyaan, plus the what-if 1945 craft).
* **Orbital Defense:** low-orbit cleanup, rogue satellites, Kessler cascade, Sun Gun boss.
* **Rocket Builder:** pick engine/count/tanks/payload; Tsiolkovsky delta-v and TWR; 6 targets from sub-orbital hop to Mars transfer; presets (F9-style, Saturn-style, Smallsat, Starship-style); records saved.
* **Moon Base:** Shackleton touchdown, base builder (hab, solar, battery, ISRU, fission), ice hunt, lunar night, long rover traverse.
* **Mars Mission:** guided EDL (bank guidance, heat shield, chute), sky crane, Starship flip-and-land, Jezero rover drive, dust storm, full mission.
* **Asteroid Mining:** rendezvous, prospect and mine, haul to depot, DART impact on Dimorphos, what-if redirect.
* **WHAT-IF 1945** (fictional alternate history, generic markings, no Nazi insignia): a 9-mission chain. A4b winged A4, Rheinbote 4-stage solid, Wasserfall SAM (dodge it in a recon plane with WINDOW chaff), Natter vertical launch, A9/A10 "Amerika", Silbervogel, the Allied counter (Lancer X), the Soviet counter (Zarya-45) and a Sun Gun finale. Every mission ends with a fact card on what really happened, e.g. Mittelwerk forced labour, Lothar Sieber's death in the Natter, Wasserfall never entering service, Silbervogel's underestimated reentry heating, Paperclip and Osoaviakhim.

## Secret: OVERFLIGHT (SPECIALIST › CLASSIFIED, access code 2003)
* Cold War strategic recon in a U-2 or SR-71: 10 missions (North Sea training, East German border, Baltic Fleet, Ukraine airbases, Baikonur, Grand Slam route, Kola, Plesetsk, Kamchatka, Moscow outskirts) plus a random sortie.
* Theatre maps with radar coverage. SA-2 and MiG-19/21/25 intercepts are flown side-on (climb, ECM, break turn). Photo runs with Type B camera, optical bar camera or SLAR, plus clouds and film limits. KC-135Q refuelling, then a film-lab intel report with stars. The code lock is saved once granted.

## Cold War music pack
* 9 original PSG tracks: DEAD DROP, MACH 3 PULSE, THIN AIR, LAUNCH WARNING, FILM DEVELOPED, ACCESS GRANTED, FULDA GAP '85, DELTA FUNK, RED SQUARE MARCH. All can be picked from the Settings music-track option.

## Fixes
* Rocket Builder TWR/burn-time maths, the Smallsat preset overstressing at max-Q, a stale title on result screens (now a space backdrop).
* Rover autopilot slope/thrust balance, DART SmartNav aiming, haul collision avoidance.
* Unclipped fill in the Overflight photo scene could corrupt EWRAM; now clipped.
* Overflight heading maths, SR-71 turn rate, SA-2/MiG ceilings.
* Rheinbote no longer overstresses at max-Q; Wasserfall's radio-command guidance now loses the track in chaff.
* Button taps made while the orbital main menu is drawing (~0.4 s on entry) are now queued instead of dropped.

## Testing
* Autopilot scripts for every new mission (`tools/v131_*.txt`). Full regression of all 175 scripts against the v1.3 baseline: no crashes or hangs. The only end-state differences are the old v1.3 Bradley scripts (the courses were replaced; the new 28-course autopilot suite clears 28/28) and one settings script that ends during the menu redraw.
* Not verified: real GBA hardware, and hand-played feel/balance (testing was autopilot and emulator).
