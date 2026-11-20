#include "types.h"


extern u8 *data_ov002_02102070;

s32 func_ov002_020b8750(u32 a)
{
    u8 *b;
    u8 *t;
    u8 *q;
    u32 i;

    b = data_ov002_02102070;
    t = b + 0x4bbc;
    if (b[0x8c] == 0)
        return 0;
    t += a;
    i = t[4];
    q = b + 0x2e4 + i * 0x158;
    if (q[0] == 0x17) {
        if (q[0xd1] == 2)
            return 1;
    }
    return 0;
}
