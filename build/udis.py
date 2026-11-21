import importlib.util, sys
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
mod=sys.argv[1]
fm.auto.set_module(mod)
for a in sys.argv[2:]:
    addr=int(a,0)
    name,size=fm.auto.FUNCS.get(addr,("?",0))
    print(f"=== {mod} 0x{addr:08x} {name} size {size}")
    for i in fm.parse_ins(addr,size):
        extra=""
        print(f"  {i.address:08x} {i.mnemonic} {i.op_str}")
