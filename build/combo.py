import importlib.util, itertools
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
fm=load("build/fastmatch.py","fm")
mod,addr,size="ov002",0x0204fdb0,200
want=fm.retail(mod,addr,size)
args = {
 "c8": "(s16)({A} << 8)",
 "s8": "(s16)({A}) << 8",
 "p8": "{A} << 8",
}
rets = {"s16":"s16","s32":"s32"}
muls = {"m1":"(s16){R} * 360 / 65536", "m2":"{R} * 360 / 65536", "m3":"(s16)({R} * 360) / 65536"}
A0="func_02034148(*(s32 *)(e + 0x60))"
A1="*(s32 *)(e + 0x5c)"
for (ka,fa),(kr,rt),(km,fm_) in itertools.product(args.items(),rets.items(),muls.items()):
    PT="s16, s16" if ka!="p8" else "s32, s32"
    body=f'''#include "types.h"
extern s32 func_02034148(s32);
extern {rt} func_ov002_0204f70c({PT});
extern void func_ov002_0204f770(s32, u32, s32 *);
void func_ov002_0204fdb0(u8 *e, s16 a1, s16 a2)
{{
    s32 out[3];
    s16 ang = a1 + {fm_.format(R="func_ov002_0204f70c("+fa.format(A=A0)+", "+fa.format(A=A1)+")")};
    s32 sg = *(s32 *)(e + 0x60) < 0 ? -1 : 1;
    func_ov002_0204f770(a2 * sg, (s16)(sg * ((ang << 8) / 360)) & 0xff, out);
    *(s32 *)(e + 0x68) = out[0];
    *(s32 *)(e + 0x6c) = out[1];
}}
'''
    got=fm.compile_text(body,"p")
    if fm.compare(body,mod,addr,size):
        print("MATCH",ka,kr,km); print(body); break
    d=next((i for i in range(min(len(got),len(want))) if got[i]!=want[i]),min(len(got),len(want)))
    print(f"{ka:3s} {kr:3s} {km} len {len(got)} first {d:#05x}")
