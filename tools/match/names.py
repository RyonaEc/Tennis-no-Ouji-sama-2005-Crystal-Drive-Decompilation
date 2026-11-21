#!/usr/bin/env python3
"""The scripted symbol-rename pass - built now, used only when matching ends.

Symbols stay `func_<address>` for the whole matching phase (AGENTS.md: never
rename existing symbols). What this file proves ahead of time is that the
rename is mechanical and loses nothing: every place a name is recorded gets
rewritten in one pass, and the linked ROM must stay byte-identical.

Reads the annotation table (`notes/naming/annotations.tsv`, see
`docs/NAMING.md`) and, for every row whose `proposed` differs from `symbol`,
rewrites:

  * src/<mod>/<symbol>.c                -> src/<mod>/<proposed>.c
  * config/ATOJ/arm9/**/delinks.txt       entry key (source path)
  * config/ATOJ/arm9/**/symbols.txt       the definition line's leading name
  * every .c under src/ and .h under include/   identifier occurrences
                                           (extern declarations, call sites)
  * config/ATOJ/mwcc_versions.txt         per-file compiler key

It is a token / path substitution: no other byte in those files changes, line
endings are preserved and the delinks entry order is untouched, so the linked
ROM must stay identical. That was verified on a mirror tree in 2026-11-22: four
units renamed, `ninja sha1` unchanged, `t10.py --check` clean (docs/NAMING.md).

    python tools/match/names.py --dry-run                 # report only
    python tools/match/names.py --verify                  # no old name left
    python tools/match/names.py --root <copy> --apply     # rehearse on a copy
    python tools/match/names.py --apply --allow-live-tree # the real pass

`--apply` refuses a tree that has a `.git` directory unless
`--allow-live-tree` is given, so the default cannot rewrite the project by
accident. `--dry-run` is always safe: it only reads.
"""

import argparse
import csv
import pathlib
import re
import sys

DEFAULT_ANNOTATIONS = "notes/naming/annotations.tsv"


def srcdir(root, mod):
    return root / ("src/arm9" if mod == "arm9" else f"src/{mod}")


def delinks(root, mod):
    if mod == "arm9":
        return root / "config/ATOJ/arm9/delinks.txt"
    return root / f"config/ATOJ/arm9/overlays/{mod}/delinks.txt"


def symbols(root, mod):
    if mod == "arm9":
        return root / "config/ATOJ/arm9/symbols.txt"
    return root / f"config/ATOJ/arm9/overlays/{mod}/symbols.txt"


def read(path):
    return path.read_text(encoding="utf-8", newline="")


def write(path, text):
    path.write_text(text, encoding="utf-8", newline="")


def rows(path):
    with path.open(encoding="utf-8", newline="") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            yield {k: (v or "").strip() for k, v in row.items()}


def token(name):
    return re.compile(r"(?<![A-Za-z0-9_])" + re.escape(name) + r"(?![A-Za-z0-9_])")


def plan(root, mod, old, new):
    """Every file edit the rename needs, as (path, transform, before, after)."""
    edits = []
    src = srcdir(root, mod)
    old_c = src / f"{old}.c"
    new_c = src / f"{new}.c"

    dl = delinks(root, mod)
    old_rel = old_c.relative_to(root).as_posix()
    new_rel = new_c.relative_to(root).as_posix()
    text = read(dl)
    if old_rel not in text:
        raise SystemExit(f"{dl}: no entry for {old_rel}")
    edits.append((dl, lambda t, a=old_rel, b=new_rel: t.replace(a, b), old_rel, new_rel))

    sym = symbols(root, mod)
    text = read(sym)
    pat = re.compile(r"^" + re.escape(old) + r"(?=[\s:])", re.M)
    if not pat.search(text):
        raise SystemExit(f"{sym}: no definition for {old}")
    edits.append((sym, lambda t, p=pat, n=new: p.sub(n, t), old, new))

    versions = root / "config/ATOJ/mwcc_versions.txt"
    if versions.is_file():
        text = read(versions)
        if old_rel in text:
            edits.append((versions, lambda t, a=old_rel, b=new_rel: t.replace(a, b),
                          old_rel, new_rel))

    pat = token(old)
    for base in (root / "src", root / "include"):
        for path in sorted(base.rglob("*")):
            if path.suffix not in (".c", ".h") or not path.is_file():
                continue
            text = read(path)
            if pat.search(text):
                edits.append((path, lambda t, p=pat, n=new: p.sub(n, t), old, new))

    return old_c, new_c, edits


def annotate(root, row, apply, out=print):
    old, new = row["symbol"], row["proposed"]
    if not old or not new or old == new:
        return 0
    old_c = srcdir(root, row["module"]).joinpath(f"{old}.c")
    new_c = srcdir(root, row["module"]).joinpath(f"{new}.c")
    if not old_c.is_file() and new_c.is_file():
        out(f"{row['module']}  {old} -> {new}  skip (already renamed)")
        return 0
    old_c, new_c, edits = plan(root, row["module"], old, new)
    if not old_c.is_file():
        raise SystemExit(f"{old_c}: source missing")
    if new_c.exists():
        raise SystemExit(f"{new_c}: target already exists")
    out(f"{row['module']}  {old} -> {new}  ({row['confidence'] or '?'})")
    for path, transform, before, after in edits:
        text = read(path)
        new_text = transform(text)
        hits = text.count(before)
        out(f"    {path.relative_to(root).as_posix():48} {hits:3}x  {before} -> {after}")
        if apply:
            write(path, new_text)
    out(f"    {old_c.relative_to(root).as_posix():48} rename -> "
        f"{new_c.relative_to(root).as_posix()}")
    if apply:
        old_c.rename(new_c)
    return len(edits) + 1


def verify(root, rows_, out=print):
    bad = 0
    for row in rows_:
        old, new = row["symbol"], row["proposed"]
        if not old or not new or old == new:
            continue
        if srcdir(root, row["module"]).joinpath(f"{old}.c").exists():
            out(f"STALE  {old}.c still present")
            bad += 1
        pat = token(old)
        for base in (root / "src", root / "config", root / "include"):
            for path in base.rglob("*"):
                if path.suffix not in (".c", ".h", ".txt") or not path.is_file():
                    continue
                if pat.search(read(path)):
                    out(f"STALE  {path.relative_to(root).as_posix()}: {old}")
                    bad += 1
        if not srcdir(root, row["module"]).joinpath(f"{new}.c").exists():
            out(f"MISSING {new}.c")
            bad += 1
    return bad


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--root", default=".", help="tree to rewrite (default: this repository)")
    ap.add_argument("--annotations", default=DEFAULT_ANNOTATIONS)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--verify", action="store_true")
    ap.add_argument("--allow-live-tree", action="store_true",
                    help="permit --apply on a tree with .git (the real rename pass)")
    args = ap.parse_args()

    root = pathlib.Path(args.root).resolve()
    rows_ = list(rows(pathlib.Path(args.annotations)))

    if args.apply and (root / ".git").exists() and not args.allow_live_tree:
        print(f"{root} is a live tree; pass --allow-live-tree for the real pass",
              file=sys.stderr)
        return 2

    if args.verify:
        bad = verify(root, rows_)
        print(f"verify: {bad} stale/missing")
        return 1 if bad else 0

    touched = 0
    for row in rows_:
        touched += annotate(root, row, args.apply)
    print(f"{'applied' if args.apply else 'dry run'}: {touched} rewrite(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
