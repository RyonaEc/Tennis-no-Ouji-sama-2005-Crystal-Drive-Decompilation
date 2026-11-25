#include "types.h"

extern u32 data_ov003_020eb720[];
extern u8 data_ov003_020dbb98[];
extern u8 data_ov003_020dbf78[];

extern void func_02025c94(u32 a0, u32 a1);
extern void func_02023f7c(s32 a0, s32 a1, s32 a2);
extern u32 func_02023df8(u32 a0, u32 a1, u32 a2);

u32 func_ov003_0204e3c8(u32 a0)
{
    u32 base = data_ov003_020eb720[0];
    u32 v;
    u32 idx;
    u32 p;

    if (a0 != 0) {
    func_02025c94((u32)data_ov003_020dbb98, 0x20);
        func_02023f7c((s32)data_ov003_020dbb98, 0x1e0, 0x20);
        v = *(u16 *)(base + 0x200 + 0x5a);
        if (v == 4)
            idx = 0;
        else if (v == 3)
            idx = 1;
        else if (v == 2)
            idx = 2;
        else
            idx = 3;
        p = (u32)data_ov003_020dbf78 + idx * 0x80;
        func_02025c94(p, 0x100);
        func_02023df8(p, 0xfa00, 0x80);
        return 1;
    }
    return 0;
}