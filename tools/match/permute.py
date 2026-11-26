"""Permutation search over one candidate: exhaustive sweeps plus random search.

Why this exists
---------------
The walls left in this project are dominated by register-allocation residue: the
instruction stream is already right, but two locals swap colours, or the branch
comes out predicated the other way.  No spelling change is obvious from reading
the disassembly, so the remaining search is over *source shapes* scored against
retail bytes.

What it borrows, and what it refuses
------------------------------------
`simonlindholm/decomp-permuter` randomises the C file and scores an objdump diff;
its own FAQ says the score "should probably be tweaked to look at e.g. the
register diff graph", and the metrics it settled on (regalloc 5, reordering 60,
insertion/deletion 100) are the same ones asm-differ and objdiff use.  Two
projects had to replace that scorer because relocation-name differences became
penalties the search could never reduce (BFM-decomp, rac3-decomp's aligned
scorer), and one survey found roughly half of upstream's "improvements" change
what the code does.  So this tool keeps the weighted gradient but:

* masks everything the linker fills in - call targets, pooled literals,
  in-function branch displacements - so a stub address in a bare compile is
  never scored as a hit;
* only offers rewrites that preserve meaning (see "What it will not do"), so a
  winner is plausible source rather than an accident of regalloc;
* scores the *structure* first and the weighted cost second, so a regperm one
  instruction from done always beats a structurally wrong candidate that happens
  to have a cheaper register bill.

Scoring
-------
Instructions are aligned with `difflib` on a key that keeps the mnemonic and
operand shapes but blanks registers, immediates and stack offsets, so an
insertion cannot cascade into a wall of unrelated diffs.  Aligned pairs cost
5 per differing register, 1 per differing immediate, 1 per differing stack
offset; replaced pairs cost 60, insertions and deletions 100.  A candidate is
compared as `(structural_pairs, weighted_cost, abs(size - retail))`, and 0 on the
first element means byte-exact.

Modes
-----
* `--decl-sweep` (default up to `--sweep-max` locals): exhaustive declaration
  order, the axis that decides colouring.
* lines ending in `// P`: exhaustive permutation of those statements.
* otherwise: seeded annealed random search over the rewrites below, with
  restarts, a mutation bandit and optional worker processes.

What it tries
-------------
statement order between neighbours whose read/write sets are disjoint and that
contain no calls or control flow; and the same for initialisation statements;
declaration order (adjacent swaps, rotations, whole-block shuffles); `T x = e;`
<-> `T x; x = e;`; `x op= y` <-> `x = x op y`; `u32` <-> `s32` counters;
`if/else` <-> a conditional expression when both arms assign the same lvalue;
two statements joined onto one line or split apart.  `--temps` additionally
splits a pure arithmetic expression through a local: evidence only, printed as
NEEDS-EXPLANATION, because it changes live ranges on purpose.

What it will not do
-------------------
No reordering across a shared name, a call or a branch, no temps by default, no
`volatile`, no padding, no narrowed types.  It never installs on its own: a zero
score still has to pass `--install`, which runs `t10.try_unit` and the ROM
digest.

Usage
-----
    python tools/match/permute.py <mod> <addr> <size> <file.c>
        [--iters N] [--islands N] [--time S] [--seed N] [--jobs N]
        [--sweep-max N] [--batch N] [--top N] [--temps] [--install] [--verbose]
"""
import collections
import difflib
import hashlib
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
REGS = r"\b(?:r\d+|sb|sl|fp|ip|lr|sp|pc)\b"

# Penalties as used by decomp-permuter's scorer.py, asm-differ and objdiff:
# a register choice is nearly free, a reorder is expensive, a missing or extra
# instruction is worst.
W_REG, W_IMM, W_STACK = 5, 1, 1
W_REPLACE, W_INSERT, W_DELETE = 60, 100, 100


# --------------------------------------------------------------------------
# state: (prefix, decls, groups, suffix) -> source text
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
# scoring
# --------------------------------------------------------------------------

_CACHE = {}


def _key(ins):
    """Alignment key: mnemonic and operand shapes, without registers/numbers."""
    ops = re.sub(REGS, "R", ins.op_str)
    ops = re.sub(r"#-?0x[0-9a-fA-F]+|#-?\d+", "#I", ops)
    ops = re.sub(r"\s+", " ", ops).strip()
    return ins.mnemonic + " " + ops


def _regs(ins):
    return re.findall(REGS, ins.op_str)


def _imms(ins):
    return re.findall(r"#(-?0x[0-9a-fA-F]+|-?\d+)", ins.op_str)


def _stacks(ins):
    return re.findall(r"\[sp, #(-?0x[0-9a-fA-F]+|-?\d+)\]", ins.op_str)


def score_blob(want_ins, got_ins, size, n):
    """(structural, weighted, size delta) for one already-disassembled pair."""
    wk = [_key(i) for i in want_ins]
    gk = [_key(i) for i in got_ins]
    structural = 0
    cost = 0
    matcher = difflib.SequenceMatcher(a=gk, b=wk, autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            for g, w in zip(got_ins[i1:i2], want_ins[j1:j2]):
                if g.op_str == w.op_str:
                    continue
                gr, wr = _regs(g), _regs(w)
                cost += W_REG * sum(1 for a, b in zip(gr, wr) if a != b)
                cost += W_REG * abs(len(gr) - len(wr))
                gi, wi = _imms(g), _imms(w)
                cost += W_IMM * sum(1 for a, b in zip(gi, wi) if a != b)
                gs, ws = _stacks(g), _stacks(w)
                cost += W_STACK * sum(1 for a, b in zip(gs, ws) if a != b)
                if not (gr or wr or gi or wi or gs or ws):
                    structural += 1
                    cost += W_REPLACE
        else:
            pairs = min(i2 - i1, j2 - j1)
            structural += max(i2 - i1, j2 - j1)
            cost += W_REPLACE * pairs
            cost += W_DELETE * max(0, (j2 - j1) - pairs)
            cost += W_INSERT * max(0, (i2 - i1) - pairs)
    if not got_ins:
        structural = len(want_ins)
        cost = W_DELETE * len(want_ins)
    return structural, cost, abs(n - size)


def score_body(mod, addr, size, body):
    """(structural, weighted, size delta) for one candidate, or None if it fails.

    Compiles once, masks the linker-filled words before the byte compare, and
    caches by body hash so repeated states across islands are free.
    """
    digest = hashlib.sha1(body.encode("utf-8")).hexdigest()
    hit = _CACHE.get(digest)
    if hit is not None:
        return hit
    blob = fm.compile_text(body, "probe")
    if not blob:
        return None
    want = fm.retail(mod, addr, size)
    ok = blob == want
    if not ok:
        relocs = fm.compile_text.last_relocs
        ok = bool(relocs) and fm.masked(blob, relocs) == fm.masked(want, relocs)
    if ok:
        result = (0, 0, 0)
    else:
        want_ins = [i for i in fm.parse_ins(addr, size)]
        got_ins = list(fm.md.disasm(blob, addr))
        result = score_blob(want_ins, got_ins, size, len(blob))
    _CACHE[digest] = result
    return result


def score_text(mod, addr, size, text):
    return score_body(mod, addr, size, body_of(text))


# --------------------------------------------------------------------------
# statement analysis
# --------------------------------------------------------------------------

def simple(line):
    """(writes, reads, lhs, rhs) for one simple statement, else None."""
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
    calls = [n for n in re.findall(r"\b([A-Za-z_]\w*)\s*\(", rhs) if n not in TYPE_WORDS]
    if calls or re.search(r"\bvolatile\b", rhs):
        return None
    writes = set(re.findall(r"[A-Za-z_]\w*", lhs))
    reads = (set(re.findall(r"[A-Za-z_]\w*", rhs)) | (writes - {lhs})) - TYPE_WORDS
    return writes - TYPE_WORDS, reads, lhs, rhs


def may_swap(a, b):
    sa, sb = simple(a), simple(b)
    if not sa or not sb:
        return False
    wa, ra, wb, rb = sa[0], sa[1], sb[0], sb[1]
    return not (wa & wb) and not (wa & rb) and not (wb & ra)


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
    typ, star, name, arr, expr = resp.DECL.match(decls[i]).groups()
    new = list(decls)
    new[i] = "    %s %s%s%s;" % (typ, star, name, arr or "")
    return "split init %s" % name, (prefix, new, [["    %s = %s;" % (name, expr)]] + list(groups), suffix)


def mut_join_init(state, rng):
    prefix, decls, groups, suffix = state
    cands = []
    for gi, group in enumerate(groups):
        if not single(group):
            continue
        m = re.match(r"^\s*(\w+)\s*=\s*(.+?);\s*$", group[0])
        if not m:
            continue
        for di, d in enumerate(decls):
            dm = resp.DECL.match(d)
            if dm and dm.group(3) == m.group(1) and dm.group(5) is None and not dm.group(4):
                cands.append((gi, di, m.group(1), dm, m.group(2)))
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
        m = IF_ELSE.match("\n".join(group))
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
        new_groups[i] = ["    %s = (%s) ? %s : %s;" % (m.group(2), m.group(1), m.group(3), m.group(5))]
    else:
        new_groups[i] = ["    if (%s) {" % m.group(2),
                         "        %s = %s;" % (m.group(1), m.group(3)),
                         "    } else {",
                         "        %s = %s;" % (m.group(1), m.group(4)),
                         "    }"]
    return "ternary %s" % kind, (prefix, decls, new_groups, suffix)


def mut_sameline(state, rng):
    prefix, decls, groups, suffix = state
    joined = [i for i, g in enumerate(groups)
              if single(g) and g[0].rstrip().endswith(";") and g[0].count(";") == 2]
    if joined:
        i = rng.choice(joined)
        a, b = groups[i][0].split(";", 1)
        new_groups = list(groups)
        new_groups[i] = [a + ";"]
        new_groups.insert(i + 1, [b.strip()])
        return "split line", (prefix, decls, new_groups, suffix)
    pairs = [i for i in range(len(groups) - 1)
             if single(groups[i]) and single(groups[i + 1])
             and may_swap(groups[i][0], groups[i + 1][0])]
    if not pairs:
        return None
    i = rng.choice(pairs)
    new_groups = list(groups)
    new_groups[i:i + 2] = [[groups[i][0].rstrip() + " " + groups[i + 1][0].strip()]]
    return "join line", (prefix, decls, new_groups, suffix)


TEMP_SAFE = re.compile(r"^\s*(\w+)\s*=\s*([A-Za-z_]\w*)\s*([-+*/&|^])\s*([A-Za-z_]\w*)\s*;$")

# An additive chain of side-effect-free atoms, e.g. `g + a0 * 0x158 + 0x2e6`.
# Operand order inside an address expression decides which register mwcc puts the
# stride product in, which is the duplicate `mul`/`ldr` exchange across the ov002
# prologue walls; no other axis reaches it.
ATOM = (r"(?:[A-Za-z_]\w*(?:\[[^\]]*\])?)"
        r"(?:\s*\*\s*(?:[A-Za-z_]\w*|0x[0-9a-fA-F]+|\d+))?"
        r"|0x[0-9a-fA-F]+|\d+")
CHAIN = re.compile(rf"(?<![\w.])(?:{ATOM})(?:\s*\+\s*(?:{ATOM})){{1,}}")


def _chain_parts(chain):
    return [p.strip() for p in re.split(r"\s*\+\s*", chain)]


def mut_addchain(state, rng):
    """Reorder an additive chain: commutative and associative, so it is free."""
    prefix, decls, groups, suffix = state
    spots = []
    for gi, group in enumerate(groups):
        for li, line in enumerate(group):
            for m in CHAIN.finditer(line):
                parts = _chain_parts(m.group(0))
                if len(parts) >= 2 and len(set(parts)) == len(parts):
                    spots.append((gi, li, line, m.span(), parts))
    if not spots:
        return None
    gi, li, line, (a, b), parts = rng.choice(spots)
    if len(parts) == 2:
        new_parts = parts[::-1]
        label = "chain swap"
    else:
        style = rng.randrange(2)
        if style == 0:
            new_parts = parts[::-1]
            label = "chain reverse"
        else:
            k = rng.randrange(1, len(parts))
            new_parts = parts[k:] + parts[:k]
            label = "chain rot %d" % k
    new_line = line[:a] + " + ".join(new_parts) + line[b:]
    new_groups = list(groups)
    new_groups[gi] = list(groups[gi])
    new_groups[gi][li] = new_line
    return label, (prefix, decls, new_groups, suffix)


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


MUTATIONS = {mut_swap_stmt: "swap stmt", mut_swap_decl: "decl swap",
             mut_rotate_decl: "decl rot", mut_shuffle_decl: "decl shuffle",
             mut_addchain: "chain order",
             mut_split_init: "split init", mut_join_init: "join init",
             mut_compound: "compound", mut_signed: "signed",
             mut_ternary: "ternary", mut_sameline: "line"}


# --------------------------------------------------------------------------
# parallel evaluation
# --------------------------------------------------------------------------

def eval_batch(job):
    """Worker entry point: score a list of bodies, return their scores."""
    mod, addr, size, bodies = job
    return [score_body(mod, addr, size, b) for b in bodies]


class Evaluator:
    """Scores candidate bodies, in worker processes when asked for them."""

    def __init__(self, mod, addr, size, jobs):
        self.mod, self.addr, self.size = mod, addr, size
        self.jobs = max(1, jobs)
        self.pool = None
        if self.jobs > 1:
            try:
                import concurrent.futures
                self.pool = concurrent.futures.ProcessPoolExecutor(max_workers=self.jobs)
            except Exception:
                self.pool = None

    def many(self, bodies):
        if not bodies:
            return []
        if self.pool is None:
            return [score_body(self.mod, self.addr, self.size, b) for b in bodies]
        chunk = max(1, len(bodies) // (self.jobs * 2))
        jobs = []
        for k in range(0, len(bodies), chunk):
            jobs.append((self.mod, self.addr, self.size, bodies[k:k + chunk]))
        return [s for part in self.pool.map(eval_batch, jobs) for s in part]

    def close(self):
        if self.pool is not None:
            self.pool.shutdown(wait=False)


# --------------------------------------------------------------------------
# sweeps
# --------------------------------------------------------------------------

def decl_sweep(evaluator, state, max_decls, verbose):
    prefix, decls, groups, suffix = state
    n = len(decls)
    if n < 2 or n > max_decls:
        return None, state, "base", 0
    base = evaluator.many([body_of(render(state))])[0]
    best = (base, list(decls), "base")
    tried = 0
    for perm in itertools.permutations(decls):
        cand = (prefix, list(perm), groups, suffix)
        text_c = render(cand)
        s = evaluator.many([body_of(text_c)])[0]
        tried += 1
        if s is None:
            continue
        if s[0] == 0:
            return text_c, cand, "%d decls, sweep order %d" % (n, tried), tried
        if best[0] is None or s < best[0]:
            best = (s, list(perm), "sweep order %d" % tried)
    if verbose:
        print("  declaration sweep: %d orders, best structural %s"
              % (tried, best[0][0] if best[0] else "-"))
    return None, (prefix, best[1], groups, suffix), best[2], tried


def marked_sweep(mod, addr, size, text, limit):
    lines = text.splitlines()
    slots = [i for i, l in enumerate(lines) if l.rstrip().endswith(MARK)]
    if not slots:
        return None
    marked = [lines[i][: lines[i].rindex(MARK)].rstrip() for i in slots]
    total = math.factorial(len(marked))
    print("%d marked statements (%d orders%s)"
          % (len(marked), total, ", sampled" if total > limit else ""))
    for n, order in enumerate(itertools.permutations(marked)):
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


# --------------------------------------------------------------------------
# random search
# --------------------------------------------------------------------------

def search(evaluator, text, iters, islands, budget, seed, use_temps, batch, verbose):
    """Anneal over semantics-preserving rewrites, with a mutation bandit.

    Each step proposes `batch` candidates (default: two per worker) by mutating
    the current state one to three times, scores them together, and moves to the
    best one that annealing accepts.  Bandit weights favour the mutations that
    have produced improvements, which matters because the productive axis
    changes as a function gets closer.
    """
    rng = random.Random(seed)
    base = read_state(text)
    muts = list(MUTATIONS) + ([mut_temp] if use_temps else [])
    weight = {m: 1.0 for m in muts}
    frontier = []
    recent = collections.deque(maxlen=1200)
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
            state = read_state(frontier[rng.randrange(min(2, len(frontier)))][2])
        current = evaluator.many([body_of(render(state))])[0]
        for _ in range(iters):
            if time.time() - started > budget:
                break
            proposals, labels = [], []
            for _ in range(batch):
                cand, labs = state, []
                for _ in range(rng.choices((1, 2, 3), weights=(65, 25, 10))[0]):
                    mut = rng.choices(muts, weights=[weight[m] for m in muts])[0]
                    got = mut(cand, rng)
                    if not got:
                        break
                    labs.append(got[0])
                    cand = got[1]
                if not labs:
                    continue
                text_c = render(cand)
                if text_c in recent_set:
                    continue
                if len(recent) == recent.maxlen:
                    recent_set.discard(recent[0])
                recent.append(text_c)
                recent_set.add(text_c)
                proposals.append((text_c, " + ".join(labs)))
                labels.append((labs, mut))
            if not proposals:
                continue
            scores = evaluator.many([body_of(p[0]) for p in proposals])
            for (text_c, label), s in zip(proposals, scores):
                tried += 1
                if s is None:
                    failed += 1
                    continue
                if s[0] == 0:
                    need = use_temps and "temp tmp" in label
                    print("MATCH after %d tries (%ds): %s"
                          % (tried, time.time() - started, label))
                    frontier.insert(0, (s, label + (" NEEDS-EXPLANATION" if need else ""), text_c))
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
                    current = s
                    state = read_state(text_c)
                    continue
                if s[0] < current[0] or (s[0] == current[0] and s[1] <= current[1]):
                    delta = 0
                else:
                    temp = max(0.15, 3.0 * (0.93 ** tried))
                    delta = (s[0] - current[0]) * 2 + max(0, s[1] - current[1]) / 20.0
                    if rng.random() >= math.exp(-delta / temp):
                        continue
                for _, mut in labels:
                    weight[mut] = min(12.0, weight[mut] + 0.5)
                state, current = read_state(text_c), s
                if verbose and tried % 100 < batch:
                    print("  [%ds] %d tries, structural %d cost %d  %s"
                          % (time.time() - started, tried, s[0], s[1], label))
                break
    return frontier, tried, failed


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------

def main(argv):
    flags = [a for a in argv if a.startswith("--")]
    args = [a for a in argv if not a.startswith("--")]
    if len(args) < 4:
        print(__doc__)
        return 2
    mod, addr, size, path = args[0], int(args[1], 0), int(args[2], 0), pathlib.Path(args[3])
    opt = {"--iters": 200, "--islands": 4, "--time": 180, "--seed": 1, "--jobs": 1,
           "--sweep-max": 6, "--limit": 4000, "--top": 4, "--batch": 0}
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
    out = pathlib.Path("build/cand/permute_%s.c" % name)

    if any(l.rstrip().endswith(MARK) for l in text.splitlines()):
        found = marked_sweep(mod, addr, size, text, opt["--limit"])
        if found:
            out.write_text(found[0] + "\n")
            print("wrote %s" % out)
            if install:
                kept = t10.try_unit(name, addr, size, found[0], mod=mod)
                print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
                if not kept:
                    return 3
            return 0

    evaluator = Evaluator(mod, addr, size, opt["--jobs"])
    try:
        match, seed_state, seed_label, swept = decl_sweep(evaluator, read_state(text),
                                                          opt["--sweep-max"], verbose)
        if swept:
            print("declaration sweep: %d orders, best state %s" % (swept, seed_label))
        if match:
            out.write_text(match)
            print("wrote %s  (%s)" % (out, seed_label))
            if install:
                kept = t10.try_unit(name, addr, size, match, mod=mod)
                print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
                if not kept:
                    return 3
            return 0

        batch = opt["--batch"] or max(2, 2 * opt["--jobs"])
        frontier, tried, failed = search(evaluator, render(seed_state), opt["--iters"],
                                         opt["--islands"], opt["--time"], opt["--seed"],
                                         use_temps, batch, verbose)
    finally:
        evaluator.close()

    print("%d candidates, %d did not compile, best:" % (tried, failed))
    if not frontier:
        print("  no improving state found - widen --iters/--islands/--time or add --temps")
    for s, label, _ in frontier[: opt["--top"]]:
        print("  %3d structural  cost %4d  %+d B  %s" % (s[0], s[1], s[2], label))
    if frontier and frontier[0][0][0] == 0:
        best = frontier[0]
        out.write_text(best[2])
        print("wrote %s  (%s)" % (out, best[1]))
        if install:
            kept = t10.try_unit(name, addr, size, best[2], mod=mod)
            print("install: %s" % ("KEPT" if kept else "rejected by the ROM digest"))
            if not kept:
                return 3
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
