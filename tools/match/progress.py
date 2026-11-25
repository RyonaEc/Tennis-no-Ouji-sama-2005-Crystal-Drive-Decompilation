"""Write PROGRESS.md from the objdiff report.

`ninja report` already measures every unit: matched and total code, matched and
total data, function counts, per-unit completeness and a fuzzy-match
percentage.  This script aggregates those numbers per module and renders them
the way bytematch-style progress pages do, so the repository shows more than
the two headline figures.

Usage:  python tools/match/progress.py [--no-report]
"""
import json
import pathlib
import subprocess
import sys

REPORT = pathlib.Path('build/ATOJ/report.json')
OBJDIFF = pathlib.Path('objdiff.json')
OUT = pathlib.Path('PROGRESS.md')
EXPECT = '9eec3248d3569eba8abcb3244adcbdf01a5ea424'

NAMES = {'arm9': 'ARM9 main', 'ov002': 'OV002', 'ov003': 'OV003',
         'ov004': 'OV004', 'ov005': 'OV005', 'ov006': 'OV006',
         'ov007': 'OV007', 'ov008': 'OV008', 'ov009': 'OV009'}
ORDER = ['arm9', 'ov002', 'ov003', 'ov004', 'ov005', 'ov006', 'ov007',
         'ov008', 'ov009']


def module_of(unit_name):
    parts = unit_name.split('/')
    if parts[0] == 'src':
        return parts[1]
    if parts[0] == 'arm9' and len(parts) > 2 and parts[1] == 'overlays':
        return parts[2]
    return parts[0]


def bar(pct, width=24):
    filled = int(round(pct / 100.0 * width))
    if pct > 0 and filled == 0:
        filled = 1
    return '█' * filled + '░' * (width - filled)


def pct(matched, total):
    return 100.0 * matched / total if total else 0.0


def refresh_report():
    if REPORT.exists():
        REPORT.unlink()
    r = subprocess.run([sys.executable, '-m', 'ninja', 'report'],
                       capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit('ninja report failed:\n' + (r.stdout + r.stderr)[-800:])


def load():
    units = {}
    complete = {}
    for u in json.loads(OBJDIFF.read_text())['units']:
        complete[u['name']] = bool(u['metadata'].get('complete'))
        units[u['name']] = {}
    for u in json.loads(REPORT.read_text())['units']:
        # a gap unit shares its name with other gaps; keep totals additively
        units[u['name']] = units.get(u['name'], {})
        units[u['name']]['measures'] = u['measures']
        units[u['name']]['name'] = u['name']
    return units, complete


def collect():
    """Aggregate per module. Returns (totals, {module: totals})."""
    units, complete = load()
    # gap units repeat the name of their container, so walk the report list
    # instead of the name-keyed dict to count each one once.
    report = json.loads(REPORT.read_text())['units']
    # pair report entries with objdiff entries by order of appearance per name
    seen = {}
    per = {}

    def bucket(mod):
        return per.setdefault(mod, dict(funcs=0, funcs_total=0, code=0,
                                        code_total=0, data=0, data_total=0,
                                        fuzzy=0.0, units=0, units_total=0,
                                        data_units=0, data_units_total=0))

    obj_units = json.loads(OBJDIFF.read_text())['units']
    # match report entries to objdiff entries: both lists contain every unit,
    # in the same order.
    if len(obj_units) != len(report):
        sys.exit(f'unit count mismatch: {len(obj_units)} objdiff vs '
                 f'{len(report)} report')
    for o, r in zip(obj_units, report):
        mod = module_of(o['name'])
        b = bucket(mod)
        m = r['measures']
        code_total = int(m.get('total_code', '0'))
        data_total = int(m.get('total_data', '0'))
        # objdiff omits matched_* for some fully-complete units, emitting only
        # complete_*. Without this fallback those digest-verified units score 0.
        code = int(m.get('matched_code', m.get('complete_code', '0')))
        data = int(round(int(m.get('matched_data_percent', 0)) *
                         data_total / 100.0))
        funcs_total = m.get('total_functions', 0) or 0
        funcs = m.get('matched_functions')
        if funcs is None:
            # a complete unit is a matched function only if it owns functions;
            # a pure data unit is complete but contributes no function count.
            funcs = m.get('complete_units', 0) if funcs_total else 0
        b['code'] += code
        b['code_total'] += code_total
        b['data'] += data
        b['data_total'] += data_total
        b['funcs'] += funcs
        b['funcs_total'] += funcs_total
        b['fuzzy'] += code_total * m.get('fuzzy_match_percent', 0.0) / 100.0
        b['units_total'] += 1
        if o['metadata'].get('complete'):
            b['units'] += 1
        if data_total:
            b['data_units_total'] += 1
            if data >= data_total:
                b['data_units'] += 1
    totals = dict(funcs=0, funcs_total=0, code=0, code_total=0, data=0,
                  data_total=0, fuzzy=0.0, units=0, units_total=0,
                  data_units=0, data_units_total=0)
    for v in per.values():
        for k in totals:
            totals[k] += v[k]
    return totals, per


def fmt(n):
    return f'{n:,}'


def render(totals, per, hash_ok, digest):
    lines = ['# Progress', '']
    lines.append('The matching C sources are compiled with `mwccarm` 1.2/b56 and '
                 'linked back into the original addresses. The rebuild is '
                 'byte-identical to the retail ROM:')
    lines.append('')
    lines.append('```')
    lines.append(f'Target: {EXPECT}')
    lines.append(f'rebuilt: {digest or "not built"}'
                 f'{"  -> OK" if hash_ok else "  -> MISMATCH"}')
    lines.append('```')
    lines.append('')
    lines.append('The figures come from `ninja report` (objdiff). A complete unit '
                 'has its whole body in matching C; gap units are still '
                 'delinked assembly.')
    lines.append('')
    lines.append('## Overall')
    lines.append('')
    lines.append('| Metric | Matched | Total | Progress |')
    lines.append('|---|---:|---:|---|')
    lines.append(f'| Code | {fmt(totals["code"])} B | '
                 f'{fmt(totals["code_total"])} B | '
                 f'{pct(totals["code"], totals["code_total"]):.2f}% '
                 f'`{bar(pct(totals["code"], totals["code_total"]))}` |')
    lines.append(f'| Functions | {fmt(totals["funcs"])} | '
                 f'{fmt(totals["funcs_total"])} | '
                 f'{pct(totals["funcs"], totals["funcs_total"]):.2f}% '
                 f'`{bar(pct(totals["funcs"], totals["funcs_total"]))}` |')
    lines.append(f'| Fuzzy match | '
                 f'{totals["fuzzy"] / totals["code_total"] * 100:.2f}% | n/a | '
                 f'`{bar(totals["fuzzy"] / totals["code_total"] * 100)}` |')
    lines.append(f'| Data | {fmt(totals["data"])} B | '
                 f'{fmt(totals["data_total"])} B | '
                 f'{pct(totals["data"], totals["data_total"]):.2f}% '
                 f'`{bar(pct(totals["data"], totals["data_total"]))}` |')
    lines.append(f'| Complete units | {fmt(totals["units"])} | '
                 f'{fmt(totals["units_total"])} | '
                 f'{pct(totals["units"], totals["units_total"]):.2f}% '
                 f'`{bar(pct(totals["units"], totals["units_total"]))}` |')
    lines.append('')
    lines.append('## By module')
    lines.append('')
    lines.append('| Module | Code | Functions | Fuzzy | Data | Complete units |')
    lines.append('|---|---:|---:|---:|---:|---:|')
    mods = [m for m in ORDER if m in per] + sorted(m for m in per
                                                    if m not in ORDER)
    for mod in mods:
        b = per[mod]
        lines.append(
            f'| {NAMES.get(mod, mod.upper())} | {pct(b["code"], b["code_total"]):.2f}% '
            f'({fmt(b["code"])} / {fmt(b["code_total"])} B) | '
            f'{fmt(b["funcs"])} / {fmt(b["funcs_total"])} '
            f'({pct(b["funcs"], b["funcs_total"]):.1f}%) | '
            f'{b["fuzzy"] / b["code_total"] * 100:.2f}% | '
            f'{pct(b["data"], b["data_total"]):.2f}% '
            f'({fmt(b["data"])} / {fmt(b["data_total"])} B) | '
            f'{fmt(b["units"])} / {fmt(b["units_total"])} |')
    lines.append('')
    lines.append('## Against the whole ROM')
    lines.append('')
    lines.append(f'{totals["funcs"]:,} of {totals["funcs_total"]:,} '
                 f'functions ({pct(totals["funcs"], totals["funcs_total"]):.2f}%) '
                 f'and {totals["code"]:,} of {totals["code_total"]:,} code bytes '
                 f'({pct(totals["code"], totals["code_total"]):.2f}%) are matching '
                 'C; the rest are still delinked assembly.')
    lines.append('')
    if totals['data']:
        lines.append(f'{totals["data"]:,} bytes of `.data` are matching C '
                     'definitions; the rest of the data sections are still '
                     'raw bytes in the delinked image.')
    else:
        lines.append('No `.data`, `.rodata` or `.bss` unit has a matching C '
                     'definition yet, so the data figures stay at zero.')
    lines.append('')
    return '\n'.join(lines)


def imperfect():
    """Report units objdiff does not score 100%.

    Every complete unit is supposed to match byte for byte, and ByteMatch
    publishes `matched_*` as it stands: a unit that links correctly but scores
    99.9% silently lowers the published matched total below the fully-linked
    one (that is how `src/arm9/func_02004a14` showed 8.87% against 8.88%).  The
    aggregate fallback in `collect()` hides it, so it is checked per unit here.
    Returns the offending units as (name, fuzzy percent, matched, complete).
    """
    bad = []
    for u in json.loads(REPORT.read_text())['units']:
        m = u['measures']
        fuzzy = float(m.get('fuzzy_match_percent', 100.0))
        if fuzzy >= 100.0:
            continue
        bad.append((u['name'], fuzzy, m.get('matched_code'),
                    m.get('complete_code')))
    return bad


def main():
    if '--no-report' not in sys.argv:
        refresh_report()
    totals, per = collect()
    digest = None
    r = subprocess.run([sys.executable, '-m', 'ninja', 'sha1'],
                       capture_output=True, text=True)
    out = r.stdout + r.stderr
    for ln in out.splitlines():
        head, sep, tail = ln.partition('tennis_ATOJ.nds:')
        if sep:
            cand = tail.strip()
            if len(cand) == 40 and all(c in '0123456789abcdef' for c in cand):
                digest = cand
    hash_ok = digest == EXPECT
    OUT.write_text(render(totals, per, hash_ok, digest), newline='\n')
    print(f'{OUT}: {totals["funcs"]}/{totals["funcs_total"]} functions, '
          f'{totals["code"]}/{totals["code_total"]} bytes')
    bad = imperfect()
    if bad:
        print()
        print(f'{len(bad)} unit(s) are complete but do not score 100%: the '
              'published matched totals will sit below fully linked until they '
              'do.')
        for name, fuzzy, matched, complete in bad:
            print(f'  {fuzzy:8.3f}%  {name}  '
                  f'matched={matched if matched is not None else "-"} '
                  f'complete={complete if complete is not None else "-"}')
        print('Diagnose one with `python tools/match/objcheck.py --unit '
              '<module> <name>`: a relocated word spelled as a number shows up '
              'as a missing relocation.')
        return 1
    return 0


if __name__ == '__main__':
    main()
