"""Ask the melonDS workbench which unmatched functions ever ran.

The digest cannot say whether a unit is *alive*: a parked wall may be dead code
that ordinary play never reaches, in which case runtime evidence will never
arrive and a plausible shape cannot be confirmed by playing. This ranks the
unmatched queue by what the recorded sessions actually executed, so effort goes
to code the game runs.

    python tools/match/coverage.py [--top N] [--rpcdir PATH] [mod ...]

Needs the workbench running with `--api` (see `notes/workbench.md`); the
coverage it reports is cumulative over the sessions in its knowledge store, not
just the current frame. It never decides a match - `ninja sha1` still does.
"""
import importlib.util
import os
import pathlib
import re
import sys

DEFAULT_RPC = pathlib.Path('M:/melonDS-workbench/pydswb')
DEFAULT_API = pathlib.Path('M:/melonDS-workbench/workbench/api.json')


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def text_range(mod):
    p = pathlib.Path('config/ATOJ/arm9/delinks.txt' if mod == 'arm9'
                     else f'config/ATOJ/arm9/overlays/{mod}/delinks.txt')
    for line in p.read_text(encoding='utf-8', errors='ignore').splitlines()[:12]:
        m = re.search(r'\.text\s+start:0x(\w+) end:0x(\w+)', line)
        if m:
            return int(m.group(1), 16), int(m.group(2), 16)
    return None


def main():
    args = sys.argv[1:]
    top, rpcdir = 20, DEFAULT_RPC
    mods = []
    i = 0
    while i < len(args):
        if args[i] == '--top':
            top = int(args[i + 1]); i += 2
        elif args[i] == '--rpcdir':
            rpcdir = pathlib.Path(args[i + 1]); i += 2
        else:
            mods.append(args[i]); i += 1
    # `tools/match/queue.py` shadows the stdlib module the client needs.
    here = pathlib.Path(__file__).resolve().parent
    sys.path[:] = [p for p in sys.path
                   if not p or pathlib.Path(p).resolve() != here]
    if str(rpcdir) not in sys.path:
        sys.path.insert(0, str(rpcdir))
    api = rpcdir.parent / 'workbench' / 'api.json'
    os.environ.setdefault('DSWB_API_INFO', str(api if api.exists() else DEFAULT_API))
    from dswb import Client

    fm = load('tools/match/fastmatch.py', 'fm')
    client = Client.connect(agent='buffy')
    st = client.status_get()
    print(f"session frame {st.frame}  recording={st.recording.enabled}  "
          f"history={st.recording.frames_recorded} frames")
    mods = mods or ['arm9', 'ov002', 'ov003', 'ov004', 'ov005', 'ov006', 'ov008',
                    'ov009', 'ov010']

    rows = []
    for mod in mods:
        rng = text_range(mod)
        if rng is None:
            print(f'{mod:6s} no .text')
            continue
        start, end = rng
        fm.auto.set_module(mod)
        try:
            cov = client.coverage_get(start, size=end - start, kind='exec')
        except Exception as exc:                      # noqa: BLE001 - reported
            print(f'{mod:6s} coverage query failed: {exc}')
            continue
        live = set()
        for r in cov.ranges:
            live.update(range(r.start, min(r.end, start + (end - start))))
        srcdir = pathlib.Path('src/arm9' if mod == 'arm9' else f'src/{mod}')
        have = {p.stem for p in srcdir.glob('*.c')}
        un_total = un_bytes = un_live = un_live_bytes = 0
        for addr, (name, size) in fm.auto.FUNCS.items():
            if name in have or size <= 0:
                continue
            un_total += 1
            un_bytes += size
            if addr in live:
                un_live += 1
                un_live_bytes += size
                rows.append((size, mod, addr, name))
        print(f'{mod:6s} unmatched {un_total:5d} functions / {un_bytes:8d} B   '
              f'executed {un_live:5d} / {un_live_bytes:8d} B')

    rows.sort(reverse=True)
    print(f'\ntop {top} executed unmatched functions (largest first):')
    for size, mod, addr, name in rows[:top]:
        print(f'  {size:5d} B  {mod:6s} 0x{addr:08x} {name}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
