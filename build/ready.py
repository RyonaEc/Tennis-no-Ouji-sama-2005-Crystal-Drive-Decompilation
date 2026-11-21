"""Rank unmatched functions by how ready they are to be written.

A function is *ready* when every function it calls is already matched: the
callee's signature and behaviour are known, so the body can be written in one
pass instead of guessed. Matching one function unlocks its callers, so working
the ready set in size order walks the call graph outward from what is done.

Usage:
    python build/ready.py [--mod arm9] [--limit N] [--max SIZE]
"""
import argparse
import collections
import importlib.util
import pathlib


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


auto = load("build/auto.py", "auto")
fm = load("build/fastmatch.py", "fm")


def framed(mod, addr, size):
    """True when retail's own prologue builds a stack frame.

    A function whose retail listing starts by pushing registers cannot usually be
    reproduced by a source shape whose mwcc build stays frameless: the frame is
    decided before allocation, so no spelling of the body removes it. Functions
    retail builds without a prologue are the reliable targets.
    """
    try:
        fm.retail(mod, addr, size)
        ins = fm.parse_ins(addr, size)
    except Exception:
        return None
    if not ins:
        return None
    first = f"{ins[0].mnemonic} {ins[0].op_str}"
    return first.startswith("push") or first.startswith("stmdb sp")


def thumb_names(mod):
    """Functions dsd decoded as Thumb: SDK `svc` wrappers, not writable in C."""
    for cand in (f"config/ATOJ/{mod}/symbols.txt",
                 f"config/ATOJ/arm9/{mod}/symbols.txt",
                 f"config/ATOJ/arm9/overlays/{mod}/symbols.txt"):
        path = pathlib.Path(cand)
        if path.exists():
            break
    else:
        return set()
    import re
    names = set()
    for line in path.read_text().splitlines():
        m = re.match(r"(\S+)\s+kind:function\(thumb", line)
        if m:
            names.add(m.group(1))
    return names


def walled(mod):
    """(addr -> reason) for functions already proven unreachable from C."""
    path = pathlib.Path("build/walls.txt")
    out = {}
    if path.exists():
        for line in path.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(None, 2)
            if len(parts) >= 2 and parts[0] == mod:
                out[int(parts[1], 16)] = parts[2] if len(parts) > 2 else ""
    return out


def calls(mod):
    """function address -> list of (site, target) call relocations in its body."""
    path = pathlib.Path(f"config/ATOJ/{mod}/relocs.txt")
    if not path.exists():
        path = pathlib.Path(f"config/ATOJ/arm9/overlays/{mod}/relocs.txt")
    text = path.read_text()
    sites = []
    for line in text.splitlines():
        if "kind:" not in line or "call" not in line.split("kind:")[1].split()[0]:
            continue
        from_addr = int(line.split("from:")[1].split()[0], 16)
        to_addr = int(line.split("to:")[1].split()[0], 16)
        sites.append((from_addr, to_addr))
    return sites


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mod", nargs="?", default="arm9")
    ap.add_argument("--limit", type=int, default=25)
    ap.add_argument("--max", type=lambda s: int(s, 0), default=0x100)
    ap.add_argument("--frameless", action="store_true",
                    help="only functions retail builds without a stack frame")
    args = ap.parse_args()

    auto.set_module(args.mod)
    have = {p.stem for p in pathlib.Path(f"src/{args.mod}").glob("*.c")}
    funcs = sorted((addr, name, size) for addr, (name, size) in auto.FUNCS.items())
    matched_addr = {addr for addr, name, _ in funcs if name in have}

    sites = calls(args.mod)
    by_func = collections.defaultdict(list)
    for from_addr, to_addr in sites:
        for addr, name, size in funcs:
            if addr <= from_addr < addr + size:
                by_func[addr].append(to_addr)
                break

    thumb = thumb_names(args.mod)
    walls = walled(args.mod)
    ready, blocked = [], []
    skipped_thumb = skipped_wall = 0
    for addr, name, size in funcs:
        if name in have or (args.max and size > args.max):
            continue
        if name in thumb:
            skipped_thumb += 1   # Thumb `svc` stubs: assembly in the SDK, no C form
            continue
        if addr in walls:
            skipped_wall += 1    # build/walls.txt: already proven unreachable
            continue
        callees = by_func[addr]
        unm = [t for t in callees if t not in matched_addr]
        (ready if not unm else blocked).append((addr, name, size, callees, unm))

    ready.sort(key=lambda r: (r[2], r[0]))
    if args.frameless:
        kept = []
        for addr, name, size, callees, unm in ready:
            if framed(args.mod, addr, size) is False:
                kept.append((addr, name, size, callees, unm))
        print(f"frameless ready: {len(kept)} of {len(ready)}")
        ready = kept
    print(f"{args.mod}: {len(have)} matched, {len(ready)} ready (all callees matched), "
          f"{len(blocked)} blocked")
    print(f"ready bytes: {sum(r[2] for r in ready)}; "
          f"blocked by one call: {sum(1 for b in blocked if len(b[4]) == 1)}; "
          f"skipped {skipped_thumb} thumb stub(s), {skipped_wall} wall(s)")

    bands = collections.Counter(r[2] for r in ready)
    print("ready sizes: " + ", ".join(f"{hex(s)}x{n}" for s, n in sorted(bands.items())[:8]))

    print(f"\n--- first {args.limit} ready ---")
    for addr, name, size, callees, _ in ready[:args.limit]:
        cal = " ".join(({t: name for _, name, _ in []}.get(t) or auto.FUNCS.get(t, (hex(t), 0))[0]) for t in callees) or "(no calls)"
        fr = framed(args.mod, addr, size)
        tag = "FRAMED" if fr else "free  "
        print(f"{tag} {size:3d}B {name}  callees: {cal}")

    print("\n--- blocked by exactly one call ---")
    for addr, name, size, callees, unm in sorted(
            (b for b in blocked if len(b[4]) == 1), key=lambda b: (b[2], b[0]))[:15]:
        print(f"{size:3d}B {name}  needs {auto.FUNCS.get(unm[0], (hex(unm[0]), 0))[0]}")


if __name__ == "__main__":
    main()
