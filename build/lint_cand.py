"""Check a candidate's extern names against this tree's real symbol tables.

The helper trees carry stale symbols.txt files. Names like `data_2042a68`,
`func_02048c8` and `func_02062d84` link-fail (or silently resolve elsewhere) here,
so a candidate can score MATCH in fastmatch and still be unbuildable. Run this
before installing anything a helper hands over:

    python build/lint_cand.py build/cand/func_020146e0.c
"""
import re, sys
from pathlib import Path

def symbols():
    names = set()
    root = Path("config/ATOJ/arm9")
    for p in [root/"symbols.txt", *root.glob("overlays/*/symbols.txt"), *root.glob("itcm/*symbols*"), *root.glob("dtcm/*symbols*")]:
        if not p.exists(): continue
        for l in p.read_text(errors="replace").splitlines():
            m = re.match(r"(\S+)\s+kind:", l)
            if m: names.add(m.group(1))
    return names

def main(paths):
    names = symbols()
    bad = 0
    for f in paths:
        src = Path(f).read_text(errors="replace")
        for kw in ("extern", "void", "u8", "u16", "u32", "s8", "s16", "s32"):
            pass
        for decl in re.findall(r"^\s*extern[^(;]*?(\w+)\s*(?:\[[^\]]*\]|\(|;)", src, re.M):
            if decl not in names:
                print(f"  BAD  {Path(f).name}: extern {decl} is not in any symbol table")
                bad += 1
    print(f"{bad} bad extern name(s) in {len(paths)} file(s)")
    return 1 if bad else 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:] or [str(p) for p in Path("build/cand").glob("*.c")]))
