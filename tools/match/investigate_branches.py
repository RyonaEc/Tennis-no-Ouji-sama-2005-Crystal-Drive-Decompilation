"""Classify branches whose target is missing from the decoded body.

Categories:
  same-unit-gap : target lies in [start,end) but was not decoded (literal pool / disasm gap)
  other-unit    : target lies inside another delinked unit
  no-unit       : target lies outside every delinked unit
  before-start  : target below the unit start
"""
import importlib.util
import pathlib
import re
import sys
import collections

ROOT = pathlib.Path(__file__).resolve().parents[2]


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


auto = load(ROOT / 'tools' / 'match' / 'auto.py', 'auto')

RANGES_RE = re.compile(r'\.text\s+start:(0x[0-9a-f]+)\s+end:(0x[0-9a-f]+)')


def unit_ranges(mod):
    if mod == 'arm9':
        dl = ROOT / 'config' / 'ATOJ' / 'arm9' / 'delinks.txt'
    else:
        dl = ROOT / 'config' / 'ATOJ' / 'arm9' / 'overlays' / mod / 'delinks.txt'
    if not dl.exists():
        return []
    return [(int(a, 16), int(b, 16), 'delinked') for a, b in RANGES_RE.findall(dl.read_text())]


def classify(mod, limit=400):
    auto.set_module(mod)
    srcdir = ROOT / 'src' / ('arm9' if mod == 'arm9' else mod)
    have = {p.stem for p in srcdir.glob('*.c')}
    ranges = unit_ranges(mod)
    counts = collections.Counter()
    examples = collections.defaultdict(list)
    checked = 0

    for addr, (name, size) in auto.FUNCS.items():
        if name in have:
            continue
        try:
            auto.gen(addr)
            continue                      # generated fine, not a bailer
        except auto.Bail as b:
            if 'branch target outside body' not in str(b):
                continue
        except Exception:
            continue
        checked += 1
        if checked > limit:
            break

        ins = auto.disasm(addr, size)
        index = {i.address for i in ins}
        end = addr + size
        for i in ins:
            base, cond = auto._norm(i.mnemonic)
            if base != 'b':
                continue
            try:
                tgt = int(i.op_str.lstrip('#'), 0)
            except ValueError:
                continue
            if tgt in index:
                continue
            if addr <= tgt < end:
                kind = 'same-unit-gap'
            elif tgt < addr:
                kind = 'before-start'
            else:
                hit = [r for r in ranges if r[0] <= tgt < r[1]]
                kind = 'other-unit' if hit else 'no-unit'
            counts[kind] += 1
            if len(examples[kind]) < 4:
                examples[kind].append((name, hex(addr), i.mnemonic, hex(tgt),
                                       'unit' if tgt in (addr, end) else '',
                                       f'delta={tgt - end:+#x}'))
    return checked, counts, examples


if __name__ == '__main__':
    for mod in sys.argv[1:] or ['ov003', 'ov002']:
        checked, counts, examples = classify(mod)
        print(f'[{mod}] bailers inspected: {checked}')
        for kind, n in counts.most_common():
            print(f'   {kind:14} {n}')
            for e in examples[kind][:2]:
                print('       ', e)
