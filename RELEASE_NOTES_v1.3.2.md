# Arsenal of Eras GBA — v1.3.2

Saves from v1.3.1 carry over automatically. The old SRAM blocks (0x0000–0x1FFF) are left alone, and each new mode keeps its own checksummed block from 0x2200 up.

## New on the main menu
The menu now has 13 categories and scrolls, showing 8 rows at a time with ^/v arrows. The new entries are **RACING, HANGAR, ARCADE, FIREFIGHTER** (tagged NEW) and **CREDITS**.

### FIREFIGHTER (dedicated to my Dad, a real hero)
- **Dedication card on entry:** "Dedicated to my Dad, a real hero. - LT".
- **Station hub:** Fire Station 1 with ENGINE 1, LADDER 1 and MEDIC 1 in the bays and a firefighter in full turnout gear. It has five options:
  - Respond to a call
  - Gear locker (helmet, hood, turnout coat, SCBA, PASS, irons, thermal camera, radio)
  - Fire safety tips
  - Records
  - Back
- **Each call runs:** dispatch card → auto-drive with lights and siren (A sounds the air horn) → the scene, played top-down.
- **On scene:**
  - Pull the hose (the trail follows you) and set the pump to 100, 150 or 200 PSI with L. At 200 PSI the hose kicks.
  - The tank water runs out unless you hook up to the hydrant (B).
  - Aim at the base of the fire. B is also the Halligan for forcing locked doors, and it vents the roof from the ladder point.
  - R turns on the thermal camera, which shows people through smoke.
  - Carry people and pets outside to MEDIC 1.
  - Watch the SCBA air gauge: the low-air alarm sounds at 33%, and you swap bottles at the engine.
  - Fire spreads cell by cell along the fuel, and smoke fills rooms and darkens what you can see.
  - Flashover warnings come before a flashover; you bail out to the nearest exit. Forcing a smoke-filled room before venting causes a backdraft.
- **Rescues are heroic and non-graphic:**
  - The people you're looking for never get hurt.
  - Running out of air is a "MAYDAY": your crew brings you out safe.
- **10 calls:**
  1. Kitchen fire
  2. Bedroom fire (locked door, vent)
  3. Car fire on Route 9
  4. Apartment ladder rescue (ladder truck minigame)
  5. Brush fire (hand line + firebreak)
  6. Highway crash (Jaws of Life extrication: stabilize, cut A and B pillars, spread the door, backboard)
  7. Find Scout (a family dog who looks like KC; heavy smoke and thermal camera)
  8. Strip mall fire (flashover)
  9. Wildland-urban interface
  10. Warehouse fire (finale)
- **Ratings and rewards:** 3 stars for people saved, time and property saved. There's a fire-safety tip card after every call, a top-5 records table with initials, Mars Rocks, and an original march, "ENGINE ONE".

### ROCKET INVADERS (ARCADE)
- An original fixed shooter. Your ship is a Falcon 9, a Falcon Heavy (3-way spread), a Starship (3 hits per life) or a Dragon (fast).
- **Enemies:**
  - Alien waves that speed up as they thin out
  - Falling debris
  - A UFO mothership bonus
  - A Hive Queen boss every 5 waves
- **Shields:** erodible launch towers and Starlink barriers.
- **Power-ups:** Raptor rapid fire, Mechazilla catch (+1 life), Starlink laser.
- **Scores and music:** a top-5 high-score table in SRAM with initials entry and a share code, plus the "ORBIT DEFENDERS" chiptune.

### RACING (Mode 7, Tesla EVs, no logos)
- **Cars:** Roadster (with a SpaceX thruster package), Model S Plaid, 3, X, Y, Cybertruck, Semi and Cybercab.
- **Driving:** battery boost and regen, plus Supercharger pads.
- **4 tracks:** Giga Texas, Pacific Coast Highway, Starbase Boca Chica, and Mars at 0.38 g.
- **Modes:** Grand Prix, Time Trial and Free Drive. Lap records are saved.

### HANGAR — Aircraft Builder
- Build a plane, drone or helicopter from body, wing, engine, weapon, armor and paint parts, with live stats and a sprite preview.
- Save to 3 slots, then test-fly the design in a range or take it into 6v6.

### VON BRAUN MISSIONS (EXPLORE)
- **Missions:**
  - Falcon 9 droneship landing
  - Super Heavy Mechazilla catch
  - Artemis HLS lunar landing
  - Mars rover sample run
  - Orbital refueling docking
- **Wernher von Braun as guide:** he gives the briefings and fact cards.

### SANDBOX (BATTLES)
- Custom mixed-era 6v6 battles. You choose:
  - Map
  - Era for each side (knights vs tanks is allowed)
  - Unit counts and bot skill
  - Win condition (TDM, time, last standing, score)
- Save up to 3 presets.

### CREDITS
- Reachable from the main menu and after you finish the campaign.
- **Cards:**
  - Created by LT Vasquez
  - Built with KC (Grok Bot)
  - KC the dog
  - Special thanks: Dad, plus the firefighter dedication
  - "Thank you Elon Musk for being the biggest inspiration for me. - LT" (text only)
  - A Morse tip
- **Two quiet memorial cards with slow fades:**
  - 9/11 tribute (Tribute in Light beams over a dark skyline): "This game honors the bravery of firefighters, military men and women, and the people who lost their lives in the 9/11 attacks."
  - Benghazi memorial (folded flag): "And in honor of the fallen heroes of Benghazi, September 11-12, 2012: Ambassador J. Christopher Stevens, Sean Smith, Glen Doherty and Tyrone Woods."

### Other changes
- V-2 altitude is corrected to about 176 km.
- **Mars Rocks wallet:** a saved balance. FIREFIGHTER pays out Mars Rocks today; the store is not built yet.

## Testing (headless mGBA autopilot)
- **FIREFIGHTER:** all 10 calls were played to the end and all succeeded, ending back at the station hub with stars saved. The ladder rescue and the Jaws of Life minigames were played through. A failure run (no input) ends in a MAYDAY after the air runs out. Each run was followed by its result screen and tip card.
- **ROCKET INVADERS:**
  - All 4 rockets were tested; the best run reached wave 8.
  - The wave 5 boss was beaten by Falcon 9 and Dragon. Falcon Heavy and Starship lost all lives at the boss.
  - Power-ups and UFOs were collected.
  - Tested: game over, the initials entry and the share code. A high score was saved to SRAM, the game was rebooted, and the score was still on the table.
- **Racing, hangar, Von Braun Missions, sandbox and credits:** win and fail runs for each.
- **Regression:** all 219 scripted runs were repeated.
  - The 127 v1.3-era scripts behave the same as v1.3.1.
  - 3 scripts that wrapped around the old 8-row menu were updated for the 13-row menu.
  - The Bradley and settings scripts that drift from the v1.3 baseline drift the same way on v1.3.1 too.
- **SRAM migration:** a v1.3.1 save was loaded. Campaign progress is intact, nothing below 0x2000 changed, and the new Firefighter and Mars Rocks blocks were added.

## Known limitations
- Rescue scenes use a tile grid with cell-based fire, smoke and heat. It's a simplified model, not a fluid simulation.
- The Mars Rocks store and achievements are not in this build yet; the balance is saved. FIREFIGHTER's leaderboard is its own top-5 table.
- Custom HANGAR aircraft use the plane flight model; helis and drones hover with DOWN.
