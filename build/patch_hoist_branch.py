"""Hoist a repeated address across branches, the way retail keeps it live.

Hoisting was disabled whenever the body contained an `if` or loop, and _subst
did not descend into those nodes, so

    if (x < a) { *p = a; }
    if (x > b) { *p = b; }
    return *p;

re-emitted the whole pointer chain three times.  Now a node is hoisted when it
is used by at least two *top-level* statements (so a value only used inside one
branch is not made unconditional), and substitution descends into branches.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# 1) _subst: descend into control-flow nodes
old_subst = """    if t == 'store':
        return ('store', _subst(x[1], subs)) + tuple(x[2:5]) + (_subst(x[5], subs),)
    return x"""
new_subst = """    if t == 'store':
        return ('store', _subst(x[1], subs)) + tuple(x[2:5]) + (_subst(x[5], subs),)
    if t == 'sset':
        return ('sset', x[1], _subst(x[2], subs))
    if t == 'guard':
        return ('guard', x[1], _subst(x[2], subs) if x[2] is not None else None)
    if t == 'if':
        return ('if', x[1], [_subst(s, subs) for s in x[2]],
                [_subst(s, subs) for s in x[3]],
                x[4], _subst(x[5], subs) if x[5] is not None else None)
    if t == 'loop':
        return ('loop', x[1], [_subst(s, subs) for s in x[2]])
    return x"""
assert t.count(old_subst) == 1, t.count(old_subst)
t = t.replace(old_subst, new_subst, 1)

# 2) collect per top-level statement, remember how many statements use each node
old_flat = """    hoist_names, hoist_assigns = [], []
    flat = []
    walk_stmts = _walk_stmts(fn['stmts'])
    for st in walk_stmts:
        if st[0] == 'store':
            _all_nodes(st[1], flat)
            _all_nodes(st[5], flat)
        elif st[0] == 'sset':
            _all_nodes(st[2], flat)
        elif st[0] == 'guard':
            _all_nodes(st[1][0], flat)
            _all_nodes(st[1][2], flat)
            if st[2] is not None:
                _all_nodes(st[2], flat)
        elif st[0] == 'call':
            for a in fn['calls'][st[1]][1]:
                _all_nodes(a, flat)
    _all_nodes(fn['ret'], flat)"""
new_flat = """    hoist_names, hoist_assigns = [], []
    flat = []
    tops = {}

    def _stmt_nodes(st, acc):
        if st[0] == 'store':
            _all_nodes(st[1], acc)
            _all_nodes(st[5], acc)
        elif st[0] == 'sset':
            _all_nodes(st[2], acc)
        elif st[0] == 'guard':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            if st[2] is not None:
                _all_nodes(st[2], acc)
        elif st[0] == 'if':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            for s2 in list(st[2]) + list(st[3]):
                _stmt_nodes(s2, acc)
            if st[4] is not None:
                _all_nodes(st[4], acc)
            if st[5] is not None:
                _all_nodes(st[5], acc)
        elif st[0] == 'loop':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            for s2 in st[2]:
                _stmt_nodes(s2, acc)

    for ti, st in enumerate(fn['stmts']):
        nodes = []
        _stmt_nodes(st, nodes)
        for n in nodes:
            flat.append(n)
            tops.setdefault(repr(n), set()).add(ti)
    ret_nodes = []
    _all_nodes(fn['ret'], ret_nodes)
    for n in ret_nodes:
        flat.append(n)
        tops.setdefault(repr(n), set()).add(len(fn['stmts']))
    arg_nodes = []
    for cn, ca, _nar in fn['calls']:
        for a in ca:
            _all_nodes(a, arg_nodes)"""
assert t.count(old_flat) == 1, t.count(old_flat)
t = t.replace(old_flat, new_flat, 1)

# 3) argument count must still see call arguments
old_argn = """    argn = max(fn['nargs'], max([n[1] for n in flat if n[0] == 'a'] or [-1]) + 1)"""
new_argn = """    all_nodes = flat + arg_nodes
    argn = max(fn['nargs'], max([n[1] for n in all_nodes if n[0] == 'a'] or [-1]) + 1)"""
assert t.count(old_argn) == 1, t.count(old_argn)
t = t.replace(old_argn, new_argn, 1)

# 4) drop the branch-free restriction; require two distinct top-level users
old_gate = """    if not any(st[0] in ('if', 'loop') for st in walk_stmts):
        for n in flat:
            if (n[0] in ('load', 'arith', 'bit', 'mul', 'mla')
                    and seen_count[repr(n)] >= 2
                    and repr(n) not in subs
                    and _has_load(n)):
                subs[repr(n)] = len(order)
                order.append(n)"""
new_gate = """    for n in flat:
        if (n[0] in ('load', 'arith', 'bit', 'mul', 'mla')
                and len(tops.get(repr(n), ())) >= 2
                and repr(n) not in subs
                and _has_load(n)):
            subs[repr(n)] = len(order)
            order.append(n)"""
assert t.count(old_gate) == 1, t.count(old_gate)
t = t.replace(old_gate, new_gate, 1)

P.write_text(t, encoding="utf-8")
print("branch-aware hoisting enabled")
