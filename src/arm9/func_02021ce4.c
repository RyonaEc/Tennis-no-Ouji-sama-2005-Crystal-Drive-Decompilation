#include "types.h"

void func_02021ce4(u32 *h, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f)
{
    *(u32 *)*h = 0x29;
    *(u32 *)h[1] = a | (b << 4) | (c << 6) | f | (d << 24) | (e << 16);
    h[0] = h[1] + 4;
    h[1] = h[0] + 4;
}