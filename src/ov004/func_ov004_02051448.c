#include "types.h"

extern void func_02026af4(u8 *, u32, u32);

s32 func_ov004_02051448(u8 *p, u32 v)
{
    if (p == 0)
        return 0;
    func_02026af4(p + 0x9c, 0, 0x10);
    *(u32 *)(p + 0x98) = v;
    return 1;
}