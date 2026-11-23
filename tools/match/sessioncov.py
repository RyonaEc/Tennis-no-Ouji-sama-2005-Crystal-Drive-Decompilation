"""Executed-function coverage from a recorded workbench session, offline.

The live API only answers while the emulator runs, and it resets an overlay's
coverage when that overlay is reloaded. A `.dswbs` recording has neither limit:
it is the whole run, every resident overlay kept apart, so this is the tool to
use once a session has been saved.

    python tools/match/sessioncov.py <session.dswbs> [--json out.json] [--top N]

It reports, per module, how many still-unmatched functions the recording
executed and how many bytes of their bodies ran, then lists the largest. With
--json it also writes per-function detail (bytes executed, call edges in and
out) so matching work can be ordered by evidence instead of size alone.

Coverage is evidence, never a gate: `ninja sha1` decides a match.

The reader is the workbench's own client package, outside this repository; point
DSWB_PY at it (default M:/melonDS-workbench/pydswb).
"""
import argparse
import importlib.util
import json
import os
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path[:] = [p for p in sys.path
               if not p or pathlib.Path(p).resolve() != REPO / 'tools/match']

WORKBENCH_PY = pathlib.Path(os.environ.get('DSWB_PY', 'M:/melonDS-workbench/pydswb'))
sys.path.insert(0, str(WORKBENCH_PY))
from dswb import session as dswb_session          # noqa: E402
from dswb._generated import Region                # noqa: E402

spec = importlib.util.spec_from_file_location('fm', str(REPO / 'tools/match/fastmatch.py'))
fm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fm)

RAM_BASE = 0x02000000         # records are keyed by module offset, not physical address
MODULES = ('arm9', 'ov002', 'ov003', 'ov004', 'ov005', 'ov006', 'ov008', 'ov009', 'ov010')
PAGE = 4096


def exec_intervals(sess):
    """{module: [(lo, hi)]} executed byte ranges, merged, per module."""
    per = {}
    for region, page, raw in sess.pages():
        if region == Region.MainRAM:
            mod, base = 'arm9', RAM_BASE + page * PAGE
        elif Region.OverlayARM9Base <= region < Region.OverlayARM7Base:
            ov = region - Region.OverlayARM9Base
            mod, base = f'ov{ov:03d}', RAM_BASE + page * PAGE
        else:
            continue
        runs, start = [], None
        for i in range(PAGE):
            hit = int.from_bytes(raw[i * 32:i * 32 + 4], 'little') > 0
            if hit and start is None:
                start = i
            elif not hit and start is not None:
                runs.append((base + start, base + i))
                start = None
        if start is not None:
            runs.append((base + start, base + PAGE))
        if runs:
            per.setdefault(mod, []).extend(runs)
    return {m: merge(sorted(r)) for m, r in per.items()}


def merge(runs):
    out = []
    for lo, hi in runs:
        if out and lo <= out[-1][1]:
            out[-1] = (out[-1][0], max(out[-1][1], hi))
        else:
            out.append((lo, hi))
    return out


def overlap(lo, hi, runs):
    """Bytes of [lo, hi) covered by runs (which are sorted and disjoint)."""
    total, i = 0, 0
    for a, b in runs:
        if b <= lo:
            continue
        if a >= hi:
            break
        total += max(0, min(b, hi) - max(a, lo))
    return total


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('session')
    ap.add_argument('--json')
    ap.add_argument('--top', type=int, default=15)
    ap.add_argument('--all-functions', action='store_true',
                    help='include functions that are already matched')
    args = ap.parse_args(argv)

    sess = dswb_session.load(args.session)
    print(f'session {pathlib.Path(args.session).name}: rom {sess.rom_sha1[:12]} '
          f'frames {sess.header.frame_start}-{sess.header.frame_end}')

    runs = exec_intervals(sess)
    print('executed: ' + ', '.join(f'{m} {sum(b - a for a, b in r):,} B'
                                   for m, r in sorted(runs.items())))

    edges = sess.call_edges()
    incoming, outgoing = {}, {}
    for e in edges:
        incoming[e.callee] = incoming.get(e.callee, 0) + e.count
        outgoing[e.caller_func] = outgoing.get(e.caller_func, 0) + e.count

    rows = []
    for mod in MODULES:
        if mod not in runs:
            continue
        fm.auto.set_module(mod)
        have = {p.stem for p in (REPO / ('src/arm9' if mod == 'arm9' else f'src/{mod}')).glob('*.c')}
        for addr, (name, size) in fm.auto.FUNCS.items():
            if name in have:
                if not args.all_functions:
                    continue
            hit = overlap(addr, addr + size, runs[mod])
            if hit:
                rows.append(dict(module=mod, addr=addr, name=name, size=size,
                                 bytes_executed=hit,
                                 calls_in=incoming.get(addr, 0),
                                 calls_out=outgoing.get(addr, 0),
                                 matched=name in have))
    rows.sort(key=lambda r: -r['size'])

    for mod in MODULES:
        rs = [r for r in rows if r['module'] == mod]
        if rs:
            print(f'  {mod:6s} {len(rs):4d} fns {sum(r["size"] for r in rs):8,d} B'
                  f'  ({sum(r["bytes_executed"] for r in rs):,d} B of body executed)')
    print(f'total {len(rows)} fns {sum(r["size"] for r in rows):,d} B')

    print(f'\nlargest {args.top}:')
    for r in rows[:args.top]:
        note = '' if r['bytes_executed'] == r['size'] else f"  partial {r['bytes_executed']}B"
        print(f'  {r["module"]:6s} 0x{r["addr"]:08x} {r["name"]:32s} {r["size"]:6d} B'
              f'  in={r["calls_in"]:6d} out={r["calls_out"]:6d}{note}')

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(rows, indent=0))
        print(f'wrote {args.json}')


if __name__ == '__main__':
    main(sys.argv[1:])
