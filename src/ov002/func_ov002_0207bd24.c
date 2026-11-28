#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_0207bd24(u32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u8 *s = base + 0x274 + e[2] * 0x1c;
    u8 *t = base + 0x3654;
    u32 v;

    s[0x10] = 0;
    if (*(u32 *)(e + 0xd4) == 0) {
        if (*(s16 *)(e + 0x138) > 0) {
            s[0x10] = 1;
            *(u32 *)(s + 0x18) |= 0x100;
        } else if (((u8 *)(t + a0))[0x54] == 1) {
            *(u32 *)(s + 0x18) |= 0x200;
        }
    }
    switch (e[0]) {
    case 2:
        v = *(u32 *)(e + 0xd4);
        if ((v & 0x40) == 0)
            return;
        if (v == 0x40)
            s[0x10] = 1;
        else
            s[0x10] = 2;
        break;
    case 11:
        v = *(u32 *)(e + 0xd4);
        if ((v & 2) == 0)
            return;
        if (v == 2)
            s[0x10] = 1;
        else
            s[0x10] = 2;
        break;
    }
}