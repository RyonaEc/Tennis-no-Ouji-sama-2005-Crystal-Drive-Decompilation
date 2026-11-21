"""A body that reads a0 needs a0 declared.

Some leaf functions keep the incoming argument in r0 without a prologue that the
generator counts, so nargs stayed 0 and the body referenced an undeclared a0.
Take the highest referenced argument index into account when writing the
signature.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old = """    _all_nodes(fn['ret'], flat)
    seen_count = {}"""
new = """    _all_nodes(fn['ret'], flat)
    argn = max(fn['nargs'], max([n[1] for n in flat if n[0] == 'a'] or [-1]) + 1)
    args = ', '.join(f'u32 a{i}' for i in range(argn)) or 'void'
    seen_count = {}"""

assert t.count(old) == 1, t.count(old)
P.write_text(t.replace(old, new, 1), encoding="utf-8")
print("arg count widened")
