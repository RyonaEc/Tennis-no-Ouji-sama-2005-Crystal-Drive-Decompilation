"""Structural mutations that change instruction order, not spelling.

`respell` varies declarations, statement order and expression forms from one
base, and that grid is a fixed point on its own output (see
`notes/mwccarm-codegen-log.md`).  What it cannot reach is a branch whose *arms*
are the other way round: mwcc chooses the layout, so `if (c) { A } else { B }`
and `if (!c) { B } else { A }` are different instruction streams, and no amount
of re-spelling the same arms gets from one to the other.  This module rewrites a
candidate into those layouts.

Only a single top-level comparison is inverted; a condition with `&&`, `||` or
nested parentheses is left alone rather than guessed at, because a wrong rewrite
costs a compile and teaches nothing.
"""
import re

CMP = {'==': '!=', '!=': '==', '<': '>=', '>=': '<', '>': '<=', '<=': '>'}


def _matching(text, i):
    """Index of the bracket that closes the one at `i`."""
    opener, closer = text[i], {'(': ')', '{': '}'}[text[i]]
    depth = 0
    while i < len(text):
        if text[i] == opener:
            depth += 1
        elif text[i] == closer:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def invert(cond):
    """`!cond` for a condition this module can invert, else None."""
    c = cond.strip()
    if not c or '&&' in c or '||' in c or '?' in c:
        return None
    if c.startswith('!'):
        return c[1:].strip() or None
    if c.startswith('(') and _matching(c, 0) == len(c) - 1:
        return '!' + c
    for op in ('==', '!=', '>=', '<=', '>', '<'):
        idx = c.find(op)
        if idx <= 0:
            continue
        left, right = c[:idx].strip(), c[idx + len(op):].strip()
        if not left or not right or '(' in left or ')' in left:
            continue
        return '%s %s %s' % (left, CMP[op], right)
    return None


def sites(text):
    """Yield (start, stop, cond, then_body, else_body) for each if/else."""
    i = 0
    while True:
        i = text.find('if', i)
        if i < 0:
            return
        before = text[i - 1] if i else ' '
        after = text[i + 2] if i + 2 < len(text) else ' '
        if before.isalnum() or before == '_' or after.isalnum() or after == '_':
            i += 2
            continue
        j = i + 2
        while j < len(text) and text[j] in ' \t\n':
            j += 1
        if j >= len(text) or text[j] != '(':
            i += 2
            continue
        k = _matching(text, j)
        if k < 0:
            return
        cond = text[j + 1:k]
        p = k + 1
        while p < len(text) and text[p] in ' \t\n':
            p += 1
        if p < len(text) and text[p] == '{':
            end = _matching(text, p)
            if end < 0:
                return
            then_body, stop = text[p:end + 1], end + 1
        else:
            end = text.find(';', p)
            if end < 0:
                return
            then_body, stop = text[p:end + 1], end + 1
        q = stop
        while q < len(text) and text[q] in ' \t\n':
            q += 1
        if not text.startswith('else', q):
            i = stop
            continue
        r = q + 4
        while r < len(text) and text[r] in ' \t\n':
            r += 1
        if r < len(text) and text[r] == '{':
            end2 = _matching(text, r)
            if end2 < 0:
                return
            else_body, last = text[r:end2 + 1], end2 + 1
        else:
            end2 = text.find(';', r)
            if end2 < 0:
                return
            else_body, last = text[r:end2 + 1], end2 + 1
        yield i, last, cond, then_body, else_body
        i = last


def forms(text, limit=4):
    """(label, source) for each branch this text can be rewritten into."""
    out, seen = [], {text}
    for start, stop, cond, then_body, else_body in sites(text):
        inv = invert(cond)
        if inv is None:
            continue
        mutated = (text[:start] + 'if (%s) %s else %s' % (inv, else_body, then_body)
                   + text[stop:])
        if mutated in seen:
            continue
        seen.add(mutated)
        out.append(('inv%d' % len(out), mutated))
        if len(out) >= limit:
            break
    return out
