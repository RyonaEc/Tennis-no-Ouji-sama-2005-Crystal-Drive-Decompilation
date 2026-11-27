#include "types.h"

extern u8 data_ov003_020eb720[];
extern u32 func_ov003_020b7484(u32 a0);

u32 func_ov003_020b6d94(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;
    u8 *q = p + 0xc8dc;

    if (*(u16 *)(p + 0xb636) < 0x20) {
        if (*(u16 *)(p + 0x8808) < 0x20) {
            u16 *t = (u16 *)(p + 0x8808);
            *t = *t + 1;
        }
        if (*(s16 *)(q + 0x24) < 0x20) {
            s16 *u = (s16 *)(q + 0x24);
            *u = *u + 1;
        }
        func_ov003_020b7484(0);
    } else {
        *(u16 *)(p + 0x86c2) = 0;
        if (*(u16 *)(p + 0x86ce) == 1)
            *(u16 *)(p + 0x86c0) = 0x13;
        else
            *(u16 *)(p + 0x86c0) = 0x16;
        *(u16 *)(p + 0x8718) = 0x1f4;
    }
    {
        u16 *v = (u16 *)(p + 0xb636);
        *v = *v + 1;
    }
    return 1;
}