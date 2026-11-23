"""Replace literal address words in data units with symbol references.

A data unit that stores a pointer as a number links to the right byte and still
scores zero in objdiff, because objdiff compares the relocation target, not only
the value. `relocs.txt` says which words in retail were relocations and where
they pointed; `symbols.txt` gives those addresses their names. This tool joins
the two and rewrites the initialiser so the compiler emits the relocation too.

Two steps:

    python tools/match/symreloc.py scan <mod>...
        Report, per module, how many units and words can be rewritten, and which
        relocation targets have no symbol (those are left alone).

    python tools/match/symreloc.py apply <mod>... [budget]
        Rewrite, relink and keep only the modules whose ROM digest still matches.
        A module that fails is bisected down to the units that broke it and those
        units are restored; `budget` is seconds per module (default 900).

Only word-aligned `u32` units are rewritten. A unit whose stored value is not
the relocation target (a different addend, or a byte array) is reported and left
untouched, because guessing there would change bytes rather than fix relocations.
"""
import pathlib
import re
import subprocess
import sys
import time


def paths_for(mod):
    base = (pathlib.Path('config/ATOJ/arm9') if mod == 'arm9'
            else pathlib.Path(f'config/ATOJ/arm9/overlays/{mod}'))
    return (base / 'delinks.txt', base / 'symbols.txt', base / 'relocs.txt',
            pathlib.Path(f'src/{mod}'))


def units(mod):
    """Every src unit in the module's delinks, with its data sections."""
    delinks, _, _, src = paths_for(mod)
    out = []
    cur = None
    for line in delinks.read_text(encoding='utf-8', errors='ignore').splitlines():
        stripped = line.strip()
        if stripped.startswith('src/') and stripped.endswith(':'):
            cur = {'name': pathlib.Path(stripped[:-1]).stem, 'secs': []}
            out.append(cur)
            continue
        m = re.search(r'\.(data|rodata)\s+start:0x([0-9a-fA-F]+) end:0x([0-9a-fA-F]+)', line)
        if cur is not None and m:
            cur['secs'].append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    for u in out:
        u['path'] = src / f'{u["name"]}.c'
    return out


def symbols(mod):
    _, sym, _, _ = paths_for(mod)
    table = {}
    for line in sym.read_text(encoding='utf-8', errors='ignore').splitlines():
        m = re.match(r'^(\S+)\s+kind:(\w+)(?:\([^)]*\))?\s+addr:0x([0-9a-fA-F]+)', line.strip())
        if m:
            table.setdefault(int(m.group(3), 16), m.group(1))
    return table


def relocs(mod):
    _, _, rel, _ = paths_for(mod)
    out = []
    for line in rel.read_text(encoding='utf-8', errors='ignore').splitlines():
        m = re.match(r'from:0x([0-9a-fA-F]+) kind:(\w+) to:0x([0-9a-fA-F]+)', line.strip())
        if m:
            out.append((int(m.group(1), 16), m.group(2), int(m.group(3), 16)))
    return out


ARRAY = re.compile(r'=\s*\{(.*)\}\s*;', re.S)
WORD = re.compile(r'0x([0-9a-fA-F]{8})\b')


def rewrite(text, start, end, edits, self_name):
    """Return the rewritten unit, or (None, reason)."""
    nl = '\r\n' if '\r\n' in text else '\n'
    m = ARRAY.search(text)
    if not m:
        return None, 'no initialiser'
    decl = text[:m.start()]
    if not re.search(r'\bu32\b', decl):
        return None, 'not a u32 array'
    values = WORD.findall(m.group(1))
    span = end - start
    if len(values) * 4 != span:
        return None, f'{len(values)} words for {span} bytes'
    names = []
    for offset, target, name in edits:
        idx = offset // 4
        if idx >= len(values):
            return None, 'relocation past the end'
        if int(values[idx], 16) != target:
            return None, f'word {idx} is 0x{values[idx]}, not 0x{target:08x}'
        values[idx] = f'(u32)&{name}'
        if name != self_name:
            names.append(name)
    head, tail = text[:m.start()], text[m.end():]
    if names:
        # The externs have to sit above the declaration: it is `u32 name[N]`
        # followed by `= {...}`, so anything inserted at head splits them.
        lines = head.split(nl)
        last = max(i for i, line in enumerate(lines)
                   if line.strip().startswith('#include'))
        block = [''] + [f'extern u8 {n}[];' for n in sorted(set(names))]
        lines[last + 1:last + 1] = block
        head = nl.join(lines)
    def emit(v):
        # findall() keeps the digits only, so the untouched words need their
        # prefix back; symbol expressions already carry their own syntax.
        return v if v.startswith('(') else '0x' + v

    body = (',' + nl).join((', '.join(emit(v) for v in values[i:i + 6])
                            for i in range(0, len(values), 6)))
    return head + '= {' + nl + body + nl + '};' + tail, 'ok'


def plan(mod):
    """[(unit, [(offset, target, name)])] plus the words that cannot be named."""
    by_addr = symbols(mod)
    rel = relocs(mod)
    rows = []
    unresolved = 0
    for u in units(mod):
        if not u['path'].exists() or not u['secs']:
            continue
        if '(u32)&' in u['path'].read_bytes().decode('utf-8', 'ignore'):
            continue  # already symbolised by an earlier run
        for sec, start, end in u['secs']:
            if start % 4 or (end - start) % 4:
                continue
            edits = []
            for frm, kind, to in rel:
                if not start <= frm < end or (frm - start) % 4:
                    continue
                name = by_addr.get(to)
                if not name:
                    unresolved += 1
                    continue
                edits.append((frm - start, to, name))
            if edits:
                rows.append((u, start, end, edits))
    return rows, unresolved


LAST_BUILD = ''


def build():
    """Relink and verify. ninja skips the check target when the ROM is unchanged,
    so an untouched tree is verified directly instead of being read as a pass."""
    global LAST_BUILD
    r = subprocess.run('ninja sha1', shell=True, capture_output=True, text=True)
    LAST_BUILD = r.stdout + r.stderr
    if r.returncode != 0:
        return False
    if 'OK' in r.stdout:
        return True
    v = subprocess.run('python tools/sha1.py tennis_ATOJ.nds -c config/ATOJ/tennis_ATOJ.sha1',
                       shell=True, capture_output=True, text=True)
    return v.returncode == 0 and 'OK' in v.stdout


def apply(mod, budget=900.0, step=8):
    rows, unresolved = plan(mod)
    if not rows:
        print(f'{mod}: nothing to rewrite ({unresolved} unnamed targets)')
        return 0
    originals = {u['path']: u['path'].read_bytes().decode('utf-8')
                 for u, _, _, _ in rows}
    made = {}
    skipped = []
    for u, start, end, edits in rows:
        out, why = rewrite(originals[u['path']], start, end, edits, u['name'])
        if out is None:
            skipped.append((u['name'], why))
        else:
            made[u['path']] = out

    def write(subset):
        # Raw writes: the sources are CRLF on disk and re-encoding them as LF
        # would show up as a whole-file change in git.
        for p, text in originals.items():
            p.write_bytes(text.encode('utf-8'))
        for p, text in subset.items():
            p.write_bytes(text.encode('utf-8'))

    keys = list(made)
    t0 = time.time()
    good = {}
    write({})
    if not build():
        raise SystemExit(f'{mod}: clean tree does not reproduce the digest')
    try:
        i = 0
        while i < len(keys) and time.time() - t0 < budget:
            hi = min(i + step, len(keys))
            trial = dict(good)
            trial.update({k: made[k] for k in keys[i:hi]})
            write(trial)
            if build():
                good = trial
                i = hi
            else:
                lo = i
                while hi - lo > 1:
                    mid = (lo + hi) // 2
                    trial = dict(good)
                    trial.update({k: made[k] for k in keys[i:mid]})
                    write(trial)
                    if build():
                        lo = mid
                    else:
                        hi = mid
                if lo > i:
                    good.update({k: made[k] for k in keys[i:lo]})
                    i = lo
                else:
                    why = ('compile failed' if 'FAILED:' in LAST_BUILD
                           else 'digest changed')
                    skipped.append((keys[i].stem, why))
                    i += 1
            print(f'{mod} {i}/{len(keys)} units accepted={len(good)}', flush=True)
    finally:
        # Whatever happened -- budget, Ctrl-C, a compiler error -- the tree goes
        # back to the last state that reproduced the digest.
        write(good)
    ok = build()
    words = sum(len(edits) for (u, start, end, edits) in rows
                if u['path'] in good)
    print(f'{mod}: KEPT {len(good)} units, {words} symbol words, digest ok={ok}, '
          f'skipped {len(keys) - len(good)} units, {int(time.time() - t0)}s')
    for name, why in skipped:
        print(f'  skip {name}: {why}')
    return 0 if ok else 1


def scan(mod):
    rows, unresolved = plan(mod)
    words = sum(len(e) for _, _, _, e in rows)
    print(f'{mod}: {len(rows)} units, {words} words, {unresolved} unnamed targets')
    return 0


def main():
    if len(sys.argv) < 3:
        raise SystemExit('usage: symreloc.py scan|apply <mod>... [budget]')
    cmd = sys.argv[1]
    args = sys.argv[2:]
    budget = 900.0
    if args and re.fullmatch(r'[\d.]+', args[-1]):
        budget = float(args.pop())
    mods = args
    rc = 0
    for mod in mods:
        if cmd == 'scan':
            rc |= scan(mod)
        elif cmd == 'apply':
            rc |= apply(mod, budget)
        else:
            raise SystemExit(__doc__.strip().splitlines()[0])
    return rc


if __name__ == '__main__':
    sys.exit(main())
