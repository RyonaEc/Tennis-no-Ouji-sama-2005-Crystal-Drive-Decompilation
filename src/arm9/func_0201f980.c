#include "types.h"

void func_0201f980(u8 **a0)
{
    u8 *p = *a0;

    if (p)
    {
        u32 *q = (u32 *)(p + 0x108);
        *q |= 4;
    }
}