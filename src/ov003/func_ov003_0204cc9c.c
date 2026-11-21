#include "types.h"

extern u8 *data_ov003_020eb720;

void func_ov003_0204cc9c(u32 a0)
{
    u8 *g = data_ov003_020eb720;
    u16 i;

    for (i = 0; i < 4; i++)
        *(u16 *)(g + i * 2 + 0x29c) = 0;
    if (a0 == 0 || (u16)(a0 + 0xfffe) <= 2) {
        *(u16 *)(g + 0x2bc) = 3;
        *(u16 *)(g + 0x2be) = 0;
        *(u16 *)(g + 0x2c0) = 2;
        *(u16 *)(g + 0x2c2) = 1;
    } else {
        *(u16 *)(g + 0x2bc) = 3;
        *(u16 *)(g + 0x2be) = 2;
        *(u16 *)(g + 0x2c0) = 0;
        *(u16 *)(g + 0x2c2) = 1;
    }
    for (i = 0; i < 4; i++) {
        u16 t = *(u16 *)(g + i * 2 + 0x2bc);

        if (*(u16 *)(g + t * 2 + 0x29c) == 0)
            *(u16 *)(g + t * 2 + 0x29c) = (u16)(i + 1);
    }
}