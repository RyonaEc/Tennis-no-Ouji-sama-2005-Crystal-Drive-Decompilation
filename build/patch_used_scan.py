"""The live-temp scan must look inside statement tuples.

`used` was computed with _all_nodes over whole statements, but _all_nodes only
understands expression nodes, so a load hoisted for a store base or call argument
was treated as unused, its declaration dropped, and mwcc failed on `v0`.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old = """        used = set()
        live = []
        for st in fn['stmts']:
            _all_nodes(st, live)
        _all_nodes(fn['ret'], live)"""
new = """        used = set()
        live = []
        for st in fn['stmts']:
            if st[0] == 'store':
                _all_nodes(st[1], live)
                _all_nodes(st[5], live)
            elif st[0] == 'sset':
                _all_nodes(st[2], live)
            elif st[0] == 'guard':
                _all_nodes(st[1][0], live)
                _all_nodes(st[1][2], live)
                if st[2] is not None:
                    _all_nodes(st[2], live)
            elif st[0] == 'call':
                for a in fn['calls'][st[1]][1]:
                    _all_nodes(a, live)
        _all_nodes(fn['ret'], live)"""

assert t.count(old) == 1, t.count(old)
P.write_text(t.replace(old, new, 1), encoding="utf-8")
print("used-scan fixed")
