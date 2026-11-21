"""Print the instructions a C body compiles to next to the retail ones.

    python tools/match/probe.py <mod> <addr> <size> <file.c>

Used while shaping a single function: differs only in what the compiler chose,
so the listing shows the register allocation gap directly.
"""
import importlib.util
import sys


def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m


fm = load("tools/match/fastmatch.py", "fm")
auto = fm.auto


def disjoint(got, want):
    auto.set_module("arm9")
    return


def listing(blob, base):
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
    md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
    md.detail = True
    return [f"{i.mnemonic} {i.op_str}" for i in md.disasm(blob, base)]


def main():
    mod, addr, size, path = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0), sys.argv[4]
    body = open(path).read()
    auto.set_module(mod)
    got = fm.compile_text(body, "probe")
    want = fm.retail(mod, addr, size)
    gi, wi = listing(got, addr), listing(want, addr)
    for k in range(max(len(gi), len(wi))):
        g = gi[k] if k < len(gi) else ""
        w = wi[k] if k < len(wi) else ""
        print(("   " if g == w else ">> ") + f"{g:<34} | {w}")
    print("identical" if got == want else f"got {len(got)} bytes, want {len(want)}")


if __name__ == "__main__":
    main()
