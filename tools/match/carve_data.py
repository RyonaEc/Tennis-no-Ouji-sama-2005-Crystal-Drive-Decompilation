"""Carve data-only units out of a module's unclaimed `.rodata`/`.data` ranges.

A data unit needs no C: the bytes are read straight out of retail, so the unit is
byte-exact by construction. What decides whether it links is whether the range
can be described by a symbol whose neighbours do not move, which is why the
candidate set is filtered and then accepted by prefix bisection against the whole
ROM digest.

Two steps:

    python tools/match/carve_data.py list <mod> [lo] [hi]
        Write `build/datacands_<mod>.json`: every `kind:data` symbol in the
        module whose distance to the next data symbol is between `lo` and `hi`
        bytes, that no source unit already claims, and that lies wholly inside
        one section. Default 8..2048. Prints the candidate count.

    python tools/match/carve_data.py accept <mod> [budget] [step]
        Write the units, reconfigure and relink, keep every candidate group that
        still reproduces the retail digest, and bisect a failing group down to
        the single range that breaks it. `budget` is seconds (default 600).
        Leaves the accepted units in `src/<mod>/` and the module's delinks.

The delinks baseline comes from `git show HEAD:<delinks>`, so commit or stash
before running `accept`: it rewrites the file from that baseline plus accepted
units. `.rodata` units are `const`, `.data` units are not: dsd places a `const`
object in `.rodata`, so `const` on a `.data` range moves it and breaks the
digest. Both facts are load-bearing.
"""
import importlib.util
import json
import pathlib
import re
import subprocess
import sys
import time


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load('tools/match/fastmatch.py', 'fm')


def delinks_for(mod):
    return pathlib.Path('config/ATOJ/arm9/delinks.txt' if mod == 'arm9'
                        else f'config/ATOJ/arm9/overlays/{mod}/delinks.txt')


def symbols_for(mod):
    return pathlib.Path('config/ATOJ/arm9/symbols.txt' if mod == 'arm9'
                        else f'config/ATOJ/arm9/overlays/{mod}/symbols.txt')


def src_for(mod):
    return pathlib.Path('src/arm9' if mod == 'arm9' else f'src/{mod}')


def candidates(mod, lo=8, hi=2048):
    dl = delinks_for(mod).read_text(encoding='utf-8', errors='ignore').splitlines()
    secs = []
    for line in dl[:12]:
        m = re.search(r'(\.\w+)\s+start:0x(\w+) end:0x(\w+)', line)
        if m:
            secs.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))

    occupied = []
    for i, line in enumerate(dl):
        m = re.search(r'start:0x(\w+) end:0x(\w+)', line)
        if not m:
            continue
        j = i
        while j >= 0 and not dl[j].rstrip().endswith(':'):
            j -= 1
        if j >= 0 and dl[j].strip().startswith('src/'):
            occupied.append((int(m.group(1), 16), int(m.group(2), 16)))

    rows = []
    for line in symbols_for(mod).read_text(encoding='utf-8', errors='ignore').splitlines():
        m = re.match(r'^(\S+)\s+kind:data\(([^)]*)\)\s+addr:0x(\w+)', line.strip())
        if m:
            rows.append((int(m.group(3), 16), m.group(1)))
    rows.sort()

    have = {p.stem for p in src_for(mod).glob('*.c')}
    out = []
    for i, (addr, name) in enumerate(rows):
        end = rows[i + 1][0] if i + 1 < len(rows) else addr
        span = end - addr
        if name in have or not lo <= span <= hi:
            continue
        if any(addr < hi2 and addr + span > lo2 for lo2, hi2 in occupied):
            continue
        sec = next((s for s in secs if s[0] in ('.rodata', '.data')
                    and s[1] <= addr and addr + span <= s[2]), None)
        if not sec:
            continue
        try:
            data = fm.retail(mod, addr, span)
        except Exception:
            continue
        out.append({'name': name, 'addr': addr, 'span': span,
                    'sec': sec[0], 'hex': data.hex()})
    return out


def body_for(c):
    data = bytes.fromhex(c['hex'])
    qual = 'const ' if c['sec'] == '.rodata' else ''
    if c['span'] % 4 == 0 and c['addr'] % 4 == 0:
        vals = ', '.join('0x%08x' % int.from_bytes(data[i:i + 4], 'little')
                         for i in range(0, c['span'], 4))
        return f'{qual}u32 {c["name"]}[{c["span"] // 4}] = {{ {vals} }};\n'
    vals = ', '.join('0x%02x' % b for b in data)
    return f'{qual}u8 {c["name"]}[{c["span"]}] = {{ {vals} }};\n'


def entry_for(mod, c):
    return (f'src/{mod}/{c["name"]}.c:\n    complete\n'
            f'    {c["sec"]:11s} start:0x{c["addr"]:08x} end:0x{c["addr"] + c["span"]:08x}\n')


def write_set(mod, sub, all_names, base):
    src = src_for(mod)
    keep = {c['name'] for c in sub}
    for f in src.glob('*.c'):
        if f.stem in all_names and f.stem not in keep:
            f.unlink()
    for c in sub:
        (src / f'{c["name"]}.c').write_text('#include "types.h"\n\n' + body_for(c),
                                            encoding='utf-8', newline='\n')
    delinks_for(mod).write_text(base + ''.join(entry_for(mod, c) for c in sub),
                                encoding='utf-8', newline='\r\n')


def build(mod, sub, all_names, base):
    write_set(mod, sub, all_names, base)
    subprocess.run('python tools/configure.py ATOJ', shell=True, capture_output=True)
    r = subprocess.run('ninja sha1', shell=True, capture_output=True, text=True)
    return 'tennis_ATOJ.nds: OK' in r.stdout


def accept(mod, budget=600.0, step=80):
    path = pathlib.Path(f'build/datacands_{mod}.json')
    cands = json.loads(path.read_text())
    all_names = {c['name'] for c in cands}
    base = subprocess.run(f'git show HEAD:{delinks_for(mod).as_posix()}',
                          shell=True, capture_output=True, text=True).stdout.replace('\r', '')
    if not base.strip():
        raise SystemExit(f'no committed delinks for {mod}')
    t0 = time.time()
    if not build(mod, [], all_names, base):
        raise SystemExit(f'{mod}: baseline tree does not reproduce the digest')

    good, i = [], 0
    while i < len(cands) and time.time() - t0 < budget:
        hi = min(i + step, len(cands))
        if build(mod, good + cands[i:hi], all_names, base):
            good += cands[i:hi]
            i = hi
        else:
            lo = i
            while hi - lo > 1:
                mid = (lo + hi) // 2
                if build(mod, good + cands[i:mid], all_names, base):
                    lo = mid
                else:
                    hi = mid
            if lo > i:
                good += cands[i:lo]
                i = lo
            else:
                print(f'  skip {cands[i]["name"]} @0x{cands[i]["addr"]:08x}', flush=True)
                i += 1
        print(f'{mod} {i}/{len(cands)} accepted={len(good)}', flush=True)

    ok = build(mod, good, all_names, base)
    print(f'{mod}: KEPT {len(good)} units, {sum(c["span"] for c in good)} bytes, '
          f'digest ok={ok}, skipped {len(cands) - len(good)}, {int(time.time() - t0)}s')
    pathlib.Path(f'build/accepted_{mod}.json').write_text(json.dumps(good))
    return 0 if ok else 1


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__.strip().splitlines()[-1])
    cmd, mod = sys.argv[1], sys.argv[2]
    if cmd == 'list':
        lo = int(sys.argv[3]) if len(sys.argv) > 3 else 8
        hi = int(sys.argv[4]) if len(sys.argv) > 4 else 2048
        out = candidates(mod, lo, hi)
        pathlib.Path(f'build/datacands_{mod}.json').write_text(json.dumps(out))
        print(f'{mod}: {len(out)} candidates, {sum(c["span"] for c in out)} bytes '
              f'(span {lo}..{hi})')
        for c in out[:10]:
            print(f'  {c["addr"]:08x} {c["span"]:5d} {c["sec"]:8s} {c["name"]}')
        return 0
    if cmd == 'accept':
        budget = float(sys.argv[3]) if len(sys.argv) > 3 else 600.0
        step = int(sys.argv[4]) if len(sys.argv) > 4 else 80
        return accept(mod, budget, step)
    raise SystemExit(f'unknown command {cmd!r}')


if __name__ == '__main__':
    sys.exit(main())
