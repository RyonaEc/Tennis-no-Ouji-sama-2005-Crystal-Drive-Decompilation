#include "types.h"

extern u32 data_ov003_020eb720;
extern void func_02023e58(u32, u32, u32);

u32 func_ov003_02050fe4(u32 a0, u32 a1)
{
    u32 base = data_ov003_020eb720;
    u16 i;

    if (a0 == 0)
        return 0;
    for (i = 0; i < 6; i++) {
        u16 k = *(u16 *)(base + 0x2d4 + i * 2);
        u32 sel = (k < 5) ? i : (u16)(i + 4);
        if (i < 4)
            func_02023e58(a0 + sel * 0x180, (a1 << 6) + (i << 8), 0x100);
        else
            func_02023e58(a0, (a1 << 6) + (i << 8), 0x100);
    }
    return 0;
}