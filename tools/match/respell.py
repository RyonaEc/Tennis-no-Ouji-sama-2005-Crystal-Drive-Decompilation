"""Re-spell one candidate along the axes that decide mwcc's codegen.

A near match usually differs in one of a handful of source-level choices, not
in the algorithm: the order of the local declarations (which decides most
register colouring), whether a local is initialised at its declaration or
assigned later, whether an address is a pointer value or an addressing mode,
`x op= y` versus `x = x op y`, signed versus unsigned loop counters, and
`volatile` on a local that retail spills.  Hand-editing the file for each of
those costs a tool call apiece; this walks the grid in one run and ranks the
result by differing instruction slots.

Nothing is decided here.  A candidate that scores zero still goes through
`t10.try_unit` (and `--install` runs exactly that), so the byte compare and the
ROM digest remain the only gate.

Usage:
    python tools/match/respell.py <mod> <addr> <size> <file.c> [--install]
                                  [--top N] [--limit N] [--pad]

`--pad` adds variants with one or two dead locals, which shifts callee-saved
register allocation on units where two variables want the same colour.
"""
import importlib.util
import itertools
import pathlib
import re
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
t10 = load("tools/match/t10.py", "t10")

# Builtin types plus typedef names (a capitalised identifier), so struct
# locals such as `B8 tmp = ...;` count as declarations too.  Keywords that can
# start a statement are excluded so `return x;` is not read as a declaration.
TYPE = (r"(?:volatile\s+)?(?:u8|u16|u32|s8|s16|s32|int|char|void|u64|s64"
        r"|[A-Z]\w*)")
NOT_A_TYPE = {"return", "goto", "if", "while", "for", "switch", "case"}

# A declaration line: a type, a name (no parentheses, so no calls), a `;`.
DECL = re.compile(r"^\s*(" + TYPE + r")\s+(\*?)\s*(\w+)\s*(\[[^\]]*\])?\s*(?:=\s*(.+?))?;\s*$")


def split_unit(text):
    """(prefix, declarations, rest) around the first function body."""
    lines = text.replace("\r\n", "\n").split("\n")
    start = None
    for i, line in enumerate(lines):
        if line.strip() == "{":
            start = i
            break
    if start is None:
        raise SystemExit("no function body found (expected a line that is `{`)")
    end = len(lines) - 1
    for i in range(start + 1, len(lines)):
        if lines[i].startswith("}"):
            end = i
            break
    prefix = lines[:start + 1]
    body = lines[start + 1:end]
    suffix = lines[end:]

    decls, rest = [], []
    in_decls = True
    for line in body:
        if in_decls:
            if not line.strip():
                continue
            m = DECL.match(line)
            if m and m.group(1).split()[-1] not in NOT_A_TYPE:
                decls.append(line.rstrip())
                continue
            in_decls = False
        rest.append(line)
    return prefix, decls, rest, suffix


def build(prefix, decls, rest, suffix, pad=0):
    lines = list(prefix) + list(decls)
    if pad:
        lines.append("    u32 %s;" % ", ".join("pad%d" % i for i in range(pad)))
    if rest and rest[0].strip():
        lines.append("")
    lines += list(rest)
    lines += list(suffix)
    return "\n".join(lines) + "\n"


def func_name(mod, addr):
    if mod == "arm9":
        return "func_%08x" % addr
    return "func_%s_%08x" % (mod, addr)


def reorder(decls):
    """Every ordering of a short declaration block, rotations otherwise."""
    if len(decls) <= 4:
        for perm in itertools.permutations(decls):
            yield list(perm)
        return
    for k in range(len(decls)):
        yield decls[k:] + decls[:k]
    yield list(reversed(decls))


def split_inits(decls):
    """`T x = e;` becomes `T x;` plus `x = e;` after the block."""
    kept, assigns = [], []
    for line in decls:
        m = DECL.match(line)
        if not m or m.group(5) is None:
            kept.append(line)
            continue
        typ, star, name, arr, expr = m.groups()
        kept.append("    %s %s%s%s;" % (typ, star, name, arr or ""))
        assigns.append("    %s = %s;" % (name, expr))
    return kept, assigns


def join_inits(decls, rest):
    """The reverse: fold a leading `x = e;` back into its declaration."""
    out = list(decls)
    body = list(rest)
    changed = False
    for i, line in enumerate(list(body)):
        m = re.match(r"^\s*(\w+)\s*=\s*(.+?);\s*$", line)
        if not m:
            continue
        name, expr = m.group(1), m.group(2)
        for k, decl in enumerate(out):
            d = DECL.match(decl)
            if d and d.group(3) == name and d.group(5) is None and not d.group(4):
                out[k] = "    %s %s%s = %s;" % (d.group(1), d.group(2), name, expr)
                body[i] = None
                changed = True
                break
    return out, ([] if not changed else [x for x in body if x is not None]), changed


COMPOUND = re.compile(r"^(\s*)(.+?)\s*(\|=|&=|\+=|-=|\*=|/=)\s*(.+?);\s*$")
EXPANDED = re.compile(r"^(\s*)(.+?)\s*=\s*\2\s*([|&+\-*/])\s*(.+?);\s*$")


def expand_assign(text):
    out, changed = [], False
    for line in text:
        m = COMPOUND.match(line)
        if m and "==" not in line:
            out.append("%s%s = %s %s %s;" % (m.group(1), m.group(2), m.group(2), m.group(3)[0], m.group(4)))
            changed = True
        else:
            out.append(line)
    return out, changed


def fold_assign(text):
    out, changed = [], False
    for line in text:
        m = EXPANDED.match(line)
        if m:
            out.append("%s%s %s= %s;" % (m.group(1), m.group(2), m.group(3), m.group(4)))
            changed = True
        else:
            out.append(line)
    return out, changed


def statements(rest):
    """Split the body into statement groups that can be reordered.

    A group is one simple statement, or a block headed by `if`/`for`/`while`
    with its braces and any `else` arm.  Reordering groups changes the order
    registers are first used in, which is what decides colouring when two
    locals fight over the same callee-saved register.
    """
    groups, cur, depth = [], [], 0
    lines = [line for line in rest]
    for i, line in enumerate(lines):
        if not line.strip():
            if cur and depth == 0:
                groups.append(cur)
                cur = []
            continue
        cur.append(line)
        depth += line.count("{") - line.count("}")
        stripped = line.strip()
        closes = depth == 0 and (stripped.endswith(";") or stripped.endswith("}"))
        if closes:
            later = next((x for x in lines[i + 1:] if x.strip()), "")
            if later.lstrip().startswith("else"):
                closes = False
        if closes:
            groups.append(cur)
            cur = []
    if cur:
        groups.append(cur)
    return groups


TOGGLE_VOLATILE = re.compile(r"^\s*volatile\s+")


def toggle_volatile(decls, index):
    out = list(decls)
    if index >= len(out):
        return out, False
    line = out[index]
    if TOGGLE_VOLATILE.match(line):
        out[index] = TOGGLE_VOLATILE.sub("    ", line)
    else:
        out[index] = "    volatile " + line.strip()
    return out, True


def loop_var_signed(decls):
    out, changed = [], False
    for line in decls:
        if re.match(r"^\s*u32\s+\w+;\s*$", line):
            out.append(line.replace("u32", "s32", 1))
            changed = True
        else:
            out.append(line)
    return out, changed


def variants(text, pad=False):
    """Yield (label, full source text) for every spelling the tool tries."""
    prefix, decls, rest, suffix = split_unit(text)
    seen = set()

    def emit(label, d, r, extra=0):
        src = build(prefix, d, r, suffix, pad=(2 if pad and extra else 0))
        if src in seen:
            return None
        seen.add(src)
        return label, src

    bases = [("base", decls)]
    for n, perm in enumerate(reorder(decls)):
        if perm != decls:
            bases.append(("order%d" % n, perm))

    stmts = statements(rest)
    if 1 < len(stmts) <= 5:
        identity = list(range(len(stmts)))
        for n, perm in enumerate(itertools.permutations(identity)):
            if list(perm) == identity:
                continue
            flat = []
            for i in perm:
                flat.extend(stmts[i])
                flat.append("")
            for base_label, d in bases[:4]:
                got = emit("%s+stmt%d" % (base_label, n), d, flat)
                if got:
                    yield got

    for base_label, d in bases:
        for extra, label in ((0, ""), (1, "+pad")):
            got = emit(base_label + label, d, rest, extra)
            if got:
                yield got

        kept, assigns = split_inits(d)
        if assigns:
            got = emit(base_label + "+split", kept, assigns + rest)
            if got:
                yield got

        folded_d, folded_r, changed = join_inits(d, rest)
        if changed:
            got = emit(base_label + "+join", folded_d, folded_r)
            if got:
                yield got

        exp, changed = expand_assign(rest)
        if changed:
            got = emit(base_label + "+expand", d, exp)
            if got:
                yield got
        fold, changed = fold_assign(rest)
        if changed:
            got = emit(base_label + "+fold", d, fold)
            if got:
                yield got

        signed, changed = loop_var_signed(d)
        if changed:
            got = emit(base_label + "+signed", signed, rest)
            if got:
                yield got

        for i in range(len(d)):
            toggled, changed = toggle_volatile(d, i)
            if changed:
                got = emit("%s+vol%d" % (base_label, i), toggled, rest)
                if got:
                    yield got


def main(argv):
    args = [a for a in argv if not a.startswith("--")]
    install = "--install" in argv
    pad = "--pad" in argv
    top, limit = 6, 400
    for i, a in enumerate(argv):
        if a == "--top":
            top = int(argv[i + 1])
        elif a == "--limit":
            limit = int(argv[i + 1])
    if len(args) < 4:
        return 2
    mod, addr, size, path = args[0], int(args[1], 0), int(args[2], 0), args[3]
    text = pathlib.Path(path).read_text()
    name = func_name(mod, addr)
    if pathlib.Path("src", mod, name + ".c").exists():
        print("note: %s is already matched" % name)
    note = fm.wall_note(mod, addr)
    if note:
        print("note: parked - %s" % note)

    scored, tried, failed = [], 0, 0
    for label, src in itertools.islice(variants(text, pad), limit):
        body = re.sub(r'^\s*#include\s+"types\.h"\s*\n', "", src)
        ok, n, diffs = fm.diff_summary(mod, addr, size, body)
        tried += 1
        if not n:
            failed += 1
            continue
        if ok:
            print("MATCH %s  (%dB)" % (label, n))
            out = pathlib.Path("build/cand/respell_%s.c" % name)
            out.write_text(src)
            print("wrote %s" % out)
            if install:
                kept = t10.try_unit(name, addr, size, src, mod=mod)
                print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
            return 0
        scored.append((len(diffs), n, label))
    scored.sort()
    print("%d variants, %d did not compile, best:" % (tried, failed))
    for d, n, label in scored[:top]:
        print("  %2d differing slots  %dB  %s" % (d, n, label))
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
