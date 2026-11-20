#include "types.h"

extern u32 func_020279e0(u32, u32, u32);
extern void func_02026ee4(u32, u32, u32, u32, u32);

void func_02027190(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u32 r;
    r = func_020279e0(a0, a3, a4);
    func_02026ee4(0xf, a0, a1, a2, r);
}