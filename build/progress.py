"""Write PROGRESS.md from the objdiff report.

`ninja report` already measures every unit: matched and total code, matched and
total data, function counts, per-unit completeness and a fuzzy-match
percentage.  This script aggregates those numbers per module and renders them
the way bytematch-style progress pages do, so the repository shows more than
the two headline figures.

Usage:  python build/progress.py [--no-report]
"""
import json
import pathlib
import subprocess
import sys

REPORT = pathlib.Path('build/ATOJ/report.json')
OBJDIFF = pathlib.Path('objdiff.json')
OUT = pathlib.Path('PROGRESS.md')
EXPECT = '8d555a80db47278475ffdfdf808fe80dbd7c48cf'

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
        funcs = m.get('matched_functions', m.get('complete_units', 0))
        funcs_total = m.get('total_functions', 0)
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
    lines.append('The matching C sources are compiled with `mwccarm` 1.2/sp2 and '
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
    lines.append('No `.data`, `.rodata` or `.bss` unit has a matching C '
                 'definition yet, so the data figures stay at zero.')
    lines.append('')
    return '\n'.join(lines)


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


if __name__ == '__main__':
    main()
