"""Read and extend the parked-unit wall log.

`walls.txt` is now a one-line-per-unit index
(`<module> 0x<addr> <size> <symbol> <reason...> L<line>`); the long notes and
stored candidate bodies live verbatim in `walls-detail.txt`, whose line numbers
the index cites. Use this instead of loading the detail file:

    python tools/match/walls.py show arm9 0x020263c0     # index line + detail
    python tools/match/walls.py grep predicated          # search the index
    python tools/match/walls.py list --mod ov003         # live walls of a module
    python tools/match/walls.py add arm9 0x02012345 "reason"

`add` keeps the format and appends the note to the detail file with the line
number the new index entry cites. Walled units are still only parked: the
project rule is that a unit counts as matched when `ninja sha1` says `OK`.
"""

import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
INDEX = os.path.join(ROOT, "tools/match/walls.txt")
DETAIL = os.path.join(ROOT, "tools/match/walls-detail.txt")

MODS = r"(arm9|itcm|dtcm|ov\d{3})"
ENTRY = re.compile(r"^(?:" + MODS + r" 0x[0-9a-f]{8}|0x[0-9a-f]{8}|===)")
ENTRY_ANY = re.compile(r"^\s*(?:=== )?" + MODS + r"\s+0x([0-9a-f]{8})\b")


def index_lines():
    with io.open(INDEX, encoding="utf-8") as f:
        return f.read().splitlines()


def detail_lines():
    with io.open(DETAIL, encoding="utf-8") as f:
        return f.read().splitlines()


def entry_at(line):
    """(module, addr) if this detail line starts a note for a unit, else None."""
    m = ENTRY_ANY.match(line)
    return (m.group(1), int(m.group(2), 16)) if m else None


def detail_start(lines, mod, addr, cited):
    """Line index of this unit's note in the detail file.

    The index cites a line number, but the detail file has been rewritten over
    time (notes wrapped, entries appended, sections moved), so citations drift:
    measured on the current file, 108 of 509 cited a different unit's note. Look
    the entry up by its tag and keep the citation only to choose between
    duplicates and as a fallback for units with no labelled note.
    """
    hits = [i for i, ln in enumerate(lines) if entry_at(ln) == (mod, addr)]
    if hits:
        return min(hits, key=lambda i: abs(i - (cited - 1)))
    return cited - 1 if 0 <= cited - 1 < len(lines) else None


def show(argv):
    if len(argv) != 2:
        print("usage: walls.py show <module> <addr>")
        return 2
    mod, addr = argv[0], int(argv[1], 0)
    tag = "%s 0x%08x " % (mod, addr)
    hits = [ln for ln in index_lines() if ln.startswith(tag)]
    if not hits:
        print("no wall entry for %s 0x%08x" % (mod, addr))
        return 1
    for ln in hits:
        print(ln)
    m = re.search(r"L(\d+)$", hits[0])
    if not m:
        return 0
    cited = int(m.group(1))
    lines = detail_lines()
    start = detail_start(lines, mod, addr, cited)
    print("\n--- walls-detail.txt L%d ---" % ((start + 1) if start is not None else cited))
    if start is None:
        return 0
    out = []
    blank = 0
    for i in range(start, min(len(lines), start + 200)):
        line = lines[i]
        if out and line.strip() and entry_at(line) not in (None, (mod, addr)):
            break
        if not line.strip():
            blank += 1
            if blank > 1:
                continue
        else:
            blank = 0
        out.append(line)
    print("\n".join(out).rstrip())
    return 0


def grep(argv):
    if not argv:
        print("usage: walls.py grep <text>")
        return 2
    needle = " ".join(argv).lower()
    n = 0
    for ln in index_lines():
        if ln.startswith("#") or not ln.strip():
            continue
        if needle in ln.lower():
            print(ln)
            n += 1
    print("-- %d match(es)" % n)
    return 0 if n else 1


def listing(argv):
    mod = None
    if "--mod" in argv:
        mod = argv[argv.index("--mod") + 1]
    n = 0
    for ln in index_lines():
        if ln.startswith("#") or not ln.strip():
            continue
        if mod and not ln.startswith(mod + " "):
            continue
        print(ln)
        n += 1
    print("-- %d entr(ies)" % n)
    return 0


def add(argv):
    if len(argv) < 3:
        print('usage: walls.py add <module> <addr> "<reason>"')
        return 2
    mod, addr, reason = argv[0], int(argv[1], 0), " ".join(argv[2:])
    tag = "%s 0x%08x " % (mod, addr)
    lines = index_lines()
    if any(ln.startswith(tag) for ln in lines):
        print("already parked: run `walls.py show %s 0x%08x`" % (mod, addr))
        return 1

    sym, size = "-", 0
    for cand in ("config/ATOJ/%s/symbols.txt" % mod,
                 "config/ATOJ/arm9/%s/symbols.txt" % mod,
                 "config/ATOJ/arm9/overlays/%s/symbols.txt" % mod):
        path = os.path.join(ROOT, cand)
        if not os.path.exists(path):
            continue
        for line in io.open(path, encoding="utf-8"):
            m = re.match(r"^(\w+) kind:function\(\w+,size=(0x[0-9a-f]+)\) addr:0x([0-9a-f]+)", line.strip())
            if m and int(m.group(3), 16) == addr:
                sym, size = m.group(1), int(m.group(2), 16)
                break
        break

    with io.open(DETAIL, "a", encoding="utf-8", newline="\r\n") as f:
        f.write("\n%s 0x%08x %s\n%s\n" % (mod, addr, sym, reason))
    # Cite the line of the note's own header, found in the file we just wrote:
    # counting lines was off (it cited the last wrapped line, two past the
    # header), which is how the index drifted to start with.
    detail_line = 1 + max(i for i, ln in enumerate(detail_lines())
                          if entry_at(ln) == (mod, addr))

    row = "%s 0x%08x %s %s %s L%d" % (mod, addr, ("%dB" % size) if size else "?B", sym, reason, detail_line)
    with io.open(INDEX, "w", encoding="utf-8", newline="\r\n") as f:
        inserted = False
        for ln in lines:
            if not inserted and ln.startswith("# ") and "resolved" in ln:
                f.write(row + "\n\n")
                inserted = True
            f.write(ln + "\n")
        if not inserted:
            f.write(row + "\n")
    print(row)
    return 0


def main(argv):
    if not argv:
        print(__doc__)
        return 0
    cmd, rest = argv[0], argv[1:]
    return {"show": show, "grep": grep, "list": listing, "add": add}.get(cmd, lambda a: (print(__doc__), 2)[1])(rest)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
