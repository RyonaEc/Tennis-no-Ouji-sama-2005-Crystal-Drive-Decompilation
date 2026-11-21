#include "types.h"

extern u32 data_ov002_02102070;

void func_ov002_02088c4c(s32 i)
{
    u8 *b = (u8 *)data_ov002_02102070;
    s32 v;
    u8 *q;

    v = *(s16 *)(b + 0x367e + i * 4);
    q = b + 0x4bc4;
    if (v < 0x1e) {
        *(u32 *)(q + 0x364) = 0x1e0000;
        return;
    }
    if (v < 0x3c) {
        *(u32 *)(q + 0x364) = 0x3c0000;
        return;
    }
    *(u32 *)(q + 0x364) = 0x640000;
}
