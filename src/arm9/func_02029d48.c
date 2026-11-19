#include "types.h"

void func_02029d48(u32 *p)
{
    u32 a = p[0];
    u32 b = p[1];

    if (a != 0)
        *(u32 *)(a + 4) = b;
    if (b != 0)
        *(u32 *)b = a;
    p[0] = 0;
    p[1] = p[0];
}