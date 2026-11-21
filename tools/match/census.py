"""Census of ready units that look like genuine, self-contained functions."""
import importlib.util, sys
from pathlib import Path
spec = importlib.util.spec_from_file_location("fm", str(Path("tools/match/fastmatch.py")))
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)
spec2 = importlib.util.spec_from_file_location("auto", str(Path("tools/match/auto.py")))
auto = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(auto)
MODS = ["arm9","ov002","ov003","ov004","ov005","ov006","ov007","ov008","ov009","ov010","ov011","ov012","ov013"]
rows=[]
for mod in MODS:
    src = Path("src")/mod
    have = {p.stem for p in src.glob("*.c")} if src.exists() else set()
    for addr,(name,size) in auto.FUNCS.items():
        if name in have or size < 8: continue
        try:
            fm.retail(mod, addr, size)
            ins = fm.parse_ins(addr, size)
        except Exception:
            continue
        if not ins: continue
        last = f"{ins[-1].mnemonic} {ins[-1].op_str}".strip()
        ret_ok = last == "bx lr" or last.startswith(("ldm sp!","pop {","ldmia sp!"))
        txt = " ".join(f"{i.mnemonic} {i.op_str}" for i in ins)
        hi = any(r in txt for r in (" r8,"," sb,"," sl,"," fp,")) and not any(
            f"{i.mnemonic} {i.op_str}".startswith(("mov r8","mov sb","mov sl","mov fp","push")) for i in ins)
        if ret_ok and not hi:
            rows.append((size, mod, addr, name))
rows.sort()
print(f"{len(rows)} self-contained ready units")
from collections import Counter
c = Counter()
for size, mod, addr, name in rows:
    c[min(size//0x40*0x40, 0x200)] += 1
for k in sorted(c): print(f"  {k:4d}-{k+0x3f:4d}B : {c[k]}")
Path("build/ready_clean.txt").write_text("\n".join(f"{s} {m} {a:#x} {n}" for s,m,a,n in rows), newline="")
print("--- smallest 8:")
for s,m,a,n in rows[:8]:
    print(f"  {s:4d}B {m} {a:#x} {n}")
