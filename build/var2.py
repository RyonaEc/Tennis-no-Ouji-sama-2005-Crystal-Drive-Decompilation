import importlib.util, sys, pathlib
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
mod,addr,size=sys.argv[1],int(sys.argv[2],0),int(sys.argv[3],0)
body=pathlib.Path(sys.argv[4]).read_text()
got=fm.compile_text(body,"probe"); want=fm.retail(mod,addr,size)
W=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.parse_ins(addr,size)]
G=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.md.disasm(got,addr)]
for i in range(max(len(W),len(G))):
    w=W[i] if i<len(W) else ""; g=G[i] if i<len(G) else ""
    print(("  " if w==g else "! ")+f"{w:44s} | {g}")
