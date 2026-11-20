#include "types.h"

extern u32 func_02017784(u32, u32);

u32 func_02017720(u32 a0, u32 a1)
{
    u32 i = 0;
    u32 v = func_02017784(a0, i);

    while (v != 0)
    {
        if (i == a1) return v;
        i++;
        v = func_02017784(a0, v);
    }
    return 0;
}