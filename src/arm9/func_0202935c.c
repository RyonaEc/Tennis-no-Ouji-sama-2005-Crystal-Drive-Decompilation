#include "types.h"

extern u32 func_02028af4(u8 *, u32);
extern void func_02028c98(u8 *, u32);
extern u32 func_02029444(u32);
extern void func_020293a4(void);

u32 func_0202935c(u8 *a0)
{
    u32 r;
    r = func_02028af4(a0, *(u32 *)(a0 + 0x10));
    func_02028c98(a0, r);
    if (func_02029444(*(u32 *)(a0 + 8)) != 0) func_020293a4();
    return *(u32 *)(a0 + 0x14) == 0;
}