#include "types.h"

extern u8 *data_ov002_02102070;
extern u8 data_ov002_02100a9c[];
extern u8 data_ov002_02100a94[];
extern u32 func_ov002_0205a510(u32 x);
extern void func_ov002_0204c884(u32 a0, u32 a1);
extern void func_ov002_0204f3cc(u32 a0, u32 a1, u32 a2);

s32 func_ov002_0204d350(u32 a0, u32 a1, s32 a2)
{
    u8 *base = data_ov002_02102070;
    u8 *tbl = base + 0x274;
    s32 r;
    u8 *e;

    r = func_ov002_0205a510(a2);

    if (a0 >= 4) {
        return 0;
    }
    if (a1 == 0 || a1 >= 0x75) {
        return 0;
    }
    if (r >= *(u16 *)(data_ov002_02100a9c + a1 * 0x30)) {
        return 0;
    }

    e = tbl + a0 * 0x1c;
    e[2] = a2;

    if (a1 != e[0]) {
        if (*(u32 *)(tbl + 0x18 + a0 * 0x1c) & 0x400) {
            func_ov002_0204c884(a0, a1);
        } else {
            func_ov002_0204f3cc(*(u32 *)(data_ov002_02100a94 + a1 * 0x30), a0 << 5, 0x20);
        }
    }

    *(u32 *)(tbl + 0x18 + a0 * 0x1c) |= 2;
    e[0] = a1;
    *(u8 *)(tbl + a0 * 0x1c + 1) = r;
    *(u16 *)(tbl + a0 * 0x1c + 4) = 0;
    *(u16 *)(tbl + a0 * 0x1c + 6) = 0;
    *(u32 *)(tbl + 0x18 + a0 * 0x1c) &= ~0x82c;

    return 1;
}