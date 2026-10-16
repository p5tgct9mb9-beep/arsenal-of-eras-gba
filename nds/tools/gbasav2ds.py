#!/usr/bin/env python3
"""gbasav2ds.py - convert an Arsenal of Eras GBA save (.sav, 32/64 KB SRAM) into a DS save
(8 KB EEPROM image) that the DS edition loads directly; the game then runs its normal save
migration. Also converts back (--to-gba) for debugging.
usage: gbasav2ds.py in.sav out.sav   |   gbasav2ds.py --to-gba ds.sav out_gba.sav"""
import sys, struct
SLOT, HDR = 0x1000, 16
def sum32(b):
    s = 0x41454F44
    for x in b: s = (((s << 5) | (s >> 27)) & 0xFFFFFFFF) ^ x
    return s
def rle_pack(d):
    o = bytearray(); i = 0; n = len(d)
    while i < n:
        r = 1
        while i + r < n and r < 130 and d[i + r] == d[i]: r += 1
        if r >= 3: o += bytes([0x80 | (r - 3), d[i]]); i += r; continue
        s = i; l = 0
        while i < n and l < 128:
            rr = 1
            while i + rr < n and rr < 3 and d[i + rr] == d[i]: rr += 1
            if rr >= 3: break
            i += 1; l += 1
        o.append(l - 1); o += d[s:s + l]
    return bytes(o)
def rle_unpack(b):
    o = bytearray(); i = 0
    while i < len(b):
        c = b[i]; i += 1
        if c & 0x80: o += bytes([b[i]]) * ((c & 0x7F) + 3); i += 1
        else: o += b[i:i + c + 1]; i += c + 1
    return bytes(o)
def to_ds(gba):
    img = bytearray(b'\xff' * 0x10000); img[:len(gba)] = gba[:0x10000]
    z = rle_pack(bytes(img))
    if len(z) > SLOT - HDR: raise SystemExit('save too big for the 8 KB DS chip (%d bytes compressed)' % len(z))
    out = bytearray(b'\xff' * (2 * SLOT))
    out[0:HDR] = b'AOD1' + struct.pack('<III', 1, len(z), sum32(z)); out[HDR:HDR + len(z)] = z
    return bytes(out), len(z)
def from_ds(ds):
    best = None
    for s in (0, 1):
        h = ds[s * SLOT:s * SLOT + HDR]
        if h[:4] != b'AOD1': continue
        q, n, cs = struct.unpack('<III', h[4:16]); z = ds[s * SLOT + HDR:s * SLOT + HDR + n]
        if sum32(z) == cs and (best is None or q >= best[0]): best = (q, z)
    if not best: raise SystemExit('no valid DS save slot')
    return rle_unpack(best[1])[:0x8000]
if __name__ == '__main__':
    a = sys.argv[1:]
    if a and a[0] == '--to-gba': open(a[2], 'wb').write(from_ds(open(a[1], 'rb').read()))
    elif len(a) == 2:
        d, n = to_ds(open(a[0], 'rb').read()); open(a[1], 'wb').write(d); print('ok: %d bytes compressed into the 8 KB DS save' % n)
    else: print(__doc__)
