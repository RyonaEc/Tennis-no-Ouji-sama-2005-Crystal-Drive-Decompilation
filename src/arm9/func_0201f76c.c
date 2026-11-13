#include "types.h"

extern u32 func_0203b2cc(u32, u32);

u32 func_0201f76c(u8 **pp)
{
    u8 *p = *pp;

    if (p == 0)
        return 0;
    return func_0203b2cc(*(u32 *)(p + 0x13c) * 1000, *(u16 *)(p + 0xbc));
}
