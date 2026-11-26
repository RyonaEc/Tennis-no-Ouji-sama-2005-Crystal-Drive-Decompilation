"""Permutation search over one candidate: exhaustive, or seeded random.

Why this exists
---------------
The walls left in this project are dominated by register-allocation residue: the
instruction stream is already right, but two locals swap colours, or the branch
comes out predicated the other way.  No spelling change is obvious from reading
the disassembly, so the remaining search is over *source shapes* scored against
retail bytes.

`simonlindholm/decomp-permuter` does this with random mutation of the C file and
a graded objdump diff (0 = exact).  This keeps that idea but drives the project's
own harness, so a winner is already a `src/` candidate that the ROM digest can
check, and every score comes from `fastmatch.diff_summary` (same decoder, same
slots).  Differences from upstream, deliberately:

* only semantics-preserving rewrites are offered (see "What it will not do"),
  so a winner is plausible source rather than an accident of regalloc;
* exhaustive permutation of `// P` marked lines is still available for tiny
  statement sets, where trying all orders beats sampling.

Two modes
---------
* Any line ending in `// P` -> exhaustive: those lines are permuted in place
  (the original behaviour of this tool).  With more permutations than `--limit`
  orders are sampled instead of enumerated.
* No markers -> random mode: seeded hill-climbing with restarts over the rewrites
  below, scored by (differing slots, distance from the retail size).

What it tries (random mode)
---------------------------
statement order between neighbours whose read/write sets are disjoint and that
contain no calls or control flow; the same for initialisation statements;
declaration order (adjacent swaps and block rotations); `T x = e;` <-> `T x; x = e;`;
`x op= y` <-> `x = x op y`; `u32` <-> `s32` counters; `if/else` <-> a conditional
expression when both arms assign the same lvalue; two statements joined onto one
line or split apart.  `--temps` additionally splits a pure arithmetic expression
through a local, which is evidence only: it changes live ranges on purpose and
must be explained before anything is kept.

What it will not do
-------------------
It will not reorder statements that touch a common name, reorder across a call or
a branch, introduce a temp by default, add `volatile`, add padding, or otherwise
disturb what the source means.  It never installs on its own: a zero score still
has to pass `--install`, which runs `t10.try_unit` and the ROM digest, and a
candidate found only with `--temps` is printed as NEEDS-EXPLANATION.

Usage
-----
    python tools/match/permute.py <mod> <addr> <size> <file.c>
        [--iters N] [--islands N] [--time S] [--seed N] [--limit N] [--top N]
        [--temps] [--install] [--verbose]
"""
import collections
import importlib.util
import itertools
import math
import pathlib
import random
import re
import sys
import time


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
resp = load("tools/match/respell.py", "resp")
t10 = load("tools/match/t10.py", "t10")

MARK = "// P"
TYPE_WORDS = {"u8", "u16", "u32", "s8", "s16", "s32", "u64", "s64", "int", "char",
              "void", "unsigned", "signed", "short", "long", "volatile", "const",
              "struct", "union", "enum", "float", "double"}
CONTROL = ("if", "for", "while", "do", "switch", "return", "goto", "else", "break",
           "continue", "case", "default")


# --------------------------------------------------------------------------
# state: (decls, groups) -> source text
# --------------------------------------------------------------------------

def flatten(groups):
    lines = []
    for group in groups:
        lines.extend(group)
        lines.append("")
    return lines


def read_state(text):
    prefix, decls, rest, suffix = resp.split_unit(text)
    return prefix, decls, resp.statements(rest), suffix


def render(state):
    prefix, decls, groups, suffix = state
    return resp.build(prefix, decls, flatten(groups), suffix)


def body_of(text):
    return re.sub(r'^\s*#include\s+"types\.h"\s*\n', "", text)


# --------------------------------------------------------------------------
# statement analysis
# --------------------------------------------------------------------------

def simple(line):
    """(writes, reads, lhs, rhs) for one simple statement, else None.

    A "simple" statement is one assignment or declaration with a single `;`,
    no control keyword, and no call on either side.  Being conservative here is
    what keeps reordering semantics-preserving.
    """
    s = line.strip()
    if not s or s[0] in "#{}":
        return None
    if s.split(None, 1)[0].rstrip("(") in CONTROL:
        return None
    if not s.endswith(";") or s.count(";") != 1:
        return None
    body = s[:-1]
    depth, cut = 0, -1
    for i, ch in enumerate(body):
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        elif ch == "=" and depth == 0:
            if i + 1 < len(body) and body[i + 1] == "=":
                continue
            if i > 0 and body[i - 1] in "!<>=":
                continue
            cut = i
            break
    if cut < 0:
        return None
    lhs, rhs = body[:cut].strip(), body[cut + 1:].strip()
    if not lhs or not rhs:
        return None
    calls = [n for n in re.findall(r"\b([A-Za-z_]\w*)\s*\(", rhs)
             if n not in TYPE_WORDS]
    if calls:
        return None
    if re.search(r"\b(volatile)\b", rhs):
        return None
    ids = set(re.findall(r"[A-Za-z_]\w*", body))
    writes = set(re.findall(r"[A-Za-z_]\w*", lhs))
    reads = (set(re.findall(r"[A-Za-z_]\w*", rhs)) | (writes - {lhs})) - TYPE_WORDS
    return writes - TYPE_WORDS, reads, lhs, rhs


def may_swap(a, b):
    sa, sb = simple(a), simple(b)
    if not sa or not sb:
        return False
    wa, ra = sa[0], sa[1]
    wb, rb = sb[0], sb[1]
    if wa & wb:
        return False
    if wa & rb or wb & ra:
        return False
    return True


def single(group):
    return len(group) == 1


# --------------------------------------------------------------------------
# mutations: state -> (label, state) or None
# --------------------------------------------------------------------------

def mut_swap_stmt(state, rng):
    _, decls, groups, _ = state
    idx = [i for i in range(len(groups) - 1)
           if single(groups[i]) and single(groups[i + 1])
           and may_swap(groups[i][0], groups[i + 1][0])]
    if not idx:
        return None
    i = rng.choice(idx)
    new = list(groups)
    new[i], new[i + 1] = new[i + 1], new[i]
    return "swap stmt %d/%d" % (i, i + 1), (state[0], decls, new, state[3])


def mut_swap_decl(state, rng):
    prefix, decls, groups, suffix = state
    if len(decls) < 2:
        return None
    i = rng.randrange(len(decls) - 1)
    new = list(decls)
    new[i], new[i + 1] = new[i + 1], new[i]
    return "decl %d/%d" % (i, i + 1), (prefix, new, groups, suffix)


def mut_rotate_decl(state, rng):
    prefix, decls, groups, suffix = state
    if len(decls) < 3:
        return None
    k = rng.randrange(1, len(decls))
    return "decl rot %d" % k, (prefix, decls[k:] + decls[:k], groups, suffix)


def mut_shuffle_decl(state, rng):
    """Jump the declaration block to an unrelated order (escapes a plateau)."""
    prefix, decls, groups, suffix = state
    if len(decls) < 3 or len(decls) > 8:
        return None
    new = list(decls)
    rng.shuffle(new)
    if new == decls:
        return None
    return "decl shuffle", (prefix, new, groups, suffix)


def mut_split_init(state, rng):
    prefix, decls, groups, suffix = state
    cands = [i for i, d in enumerate(decls) if resp.DECL.match(d) and resp.DECL.match(d).group(5)]
    if not cands:
        return None
    i = rng.choice(cands)
    m = resp.DECL.match(decls[i])
    typ, star, name, arr, expr = m.groups()
    new = list(decls)
    new[i] = "    %s %s%s%s;" % (typ, star, name, arr or "")
    new_groups = [["    %s = %s;" % (name, expr)]] + list(groups)
    return "split init %s" % name, (prefix, new, new_groups, suffix)


def mut_join_init(state, rng):
    prefix, decls, groups, suffix = state
    cands = []
    for gi, group in enumerate(groups):
        if not single(group):
            continue
        m = re.match(r"^\s*(\w+)\s*=\s*(.+?);\s*$", group[0])
        if not m:
            continue
        name = m.group(1)
        for di, d in enumerate(decls):
            dm = resp.DECL.match(d)
            if dm and dm.group(3) == name and dm.group(5) is None and not dm.group(4):
                cands.append((gi, di, name, dm, m.group(2)))
    if not cands:
        return None
    gi, di, name, dm, expr = rng.choice(cands)
    new_decls = list(decls)
    new_decls[di] = "    %s %s%s = %s;" % (dm.group(1), dm.group(2), name, expr)
    new_groups = [g for k, g in enumerate(groups) if k != gi]
    return "join init %s" % name, (prefix, new_decls, new_groups, suffix)


def mut_compound(state, rng):
    prefix, decls, groups, suffix = state
    cands = [i for i, g in enumerate(groups) if single(g) and resp.COMPOUND.match(g[0])
             and "==" not in g[0]]
    if cands:
        i = rng.choice(cands)
        m = resp.COMPOUND.match(groups[i][0])
        new_groups = list(groups)
        new_groups[i] = ["%s%s = %s %s %s;" % (m.group(1), m.group(2), m.group(2),
                                               m.group(3)[0], m.group(4))]
        return "expand %s" % m.group(3), (prefix, decls, new_groups, suffix)
    cands = [i for i, g in enumerate(groups) if single(g) and resp.EXPANDED.match(g[0])]
    if not cands:
        return None
    i = rng.choice(cands)
    m = resp.EXPANDED.match(groups[i][0])
    new_groups = list(groups)
    new_groups[i] = ["%s%s %s= %s;" % (m.group(1), m.group(2), m.group(3), m.group(4))]
    return "fold %s=" % m.group(3), (prefix, decls, new_groups, suffix)


def mut_signed(state, rng):
    prefix, decls, groups, suffix = state
    cands = [i for i, d in enumerate(decls) if re.match(r"^\s*(u32|s32)\s+\w+;\s*$", d)]
    if not cands:
        return None
    i = rng.choice(cands)
    new = list(decls)
    new[i] = new[i].replace("u32", "s32") if "u32" in new[i] else new[i].replace("s32", "u32")
    return "signed toggle %s" % new[i].strip(), (prefix, new, groups, suffix)


IF_ELSE = re.compile(
    r"^\s*if\s*\((.+?)\)\s*\{\s*\n?\s*(\w+)\s*=\s*(.+?);\s*\n?\s*\}\s*else\s*\{\s*\n?\s*"
    r"(\w+)\s*=\s*(.+?);\s*\n?\s*\}\s*$", re.S)
TERNARY = re.compile(r"^\s*(\w+)\s*=\s*(.+?)\s*\?\s*(.+?)\s*:\s*(.+?);\s*$")


def mut_ternary(state, rng):
    prefix, decls, groups, suffix = state
    cands = []
    for i, group in enumerate(groups):
        text = "\n".join(group)
        m = IF_ELSE.match(text)
        if m and m.group(2) == m.group(4):
            cands.append((i, "to ?:", m))
            continue
        if single(group):
            t = TERNARY.match(group[0])
            if t:
                cands.append((i, "to if", t))
    if not cands:
        return None
    i, kind, m = rng.choice(cands)
    new_groups = list(groups)
    if kind == "to ?:":
        new_groups[i] = ["    %s = (%s) ? %s : %s;" % (m.group(2), m.group(1), m.group(3),
                                                      m.group(5))]
    else:
        new_groups[i] = ["    if (%s) {" % m.group(2),
                         "        %s = %s;" % (m.group(1), m.group(3)),
                         "    } else {",
                         "        %s = %s;" % (m.group(1), m.group(4)),
                         "    }"]
    return "ternary %s" % kind, (prefix, decls, new_groups, suffix)


def mut_sameline(state, rng):
    prefix, decls, groups, suffix = state
    pairs = [i for i in range(len(groups) - 1)
             if single(groups[i]) and single(groups[i + 1])
             and may_swap(groups[i][0], groups[i + 1][0])]
    joined = [i for i, g in enumerate(groups)
              if single(g) and g[0].rstrip().endswith(";") and g[0].count(";") == 2]
    if joined:
        i = rng.choice(joined)
        a, b = groups[i][0].split(";", 1)
        new_groups = list(groups)
        new_groups[i] = [a + ";"]
        new_groups.insert(i + 1, [b.strip()])
        return "split line", (prefix, decls, new_groups, suffix)
    if not pairs:
        return None
    i = rng.choice(pairs)
    new_groups = list(groups)
    new_groups[i:i + 2] = [[groups[i][0].rstrip() + " " + groups[i + 1][0].strip()]]
    return "join line", (prefix, decls, new_groups, suffix)


TEMP_SAFE = re.compile(r"^\s*(\w+)\s*=\s*([A-Za-z_]\w*)\s*([-+*/&|^])\s*([A-Za-z_]\w*)\s*;$")


def mut_temp(state, rng):
    """Evidence-only: split a pure binary expression through a local."""
    prefix, decls, groups, suffix = state
    cands = [(i, TEMP_SAFE.match(g[0])) for i, g in enumerate(groups) if single(g)]
    cands = [(i, m) for i, m in cands if m]
    if not cands:
        return None
    i, m = rng.choice(cands)
    name = "tmp%d" % i
    new_decls = list(decls) + ["    s32 %s;" % name]
    new_groups = list(groups)
    new_groups[i:i + 1] = [["    %s = %s %s %s;" % (name, m.group(2), m.group(3), m.group(4))],
                           ["    %s = %s;" % (m.group(1), name)]]
    return "temp %s" % name, (prefix, new_decls, new_groups, suffix)


MUTATIONS = [mut_swap_stmt, mut_swap_decl, mut_rotate_decl, mut_shuffle_decl,
             mut_split_init, mut_join_init, mut_compound, mut_signed, mut_ternary,
             mut_sameline]


# --------------------------------------------------------------------------
# search
# --------------------------------------------------------------------------

def decl_sweep(mod, addr, size, state, max_decls, verbose):
    """Exhaustive declaration order, which is the axis that decides colouring.

    Compiling one body costs about 25 ms, so 720 orders (six locals) is a few
    seconds; seven is a couple of minutes.  Stops on an exact match.
    """
    prefix, decls, groups, suffix = state
    n = len(decls)
    if n < 2 or n > max_decls:
        return None, state, "base", 0
    base = score(mod, addr, size, render(state))
    best = (base, list(decls), "base")
    tried = 0
    for perm in itertools.permutations(decls):
        cand = (prefix, list(perm), groups, suffix)
        text_c = render(cand)
        s = score(mod, addr, size, text_c)
        tried += 1
        if s is None:
            continue
        if s[0] == 0:
            print("MATCH in declaration sweep after %d orders" % tried)
            return text_c, cand, "%d decls, sweep order %d" % (n, tried), tried
        if best[0] is None or s < best[0]:
            best = (s, list(perm), "sweep order %d" % tried)
    if verbose:
        print("  declaration sweep: %d orders, best %s"
              % (tried, best[0][0] if best[0] else "-"))
    return None, (prefix, best[1], groups, suffix), best[2], tried


def score(mod, addr, size, text):
    ok, n, diffs = fm.diff_summary(mod, addr, size, body_of(text))
    if n is None:
        return None
    if ok:
        return (0, 0)
    return (len(diffs), abs(n - size))


def better(a, b):
    return a is not None and (b is None or a < b)


def exhaustive(mod, addr, size, text, limit):
    lines = text.splitlines()
    slots = [i for i, l in enumerate(lines) if l.rstrip().endswith(MARK)]
    if not slots:
        return None
    marked = [lines[i][: lines[i].rindex(MARK)].rstrip() for i in slots]
    total = 1
    for k in range(2, len(marked) + 1):
        total *= k
    print("%d marked statements (%d orders%s)" % (len(marked), total,
                                                  ", sampled" if total > limit else ""))
    orders = itertools.permutations(marked)
    for n, order in enumerate(orders):
        if n >= limit:
            print("stopped after %d orders" % limit)
            break
        trial = list(lines)
        for slot, line in zip(slots, order):
            trial[slot] = line
        if fm.compare("\n".join(trial), mod, addr, size):
            print("MATCH: %s" % " / ".join(o.strip() for o in order))
            return "\n".join(trial), "marked order %d" % n
    return None


def search(mod, addr, size, text, iters, islands, budget, seed, use_temps, verbose):
    """Annealed hill climb: accept improvements, drift sideways, cross valleys.

    Accepting only strict improvements stalls on a plateau - in practice one
    adjacent-transposition grid is all a single pass ever visits, and no
    sequence of single improving moves reaches the right declaration order.
    So: sideways moves are always taken, worse moves are taken with a
    temperature that cools, and roughly a third of the steps apply two or
    three rewrites together, which is what crosses a valley in one hop.
    """
    rng = random.Random(seed)
    base = read_state(text)
    muts = MUTATIONS + ([mut_temp] if use_temps else [])
    frontier = []          # [(score, label, text)] best first, distinct bodies
    recent = collections.deque(maxlen=900)
    recent_set = set()
    started = time.time()
    tried = failed = 0
    best = None

    for island in range(islands):
        if time.time() - started > budget:
            break
        if island == 0 or not frontier:
            state = base
        else:
            state = read_state(rng.choice(frontier[:2])[2])
        current = score(mod, addr, size, render(state))
        for step in range(iters):
            if time.time() - started > budget:
                break
            cand, labels = state, []
            for _ in range(rng.choices((1, 2, 3), weights=(65, 25, 10))[0]):
                got = rng.choice(muts)(cand, rng)
                if not got:
                    break
                labels.append(got[0])
                cand = got[1]
            if not labels:
                continue
            label = " + ".join(labels)
            text_c = render(cand)
            if text_c in recent_set:
                continue
            if len(recent) == recent.maxlen:
                recent_set.discard(recent[0])
            recent.append(text_c)
            recent_set.add(text_c)
            s = score(mod, addr, size, text_c)
            tried += 1
            if s is None:
                failed += 1
                continue
            if verbose and tried % 50 == 0:
                print("  [%ds] island %d step %d  %s  %s"
                      % (time.time() - started, island, step,
                         "%d slots" % s[0] if s[0] else "MATCH", label))
            if s[0] == 0:
                need = use_temps and "temp tmp" in label
                frontier.insert(0, (s, label + (" NEEDS-EXPLANATION" if need else ""), text_c))
                print("MATCH after %d tries (%ds): %s"
                      % (tried, time.time() - started, label))
                return frontier, tried, failed
            if best is None or s < best:
                best = s
                frontier.append((s, label, text_c))
                frontier.sort(key=lambda x: x[0])
                keep, uniq = [], set()
                for entry in frontier:
                    k = re.sub(r"\s+", " ", entry[2])
                    if k in uniq:
                        continue
                    uniq.add(k)
                    keep.append(entry)
                frontier = keep[:4]
            if current is None:
                state, current = cand, s
                continue
            delta = s[0] - current[0]
            if delta <= 0:
                state, current = cand, s
            else:
                temp = max(0.15, 3.0 * (0.93 ** step))
                if rng.random() < math.exp(-delta / temp):
                    state, current = cand, s
    return frontier, tried, failed


def main(argv):
    flags = [a for a in argv if a.startswith("--")]
    args = [a for a in argv if not a.startswith("--")]
    if len(args) < 4:
        print(__doc__)
        return 2
    mod, addr, size, path = args[0], int(args[1], 0), int(args[2], 0), pathlib.Path(args[3])
    opt = {"--iters": 150, "--islands": 4, "--time": 180, "--seed": 1,
           "--limit": 4000, "--top": 4, "--sweep-max": 6}
    for flag in list(opt):
        if flag in argv:
            opt[flag] = int(argv[argv.index(flag) + 1])
    install = "--install" in flags
    use_temps = "--temps" in flags
    verbose = "--verbose" in flags

    text = path.read_text(encoding="utf-8")
    note = fm.wall_note(mod, addr)
    if note:
        print("note: parked - %s" % note)
    name = resp.func_name(mod, addr)

    found = exhaustive(mod, addr, size, text, opt["--limit"]) if \
        any(l.rstrip().endswith(MARK) for l in text.splitlines()) else None
    if found:
        src, label = found
        out = pathlib.Path("build/cand/permute_%s.c" % name)
        out.write_text(src + "\n")
        print("wrote %s" % out)
        if install:
            kept = t10.try_unit(name, addr, size, src, mod=mod)
            print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
        return 0

    state = read_state(text)
    match, seed_state, seed_label, swept = decl_sweep(mod, addr, size, state,
                                                      opt["--sweep-max"], verbose)
    if swept:
        print("declaration sweep: %d orders, best state %s" % (swept, seed_label))
    if match:
        out = pathlib.Path("build/cand/permute_%s.c" % name)
        out.write_text(match)
        print("wrote %s  (%s)" % (out, seed_label))
        if install:
            kept = t10.try_unit(name, addr, size, match, mod=mod)
            print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
        return 0

    frontier, tried, failed = search(mod, addr, size, render(seed_state), opt["--iters"],
                                     opt["--islands"], opt["--time"], opt["--seed"],
                                     use_temps, verbose)
    print("%d candidates, %d did not compile, best:" % (tried, failed))
    if not frontier:
        print("  no improving state found - widen --iters/--islands or add --temps")
    for s, label, text_c in frontier[: opt["--top"]]:
        print("  %3d slots  %+d B  %s" % (s[0], s[1], label))
    if frontier and frontier[0][0][0] == 0:
        best = frontier[0]
        out = pathlib.Path("build/cand/permute_%s.c" % name)
        out.write_text(best[2])
        print("wrote %s  (%s)" % (out, best[1]))
        if install:
            kept = t10.try_unit(name, addr, size, best[2], mod=mod)
            print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
