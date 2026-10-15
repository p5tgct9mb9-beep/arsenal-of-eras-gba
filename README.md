# Arsenal of Eras — GBA demake (personal build) · v1.1

A Game Boy Advance homebrew demake of LT Vasquez's *Arsenal of Eras*: a top-down 240x160 pixel-art shooter.
It's a personal build, made to be played in an emulator such as Delta on iPhone. All art and music are original and generated procedurally.

**ROM:** `ArsenalOfEras.gba` (header ARSENALERAS / AOEG, SRAM save, save format v3).
Download it from the GitHub release, then use *Open in… → Delta*. Saves from v1.0 and from the v1.1 WIP builds are migrated automatically.

## Main menu
CAMPAIGN · 6V6 INFANTRY · 6V6 TANKS · 6V6 DOGFIGHT · DRONE OPS · LOADOUT · SETTINGS

## Campaign (14 missions, WW2 → Modern)
You pick a mission, see two briefing cards (story, then objectives) and finish on a stats screen showing time, objectives, enemies downed, times downed, civilians and a 1–3 star rating.
* **Unlocks:** finishing a mission unlocks the next one, and progress is saved to SRAM.
* **Test/cheat:** hold **L+R** and press **SELECT** on the mission select screen to unlock everything. Inside a mission the same combo completes the objectives.
* **Characters:** every protagonist is fictional, placed inside a real battle and treated with respect.

| # | Mission | Setting | Mode | Objectives | Side choice |
|---|---|---|---|---|---|
| 1 | Point Luck | Midway, Jun 1942 | dive-bomber (SBD) | break fighter screen, bomb 2 carriers | yes (IJN fighter pilot: intercept / protect fleet) |
| 2 | Steel Winter | Stalingrad, winter 1942 | infantry, snow | clear block, find the enemy sniper, reach the elevator | yes (encircled Axis infantry: hold / break out) |
| 3 | Black Sky | Normandy hedgerows, 6 Jun 1944 night | infantry | regroup 3 troopers, destroy 2 flak guns, clear crossroads | yes (garrison: stop paratroopers / hold) |
| 4 | Dog Green | Omaha Beach, 6 Jun 1944 | scripted v1.0 mission | beach, Bangalore, bunker | – |
| 5 | Frozen Forest | near Bastogne, Dec 1944 | tanks, snow | hold the crossroads (75 s), break the push | yes (Axis tank crew: break line / reach crossroads) |
| 6 | Suribachi | Iwo Jima, Feb 1945 | infantry, ash | get off the terraces, 3 pillboxes, hold overnight | yes (island defender: hold / repel) |
| 7 | Tidal Wall | Inchon, Sep 1950 | infantry | beat the 90 s tide to the wall, warehouses, OP radio | yes (harbour defender: hold / repel) |
| 8 | Frozen Reservoir | Chosin, Nov 1950 | convoy escort | escort trucks (≥2 survive), hold the pass | yes (volunteer infantry: cut road / hold) |
| 9 | Air Cavalry | Ia Drang, Nov 1965 | LZ defence | defend the LZ, reach the cut-off squad, push back | yes (PAVN infantry: assault LZ / hold under artillery) |
| 10 | Perfume River | Hue, Feb 1968 | infantry + civilians | clear block, lead 4 civilians to safety, reach the gate | no (civilians) |
| 11 | Left Hook | 73 Easting, Feb 1991 | tanks, desert | destroy dug-in armour, crest the ridge | yes (defending tank company: hold / stop the advance) |
| 12 | Relief Column | Mogadishu, Oct 1993 | convoy + civilians | clear roadblocks, guide the UN column, secure the site | no (civilians) |
| 13 | Shah-i-Kot | Mar 2002 | infantry, mortars | hold blocking position, climb to the crash site, hold | no |
| 14 | Phantom Fury | Fallujah, Nov 2004 | infantry, ROE | clear district, evacuate the family, seize the cache | no (civilians) |

**Side choice:** 9 missions offer a second perspective with mirrored objectives (attack ↔ defend). Uniform colours are swapped, and every faction uses generic markings with no insignia. The alternate side is never offered in missions that include civilians. Missions with civilians enforce ROE: harming 2 civilians fails the mission.

**Left out:** East Berlin "Checkpoint" (needs stealth systems that aren't built) and a flamethrower.

## 6v6 (Infantry / Tanks / Dogfight)
The flow is **Choose side → Arsenal → Map → match**. There's no era select.
* **Factions (11):** USA, UK, USSR, Germany, Japan, China, Russia, NATO, Coalition, Federation, Syndicate. Each team is coloured by its faction, and the bots fight on the other side.
* **Arsenal:** one pool across all eras.
  * 33 primaries, 13 sidearms, 10 launchers plus none, 6 lethals, 4 tacticals, 10 tanks and 21 aircraft.
  * Use L/R to jump between era tabs and Left/Right to browse.
  * Attachments and perk apply to the primary.
* **Bots:** balanced, mirrored kits. Bot *k* on each team draws gear from the same random era. On a WW2 aircraft roll, Axis-style factions fly Axis types and the others fly Allied types.
* **Maps:** **60** (20 per mode, 4 per era per mode), all selectable regardless of era, plus RANDOM. A map's era sets its terrain and soldier look; L/R skip by era.
* **Naval:** there are no ships or naval mode, so no naval maps.

## Drone Ops (Modern only)
* **Drones (9):** MQ-9 Reaper, MQ-1C Gray Eagle, MQ-1 Predator, RQ-4 Global Hawk (recon/mark), Bayraktar TB2 (MAM-L), MQ-20 Avenger, Switchblade 600 (kamikaze), Wing Loong II, MQ-4C Triton (maritime recon, unarmed).
* **Call-ins:** artillery and AC-130.
* **Missions (6):** Highway Interdiction, SAM Belt, Town Overwatch, Coastal Watch (maritime), Score Attack and Support 6v6. All are fictional, set in generic locations.
* **Systems:** thermal overlay (WHOT/BHOT/TV), SAM/AAA threats that scale with altitude, and civilian/friendly penalties.

## Loadout menu
* **Campaign kits (by era):** weapons, attachments, perk, tank and aircraft per era.
* **6v6 Arsenal (all eras).**
* **Character creator:**
  * Name, typed on an on-screen keyboard (up to 10 characters).
  * Look: 6 skin tones, 5 hair colours, 3 face styles.
  * Kit: headgear (helmet/beret/cap/boonie/none), 8 uniform colours, 5 camo patterns, gear (none/webbing/plate carrier/radio pack), standard/slim build.
  * Live preview; use it in game ON/OFF.
  * Your soldier is the player sprite in 6v6 infantry and all on-foot campaign missions, including Dog Green.

## Settings (saved)
* **Music:** AUTO (era/mode default), any of the 10 original PSG tracks, or OFF.
* **Tracks:** Bugle Call, Dog Green, Hedgerow March, Swing Squadron, Iron Curtain, Task Force, Neon Protocol, Void Drift, Arsenal Theme, Victory.
* **Other options:** music volume, SFX volume, screen shake, difficulty (Easy/Normal/Hard) and control hints.
* **Vibration:** not available on GBA.
* **Sequencer:** VBlank-IRQ driven.

## Controls
| | Infantry / Campaign | Tanks | Air | Drone |
|---|---|---|---|---|
| D-pad | move | throttle / hull | turn / throttle | fly reticle |
| A | fire | main gun | guns | fire weapon |
| B | lethal | coax MG | rockets / missiles / bombs | hold: scan / mark |
| L | swap weapon (primary/sidearm/launcher) | turret left | roll | cycle weapon |
| R | strafe | turret right | roll | slow |
| SELECT | tactical | centre turret | scoreboard | thermal mode |
| START | pause | pause | pause | pause |

Objective arrows and brackets point to the current target, and the radar shows objective markers.

## Build / test
* `make` builds the ROM. It needs `gcc-arm-none-eabi` and `python3-pil`, and is bare metal with its own crt0 and linker script.
* `tools/gen_assets.py` draws every sprite, tile, palette and the music data procedurally.
* Headless tests: `make tools/harness` (needs libmgba). Then run `tools/harness ROM script outdir [save]` and `python3 tools/topng.py outdir`.
* Screenshots are in `shots/v11_*.png` (3x), and WAV captures of the music are in `shots/v11_music_*.wav`.
