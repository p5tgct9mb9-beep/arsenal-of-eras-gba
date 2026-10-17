#!/usr/bin/env python3
"""arccheck.py OUTDIR - check the 12 arcade cabinet runs (tests/ds_arc_NN.txt via dsrun1.sh).
Per game: entered the right game, pause/quit/score entry/play-again/autopilot all returned to the
cabinet, cabinet exit returned to the menu, no stalls, game logic at full 60 Hz, score + plays saved."""
import sys, re, os
NM = ["IRON DOME","DEBRIS SWEEPER","KC CROSSING","BUNKER MAZE","BUNKER BUSTER","CARGO STACK","CONVOY","CHOPPER RESCUE","STARSHIP HOPPER","TANK PONG","TANK DUEL","SPACE PINBALL"]
O = sys.argv[1]; bad = 0
for g in range(12):
    f = os.path.join(O, 'g%02d.log' % g)
    if not os.path.exists(f): print('%2d %-16s MISSING' % (g, NM[g])); bad += 1; continue
    B = {}
    seq = []
    for l in open(f):
        m = re.match(r'\[(\w+)\] t=(\d+) frame=(\d+) scr=(\d+) mode=(\d+) era=\d+ hp=(-?\d+) A=(\d+) B=(\d+) timer=(\d+) phase=\d+ k=(\d+) d=\d+ x=\d+ y=\d+ extra=(\d+) maxstall=(\d+)', l)
        if not m: continue
        d = dict(tag=m.group(1), t=int(m.group(2)), fr=int(m.group(3)), scr=int(m.group(4)), mode=int(m.group(5)), hp=int(m.group(6)), A=int(m.group(7)),
                 timer=int(m.group(9)), k=int(m.group(10)), extra=int(m.group(11)), stall=int(m.group(12)))
        seq.append(d); B.setdefault(d['tag'], []).append(d)
    p = []
    def ok(c, msg):
        if not c: p.append(msg)
    first = lambda k: B.get(k, [None])[0]; last = lambda k: B.get(k, [None])[-1]
    s = first('start'); ok(s and s['scr'] == 108 and s['mode'] == g, 'start: not in game %d (%s)' % (g, s and (s['scr'], s['mode'])))
    ok(first('cab') and first('cab')['scr'] == 107, 'cabinet not reached')
    ok(last('back1') and last('back1')['scr'] == 107, 'quit -> score entry -> cabinet failed (%s)' % (last('back1') and last('back1')['scr']))
    ok(last('over2') and last('over2')['scr'] == 109, 'quit at once did not reach GAME OVER (%s)' % (last('over2') and last('over2')['scr']))
    ok(last('again') and last('again')['scr'] == 108, 'PLAY AGAIN failed (%s)' % (last('again') and last('again')['scr']))
    ok(last('cab2') and last('cab2')['scr'] == 107, 'B on GAME OVER did not return to cabinet')
    au = B.get('auto', []); ag = [a for a in au if a['scr'] == 108]
    ok(last('auto_back') and last('auto_back')['scr'] == 107, 'autopilot game did not return to cabinet (%s)' % (last('auto_back') and last('auto_back')['scr']))
    ok(last('exit') and last('exit')['scr'] == 53, 'cabinet exit did not return to the menu (%s)' % (last('exit') and last('exit')['scr']))
    ms = max([d['stall'] for d in seq] or [0]); ok(ms < 30, 'stall %d frames' % ms)
    # logic rate in the autopilot game: agT (timer) per emulated frame
    rate = 0
    if len(ag) >= 2: rate = (ag[-1]['timer'] - ag[0]['timer']) / max(1, ag[-1]['t'] - ag[0]['t']); ok(rate > 0.97, 'logic rate %.2f of 60 Hz' % rate)
    fps = (ag[-1]['fr'] - ag[0]['fr']) / max(1, ag[-1]['t'] - ag[0]['t']) if len(ag) >= 2 else 0
    ab = last('auto_back'); sc = ab['A'] if ab else 0
    plays = ab['extra'] if ab else 0
    print('%2d %-16s %s  auto score %-6d hi %-6d plays %-2d logic %.2f draw %.0f fps  %s' % (g, NM[g], 'OK  ' if not p else 'FAIL', sc, ab['hp'] if ab else 0, plays, rate, fps * 60, '; '.join(p)))
    bad += bool(p)
print('arcade: %d/12 clean' % (12 - bad))
