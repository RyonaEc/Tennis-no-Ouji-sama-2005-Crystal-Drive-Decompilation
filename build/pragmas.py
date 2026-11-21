"""Try mwcc pragmas around one candidate function.

A register-only diff can come from the optimisation switches the original
translation unit was built with, not from the C. This wraps the candidate in
each pragma that changes codegen and reports which ones compile byte-exact.

    python build/pragmas.py <mod> <addr> <size> <file.c> [name=value ...]
"""
import importlib.util
import pathlib
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


fm = load("build/fastmatch.py", "fm")

PRAGMAS = ([("optimization_level", v) for v in "0123"]
           + [(s, v) for s in ("opt_common_subs", "opt_propagation", "opt_dead_assignments",
                               "opt_loop_invariants", "opt_strength_reduction", "peephole",
                               "scheduling", "optimize_for_size", "opt_cse", "opt_strength_reduction")
              for v in ("on", "off")])


def body_of(path):
    lines = pathlib.Path(path).read_text().splitlines()
    return "\n".join(l for l in lines if not l.lstrip().startswith("#include")).strip()


def matches(got, want):
    if not got:
        return False, len(got)
    if got == want:
        return True, len(got)
    rel = fm.compile_text.last_relocs
    return (bool(rel) and fm.masked(got, rel) == fm.masked(want, rel)), len(got)


def main():
    mod, addr, size, path = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0), sys.argv[4]
    body = body_of(path)
    want = fm.retail(mod, addr, size)
    if matches(fm.compile_text(body, "probe"), want)[0]:
        print("already MATCH without a pragma")
        return 0

    tried, winners = 0, []
    for name, val in PRAGMAS:
        text = f"#pragma push\n#pragma {name} {val}\n{body}\n#pragma pop\n"
        got = fm.compile_text(text, "probe")
        ok, got_size = matches(got, want)
        tried += 1
        if ok:
            winners.append(f"{name} {val}")
            print(f"  MATCH  #pragma {name} {val}")
    print(f"{pathlib.Path(path).name}: {len(winners)} matching pragma(s) of {tried}")
    if not winners:
        base = len(fm.compile_text(body, "probe"))
        print(f"  no pragma helped; bare candidate is {base} vs {len(want)}")
    return 0 if winners else 1


if __name__ == "__main__":
    raise SystemExit(main())
