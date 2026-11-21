import importlib.util
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
target=bytes.fromhex("b7600bb6")  # little-endian of 0xb60b60b7
hits=[]
for d in range(2,2000):
    for form in (f"s32 f(s32 v){{ return (v << 8) / {d}; }}",
                 f"s32 f(s32 v){{ return v / {d}; }}"):
        got=fm.compile_text('#include "types.h"\n'+form,"p")
        if target in got:
            hits.append((d,form))
for h in hits[:20]: print(h)
print("hits",len(hits))
