# Arsenal of Eras GBA — v1.3.5 (checkpoint)

Saves from v1.3.1 through v1.3.4 carry over. Save version 7 adds a v6→v7 step because three new modern 6v6 aircraft shift the flat aircraft index. The World Flight block at SRAM 0x400 also gets a one-time remap from the old 4-aircraft index to the new 113-aircraft index. Both migrations are tested (see Tests).

## New: Mars Rocks payouts in every mode
- After every session a **MISSION PAY** card shows the breakdown: wins × the mode's base pay, plus 4 per loss, plus a time bonus (up to 12). You get at least 2 Mars Rocks for any real session, and at most 120 per session.
- Base win pay: 30 for campaign, FPS, LRRP, Bradley and Spec Ops; 25 for 6v6, sniper, EOD and drones; 15 for sword, invaders, Grand Prix and Time Trial; 20 for other modes. Fire training pays nothing.
- The Arcade Cabinet keeps its own per-game payout.

## Fixed: 6v6 Dogfight / Armor pacing and bot variety
- Air combat is slower:
  - Speed is ×11/16 and turn rate ×3/4.
  - Bullets and missiles close more slowly (×3/4).
  - The lock cone is narrower and the missile cooldown is longer (90 frames instead of 40).
  - Damage is halved.
  - Respawn takes 4.3 s instead of 3 s.
  - Bots fire missiles less often.
- Bots now fly **era-correct mixed aircraft**, picking unused types first. Armor bots drive a mix of era vehicles: tank destroyers, IFVs and armored cars as well as tanks.
- New modern 6v6 aircraft: MiG-29 Fulcrum, Su-27 Flanker, F-15C Eagle.
- Measured over 300 headless beats with the player on autopilot (`shots/v135/dogfight_pacing.csv`):

  | Map | Build | Frames per kill | Average life | Missiles/min | Lock-ons/min |
  |---|---|---|---|---|---|
  | WW2 | before | 61 | 8.6 s | 62 | 7.7 |
  | WW2 | after | 104 | 15.1 s | 21 | 3.5 |
  | Modern | before | 70 | 9.7 s | 71 | 44 |
  | Modern | after | 106 | 15.2 s | 35 | 19 |

- Before/after screenshots: `shots/v135/df_ww2_before_after.png`, `df_mod_before_after.png`, `ar_ww2_before_after.png` and `ar_mod_before_after.png`.

## New: World Flight — 113 aircraft, all unlocked
- **33 WW2 + 20 Cold War + 60 Modern = 113 aircraft.** There are five categories: Planes, Drones, Helicopters, Experimental, and Wunderwaffen (WW2 experimental).
- **AIRCRAFT browser:**
  - Controls: L/R changes era (WW2, Cold War, Modern, All), SELECT cycles the category, ◀/▶ browses, A picks the aircraft.
  - Each entry has a big side-view sprite, its name, era, category and country, a 2-line factual card, top speed, and SPEED/AGILE bars.
- **Each aircraft has its own sprite:**
  - Side art uses the F-22-reference style: 6-step shade ramps, 1 px outline, panel lines, glossy canopy, drop shadow, and a dithered band background.
  - In-flight top-down sprites are generated from the same specification.
  - Shared base shapes are allowed; every aircraft has distinct proportions, wings, tails, engines, rotors, paint and markings.
- **Realistic relative performance:**
  - Top speed comes from each aircraft's real top Mach at a typical altitude, using the game's density model.
  - Ceiling, stall speed and agility come from the roster.
  - Rockets either drop from a mothership (X-1, X-15, X-43A) or launch from the ground and glide after burnout (Me 163, Ba 349).
- **Helicopters hover:**
  - UP/DOWN fly forward and back; let go to slow to a hover.
  - ◀/▶ yaw, A/B climb and descend, L/R strafe.
  - You can set down slowly on any open ground. Airfields refuel you. Touching down fast, or landing on water, is a crash.
  - The HUD shows knots and collective.
- Take-off now picks a heading clear of high terrain.
- **Markings:**
  - WW2 German aircraft carry the Balkenkreuz. There is no swastika anywhere.
  - Japanese aircraft use the hinomaru roundel only; there is no rising-sun flag.
  - Descriptions are neutral and factual, with "about"/"credited" where figures vary.
- **ROM:** the art adds about 260 KB (RLE side art plus 4bpp top-down tiles). The ROM is 4.4 MB of the 32 MB limit.

### Final roster
- **WW2 (33).**
  - Planes (19): P-51D MUSTANG, SPITFIRE MK IX, HURRICANE MK IIC, P-47D THUNDERBOLT, P-38 LIGHTNING, F4U CORSAIR, F6F HELLCAT, A6M ZERO, BF 109 G, FW 190 A, YAK-3, IL-2 STURMOVIK, B-17G FLYING FORTRESS, B-24 LIBERATOR, B-29 SUPERFORTRESS, AVRO LANCASTER, DH.98 MOSQUITO, C-47 SKYTRAIN, NORTH AMERICAN AT-6 TEXAN.
  - Helicopters (2): SIKORSKY R-4 HOVERFLY, FLETTNER FL 282 KOLIBRI.
  - Drones (2): RADIOPLANE OQ-2A, INTERSTATE TDR-1.
  - Experimental (3): BELL XP-59A AIRACOMET, GLOSTER E.28/39, NORTHROP N-9M.
  - Wunderwaffen (WW2 experimental) (7): MESSERSCHMITT ME 262, MESSERSCHMITT ME 163 KOMET, HORTEN HO 229, ARADO AR 234 BLITZ, HEINKEL HE 162, BACHEM BA 349 NATTER, DORNIER DO 335 PFEIL.
- **Cold War (20).**
  - Planes (10): F-86 SABRE, MIG-15, F-4 PHANTOM II, MIG-21, F-14 TOMCAT, SR-71 BLACKBIRD, B-52 STRATOFORTRESS, C-130 HERCULES, T-38 TALON, CONCORDE.
  - Helicopters (4): BELL UH-1 HUEY, MIL MI-24 HIND, CH-47A CHINOOK, AH-1G COBRA.
  - Drones (1): RYAN MODEL 147 LIGHTNING BUG.
  - Experimental (5): BELL X-1, NORTH AMERICAN X-15, XB-70 VALKYRIE, GRUMMAN X-29, NORTHROP YF-23.
- **Modern (60).**
  - Planes (30): F-15C EAGLE, F-15E STRIKE EAGLE, F-15EX EAGLE II, F-16C FIGHTING FALCON, F/A-18E SUPER HORNET, EA-18G GROWLER, F-22 RAPTOR, F-35A LIGHTNING II, F-35B LIGHTNING II, F-35C LIGHTNING II, F-117A NIGHTHAWK, A-10C THUNDERBOLT II, B-1B LANCER, B-2 SPIRIT, B-21 RAIDER, C-17 GLOBEMASTER III, P-8A POSEIDON, BOEING T-7A RED HAWK, MIG-29 FULCRUM, SU-27 FLANKER, SU-30MKI, SU-35S, SU-57, SU-34 FULLBACK, EUROFIGHTER TYPHOON, DASSAULT RAFALE, SAAB GRIPEN E, CHENGDU J-20, CHENGDU J-10C, KAI KF-21 BORAMAE.
  - Helicopters (12): UH-60 BLACK HAWK, AH-64E APACHE GUARDIAN, AH-1Z VIPER, CH-47F CHINOOK, MH-6 LITTLE BIRD, KAMOV KA-52, MIL MI-28, MV-22 OSPREY, CH-53K KING STALLION, UH-1Y VENOM, EUROCOPTER TIGER, MH-60R SEAHAWK.
  - Drones (10): MQ-1 PREDATOR, MQ-9 REAPER, RQ-4 GLOBAL HAWK, BAYRAKTAR TB2, MQ-25 STINGRAY, CCA WINGMAN (FURY-STYLE), RQ-170 SENTINEL, MQ-4C TRITON, KRATOS XQ-58 VALKYRIE, BAYRAKTAR AKINCI.
  - Experimental (8): X-31 VECTOR, BOEING X-32, LOCKHEED X-35, MCDONNELL DOUGLAS X-36, NASA X-43A, NORTHROP X-47B, LOCKHEED X-59 QUESST, SR-72 CONCEPT.

## Art direction (first pass)
- `shots/v135/art_before_after_f22_f16.png` compares the old 32×32 F-16C 6v6 sprite with the new large hand-built F-22/F-16 samples and the procedural World Flight renders.
- The reference image is kept outside the repository and is not distributed.

## Tests
- **Full regression** (220 scripts, `tools/regress133.sh` against the v1.3.4 baseline `build/R134`):
  - 219 match on end screen and mode.
  - `v13_eod_endless` diverges because of RNG: the randomly generated endless field differs, and the run is still in play with no crash.
- **v1.3.5 tests** (`tools/t135/`):
  - `pay_inv`: payout card.
  - `df_ww2`/`df_mod`/`ar_ww2`/`ar_mod`: pacing metrics.
  - `wf_count` with `wf_count_check.py`:
    - Era totals: 33/20/60/113.
    - WW2 by category: Planes 19, Drones 2, Helicopters 2, Experimental 3, Wunderwaffen 7.
    - Cold War: 10/1/4/5/0. Modern: 30/10/12/8/0. All eras: 59/13/18/16/7.
    - Browsing visits all 113 entries.
    - Result: **PASS**.
  - `wf_p51`: takes off and climbs.
  - `wf_heli`: UH-60 lifts off, flies forward, hovers to 0 kt and lands on open ground.
  - `wf_me163`: ground-launched rocket climbs to about 23,000 ft.
  - `wf_x15`: mothership drop reaches about 159,000 ft at Mach 3.96 within 15 s.
  - `wf_migrate`: a v1.3.4 World Flight save with the SR-71 selected opens as the SR-71 (new index 38).
- Category screenshots: `shots/v135/worldflight/*.png` and `worldflight_sheet.png`.

## Known caveats
- Side art is procedural from per-aircraft specifications. It reads clearly but is less detailed than the hand-built F-22/F-16 samples. Hand-tuned art for the most-flown aircraft is planned.
- Helicopters use the top-down sprite with no animated rotor yet.
