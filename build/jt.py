"""Scan ready units for jump-table switches and classify them."""
import importlib.util, sys
from pathlib import Path

spec = importlib.util.spec_from_file_location("fm", str(Path("build/fastmatch.py")))
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)
spec2 = importlib.util.spec_from_file_location("auto", str(Path("build/auto.py")))
auto = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(auto)

MODS = ["arm9","ov002","ov003","ov004","ov005","ov006","ov007","ov008","ov009","ov010","ov011","ov012","ov013"]
rows = []
for mod in MODS:
    src = Path("src")/mod
    have = {p.stem for p in src.glob("*.c")} if src.exists() else set()
    for addr,(name,size) in sorted(auto.FUNCS.items(), key=lambda kv: kv[1][1]):
        if name in have: continue
        try:
            fm.retail(mod, addr, size)
            ins = fm.parse_ins(addr, size)
        except Exception:
            continue
        if not ins: continue
        txt = [f"{i.mnemonic} {i.op_str}" for i in ins]
        if not any(t.startswith("addls pc") for t in txt): continue
        # pure map: every block between the table end and the epilogue is mov-imm only
        n_mov = sum(1 for t in txt if t.startswith("mov r") and "#" in t)
        n_b   = sum(1 for t in txt if t == "b " or t.startswith("b #"))
        calls = sum(1 for t in txt if t.startswith("bl "))
        rows.append((size, mod, addr, name, n_mov, n_b, calls))
rows.sort()
print(f"{len(rows)} jump-table units")
for size, mod, addr, name, nm, nb, calls in rows:
    print(f"  {size:5d}B {mod:6s} {addr:#010x} {name:34s} mov={nm:2d} b={nb:2d} bl={calls}")
