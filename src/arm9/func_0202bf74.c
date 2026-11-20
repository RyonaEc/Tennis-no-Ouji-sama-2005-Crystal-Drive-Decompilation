#include "types.h"

extern u32 func_0202c090(u32);

u32 func_0202bf74(u32 a0)
{
    u32 r;

    switch (a0)
    {
    case 1:
        r = 1;
        break;
    case 3:
        r = 2;
        break;
    case 2:
        r = 3;
        break;
    default:
        r = 0;
        break;
    }
    if (r == 0) return 0xffff;
    return func_0202c090(r);
}