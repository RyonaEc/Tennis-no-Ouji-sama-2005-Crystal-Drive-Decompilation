#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_020820f0(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *e = b + 0x36ac;
    u8 *t = b + 0x2e4;
    u32 h;

    if (b[0xba] == 0)
        return;
    if (t[0] == 0)
        return;
    h = (u32)t[2] >> 1;
    if (!(h == 0 && b[0x100] == 3)) {
        if (h != 1)
            return;
        if (b[0x100] != 2)
            return;
    }
    b[0x127] = e[7];
    b[0x128] = e[8];
    b[0x10e] |= 8;
    e[0x27] = 1;
    if (b[0x100] == 3)
        e[0x28] = 1;
    else
        e[0x28] = 0;
    if (b[0x10b] % 2 != 1)
        return;
    e[0x28] = (e[0x28] == 0);
}