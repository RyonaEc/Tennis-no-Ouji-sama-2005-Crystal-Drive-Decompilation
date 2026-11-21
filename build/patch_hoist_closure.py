"""A kept temp may use another temp in its own initialiser.

`v2 = <expr using v0>` means v0 must also stay declared, but the used-set was
computed from the rewritten statements only, so v0's declaration was dropped.
Close over the kept temps' initialisers until nothing new appears.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old = """        for i, expr in enumerate(inner_exprs):
            if i in used:"""
new = """        changed = True
        while changed:
            changed = False
            for i in list(used):
                for n2 in _all_nodes(inner_exprs[i], []):
                    if n2[0] == 'le' and n2[1] not in used:
                        used.add(n2[1])
                        changed = True
        for i, expr in enumerate(inner_exprs):
            if i in used:"""

assert t.count(old) == 1, t.count(old)
P.write_text(t.replace(old, new, 1), encoding="utf-8")
print("hoist closure added")
