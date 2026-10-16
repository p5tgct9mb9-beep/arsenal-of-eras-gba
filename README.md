# Arsenal of Eras — GBA demake (personal build) · v1.3

A Game Boy Advance homebrew demake of LT Vasquez's *Arsenal of Eras*: a top-down 240x160 pixel-art shooter.
It's a personal build, made to be played in an emulator such as Delta on iPhone. All art and music are original and generated procedurally.

**ROM:** `ArsenalOfEras.gba` (header ARSENALERAS / AOEG, SRAM save, save format v5).
Download it from the GitHub release, then use *Open in… → Delta*. Saves from v1.0, v1.1 WIP and v1.1 (v3) are migrated automatically. Arsenal picks are re-mapped onto the bigger v1.2 lists (tested with a real v1.1 save), and Bot Difficulty starts at Regular. Spec Ops progress, the KC tutorial flag and the new attachment slots each live in their own checksummed SRAM blocks.

## Main menu
Categories: **CAMPAIGN · BATTLES** (6v6 Infantry / Armor / Dogfight / Naval, Sword Fight 1v1) **· SPEC OPS** (Spec Ops, Bradley Ops) **· SPECIALIST** (Sniper, EOD, Anti-Aircraft, Drone Ops, LRRP, FPS) **· EXPLORE** (Space Launch, World Flight, Space Program, Astronaut Explorer) **· LOADOUT · SETTINGS · BOOT CAMP (KC)**. Every mode is finished: none is marked IN TRAINING any more.

## What's new in v1.3
* **Saves:** v1.2 saves migrate automatically (tested with a real v1.2 save).
  * Spec Ops progress block v1 → v2: stars are kept and the new worlds start at zero.
  * The old *UKRAINIAN SOF* unit becomes the **3rd Separate SOF Regt** on the new UA tab.
  * Campaign unlocks are bridged, so a player who had finished the old 14 missions also gets FULDA GAP and GUNS OF TEXAS.
* **Campaign: 14 → 27 missions.**
  * New: FULDA GAP (what-if), GUNS OF TEXAS (naval gunnery), TORA BORA, MEDUSA, MOSHTARAK, HOSTOMEL, IRPIN BRIDGE, OLD CITY (Mosul), LAKE CITY (Marawi), KHARKIV DASH (armour), LAST FLIGHTS (Kabul evacuation), BLACK SEA SKIES (flight) and IRON CURTAIN 2035 (what-if, future era).
  * Modern missions now open the modern loadout. They used to show the WW2 title and guns.
* **Bot AI and buildings (campaign + 6v6):**
  * Bots need line of sight to fire, except at point-blank range, and hurt bots retreat into buildings.
  * Allies without a role follow you in formation.
  * Buildings can be entered: plank floors, second doors, and farmhouses on field and forest maps. Standing inside gives **25% cover** (*INDOORS - COVER*).
* **Enemy weapon pickup:** fallen enemies drop their guns.
  * 6v6/campaign: **L** takes the weapon (*L: TAKE x*).
  * Spec Ops: drops are a captured AK / PKM / RPG-7 / SVD (*SEL SWAPS*).
  * FPS: AK-47 and PKM pickups.
* **Three new eras: WW1, American Revolution and Medieval.**
  * Each era has its own weapons, vehicles, aircraft and maps: Somme, Cambrai, Lexington, Yorktown, Agincourt, Castle Siege and more.
  * In 6v6, choosing a period map equips both teams with period kit, for example Lee-Enfield, Mark IV, English Longbow and Battering Ram.
* **Spec Ops:**
  * **UA tab** with 17 Ukrainian units (3rd/8th Separate SOF Regt, 4th Ranger Regt, 7th SOF Regt, 140th/144th SOC, 73rd Naval SOC, Omega, HUR, Bureviy, Harmash, Azov, Chervona Kalyna, Kara Dag, Lubart, Pilum, Forge).
    * Each unit has its own sprite, default weapons and loadout, with generic badges only.
    * The unit grid scrolls. 47 units in total.
  * **Worlds 11–13:**
    * Western Front 1917: trenches, barbed wire, A7V boss.
    * Colonial War 1776: Delaware, frigate boss.
    * Castle Siege: dungeon, trebuchet and siege-tower bosses.
    * Each world has its own trench or castle backdrop.
* **LRRP** (jungle patrol) and **FPS vs Bots** (raycast 3D, 8 missions) are complete.
  * The FPS renderer was rewritten in ARM/IWRAM with a reciprocal LUT. It went from 16–19 fps to **~26 fps on average (range 22–29)** in mGBA-equivalent timing.
  * New wall textures: concrete, brick, riveted metal, sandbags and doors.
* **Art pass, original art blending three GBA-era styles** (see `shots/v13/v13_style_*.png`, before/after):
  * **Platformer style** (Spec Ops, Bradley Ops, menus, world maps): brighter, more saturated palettes, round dome hills, puffy parallax clouds, and squash-and-stretch on the player's jumps and landings.
  * **Anime action-RPG style** (campaign, 6v6, sword fight):
    * Cel-shaded briefing portraits in a dialogue-box layout: 42 variants by era, role, gender and skin tone. KC's boot-camp portrait also gets a speed-line burst.
    * Energy-style muzzle flashes, star impact sparks, and explosions with a white core and radial speed lines.
    * Sword fight: energy slash arcs, hit starbursts, dodge speed lines and screen shake.
  * **Gritty FPS style:** a HUD face that reacts to damage (bruises, blood, grimace when hit, glances around), chunkier outlined guns, and darker, grimier textures.
* **Bugs fixed in v1.3:**
  * Deaths in tunnel gaps.
  * A freeze when a new boss appeared, and the A7V named "MERC CONVOY".
  * Period 6v6 maps showing WW2/modern kit.
  * Worlds 11/13 using generic city/mountain art.
  * The WW2 loadout appearing on modern campaign missions.
  * Lost menu taps on slow redraws (test scripts).
  * IWRAM overflow, fixed by moving the AA/Bradley pools to EWRAM.
* **Space Launch (new main-menu mode):** 35 real launch vehicles from 6 agencies. You fly the countdown (TR_ASCENT theme), the ascent (throttle, pitch, stage separation, max-Q stress), booster landings, a debris field and docking. Best scores are saved to SRAM. See [Space Launch](#space-launch) below.
* **Spec Ops enemy factions per world:** worlds 1–6 are renamed and rethemed, and each has its own enemy sprites, colours, vehicles and boss. W1 Border Towns are held by cartels, W2 Desert Compounds by proxy militias, W3 The DMZ by the KPA, W4 Mountain Bases by the CSTO, W5 Arctic City by Russian forces and W6 Island Fortress by the PLA (carrier finale). W7 Wagner, W8 NVA/VC + Warsaw Pact, W9 Axis and W10 aliens are unchanged.
* **4 Israeli units:** Sayeret Matkal, Shayetet 13, Unit 669 and a Mossad operative. That makes 25 units.
* **Unit choice is now up front:** entering Spec Ops always opens SELECT UNIT. It's a grid grouped by nation, with flag-colour tags and a big portrait, and all 25 units are free from the start. Every course opens with a *DEPLOY AS: <unit> · A=GO · SELECT=CHANGE UNIT* prompt. The campaign map and world maps show *SELECT: CHANGE UNIT* in the bottom bar, and KC's boot camp mentions it.
* **Fix:** artillery call-ins (Paladin, HIMARS/MLRS, TOS-1A…) now land inside the visible screen, even near map edges.

## What's new in v1.2
* **Bot Difficulty** (Settings): Recruit / Regular / Veteran / Elite. It sets enemy aim jitter, reaction/think time, fire delay, engagement range, aggression (grenades, missiles, torpedoes), bot HP, and drone SAM/AAA strength. It applies to 6v6 enemy bots (allied bots stay Regular), campaign enemies and Drone Ops air defences. The old Easy/Normal/Hard option stays as **Damage Taken** (how hard *you* get hit).
* **Land vehicles: 10 → 105.** 55 WW2 (39 production + 16 experimental), 6 Cold War, 40 Modern, 2 Future and 2 Space. Vehicle classes now behave differently: light/medium/heavy tanks; casemate tank destroyers and SPGs with a fixed gun (the hull aims, and the SPG fires HE splash); IFVs with an autocannon, homing ATGMs (B) and 2-man dismounts (SELECT); APC/halftracks with an HMG and dismounts; armoured cars and jeeps (fast, MG); Goliath (tracked mine that detonates on contact); M4 Calliope (rocket salvo). M2A3 Bradley: 25 mm Bushmaster, TOW (B), dismount rifle + AT team.
  * **+34 modern vehicles:** M1A2 SEPv3, M10 Booker, Stryker Dragoon, LAV-25, AAV/ACV, JLTV (TOW), Humvee (M2), MaxxPro MRAP, M109A7 Paladin, M142 HIMARS, M270 MLRS, AMPV, T-14 Armata, T-80BVM, T-72B3, BMP-3, BTR-82A, 2S19 Msta-S, TOS-1A, Pantsir-S1, Leopard 2A8, Challenger 3, Leclerc, Merkava Mk4, Namer, K2 Black Panther, K21, Type 99A, ZTZ-15, ZBD-04, CV9035, Puma, Boxer and Patria AMV.
  * **New class behaviours:** SP howitzer (B = artillery call-in; 5 HE impacts land 2 s later), rocket artillery (A = rocket, B = area ripple strike), SPAAG (twin cannon + B = SAM), and MRAP (MG, plus TOW on JLTV).
  * **Bot vehicles:** in mixed 6v6, Cold War and Modern bots pick East or West vehicles to match their faction.
* **Aircraft: 21 → 142.** 93 WW2 (13 experimental), 17 Cold War, 28 Modern, 2 Future, 2 Space. Each has its own parametric sprite in its silhouette class (single/twin engine, jet, dive bomber, torpedo bomber, medium/heavy bomber, flying boat, flying wing, canard, twin boom, rocket interceptor and more) and its own stats. Bombers carry bombs, torpedo planes carry torpedoes, and rocket interceptors and jets have short fuel/burn.
  * **+14 Cold War:** F-100, F-104, F-105, F-111, A-4, F-8, Mirage IIIC, AH-1G Cobra, MiG-17, MiG-19, MiG-23, MiG-25, Su-7 and Mi-24 Hind.
  * **+24 Modern:** F-16C, F-15E, F-15EX, EA-18G, AV-8B, A-10C, B-1B, B-2, AC-130J, Gripen E, Mirage 2000-5, KF-21, Tejas Mk1A, AH-64E, Su-30SM, Su-34, Su-35, MiG-31, Tu-160, J-10C, J-35, JF-17, Ka-52 and Su-25.
  * **New silhouette classes:** jet fighter, strike jet, jet bomber, close support, attack helicopter (rotor sprite, no wings) and gunship.
  * **Missiles:** real homing missiles (they track their target).
  * **Bots:** in every era they fly their own side's types (Allied/Axis in WW2, West/East later).
* **Infantry arsenal: 35 → 78 primaries, 13 → 20 sidearms, 11 → 16 launchers.**
  * **WW2 primaries (+18):** M1A1 folding carbine, Johnson M1941, M1919A6, Gewehr 43, FG 42, MG34, Beretta M38, Type 99 Arisaka, Type 100, Type 96 LMG, Bren, Owen, De Lisle (integrally silenced), SVT-40, DP-28, PPS-43, Boys AT rifle, and the M2 flamethrower (short-range fire stream).
  * **WW2 sidearms and launchers:** Nagant M1895, Nambu Type 14 and Welrod (silenced); PIAT.
  * **Modern primaries (+25):** M27 IAR, M7 (XM7), M250, Mk 18, ACR, Tavor X95, Galil ACE, F2000, FAMAS, HK433, QBZ-191, AK-15, AK-308, AN-94, SR-25, M2010, CheyTac M200, AWM, AX50, KS-23, Benelli M4, AA-12, P90, UMP45 and PP-19 Bizon.
  * **Modern sidearms and launchers:** SIG M17, SIG M18, Five-seveN and HK USP; Carl Gustaf M4, NLAW, Stinger and RPG-29.
  * **WW2 bots** also carry the new Allied and Axis weapons.
* **Attachments: 12 → 49, now in 8 slots, plus an armor slot (7 options).** Each era only lists what fits it.
  * **Optics:** iron, red dot, 4x, reflex, holo, LPVO 1-6x, ACOG, thermal, NV, hybrid, and a WW2 scope mount.
  * **Muzzle:** suppressor, compact and heavy suppressors, compensator, muzzle brake, flash hider, bayonet (point-blank stab), and a rifle grenade (+2 lethals, thrown 2x farther).
  * **Underbarrel:** vertical/angled grip, bipod, handstop, M203/GP-25 (+2 lethals), and a Masterkey shotgun (takes the launcher slot).
  * **Magazine:** extended, fast, drum.
  * **Barrel:** short, long, heavy.
  * **Laser/light:** laser, IR laser (stronger with NV/thermal), flashlight (stuns close enemies), laser + light.
  * **Stock:** folding, heavy, none.
  * **Ammo:** FMJ, AP, hollow point, incendiary (hits can set fires).
  * **Armor:** plates levels I–III (+15/+30/+50 HP), ghillie suit (bots only spot you up close), NVG (+10% damage), ballistic shield (-60% frontal damage).
  * **Menus:** the Loadout and 6v6 Arsenal screens scroll to fit the new rows.
* **Equipment.**
  * **Lethals (15):** frag variants, Mills bomb, stick grenade, Gammon, semtex, cluster, impact frag, M18 claymore (proximity), C4 and satchel charge (sticky, big blast), thermite (burns an area), Molotov (fire pool), throwing knife and tomahawk (silent, single target).
  * **Tacticals (17):** flash/stun, smoke, stim/morphine, CS gas (damage over time), decoy (distracts bots), EMP (stuns in a radius), med kit (full heal), ammo crate, deployable/sandbag cover (stops bullets for 10 s), trophy system (shoots down 3 grenades or rockets), and recon drone (marks enemies for +25% damage).
* **Music: 10 → 29 original PSG tracks.** New ones:
  * **Spec Ops world themes:** DMZ, arctic, desert canyon, jungle river, island fortress, desert airbase, Vietnam, WW2 Europe and alien.
  * **Spec Ops event music:** boss battle, fortress/airship and world map themes.
  * **Other modes:** X-15/rocket ascent, World Flight cruise, naval battle, Drone Ops (tense), and KC's cheerful boot-camp theme.
  * **Jingles:** defeat and mission-clear.
  * **Where:** every mode now plays a matching theme, and all tracks are selectable in Settings.
* **KC, your boot-camp guide.** KC, the developer's own senior shepherd-mix, is drawn as a new pixel sprite with walk, sit and bark animations, a dog-tag collar and a 48x48 dialogue portrait.
  * **When she appears:** on first boot (and from the TUTORIAL menu item), KC says "Welcome, Captain!" and runs a 15-step interactive boot camp.
  * **What it covers:** move, aim/shoot, grenades, crouch/cover, USE, weapon swap, loadout, vehicles, planes, and an overview of every mode.
  * **Skip and save:** START skips it, and a "tutorial done" flag is saved to SRAM.
  * **Spec Ops cameo:** KC waits at every checkpoint flag ("WOOF! CHECKPOINT!").
* **WW2 Experimental tab** in the arsenal, tagged EXPERIMENTAL, with generic markings only.
* **6v6 Naval:** 23 real ships across three eras, in the roles Battleship / Cruiser / Destroyer / Carrier.
* **Art overhaul**, adventure-RPG style (original art):
  * Look: brighter, softer palettes; dark outlines with 3-tone cel shading on every sprite; chunky big-head soldiers.
  * Tiles: grass with tufts and flowers, stone walls in 3/4 view, cliffs with ledges, and water with animated shimmer.
  * HUD and menus: a heart health bar, gold-framed panels and menus, and speaker portraits in dialogue boxes.
  * See `shots/v12_art_before_after_*.png`.
* **WORLD FLIGHT**, a new mode: fly the X-15 around the world (details below).
* **SPEC OPS**, a new mode: a side-scrolling run-and-gun platformer (details below).

## Campaign (27 missions, WW2 → Modern + what-if)
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
| 15 | Fulda Gap | West Germany (what-if) | armour | hold the treeline until the reinforcements cross | – |
| 16 | Guns of Texas | Omaha / Pointe du Hoc, 6 Jun 1944 | naval gunnery | close the beach, shell the bluffs, clear the sky | – |
| 17 | Tora Bora | White Mountains, Dec 2001 | infantry, mortars | burn the caches, clear the ridge | – |
| 18 | Medusa | Panjwayi, Sep 2006 | infantry | cross the riverbed, strongpoints, hold the school | – |
| 19 | Moshtarak | Marjah, Feb 2010 | infantry, ROE | clear the road, secure the bazaar | no (civilians) |
| 20 | Hostomel | Antonov Airport, 24 Feb 2022 | infantry | hold the runway edge, wreck equipment | – |
| 21 | Irpin Bridge | Irpin river, Mar 2022 | infantry + civilians | hold the bank, guide families to the buses | no (civilians) |
| 22 | Old City | West Mosul, Jun 2017 | infantry, ROE | clear the alleys, evacuate, seize caches | no (civilians) |
| 23 | Lake City | Marawi, Jul 2017 | infantry | clear the market block, knock out sniper nests, hold the bridge | – |
| 24 | Kharkiv Dash | Balakliia axis, Sep 2022 | armour | break the forward line, reach the Oskil river | – |
| 25 | Last Flights | Kabul airport, Aug 2021 | infantry + civilians | escort evacuees to the gate, hold the perimeter, reach the last aircraft | no (civilians) |
| 26 | Black Sea Skies | NW Black Sea, spring 2022 | flight | protect the city, break the strike package | – |
| 27 | Iron Curtain 2035 | Suwalki Gap (fictional what-if) | armour, night | hold the corridor, stop the robot vanguard | – |

**Side choice:** 9 missions offer a second perspective with mirrored objectives (attack ↔ defend). Uniform colours are swapped, and every faction uses generic markings with no insignia. The alternate side is never offered in missions that include civilians. Missions with civilians enforce ROE: harming 2 civilians fails the mission.

**Left out:** East Berlin "Checkpoint" (needs stealth systems that aren't built) and a flamethrower.

## 6v6 (Infantry / Armor / Dogfight / Naval)
The flow is **Choose side → Arsenal → Map → match**. There's no era select: picking a WW1, Revolution or Medieval map gives both teams period kit.
* **Factions (11):** USA, UK, USSR, Germany, Japan, China, Russia, NATO, Coalition, Federation, Syndicate. Each team is coloured by its faction, and the bots fight on the other side.
* **Arsenal:** one pool across all eras.
  * 35 primaries (including the StG 44 *Vampir* night scope and the *Krummlauf* curved barrel), 13 sidearms, 11 launchers plus none (including the Fliegerfaust), 6 lethals, 4 tacticals, 71 land vehicles, 104 aircraft and 23 warships.
  * Use L/R to jump between era tabs and Left/Right to browse.
  * Attachments and perk apply to the primary.
* **Bots:** balanced, mirrored kits. Bot *k* on each team draws gear from the same random era. On a WW2 aircraft roll, Axis-style factions fly Axis types and the others fly Allied types.
* **Maps:** **72**, all selectable regardless of era, plus RANDOM. A map's era sets its terrain and soldier look; L/R skip by era.
  * 60 land/air maps: 20 per mode, 4 per era per mode.
  * 12 naval maps: Midway Atoll, Ironbottom Sound, Denmark Strait, Leyte Gulf, GIUK Gap, Norwegian Sea, Blockade Line, Barents Night, Coral Archipelago, Talvara Strait, Task Force Ocean and Storm Front.
* **Naval ships (23):**
  * WW2: Iowa, Yamato, Bismarck, Hood, Baltimore, Takao, Fletcher, Kagero, Enterprise CV-6, Akagi.
  * Cold War: Iowa (1984 refit), Kirov, Sverdlov, Spruance, Forrestal, Kiev.
  * Modern: Arleigh Burke, Ticonderoga, Type 055, Nimitz, Gerald R. Ford, Queen Elizabeth, Fujian.
* **Naval controls:**
  * Up/Down sets the engine telegraph (astern / stop / slow / standard / flank) and Left/Right steers. Turning is slow and depends on speed.
  * L/R train the turrets. A fires a main-gun salvo: battleships fire 3 HE shells, cruisers 2, destroyers 1.
  * B fires the secondary: destroyer torpedo spreads, AA guns, anti-ship missiles on modern ships, or a carrier air-wing launch (up to 3 AI aircraft from the ship's era).
  * Every ship has automatic close-in AA.

## World Flight — Flight Around the World (free roam, no combat)
* **The world:**
  * A wrap-around map of 512×256 cells (8192×4096 px), built from a real land mask and drawn in the adventure style: oceans, shallows, beaches, grass, forest, jungle, savanna, desert, mountains, tundra, snow, sea ice and cities.
  * It's rendered on an affine BG, so the view **zooms out as you climb**.
  * Altitude bands: **LOW** (clouds, ground shadow), **HIGH** (haze, wider view) and **NEAR SPACE** (darker sky, stars and a curved horizon around the screen edge).
* **The X-15:**
  * Air-launched from a B-52 mothership at 45,000 ft. The XLR99 burns for about 45 s at full throttle, then you glide.
  * Physics: speed comes from thrust, drag that falls with altitude, and gravity along the flight path. You can reach about Mach 6–7 above 100,000 ft and roughly 350,000 ft on a steep climb.
  * Above about 130,000 ft the jet is ballistic and steers with RCS.
  * Above 264,000 ft you earn **astronaut wings**, which are saved.
* **Refuel / relaunch:** two options.
  * Glide down and land on any of **24 airfields** (Edwards AFB, Anchorage, Honolulu, Goose Bay, Keflavik, RAF Fairford, Cairo, Dubai, Delhi, Diego Garcia, Singapore, Guam, Tokyo, Darwin, Sydney, Nairobi, Cape Town, Rio, Lima, Mexico City, Cape Canaveral, Moscow, Beijing, McMurdo). Touch down slower than M0.42 with the nose not high. The B-52 then relaunches you.
  * Re-mate with the orbiting B-52 near 45,000 ft below Mach 1.4.
  * If you crash, you are recovered to the nearest field.
* **HUD:** Mach, altitude, band, throttle, LOX/fuel, heading, pitch, and a nav arrow with the distance in miles to the next unlogged landmark (or the nearest airfield once you're out of fuel).
* **Discovery:** fly below 120,000 ft within range of a landmark to log it. There are **19 landmarks**: Statue of Liberty, Golden Gate, Grand Canyon, Eiffel Tower, Big Ben, Pyramids of Giza, Taj Mahal, Great Wall, Mt Fuji, Sydney Opera House, Christ the Redeemer, Kremlin, Kilimanjaro, Antarctica (McMurdo), Everest, Colosseum, Machu Picchu, Uluru and Niagara Falls. Each has a pixel icon and a fact card, and the Landmark Log keeps them.
* **Challenges (timed, best times saved):**
  * Three ring courses: *Mojave Gates* (descending low gates), *Edge of Space* (climb gates up to 290,000 ft) and *Transatlantic* (New York → London).
  * A full **circumnavigation**: one complete lap of longitude, finishing back over Edwards.
* **Other aircraft:**
  * **P-51D Mustang:** unlocked after logging 3 landmarks.
  * **SR-71 Blackbird:** unlocked by finishing any ring course.
  * **Concorde:** unlocked after logging 10 landmarks or completing a circumnavigation.
  * These are jets/props: they take off from runways and have altitude ceilings.
* **Cheat:** L+R+SELECT on the World Flight menu unlocks all four aircraft.
* **Controls:**
  * D-pad: Up/Down pitch, Left/Right turn.
  * A/B: throttle up/down (A also drops from the B-52 and takes off).
  * L: speed brake.
  * SELECT: world map.
  * START: pause (resume / world map / exit).
* **Saving:** progress (logged landmarks, best times, max altitude, wings and unlocks) lives in its own SRAM block at 0x400.

## Spec Ops (side-scrolling run-and-gun platformer)
* **Setup:** fictional missions, generic unit badges (no official insignia), no real leaders and no civilian targets.
* **Map and saves:** an SMB3/SMW-style world map with difficulty stars on every node. Stars and completion are saved per course in SRAM.
* **Controls:**
  * Movement: D-pad to move/climb/crouch; A to jump (hold for higher); R to sprint where your unit has it.
  * Combat: B to shoot; L to throw a grenade; R for your unit's special ability.
  * Menus: START to pause. Entering Spec Ops opens unit select. SELECT on the campaign map, a world map or the deploy prompt changes unit.
  * Course clear: there's no stomping; you finish a course at its flag.
* **Pickups:** intel (3 per course), dog tags (100 = 1-up), ammo, medkits (hearts), armor, rapid fire, jetpack/parachute, and plasma rifle/coilgun in World 10.
* **Obstacles (SMB3-style, military reskins):** moving and falling platforms, conveyors, spinning flame/laser bars, shell cannons, vents and pipes to secret rooms, hydraulic crushers, spikes/wire, fuel pits, rising water, auto-scroll river boats and bomber decks, crumbling bridges, jump pads, P-switch-style timed switches, ice, wind, low/zero gravity, and hidden blocks.
* **25 playable units from 8 nations, each with a look, weapon and ability. All are unlocked from the start:**
  * **USA:** 101st, 82nd, PJ, CCT, 75th Ranger, Marine Recon, MARSOC, DEVGRU, CIA SAD and LRRP.
  * **UK:** SAS and RM Commando.
  * **Others:** Ukraine SOF; JGSDF SF; KSK; Mexico's GAFE and SEMAR.
  * **Poland:** **JW GROM** (fast-rope insertion; R = breach charge), **JW FORMOZA** (combat diver: fast swim with no air limit, limpet mine, rapid fire from boats), **JW KOMANDOS** (paradrop start; R = chute or demolition charge) and **JW AGAT** (LMG; R = plant a heavy MG for 2x fire rate).
  * **Israel (new in v1.3):** **Sayeret Matkal** (Tavor X95 carbine; R = stealth recon, enemies can't see you and your shots do 2x damage), **Shayetet 13** (naval commando: swims with no air limit; R = DPV dash underwater or a limpet mine on land), **Unit 669** (airborne rescue: paradrop start, 8 lives instead of 5; R = chute in the air or a +1 casevac heal on the ground) and **Mossad operative** (suppressed pistol; R = disguise in enemy colours so enemies ignore you and contact doesn't hurt, until you fire; walks through locked doors with a lockpick).
* **10 worlds, 90 courses.** Each world ends in a boss fortress, and bonus/secret courses count toward the total.

| World | Theme / enemies | Courses | 1★ | 2★ | 3★ | 4★ | 5★ |
|---|---|---|---|---|---|---|---|
| 1 Border Towns (desert / jungle) | Drug cartels (bandanas, SUVs); boss: cartel armored truck | 9 | 3 | 4 | 2 | 0 | 0 |
| 2 Desert Compounds (canyon / desert) | Proxy militias (scarves, technicals); boss: heavy technical | 9 | 1 | 3 | 4 | 1 | 0 |
| 3 The DMZ (forest / tunnels / ridges) | KPA (soft caps, olive); boss: KPA tank | 9 | 1 | 3 | 3 | 2 | 0 |
| 4 Mountain Bases | CSTO coalition (mountain flora, BTRs); boss: Mi-24 Hind | 9 | 1 | 3 | 3 | 2 | 0 |
| 5 Arctic City (arctic / urban night) | Russian forces (winter white); boss: T-90 | 9 | 0 | 3 | 2 | 4 | 0 |
| 6 Island Fortress (pacific / fortress) | Chinese PLA (blue digital); bosses: Type 99 beachhead, Z-10 carrier finale | 9 | 0 | 2 | 3 | 3 | 1 |
| 7 Mining Compound | Wagner Group (PMC); boss: commander "GRANIT" | 9 | 0 | 1 | 3 | 4 | 1 |
| 8 Cold War | NVA/VC, Warsaw Pact; bosses: AA emplacement, Mi-24 Hind | 9 | 0 | 2 | 4 | 2 | 1 |
| 9 World War II | Axis (generic grey markings, no Nazi symbols); bosses: Tiger, armored train, bunker, fortress cannon | 10 | 0 | 0 | 4 | 3 | 3 |
| 10 Space Aliens | drones, walkers, brood; bosses: Brood Queen, Mothership Core | 8 | 0 | 0 | 2 | 4 | 2 |
| **Total** | | **90** | **6** | **21** | **30** | **25** | **8** |

* **Verification:**
  * **BFS validator:** every course passes a reachability check when it's generated.
  * **Autopilot bot:** it clears 45/90 courses headless (`build/run_demo.sh`), with at least one clear in every world. The failures are bot skill, not level faults (mostly long jumps on auto-scroll decks and timing-heavy late courses).

## Space Launch
* **Menu:** pick an agency with L/R or Left/Right, then a rocket. The preview draws the actual stack: stages, boosters, fairing, escape tower or orbiter. It also shows the rocket's best score and any bonus mission. All agency colour schemes are generic, with no logos.
* **Roster (35):**
  * **NASA / USA:** Mercury-Redstone (suborbital: reach 187 km), Mercury-Atlas, Titan II Gemini, Saturn IB, Saturn V (Apollo bonus: CSM/LM docking, then a lunar landing), the Space Shuttle as Columbia, Challenger, Discovery, Atlantis and Endeavour (one fact each; no disaster is depicted), Delta II, Delta IV Heavy, Atlas V, Titan IV, Antares and SLS/Orion.
  * **SpaceX:** Falcon 1, Falcon 9 Block 5 (drone-ship booster landing), Falcon Heavy (twin side-booster landings), Starship + Super Heavy (tower catch), Crew Dragon, Cargo Dragon and Grasshopper (a 250 m hop and landing).
  * **ESA:** Ariane 5, Ariane 6, Vega.
  * **Russia / USSR:** Soyuz, Proton, Energia/Buran, N1.
  * **China:** Long March 5, Long March 2F.
  * **Japan:** H-IIA, H3, Epsilon.
* **Countdown:** T-10 with the countdown theme. Press A within about 0.2 s of T-0 for a perfect-ignition bonus; START skips to T-3.
* **Ascent:**
  * UP/DOWN sets throttle (40–100%), LEFT/RIGHT pitches, and A separates stages. Spent stages and boosters fall away on screen.
  * Each stage has its own fuel and thrust. You get a bonus for staging cleanly within 1 s of burnout and a penalty for staging early.
  * Dynamic pressure is modelled. Above 30 kPa the stress bar fills, so throttle down through max-Q. If it fills completely you get a non-destructive abort: launch escape, RTLS or range-safety abort.
  * Reach 150 km at 7.7 km/s to make orbit. A yellow tick on the attitude dial is a closed-loop guidance cue.
  * Every rocket's stage data is test-flown by `tools/gen_rockets.py`, so the build fails if any rocket can't reach its goal.
* **Booster landings:** A/UP fires the engine and LEFT/RIGHT cancels drift on limited fuel. Land softly on the pad, ship or zone, or stop between the tower's chopsticks.
* **Orbit:** RCS on the D-pad. Dodge satellites, spent stages and micrometeoroids (3 hull points), then dock below 0.75 m/s and within 3 px of the port.
* **Autopilot:** hold SELECT in any phase. It's a training aid that halves the score.

## Drone Ops (Modern only)
* **Drones (9):** MQ-9 Reaper, MQ-1C Gray Eagle, MQ-1 Predator, RQ-4 Global Hawk (recon/mark), Bayraktar TB2 (MAM-L), MQ-20 Avenger, Switchblade 600 (kamikaze), Wing Loong II, MQ-4C Triton (maritime recon, unarmed).
* **Call-ins:** artillery and AC-130.
* **Missions (6):** Highway Interdiction, SAM Belt, Town Overwatch, Coastal Watch (maritime), Score Attack and Support 6v6. All are fictional, set in generic locations.
* **Systems:** thermal overlay (WHOT/BHOT/TV), SAM/AAA threats that scale with altitude, and civilian/friendly penalties.

## Loadout menu
* **Campaign kits (by era):** weapons, 8 attachment slots, armor, lethal, tactical, perk, tank and aircraft per era.
* **6v6 Arsenal (all eras).**
* **Character creator:**
  * Name, typed on an on-screen keyboard (up to 10 characters).
  * Look: 6 skin tones, 5 hair colours, 3 face styles.
  * Kit: headgear (helmet/beret/cap/boonie/none), 8 uniform colours, 5 camo patterns, gear (none/webbing/plate carrier/radio pack), standard/slim build.
  * Live preview; use it in game ON/OFF.
  * Your soldier is the player sprite in 6v6 infantry and all on-foot campaign missions, including Dog Green.

## Settings (saved)
* **Music:** AUTO (era/mode default), any of the 29 original PSG tracks, or OFF.
* **Tracks:** Bugle Call, Dog Green, Hedgerow March, Swing Squadron, Iron Curtain, Task Force, Neon Protocol, Void Drift, Arsenal Theme, Victory, DMZ Patrol, Permafrost, Dune Raiders, Jungle River, Island Fortress, Sand Airstrip, Paddy Fields, Old Continent, Xeno Signal, Boss Battle, Fortress, World Map, Ascent, Cruise, Broadside, Loiter, KC's Boot Camp, Defeat, Mission Clear.
* **Other options:** music volume, SFX volume, screen shake, Damage Taken (Low/Normal/High), **Bot Difficulty (Recruit/Regular/Veteran/Elite)** and control hints.
* **Vibration:** not available on GBA.
* **Sequencer:** VBlank-IRQ driven.

## Controls
| | Infantry / Campaign | Tanks | Air | Drone |
|---|---|---|---|---|
| D-pad | move | throttle / hull | turn / throttle | fly reticle |
| (Naval) | | Up/Down telegraph, Left/Right rudder; A main guns, B secondary/air wing, L/R turrets | | |
| A | fire | main gun | guns | fire weapon |
| B | lethal | coax MG / ATGM (IFV) / rockets | rockets / missiles / bombs / torpedo | hold: scan / mark |
| L | swap weapon (primary/sidearm/launcher) | turret left | roll | cycle weapon |
| R | strafe | turret right | roll | slow |
| SELECT | tactical | centre turret / dismount squad (IFV, APC) | scoreboard | thermal mode |
| B (armor, new classes) | | artillery call-in (SPG) / ripple (MLRS) / SAM (SPAAG) / TOW (JLTV) | | |
| START | pause | pause | pause | pause |

Objective arrows and brackets point to the current target, and the radar shows objective markers.

## Build / test
* `make` builds the ROM. It needs `gcc-arm-none-eabi` and `python3-pil`, and is bare metal with its own crt0 and linker script.
* `tools/gen_assets.py` draws every sprite, tile, palette and the music data procedurally.
* Headless tests: `make tools/harness` (needs libmgba). Then run `tools/harness ROM script outdir [save]` and `python3 tools/topng.py outdir`.
* Screenshots: v1.3 in `shots/v13_*.png` (unit select, factions, Israeli units, Space Launch). v1.2 in `shots/v12_*.png` (3x; Spec Ops `v12_platformer_*`, `v12_specops_*`; KC `v12_kc_*`; gear `v12_gear_*`; vehicles `v12_armor_*`), v1.1 in `shots/v11_*.png`, and music WAVs in `shots/v11_music_*.wav`.
* `tools/gen_vehicles.py` holds every land vehicle, aircraft and ship (stats plus parametric sprite specs) and emits `src/vehdb.inc`.
* `tools/gen_platform.py`, `gen_courses.py` and `gen_levels.py` build the Spec Ops units, themes, bosses and the 90 RLE-compressed courses (`src/platdb.inc`, `src/pflevels.inc`). `tools/gen_kc.py` draws KC. `tools/gen_rockets.py` builds and test-flies the Space Launch roster (`src/rocketdb.inc`).
* Regression: `tools/run_v12_regress.sh` (it auto-skips the first-boot tutorial via `tools/kcskip.py`).
* `tools/gen_world.py` builds the World Flight map, tiles and landmark data. Its land mask is cached in `tools/world_mask.bin`; it was originally built with the `global-land-mask` Python package.
