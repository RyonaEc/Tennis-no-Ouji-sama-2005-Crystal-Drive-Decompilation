#include "types.h"

extern u32 data_ov003_020eb720[];
extern u32 func_02008104(u32 a0, u32 a1, u32 a2, u32 a3);

void func_ov003_02053b58(void)
{
    u8 *st = (u8 *)data_ov003_020eb720[0];
    u16 k;

    func_02008104((u32)st + 0x29c, (u32)st + 0x29e, (u32)st + 0x2a0, (u32)st + 0x2a2);

    *(u16 *)(st + 0x2bc) = 0;
    *(u16 *)(st + 0x2be) = 0;
    *(u16 *)(st + 0x2c0) = 0;
    *(u16 *)(st + 0x2c2) = 0;

    if (*(u16 *)(st + 0x29c)) {
        k = *(u16 *)(st + 0x29c) - 1;
        *(u16 *)(st + k * 2 + 0x2bc) = 0;
    }
    if (*(u16 *)(st + 0x29e)) {
        k = *(u16 *)(st + 0x29e) - 1;
        *(u16 *)(st + k * 2 + 0x2bc) = 1;
    }
    if (*(u16 *)(st + 0x2a0)) {
        k = *(u16 *)(st + 0x2a0) - 1;
        *(u16 *)(st + k * 2 + 0x2bc) = 2;
    }
    if (*(u16 *)(st + 0x2a2)) {
        k = *(u16 *)(st + 0x2a2) - 1;
        *(u16 *)(st + k * 2 + 0x2bc) = 3;
    }
}