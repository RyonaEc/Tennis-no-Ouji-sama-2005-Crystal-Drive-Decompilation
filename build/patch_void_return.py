"""A void function must not `return 0x0;`.

The generator keeps the early-exit value from the guard (or the branch return)
without checking the function's return type, so void functions emitted
`return 0x0;`, which mwcc rejects (reporting it at EOF as `<string not found>`).
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

pairs = [
    # guard
    ("""                if retval is None:
                    out.append(f'{pad}if ({cond}) return;' if rtype == 'void' else f'{pad}if ({cond}) return 0;')
                else:
                    out.append(f'{pad}if ({cond}) return {e(retval)};')""",
     """                if rtype == 'void':
                    out.append(f'{pad}if ({cond}) return;')
                elif retval is None:
                    out.append(f'{pad}if ({cond}) return 0;')
                else:
                    out.append(f'{pad}if ({cond}) return {e(retval)};')"""),
    # explicit return statement
    ("""            if st[0] == 'return':
                out.append(f'{pad}return {e(st[1])};' if st[1] is not None else f'{pad}return;')
                continue""",
     """            if st[0] == 'return':
                out.append(f'{pad}return;' if (st[1] is None or rtype == 'void')
                           else f'{pad}return {e(st[1])};')
                continue"""),
    # branch-local return
    ("""            if st[0] == 'ret':
                out.append(f'{pad}return {e(st[1])};' if st[1] is not None else f'{pad}return;')
                continue""",
     """            if st[0] == 'ret':
                out.append(f'{pad}return;' if (st[1] is None or rtype == 'void')
                           else f'{pad}return {e(st[1])};')
                continue"""),
]

for old, new in pairs:
    assert t.count(old) == 1, (t.count(old), old[:60])
    t = t.replace(old, new, 1)

P.write_text(t, encoding="utf-8")
print("void returns fixed")
