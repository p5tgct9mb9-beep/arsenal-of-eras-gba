# Arsenal of Eras DS: changelog

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
