"""Verify all helper candidates (arm9 + overlays), lint them, install through reg.py."""
import importlib.util, re, subprocess, sys, shutil
from pathlib import Path

spec = importlib.util.spec_from_file_location("fm", "build/fastmatch.py")
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)

def names():
    n = set()
    for p in [Path("config/ATOJ/arm9/symbols.txt"), *Path("config/ATOJ/arm9/overlays").glob("*/symbols.txt")]:
        for l in p.read_text(errors="replace").splitlines():
            m = re.match(r"(\S+)\s+kind:", l)
            if m: n.add(m.group(1))
    return n

def tables():
    t = {}
    for p in [Path("config/ATOJ/arm9/symbols.txt"), *sorted(Path("config/ATOJ/arm9/overlays").glob("*/symbols.txt"))]:
        mod = "arm9" if p.parent.name == "arm9" else p.parent.name
        d = {}
        for l in p.read_text(errors="replace").splitlines():
            m = re.match(r"(\S+)\s+kind:function\S*\s+size=(0x[0-9a-f]+)\s+addr:(0x[0-9a-f]+)", l)
            if not m:
                m = re.match(r"(\S+)\s+kind:function\([^)]*size=(0x[0-9a-f]+)\)\s+addr:(0x[0-9a-f]+)", l)
            if m: d[int(m.group(3), 16)] = (m.group(1), int(m.group(2), 16))
        t[mod] = d
    return t

T = tables(); N = names()
installed = {p.stem for p in Path("src").rglob("*.c")}
todo = []
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
        body = f.read_text(errors="replace")
        # repair stale symbol names: data_XXXXXX -> data_0XXXXXX, func_ likewise
        def fix(mo):
            pre, rest = mo.group(1), mo.group(2)
            if pre + rest in N: return mo.group(0)
            cand = f"{pre}0{rest}"
            return cand if cand in N else mo.group(0)
        body = re.sub(r"\b(data_|func_)([0-9a-f]{7})\b", fix, body)
        b = body.replace('#include "types.h"', "")
        try:
            ok = fm.compare(b, mod, addr, size)
        except Exception:
            ok = False
        if ok:
            todo.append((mod, name, addr, size, f, body))
            installed.add(name)
print(f"{len(todo)} verified candidates", flush=True)
done = 0; tot = 0
for mod, name, addr, size, f, body in todo:
    dst = Path(f"build/cand/{name}.c")
    dst.write_text(body)
    r = subprocess.run([sys.executable, "build/reg.py", mod, name, hex(addr), str(size), str(dst)],
                       capture_output=True, text=True)
    good = "MATCH" in r.stdout and "True" in r.stdout
    print(f"  {'INSTALLED' if good else 'REJECTED '} {mod} {addr:#x} {size}B {name}", flush=True)
    if good: done += 1; tot += size
print(f"\n{done}/{len(todo)} installed, {tot} bytes")
