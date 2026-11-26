"""Bridge a retail function into m2c, so a draft C body exists before hand work.

m2c (<https://github.com/matt-kempster/m2c>) lifts assembly to C with a
matching-decompilation bias, and it has an ARM backend with an mwcc flavour.
This project has no objdump, so the listing has to be produced from the retail
bytes: `annotate.py` already knows every instruction, its pooled literal, the
symbol that literal names and each callee's name, which is exactly what an m2c
listing wants.

The output is a **draft**, never a candidate: m2c's own README says its C is a
starting point, and this project's rule is that only `reg.py` plus the ROM digest
accept a unit. Use it for the two things that cost the most time - the shape of a
200-instruction function and the struct fields it touches.

Usage:
    python tools/match/lift.py <module> 0xADDR [size] [--keep] [--raw]

Needs a checkout of m2c at build/m2c (gitignored):

    git clone --depth 1 https://github.com/matt-kempster/m2c build/m2c
"""
import importlib.util
import os
import pathlib
import re
import subprocess
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
auto = fm.auto

M2C = pathlib.Path("build/m2c/m2c.py")
OUT = pathlib.Path("build/cand")


def resolve(addr, names):
    """Symbol name for an address, or func_<addr>."""
    if addr in names:
        return names[addr]
    sym = auto.SYMS.get(addr) if hasattr(auto, "SYMS") else None
    return sym or "func_%08x" % addr


BRANCH = re.compile(r"b(eq|ne|lt|le|gt|ge|hi|hs|lo|ls|mi|pl|vs|vc|cs|cc)?$|bl$|bx$|blx$")


def pool_addr(ins, size):
    """Address of the word a pc-relative load reads."""
    token = [t for t in ins.op_str.replace("]", "").split(",") if "#" in t][-1]
    offset = int(token.split("#")[-1], 16)
    return ins.address + 8 + offset


def branch_target(op_str):
    m = re.search(r"#?(0x[0-9a-f]+)", op_str)
    return int(m.group(1), 16) if m else None


def listing(mod, addr, size, name):
    """m2c-style .s for one function: labels, instructions, pool comments."""
    ins = fm.parse_ins(addr, size)
    if not ins:
        raise SystemExit("no instructions at 0x%08x" % addr)
    end = addr + size
    names = {a: n for a, (n, s) in auto.FUNCS.items()} if hasattr(auto, "FUNCS") else {}

    # every in-range branch target becomes a label, so m2c can build a CFG
    labels = {}
    for i in ins:
        if i.mnemonic.startswith("b") and "[pc" not in i.op_str:
            t = branch_target(i.op_str)
            if t is not None and addr <= t < end:
                labels[t] = ".L%08x" % t

    pools = {}
    pool_slots = set()
    for i in ins:
        if "[pc" in i.op_str:
            pool_slots.add(pool_addr(i, size))
    lines = [
        ".text",
        ".arm",
        ".align 2, 0",
        ".global %s" % name,
        ".type %s,function" % name,
        "%s:" % name,
    ]
    for i in ins:
        if i.address in pool_slots:
            continue  # literal pool word, not code
        if i.address in labels:
            lines.append("%s:" % labels[i.address])
        mn, op = i.mnemonic, i.op_str
        # aliases m2c's ARM parser does not know
        if mn == "push":
            mn, op = "stmdb", "sp!, " + op
        elif mn == "pop":
            mn, op = "ldmia", "sp!, " + op
        elif mn == "stm":
            mn = "stmia"
        elif mn == "ldm":
            mn = "ldmia"
        # a pc-relative load becomes a reference to a .word after the function,
        # which is the only shape m2c can turn back into a constant
        if "[pc" in op:
            word = fm.literal_of(i)
            pool = pool_addr(i, size)
            pools[pool] = word
            lines.append("/* %x */ %s %s" % (i.address - addr, mn,
                                             re.sub(r"\[pc[^\]]*\]", ".LC_%08x" % pool, op)))
            continue
        # branches and calls address a label inside the unit or a symbol outside.
        # Match the branch mnemonics exactly: `bic r5, r5, #0xf000` also starts
        # with "b" and carries an immediate, and rewriting its operands as a
        # symbol produced a listing m2c could not parse.
        if BRANCH.fullmatch(mn) and "[" not in op and "#" in op:
            t = branch_target(op)
            if t is not None:
                op = labels[t] if (addr <= t < end) else resolve(t, names)
        lines.append("/* %x */ %s %s" % (i.address - addr, mn, op))
    lines.append(".size %s,.-%s" % (name, name))
    for pool in sorted(pools):
        word = pools[pool]
        tgt = resolve(word, names) if word is not None else None
        lines.append(".LC_%08x: .word 0x%08x /* %s */" %
                     (pool, word or 0, tgt or ""))
    return "\n".join(lines) + "\n"


def run_m2c(path, name):
    if not M2C.exists():
        raise SystemExit(
            "m2c is not checked out; run:\n"
            "  git clone --depth 1 https://github.com/matt-kempster/m2c build/m2c"
        )
    cmd = [
        sys.executable,
        str(M2C),
        "-t", "arm-mwcc-c",
        "--valid-syntax",
        "--pointer-style", "right",
        "--comment-style", "oneline",
        "-f", name,
        str(path),
    ]
    p = subprocess.run(cmd, capture_output=True, text=True)
    return p.returncode, p.stdout, p.stderr


def main(argv):
    if len(argv) < 3:
        raise SystemExit(__doc__)
    mod = argv[1]
    addr = int(argv[2], 0)
    auto.set_module(mod)
    name, size = auto.FUNCS.get(addr, (None, 0))
    if not name:
        raise SystemExit("0x%08x is not a function start in %s" % (addr, mod))
    if len(argv) > 3 and not argv[3].startswith("-"):
        size = int(argv[3], 0)
    keep = "--keep" in argv

    OUT.mkdir(parents=True, exist_ok=True)
    asm_path = OUT / ("lift_%s.s" % name)
    asm_path.write_text(listing(mod, addr, size, name), newline="\n")

    code, out, err = run_m2c(asm_path, name)
    if code != 0 and not out.strip():
        print("m2c failed (exit %d)" % code)
        print(err.strip()[:4000])
        print("\nlisting kept at %s" % asm_path)
        return 1
    if err.strip():
        print("m2c notes:")
        print(err.strip()[:1500])
    print(out if "--raw" in argv else out.rstrip())
    print("\n/* draft only: m2c output, not a candidate. Verify against"
          " tools/match/annotate.py and gate with reg.py. */")
    if not keep:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
