"""A bare `extern u8 data_x[];` used as a value is not a valid u32 expression.

mwcc rejects `return data_x;` with its odd `<string not found>` diagnostic, which
is what made ~100 arm9 candidates fail to compile.  Render symbols as
`(u32)data_x` in value positions, while keeping base positions as the bare name
so loads still emit `*(u32 *)(data_x)`.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# e(): symbols used as values need a cast to u32.
old_sym = """    if t == 'sym':
        return x[1]"""
new_sym = """    if t == 'sym':
        return f'(u32){x[1]}'"""
assert t.count(old_sym) == 1
t = t.replace(old_sym, new_sym, 1)

# e(): a symbol used as a load base keeps its array form.
old_load = """        b = f'(u8 *)({e(base)})' if base[0] != 'sym' else e(base)
        return f'*({ty} *)({b} + {off})' if off else f'*({ty} *)({b})'"""
new_load = """        b = f'(u8 *)({e(base)})' if base[0] != 'sym' else base[1]
        return f'*({ty} *)({b} + {off})' if off else f'*({ty} *)({b})'"""
assert t.count(old_load) == 1
t = t.replace(old_load, new_load, 1)

# emit(): same for a store destination.
old_store = """                b = f'(u8 *)({e(base)})' if base[0] != 'sym' else e(base)"""
new_store = """                b = f'(u8 *)({e(base)})' if base[0] != 'sym' else base[1]"""
assert t.count(old_store) == 1
t = t.replace(old_store, new_store, 1)

P.write_text(t, encoding="utf-8")
print("symbol value casts added")
