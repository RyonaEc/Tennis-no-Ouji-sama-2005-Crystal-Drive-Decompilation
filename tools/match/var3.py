import importlib.util, sys, pathlib
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("tools/match/fastmatch.py","fm")
mod,addr,size=sys.argv[1],int(sys.argv[2],0),int(sys.argv[3],0)
body=pathlib.Path(sys.argv[4]).read_text()
got=fm.compile_text(body,"probe"); want=fm.retail(mod,addr,size)
if fm.compare(body,mod,addr,size): print("MATCH"); sys.exit(0)
print(f"got {len(got)} want {len(want)}")
W=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.parse_ins(addr,size)]
G=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.md.disasm(got,addr)]
import difflib
for l in difflib.unified_diff(W,G,"retail","got",n=1,lineterm=""): print(l)
