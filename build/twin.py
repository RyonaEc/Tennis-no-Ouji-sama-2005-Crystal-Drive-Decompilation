"""Regenerate untried functions from matched siblings with the same shape.

Usage:
    python build/twin.py [--write] [--all-mods] [--min B] [--max B]

For every untried function, find a matched function whose instruction
skeleton (mnemonics plus operands with constants normalised away) is
identical. Copy that sibling's source, substitute only the immediates that
belong to the instructions themselves -- never pc-relative pool offsets or
branch targets, which legitimately differ between siblings -- rename the
callee when the call target moved, and check the result with var3.

Without --write nothing is installed; candidates land in build/cand.
"""

import importlib.util
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SPEC = importlib.util.spec_from_file_location(
    "fm", os.path.join(ROOT, "build", "fastmatch.py")
)
fm = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(fm)


def ins(addr, size):
    return [(i.mnemonic, i.op_str) for i in fm.parse_ins(addr, size)]


def normalise(text):
    return re.sub(r"\b\d+\b", "#", re.sub(r"0x[0-9a-f]+", "#", text))


def skel(items):
    return tuple(normalise(m + " " + o) for m, o in items)


def literal_imms(items):
    out = []
    for m, o in items:
        if "[pc" in o or m in ("b", "bl", "blx", "bx") or m.startswith("b"):
            out.append([])
        else:
            out.append(re.findall(r"#(0x[0-9a-f]+|\d+)", o))
    return out


def subtag(name):
    return re.sub(r"^(func_ov\d+_|func_)", "", name)


def generate(mod, tpl_name, tpl_items, name, items):
    txt = open(
        os.path.join(ROOT, "src", mod, tpl_name + ".c"), encoding="utf-8"
    ).read()
    subs = []
    for old, new in zip(literal_imms(tpl_items), literal_imms(items)):
        for a, b in zip(old, new):
            if a != b:
                subs.append((a, b))
    for i, (a, _) in enumerate(subs):
        num = int(a, 0)
        for pat in (r"0x%x\b" % num, r"(?<![\w.])%d(?![\w.])" % num):
            if re.search(pat, txt):
                txt = re.sub(pat, "@@%d@@" % i, txt, count=1)
                break
    for i, (_, b) in enumerate(subs):
        txt = txt.replace("@@%d@@" % i, "0x%x" % int(b, 0))
    for (m1, o1), (m2, o2) in zip(tpl_items, items):
        if m1 == "bl" and m2 == "bl":
            try:
                ta = int(o1.split("#")[-1], 0)
                tb = int(o2.split("#")[-1], 0)
            except ValueError:
                continue
            if ta != tb:
                na = fm.auto.FUNCS.get(ta, ("", 0))[0]
                nb = fm.auto.FUNCS.get(tb, ("", 0))[0]
                if na and nb:
                    txt = txt.replace(na, nb)
    txt = txt.replace(tpl_name, name)
    if "#include" not in txt:
        txt = '#include "types.h"\n\n' + txt
    path = os.path.join(ROOT, "build", "cand", name + ".c")
    open(path, "w", newline="").write(txt)
    return path


def main(argv):
    write = "--write" in argv
    all_mods = "--all-mods" in argv
    lo, hi = 40, 400
    if "--min" in argv:
        lo = int(argv[argv.index("--min") + 1])
    if "--max" in argv:
        hi = int(argv[argv.index("--max") + 1])
    mods = [
        m
        for m in ("arm9", "ov002", "ov004", "ov008", "ov010")
        if os.path.isdir(os.path.join(ROOT, "src", m))
    ]
    index = {}
    for mod in mods:
        fm.auto.set_module(mod)
        done = {f[:-2] for f in os.listdir(os.path.join(ROOT, "src", mod))}
        for addr, (name, size) in fm.auto.FUNCS.items():
            if name in done and lo <= size <= hi:
                index.setdefault(skel(ins(addr, size)), []).append(
                    (mod, name, size)
                )
    hits = 0
    for mod in mods:
        fm.auto.set_module(mod)
        done = {f[:-2] for f in os.listdir(os.path.join(ROOT, "src", mod))}
        for addr, (name, size) in sorted(fm.auto.FUNCS.items()):
            if name in done or not (lo <= size <= hi):
                continue
            cands = [
                c
                for c in index.get(skel(ins(addr, size)), [])
                if all_mods or c[0] == mod
            ]
            if not cands:
                continue
            tmod, tname, _ = cands[0]
            fm.auto.set_module(tmod)
            tpl = ins(addr_from(tname, tmod), size)
            fm.auto.set_module(mod)
            path = generate(mod, tname, tpl, name, ins(addr, size))
            out = subprocess.run(
                [
                    sys.executable,
                    os.path.join(ROOT, "build", "var3.py"),
                    mod,
                    hex(addr),
                    str(size),
                    path,
                ],
                capture_output=True,
                text=True,
                cwd=ROOT,
            ).stdout
            ok = out.splitlines()[0].strip() == "MATCH" if out else "?"
            print("%-6s %-32s %3d  <- %s/%s  %s" % (mod, name, size, tmod, tname, ok))
            if ok is True:
                hits += 1
                if write:
                    open(
                        os.path.join(ROOT, "src", mod, name + ".c"),
                        "w",
                        newline="",
                    ).write(open(path).read())
    print("matched", hits)


def addr_from(name, mod):
    for a, (n, s) in fm.auto.FUNCS.items():
        if n == name:
            return a
    raise KeyError(name)


if __name__ == "__main__":
    main(sys.argv[1:])
