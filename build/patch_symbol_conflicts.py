"""Two declaration issues in emit().

1. A name used both as a call target and as a data symbol produced both
   `extern u32 f(u32,u32);` and `extern u8 f[];`, which mwcc rejects.  Keep the
   prototype only; the data use renders as `(u32)f`.
2. Calls to local labels such as `.L_02033bd0` are not valid C identifiers, so
   bail on them instead of emitting uncompilable code.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old = """    for s in sorted(syms):
        protos.append(f'extern u8 {s}[];')"""
new = """    call_names = {cn for cn, _ca, _nar in fn['calls']}
    for s in sorted(syms):
        if s in call_names:
            continue
        if not IDENT_RE.match(s):
            raise Bail('non-identifier data symbol')
        protos.append(f'extern u8 {s}[];')"""
assert t.count(old) == 1, t.count(old)
t = t.replace(old, new, 1)

old_counts = """    counts = {}
    for cn, ca, _nar in fn['calls']:"""
new_counts = """    for cn, _ca, _nar in fn['calls']:
        if not IDENT_RE.match(cn):
            raise Bail('non-identifier call target')
    counts = {}
    for cn, ca, _nar in fn['calls']:"""
assert t.count(old_counts) == 1
t = t.replace(old_counts, new_counts, 1)

# define the identifier pattern next to the other module constants
anchor = "ZERO = b\"\\0\"\n"
if anchor in t:
    t = t.replace(anchor, anchor + "IDENT_RE = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')\n", 1)
else:
    marker = "import re\n"
    t = t.replace(marker, marker + "IDENT_RE = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')\n", 1)

P.write_text(t, encoding="utf-8")
print("symbol conflicts handled")
