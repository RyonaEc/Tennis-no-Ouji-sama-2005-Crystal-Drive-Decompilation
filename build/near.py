"""Near-twin propagation: same shape, different immediates.

A near twin has the template's size, mnemonics and relocation offsets, but
other immediates and pool constants. The values are mapped by position, refused
when one template value would map to two, then written into the template's
source and checked by compiling.

    python build/near.py [--install] [--limit N] [--min-size 0x20] [--mod M]
"""
import argparse
import importlib.util
import pathlib
import re


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


auto = load("build/auto.py", "auto")
fm = load("build/fastmatch.py", "fm")
t10 = load("build/t10.py", "t10")

HDR = "#include " + chr(34) + "types.h" + chr(34) + chr(10) * 2
IMM = re.compile(r"#(-?0x[0-9a-fA-F]+|-?\d+)")
NUM = re.compile(r"(?<![\w.])(-?\s*)?(0x[0-9a-fA-F]+|\d+)([uUlL]*)")


def modules():
    mods = ["arm9"]
    root = pathlib.Path("config/ATOJ/arm9")
    for d in sorted((root / "overlays").glob("ov*")):
        if (d / "delinks.txt").exists():
            mods.append(d.name)
    return mods


def reloc_map(mod):
    path = pathlib.Path(t10.delinks_for(mod)).parent / "relocs.txt"
    out = {}
    if path.exists():
        for line in path.read_text().splitlines():
            f = dict(kv.split(":", 1) for kv in line.split() if ":" in kv)
            if "from" in f and "to" in f:
                out[int(f["from"], 16)] = int(f["to"], 16)
    return out


def features(addr, size, rel):
    """(mnemonics, immediates in order, relocation target addresses in order)."""
    ins = fm.parse_ins(addr, size)
    sig, imms, offs, tgts = [], [], [], []
    for i in ins:
        sig.append(i.mnemonic)
        if "[pc" in i.op_str:
            try:
                word_at = i.address + 8 + int(i.op_str.replace("]", "").split(",")[-1].split("#")[-1], 16)
            except ValueError:
                continue
            if rel.get(word_at):
                offs.append(word_at - addr)
                tgts.append(rel[word_at])
            else:
                try:
                    imms.append(fm.literal_of(i))
                except Exception:
                    pass
            continue
        for m in IMM.findall(i.op_str):
            imms.append(int(m, 0))
    return tuple(sig), imms, tuple(offs), tgts


def rename_all(text, mapping):
    """Rename whole identifiers only; no boundary escapes needed."""
    if not mapping:
        return text
    return re.sub(r"[A-Za-z_]\w*", lambda m: mapping.get(m.group(0), m.group(0)), text)


def body_of(path):
    lines = pathlib.Path(path).read_text().splitlines()
    return "\n".join(l for l in lines if not l.lstrip().startswith("#include")).strip()


def render(text, val, scaled, min_val):
    subs = {a: b for a, b in val.items() if a != b and abs(a) >= min_val}
    if scaled:
        for a, b in list(subs.items()):
            for d in (4, 2):
                if a % d == 0 and b % d == 0 and abs(a) >= 2 * d and a // d not in subs:
                    subs[a // d] = b // d
    if not subs:
        return text

    def swap(m):
        sign, digits, suffix = m.group(1) or "", m.group(2), m.group(3)
        try:
            v = int(digits, 0)
        except ValueError:
            return m.group(0)
        if sign and sign.strip() == "-":
            v = -v
        if v not in subs:
            return m.group(0)
        return f"{subs[v]}{suffix}"

    return NUM.sub(swap, text)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--install", action="store_true")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--min-size", type=lambda v: int(v, 0), default=0x20)
    ap.add_argument("--mod")
    args = ap.parse_args()

    index = {}
    symbols, rels = {}, {}
    for mod in ([args.mod] if args.mod else modules()):
        try:
            auto.set_module(mod)
        except Exception:
            continue
        symbols[mod] = dict(auto.SYM)
        rels[mod] = reloc_map(mod)
        have = {p.stem for p in pathlib.Path(f"src/{mod}").glob("*.c")}
        for addr, (name, size) in auto.FUNCS.items():
            if size < args.min_size:
                continue
            sig, imms, offs, tgts = features(addr, size, rels[mod])
            bucket = index.setdefault((size, sig, offs), ([], []))
            entry = (mod, addr, name, size, imms, tgts)
            (bucket[0] if name in have else bucket[1]).append(entry)

    hits = tried = 0
    for (size, sig, offs), (tpls, cands) in index.items():
        if not tpls or not cands:
            continue
        tmod, taddr, tname, tsize, timms, ttgts = tpls[0]
        body = body_of(f"src/{tmod}/{tname}.c")
        for cmod, caddr, cname, csize, cimms, ctgts in cands:
            if len(timms) != len(cimms):
                continue
            val, ok = {}, True
            for a, b in zip(timms, cimms):
                if a in val and val[a] != b:
                    ok = False
                    break
                val[a] = b
            if not ok:
                continue
            smap = {}
            for a, b in zip(ttgts, ctgts):
                na, nb = symbols[tmod].get(a), symbols[cmod].get(b)
                if not na or not nb or (na in smap and smap[na] != nb):
                    smap = None
                    break
                smap[na] = nb
            if smap is None:
                continue
            smap[tname] = cname
            base = rename_all(body, {k: v for k, v in smap.items() if k != v})
            done = False
            for scaled in (False, True):
                for min_val in (0, 16):
                    tried += 1
                    text = render(base, val, scaled, min_val)
                    if not fm.compare(text, cmod, caddr, csize):
                        continue
                    pathlib.Path(f"build/cand/{cname}.c").write_text(HDR + text + chr(10))
                    print(f"  MATCH  {cmod} {caddr:#010x} {csize:3d} {cname:22} <- {tname}"
                          f"{' scaled' if scaled else ''}{' min16' if min_val else ''}", flush=True)
                    if args.install and t10.try_unit(cname, caddr, csize, text, mod=cmod):
                        print(f"  installed {cname}", flush=True)
                    hits += 1
                    done = True
                    break
                if done:
                    break
            if args.limit and hits >= args.limit:
                break
        if args.limit and hits >= args.limit:
            break

    print(f"{hits} near twins matched, {tried} variants tried, {sum(len(b[1]) for b in index.values())} candidates")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
