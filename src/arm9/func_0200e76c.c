#include "types.h"

u32 func_0200e76c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
{
    u32 n = 0;

    if (a0 == a3 || a0 == a4 || a0 == a5) n++;
    if (a1 == a3 || a1 == a4 || a1 == a5) n++;
    if (a2 == a3 || a2 == a4 || a2 == a5) n++;
    return n;
}