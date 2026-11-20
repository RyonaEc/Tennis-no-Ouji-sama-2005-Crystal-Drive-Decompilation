#include "types.h"

extern u32 func_0200fac8(u32);
extern void func_0200fdf4(u32, u32 *);

s32 func_0200fd58(void)
{
    u32 buf[4];
    s32 total = 0;
    s32 i;

    for (i = 0; i < 0x24; i++)
    {
        if (i == 0xb || i == 0x14 || i == 0x25) continue;
        if (func_0200fac8(i) == 0) return -1;
        func_0200fdf4(i, buf);
        total += buf[3];
    }
    return total;
}