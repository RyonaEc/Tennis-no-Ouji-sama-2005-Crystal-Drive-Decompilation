"""Permute marked statements in a candidate and score every variant.

Mark the lines to permute with a trailing `// P`. Each permutation keeps those
lines in their original slots, compiles the body with the requested mwccarm
build and scores it by byte difference against retail. Nothing is installed.

Usage:
    python build/permute.py <mod> <addr> <size> <file.c> [version] [--top N]

This exists because the walls left in this project are dominated by scheduling
residue: the same opcodes in different slots. Punctuation and formulation are
already covered by hand; statement order is the axis nothing else searches.
"""
import importlib.util
import itertools
import sys
from pathlib import Path

MARK = "// P"


def load_version(tag):
    path = Path(f"build/fm_{tag.replace('/', '_')}.py")
    if not path.exists():
        base = Path("build/fastmatch.py").read_text(encoding="utf-8")
        path.write_text(
            base.replace("tools/mwccarm/1.2/sp2/mwccarm.exe", f"tools/mwccarm/{tag}/mwccarm.exe"),
            encoding="utf-8",
        )
    spec = importlib.util.spec_from_file_location(f"fm_{tag}", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    top = 5
    for i, a in enumerate(sys.argv):
        if a == "--top":
            top = int(sys.argv[i + 1])
    mod, addr, size, src = args[0], int(args[1], 0), int(args[2], 0), Path(args[3])
    version = args[4] if len(args) > 4 else "1.2/sp2"
    fm = load_version(version)

    lines = src.read_text(encoding="utf-8").splitlines()
    slots = [i for i, l in enumerate(lines) if l.rstrip().endswith(MARK)]
    if not slots:
        raise SystemExit(f"{src}: no lines marked with {MARK!r}")
    marked = [lines[i][: lines[i].rindex(MARK)].rstrip() for i in slots]
    print(f"{mod} {addr:#x} {size}B  {version}  permuting {len(marked)} statements "
          f"({len(list(itertools.permutations(marked)))} orders)")

    scored = []
    seen = set()
    for order in itertools.permutations(marked):
        if order in seen:
            continue
        seen.add(order)
        trial = list(lines)
        for slot, text in zip(slots, order):
            trial[slot] = text
        body = "\n".join(trial)
        if not fm.compile_text(body, "perm"):
            continue
        if fm.compare(body, mod, addr, size):
            print(f"  *** MATCH: {' / '.join(o.strip() for o in order)}")
            out = src.with_suffix(".win.c")
            out.write_text(body, encoding="utf-8")
            print(f"  written to {out}")
            return 0
        diff = fm.diff_summary(mod, addr, size, body)
        scored.append((diff[1] if diff[1] is not None else 10**6, order))

    scored.sort()
    for score, order in scored[:top]:
        print(f"  {score:4}  {' | '.join(o.strip()[:46] for o in order)}")
    return 1 if scored else 2


if __name__ == "__main__":
    raise SystemExit(main())
