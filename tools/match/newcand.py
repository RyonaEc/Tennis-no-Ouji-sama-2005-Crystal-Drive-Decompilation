import importlib.util, sys
from pathlib import Path
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
auto=load("tools/match/auto.py","auto"); t10=load("tools/match/t10.py","t10")
tried=set()
for line in Path("tools/match/tried.txt").read_text(errors="ignore").splitlines():
    parts=line.replace("\r","").split("\t")
    if len(parts)==2: tried.add(parts[1])
rows=[]
for mod in (sys.argv[1:] or ["arm9","ov002","ov003","ov004","ov008","ov010"]):
    auto.set_module(mod)
    src=Path(t10.srcdir_for(mod))
    have={p.stem for p in src.glob("*.c")} if src.exists() else set()
    for addr,(name,size) in auto.FUNCS.items():
        if name in have or name in tried: continue
        rows.append((size,mod,name,addr))
rows.sort()
for r in rows[:60]: print(f"{r[0]:5d} {r[1]:6s} {r[2]} 0x{r[3]:08x}")
print("total untried", len(rows))
