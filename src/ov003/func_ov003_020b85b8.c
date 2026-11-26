#include "types.h"

extern u32 data_ov003_020eb720[];
extern u32 func_020209c8(u32 a0);
extern void func_020208b0(u32 sel);
extern void func_ov003_0204e46c(u16 a0);

s32 func_ov003_020b85b8(s32 a0)
{
    u32 ip;
    u32 base = data_ov003_020eb720[0];
    u16 i;

    if (a0 == 0) {
        func_020209c8(8);
        func_020208b0(0x100);
        *(u32 *)0x04001000 = (*(u32 *)0x04001000 & 0xffcfffef) | 0x00100010;
        for (i = 0; i < 0x200; i++) {
            *(u16 *)(0x8de8 + base + i * 2) = 0;
            ip = i * 2 + base;
            if (i % 4 == 0)
                *(u32 *)(ip + 0x8de8) = 0x200;
        }
    } else {
        func_ov003_0204e46c(*(u16 *)(base + 0x86c2));
    }

    return 0;
}