#include "types.h"

extern u8 *func_0201d554(u32);
extern u32 func_0201e2ac(u32, u32, u32, u32, u32, u32);

u32 func_0201e670(u32 a0, u32 a1)
{
    u8 *p = func_0201d554(a1);

    if (p == 0) return 0;
    return func_0201e2ac(a0, p[9], *(u16 *)(p + 4), p[8], (u32)p, a1);
}