#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_02068bf4(void)
{
    u16 *q = (u16 *)(data_ov002_02102070 + 0x2540);
    s32 i;

    for (i = 0; i < 0x40; i++, q += 0x22)
        *q = 0;
}