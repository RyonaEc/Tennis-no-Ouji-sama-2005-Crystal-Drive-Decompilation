#include "types.h"

s32 func_02018318(u8 *p)
{
    u32 *q = (u32 *)(p + 0x24);

    *q = *(u32 *)(p + 0x18);
    q[2] = 0;
    return 0;
}