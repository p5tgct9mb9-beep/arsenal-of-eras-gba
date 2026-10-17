#!/usr/bin/env python3
"""veh_hd.py - ds-v0.3.0 top-down armour sprites, rebuilt from real dimensions.

Every real tank / SPG / IFV / armoured car in src/vehdb.inc gets a spec (real hull length and width in metres,
track width, turret shape / size / position, gun length and calibre, muzzle brake, fume extractor, skirts,
fenders, ERA, bustle, stowage ...).  The spec is drawn as a layered model at 4x resolution and reduced to the
32x32 4bpp hull + turret sprite pair the game rotates in hardware:
  hull   - track runs with links and rounded sprocket / idler ends (or tyres for wheeled vehicles), fenders and
           side skirts, a lit glacis with the driver hatch, engine deck with grille louvres and exhausts,
           headlights, tools, spare track links, fuel drums, team marking
  turret - cast domes (Sherman, T-34/85, IS-2, T-55/62/72/90 ...), welded boxes (Tiger, KV-1, Churchill ...),
           flat angular composite turrets (Abrams, Leopard 2, Challenger, K2 ...), open-top rings (M10/M18/M36),
           mantlet, commander's cupola with vision blocks, loader hatch, bustle racks, smoke dischargers,
           ERA bricks, roof machine guns, and a round barrel with muzzle brake / fume extractor
Scale: each vehicle keeps the hull length of its GBA sprite (gameplay size), its width comes from the real
length:width ratio.  The turret ring sits on the sprite centre (16,16), the rotation centre of both sprites.
Lighting is overhead (rotation invariant: the sprites rotate in hardware).  Colours stay on the 16-colour team
palette ramps that are consistent in every era: camo 2<3<4, metal 12<13<14<11, dark 7/1, team 9, wood 8,
glass 10, so team colours, hangar and loadout palettes keep working.
Vehicles without a spec (fantasy, sci-fi, medieval, field guns) fall back to veh_ds-style re-shading."""
import numpy as np
from PIL import Image, ImageDraw
S = 4; N = 32 * S
CAMO, METAL, DARK, TEAM, GLASS, WOOD, GOLD = 1, 2, 3, 4, 5, 6, 7
def _dist(mask, cap):
    d = np.where(mask, cap, 0).astype(np.int32)
    for _ in range(cap):
        p = np.pad(d, 1); nb = np.minimum.reduce([p[:-2, 1:-1], p[2:, 1:-1], p[1:-1, :-2], p[1:-1, 2:]]) + 1
        d = np.where(mask, np.minimum(d, nb), 0)
    return d
class Model:
    """4x-resolution layered model: material, paint tone and a height field.  Shading = paint tone + lambert from
    the height-field normals (light from the front-left, as in the approved side-view art) - cavity occlusion."""
    LIGHT = np.array([-0.45, -0.55, 0.70]) / np.linalg.norm([-0.45, -0.55, 0.70])
    def __init__(s): s.mat = np.zeros((N, N), np.uint8); s.tone = np.zeros((N, N)); s.h = np.zeros((N, N)); s.tag = np.zeros((N, N), np.uint8)
    def _mask(s, fn):
        im = Image.new('L', (N, N), 0); fn(ImageDraw.Draw(im)); return np.array(im) > 127
    def put(s, mask, mat, tone, bevel=0.0, dome=0.0, edge=0.9, dz=None, cyl=0, tag=0):
        """bevel: chamfered edge (height ramps up over ~1 px), dome: rounded top, cyl: 1 = tube along y, 2 = along x,
        dz: height added on top of what is underneath (default: small for paint, bigger for bevelled parts)"""
        if not mask.any(): return
        if dz is None: dz = 2.0 * S * (bevel > 0 or dome > 0) + (0.25 * S if cyl else 0)
        base = s.h[mask].max() if dz > 0 else 0
        d = _dist(mask, int(S * 6)).astype(float)
        prof = np.ones((N, N))
        if bevel: prof = np.minimum(1.0, d / (S * (0.9 + 2.5 * bevel)))
        if dome:
            mx = max(1.0, d.max()); r = 1 - d / mx; prof = np.sqrt(np.clip(1 - r * r, 0, 1)) * dome + prof * (1 - dome)
        if cyl:
            ax = 1 if cyl == 1 else 0; ys, xs = np.nonzero(mask)
            # half-cylinder across the tube: use distance to the edge across its width
            dd = d.copy(); mx = max(1.0, dd.max()); prof = np.sqrt(np.clip(1 - (1 - dd / mx) ** 2, 0, 1))
        t = np.full((N, N), float(tone))
        if bevel or dome: t = np.where(d <= S * edge, t - bevel * 0.35, t)
        hh = (base if dz > 0 else s.h) + dz * prof
        s.mat[mask] = mat; s.tone[mask] = t[mask]; s.h[mask] = hh[mask] if np.ndim(hh) else hh; s.tag[mask] = tag
    def rect(s, x0, y0, x1, y1, mat, tone, **k):
        if x1 - x0 < 0.3 or y1 - y0 < 0.3: return
        s.put(s._mask(lambda d: d.rectangle([x0 * S, y0 * S, x1 * S - 1, y1 * S - 1], fill=255)), mat, tone, **k)
    def rrect(s, x0, y0, x1, y1, r, mat, tone, **k):
        if x1 - x0 < 0.3 or y1 - y0 < 0.3: return
        r = min(r, (x1 - x0) / 2, (y1 - y0) / 2)
        s.put(s._mask(lambda d: d.rounded_rectangle([x0 * S, y0 * S, x1 * S - 1, y1 * S - 1], radius=r * S, fill=255)), mat, tone, **k)
    def ell(s, x0, y0, x1, y1, mat, tone, **k):
        if x1 - x0 < 0.3 or y1 - y0 < 0.3: return
        s.put(s._mask(lambda d: d.ellipse([x0 * S, y0 * S, x1 * S - 1, y1 * S - 1], fill=255)), mat, tone, **k)
    def poly(s, pts, mat, tone, **k): s.put(s._mask(lambda d: d.polygon([(x * S, y * S) for x, y in pts], fill=255)), mat, tone, **k)
    def shade(s):
        h = s.h; gy, gx = np.gradient(h); n = np.dstack([-gx, -gy, np.ones_like(h)]); n /= np.linalg.norm(n, axis=2)[..., None]
        lam = (n @ s.LIGHT) - s.LIGHT[2]
        # cavity: pixels lower than their neighbourhood (contact shadows where a part meets the deck)
        k = S * 2; p = np.pad(h, k, mode='edge'); acc = np.zeros_like(h)
        for dy in (-k, 0, k):
            for dx in (-k, 0, k): acc += p[k + dy:k + dy + N, k + dx:k + dx + N]
        cav = np.clip((acc / 9 - h) / (S * 2.0), 0, 1)
        return s.tone + 0.55 * np.clip(lam, -0.5, 0.35) - 0.22 * cav
    def reduce(s, outline=True, cov_min=0.45):
        g = np.zeros((32, 32), np.uint8); T = s.shade(); tg = np.zeros((32, 32), np.uint8)
        for y in range(32):
            for x in range(32):
                bm = s.mat[y * S:(y + 1) * S, x * S:(x + 1) * S]; bt = T[y * S:(y + 1) * S, x * S:(x + 1) * S]
                if (bm > 0).mean() < cov_min: continue
                vals, cnt = np.unique(bm[bm > 0], return_counts=True); m = int(vals[cnt.argmax()])
                g[y, x] = _pal(m, float(bt[bm == m].mean()))
                tv, tc = np.unique(s.tag[y * S:(y + 1) * S, x * S:(x + 1) * S], return_counts=True); tg[y, x] = tv[tc.argmax()]
        _post(g, tg)
        if outline:
            m = g > 0; p = np.pad(m, 1); nb = p[:-2, 1:-1] | p[2:, 1:-1] | p[1:-1, :-2] | p[1:-1, 2:]
            g[(~m) & nb] = 1
        return g
TRACK, GRILLE = 1, 2
def _post(g, tg):
    """pixel-grid patterns that a 4x render would blur: track links, grille louvres"""
    tr = tg == TRACK
    for x in range(32):
        col = np.nonzero(tr[:, x])[0]
        if not len(col): continue
        for y in col:
            outer = not (x > 0 and tr[y, x - 1] and x < 31 and tr[y, x + 1]) and not (x > 0 and tr[y, x - 1] and x > 1 and tr[y, x - 2] and not (x < 31 and tr[y, x + 1]))
            g[y, x] = 13 if (y % 3 == 1) else 12          # link rows (lit) / gaps, like the GBA tracks
    # rounded sprocket / idler ends: outer corners of each track run drop out
    for x in range(32):
        col = np.nonzero(tr[:, x])[0]
        if not len(col): continue
        side = not (x > 0 and tr[col[0], x - 1]) or not (x < 31 and tr[col[0], x + 1])
        if side and len(col) > 6: g[col[0], x] = 0; g[col[-1], x] = 0
    gr = tg == GRILLE
    for y in range(32):
        for x in range(32):
            if gr[y, x]: g[y, x] = 2 if y % 2 == 0 else 3
def _pal(m, t):
    t = min(0.999, max(0.0, t))
    if m == CAMO: return 7 if t < 0.12 else 2 if t < 0.40 else 3 if t < 0.72 else 4
    if m == METAL: return [12, 13, 14, 11][int(t * 4)]
    if m == DARK: return 1 if t < 0.35 else 7
    if m == TEAM: return 9
    if m == GLASS: return 10
    if m == WOOD: return 8
    if m == GOLD: return 15
    return 0
# ------------------------------------------------------------------------------------------------ specs
# L,W real hull length / overall width (m); tw track width (m); tp turret ring position from the front (fraction of L)
# tur: shape c=cast dome, s=soviet saucer, b=welded box, a=flat angular composite, w=arrowhead wedge, o=open ring,
#      n=none (casemate gun in the hull), m=MG/RWS only, r=small one-man turret;  tl,tws turret length/width (m)
# gun: barrel length beyond the turret ring (m); cal: 1 thin / 2 / 3 thick; mb muzzle brake; fx fume extractor;
# flags: fend (fenders cover the tracks), skirt (side skirts), era (reactive armour), bus (turret bustle / rack),
#        smoke (dischargers), spare (spare track links on the glacis), drums (rear fuel drums), mg (roof MG),
#        open (open-top crew bay), wh=N wheels per side, half (halftrack), sponson (side gun), deck (engine deck rows)
def V(L, W, tw=0.5, tp=0.42, tur='c', tl=2.2, tws=2.0, gun=3.0, cal=2, **k):
    d = dict(L=L, W=W, tw=tw, tp=tp, tur=tur, tl=tl, tws=tws, gun=gun, cal=cal); d.update(k); return d
SPEC = {
 'M4 SHERMAN':      V(5.84, 2.62, .42, .45, 'c', 2.1, 1.9, 2.9, 2, spare=1, mg=1, sponsonbox=1),
 'TIGER I':         V(6.32, 3.56, .72, .45, 'b', 2.6, 2.3, 4.9, 2, mb=1, fend=1, tools=1),
 'SHERMAN FIREFLY': V(5.89, 2.62, .42, .45, 'c', 2.1, 1.9, 4.3, 2, mb=1, bus=1, mg=1),
 'M4A3E8 EASY 8':   V(5.89, 2.67, .58, .45, 'c', 2.4, 2.1, 4.0, 2, mb=1, mg=1, spare=1),
 'M4A3E2 JUMBO':    V(5.89, 2.94, .42, .45, 'b', 2.3, 2.2, 3.0, 2, mg=1),
 'M26 PERSHING':    V(6.33, 3.51, .58, .45, 'c', 2.8, 2.3, 4.6, 2, mb=1, fend=1, mg=1),
 'M24 CHAFFEE':     V(5.03, 2.84, .41, .40, 'c', 2.0, 1.9, 2.6, 1, mg=1, fend=1),
 'M10 WOLVERINE':   V(5.82, 3.05, .42, .45, 'o', 2.6, 2.4, 3.6, 2, fend=1),
 'M18 HELLCAT':     V(5.28, 2.87, .37, .42, 'o', 2.3, 2.1, 4.0, 2, mb=1, fend=1),
 'M36 JACKSON':     V(5.88, 3.05, .42, .45, 'o', 2.7, 2.5, 4.8, 2, mb=1, fend=1, bus=1),
 'M3 STUART':       V(4.53, 2.24, .29, .40, 'r', 1.6, 1.5, 1.7, 1, mg=1),
 'M3 LEE':          V(5.64, 2.72, .42, .30, 'r', 1.6, 1.5, 2.0, 1, sponson=1),
 'M8 GREYHOUND':    V(5.00, 2.54, 0, .45, 'o', 2.0, 1.9, 1.6, 1, wh=3),
 'M3 HALFTRACK':    V(6.18, 2.22, .30, .5, 'n', gun=0, half=1, open=1),
 'WILLYS JEEP':     V(3.36, 1.57, 0, .5, 'n', gun=0, jeep=1, wh=2),
 'CHURCHILL VII':   V(7.44, 3.45, .56, .42, 'b', 2.2, 2.2, 2.5, 2, fend=1, chur=1),
 'CROMWELL':        V(6.35, 3.05, .36, .42, 'b', 2.0, 2.1, 2.6, 2, fend=1),
 'COMET':           V(6.55, 3.05, .46, .42, 'b', 2.3, 2.3, 3.8, 2, mb=0, fend=1, bus=1),
 'MATILDA II':      V(5.61, 2.59, .36, .45, 'c', 1.9, 1.8, 1.6, 1, skirt=1),
 'VALENTINE':       V(5.41, 2.63, .36, .45, 'c', 1.9, 1.8, 1.4, 1, fend=1),
 'T-34/76':         V(5.92, 3.00, .50, .35, 'h', 2.2, 2.0, 2.8, 2, spare=1, drums=0),
 'T-34/85':         V(6.10, 3.00, .50, .40, 'c', 2.6, 2.2, 4.0, 2, spare=1, drums=1),
 'KV-1':            V(6.75, 3.32, .70, .42, 'b', 2.3, 2.2, 2.6, 2, fend=1),
 'KV-2':            V(6.95, 3.32, .70, .42, 'b', 3.0, 2.6, 1.6, 3, fend=1),
 'IS-2':            V(6.77, 3.07, .65, .42, 'c', 2.6, 2.3, 4.9, 3, mb=1, fend=1, drums=1),
 'SU-100':          V(6.10, 3.00, .50, .45, 'n', gun=4.0, cal=2, cas=(0.0, .55)),
 'SU-152':          V(6.95, 3.25, .70, .42, 'n', gun=2.8, cal=3, mb=1, cas=(0.0, .55)),
 'TYPE 97 CHI-HA':  V(5.50, 2.33, .33, .40, 'r', 1.7, 1.6, 1.2, 1, mg=1),
 'TYPE 95 HA-GO':   V(4.38, 2.06, .25, .38, 'r', 1.5, 1.4, 1.0, 1),
 'PANZER III':      V(5.56, 2.95, .40, .45, 'b', 2.2, 2.0, 2.6, 2, fend=1),
 'PANZER IV H':     V(5.92, 2.88, .40, .42, 'b', 2.3, 2.0, 3.9, 2, mb=1, skirt=1, tskirt=1),
 'PANTHER':         V(6.87, 3.42, .66, .45, 'b', 2.6, 2.2, 5.0, 2, mb=1, fend=1, cupola=1),
 'TIGER II':        V(7.38, 3.76, .80, .45, 'b', 3.2, 2.4, 6.0, 2, mb=1, fend=1, bus=1),
 'JAGDPANTHER':     V(6.87, 3.42, .66, .45, 'n', gun=5.0, cal=2, mb=1, cas=(0.0, .62), fend=1),
 'STUG III':        V(5.38, 2.92, .40, .45, 'n', gun=3.0, cal=2, mb=1, cas=(.12, .50), skirt=1),
 'HETZER':          V(4.87, 2.63, .35, .45, 'n', gun=2.6, cal=2, cas=(0.0, .70), off=.3),
 'ELEFANT':         V(6.80, 3.38, .64, .45, 'n', gun=5.6, cal=2, mb=1, cas=(.35, .45)),
 'SDKFZ 251':       V(5.80, 2.10, .28, .5, 'n', gun=0, half=1, open=1, mg=1),
 'PUMA 8-RAD':      V(6.85, 2.40, 0, .45, 'b', 1.9, 1.6, 2.1, 1, mb=1, wh=4),
 'MAUS':            V(10.2, 3.71, .96, .40, 'b', 3.6, 3.0, 5.0, 3, fend=1),
 'E-100':           V(8.73, 4.48, 1.0, .40, 'b', 3.6, 3.0, 5.0, 3, fend=1, skirt=1),
 'JAGDTIGER':       V(7.38, 3.77, .80, .45, 'n', gun=4.2, cal=3, cas=(.28, .50), fend=1),
 'STURMTIGER':      V(6.28, 3.57, .72, .45, 'n', gun=0.5, cal=3, cas=(0.0, .55), mort=1),
 'PANTHER II':      V(7.10, 3.40, .66, .45, 'b', 2.5, 2.2, 5.0, 2, mb=1, fend=1),
 'VK 45.01 (P)':    V(6.70, 3.14, .64, .42, 'b', 2.6, 2.3, 4.9, 2, mb=1, fend=1),
 'GOLIATH':         V(1.50, 0.85, .18, .5, 'n', gun=0, mine=1),
 'T28 SUPER HEAVY': V(7.52, 4.39, .55, .45, 'n', gun=4.3, cal=3, cas=(.05, .45), dbl=1),
 'T29 HEAVY':       V(7.60, 3.80, .70, .42, 'b', 3.2, 2.8, 6.0, 3, fend=1),
 'T30 HEAVY':       V(7.60, 3.80, .70, .42, 'b', 3.2, 2.8, 5.0, 3, fend=1),
 'M4 CALLIOPE':     V(5.84, 2.62, .42, .45, 'c', 2.1, 1.9, 2.9, 2, rk=1),
 'IS-3':            V(6.77, 3.15, .65, .42, 's', 2.8, 2.6, 4.9, 3, mb=1, pike=1),
 'TOG II':          V(10.1, 3.12, .60, .40, 'b', 2.4, 2.2, 3.8, 2, fend=1),
 'A39 TORTOISE':    V(7.24, 3.91, .80, .45, 'n', gun=4.8, cal=3, cas=(0.0, .62)),
 'M48 PATTON':      V(6.82, 3.63, .70, .42, 'c', 3.0, 2.8, 4.0, 2, fend=1, cupola=2, bus=1),
 'T-55':            V(6.20, 3.37, .58, .36, 's', 2.4, 2.4, 5.2, 2, fx=1, fend=1, drums=1),
 'M60A3':           V(6.95, 3.63, .71, .42, 'c', 3.1, 2.9, 4.7, 2, fx=1, fend=1, cupola=2, bus=1, smoke=1),
 'T-72A':           V(6.95, 3.59, .58, .40, 's', 2.4, 2.4, 6.0, 2, fend=1, skirt=1, drums=1, smoke=1),
 'BMP-2':           V(6.72, 3.15, .38, .40, 's', 1.7, 1.8, 3.0, 1, ifv=1, atgm=1),
 'M113 APC':        V(4.86, 2.69, .38, .5, 'm', 1.0, 1.0, 1.3, 1, apc=1),
 'T-62':            V(6.63, 3.30, .58, .40, 's', 2.5, 2.5, 5.7, 2, fx=1, fend=1, drums=1),
 'BMP-1':           V(6.74, 2.94, .30, .40, 's', 1.6, 1.6, 1.8, 1, ifv=1, atgm=1),
 'M1A2 ABRAMS':     V(7.93, 3.66, .64, .44, 'a', 3.9, 3.6, 5.6, 2, fx=1, skirt=1, fend=1, bus=1, smoke=1, citv=1),
 'T-90M':           V(6.86, 3.78, .58, .40, 's', 2.6, 2.7, 6.0, 2, era=2, skirt=1, fend=1, bus=1, smoke=1, rws=1),
 'M2A3 BRADLEY':    V(6.55, 3.60, .53, .45, 'b', 2.0, 2.4, 2.4, 1, ifv=1, tow=1, skirt=1),
 'LEOPARD 2A7':     V(7.70, 3.75, .63, .42, 'w', 3.6, 3.4, 5.4, 2, skirt=1, fend=1, bus=1, smoke=1, rws=1),
 'CHALLENGER 2':    V(8.33, 3.52, .63, .42, 'a', 3.5, 3.3, 6.0, 2, fx=1, skirt=1, fend=1, bus=1, smoke=1),
 'STRYKER':         V(6.95, 2.72, 0, .5, 'm', 1.0, 1.0, 1.5, 1, wh=4, apc=1),
 'M1A2 SEPV3':      V(7.93, 3.66, .64, .44, 'a', 3.9, 3.6, 5.6, 2, fx=1, skirt=1, fend=1, bus=1, smoke=1, citv=1, rws=1),
 'M10 BOOKER':      V(6.90, 3.60, .55, .42, 'a', 3.2, 3.0, 4.6, 2, fx=1, skirt=1, fend=1, bus=1, smoke=1),
 'STRYKER DRAGOON': V(6.95, 2.72, 0, .45, 'r', 1.8, 1.8, 2.3, 1, wh=4),
 'LAV-25':          V(6.39, 2.50, 0, .42, 'r', 1.6, 1.8, 2.2, 1, wh=4),
 'AAV/ACV AMPHIB':  V(8.16, 3.27, .53, .5, 'm', 1.0, 1.0, 1.0, 1, apc=1, boat=1),
 'JLTV (TOW)':      V(6.20, 2.50, 0, .5, 'm', 1.2, 1.2, 1.4, 1, wh=2, truck=1),
 'HUMVEE (M2)':     V(4.57, 2.16, 0, .55, 'm', 1.0, 1.0, 1.3, 1, wh=2, truck=1),
 'MAXXPRO MRAP':    V(6.45, 2.52, 0, .55, 'm', 1.0, 1.0, 1.3, 1, wh=2, truck=1),
 'M109A7 PALADIN':  V(6.80, 3.90, .40, .62, 'b', 4.0, 3.4, 5.0, 3, mb=1, fx=1, fend=1, arty=1),
 'M142 HIMARS':     V(7.00, 2.40, 0, .5, 'n', gun=0, wh=3, truck=1, pod=(1, 2.2, 4.2)),
 'M270 MLRS':       V(6.85, 2.97, .53, .5, 'n', gun=0, pod=(2, 2.6, 4.0), fend=1),
 'AMPV':            V(6.70, 3.60, .53, .5, 'm', 1.0, 1.0, 1.4, 1, apc=1, skirt=1),
 'T-14 ARMATA':     V(8.70, 3.50, .62, .44, 'a', 3.4, 2.9, 6.0, 2, skirt=1, fend=1, bus=1, smoke=1, rws=1, unmanned=1),
 'T-80BVM':         V(7.01, 3.58, .58, .40, 's', 2.5, 2.5, 6.0, 2, era=2, skirt=1, fend=1, bus=1, smoke=1),
 'T-72B3':          V(6.95, 3.59, .58, .40, 's', 2.5, 2.5, 6.0, 2, era=1, skirt=1, fend=1, smoke=1, drums=1),
 'BMP-3':           V(7.14, 3.15, .38, .55, 's', 2.0, 2.0, 2.6, 2, ifv=1, coax30=1),
 'BTR-82A':         V(7.65, 2.90, 0, .35, 'r', 1.4, 1.5, 2.0, 1, wh=4),
 '2S19 MSTA-S':     V(7.15, 3.38, .58, .55, 'b', 4.2, 3.3, 6.0, 3, mb=1, fx=1, fend=1, arty=1),
 'TOS-1A':          V(6.86, 3.46, .58, .5, 'n', gun=0, pod=(1, 2.8, 3.6), fend=1),
 'PANTSIR-S1':      V(7.20, 2.55, 0, .55, 'b', 2.4, 2.2, 1.8, 1, wh=4, truck=1, sam=1),
 'LEOPARD 2A8':     V(7.70, 3.75, .63, .42, 'w', 3.6, 3.4, 5.4, 2, skirt=1, fend=1, bus=1, smoke=1, rws=1),
 'CHALLENGER 3':    V(8.33, 3.52, .63, .42, 'a', 3.5, 3.3, 5.5, 2, skirt=1, fend=1, bus=1, smoke=1, rws=1),
 'LECLERC':         V(6.88, 3.71, .63, .42, 'a', 3.8, 3.3, 5.0, 2, fx=0, skirt=1, fend=1, bus=1, smoke=1),
 'MERKAVA MK4':     V(7.60, 3.72, .60, .58, 'w', 4.2, 3.2, 5.4, 2, fx=1, skirt=1, fend=1, bus=1, smoke=1, chains=1),
 'NAMER':           V(7.45, 3.72, .60, .5, 'm', 1.0, 1.0, 1.3, 1, apc=1, skirt=1, frontengine=1),
 'K2 BLACK PANTHER': V(7.50, 3.60, .63, .42, 'a', 3.6, 3.3, 5.6, 2, skirt=1, fend=1, bus=1, smoke=1),
 'K21 KNIFV':       V(6.90, 3.40, .50, .45, 'b', 2.2, 2.4, 2.8, 1, ifv=1, skirt=1),
 'TYPE 99A':        V(7.60, 3.50, .60, .42, 'w', 3.4, 3.1, 5.6, 2, skirt=1, fend=1, bus=1, smoke=1, era=1),
 'ZTZ-15':          V(6.80, 3.30, .55, .42, 'a', 3.0, 2.8, 4.4, 2, skirt=1, fend=1, bus=1, smoke=1),
 'ZBD-04':          V(7.20, 3.20, .50, .50, 's', 1.9, 2.0, 2.6, 2, ifv=1),
 'CV9035':          V(6.55, 3.10, .53, .45, 'b', 2.4, 2.3, 3.4, 1, ifv=1, skirt=1),
 'PUMA IFV':        V(7.60, 3.90, .55, .45, 'b', 2.0, 2.0, 2.8, 1, ifv=1, skirt=1, unmanned=1),
 'BOXER':           V(7.93, 2.99, 0, .5, 'm', 1.0, 1.0, 1.3, 1, wh=4, apc=1),
 'PATRIA AMV':      V(7.70, 2.80, 0, .45, 'r', 1.6, 1.6, 2.4, 1, wh=4),
 'MARK IV MALE':    V(8.05, 4.12, .52, .5, 'n', gun=0, ww1=1, sponson=2),
 'MARK V':          V(8.05, 4.11, .52, .5, 'n', gun=0, ww1=1, sponson=2, cab=1),
 'A7V STURMPANZER': V(7.34, 3.06, .50, .5, 'n', gun=0.8, cal=2, a7v=1),
 'RENAULT FT':      V(5.00, 1.74, .32, .42, 'c', 1.4, 1.3, 1.0, 1, ft=1),
 'WHIPPET MK A':    V(6.10, 2.62, .50, .5, 'n', gun=0, whip=1),
 'SCHNEIDER CA1':   V(6.32, 2.05, .36, .5, 'n', gun=0, box=1),
 'ST CHAMOND':      V(8.90, 2.67, .32, .5, 'n', gun=1.2, cal=2, box=1),
 'LK II':           V(5.10, 1.95, .30, .55, 'b', 1.6, 1.5, 1.0, 1),
 'ROLLS-ROYCE AC':  V(4.93, 1.91, 0, .45, 'c', 1.4, 1.4, 0.8, 1, wh=2, bonnet=1),
 'EHRHARDT E-V/4':  V(5.30, 2.00, 0, .5, 'c', 1.4, 1.4, 0.8, 1, wh=2),
}
SPEC['T-72A']['tp'] = .40
# ------------------------------------------------------------------------------------------------ armour model
def _geom(sp, glen, gwid):
    """pixel geometry: keep the GBA hull length, width from the real L:W ratio"""
    L = float(glen); sc = L / sp['L']
    W = min(float(gwid), max(8.0, sp['W'] * sc * 1.25)); W = round(W / 2) * 2   # 25% wider than true scale so it reads at 32 px
    y0 = 16 - sp['tp'] * L; y0 = max(1.0, min(y0, 31.0 - L)); y0 = round(y0)
    return L, W, sc, y0
def hull_model(sp, glen=24, gwid=20):
    L, W, sc, y0 = _geom(sp, glen, gwid); y1 = y0 + L; cx = 16.0; x0, x1 = cx - W / 2, cx + W / 2
    m = Model(); wh = sp.get('wh', 0)
    tw = 0 if wh else max(3.0, min(5.0, round(sp['tw'] * sc * 1.4)))
    if sp.get('mine'):
        tw = 2; 
    # --- running gear
    if wh:
        n = wh; ty = [y0 + 2 + k * (L - 6) / max(1, n - 1) for k in range(n)] if n > 1 else [y0 + L / 2]
        if n == 2: ty = [y0 + L * 0.12, y1 - L * 0.12 - 4]
        for y in ty:
            for xa in (x0 - 0.2, x1 - 2.8):
                m.rrect(xa, y, xa + 3, y + 4.4, 1.2, METAL, 0.12)                  # tyre
                m.rect(xa + 0.6, y + 0.5, xa + 2.4, y + 1.2, METAL, 0.30)          # tread lit
                m.rect(xa + 0.6, y + 3.0, xa + 2.4, y + 3.7, METAL, 0.30)
        bx0, bx1 = x0 + 1.4, x1 - 1.4
    elif sp.get('half'):
        for xa in (x0, x1 - 3):
            m.rrect(xa, y0 + L * 0.45, xa + 3, y1, 1.0, METAL, 0.12)
            for k in range(int(L * 0.55 / 1.5)): m.rect(xa + 0.4, y0 + L * 0.45 + 0.6 + k * 1.5, xa + 2.6, y0 + L * 0.45 + 1.3 + k * 1.5, METAL, 0.32)
            m.rrect(xa + 0.2, y0 + 1.5, xa + 2.8, y0 + 6.0, 1.0, METAL, 0.12); m.rect(xa + 0.8, y0 + 2.6, xa + 2.2, y0 + 4.9, METAL, 0.5)
        bx0, bx1 = x0 + 2.2, x1 - 2.2
    else:
        for xa in (x0, x1 - tw):
            m.rect(xa, y0, xa + tw, y1, METAL, 0.30, tag=TRACK, dz=S * 1.0)     # track run (links drawn on the pixel grid by _post)
        ov = 0.5 if (sp.get('fend') or sp.get('skirt')) else 0.4                 # sponsons / fenders overhang the tracks
        bx0, bx1 = x0 + tw * (1 - ov), x1 - tw * (1 - ov)
    bw = bx1 - bx0
    if sp.get('mine'):
        m.rect(bx0 - 0.3, y0 + 1, bx1 + 0.3, y1 - 1, CAMO, 0.55, bevel=0.25); m.rect(bx0 + 1, y0 + 1.5, bx1 - 1, y0 + 3, CAMO, 0.8)
        m.rect(cx - 0.5, y1 - 1.6, cx + 0.5, y1 - 0.8, TEAM, .5); return m
    if sp.get('jeep'):
        m.rrect(bx0 - .6, y0, bx1 + .6, y1, 1.2, CAMO, 0.50, bevel=0.2)
        m.rrect(bx0 + .4, y0 + .3, bx1 - .4, y0 + L * .34, 1.0, CAMO, 0.72, bevel=0.15)          # bonnet
        m.rect(cx - .3, y0 + .6, cx + .3, y0 + L * .32, CAMO, 0.45)
        m.rect(bx0, y0 + L * .37, bx1, y0 + L * .42, GLASS, .5)                                # folded windscreen
        m.rect(bx0 + .5, y0 + L * .45, bx1 - .5, y1 - 2.2, DARK, 0.6)                          # open body
        for sx in (bx0 + 1, cx + .5): m.rect(sx, y0 + L * .47, sx + bw / 2 - 1.5, y0 + L * .62, CAMO, 0.35)   # seats
        m.rect(bx0 + 1, y1 - 2.4, bx1 - 1, y1 - 0.6, METAL, 0.25)                              # spare wheel
        for hx in (bx0 + .2, bx1 - 1.2): m.rect(hx, y0 + .2, hx + 1, y0 + 1.0, METAL, 0.95)
        return m
    if sp.get('truck'):
        cab = L * 0.30
        m.rrect(bx0 - .4, y0, bx1 + .4, y0 + cab, 1.4, CAMO, 0.55, bevel=0.22)                 # cab / engine
        m.rect(bx0 + .6, y0 + cab * .45, bx1 - .6, y0 + cab * .62, GLASS, .5)                  # windscreen
        m.rect(bx0 + 1, y0 + .6, bx1 - 1, y0 + cab * .38, CAMO, 0.72)                           # bonnet top
        m.rect(bx0, y0 + cab + .3, bx1, y1, CAMO, 0.45, bevel=0.22)                            # bed / module
        m.rect(bx0 + 1, y0 + cab + 1.2, bx1 - 1, y1 - 1.0, CAMO, 0.32)
        for k in range(int((L - cab - 3) / 2.5)): m.rect(bx0 + 1.2, y0 + cab + 2 + k * 2.5, bx1 - 1.2, y0 + cab + 2.6 + k * 2.5, CAMO, 0.55)
        for hx in (bx0 + .1, bx1 - 1.1): m.rect(hx, y0 + .2, hx + 1, y0 + 1.0, METAL, 0.95)
        if sp.get('pod'): _pods(m, sp, cx, y0 + cab + .5, y1, sc)
        m.rect(cx - 1, y1 - 1.0, cx + 1, y1 - .2, TEAM, .5); return m
    # --- fenders / sponsons over the tracks
    if sp.get('skirt') or sp.get('ww1'):
        cov = 0.65
        for side in (0, 1):
            fx0, fx1 = (x0 - 0.1, x0 + tw * cov + 0.4) if side == 0 else (x1 - tw * cov - 0.4, x1 + 0.1)
            ya, yb = y0 + (2.2 if not sp.get('ww1') else 0), y1 - (1.8 if not sp.get('ww1') else 0)
            m.rect(fx0, ya, fx1, yb, CAMO, 0.44 if sp.get('skirt') else 0.50, bevel=0.15)
            if sp.get('skirt'):
                for k in range(1, int((yb - ya) / 4.2) + 1):
                    m.rect(fx0, ya + k * 4.2 - .35, fx1, ya + k * 4.2 + .25, CAMO, 0.2)          # skirt plate seams
            if sp.get('fend') and L >= 20 and not sp.get('skirt'):
                for k, by in enumerate((ya + (yb - ya) * .45, ya + (yb - ya) * .65)):
                    m.rect(fx0 + .3, by, fx1 - .2, by + 2.2, CAMO, 0.66, bevel=0.2)               # stowage bins
    # --- hull body
    gl = max(2.5, L * 0.17)
    if sp.get('ww1'):
        m.rect(bx0, y0 + .8, bx1, y1 - .8, CAMO, 0.48, bevel=0.22)
        m.rect(bx0 + 1, y0 + 2, bx1 - 1, y0 + L * .38, CAMO, 0.66, bevel=0.15)                 # cab / driver box
        m.rect(bx0 + 1.5, y0 + 2.6, bx1 - 1.5, y0 + 3.4, GLASS, .5)                             # vision slits
        if sp.get('cab'): m.rect(cx - 2, y0 + L * .55, cx + 2, y0 + L * .72, CAMO, 0.7, bevel=0.2)
        for k in range(3): m.rect(bx0 + 1.4, y1 - 2.5 - k * 1.8, bx1 - 1.4, y1 - 1.9 - k * 1.8, DARK, 0.6)   # louvres
        for side in (0, 1):                                                                         # sponson 6-pdrs
            sx = bx0 - 1.5 if side == 0 else bx1 - 1.3; sy = y0 + L * .42
            m.rect(sx, sy, sx + 2.8, sy + 4.5, CAMO, 0.55, bevel=0.25)
            gx = sx - 2.0 if side == 0 else sx + 2.8
            m.rect(gx, sy + 1.8, gx + 2.0, sy + 2.7, METAL, 0.6)
        m.rect(bx0 + 1.5, y0 + L * .5, bx0 + 2.4, y0 + L * .8, METAL, .5)                     # unditching beam rails
        m.rect(bx1 - 2.4, y0 + L * .5, bx1 - 1.5, y0 + L * .8, METAL, .5)
        m.rect(bx0 + 1.5, y0 + L * .78, bx1 - 1.5, y0 + L * .84, WOOD, .5)                      # unditching beam
        m.rect(cx - 1, y1 - 1.6, cx + 1, y1 - .9, TEAM, .5); return m
    if sp.get('a7v'):
        m.poly([(bx0 - 1, y0 + 2), (bx0 + 1, y0), (bx1 - 1, y0), (bx1 + 1, y0 + 2), (bx1 + 1, y1 - 2), (bx1 - 1, y1), (bx0 + 1, y1), (bx0 - 1, y1 - 2)], CAMO, 0.48, bevel=0.25)
        m.rect(cx - 2.5, y0 + L * .32, cx + 2.5, y0 + L * .56, CAMO, 0.72, bevel=0.2)          # commander / driver cupola
        m.rect(cx - 2, y0 + L * .36, cx + 2, y0 + L * .38, GLASS, .5)
        for k in range(4): m.rect(bx0 + .5, y0 + 3 + k * L * .2, bx0 + 1.2, y0 + 4 + k * L * .2, DARK, .5); m.rect(bx1 - 1.2, y0 + 3 + k * L * .2, bx1 - .5, y0 + 4 + k * L * .2, DARK, .5)
        m.rect(cx - 1, y1 - 1.6, cx + 1, y1 - .9, TEAM, .5); return m
    pts = [(bx0, y0 + gl * 0.7), (bx0 + 1.0, y0 + 0.3), (bx1 - 1.0, y0 + 0.3), (bx1, y0 + gl * 0.7), (bx1, y1 - 0.6), (bx1 - 0.6, y1), (bx0 + 0.6, y1), (bx0, y1 - 0.6)]
    if sp.get('chur'): pts = [(bx0, y0 + 1), (bx1, y0 + 1), (bx1, y1), (bx0, y1)]
    m.poly(pts, CAMO, 0.52, bevel=0.20)
    if not sp.get('frontengine'):
        m.poly([(bx0 + 0.6, y0 + gl * 0.75), (bx0 + 1.3, y0 + 0.6), (bx1 - 1.3, y0 + 0.6), (bx1 - 0.6, y0 + gl * 0.75)], CAMO, 0.40)   # sloped glacis (darker than the deck)
        m.rect(bx0 + 1.2, y0 + gl * 0.75 - 0.2, bx1 - 1.2, y0 + gl * 0.75 + 0.6, CAMO, 0.82)                                      # lit roof edge
        m.rect(bx0 + 1.6, y0 + 0.5, bx1 - 1.6, y0 + 1.2, CAMO, 0.72)                                                             # lit nose
    for hx in (bx0 + 0.6, bx1 - 1.6): m.rect(hx, y0 + 0.4, hx + 1, y0 + 1.3, METAL, 0.95)                                          # headlights
    if sp.get('spare') and bw >= 9:
        for k in range(3): m.rect(cx - 3.5 + k * 2.4, y0 + 1.3, cx - 1.8 + k * 2.4, y0 + 2.5, METAL, 0.32)                     # spare track links
    if sp.get('chains'): m.rect(bx0 + 1, y1 - 1.6, bx1 - 1, y1 - .4, METAL, .32)                                            # Merkava rear ball chains
    # driver hatch (left on most western tanks, centre on Soviet)
    dh = cx if sp.get('tur') == 's' or sp.get('cas') else cx - bw * 0.22
    if not sp.get('cas') and not sp.get('open') and not sp.get('frontengine'):
        m.ell(dh - 1.2, y0 + gl * 0.8 + 0.3, dh + 1.2, y0 + gl * 0.8 + 2.5, CAMO, 0.78, bevel=0.35)
        if sp.get('tur') in ('c', 'r', 'o') and bw >= 10: m.ell(2 * cx - dh - 1.2, y0 + gl * 0.8 + 0.3, 2 * cx - dh + 1.2, y0 + gl * 0.8 + 2.5, CAMO, 0.78, bevel=0.35)
    if sp.get('frontengine'):
        for k in range(3): m.rect(bx0 + 1.5, y0 + 1.6 + k * 1.6, bx1 - 1.5, y0 + 2.2 + k * 1.6, CAMO, 0.25)
        m.rect(bx0 + 1, y1 - 3, bx1 - 1, y1 - .8, CAMO, .66, bevel=.2)
    # engine deck
    if not sp.get('open') and not sp.get('frontengine'):
        ed = max(4.0, L * 0.27); ey = y1 - ed
        m.rect(bx0 + 0.8, ey, bx1 - 0.8, y1 - 0.8, CAMO, 0.44)
        m.rect(bx0 + 1.8, round(ey) + 1, cx - 0.6, round(y1) - 1.2, CAMO, 0.4, tag=GRILLE); m.rect(cx + 0.6, round(ey) + 1, bx1 - 1.8, round(y1) - 1.2, CAMO, 0.4, tag=GRILLE)
        m.rect(cx - 0.5, ey, cx + 0.5, y1 - 0.8, CAMO, 0.66)
        if sp.get('drums'):
            for ex in (bx0 + 0.6, bx1 - 3.0): m.rrect(ex, y1 - 1.6, ex + 2.4, y1 + 0.2, 1.0, METAL, 0.45, bevel=0.2)   # fuel drums
        else:
            for ex in (bx0 + 1.2, bx1 - 3.0): m.rect(ex, y1 - 1.0, ex + 1.8, y1, DARK, 0.6)                          # exhausts
    else:
        if sp.get('open'):
            m.rect(bx0 + 1.0, y0 + L * 0.36, bx1 - 1.0, y1 - 1.0, DARK, 0.58)                                      # open crew bay
            for xx in (bx0 + 1.2, bx1 - 2.2):
                m.rect(xx, y0 + L * .4, xx + 1, y1 - 1.6, CAMO, 0.32)                                                # benches
            if sp.get('half'):
                m.rect(bx0 + 0.6, y0 + 0.5, bx1 - 0.6, y0 + L * 0.28, CAMO, 0.66, bevel=.25)                           # armoured bonnet
                for k in range(3): m.rect(bx0 + 1.4, y0 + 1.2 + k * 1.3, bx1 - 1.4, y0 + 1.7 + k * 1.3, DARK, .6)      # radiator louvres
                m.rect(bx0 + .4, y0 + L * .30, bx1 - .4, y0 + L * .34, GLASS, .5)
    if L >= 18 and not sp.get('open'): m.rect(bx0 + 0.7, y0 + L * 0.42, bx0 + 1.5, y0 + L * 0.66, METAL, 0.55)               # pioneer tools
    if sp.get('apc'):                                                                                                      # roof hatches, rear ramp
        m.rect(cx - bw * .3, y0 + L * .45, cx + bw * .3, y0 + L * .7, CAMO, 0.66, bevel=0.25)
        m.rect(cx - bw * .3 + .4, y0 + L * .57, cx + bw * .3 - .4, y0 + L * .58 + .4, CAMO, 0.3)
        m.rect(bx0 + 1.2, y1 - 1.8, bx1 - 1.2, y1 - .4, CAMO, .32)
        if sp.get('boat'): m.poly([(bx0, y0 + 2), (cx, y0 - .4), (bx1, y0 + 2)], CAMO, .62)
    if sp.get('cas'): _casemate(m, sp, cx, y0, L, bw, sc)
    if sp.get('pod') and not sp.get('truck'): _pods(m, sp, cx, y0 + L * 0.35, y1 - 1, sc)
    m.rect(cx - 1, y1 - 1.6, cx + 1, y1 - 0.9, TEAM, 0.5)                                                                # team marking
    return m
def _pods(m, sp, cx, ya, yb, sc):
    n, pw, pl = sp['pod']; pw *= sc; pl *= sc; pl = min(pl, yb - ya)
    for k in range(n):
        px0 = cx - n * pw / 2 - (n - 1) * 0.3 + k * (pw + 0.6)
        m.rect(px0, yb - pl, px0 + pw, yb, CAMO, 0.62, bevel=0.22)
        for xx in np.arange(px0 + 0.8, px0 + pw - 0.6, 1.5):
            for yy in np.arange(yb - pl + 0.8, yb - pl + min(pl, 5.5), 1.5): m.rect(xx, yy, xx + 0.8, yy + 0.8, DARK, 0.2)   # tube mouths
def _casemate(m, sp, cx, y0, L, bw, sc):
    fy, ln = sp['cas']; cy0 = y0 + fy * L + 1.2; cy1 = cy0 + ln * L; cw = bw - 1.2 if fy < .2 else bw - 2.2
    if sp.get('off'): cx2 = cx - 0.6
    else: cx2 = cx
    m.poly([(cx - cw / 2, cy0 + 2), (cx - cw / 2 + 1.5, cy0), (cx + cw / 2 - 1.5, cy0), (cx + cw / 2, cy0 + 2), (cx + cw / 2, cy1), (cx - cw / 2, cy1)], CAMO, 0.64, bevel=0.30)
    m.rect(cx - cw / 2 + 1.4, cy0 + 0.3, cx + cw / 2 - 1.4, cy0 + 1.0, CAMO, 0.86)                 # lit front plate edge
    hx, hy = cx + cw / 2 - 2.6, cy1 - 2.6
    m.ell(hx - 1.3, hy - 1.3, hx + 1.3, hy + 1.3, METAL, 0.6, bevel=0.3); m.ell(hx - .6, hy - .6, hx + .6, hy + .6, DARK, 0.5)   # commander periscope / hatch
    m.ell(cx - cw / 2 + 1.0, cy1 - 3.5, cx - cw / 2 + 3.4, cy1 - 1.1, CAMO, 0.8, bevel=0.35)                                     # loader hatch
    if sp.get('mort'):
        m.ell(cx2 - 2.4, cy0 - 1.6, cx2 + 2.4, cy0 + 2.4, METAL, 0.5, bevel=0.3); m.ell(cx2 - 1.3, cy0 - .6, cx2 + 1.3, cy0 + 1.6, DARK, 0.2); return
    gl = sp['gun'] * sc; w = {1: 1.0, 2: 1.7, 3: 2.3}[sp['cal']]
    ytop = max(0.0, cy0 - gl)
    m.ell(cx2 - w / 2 - 1.6, cy0 - 1.4, cx2 + w / 2 + 1.6, cy0 + 1.8, CAMO, 0.5, bevel=0.25)       # Saukopf / ball mantlet
    _barrel(m, cx2, ytop, cy0 + 0.5, w, sp)
    if sp.get('dbl'):
        pass
def _barrel(m, gx, ytop, ybase, w, sp):
    m.rect(gx - w / 2, ytop, gx + w / 2, ybase, METAL, 0.55, cyl=1, dz=S * 1.2)
    if sp.get('fx'):
        fy = ytop + (ybase - ytop) * 0.45; m.rect(gx - w / 2 - 0.5, fy, gx + w / 2 + 0.5, fy + 1.4, METAL, 0.52, bevel=0.15)   # fume extractor
    if sp.get('mb'): m.rect(gx - w / 2 - 0.7, ytop, gx + w / 2 + 0.7, ytop + 1.3, METAL, 0.42, bevel=0.2)                     # muzzle brake
    m.rect(gx - w / 2 + 0.2, ytop, gx + w / 2 - 0.2, ytop + 0.5, METAL, 0.02)                                                 # bore
def turret_model(sp, glen=24, gwid=20):
    L, W, sc, y0 = _geom(sp, glen, gwid); m = Model(); cx = 16.0; tu = sp['tur']
    if tu == 'n': return m
    tl = sp['tl'] * sc * 1.12; tws = min(sp['tws'] * sc * 1.25, W - 2); tl = min(tl, 15.0)   # same exaggeration as the hull width
    ty0 = 16 - tl * 0.42; ty1 = ty0 + tl; tx0, tx1 = cx - tws / 2, cx + tws / 2
    w = {1: 1.0, 2: 1.7, 3: 2.3}[sp['cal']]; gl = min(sp['gun'] * sc + (16 - ty0), 15.6)
    ytop = max(0.2, 16 - gl)
    if tu == 'm':                                                                                  # cupola / RWS only
        m.ell(cx - 2.4, 13.6, cx + 2.4, 18.4, CAMO, 0.55, bevel=0.3); m.ell(cx - 1.2, 14.8, cx + 1.2, 17.2, DARK, 0.5)
        m.rect(cx - 1.6, 13.2, cx + 1.6, 14.6, METAL, 0.45, bevel=.2)                              # gun shield / RWS head
        m.rect(cx - 0.4, 16 - min(6.0, sp['gun'] * sc + 3), cx + 0.4, 14.0, METAL, 0.6)
        return m
    # gun first so the turret front / mantlet overlap its base
    if tu != 'o': _barrel(m, cx, ytop, ty0 + 1.5, w, sp)
    if tu == 'c':
        m.ell(tx0, ty0, tx1, ty1, CAMO, 0.66, bevel=0.30, dome=0.42)
        m.rect(cx - w / 2 - 1.4, ty0 - 0.9, cx + w / 2 + 1.4, ty0 + 1.6, CAMO, 0.45, bevel=0.2)                  # cast mantlet
    elif tu == 's':                                                                                               # Soviet saucer
        m.ell(tx0, ty0 + 0.2, tx1, ty1, CAMO, 0.66, bevel=0.32, dome=0.55)
        m.rect(cx - w / 2 - 0.8, ty0 - 0.4, cx + w / 2 + 0.8, ty0 + 1.4, CAMO, 0.35)
    elif tu == 'h':                                                                                               # T-34/76 hex
        m.poly([(tx0 + 1.6, ty0), (tx1 - 1.6, ty0), (tx1, ty0 + tl * .4), (tx1 - .4, ty1), (tx0 + .4, ty1), (tx0, ty0 + tl * .4)], CAMO, 0.66, bevel=0.28, dome=0.25)
        m.rect(cx - w / 2 - 1.4, ty0 - 0.6, cx + w / 2 + 1.4, ty0 + 1.2, CAMO, 0.42)
    elif tu == 'r':                                                                                               # small turret
        m.rrect(tx0, ty0, tx1, ty1, min(tws, tl) * 0.35, CAMO, 0.66, bevel=0.30, dome=0.35)
        m.rect(cx - w / 2 - 0.8, ty0 - 0.5, cx + w / 2 + 0.8, ty0 + 1.0, CAMO, 0.42)
    elif tu == 'b':                                                                                               # welded box
        m.poly([(tx0 + 1.0, ty0), (tx1 - 1.0, ty0), (tx1, ty0 + 1.2), (tx1, ty1 - 0.6), (tx1 - .6, ty1), (tx0 + .6, ty1), (tx0, ty1 - 0.6), (tx0, ty0 + 1.2)], CAMO, 0.66, bevel=0.30, dome=0.18)
        m.rect(tx0 + 1.0, ty0 + 0.2, tx1 - 1.0, ty0 + 0.9, CAMO, 0.84)                                           # lit front plate edge
        m.rect(cx - w / 2 - 1.5, ty0 - 1.0, cx + w / 2 + 1.5, ty0 + 0.9, CAMO, 0.40, bevel=0.15)                 # mantlet
    elif tu == 'a':                                                                                               # flat composite (Abrams, Challenger, K2)
        f = tl * 0.30
        m.poly([(tx0 + tws * .28, ty0), (tx1 - tws * .28, ty0), (tx1, ty0 + f), (tx1, ty1 - .8), (tx1 - .8, ty1), (tx0 + .8, ty1), (tx0, ty1 - .8), (tx0, ty0 + f)], CAMO, 0.68, bevel=0.28, dome=0.12)
        m.poly([(tx0 + tws * .30, ty0 + .5), (tx1 - tws * .30, ty0 + .5), (tx1 - .7, ty0 + f), (tx0 + .7, ty0 + f)], CAMO, 0.46)   # sloped front cheeks
        m.rect(tx0 + .8, ty0 + f, tx1 - .8, ty0 + f + .6, CAMO, 0.84)                                            # roof edge catches the light
        m.rect(cx - w / 2 - .8, ty0 - .6, cx + w / 2 + .8, ty0 + 1.0, CAMO, 0.38)                                # gun shield
        m.rect(cx + tws * .14, ty0 + f * .55, cx + tws * .14 + 1.8, ty0 + f * .55 + 1.6, METAL, 0.62, bevel=.3)   # gunner's primary sight
        m.rect(tx0 + 1.0, ty0 + tl * .62, tx1 - 1.0, ty0 + tl * .62 + .5, CAMO, 0.40)                            # roof plate seam
    elif tu == 'w':                                                                                               # arrowhead wedge (Leopard 2A5+, Merkava, Type 99A)
        f = tl * 0.36
        m.poly([(cx - 1.3, ty0 - 1.0), (cx + 1.3, ty0 - 1.0), (tx1, ty0 + f), (tx1, ty1 - .8), (tx1 - .8, ty1), (tx0 + .8, ty1), (tx0, ty1 - .8), (tx0, ty0 + f)], CAMO, 0.68, bevel=0.28, dome=0.12)
        m.poly([(cx - 1.1, ty0 - .4), (tx0 + .9, ty0 + f), (tx0 + 1.5, ty0 + f + .5), (cx - .6, ty0 + 1.4)], CAMO, 0.42)   # wedge faces
        m.poly([(cx + 1.1, ty0 - .4), (tx1 - .9, ty0 + f), (tx1 - 1.5, ty0 + f + .5), (cx + .6, ty0 + 1.4)], CAMO, 0.42)
        m.rect(tx0 + 1, ty0 + f + .4, tx1 - 1, ty0 + f + 1.0, CAMO, 0.84)
        m.rect(cx + tws * .16, ty0 + f + 1.2, cx + tws * .16 + 1.6, ty0 + f + 2.8, METAL, 0.62, bevel=.3)          # gunner's sight
    elif tu == 'o':                                                                                               # open-top TD ring
        m.ell(tx0, ty0, tx1, ty1, CAMO, 0.58, bevel=0.3, dome=0.1)
        m.ell(tx0 + 1.4, ty0 + 1.6, tx1 - 1.4, ty1 - 1.2, DARK, 0.62)                                            # open fighting compartment
        m.rect(cx - 1.2, ty0 + 2.4, cx + 1.2, ty0 + tl * .55, METAL, 0.35)                                       # gun breech
        m.rect(cx - w / 2 - 1.4, ty0 - 0.8, cx + w / 2 + 1.4, ty0 + 1.6, CAMO, 0.48, bevel=0.2)
        _barrel(m, cx, ytop, ty0 + 0.4, w, sp)
        if sp.get('bus'): m.rect(tx0 + 1.5, ty1 - 1.2, tx1 - 1.5, ty1 + 1.0, CAMO, 0.45, bevel=0.2)
        m.rect(tx0 + 1.4, ty1 - 2.2, tx0 + 2.4, ty1 - 1.4, TEAM, .5)
        return m
    if sp.get('bus') and tu in ('a', 'w', 's', 'b', 'c'):                                                        # bustle rack / basket
        by0 = ty1 - 0.6; by1 = min(31.0, ty1 + max(1.6, tl * .16))
        m.rect(tx0 + .8, by0, tx1 - .8, by1, METAL, 0.30, bevel=0.1)
        for xx in np.arange(tx0 + 1.4, tx1 - 1.2, 1.4): m.rect(xx, by0 + .3, xx + .6, by1 - .3, METAL, 0.12)
    if sp.get('era'):                                                                                             # ERA bricks on the front arc
        rows = 2 if sp['era'] >= 2 else 1
        for r in range(rows):
            yy = ty0 + 0.6 + r * 1.4
            for k, xx in enumerate(np.arange(tx0 + .6, cx - w / 2 - 1.0, 1.5)):
                m.rect(xx, yy, xx + 1.2, yy + 1.1, CAMO, 0.80 if (k + r) % 2 == 0 else 0.48)
            for k, xx in enumerate(np.arange(cx + w / 2 + 1.0, tx1 - 1.0, 1.5)):
                m.rect(xx, yy, xx + 1.2, yy + 1.1, CAMO, 0.80 if (k + r) % 2 == 0 else 0.48)
    if sp.get('tskirt'):                                                                                          # Pz IV turret Schuerzen
        m.rect(tx0 - .8, ty0 + 1.4, tx0 + .2, ty1 + .4, CAMO, 0.4); m.rect(tx1 - .2, ty0 + 1.4, tx1 + .8, ty1 + .4, CAMO, 0.4)
        m.rect(tx0 - .8, ty1 - .2, tx1 + .8, ty1 + .6, CAMO, 0.4)
    # commander cupola (right rear), loader hatch (left), periscopes
    if not sp.get('unmanned'):
        r = 1.8 if tws >= 10 else 1.4 if tws >= 7 else 1.1
        ccx, ccy = tx1 - max(r + .8, tws * 0.27), ty1 - max(r + .7, tl * 0.32)
        if sp.get('cupola') == 2: r += .3
        m.ell(ccx - r, ccy - r, ccx + r, ccy + r, METAL, 0.62, bevel=0.35)
        m.ell(ccx - r * .5, ccy - r * .5, ccx + r * .5, ccy + r * .5, CAMO, 0.85 if r > 1.3 else 0.3, bevel=0.4)
        m.rect(ccx - .35, ccy - r - .6, ccx + .35, ccy - r + .2, GLASS, .5)                                     # vision block
        if tws >= 8 and tu != 'r':
            lx = tx0 + max(1.6, tws * .26)
            m.ell(lx - 1.3, ccy - 1.1, lx + 1.3, ccy + 1.3, CAMO, 0.82, bevel=0.4)                               # loader hatch
            m.rect(lx - .3, ccy - 2.2, lx + .3, ccy - 1.4, GLASS, .5)
    else:
        m.rect(cx + tws * .15, ty1 - tl * .45, cx + tws * .15 + 1.6, ty1 - tl * .45 + 1.4, METAL, 0.62, bevel=.2)  # sensor head
    if sp.get('citv'): m.rect(tx0 + tws * .18, ty0 + tl * .28, tx0 + tws * .18 + 1.6, ty0 + tl * .28 + 1.6, METAL, 0.70, bevel=0.3)
    if sp.get('smoke'):
        for side in (-1, 1):
            sx = cx + side * (tws / 2 - 1.2); sy = ty0 + tl * .30
            for k in range(2): m.rect(sx - .5, sy + k * 1.1, sx + .5, sy + k * 1.1 + .8, METAL, 0.20)
    if sp.get('rws') or sp.get('mg'):
        mx = tx1 - max(1.8, tws * .27) if sp.get('rws') else tx0 + max(1.5, tws * .26)
        my = ty1 - tl * .32 - 2.2
        m.rect(mx - .35, my - 2.6, mx + .35, my + .6, METAL, 0.60)
        if sp.get('rws'): m.rect(mx - 1.0, my - .4, mx + 1.0, my + 1.2, METAL, 0.42, bevel=.2)
    if sp.get('atgm'): m.rect(cx + tws / 2 - 1.0, ty0 + .4, cx + tws / 2 + .3, ty0 + 4.4, METAL, 0.55, bevel=0.2)
    if sp.get('tow'):
        m.rect(tx0 - 2.0, ty0 + 1, tx0 + .2, ty0 + 5.0, CAMO, 0.6, bevel=0.25)
        for yy in (ty0 + 1.4, ty0 + 3.0): m.rect(tx0 - 1.6, yy, tx0 - .2, yy + .8, DARK, 0.2)
    if sp.get('coax30'):
        m.rect(cx + 1.6, 16 - min(15, sp['gun'] * sc * .6 + 6), cx + 2.3, ty0 + 1, METAL, .55)
    if sp.get('sam'):
        for side in (-1, 1):
            sx = cx + side * (tws / 2 + .8); m.rect(sx - 1.0, ty0 + .5, sx + 1.0, ty0 + 6.5, CAMO, 0.6, bevel=.2)
            for yy in (ty0 + 1, ty0 + 2.6, ty0 + 4.2): m.rect(sx - .5, yy, sx + .5, yy + .8, DARK, .2)
        m.rect(cx - 2.0, 16 - 4, cx - 1.4, ty0, METAL, 0.6); m.rect(cx + 1.4, 16 - 4, cx + 2.0, ty0, METAL, 0.6)
        m.ell(cx - 1.6, ty1 - 3.2, cx + 1.6, ty1 - .2, METAL, 0.66, bevel=.3)                                    # radar dish
    if sp.get('rk'):                                                                                              # T34 Calliope rocket frame
        m.rect(cx - 7.5, ty0 - 3.2, cx + 7.5, ty0 + 0.6, METAL, 0.4, bevel=0.15)
        for xx in np.arange(cx - 7.0, cx + 7.0, 1.0): m.rect(xx, ty0 - 2.8, xx + .5, ty0 + .2, DARK, 0.3)
    if sp.get('sponson'):                                                                                         # M3 Lee 75mm in the right sponson: shown on the hull
        pass
    m.rect(tx0 + max(1.0, tws * .12), ty1 - max(1.6, tl * .2), tx0 + max(1.0, tws * .12) + .9, ty1 - max(1.6, tl * .2) + .9, TEAM, 0.5)   # team marking
    return m
def tank(sp, glen=24, gwid=20):
    """-> (hull 32x32, turret 32x32) palette-index grids"""
    h = hull_model(sp, glen, gwid)
    if sp.get('sponson') == 1:                                                                      # M3 Lee: 75mm sponson gun on the right
        L, W, sc, y0 = _geom(sp, glen, gwid)
        gx = 16 + W / 2 - 4.2; h.rect(gx - 1.6, y0 + 1.0, gx + 1.6, y0 + 4.2, CAMO, 0.6, bevel=.25)
        _barrel(h, gx, max(0.2, y0 - sp['gun'] * sc * .5), y0 + 2.0, 1.4, {})
    return h.reduce(), turret_model(sp, glen, gwid).reduce()
# ------------------------------------------------------------------------------------------------ GBA bank patch
def _gba_dims(hg):
    ys, xs = np.nonzero(hg > 1)
    if not len(ys): return 24, 20
    return int(ys.max() - ys.min() + 1), int(xs.max() - xs.min() + 1)
def patch_tanks(words, names):
    """words: tank_spr (128 x 256 u32) -> words with every spec'd vehicle rebuilt"""
    import veh_ds
    out = list(words)
    for i in range(128):
        nm = names.get(i); sp = SPEC.get(nm)
        hg = veh_ds.grids(words, 1, 256, i * 256)[0]; tg = veh_ds.grids(words, 1, 256, i * 256 + 128)[0]
        if sp is None:
            out[i * 256:i * 256 + 128] = veh_ds.pack(veh_ds.hull(hg, i)); out[i * 256 + 128:i * 256 + 256] = veh_ds.pack(veh_ds.turret(tg, i)); continue
        gl, gw = _gba_dims(hg)
        h, t = tank(sp, gl, gw)
        out[i * 256:i * 256 + 128] = veh_ds.pack(h); out[i * 256 + 128:i * 256 + 256] = veh_ds.pack(t)
    return out
