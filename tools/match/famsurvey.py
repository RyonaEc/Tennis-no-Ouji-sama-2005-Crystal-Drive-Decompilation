import importlib.util, pathlib, sys, collections
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load('tools/match/fastmatch.py','fm')
mods=sys.argv[1:] or ['arm9']
for mod in mods:
    fm.auto.set_module(mod)
    srcdir=pathlib.Path('src/arm9' if mod=='arm9' else 'src/'+mod)
    have={p.stem for p in srcdir.glob('*.c')}
    groups=collections.defaultdict(list)
    for addr,(name,size) in fm.auto.FUNCS.items():
        if name in have or size<0x30: continue
        try:
            ins=fm.parse_ins(addr,size)
        except Exception:
            continue
        sig=tuple(i.mnemonic for i in ins)
        groups[(size,sig)].append((addr,name))
    rows=[(size*len(v),size,len(v),v) for (size,sig),v in groups.items() if len(v)>=2]
    rows.sort(reverse=True)
    print(f'== {mod}: {len(rows)} groups')
    for val,size,n,v in rows[:25]:
        print(f'  {n}x{size} val {val}  ' + ' '.join(f'0x{a:08x}' for a,_ in v[:6]))
