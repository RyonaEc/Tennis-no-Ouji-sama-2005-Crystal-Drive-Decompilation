#include "types.h"

extern s32 func_ov004_0206256c(u32 a0);
extern s32 func_0200fac8(u32 a0);
extern void func_ov004_020624e0(u32 a0, u32 a1);

void func_ov004_0206222c(void)
{
    if (func_ov004_0206256c(0xb))
        return;
    if (func_ov004_0206256c(0xa))
        return;
    if (!func_0200fac8(4))
        return;
    if (!func_0200fac8(0xe))
        return;
    if (!func_0200fac8(0x1f))
        return;
    func_ov004_020624e0(0xa, 1);
}