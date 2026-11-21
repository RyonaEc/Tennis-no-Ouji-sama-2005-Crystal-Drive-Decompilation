import importlib.util, re
from pathlib import Path
spec = importlib.util.spec_from_file_location("g", str(Path("tools/match/jtgen.py")))
g = importlib.util.module_from_spec(spec); spec.loader.exec_module(g)
fm = g.fm
rows = []
for line in Path("tools/match/jt.txt").read_text().splitlines()[1:]:
    m = re.search(r"(\d+)B\s+(\w+)\s+(0x[0-9a-f]+)\s+(\S+)", line)
    if m:
        rows.append((int(m.group(1)), m.group(2), int(m.group(3), 0), m.group(4)))
matches, diffs, skips = [], [], 0
for size, mod, addr, name in sorted(rows, reverse=True):
    try:
        body, why = g.gen_body(mod, addr, size, name)
    except Exception as e:
        skips += 1; continue
    if body is None:
        skips += 1; continue
    try:
        ok = fm.compare(body, mod, addr, size)
    except Exception:
        skips += 1; continue
    if ok:
        matches.append((size, mod, addr, name, body))
        print(f"MATCH {size:5d}B {mod} {addr:#x} {name}", flush=True)
    else:
        diffs.append((size, mod, addr, name))
print(f"\n{len(matches)} matches, {len(diffs)} generated-but-diff, {skips} unparseable")
Path("build/jtmatch.json").write_text(__import__("json").dumps(
    [{"size":s,"mod":m,"addr":a,"name":n,"body":b} for s,m,a,n,b in matches], indent=1), newline="")
for d in diffs[:12]:
    print(f"  diff {d[0]:5d}B {d[1]} {d[2]:#x} {d[3]}")
