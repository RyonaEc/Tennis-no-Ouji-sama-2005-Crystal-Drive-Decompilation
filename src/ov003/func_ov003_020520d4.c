#include "types.h"

void func_ov003_020520d4(u8 *p, s32 a)
{
    u16 v;

    if (!p)
        return;
    v = *(u16 *)(p + 2) & 0xc1ff;
    *(u16 *)(p + 2) = v + (a << 9);
}
