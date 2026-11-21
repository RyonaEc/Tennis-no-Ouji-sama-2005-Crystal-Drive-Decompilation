"""Re-probe parked walls under every compiler retail used.

`walls.txt` was written when every unit was assumed to be 1.2/sp2. Retail is a
split build, so a body parked before that was known may match under `1.2/b56`
with no rewrite. This reads the wall list, skips units that have a source,
finds candidate bodies in `build/cand` by address, and probes each under
`1.2/b56` then `1.2/sp2` with relocations masked. Nothing is installed.

    python tools/match/wallprobe.py [--mod arm9] [--install]

`--install` hands a matching body to `reg.py` with the compiler that produced it.
"""

import pathlib
import re
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import fastmatch as f

VERSIONS = ("1.2/b56", "1.2/sp2")
WALLS = pathlib.Path("tools/match/walls.txt")
CAND = pathlib.Path("build/cand")
MOD_RE = re.compile(r"\b(arm9|itcm|dtcm|ov\d{3}) 0x([0-9a-f]{8})")
BARE_RE = re.compile(r"^0x([0-9a-f]{8})")


def walls():
    """(module, addr) for every wall, in file order."""
    mod, out = None, []
    for line in WALLS.read_text(encoding="utf-8").splitlines():
        m = MOD_RE.search(line)
        if m:
            mod, addr = m.group(1), int(m.group(2), 16)
        else:
            m = BARE_RE.match(line)
            if not (m and mod):
                continue
            addr = int(m.group(1), 16)
        if (mod, addr) not in out:
            out.append((mod, addr))
    return out


def bodies(addr):
    pat = f"{addr:08x}"
    return sorted(p for p in CAND.glob("*.c") if pat in p.name)


def main(argv):
    wanted_mod = None
    install = "--install" in argv
    if "--mod" in argv:
        wanted_mod = argv[argv.index("--mod") + 1]

    entries = walls()
    stats = {"installed": 0, "unknown": 0, "no body": 0, "probed": 0}
    matches = []

    for mod, addr in entries:
        if wanted_mod and mod != wanted_mod:
            continue
        f.auto.set_module(mod)
        if addr not in f.auto.FUNCS:
            stats["unknown"] += 1
            continue
        name, size = f.auto.FUNCS[addr][:2]
        src = pathlib.Path("src") / mod / f"{name}.c"
        if src.exists():
            stats["installed"] += 1
            continue
        cands = bodies(addr)
        if not cands:
            stats["no body"] += 1
            continue
        want = f.retail(mod, addr, size)
        for path in cands:
            body = path.read_text(encoding="utf-8")
            for version in VERSIONS:
                blob = f.compile_text(body, "probe", f.mwcc_path(version))
                if not blob:
                    continue
                stats["probed"] += 1
                relocs = f.compile_text.last_relocs
                ok = blob == want or (bool(relocs) and
                                      f.masked(blob, relocs) == f.masked(want, relocs))
                if ok:
                    matches.append((mod, name, addr, size, version, path))
                    print(f"MATCH {version} {mod} {name} {addr:#x} {size}B {path}")
                    break
            else:
                continue
            break

    print("\n".join(
        f"{k}: {v}" for k, v in stats.items()) + f"\nmatches: {len(matches)}")

    if install:
        for mod, name, addr, size, version, path in matches:
            print(f"installing {name} ({version})")
            subprocess.run([sys.executable, "tools/match/reg.py", mod, name,
                            hex(addr), str(size), str(path), version], check=True)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
