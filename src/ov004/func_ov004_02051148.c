#include "types.h"

extern s32 func_ov004_02051448(u8 *, u32);

s32 func_ov004_02051148(u8 *p, u32 a1, u32 a2, u16 a3, u32 a4)
{
    if (p == 0)
        return 0;
    if (a2 == 0)
        return 0;
    func_ov004_02051448(p, 0x020511c0);
    *(u32 *)(p + 0x9c) = a2;
    *(u16 *)(p + 0xa0) = 0;
    *(u16 *)(p + 0xa2) = 0;
    *(u16 *)(p + 0xa4) = a4;
    *(u16 *)(p + 0xa6) = a3;
    *(u32 *)(p + 0xa8) = a1;
    return 1;
}