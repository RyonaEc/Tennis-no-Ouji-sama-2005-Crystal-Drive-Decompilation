#include "types.h"

extern u8 *data_ov003_020eb720;

s16 func_ov003_020c7d18(s32 a0, s32 a1) {
    u8 *b;
    s32 i;
    s32 s;
    s16 r;

    b = data_ov003_020eb720;
    i = a1 + a0 * 9;
    s = *(u16 *)(b + 0xb600 + 0x26) + *(u16 *)(b + 0xbac2 + i * 2);
    r = (s16)(s % 360);
    if (r < 90)
        return r;
    if (r < 180)
        return (s16)(r * 1);
    if (r < 270)
        return 360 - r;
    return 90 - (r - 270);
}