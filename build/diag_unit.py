"""Show what mwcc is actually complaining about for named units."""
import importlib.util
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def main(module, names):
    a = load(ROOT / 'build' / 'auto.py', 'auto')
    cs = load(ROOT / 'build' / 'compile_scan.py', 'cs')
    a.set_module(module)
    out = ROOT / 'build' / 'verify'
    for name in names:
        addr = next((x for x, (n, _) in a.FUNCS.items() if n == name), None)
        if addr is None:
            print(f'{name}: not in FUNCS')
            continue
        body = a.emit(a.gen(addr))
        src = out / 'd.c'
        src.write_text('#include "types.h"\n\n' + body, encoding='utf-8')
        r = subprocess.run(cs.EXE + cs.FLAGS + [str(src), '-o', str(out)],
                           capture_output=True, text=True)
        text = r.stdout + r.stderr
        print(f'== {module} {name} rc={r.returncode}')
        for line in text.splitlines():
            if '.c:' in line:
                print('   ', line.strip())
        lines = body.split('\n')
        nums = [int(m.group(1)) for m in
                (re.search(r'\.c:(\d+):', l) for l in text.splitlines()) if m]
        for ln in nums[:3]:
            idx = ln - 3            # body is offset by the 2-line include prelude
            for k in range(max(0, idx - 1), min(len(lines), idx + 1)):
                print(f'    {k + 1:3} {lines[k]}')
        print()


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2:])
