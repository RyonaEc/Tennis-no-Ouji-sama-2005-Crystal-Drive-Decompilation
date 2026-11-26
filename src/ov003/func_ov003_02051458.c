#include "types.h"

extern u32 data_ov003_020eb720[];
extern u32 func_02023e58(u32, u32, u32);

u32 func_ov003_02051458(u8 *a0)
{
    u8 *base = (u8 *)data_ov003_020eb720[0];
    u16 i;
    u32 off = 0;

    if (a0 == 0)
        return 0;
    for (i = 0; i < 4; i++) {
        if (*(u16 *)(base + 0x26e) == 0) {
            if (*(u16 *)(base + i * 2 + 0x2d4) < 5)
                off = 0;
            else
                off = 4;
        }
        if (i < 4)
            func_02023e58((u32)(a0 + (i + off) * 0x40), i * 0x40, 0x40);
        else
            func_02023e58((u32)a0, i * 0x40, 0x40);
    }
    return 0;
}