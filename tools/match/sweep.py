"""Sweep formulation variants of one candidate and score them against retail.

A candidate carries alternatives inline:

    u32 *p = {{base:(u32 *)0x027c0000 + 0xffe|(u32 *)0x027c3ff8}};
    {{store:*p = *p | 1;|*p |= 1;}}

Every `{{id:alt1|alt2|...}}` block expands into one axis; the tool compiles the
cartesian product and ranks the results by instruction-level difference from the
retail bytes. Nothing is installed -- a winner still goes through reg.py.

Usage:
    python tools/match/sweep.py <mod> <addr> <size> <file.c> [--top N]
                               [--limit N] [--compiler 1.2/b56]

This exists because the project's remaining walls are dominated by four
mechanical axes (offset folding, constant-pool choice, register colour, stack
alignment). Enumerating them by hand costs one tool call per guess; this walks
the grid in one run and prints what was closest.

Note: `|` separates alternatives, so a variant cannot contain one. Write
`x = x | 1` instead of `x |= 1`, and avoid `||` in a variant.
"""
import importlib.util
import itertools
import pathlib
import re
import sys

BLOCK = re.compile(r"\{\{(\w+):(.*?)\}\}", re.S)


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def norm(mnemonic, op_str):
    """Blank branch targets: they follow relocations, not the source."""
    if mnemonic.startswith("b"):
        op_str = re.sub(r"#0x[0-9a-f]+", "#T", op_str)
    op_str = re.sub(r"#0x[0-9a-f]{6,}", "#A", op_str)
    return mnemonic + " " + op_str


def score(fm, got, mod, addr, size, want):
    want_lines = [norm(i.mnemonic, i.op_str) for i in fm.parse_ins(addr, size)]
    got_lines = [norm(i.mnemonic, i.op_str) for i in fm.md.disasm(got, addr)]
    diff = abs(len(got) - len(want)) // 4 * 1000
    diff += abs(len(got_lines) - len(want_lines))
    for a, b in zip(want_lines, got_lines):
        if a != b:
            diff += 1
    return diff


def main() -> int:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    top, limit, compiler = 8, 4000, None
    for i, a in enumerate(sys.argv):
        if a == "--top":
            top = int(sys.argv[i + 1])
        elif a == "--limit":
            limit = int(sys.argv[i + 1])
        elif a == "--compiler":
            compiler = sys.argv[i + 1]
    mod = args[0]
    addr, size = int(args[1], 0), int(args[2], 0)
    template = pathlib.Path(args[3]).read_text()

    fm = load("tools/match/fastmatch.py", "fm")
    # `compile_text`'s compiler argument is an executable path, not a version tag;
    # passing a tag silently yields b"" and every combination looks like a miss.
    compiler_path = fm.mwcc_path(compiler) if compiler and "/" in compiler else compiler
    want = fm.retail(mod, addr, size)

    axes = []
    for match in BLOCK.finditer(template):
        axes.append((match.group(1), match.group(2).split("|")))
    if not axes:
        print("no {{axis:...}} blocks found")
        return 2

    combos = 1
    for _, alts in axes:
        combos *= len(alts)
    print(f"{mod} {addr:#x} {size}B  {combos} combinations "
          f"over {len(axes)} axes: {', '.join(n for n, _ in axes)}")
    if combos > limit:
        print(f"refusing: {combos} > {limit} (raise --limit)")
        return 2

    best = []
    tried = 0
    failed = 0
    for choice in itertools.product(*[alts for _, alts in axes]):
        body = template
        for (name, _), value in zip(axes, choice):
            body = re.sub(r"\{\{" + name + r":.*?\}\}", lambda _m, v=value: v, body, count=1, flags=re.S)
        tried += 1
        try:
            got = fm.compile_text(body, "sweep", compiler_path)
        except Exception:
            failed += 1
            continue
        if got == want:
            print(f"MATCH after {tried} combinations")
            for name, value in zip([n for n, _ in axes], choice):
                print(f"  {name} = {value}")
            return 0
        best.append((score(fm, got, mod, addr, size, want), choice, len(got)))

    best.sort(key=lambda r: (r[0], r[2]))
    note = f" ({failed} did not compile)" if failed else ""
    print(f"no match in {tried}{note}; closest:")
    for diff, choice, got_len in best[:top]:
        pairs = ", ".join(f"{n}={v}" for (n, _), v in zip(axes, choice))
        print(f"  diff {diff:4d}  {got_len}B  {pairs}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
