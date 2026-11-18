#include "types.h"

void func_ov005_0204f2ac(u8 *p, s32 a, s32 b, s32 c)
{
    if (!p)
        return;
    *(u16 *)(p + 4) = ((a << 12) & 0xf000) | ((b << 10) & 0xc00) | (c & 0x3ff);
}
