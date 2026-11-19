#include "types.h"

extern s32 func_ov004_0206256c(u32 a0);
extern u32 func_0200fb38(void);
extern void func_ov004_020624e0(u32 a0, u32 a1);

void func_ov004_020621c8(void)
{
    if (func_ov004_0206256c(4))
        return;
    if (func_ov004_0206256c(3))
        return;
    if (!(func_0200fb38() & 8))
        return;
    func_ov004_020624e0(3, 1);
}