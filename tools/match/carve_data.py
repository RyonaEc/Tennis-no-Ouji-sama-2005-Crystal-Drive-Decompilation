"""Carve data-only units out of a module's unclaimed `.rodata`/`.data` ranges.

A data unit needs no C: the bytes are read straight out of retail, so the unit is
byte-exact by construction. What decides whether it links is whether the range
can be described by a symbol whose neighbours do not move, which is why the
candidate set is filtered and then accepted by prefix bisection against the whole
ROM digest.

Two steps:

    python tools/match/carve_data.py runs <mod> [minlen]
        Write the unclaimed space between claimed units as candidates, clamped
        to word boundaries. `accept` then treats them like any other candidate.

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


def module_sections(mod):
    dl = delinks_for(mod).read_text(encoding='utf-8', errors='ignore').splitlines()
    secs = []
    for line in dl[:14]:
        m = re.search(r'(\.\w+)\s+start:0x(\w+) end:0x(\w+)', line)
        if m and m.group(1) in ('.rodata', '.data'):
            secs.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return secs


def occupied(mod):
    dl = delinks_for(mod).read_text(encoding='utf-8', errors='ignore').splitlines()
    out = []
    for i, line in enumerate(dl):
        m = re.search(r'start:0x(\w+) end:0x(\w+)', line)
        if not m:
            continue
        j = i
        while j >= 0 and not dl[j].rstrip().endswith(':'):
            j -= 1
        if j >= 0 and dl[j].strip().startswith('src/'):
            out.append((int(m.group(1), 16), int(m.group(2), 16)))
    return sorted(out)


def data_symbols(mod):
    rows = []
    for line in symbols_for(mod).read_text(encoding='utf-8', errors='ignore').splitlines():
        m = re.match(r'^(\S+)\s+kind:data\(([^)]*)\)\s+addr:0x(\w+)', line.strip())
        if m:
            rows.append((int(m.group(3), 16), m.group(1)))
    rows.sort()
    return rows


def runs(mod, minlen=16):
    """Candidates for the space between claimed units.

    Data symbol ranges usually end on an odd address, and a section cannot be
    placed there without shifting everything after it (measured: an unaligned
    range changes the digest). The unclaimed space between units can still be
    carved from its aligned interior, which leaves at most three bytes at each
    end behind.
    """
    occ = occupied(mod)
    out = []
    for sec, lo, hi in module_sections(mod):
        cur = lo
        for a, b in occ:
            if b <= lo or a >= hi:
                continue
            if a > cur:
                out.append((cur, min(a, hi)))
            cur = max(cur, b)
        if cur < hi:
            out.append((cur, hi))
    syms = data_symbols(mod)
    cands = []
    for g0, g1 in out:
        s = (g0 + 3) & ~3
        e = g1 & ~3
        if e - s < minlen:
            continue
        sec = next((n for n, a, b in module_sections(mod)
                    if a <= s and e <= b), None)
        if not sec:
            continue
        # Every data symbol that starts in this run has to be defined by the
        # unit, or the link fails with an undefined symbol referenced by a
        # neighbour. sizes come from the next symbol, the last one from the run.
        inside = [(a, n) for a, n in syms if s <= a < e]
        parts = []
        for i, (a, n) in enumerate(inside):
            end = inside[i + 1][0] if i + 1 < len(inside) else e
            parts.append({'name': n, 'addr': a, 'span': end - a,
                          'hex': fm.retail(mod, a, end - a).hex()})
        if not parts:
            parts = [{'name': f'data_{mod}_{s:08x}' if mod != 'arm9' else f'data_{s:08x}',
                      'addr': s, 'span': e - s, 'hex': fm.retail(mod, s, e - s).hex()}]
        # A symbol at an odd or half-word address makes the linker shift the
        # data that follows it (measured: ov006 0x02076024, symbol
        # data_ov006_0207623b at 0x0207623b, 1665 bytes differ in 109 runs, each
        # record gaining a leading 00). Such a symbol cannot be covered, but the
        # aligned stretches on either side of it can, so split the run at it.
        bad = [p for p in parts if p['addr'] % 4 or p['span'] % 4]
        groups, cur = [], []
        for p in parts:
            if p in bad:
                if cur:
                    groups.append(cur)
                cur = []
            else:
                cur.append(p)
        if cur:
            groups.append(cur)
        for g in groups:
            s2 = g[0]['addr']
            e2 = g[-1]['addr'] + g[-1]['span']
            if s2 % 4 or e2 % 4 or e2 - s2 < minlen:
                continue
            cands.append({'name': g[0]['name'], 'addr': s2, 'span': e2 - s2,
                          'sec': sec, 'parts': g,
                          'hex': ''.join(p['hex'] for p in g)})
    return cands


def whole(mod, minlen=16):
    """One candidate per whole unclaimed gap, strings at their natural offsets.

    Packed string tables are odd-addressed and odd-length with no aligned
    interior, so `runs` cannot take them (measured: a unit starting on an odd
    address rebases 122,543 bytes). The gap between two claimed units is usually
    word aligned at both ends, though, so the whole gap can be one unit whose
    parts keep their retail offsets inside a single section.
    """
    secs = module_sections(mod)
    occ = occupied(mod)
    syms = data_symbols(mod)
    cands = []
    for sec, lo, hi in secs:
        gaps, cur = [], lo
        for a, b in occ:
            if b <= lo or a >= hi:
                continue
            if a > cur:
                gaps.append((cur, min(a, hi)))
            cur = max(cur, b)
        if cur < hi:
            gaps.append((cur, hi))
        for g0, g1 in gaps:
            s, e = (g0 + 3) & ~3, g1 & ~3
            if e - s < minlen:
                continue
            inside = [(a, n) for a, n in syms if s <= a < e]
            parts = []
            if not inside or inside[0][0] > s:
                parts.append({'name': (f'data_{mod}_{s:08x}' if mod != 'arm9'
                                       else f'data_{s:08x}'),
                              'addr': s,
                              'span': (inside[0][0] if inside else e) - s})
            for i, (a, n) in enumerate(inside):
                end = inside[i + 1][0] if i + 1 < len(inside) else e
                parts.append({'name': n, 'addr': a, 'span': end - a})
            for p in parts:
                p['hex'] = fm.retail(mod, p['addr'], p['span']).hex()
            cands.append({'name': parts[0]['name'], 'addr': s, 'span': e - s,
                          'sec': sec, 'parts': parts,
                          'hex': ''.join(p['hex'] for p in parts)})
    return cands


def body_for(c):
    parts = c.get('parts') or [c]
    qual = 'const ' if c['sec'] == '.rodata' else ''
    out = ''
    for p in parts:
        data = bytes.fromhex(p['hex'])
        if p['span'] % 4 == 0 and p['addr'] % 4 == 0:
            vals = ', '.join('0x%08x' % int.from_bytes(data[i:i + 4], 'little')
                             for i in range(0, p['span'], 4))
            out += f'{qual}u32 {p["name"]}[{p["span"] // 4}] = {{ {vals} }};\n'
        else:
            vals = ', '.join('0x%02x' % b for b in data)
            out += f'{qual}u8 {p["name"]}[{p["span"]}] = {{ {vals} }};\n'
    return out


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
    # A range whose address or length is not word aligned cannot be placed: dsd
    # would have to shift every byte after it. Measured on arm9: 13 of 13 such
    # ranges failed, so they are dropped here rather than bisected one by one.
    unaligned = [c for c in cands if c['addr'] % 4 or c['span'] % 4]
    cands = [c for c in cands if not (c['addr'] % 4 or c['span'] % 4)]
    if unaligned:
        print(f'  {len(unaligned)} unaligned ranges dropped '
              f'({sum(c["span"] for c in unaligned)} bytes)', flush=True)
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
    if cmd == 'runs':
        minlen = int(sys.argv[3]) if len(sys.argv) > 3 else 16
        cands = runs(mod, minlen)
        pathlib.Path(f'build/datacands_{mod}.json').write_text(json.dumps(cands))
        print(f'{mod}: {len(cands)} runs, {sum(c["span"] for c in cands)} bytes '
              f'(min {minlen})')
        for c in cands[:12]:
            print(f'  {c["addr"]:08x} {c["span"]:6d} {c["sec"]:8s}  {c["name"]}')
        return 0
    if cmd == 'gaps':
        minlen = int(sys.argv[3]) if len(sys.argv) > 3 else 16
        cands = whole(mod, minlen)
        pathlib.Path(f'build/datacands_{mod}.json').write_text(json.dumps(cands))
        print(f'{mod}: {len(cands)} whole gaps, {sum(c["span"] for c in cands)} bytes '
              f'(min {minlen})')
        for c in cands[:12]:
            print(f'  {c["addr"]:08x} {c["span"]:6d} {c["sec"]:8s}  {c["name"]} '
                  f'({len(c["parts"])} parts)')
        return 0
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
