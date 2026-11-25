"""Census matched units whose source spells a pooled load's target as a number.

THE BUG SHAPE. mwccarm compiles `return 0x02102070;` to exactly the same bytes
as `return (u32)data_ov002_02102070;`: `ldr r0, [pc]` plus a pool word. The
digest cannot see the difference either, because a hard-coded literal links to
the same word the relocation would have produced -- so an address written as a
number is a source-honesty bug that survives every byte gate we have.

WHAT THIS FINDS. For every `kind:load` relocation in `config/ATOJ/**/relocs.txt`
whose `from` address lies inside a unit that has a `src/` file, it looks for the
`to` address written as a bare literal (hex or decimal) in that source, outside
comments and strings. A hit is a CANDIDATE: the literal has to sit at the same
access as the pool word, which this tool cannot tell, and some hits are genuine
(a fixed hardware address, or a value that happens to equal a data address).

WHAT TO DO WITH A ROW. Read the pool word out of the image, confirm it is the
symbol the row names, and only then replace the number with the symbol name.
Re-run `reg.py` (or a full `ninja sha1`) afterwards: naming a symbol moves the
pool word away sometimes, and that is a real change.

    python tools/match/literal_load.py              # whole tree
    python tools/match/literal_load.py ov002        # one module

Only units that have a `src/` file are scanned: a delinked unit has no source to
be honest or dishonest about, so its pool loads are counted and skipped.
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config" / "ATOJ" / "arm9"

UNIT = re.compile(r"^\s*src/(\S+\.c):\s*$")
TEXT = re.compile(r"^\s*\.text\s+start:(0x[0-9a-fA-F]+)\s+end:(0x[0-9a-fA-F]+)")
RELOC = re.compile(r"^from:(0x[0-9a-fA-F]+)\s+kind:(\w+)\s+to:(0x[0-9a-fA-F]+)")
SYMBOL = re.compile(r"^(0x[0-9a-fA-F]+)\s+(\S+)")


def module_dirs():
    dirs = []
    for symbols in sorted(CONFIG.glob("**/symbols.txt")):
        base = symbols.parent
        if (base / "delinks.txt").is_file():
            dirs.append(base)
    return dirs


def read_units(base):
    """[(start, end, src_path)] for one module."""
    units = []
    current = None
    delinks = base / "delinks.txt"
    if not delinks.is_file():
        return units
    for line in delinks.read_text(encoding="utf-8", errors="replace").splitlines():
        m = UNIT.match(line)
        if m:
            current = m.group(1)
            continue
        m = TEXT.match(line)
        if m and current:
            units.append((int(m.group(1), 16), int(m.group(2), 16), current))
            current = None
    return units


def read_symbols(base):
    out = {}
    path = base / "symbols.txt"
    if not path.is_file():
        return out
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SYMBOL.match(line.strip())
        if m:
            out[int(m.group(1), 16)] = m.group(2)
    return out


def read_loads(base):
    """[(from, to)] for kind:load relocations."""
    out = []
    for path in sorted(base.glob("relocs.txt")):
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = RELOC.match(line.strip())
            if m and m.group(2) == "load":
                out.append((int(m.group(1), 16), int(m.group(3), 16)))
    return out


def strip_source(text):
    """Drop comments, string and char literals so a mention in a note is not a hit."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    text = re.sub(r"'(?:\\.|[^'\\])*'", "''", text)
    return text


def literal_forms(address):
    forms = ["0x%x" % address, "0x%08x" % address, "%d" % address]
    if address & 0xFF000000 < 0x02000000:
        forms.append("0x%X" % address)
    return forms


def find_literal(text, address):
    """The first literal spelling of `address`, if any, with a word boundary."""
    for form in literal_forms(address):
        if re.search(r"(?<![\w.])" + re.escape(form) + r"(?![\w.])", text, re.I):
            return form
    return None


def owner(units, address):
    for start, end, src in units:
        if start <= address < end:
            return src
    return None


def main(argv):
    wanted = [a for a in argv if not a.startswith("-")]
    rows = []
    seen = set()
    skipped = 0
    for base in module_dirs():
        label = base.relative_to(CONFIG).as_posix()
        mod = base.name
        if wanted and mod not in wanted and label not in wanted:
            continue
        units = read_units(base)
        if not units:
            continue
        symbols = read_symbols(base)
        loads = read_loads(base)
        cache = {}
        for frm, to in loads:
            src = owner(units, frm)
            if src is None:
                skipped += 1
                continue
            key = (src, to)
            if key in seen:
                continue
            seen.add(key)
            path = ROOT / src
            if not path.is_file():
                continue
            if src not in cache:
                cache[src] = strip_source(path.read_text(encoding="utf-8", errors="replace"))
            form = find_literal(cache[src], to)
            if form:
                rows.append((mod, src, "0x%08x" % frm, to, symbols.get(to, "(none)"), form))
    if not rows:
        print("literal_load: no pooled-load target is spelled as a number "
              "(%d pool loads in delinked units skipped)" % skipped)
        return 0
    print("module  source  from  to  symbol  literal")
    for mod, src, frm, to, sym, form in rows:
        print("%-6s %-45s %s -> 0x%08x %-24s %s" % (mod, src, frm, to, sym, form))
    print("%d candidate(s) (%d pool loads in delinked units skipped). Each is a "
          "question, not a fix: read the retail pool word" % (len(rows), skipped))
    print("at `from` before renaming anything, then re-run reg.py / ninja sha1.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
