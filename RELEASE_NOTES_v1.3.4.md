# Arsenal of Eras GBA — v1.3.4 (checkpoint)

Saves from v1.3.1 through v1.3.3 carry over. A dedicated migration test from a v1.3.2 save is described below. The only new SRAM is the arcade record at 0x3400 ('Z'), which holds high scores, initials and play counts.

## Urgent fix: main-menu blinking
- **Cause:** every frame, the ISS, satellites and rockets were erased first and only redrawn after the heavy Earth/tile work. While they were missing, the screen beam scanned past them, so they blinked. The exhaust trail was only sampled every 4 frames, which made it look dashed.
- **Fix:** these objects are now hardware OBJ sprites. They are pre-rendered once and their positions are uploaded during VBlank, so they can't be caught half-drawn. The exhaust trail now places a puff every ~2 px and looks continuous.
- **Verified** over 600 consecutive frames:

  | | ISS blink/partial frames | Rocket |
  |---|---|---|
  | Before | 33 | dashed trail |
  | After | 0 of 600 | 0 of 216 frames |

- Before/after evidence: `shots/v134/menu_flicker_before_after.gif` and `menu_flicker_contact_sheet.png`, plus the CSVs.

## New: ARCADE CABINET (Arcade → CABINET)
- 12 original arcade games, each with its own soundtrack. The cabinet shows a live attract preview, a description, the high score and the play count for each game.
- Game over shows your score and **+N Mars Rocks**. A new best lets you enter initials, and you get a top-5 board plus a share code.
- All games run on a fixed 60 Hz step.

The 12 games:
1. **Iron Dome.** Missile defense.
2. **Debris Sweeper.** Asteroids-style, flying a Dragon capsule.
3. **KC Crossing.** Get KC the dog across roads, a river and mines.
4. **Bunker Maze.** Maze chase.
5. **Bunker Buster.** Breakout.
6. **Cargo Stack.** Falling blocks inside a C-17 bay.
7. **Convoy.** Snake-style truck convoy.
8. **Chopper Rescue.** Fly a Black Hawk and bring hostages home. Hitting a hostage costs points.
9. **Starship Hopper.** Fly between towers.
10. **Tank Pong.**
11. **Tank Duel.** 1v1 in a maze with ricochets.
12. **Space Pinball.** Ramps, Mars multiball and a ball saver.

## Testing
- **Arcade autopilot:** all 12 games were played to a natural game over, with high scores, plays and payouts confirmed in SRAM.

  | Game | Score |
  |---|---|
  | Iron Dome | 14840 |
  | Debris Sweeper | 11300 |
  | KC Crossing | 1177 |
  | Bunker Maze | 14820 |
  | Bunker Buster | 3820 |
  | Cargo Stack | 526280 |
  | Convoy | 3100 |
  | Chopper Rescue | 0 |
  | Starship Hopper | 11 |
  | Tank Pong | 2400 |
  | Tank Duel | 1300 |
  | Space Pinball | 9000 |

  The sheets are `shots/v134/arcade_gameplay_sheet.png` and `cabinet_all12_sheet.png`.
- **Full regression:** all 220 scripts reached the same end screen as v1.3.3. 166 logs were byte-identical. The rest differ only in main-menu timing and pixels, because the menu now uses sprites.
- **v1.3.2 save migration:** the K, R, H, I and F save sections were byte-identical after booting v1.3.4 and after playing. The wallet went 65 → 69 after an arcade game. See `shots/v134/migration/`.

## Known limitations
- The autopilot is weak in a few games (KC Crossing, Chopper Rescue, Starship Hopper), so those low scores show the bot's skill rather than the difficulty. The games themselves are fully playable.
- The 147 achievements are defined but not active yet. They arrive with the achievements/profile update.
