v1.3.2 -> v1.3.4 save migration test (tools/mig132.sh + tools/t134/mig_load.txt)
1. Seven v1.3.2 scripts (Invaders w/ initials, Firefighter call, GP race, Time Trial, Hangar, Sandbox UI, VBM) were played
   on the v1.3.2 ROM (build/v132.gba), each from a fresh boot; their SRAM sections were merged into one v1.3.2 save:
   K (KC boot camp) 0x600, R (race records) 0x2200, H (hangar) 0x2300, I (invaders table) 0x2480, F (firefighter) 0x2500,
   M (Mars Rocks wallet, 65 rocks) 0x2600.
2. That save was loaded on v1.3.4: boot -> main menu (wallet chip shows 65 ROCKS) -> Arcade submenu -> Invaders ->
   Arcade Cabinet -> one Starship Hopper game (autopilot) -> back to the cabinet.
3. Result: K, R, H, I, F sections byte-identical to the v1.3.2 save after boot AND after playing; M changed only by the
   payout (65 -> 69 rocks, earned total 65 -> 69); new sections N (NEW-tag seen bits) and Z (arcade records, 12 tables +
   play counts) were created. 19 bytes changed below 0x3400, all inside M (0x2600) and N (0x26C0).
