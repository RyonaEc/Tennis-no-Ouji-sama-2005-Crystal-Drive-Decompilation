#include "types.h"

extern u32 data_ov002_02102070[];
extern void func_ov002_02080114(u32 a0, s32 a1);

void func_ov002_02080ce8(void)
{
    u32 base = data_ov002_02102070[0];
    u32 tbl = base + 0x3654;
    u8 *obj = (u8 *)(base + 0x2e4);
    u16 i = 0;
    u32 k;
    u32 off;
    s32 v;

    do {
        if (*(s8 *)(tbl + i + 9) == 1) {
            k = ((u32)*(u8 *)(obj + 2) >> 1) & 0xff;
            off = k * 4;
            v = *(s32 *)(tbl + k * 4);
            if (v < 0x320000)
                *(s32 *)(tbl + off) = 0x320000;
            func_ov002_02080114(k, 0x320000);
        }
        i++;
        obj += 0x158;
    } while (i < 4);

    for (i = 0; i < 4; i++)
        *(u8 *)(tbl + i + 9) = 0;
}