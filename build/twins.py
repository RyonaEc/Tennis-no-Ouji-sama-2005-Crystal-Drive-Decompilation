"""Find unmatched functions whose retail mnemonic multiset matches an already-matched one."""
import importlib.util, re
from collections import Counter
from pathlib import Path
spec = importlib.util.spec_from_file_location("fm", "build/fastmatch.py")
fm = importlib.util.module_from_spec(spec); spec.loader.exec_module(fm)

def funcs(mod):
    p = Path("config/ATOJ/arm9/symbols.txt") if mod == "arm9" else Path(f"config/ATOJ/arm9/overlays/{mod}/symbols.txt")
    out = []
    for l in p.read_text(errors="replace").splitlines():
        m = re.match(r"(\S+)\s+kind:function\S*\s+size=(0x[0-9a-f]+)\s+addr:(0x[0-9a-f]+)", l)
        if not m:
            m = re.match(r"(\S+)\s+kind:function\([^)]*size=(0x[0-9a-f]+)\)\s+addr:(0x[0-9a-f]+)", l)
        if m: out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out

mods = ["arm9", "itcm", "dtcm"] + [f"ov{i:03d}" for i in range(11)]
groups = {}
for mod in mods:
    try: fs = funcs(mod)
    except Exception: continue
    for name, size, addr in fs:
        if size < 16: continue
        try:
            fm.retail(mod, addr, size); ins = fm.parse_ins(addr, size)
        except Exception: continue
        groups.setdefault((mod, size, frozenset(Counter(i.mnemonic for i in ins).items())), []).append((name, addr, size))

matched = lambda mod, n: Path(f"src/{mod}/{n}.c").exists()
mixed = []
for (mod, size, sig), members in groups.items():
    have = [m for m in members if matched(mod, m[0])]
    lack = [m for m in members if not matched(mod, m[0])]
    if have and lack: mixed.append((mod, have[0], lack))
print(f"{len(mixed)} matched/unmatched twin groups")
for mod, h, lack in sorted(mixed, key=lambda x: -x[1][2]):
    print(f"  {mod} {h[2]:>4}B matched {h[0]} -> " + ", ".join(f"{x[0]}" for x in lack))
