"""Generate pure-map switch candidates from a retail jump table."""
import importlib.util, sys
from pathlib import Path

spec = importlib.util.spec_from_file_location("fm", str(Path("build/fastmatch.py")))
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)

def glyph(ins):
    return f"{ins.mnemonic} {ins.op_str}"

def gen_body(mod, addr, size, name):
    fm.retail(mod, addr, size)
    ins = fm.parse_ins(addr, size + 0x800)
    if not ins:
        return None, "no disasm"
    k = None
    for j, i in enumerate(ins):
        if i.address >= addr + size:
            break
        if i.mnemonic == "addls" and "pc, pc" in i.op_str:
            k = j
            break
    if k is None:
        return None, "no table"
    # subject register from 'addls pc, pc, rK, lsl #2'
    op = ins[k].op_str.replace(" ", "")
    parts = op.split(",")
    if len(parts) < 3:
        return None, "bad addls"
    sub = parts[2]
    if sub == "pc":
        return None, "bad sub"
    # number of entries: from cmp before the table
    n = None
    for j in range(k - 1, -1, -1):
        g = glyph(ins[j])
        if g.startswith("cmp "):
            try:
                n = int(g.split("#")[1], 0) + 1
            except Exception:
                n = None
            break
    if n is None or n > 64:
        return None, "no range"
    by_addr = {i.address: i for i in ins}
    table = ins[k + 2: k + 2 + n]
    if len(table) < n:
        return None, "short table"
    blocks = {}
    for c, i in enumerate(table):
        if i.mnemonic != "b" or "#" not in i.op_str:
            return None, f"entry {c} not b"
        tgt = int(i.op_str.split("#")[1], 0)
        if tgt not in by_addr:
            return None, "tgt outside"
        blocks.setdefault(tgt, []).append(c)
    # classify: value block is `mov rD,#imm` followed by `b #epi` or a fall-through into epi
    from collections import Counter
    bcnt = Counter()
    for tgt in blocks:
        a = tgt
        for _ in range(4):
            i = by_addr.get(a)
            if i is None:
                break
            if i.mnemonic == "b" and "#" in i.op_str:
                bcnt[int(i.op_str.split("#")[1], 0)] += 1
                break
            if i.mnemonic == "bx" or (i.mnemonic == "ldm" and "pc" in i.op_str):
                break
            a += 4
    epi = bcnt.most_common(1)[0][0] if bcnt else None
    vals = {}
    for tgt in list(blocks):
        if tgt == epi:
            del blocks[tgt]
            continue
        i0 = by_addr.get(tgt)
        i1 = by_addr.get(tgt + 4)
        if i0 is None or i0.mnemonic != "mov" or "#" not in i0.op_str or i1 is None:
            return None, f"block {tgt:#x} not pure"
        if i1.mnemonic == "b" and "#" in i1.op_str:
            if int(i1.op_str.split("#")[1], 0) != epi:
                return None, f"block {tgt:#x} other target"
        elif tgt + 4 != epi:
            return None, f"block {tgt:#x} no tail"
        vals[tgt] = i0.op_str.split("#")[1]
    if not vals:
        return None, "no value blocks"
    order = sorted(vals)
    body = [f"\nu32 {name}(u32 a0)\n{{\n    u32 r;\n\n    r = 0;\n    switch (a0) {{\n"]
    for tgt in order:
        cases = sorted(blocks[tgt])
        for c in cases:
            body.append(f"    case {c}:\n")
        body.append(f"        r = {vals[tgt]};\n        break;\n")
    body.append("    }\n    return r;\n}\n")
    return "".join(body), "ok"

def main():
    args = sys.argv[1:]
    mod, addr, size, name = args[0], int(args[1], 0), int(args[2], 0), args[3]
    body, why = gen_body(mod, addr, size, name)
    if body is None:
        print(f"SKIP {mod} {addr:#x}: {why}")
        return 1
    if len(args) > 4:
        Path(args[4]).write_text(body, newline="")
    ok = fm.compare(body, mod, addr, size)
    print(f"{'MATCH' if ok else 'diff '} {mod} {addr:#x} {size}B {name}")
    if not ok:
        print(fm.diff_summary(mod, addr, size, body)[:1200])
    if ok and len(args) > 4:
        print(f"  body saved to {args[4]}")
    return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())
