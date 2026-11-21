import importlib.util, sys, pathlib
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
mod,addr,size=sys.argv[1],int(sys.argv[2],0),int(sys.argv[3],0)
body=pathlib.Path(sys.argv[4]).read_text()
got=fm.compile_text(body,"probe"); want=fm.retail(mod,addr,size)
print(f"got {len(got)} want {len(want)}")
if fm.compare(body,mod,addr,size):
    print("MATCH"); sys.exit(0)
first=next((i for i in range(min(len(got),len(want))) if got[i]!=want[i]), min(len(got),len(want)))
print(f"first diff {first:#x}")
for i in fm.parse_ins(addr,size):
    if i.address-addr>=first-12 and i.address-addr<=first+16: print("  W",hex(i.address),i.mnemonic,i.op_str)
d=fm.md.disasm(got,addr)
for i in d:
    if i.address-addr>=first-12 and i.address-addr<=first+16: print("  G",hex(i.address),i.mnemonic,i.op_str)
