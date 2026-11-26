#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 data_ov002_020fc774[];
extern void func_020064c0(s32 sel, s32 arg);

void func_ov002_0207c584(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *ep = base + 0x2e4;
    u8 *e = (u8 *)(ep + 0x158 * a0);
    u8 *t = base + 0x3654;
    s32 i = *(u8 *)(e + 0xd1) - 1;
    u32 idx;
    u16 v;

    if (i < 0)
        return;
    if (i > 8)
        return;
    idx = (u32)i;
    if (*(u8 *)e == 9) {
        if (*(u8 *)(t + a0 + 0x54) == 1)
            idx = 4;
    }
    v = *(u16 *)((u8 *)data_ov002_020fc774 + *(u8 *)e * 0x1e + idx * 2);
    if (v == 0)
        return;
    func_020064c0((u32)*(u8 *)(e + 2) >> 1, v);
}