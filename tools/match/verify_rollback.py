"""Exercise t10 rollback: match path, miss path, crash path, and repair."""
import importlib.util
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


t10 = load(ROOT / "tools" / "match" / "t10.py", "t10")
auto = load(ROOT / "tools" / "match" / "auto.py", "auto")


def report(label):
    state = t10.selfcheck('arm9')
    ok = not (state['dangling'] or state['unlinked'] or state['duplicates'])
    print(f"{label}: entries={state['entries']} sources={state['sources']} "
          f"dangling={len(state['dangling'])} unlinked={len(state['unlinked'])} "
          f"dupes={len(state['duplicates'])} sha_ok={t10.sha1()} invariant_ok={ok}")
    return ok and t10.sha1()


def main():
    auto.set_module('arm9')
    good = True
    good &= report('baseline')

    # 1. a unit we know matches, re-verified from a regenerated body
    name = 'func_0200851c'
    addr = auto.NAME2ADDR[name]
    body = pathlib.Path('src/arm9', f'{name}.c').read_text(encoding='utf-8').split('\n\n', 1)[1]
    matched = t10.try_unit(name, addr, auto.FUNCS[addr][1], body, mod='arm9')
    print('match path returned:', matched)
    good &= matched
    good &= report('after match')

    # 2. a candidate that does not match: tree must be untouched
    name = 'func_020103f8'
    addr = auto.NAME2ADDR[name]
    body = auto.emit(auto.gen(addr))
    missed = t10.try_unit(name, addr, auto.FUNCS[addr][1], body, mod='arm9')
    print('miss path returned:', missed)
    good &= not missed
    good &= report('after miss')

    # 2b. re-testing an existing unit with a bad body must restore its source,
    # not delete it and leave a dangling delinks entry behind
    name = 'func_02024780'
    addr = auto.NAME2ADDR[name]
    src = pathlib.Path('src/arm9', f'{name}.c')
    before = src.read_bytes()
    retested = t10.try_unit(name, addr, auto.FUNCS[addr][1],
                            'u32 ' + name + '(void) { return 0x1234; }', mod='arm9')
    print('re-test path returned:', retested,
          '| source restored:', src.exists() and src.read_bytes() == before)
    good &= (not retested) and src.exists() and src.read_bytes() == before
    good &= report('after re-test')

    # 3. a crash inside the build: restore must still happen.  The body is the
    # unit's own source so that it passes try_unit's byte-compare prefilter and
    # the build is actually reached.
    name = 'func_02026140'
    addr = auto.NAME2ADDR[name]
    crash_body = pathlib.Path('src/arm9', f'{name}.c').read_text(
        encoding='utf-8').split('\n\n', 1)[1]
    original = t10.configure_build
    t10.configure_build = lambda: (_ for _ in ()).throw(RuntimeError('simulated crash'))
    try:
        t10.try_unit(name, addr, auto.FUNCS[addr][1], crash_body, mod='arm9')
        print('crash path: NO EXCEPTION (unexpected)')
        good = False
    except RuntimeError:
        print('crash path: exception propagated as expected')
    finally:
        t10.configure_build = original
    good &= report('after crash')

    # 4. repair() heals a deliberately broken tree
    name = 'func_020187f8'
    src = pathlib.Path('src/arm9', f'{name}.c')
    saved = src.read_bytes()
    src.unlink()
    print('after deleting a source:', t10.selfcheck('arm9')['dangling'])
    t10.repair('arm9')
    src.write_bytes(saved)
    t10.repair('arm9')
    good &= report('after repair')

    print('RESULT:', 'PASS' if good else 'FAIL')
    return 0 if good else 1


if __name__ == '__main__':
    sys.exit(main())
