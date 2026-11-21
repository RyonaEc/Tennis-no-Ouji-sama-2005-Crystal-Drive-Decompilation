"""Verify every helper candidate against this tree's retail ROM - arm9 AND overlays."""
import importlib.util, re
from pathlib import Path

spec = importlib.util.spec_from_file_location("fm", "build/fastmatch.py")
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)

def tables():
    t = {}
    for p in [Path("config/ATOJ/arm9/symbols.txt"), *sorted(Path("config/ATOJ/arm9/overlays").glob("*/symbols.txt"))]:
        mod = "arm9" if p.parent.name == "arm9" else p.parent.name
        d = {}
        for l in p.read_text(errors="replace").splitlines():
            m = re.match(r"(\S+)\s+kind:function\S*\s+size=(0x[0-9a-f]+)\s+addr:(0x[0-9a-f]+)", l)
            if not m:
                m = re.match(r"(\S+)\s+kind:function\([^)]*size=(0x[0-9a-f]+)\)\s+addr:(0x[0-9a-f]+)", l)
            if m:
                d[int(m.group(3), 16)] = (m.group(1), int(m.group(2), 16))
        t[mod] = d
    return t

T = tables()
installed = {p.stem for p in Path("src").rglob("*.c")}
for tree in ("M:/ATOJ-scout", "M:/ATOJ-worker", "M:/ATOJ-delta"):
    cd = Path(tree, "build/cand")
    if not cd.exists(): continue
    for f in sorted(cd.glob("*.c")):
        m = re.match(r"func_(?:(ov\d{3})_)?([0-9a-f]{8})$", f.stem)
        if not m: continue
        mod = m.group(1) or "arm9"
        addr = int(m.group(2), 16)
        info = T.get(mod, {}).get(addr)
        if not info: continue
        name, size = info
        if name in installed: continue
        body = f.read_text(errors="replace").replace('#include "types.h"', "")
        try:
            if fm.compare(body, mod, addr, size):
                print(f"MATCH {mod} {addr:#x} {size}B {name} {f}")
        except Exception:
            pass
