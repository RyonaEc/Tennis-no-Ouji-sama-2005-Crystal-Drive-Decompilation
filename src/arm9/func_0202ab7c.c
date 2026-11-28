#include "types.h"

extern void func_0202a8bc(u8 *p);

s32 func_0202ab7c(u8 *p, u8 *src, s32 n)
{
    if (n == 0)
        return 0;
    if (p == 0 || src == 0)
        return 1;
    if (*(u32 *)(p + 0x60) != 0) {
        *(u32 *)(p + 0x64) = 3;
        return 3;
    }
    {
        u32 st = *(u32 *)(p + 0x64);

        if (st != 0)
            return st;
    }
    while (n-- != 0 && *(u32 *)(p + 0x64) == 0) {
        *(u8 *)(p + 0x20 + (*(u32 *)(p + 0x1c))++) = *src;
        *(u32 *)(p + 0x14) += 8;
        if (*(u32 *)(p + 0x14) == 0) {
            *(u32 *)(p + 0x18) += 1;
            if (*(u32 *)(p + 0x18) == 0)
                *(u32 *)(p + 0x64) = 1;
        }
        if (*(u32 *)(p + 0x1c) == 0x40)
            func_0202a8bc(p);
        src++;
    }
    return 0;
}