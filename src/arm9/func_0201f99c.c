#include "types.h"

extern u32 func_0201d3c4(u32);
extern u32 func_0201f438(u32, u32, u32, u32, u32);

u32 func_0201f99c(u32 a0, u32 a1, u32 a2)
{
    u8 *p = (u8 *)func_0201d3c4(a1);
    if (p == 0) return 0;
    return func_0201f438(a0, (u32)p, p[6], p[5], a2);
}