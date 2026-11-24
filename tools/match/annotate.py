"""One-shot annotation of a function: disassembly plus everything needed to write it.

Reading a retail body normally costs three round trips - `udis.py` for the
instructions, a script for the literal-pool words and their symbols, and a grep
for each callee's signature.  This prints all of it at once, which is the part
that actually costs time when several units are in flight.

Usage:
    python tools/match/annotate.py <mod> <addr> [<addr> ...] [--no-sig]

Each line carries the instruction, and:
  * a pc-relative load shows the word it reads, its hex, and the symbol it names
    (plus the symbol's kind, so `bss`/`data` versus `func_` is visible);
  * a `bl`/`b` shows the callee's name from the module's symbol table, with the
    prototype found in `src/` when one exists.

The disassembly is a reading aid: `ninja sha1` still decides a match.
"""
import importlib.util
import pathlib
import re
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")

SYMBOLS = {}


def symbols_for(mod):
    if mod in SYMBOLS:
        return SYMBOLS[mod]
    path = ("config/ATOJ/arm9/symbols.txt" if mod == "arm9"
            else "config/ATOJ/arm9/overlays/%s/symbols.txt" % mod)
    table = {}
    p = pathlib.Path(path)
    if p.exists():
        for line in p.read_text(errors="replace").splitlines():
            m = re.match(r"(\S+)\s+kind:(\w+)(?:\(([^)]*)\))?", line)
            a = re.search(r"addr:0x([0-9a-f]+)", line)
            if m and a:
                table[int(a.group(1), 16)] = (m.group(1), m.group(2))
    SYMBOLS[mod] = table
    return table


def lookup(mod, addr):
    """Name/kind for an address, preferring the module and falling back to arm9.

    `bl` from an overlay almost always targets arm9, so the module's own table
    alone leaves most call targets unnamed.
    """
    for table in (symbols_for(mod), symbols_for("arm9")):
        if addr in table:
            return table[addr]
    return ("", "")


PROTO = {}


def prototype(name):
    """The prototype or definition line for a function name, from src/."""
    if name in PROTO:
        return PROTO[name]
    if not name or name == "?":
        return ""
    pat = re.compile(r"^\s*extern\s+.*\b%s\s*\(" % re.escape(name))
    found = ""
    for path in pathlib.Path("src").rglob(name + ".c"):
        for line in path.read_text(errors="replace").splitlines():
            if name + "(" in line and line.rstrip().endswith((";", ")")):
                found = line.strip()
                break
        if found:
            break
    if not found:
        for path in pathlib.Path("src").rglob("*.c"):
            for line in path.read_text(errors="replace").splitlines():
                if pat.match(line):
                    found = line.strip()
                    break
            if found:
                break
    PROTO[name] = found
    return found


def annotate(mod, addr, want_sig=True):
    fm.auto.set_module(mod)
    name, size = fm.auto.FUNCS.get(addr, ("?", 0))
    print("=== %s 0x%08x %s  %dB" % (mod, addr, name, size))
    for ins in fm.parse_ins(addr, size):
        line = "  %08x  %-8s %s" % (ins.address, ins.mnemonic, ins.op_str)
        if "[pc" in ins.op_str:
            word = fm.literal_of(ins)
            sym, kind = lookup(mod, word)
            line += "   = 0x%08x" % word
            if sym:
                line += "  %s (%s)" % (sym, kind)
        elif ins.mnemonic in ("bl", "blx", "b") and ins.op_str.startswith("#"):
            target = int(ins.op_str.lstrip("#"), 16)
            sym, kind = lookup(mod, target)
            sym = sym or "?"
            line += "  -> %s" % sym
            if want_sig:
                proto = prototype(sym)
                if proto:
                    line += "   [%s]" % proto
        print(line)


def main(argv):
    args = [a for a in argv if not a.startswith("--")]
    if len(args) < 2:
        print(__doc__)
        return 2
    mod = args[0]
    want_sig = "--no-sig" not in argv
    for a in args[1:]:
        annotate(mod, int(a, 0), want_sig)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
