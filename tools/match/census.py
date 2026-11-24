"""Classify the unmatched queue by instruction pattern, not by size.

`grind.py` records every unit it could not generate a candidate for as
`no-candidate`, and the roster says nothing about what those units are. Many are
not functions at all: the delink carves units out of `symbols.txt`, which marks
address-taken labels, veneers, padding and non-returning fragments as functions
too. This tool reads the retail bytes of each unit and puts it in a bucket, so
the size of the real remaining work is a number rather than an impression.

    python tools/match/census.py                 # journal's no-candidate units
    python tools/match/census.py --all           # every unit in true_fresh.txt
    python tools/match/census.py --module ov003  # one module
    python tools/match/census.py --write build/census.tsv --top 20

Buckets:

    zero-fill    every word is 0 (alignment padding inside a "function")
    veneer       `ldr rX, [pc]; bx rX` -- a 8-byte interworking jump
    stack-stub   `add sp, sp, #N; ldm sp!, {lr}; bx lr` -- no body at all
    swi-stub     nothing but a `swi` and its setup
    no-return    no `bx lr` / pop-pc / tail `b` anywhere: falls into the next unit
    multi-body   more than one prologue: two functions carved as one unit
    pool-tail    ends in a terminator, then non-code words (a literal pool)
    code         a real function body the generator failed to express
"""

import argparse
import collections
import io
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import fastmatch as fm

BUCKETS = ("zero-fill", "data", "veneer", "stack-stub", "swi-stub", "no-return",
           "multi-body", "body", "body+pool", "body+code")

STACK_ONLY = frozenset(("add", "sub", "ldm", "ldmia", "pop", "push", "mov",
                        "bx", "nop"))

CONDS = ("eq", "ne", "cs", "hs", "cc", "lo", "mi", "pl", "vs", "vc",
         "hi", "ls", "ge", "lt", "gt", "le", "al")

# Mnemonics that plausibly open a function body (entry flags are undefined, so a
# predicated instruction cannot come first).
OPENERS = frozenset(
    "push sub add mov movw movt ldr ldrh ldrb ldrsh ldrsb str strh strb cmp cmn "
    "tst teq mvn bic orr eor and mul mla umull smull umlal smlal bl b blx swi "
    "mrs msr lsl lsr asr ror rsb adr adr ldm stm stmia ldmia clz uxth sxtb uxtb "
    "sxth rev rev16 bfc bfi sbfx ubfx ldrd strd pld nop".split())


def opens_function(ins):
    """Is this first instruction a plausible function entry?"""
    m = ins.mnemonic
    if m in OPENERS:
        return True
    for c in CONDS:
        if m.endswith(c) and m[:-len(c)] in OPENERS:
            return False
    return True


def is_term(ins):
    """Does this instruction leave the function?"""
    m, o = ins.mnemonic, ins.op_str
    if m.startswith("bx"):
        return o.strip() in ("lr", "r14")
    if m.startswith("ldm") or m.startswith("pop"):
        return "pc" in o
    if m in ("mov", "add") and o.replace(" ", "").endswith(",pc"):
        return True
    if m.startswith("ldr") and o.replace(" ", "").endswith(",pc]") and o.startswith("pc"):
        return True
    return False


def is_prologue(ins):
    return ins.mnemonic == "push" and "lr" in ins.op_str


def is_pad(ins_list):
    """Words that carry no code: zeros, or `andeq r0, r0, r0`-style filler."""
    return all(i.op_str.strip().replace(" ", "") in ("r0,r0,r0", "r0,r0") for i in ins_list)


def pc_literals(ins):
    """Addresses the body loads through pc: those tail words are its pool."""
    out = set()
    for i in ins:
        if "[pc" not in i.op_str:
            continue
        try:
            token = [t for t in i.op_str.replace("]", "").split(",") if "#" in t][-1]
            out.add(i.address + 8 + int(token.split("#")[-1], 16))
        except (IndexError, ValueError):
            continue
    return out


def classify(ins, size):
    """Bucket one unit by what its bytes are.

    Retail units are a function body followed by whatever the delink put in the
    same range: a literal pool (`body+pool`), padding (`zero-fill`), or another
    function's code (`body+code`, the carver lumped two together). A tail word
    counts as pool when one of the body's own `[pc, #n]` loads reads it, so the
    distinction does not depend on guessing from mnemonics.
    """
    if not ins or is_pad(ins):
        return "zero-fill"
    mn = [i.mnemonic for i in ins]
    ops = [i.op_str for i in ins]
    n = len(ins)
    real = [i for i in ins if not is_pad([i])]
    # `ldr rX, [pc, #n]; bx rX` -- an 8-byte interworking jump, then padding
    if n >= 2 and mn[0] == "ldr" and "[pc" in ops[0] and mn[1].startswith("bx")             and len(real) <= 3:
        return "veneer"
    if all(m in STACK_ONLY for m in [i.mnemonic for i in real]) and any(is_term(i) for i in real):
        return "swi-stub" if "swi" in mn else "stack-stub"
    if not opens_function(ins[0]):
        return "data"
    if sum(1 for i in ins if is_prologue(i)) > 1:
        return "multi-body"
    exits = [i for i, x in enumerate(ins) if is_term(x)]
    if not exits:
        exits = [i for i, x in enumerate(ins)
                 if mn[i].startswith("b") and mn[i] not in ("bx", "bl", "blx")]
    if not exits:
        return "no-return"
    pool = pc_literals(ins)
    tail = [t for t in ins[exits[-1] + 1:] if not is_pad([t])]
    if not tail:
        return "body"
    stray = [t for t in tail if t.address not in pool]
    if stray:
        return "body+code"
    return "body+pool"


def units_from_journal(path):
    seen = {}
    with io.open(path, encoding="utf-8") as f:
        for line in f:
            try:
                rec = json.loads(line)
            except ValueError:
                continue
            if rec.get("t") != "unit" or rec.get("status") != "no-candidate":
                continue
            seen[(rec["mod"], rec["addr"])] = (rec["name"], rec["size"])
    return [(m, a, s, n) for (m, a), (n, s) in sorted(seen.items())]


def units_from_roster(path):
    out = []
    with io.open(path, encoding="utf-8") as f:
        for line in f:
            parts = line.split()
            if len(parts) == 4:
                out.append((parts[0], int(parts[1], 16), int(parts[2]), parts[3]))
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--journal", default="build/grind-journal.jsonl")
    ap.add_argument("--roster", default="tools/match/true_fresh.txt")
    ap.add_argument("--all", action="store_true", help="use the roster, not the journal")
    ap.add_argument("--module", action="append", default=None)
    ap.add_argument("--write", default=None, help="write a TSV here")
    ap.add_argument("--top", type=int, default=12)
    args = ap.parse_args(argv)

    units = units_from_roster(args.roster) if args.all else units_from_journal(args.journal)
    if args.module:
        want = set(args.module)
        units = [u for u in units if u[0] in want]
    if not units:
        print("no units to classify")
        return 1

    rows = []
    counts = collections.Counter()
    bytess = collections.Counter()
    for mod, addr, size, name in units:
        blob = fm.retail(mod, addr, size)
        try:
            ins = list(fm.md.disasm(blob, addr))
        except Exception:
            ins = []
        tag = classify(ins, size)
        counts[tag] += 1
        bytess[tag] += size
        rows.append((mod, addr, size, name, tag, len(ins)))

    total = len(rows)
    total_b = sum(r[2] for r in rows)
    print("%d units, %d bytes" % (total, total_b))
    print("%-12s %6s %7s %9s" % ("bucket", "units", "%", "bytes"))
    for tag in BUCKETS:
        if not counts[tag]:
            continue
        print("%-12s %6d %6.1f%% %9d" % (tag, counts[tag], 100.0 * counts[tag] / total, bytess[tag]))

    big = [r for r in rows if r[4] in ("body", "body+pool")]
    print("")
    print("--- function bodies the generator could not express: %d units, %d bytes"
          % (len(big), sum(r[2] for r in big)))
    for mod, addr, size, name, tag, n in sorted(big, key=lambda r: -r[2])[:args.top]:
        print("  %-6s 0x%08x %5dB %-30s %-9s %d ins" % (mod, addr, size, name, tag, n))
    lumped = [r for r in rows if r[4] == "body+code"]
    if lumped:
        print("")
        print("--- units holding a second function after the first (%d):" % len(lumped))
        for mod, addr, size, name, tag, n in sorted(lumped, key=lambda r: -r[2])[:args.top]:
            print("  %-6s 0x%08x %5dB %-30s %d ins" % (mod, addr, size, name, n))

    if args.write:
        with io.open(args.write, "w", encoding="utf-8", newline="\n") as f:
            f.write("module\taddr\tsize\tname\tbucket\tins\n")
            for mod, addr, size, name, tag, n in rows:
                f.write("%s\t0x%08x\t%d\t%s\t%s\t%d\n" % (mod, addr, size, name, tag, n))
        print("\nwrote %s" % args.write)
    return 0


if __name__ == "__main__":
    sys.exit(main())
