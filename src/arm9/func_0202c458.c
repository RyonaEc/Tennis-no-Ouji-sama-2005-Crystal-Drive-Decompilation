#include "types.h"

u32 func_0202c458(u32 v)
{
    s32 i;
    u32 r = 0;
    u32 p;

    for (i = 0; i < 8; i++)
        if (((v >> (i * 4)) & 0xf) >= 10)
            return 0;

    p = 1;
    for (i = 0; i < 8; i++)
    {
        r += ((v >> (i * 4)) & 0xf) * p;
        p *= 10;
    }
    return r;
}