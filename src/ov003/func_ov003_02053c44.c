#include "types.h"

extern u8 *data_ov003_020eb720;
extern s32 func_0200ef7c(s32 a, s32 b);

s32 func_ov003_02053c44(s32 a0, s32 a1) {
    u8 *b;
    u8 *p;
    u16 idx;
    u16 v;
    u16 r;

    b = data_ov003_020eb720;
    p = b + 0xb8dc;
    if (*(u16 *)(b + 0x254) != 0) {
        idx = *(u16 *)(p + 0x21e);
        v = *(u16 *)(p + *(u16 *)(p + idx * 2 + 0x110) * 2 + 0xa0);
        if (a0 != 0)
            return a1;
        r = (u16)func_0200ef7c(v, idx);
        if (r < 0x45)
            a1 = r;
        return a1;
    }
    return a1;
}