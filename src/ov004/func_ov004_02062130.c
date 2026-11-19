#include "types.h"

extern s32 func_ov004_0206256c(u32 a0);
extern s32 func_02010150(u32 a0);
extern void func_ov004_020624e0(u32 a0, u32 a1);

void func_ov004_02062130(void)
{
    if (func_ov004_0206256c(6))
        return;
    if (func_ov004_0206256c(5))
        return;
    if (!func_ov004_0206256c(4))
        return;
    if (!func_02010150(0x36))
        return;
    if (!func_02010150(0x37))
        return;
    func_ov004_020624e0(5, 1);
}