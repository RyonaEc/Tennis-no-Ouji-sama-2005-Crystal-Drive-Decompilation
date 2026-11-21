"""Find unmatched functions whose instruction skeleton matches a matched one.

twins.py works inside a module; the overlays share a lot of code, so the same
skeleton often spans modules. A group is worth deriving when one member is
already matched: the shape comes from the matched unit, the constants from the
unmatched unit's own disassembly (never from the twin - see notes 2.50).
"""
import importlib.util
import re
import sys
from collections import defaultdict
from pathlib import Path

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

spec = importlib.util.spec_from_file_location("fm", "build/fastmatch.py")
fm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fm)
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
RX = re.compile(r"^(\S+)\s+kind:function\([^,]*,\s*size=(0x[0-9a-f]+)\)\s+addr:(0x[0-9a-f]+)")

MODULES = ["arm9"] + [f"ov{i:03d}" for i in range(11)]


def functions(mod):
    path = Path("config/ATOJ/arm9/symbols.txt") if mod == "arm9" else Path(f"config/ATOJ/arm9/overlays/{mod}/symbols.txt")
    out = []
    for line in path.read_text(errors="replace").splitlines():
        m = RX.match(line)
        if not m:
            continue
        name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
        if size == 0 or size > 0x200:
            continue
        out.append((addr, size, name))
    return out


def skeleton(mod, addr, size):
    try:
        fm.retail(mod, addr, size)
        ins = fm.parse_ins(addr, size)
    except Exception:
        return None
    # Mnemonics plus operand shape, with all immediates and registers masked, so
    # only the instruction pattern and memory/offset form remain.
    key = []
    for i in ins:
        ops = re.sub(r"#?(0x[0-9a-f]+|\d+)", "#", i.op_str)
        ops = re.sub(r"\b(r\d+|ip|lr|sp|pc)\b", "R", ops)
        key.append(f"{i.mnemonic} {ops}")
    return tuple(key)


def main() -> int:
    groups = defaultdict(lambda: {"matched": [], "open": []})
    for mod in MODULES:
        for addr, size, name in functions(mod):
            key = skeleton(mod, addr, size)
            if key is None:
                continue
            bucket = groups[key]
            entry = f"{mod} 0x{addr:08x} {size:>4}B {name}"
            (bucket["matched"] if Path(f"src/{mod}/{name}.c").exists() else bucket["open"]).append(entry)
    rows = [(len(b["open"]), len(b["matched"]), b) for b in groups.values()
            if b["matched"] and b["open"]]
    rows.sort(key=lambda r: (r[0], -r[1]))
    print(f"{len(rows)} groups with a matched member and open members "
          f"({sum(r[0] for r in rows)} open functions)")
    for nopen, nmatched, b in rows[: int(sys.argv[1]) if len(sys.argv) > 1 else 12]:
        print(f"  {nopen} open / {nmatched} matched   matched: {b['matched'][0]}")
        for e in b["open"][:4]:
            print(f"      open: {e}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
