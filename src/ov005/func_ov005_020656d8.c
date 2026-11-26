#include "types.h"

extern u32 data_ov005_020c9740[];
extern u32 func_020209c8(u32 a0);
extern void func_020208b0(u32 sel);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02026a50(u32 a0, u32 a1, u32 a2);

u32 func_ov005_020656d8(u32 a0)
{
    u32 ip;
    u32 base = data_ov005_020c9740[0];
    u16 i;

    if (a0 == 0) {
        func_020209c8(8);
        func_020208b0(0x100);
        *(u32 *)0x04001000 = (*(u32 *)0x04001000 & 0xffcfffef) | 0x00100010;
        for (i = 0; i < 0x200; i++) {
            *(u16 *)(0x4a58 + base + i * 2) = 0;
            ip = i * 2 + base;
            if (i % 4 == 0)
                *(u32 *)(ip + 0x4a58) = 0x200;
        }
        if (*(u16 *)(0x4388 + base) < 0x1f4) {
            func_02025c94(0x4658 + base, 0x400);
            func_02026a50(0x4658 + base, 0x07000000, 0x400);
        }
    }

    return 0;
}