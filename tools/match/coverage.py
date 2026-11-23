"""Ask the melonDS workbench which unmatched functions ever ran.

The digest cannot say whether a unit is *alive*: a parked wall may be dead code
that ordinary play never reaches, in which case runtime evidence will never
arrive and a plausible shape cannot be confirmed by playing. This ranks the
unmatched queue by what the recorded sessions executed, so effort goes to code
the game runs.

    python tools/match/coverage.py [--top N] [--rpcdir PATH] [mod ...]

Needs the workbench running with `--api` (see `notes/workbench.md`); coverage is
cumulative over its knowledge store, not just the current frame. It never
decides a match - `ninja sha1` still does.

Two traps this handles, both measured:

- **Every overlay loads at the same base** (`0x0204c7c0`), so a query over an
  overlay's address range returns whichever overlay is resident *now*. Ranges
  carry an `overlay` id (`-1` for arm9, `N` for ov00N), so the filter has to be
  on that, not on the address.
- **`coverage.get` truncates at 1000 ranges** (`truncated=True`, silently). The
  range is therefore queried in windows, and any window that still truncates is
  reported instead of being counted short.
"""
import importlib.util
import os
import pathlib
import re
import sys

DEFAULT_RPC = pathlib.Path('M:/melonDS-workbench/pydswb')
DEFAULT_API = pathlib.Path('M:/melonDS-workbench/workbench/api.json')
WINDOW = 0x20000          # bytes per coverage.get call
MODULES = ['arm9', 'ov002', 'ov003', 'ov004', 'ov005', 'ov006', 'ov008',
           'ov009', 'ov010']


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def overlay_id(mod):
    return -1 if mod == 'arm9' else int(mod[2:])


def text_range(mod):
    p = pathlib.Path('config/ATOJ/arm9/delinks.txt' if mod == 'arm9'
                     else f'config/ATOJ/arm9/overlays/{mod}/delinks.txt')
    for line in p.read_text(encoding='utf-8', errors='ignore').splitlines()[:12]:
        m = re.search(r'\.text\s+start:0x(\w+) end:0x(\w+)', line)
        if m:
            return int(m.group(1), 16), int(m.group(2), 16)
    return None


def connect(rpcdir=DEFAULT_RPC):
    here = pathlib.Path(__file__).resolve().parent
    sys.path[:] = [p for p in sys.path
                   if not p or pathlib.Path(p).resolve() != here]
    sys.path.insert(0, str(rpcdir))
    api = rpcdir.parent / 'workbench' / 'api.json'
    os.environ.setdefault('DSWB_API_INFO', str(api if api.exists() else DEFAULT_API))
    from dswb import Client
    return Client.connect(agent='buffy')


def executed(client, start, end, window=WINDOW):
    """{overlay: set(address)} for every executed byte in [start, end)."""
    out = {}
    truncated = []
    addr = start
    while addr < end:
        size = min(window, end - addr)
        cov = client.coverage_get(addr, size=size, kind='exec')
        if getattr(cov, 'truncated', False):
            truncated.append((addr, size))
        for r in cov.ranges:
            s = out.setdefault(r.overlay, set())
            lo = max(r.start, addr)
            hi = min(r.end, addr + size)
            if hi > lo:
                s.update(range(lo, hi))
        addr += size
    return out, truncated


def main():
    args = sys.argv[1:]
    top, rpcdir, mods = 20, DEFAULT_RPC, []
    i = 0
    while i < len(args):
        if args[i] == '--top':
            top = int(args[i + 1]); i += 2
        elif args[i] == '--rpcdir':
            rpcdir = pathlib.Path(args[i + 1]); i += 2
        else:
            mods.append(args[i]); i += 1

    fm = load('tools/match/fastmatch.py', 'fm')
    client = connect(rpcdir)
    st = client.status_get()
    print(f'session frame {st.frame}  fps {st.fps}  recording={st.recording.enabled}  '
          f'frames_recorded={st.recording.frames_recorded}')

    rows = []
    for mod in (mods or MODULES):
        rng = text_range(mod)
        if rng is None:
            print(f'{mod:6s} no .text')
            continue
        live, truncated = executed(client, *rng)
        hits = live.get(overlay_id(mod), set())
        fm.auto.set_module(mod)
        srcdir = pathlib.Path('src/arm9' if mod == 'arm9' else f'src/{mod}')
        have = {p.stem for p in srcdir.glob('*.c')}
        un_total = un_bytes = un_live = un_live_bytes = 0
        for addr, (name, size) in fm.auto.FUNCS.items():
            if name in have or size <= 0:
                continue
            un_total += 1
            un_bytes += size
            if addr in hits:
                un_live += 1
                un_live_bytes += size
                rows.append((size, mod, addr, name))
        warn = f'  [{len(truncated)} truncated windows]' if truncated else ''
        print(f'{mod:6s} unmatched {un_total:5d} / {un_bytes:8d} B   '
              f'executed {un_live:5d} / {un_live_bytes:8d} B{warn}')

    rows.sort(reverse=True)
    print(f'\ntop {top} executed unmatched functions (largest first):')
    for size, mod, addr, name in rows[:top]:
        print(f'  {size:5d} B  {mod:6s} 0x{addr:08x} {name}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
