#include "types.h"

extern void func_02025c78(u32, u32);

void func_02027bc0(u32 a0)
{
    u32 p;
    u32 n;
    p = *(u32 *)(a0 + 0x18);
    if (p == 0) return;
    do
    {
        n = *(u32 *)(p + 4);
        *(u32 *)p = 0;
        *(u32 *)(p + 4) = 0;
        func_02025c78(p, 8);
        p = n;
    } while (p != 0);
}