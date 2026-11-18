#include "types.h"

extern s32 func_ov004_02050cbc(void);

s32 func_ov004_02050c98(u8 *a0)
{
    if (*(u32 *)(a0 + 0x98) == (u32)func_ov004_02050cbc) {
        *(u16 *)(a0 + 0xa6) = 2;
        return 1;
    }
    return 0;
}
