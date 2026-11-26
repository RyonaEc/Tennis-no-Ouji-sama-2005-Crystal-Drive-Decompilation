#include "types.h"

#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204f5a0(u32 a0, u32 a1, u32 a2);

void func_ov002_0208a3cc(void)
{
    u32 b = data_ov002_02102070[0] + 0x4bc4;
    s32 i;
    u32 t = 0x0500071c;
    u32 p = b + 0x138;

    for (i = 0; i < 4; i++, t += 0x20, p += 0x80)
        func_ov002_0204f5a0(p + *(u8 *)(b + 0x338) * 4, t, 4);

    if (*(u8 *)(b + 0x338) != 0) {
        if (*(u8 *)(b + 0x338) == 0x1f)
            *(s8 *)(b + 0x339) = -1;
    } else {
        *(u8 *)(b + 0x339) = 1;
    }

    *(u8 *)(b + 0x338) += *(s8 *)(b + 0x339);

}

