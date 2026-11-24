import importlib.util, sys, pathlib
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("tools/match/fastmatch.py","fm")
mod,addr,size=sys.argv[1],int(sys.argv[2],0),int(sys.argv[3],0)
body=pathlib.Path(sys.argv[4]).read_text()
# Say up front when the work is already done: a matched unit needs no candidate,
# and a parked one has had this shape tried before.
name=f"func_{addr:08x}" if mod=="arm9" else f"func_{mod}_{addr:08x}"
have=pathlib.Path("src")/mod/(name+".c")
if have.exists(): print(f"note: {name} is already matched ({have})",file=sys.stderr)
note=fm.wall_note(mod,addr)
if note: print(f"note: parked - {note}",file=sys.stderr)
got=fm.compile_text(body,"probe"); want=fm.retail(mod,addr,size)
if fm.compare(body,mod,addr,size): print("MATCH"); sys.exit(0)
print(f"got {len(got)} want {len(want)}")
W=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.parse_ins(addr,size)]
G=[f"{i.address-addr:03x} {i.mnemonic} {i.op_str}" for i in fm.md.disasm(got,addr)]
import difflib
for l in difflib.unified_diff(W,G,"retail","got",n=1,lineterm=""): print(l)
