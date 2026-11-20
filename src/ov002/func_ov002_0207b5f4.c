#include "types.h"

extern u8 data_ov002_02102070[];

s32 func_ov002_0207b5f4(u32 a0, s32 a1)
{
    u32 t;

    t = *(u8 *)(*(u8 **)data_ov002_02102070 + a0 + 0x4f13);
    if (t != 4) {
        if (t == 5)
            a1 = a1 * 3 / 2;
    } else {
        a1 = a1 * 5 / 4;
    }
    return a1;
}