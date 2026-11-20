#include "types.h"

s32 func_02019584(u16 *a, s32 start, s32 end)
{
    u16 *p = a + start;
    u16 *e = a + end;

    for (; (u32)p <= (u32)e; p++)
        if (*p != 0xffff)
            return 0;
    return 1;
}