#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

u32 func_ov002_020ac8cc(u32 a0)
{
    u8 *base = B;
    u32 off = a0 * 0x158;
    u8 *e = base + 0x2e4 + off;
    u8 *r = base + 0x274 + *(u8 *)(base + 0x2e6 + off) * 0x1c;
    u8 *g = base + 0x1824;
    u32 *p = (u32 *)(e + 0xa8);
    u32 res = 0;
    u32 v;

    if (*(u8 *)(base + a0 + 0x13c) == 0)
        return 0;
    v = *(u32 *)(g + 0xb8);
    if ((v & 1) && !(v & 0x10000000))
        goto fail;
    if (r[2] == 0xb || (u8)(r[2] + 0xf0) <= 1) {
        *p = 0;
        res = 1;
    }
    if (r[2] == 0 || r[2] == 0x20 || (u8)(r[2] + 0xff) <= 3) {
        res = 1;
        *p = 1;
    }
    if (res != 1)
        goto fail;
    *(u32 *)(e + 0xd4) |= 4;
    return 1;
fail:
    return 0;
}