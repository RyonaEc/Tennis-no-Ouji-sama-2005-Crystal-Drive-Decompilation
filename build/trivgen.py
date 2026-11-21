"""Generate C for dead-simple unmatched functions: no calls, no branches, base register r0 only.

Handles the shapes: field load->store copies, immediate stores, mov/add/and/orr/bic on a loaded
value, then bx lr. Prints MATCH lines; install with reg.py.
"""
import importlib.util, re, sys
from pathlib import Path

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

fm = load("build/fastmatch.py", "fm")
auto = load("build/auto.py", "auto")
LIMIT = int(sys.argv[1]) if len(sys.argv) > 1 else 150
MAXSZ = int(sys.argv[2]) if len(sys.argv) > 2 else 48
installed = {p.stem for p in Path("src").rglob("*.c")}
OK = {"ldr","str","ldrb","strb","ldrh","strh","mov","add","sub","and","orr","bic","eor","bx","lsl","lsr","asr","cmp","mvns","mvn"}
W = {"ldr":("u32",4),"str":("u32",4),"ldrb":("u8",1),"strb":("u8",1),"ldrh":("u16",2),"strh":("u16",2)}
tested = 0
for addr, (name, size) in sorted(auto.FUNCS.items()):
    if tested >= LIMIT: break
    if name in installed or size > MAXSZ or size < 8: continue
    try:
        fm.retail("arm9", addr, size); ins = fm.parse_ins(addr, size)
    except Exception:
        continue
    bxidx = [k for k, x in enumerate(ins) if x.mnemonic == "bx"]
    if not bxidx: continue
    ins = ins[:bxidx[-1] + 1]
    body = ins[:-1]
    if body and body[0].mnemonic in ("push", "stmdb"): body = body[1:]
    if body and body[-1].mnemonic in ("pop", "ldm"): body = body[:-1]
    if any(i.mnemonic not in OK for i in body): continue
    if any(i.mnemonic in ("b","bl","blx","beq","bne") for i in body): continue
    ok = True
    for i in body:
        if i.mnemonic in W and not re.match(r"r\d+, \[r0(, #0x[0-9a-f]+)?\]$", i.op_str.replace(" ", " ")):
            ok = False
    if not ok: continue
    stmts = []
    loaded = {}
    for i in body:
        m = re.match(r"(r\d+), \[r0(?:, #(0x[0-9a-f]+))?\]$", i.op_str)
        if i.mnemonic in ("ldr","ldrb","ldrh") and m:
            loaded[m.group(1)] = (i.mnemonic, int(m.group(2) or "0", 16))
            continue
        if i.mnemonic in ("str","strb","strh") and m:
            src, off = m.group(1), int(m.group(2) or "0", 16)
            if src not in loaded: ok = False; break
            lm, loff = loaded[src]
            t = W[i.mnemonic][0]
            stmts.append(f"    *({t} *)(p + {off:#x}) = *({t} *)(p + {loff:#x});")
            continue
        if i.mnemonic in ("mov","add","and","orr","bic","eor") and m and m.group(2) == "0" and re.match(r"r\d+, #0x[0-9a-f]+$", i.op_str.split(", ")[1] if ", " in i.op_str else ""):
            pass
        ok = False; break
    if not ok or not stmts: continue
    src = "void %s(u8 *p)\n{\n%s\n}\n" % (name, "\n".join(stmts))
    tested += 1
    try:
        if fm.compare(src, "arm9", addr, size):
            print(f"MATCH arm9 {addr:#x} {size}B {name}")
    except Exception:
        pass
print(f"tested {tested} generated functions")
