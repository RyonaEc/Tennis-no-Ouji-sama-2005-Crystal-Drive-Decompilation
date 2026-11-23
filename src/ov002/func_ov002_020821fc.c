#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_020821fc(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    u8 *b = g + 0x36ac;
    u8 *a = g + 0x47ac;

    if (a[0x20] == 0)
        return;
    g[0x127] = b[7];
    g[0x128] = b[8];
    g[0x10e] |= 8;
    b[0x25] = 1;
    if (a[0x20] == 1)
        b[0x26] = 1;
    else
        b[0x26] = 0;
    if (g[0x10b] % 2 != 1)
        return;
    b[0x26] = !b[0x26];
}