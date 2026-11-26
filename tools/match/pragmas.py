"""Try mwcc pragmas around one candidate function.

A register-only diff can come from the optimisation switches the original
translation unit was built with, not from the C. This wraps the candidate in
each pragma that changes codegen and reports which ones compile byte-exact.

    python tools/match/pragmas.py <mod> <addr> <size> <file.c> [name=value ...]
"""
import importlib.util
import pathlib
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


fm = load("tools/match/fastmatch.py", "fm")

# Only these change codegen in mwccarm 1.2/b56. Measured 2026-11-26 on a 260-byte
# body (notes/mwccarm-codegen.md 2.52): Metrowerks' pragma reference documents a
# dozen optimiser switches, but the ARM backend accepts and IGNORES schedule/
# scheduling, opt_strength_reduction, opt_loop_invariants, opt_common_subs, opt_cse,
# opt_dead_assignments, opt_unroll_loops, peephole, inline_depth, dont_inline and
# pool_strings - flipping any of them leaves the bytes identical. Sweeping them
# costs a compile each and can never win, so the default list is the honoured set.
PRAGMAS = ([("optimization_level", v) for v in "012"]
           + [("opt_propagation", v) for v in ("off", "on")]
           + [("opt_dead_code", "off"), ("optimize_for_size", "on")])

# Accepted and ignored by this build; `--all` sweeps them anyway, for the day a
# different compiler key makes one live.
IGNORED = [(s, v) for s in ("opt_common_subs", "opt_dead_assignments", "opt_loop_invariants",
                            "opt_strength_reduction", "peephole", "scheduling", "opt_cse",
                            "opt_unroll_loops", "dont_inline", "pool_strings")
           for v in ("on", "off")] + [("inline_depth", "(0)")]


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
    argv = [a for a in sys.argv[1:] if a != "--all"]
    sweep = PRAGMAS + IGNORED if "--all" in sys.argv[1:] else PRAGMAS
    mod, addr, size, path = argv[0], int(argv[1], 0), int(argv[2], 0), argv[3]
    body = body_of(path)
    want = fm.retail(mod, addr, size)
    if matches(fm.compile_text(body, "probe"), want)[0]:
        print("already MATCH without a pragma")
        return 0

    tried, winners = 0, []
    for name, val in sweep:
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
