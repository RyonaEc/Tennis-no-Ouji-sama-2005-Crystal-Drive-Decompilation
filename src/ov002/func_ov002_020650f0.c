#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_02062dbc(u32 a0);
extern u32 func_ov002_0207fd90(u32 a0, u32 a1, u32 a2);

void func_ov002_020650f0(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *e = b + 0x2e4 + a0 * 0x158;
    u8 *s = b + 0x1744 + a0 * 0x38;
    s32 v = func_ov002_02062dbc(0x100);
    s32 off;
    s32 on;

    if (e[0] == 0x1a) {
        if (func_ov002_0207fd90((u32)e[2] >> 1, e[0], 1) == 0) {
            off = 0x20;
            on = 0xe0;
        } else {
            off = 0x80;
            on = off;
        }
        if (*(u32 *)(s + 0x24) & 0x10) {
            if (v < on)
                *(u32 *)(s + 0x24) &= ~0x10;
        } else {
            if (v < off)
                *(u32 *)(s + 0x24) |= 0x10;
        }
    } else {
        if (*(u32 *)(s + 0x24) & 0x10) {
            if (v < 0xe0)
                *(u32 *)(s + 0x24) &= ~0x10;
        } else {
            if (v < 0x20)
                *(u32 *)(s + 0x24) |= 0x10;
        }
    }
}