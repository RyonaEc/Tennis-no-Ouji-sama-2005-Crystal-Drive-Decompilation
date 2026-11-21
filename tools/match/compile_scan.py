"""Compile every generatable candidate in isolation and cluster the failures.

Uses the exact mwcc invocation from build.ninja's mwcc rule, so a failure here is
the same failure the real build hits, but costs ~0.1s instead of a full relink.
"""
import importlib.util
import collections
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
EXE = [str(ROOT / 'tools' / 'sjiswrap.exe'),
       str(ROOT / 'tools' / 'mwccarm' / '1.2' / 'sp2' / 'mwccarm.exe')]
FLAGS = ('-O4,p -enum int -proc arm946e -gccext,on -fp soft -char signed '
         '-inline on,noauto -Cpp_exceptions off -gccinc -interworking -sym on '
         '-nolink -msgstyle gcc -str reuse -enc sjis -i include -lang=c -d ATOJ '
         '-c').split()
ERR_RE = re.compile(r'^.*?\.c:(\d+): (.+)$')


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def compile_error(body, out_dir, stem='scan'):
    src = out_dir / f'{stem}.c'
    src.write_text('#include "types.h"\n\n' + body, encoding='utf-8')
    r = subprocess.run(EXE + FLAGS + [str(src), '-o', str(out_dir)],
                       capture_output=True, text=True)
    if r.returncode == 0:
        return None
    msgs = [m.group(2).strip() for m in
            (ERR_RE.match(l) for l in (r.stdout + r.stderr).splitlines()) if m]
    return msgs[0] if msgs else (r.stdout + r.stderr).strip().splitlines()[-1:]


def main(module, limit=None):
    a = load(ROOT / 'tools' / 'match' / 'auto.py', 'auto')
    a.set_module(module)
    srcdir = ROOT / 'src' / module
    have = {p.stem for p in srcdir.glob('*.c')}
    out_dir = ROOT / 'build' / 'verify'
    out_dir.mkdir(parents=True, exist_ok=True)
    clusters = collections.Counter()
    examples = {}
    checked = failed = 0
    for addr, (name, size) in a.FUNCS.items():
        if name in have:
            continue
        try:
            body = a.emit(a.gen(addr))
        except Exception:
            continue
        checked += 1
        if limit and checked > limit:
            break
        err = compile_error(body, out_dir)
        if err is None:
            continue
        failed += 1
        clusters[err] += 1
        examples.setdefault(err, (name, body))
    print(f'[{module}] generatable {checked}, failed {failed}')
    for msg, n in clusters.most_common(15):
        print(f'   {n:4}  {msg}')
        print(f'         e.g. {examples[msg][0]}')
    return clusters, examples


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else 'arm9',
         int(sys.argv[2]) if len(sys.argv) > 2 else None)
