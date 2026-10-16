# Arsenal of Eras GBA — v1.3.3 (checkpoint)

Saves from v1.3.1 and v1.3.2 carry over. The only new SRAM is a 16-byte "seen" block at 0x26C0 (used for NEW tags), and race difficulty is stored in a byte that used to be unused padding in the racing record.

## Priority fixes
- **Racing AI is competitive now.** Rivals follow real racing lines (they go wide, clip the apex and use the full road), use boost on straights and to overtake, and react to the pack with a continuous rubber band.
  - Their pace scales toward the car you pick, so choosing the fastest car no longer turns the cup into a walkover.
  - New **EASY / NORMAL / HARD** select. Grand Prix defaults to **NORMAL** and your choice is saved.
  - Measured against a "strong human" reference driver (the same autopilot in every run, 4 races per setting; 60 frames = 1 s):

    | Setting | Result |
    |---|---|
    | v1.3.2 | reference won every race by about 6 s |
    | v1.3.3 EASY | reference wins by 11–14 s |
    | v1.3.3 NORMAL | 3 wins (by 0.1–1.5 s), 1 loss (by 0.3 s) |
    | v1.3.3 HARD | AI wins every race by 3.3–5.8 s |

- **Rocket Invaders balance.**
  - Falcon Heavy is now a triple-shot ship (all 3 cores fire).
  - Starship takes 4 hits per life and has a tighter hitbox.
  - Seeded autopilot, 12 runs per ship, beating the wave-5 boss: Falcon 9 7/12, Falcon Heavy 10/12, Starship 8/12, Dragon 11/12. In v1.3.2, Falcon Heavy died at the wave-5 boss.
- **Hangar hover flight model.** Custom helicopters and multirotor drones now really hover (fixed-wing drones and planes are unchanged):
  - UP flies forward and DOWN backs up slowly. LEFT/RIGHT yaw, and L/R strafe.
  - SELECT+UP/DOWN climbs or descends (0–60 m). The HUD shows ALT and HOVER/LOW, and the shadow distance follows your altitude.
  - Release the stick and the craft brakes to a hover.
- Reran the Bradley autopilot batch: all 28 runs reached mission end. The Sandbox preset-1 run also completed with no stalls.

## New main menu
- The Earth-from-orbit scene (ISS, launches, KC) stays. The cluttered 13-row list is replaced by **10 big tiles**, each with an icon, a label and a tag line: CAMPAIGN, BATTLES, SPEC OPS, AIR, SPACE, ARCADE, LEARN, CREATE, STORE, EXTRAS.
  - The bottom bar gives a one-line description and what's inside. It also flags NEW content.
  - The STORE tile shows your Mars Rocks balance.
  - The highlight glides between tiles, and the key-press queue fix is kept.
- **Each tile opens a clean submenu.** Every row has an icon, and the cursor is a gliding highlight bar.
  - NEW tags stay until you open the mode once.
  - FIREFIGHTER is pinned at the top of SPEC OPS with a FEATURED badge.
  - The header shows your Mars Rocks balance, and the right panel holds the same per-mode options as before.
- **Where things went:**

  | Tile | Contents |
  |---|---|
  | BATTLES | 6v6 ×4, Sword, FPS |
  | SPEC OPS | Firefighter, Spec Ops, Bradley, Sniper, EOD, LRRP |
  | AIR | Dogfight, Anti-Air, Drones, World Flight, Classified |
  | SPACE | Launch, Space Program, Astronaut, Von Braun, Moon, Mars, Orbital Defense, Asteroids |
  | ARCADE | Invaders, Grand Prix, Time Trial, Free Drive |
  | LEARN | Boot Camp (KC), History Tour, What-If 1945 |
  | CREATE | Hangar, Rocket Builder, Sandbox, Loadout |
  | EXTRAS | Secret Codes, Credits, Settings |

  The secret code entry for CLASSIFIED still works from AIR, and it now also has its own entry in EXTRAS.
- STORE currently shows your wallet and how to earn rocks. The item catalog is the next checkpoint.

## Credits
- New THANK YOU cards come just before the memorial cards:
  1. "Thank you to all active-duty and veteran members of the armed forces."
  2. "Thank you to the U.S. Army, U.S. Navy, U.S. Air Force, U.S. Marine Corps and U.S. Space Force."
  3. "Thank you SpaceX, Tesla and Anduril Industries for the inspiration."
- The cards are text only, with no seals or logos.
- A closing note follows "Thanks for playing": "Arsenal of Eras is a fan-made personal project and is not affiliated with or endorsed by any company or military branch mentioned."

## Testing
- **Regression scripts.** 206 of the 220 regression scripts were machine-translated to the new navigation (tools/menu133_xlate.py), and each one was checked to end on the same screen as its legacy-menu run.
  - The rest (multi-boot and multi-visit scripts) run with a legacy-menu test flag (tools/kcskip.py), so they keep using the v1.3.2 menu.
  - Against the v1.3.2 ROM, all 220 scripts end on the same screen, and 177 end on a bit-identical state. The other 43 are 6v6 / mission matches whose random scores shift with frame timing.
  - This includes the v1.0 / v1.1 / v1.2 save-migration scripts.
- **New tests:**
  - tools/t133/menu_tour.txt opens every tile and submenu and returns from each.
  - Plus: race difficulty select, the race difficulty batches, the seeded Invaders batches, and hover tests.
- Screenshots are in `shots/v133/`, including `menu_before_after.png`.

## Known limitations
- The Mars Rocks store catalog, achievements, leaderboards and the new modes are still in progress, so they aren't in this build.
