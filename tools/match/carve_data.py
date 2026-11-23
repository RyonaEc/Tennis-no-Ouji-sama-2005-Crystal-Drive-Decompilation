"""Carve data-only units out of a module's unclaimed `.rodata`/`.data` ranges.

A data unit needs no C: the bytes are read straight out of retail, so the unit is
byte-exact by construction. What decides whether it links is whether the range
can be described by a symbol whose neighbours do not move, which is why the
candidate set is filtered and then accepted by prefix bisection against the whole
ROM digest.

    python tools/match/carve_data.py gaps <mod> [minlen]
        Write every carveable stretch of unclaimed space as one unit of its own.
        The linker pads a section start to 4, so a stretch must be word aligned
        at both ends. It is cut at any interior address that has to stay a real
        symbol (dsd range bookkeeping, or a function unit's reference); names
        only carved data units use are dropped, and `accept` rewrites those
        `(u32)&name` references to the plain address. This is how the packed
        Shift-JIS tables are carved without moving a byte.

Steps:

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
        Reads the candidates written by `runs`, `list` or `gaps` (whichever ran
        last), so regenerate them first when the configuration changed.

The delinks baseline comes from `git show HEAD:<delinks>`, so commit or stash
before running `accept`: it rewrites the file from that baseline plus accepted
units. `.rodata` units are `const`, `.data` units are not: dsd places a `const`
object in `.rodata`, so `const` on a `.data` range moves it and breaks the
digest. Both facts are load-bearing.
"""
import bisect
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


def relocs_for(mod):
    return pathlib.Path('config/ATOJ/arm9/relocs.txt' if mod == 'arm9'
                        else f'config/ATOJ/arm9/overlays/{mod}/relocs.txt')


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


def all_modules():
    mods = ['arm9']
    for d in sorted(pathlib.Path('config/ATOJ/arm9/overlays').iterdir()):
        if (d / 'delinks.txt').exists():
            mods.append(d.name)
    return mods


_MUST = None


_MODSECTS = None


def modules_in(expr):
    """Modules a relocs `module:` field can denote.

    Overlays load over one another, so their address ranges overlap and an
    address alone cannot name the module; the field can list several
    (`overlays(1,2,3)`), in which case the one that owns the symbol is chosen by
    the caller.
    """
    if expr == 'main':
        return ['arm9']
    if expr in ('itcm', 'dtcm', 'none'):
        return []
    m = re.match(r'overlays?\(([\d,]+)\)', expr)
    if not m:
        return []
    return [f'ov{int(n):03d}' for n in m.group(1).split(',')]


def must_symbols():
    """Addresses that have to stay real symbols at their exact address.

    Two kinds: a symbol a delinked gap object still needs (assembly we do not
    own), and a data symbol mentioned by a function unit (we cannot rewrite C
    there). For a relocation the delinker kept as `symbol + addend`, what has to
    survive is the symbol, not the addend address, so the base symbol is what is
    recorded; dsd then also agrees that the symbol ends at the next symbol.
    Every other interior name is only used by a carved data unit, whose
    `(u32)&name` can become a plain address with the same bytes.
    """
    global _MUST
    if _MUST is not None:
        return _MUST
    mods = all_modules()
    syms = {m: sorted(data_symbols(m)) for m in mods}
    addrs = {m: [a for a, _ in syms[m]] for m in mods}
    must = set()
    for m in mods:
        occ = occupied(m)
        path = relocs_for(m)
        if not path.exists():
            continue
        for line in path.read_text(encoding='utf-8', errors='ignore').splitlines():
            r = re.match(r'from:0x(\w+) kind:\w+ to:0x(\w+)', line.strip())
            if not r:
                continue
            frm, to = int(r.group(1), 16), int(r.group(2), 16)
            if any(a <= frm < b for a, b in occ):
                continue
            fm = re.search(r'module:(\S+)', line)
            cands = modules_in(fm.group(1)) if fm else []
            cands = [c for c in cands if addrs.get(c)]
            if not cands:
                continue
            exact = [c for c in cands if to in addrs[c]]
            tm = exact[0] if exact else cands[0]
            i = bisect.bisect_right(addrs[tm], to) - 1
            if i >= 0:
                must.add(syms[tm][i][0])
    funcs = ['\n'.join(l for l in f.read_text(encoding='utf-8', errors='ignore')
                      .splitlines() if not l.strip().startswith('extern '))
             for f in pathlib.Path('src').rglob('func_*.c')]
    for m in mods:
        for a, n in data_symbols(m):
            pat = re.compile(r'\b' + re.escape(n) + r'\b')
            if any(pat.search(t) for t in funcs):
                must.add(a)
    _MUST = must
    return must


def whole(mod, minlen=16):
    """One candidate per carveable stretch of an unclaimed gap.

    Packed string tables are odd-addressed and odd-length, and the linker pads
    every section to 4, so a unit must start and end on a word boundary. A gap
    is cut at every interior address that has to stay a symbol of its own
    (`must_symbols`); each remaining stretch becomes a single array holding
    exactly its retail bytes. Interior names that only carved data units
    reference are dropped from the array and their `(u32)&name` references are
    rewritten to plain addresses by `accept`.
    """
    must = must_symbols()
    secs = module_sections(mod)
    occ = occupied(mod)
    syms = data_symbols(mod)
    names = {a: n for a, n in syms}
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
            ms = sorted(a for a in must if s < a < e)
            bounds = [s] + [a for a in ms if a % 4 == 0] + [e]
            for b0, b1 in zip(bounds, bounds[1:]):
                if b1 - b0 < minlen:
                    continue
                if any(b0 < a < b1 for a in ms):
                    continue  # an unaligned required symbol inside: cannot place
                name = names.get(b0) or (f'data_{mod}_{b0:08x}' if mod != 'arm9'
                                         else f'data_{b0:08x}')
                cands.append({
                    'name': name, 'addr': b0, 'span': b1 - b0, 'sec': sec,
                    'inner': [{'addr': a, 'name': n} for a, n in syms
                              if b0 < a < b1 and n != name],
                    'parts': [{'name': name, 'addr': b0, 'span': b1 - b0,
                               'hex': fm.retail(mod, b0, b1 - b0).hex()}]})
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


class Desym:
    """Rewrite data-unit `(u32)&name` references for names a unit will absorb.

    A whole-gap array does not define the interior names, so their references
    must become plain addresses. The bytes are unchanged, because the address is
    fixed; function units are never touched (whole() skips those gaps).
    """

    def __init__(self):
        self.orig = {}

    def apply(self, inner, skip=frozenset()):
        subs = {i['name']: i['addr'] for i in inner}
        for f in pathlib.Path('src').rglob('data_*.c'):
            if f.stem in skip:
                continue
            txt = self.orig.get(f)
            if txt is None:
                txt = f.read_text(encoding='utf-8', errors='ignore')
                self.orig[f] = txt
            new = txt
            for n, a in subs.items():
                new = re.sub(r'\(u32\)\s*&' + re.escape(n) + r'\b',
                             f'0x{a:08x}', new)
            if f.read_text(encoding='utf-8', errors='ignore') != new:
                f.write_text(new, encoding='utf-8', newline='\n')


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


def build(mod, sub, all_names, base, desym):
    write_set(mod, sub, all_names, base)
    desym.apply([i for c in sub for i in c.get('inner', [])], skip=all_names)
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
    desym = Desym()
    t0 = time.time()
    if not build(mod, [], all_names, base, desym):
        raise SystemExit(f'{mod}: baseline tree does not reproduce the digest')

    good, i = [], 0
    while i < len(cands) and time.time() - t0 < budget:
        hi = min(i + step, len(cands))
        if build(mod, good + cands[i:hi], all_names, base, desym):
            good += cands[i:hi]
            i = hi
        else:
            lo = i
            while hi - lo > 1:
                mid = (lo + hi) // 2
                if build(mod, good + cands[i:mid], all_names, base, desym):
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

    ok = build(mod, good, all_names, base, desym)
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
