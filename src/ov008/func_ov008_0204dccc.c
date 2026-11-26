#include "types.h"

extern u32 data_ov008_0204f440[];
extern void func_ov008_0204da58(s32 a0, s32 a1, s32 a2, s32 a3);

void func_ov008_0204dccc(void)
{
    u8 *base = (u8 *)data_ov008_0204f440[0];
    u8 *p = base + 0xf;
    s32 i;

    for (i = 0; i < *(u8 *)(base + 0xb); i++) {
        func_ov008_0204da58(p[0], p[1], (i % 3) * 0x50 + 0x10, (i / 3) * 0x38 + 0x10);
        p += 2;
    }
    if (*(u8 *)(base + 0xa) < 5) {
        u8 *q = base + 0xa;

        *q = *q + 1;
    } else {
        *(u8 *)(base + 0xa) = 0;
    }
}