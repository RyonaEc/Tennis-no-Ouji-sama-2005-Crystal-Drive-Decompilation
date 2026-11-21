"""Reuse a computed address instead of re-emitting it.

Retail computes a pointer once, keeps it in a register and stores through it
several times.  The emitter inlined the whole expression at every use, so a
function like

    ldr r0, [pc]          ; base
    ldr r0, [r0, #0x10]   ; object
    ldr r1, [r0, #0x154]  ; table
    ldrh r2, [r1, #4]
    strh r2, [r1, #6]

came out with the full `data -> +0x10 -> +0x154` chain repeated per store.

Hoist repeated expressions that contain a load into a temp, largest first (a
parent is hoisted before its children, so the children become dead and drop
out).  Byte equality is still the only acceptance test.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# helper: does this expression read memory?
anchor = "def _subst(x, subs):"
helper = """def _has_load(x):
    return any(n[0] == 'load' for n in _all_nodes(x, []))


"""
assert t.count(anchor) == 1
t = t.replace(anchor, helper + anchor, 1)

old = """            if n[0] == 'load' and seen_count[repr(n)] >= 2 and repr(n) not in subs:"""
new = """            if (n[0] in ('load', 'arith', 'bit', 'mul', 'mla')
                    and seen_count[repr(n)] >= 2
                    and repr(n) not in subs
                    and _has_load(n)):"""
assert t.count(old) == 1, t.count(old)
t = t.replace(old, new, 1)

P.write_text(t, encoding="utf-8")
print("address hoisting enabled")
