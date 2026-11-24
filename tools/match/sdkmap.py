"""Map a module's unmatched functions onto the prebuilt NitroSDK libraries.

`tools/match/sdk.py` answers "what is this one unit?"; this answers "how much of
this module is SDK code, and which functions?". For every function still
delinked as assembly (a bare `.text` block in `delinks.txt`), the retail bytes
are searched in the SDK archives' ROM libraries. An `exact` row means the whole
function is byte-identical to a prebuilt library object, with the symbol name
from that object's symbol table when it can be resolved.

    python tools/match/sdkmap.py arm9
    python tools/match/sdkmap.py ov002 --root build/cand/sdk
    python tools/match/sdkmap.py arm9 --out build/sdkmap-arm9.tsv

Rows go to `build/sdkmap-<module>.tsv` by default (gitignored). A row is
evidence about provenance, never a match: `reg.py` and the ROM digest decide,
and an SDK unit still needs its source written (the SDK ships it under
`build/libraries/<lib>/src/`) before it can be installed.

Roots default to `build/cand/sdk`; see `notes/nitrosdk.md` for what to download
and how it is laid out.
"""

import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))


def module_config(module):
    if module == "arm9":
        return "config/ATOJ/arm9"
    return "config/ATOJ/arm9/overlays/" + module


def ar_members(data):
    if not data.startswith(b"!<arch>\n"):
        return
    i = 8
    while i + 60 <= len(data):
        hdr = data[i:i + 60]
        name = hdr[:16].decode("latin-1").strip().rstrip("/")
        try:
            size = int(hdr[48:58].decode("latin-1").strip())
        except ValueError:
            return
        yield name, data[i + 60:i + 60 + size]
        i += 60 + size + (size & 1)


def elf_funcs(blob):
    """Global FUNC symbols and executable sections of an ELF32 little-endian object."""
    if blob[:4] != b"\x7fELF":
        return [], []
    shoff, = struct.unpack_from("<I", blob, 0x20)
    shentsize, shnum, _shstrndx = struct.unpack_from("<HHH", blob, 0x2e)
    secs = [struct.unpack_from("<IIIIIIIIII", blob, shoff + i * shentsize) for i in range(shnum)]
    execs = [(s[4], s[5], s[3]) for s in secs if s[2] & 4 and s[1] != 8]
    symtab = next((s for s in secs if s[1] == 2), None)
    funcs = []
    if symtab:
        strbase = secs[symtab[6]][4]
        base, size, ent = symtab[4], symtab[5], symtab[9] or 16
        for o in range(base, base + size, ent):
            nameoff, value, sz, info, _other, _shndx = struct.unpack_from("<IIIBBH", blob, o)
            if (info & 0xf) != 2:
                continue
            end = blob.index(b"\0", strbase + nameoff)
            funcs.append((blob[strbase + nameoff:end].decode("latin-1"), value, sz))
    return funcs, execs


def symbol_at(blob, off):
    """(name, size) of the SDK symbol starting at this object offset, if any."""
    funcs, execs = elf_funcs(blob)
    for sh_off, sh_size, sh_addr in execs:
        if sh_off <= off < sh_off + sh_size:
            vaddr = sh_addr + (off - sh_off)
            cands = [(n, v, s) for n, v, s in funcs
                     if not n.startswith("$") and (v == vaddr or (s and v <= vaddr < v + s))]
            if not cands:
                return None, 0
            exact = [c for c in cands if c[1] == vaddr]
            pick = exact[0] if exact else min(cands, key=lambda c: c[2])
            return pick[0], pick[2]
    return None, 0


def load_releases(roots, arm9_rom_only=True):
    """{release label: (blob, member index)} over the ARM9 release libraries."""
    out = {}
    for root in roots:
        for dirpath, _dirnames, filenames in os.walk(root):
            tail = dirpath.replace("\\", "/")
            if arm9_rom_only and not tail.endswith("lib/ARM9-TEG/Rom"):
                continue
            if "/demos/" in tail or "/build/tools" in tail:
                continue  # sample and tool libraries are not linked into a retail ROM
            libs = sorted(f for f in filenames if f.endswith(".a"))
            if not libs:
                continue
            label = os.path.relpath(dirpath, root).replace("\\", "/").split("/")[0]
            blob = bytearray()
            index = []
            for fn in libs:
                data = open(os.path.join(dirpath, fn), "rb").read()
                for name, payload in ar_members(data) or ():
                    index.append((len(blob), len(payload), fn, name, payload))
                    blob += payload
            if label in out:  # several SDK10 layouts can define the same label
                prev_blob, prev_index = out[label]
                base = len(prev_blob)
                index = [(o + base, s, l, m, p) for o, s, l, m, p in index]
                out[label] = (prev_blob + bytes(blob), prev_index + index)
            else:
                out[label] = (bytes(blob), index)
    return out


def owner(index, off):
    for start, size, lib, member, payload in index:
        if start <= off < start + size:
            return lib, member, payload, off - start
    return None, None, None, None


def unmatched_functions(cfg):
    ranges = []
    cur_src = False
    for line in open(os.path.join(cfg, "delinks.txt"), encoding="utf-8"):
        s = line.strip()
        if re.match(r"^\S.*:$", s):
            cur_src = s.startswith("src/")
            continue
        m = re.match(r"^\.text\s+start:(0x[0-9a-f]+)\s+end:(0x[0-9a-f]+)", s)
        if m and not cur_src:
            a, b = int(m.group(1), 16), int(m.group(2), 16)
            if b > a:
                ranges.append((a, b))

    def inside(addr):
        return any(a <= addr < b for a, b in ranges)

    out = []
    for line in open(os.path.join(cfg, "symbols.txt"), encoding="utf-8"):
        m = re.match(r"^(\w+) kind:function\((\w+),size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)", line.strip())
        if m:
            size, addr = int(m.group(3), 16), int(m.group(4), 16)
            if size and inside(addr):
                out.append((addr, size, m.group(1), m.group(2)))
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("module")
    ap.add_argument("--root", action="append", help="SDK tree (default build/cand/sdk)")
    ap.add_argument("--out", help="TSV path (default build/sdkmap-<module>.tsv)")
    ap.add_argument("--top", type=int, default=15)
    args = ap.parse_args(argv)

    roots = args.root or ["build/cand/sdk"]
    roots = [r for r in roots if os.path.isdir(r)]
    if not roots:
        print("no SDK tree under %s; see notes/nitrosdk.md" % ", ".join(roots))
        return 2

    import fastmatch as fm

    funcs = unmatched_functions(module_config(args.module))
    releases = load_releases(roots)
    print("%s: %d unmatched functions, %d bytes" % (args.module, len(funcs), sum(f[1] for f in funcs)))
    print("searched %d library sets (%s)" % (len(releases), ", ".join(sorted(releases))))

    rows = []
    for addr, size, name, kind in funcs:
        code = fm.retail(args.module, addr, size)
        hit = []
        lib = member = sym = None
        symsize = 0
        for label, (blob, index) in releases.items():
            pos = blob.find(code)
            if pos != -1:
                hit.append(label)
                lib, member, payload, off = owner(index, pos)
                sym, symsize = symbol_at(payload, off)
        if hit:
            rows.append((addr, size, name, kind, lib, member, sym, symsize, sorted(hit)))

    print("exact SDK hits: %d functions, %d bytes" % (len(rows), sum(r[1] for r in rows)))
    if rows:
        multi = [r for r in rows if len(r[8]) == 1]
        print("  unique to one release: %d" % len(multi))
        for label in sorted(releases):
            n = sum(1 for r in rows if r[8] == [label])
            b = sum(r[1] for r in rows if r[8] == [label])
            print("    only %-18s %d functions, %d bytes" % (label, n, b))
        libs = {}
        for r in rows:
            libs[(r[4] or "?").replace(".a", "")] = libs.get((r[4] or "?").replace(".a", ""), 0) + 1
        print("  by library: " + ", ".join("%s=%d" % kv for kv in sorted(libs.items(), key=lambda kv: -kv[1])))
        for r in sorted(rows, key=lambda r: -r[1])[:args.top]:
            print("  0x%08x %4dB %-28s %s(%s) sdk_size=%d" % (
                r[0], r[1], r[6] or r[2], (r[4] or "?").replace(".a", ""), r[5] or "?", r[7]))

    out = args.out or "build/sdkmap-%s.tsv" % args.module
    with open(out, "w", encoding="utf-8") as f:
        f.write("addr\tsize\tname\tisa\tlib\tmember\tsdk_symbol\treleases\n")
        for r in sorted(rows):
            f.write("0x%08x\t%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\n" % (
                r[0], r[1], r[2], r[3], r[4] or "?", r[5] or "?", r[6] or "?", r[7], ",".join(r[8])))
    print("wrote", out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
