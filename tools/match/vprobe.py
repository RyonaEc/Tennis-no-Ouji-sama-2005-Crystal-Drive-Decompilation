"""Probe one candidate under every compiler retail used, in one run.

Usage: python tools/match/vprobe.py <mod> <addr> <size> <file.c> [file.c ...]
Prints MATCH or the first differing instructions for 1.2/b56 and 1.2/sp2.
"""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import fastmatch as f

VERSIONS = ("1.2/b56", "1.2/sp2")


def summary(blob, want, addr):
    got = [(i.address - addr, i.mnemonic, i.op_str) for i in f.md.disasm(blob, addr)]
    ref = [(i.address - addr, i.mnemonic, i.op_str) for i in f.parse_ins(addr, len(want))]
    out = []
    for n in range(max(len(got), len(ref))):
        g = got[n] if n < len(got) else None
        w = ref[n] if n < len(ref) else None
        if w is not None and g is not None and w[1:] == g[1:]:
            continue
        out.append("    %03x want %-20s got %s" % (
            (w or g)[0],
            "(end)" if w is None else f"{w[1]} {w[2]}",
            "(end)" if g is None else f"{g[1]} {g[2]}"))
        if len(out) == 6:
            break
    return out


def main(argv):
    mod, addr, size = argv[0], int(argv[1], 0), int(argv[2], 0)
    want = f.retail(mod, addr, size)
    for path in argv[3:]:
        body = open(path, encoding="utf-8").read()
        print("== %s %s %#x %dB" % (path, mod, addr, size))
        for version in VERSIONS:
            blob = f.compile_text(body, "probe", f.mwcc_path(version))
            if not blob:
                print("  %-9s compile error" % version)
                continue
            relocs = f.compile_text.last_relocs
            if blob == want or (relocs and f.masked(blob, relocs) == f.masked(want, relocs)):
                print("  %-9s MATCH" % version)
                break
            print("  %-9s %dB" % (version, len(blob)))
            for line in summary(blob, want, addr):
                print(line)


if __name__ == "__main__":
    main(sys.argv[1:])
