#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_02080edc(u32 a0, u32 a1);
extern u32 func_ov002_0207ff34(u32 a0, s32 a1);

void func_ov002_020810b4(s32 a0, s32 a1, s32 a2)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *t = base + 0x3654;
    s32 m = a2;
    u8 g = *(u8 *)(t + a0 + 0x30);

    if (*(u32 *)(t + a0 * 4) == 0x640000) {
        if (g == 3) {
            if (a1 < 0)
                return;
        }
    }
    if (*(u8 *)(base + 0x8c) == 1)
        a1 = a1 / 2;
    func_ov002_02080edc(a0, a1);
    func_ov002_0207ff34(a0, m);
}