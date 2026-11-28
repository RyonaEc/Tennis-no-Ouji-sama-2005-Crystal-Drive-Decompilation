#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_02062dbc(u32 a0);

void func_ov002_02063720(u32 a0, s32 a1)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *s = base + 0x1744 + a0 * 0x38;
    u32 buf[2] = { 0, 0 };
    u8 *e = base + 0x2e4;
    s32 i;
    s32 n = 0;
    u32 v;

    for (i = 0; i < 4; i++, e += 0x158) {
        if (e[0] == 0)
            continue;
        if ((((u32)e[2]) >> 1) == a1)
            continue;
        if (*(u16 *)(e + 0x50) & 0x20)
            buf[n] = 1;
        else if (*(u16 *)(e + 0x50) & 0x10)
            buf[n] = 2;
        n++;
        if (n == 2)
            break;
    }
    v = (n == 2) ? buf[func_ov002_02062dbc(2)] : buf[0];
    if (v == 0)
        return;
    if (func_ov002_02062dbc(5) == 0) {
        if (v == 1)
            v = 2;
        else
            v = 1;
    }
    if (v == 1)
        *(u8 *)(s + 8) = 0;
    else
        *(u8 *)(s + 8) = 2;
}