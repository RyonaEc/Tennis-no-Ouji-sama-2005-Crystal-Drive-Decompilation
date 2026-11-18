#include "types.h"

s32 func_ov004_0205240c(u8 *base, s32 i)
{
    return *(s32 *)(base + (i << 5) + 0x28) == 0;
}
