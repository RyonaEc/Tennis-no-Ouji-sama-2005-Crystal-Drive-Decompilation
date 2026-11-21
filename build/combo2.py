import importlib.util, itertools
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
mod,addr,size="ov002",0x0204fdb0,200
want=fm.retail(mod,addr,size)
A0="func_02034148(*(s32 *)(e + 0x60))"
A1="*(s32 *)(e + 0x5c)"
args={
 "u8":"(s16)((u32){A} << 8)",
 "c8":"(s16)({A} << 8)",
 "sr":"({A} << 8) >> 16",
}
muls={
 "M1":"s16 s = (s16)func_ov002_0204f70c({ARGS});\n    s16 ang = a1 + (s16)(s * 360 / 65536);",
 "M2":"s16 s = (s16)func_ov002_0204f70c({ARGS});\n    s16 d = (s16)(s * 360 / 65536);\n    s16 ang = a1 + d;",
 "M3":"s16 ang = a1 + (s16)((s16)func_ov002_0204f70c({ARGS}) * 360 / 65536);",
 "M4":"s32 s = func_ov002_0204f70c({ARGS});\n    s16 ang = a1 + (s16)(s * 360 / 65536);",
}
for (ka,fa),(km,fm_) in itertools.product(args.items(),muls.items()):
    body=f'''#include "types.h"
extern s32 func_02034148(s32);
extern s16 func_ov002_0204f70c(s16, s16);
extern void func_ov002_0204f770(s32, u32, s32 *);
void func_ov002_0204fdb0(u8 *e, s16 a1, s16 a2)
{{
    s32 out[3];
    {fm_.format(ARGS=fa.format(A=A0)+", "+fa.format(A=A1))}
    s32 sg = *(s32 *)(e + 0x60) < 0 ? -1 : 1;
    func_ov002_0204f770(a2 * sg, (s16)(sg * ((ang << 8) / 360)) & 0xff, out);
    *(s32 *)(e + 0x68) = out[0];
    *(s32 *)(e + 0x6c) = out[1];
}}
'''
    got=fm.compile_text(body,"p")
    if fm.compare(body,mod,addr,size):
        print("MATCH",ka,km); print(body); break
    d=next((i for i in range(min(len(got),len(want))) if got[i]!=want[i]),min(len(got),len(want)))
    print(f"{ka:3s} {km} len {len(got)} first {d:#05x}")
