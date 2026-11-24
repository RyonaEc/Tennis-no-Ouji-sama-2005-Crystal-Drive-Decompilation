#include "types.h"

extern u8 data_ov003_020eb720[];
extern u8 data_02042a68[];

void func_ov003_020c0d20(void)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u16 *q = (u16 *)(base + 0xb8dc);
    u16 f = *(u16 *)(data_02042a68 + 0x32);
    u16 v;
    u16 g;

    if (f & 1) {
        v = q[1];
        if (v == 1)
            q[1] = 3;
        else if (v == 2)
            q[1] = 4;
        *(u16 *)(base + 0xb60c) = 8;
        return;
    }
    if (f & 2) {
        *(u16 *)(base + 0x86d6) = 1;
        *(u16 *)(base + 0x86ce) = 1;
        *(u16 *)(base + 0xb60c) = 8;
        return;
    }
    if (f & 0x40)
        return;
    g = *(u16 *)(data_02042a68 + 0x30);
    if (g & 0x80)
        return;
    if (g & 0x10)
        return;
    if (g & 0x20)
        return;
    if (g & 0x200) {
        v = q[0x14];
        if (v != 0)
            q[0x14] = 0;
        else
            q[0x14] = 1;
        *(u16 *)(base + 0xb60c) = 8;
        return;
    }
    if (!(g & 0x100))
        return;
    v = q[0x14];
    if (v != 0)
        q[0x14] = 0;
    else
        q[0x14] = 1;
    *(u16 *)(base + 0xb60c) = 8;
}