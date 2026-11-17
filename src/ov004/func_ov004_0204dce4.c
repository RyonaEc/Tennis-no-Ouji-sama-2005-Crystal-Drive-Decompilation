#include "types.h"

s32 func_ov004_0204dce4(u8 *a0, u16 a1)
{
    if (!*(u32 *)(a0 + 0x10)) return 0;
    *(u16 *)(a0 + 8) = a1;
    return 1;
}
