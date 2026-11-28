#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_ov003_0204f0c8(u32 a0);
extern int func_ov003_020dba30(u32 a0);
extern void func_0200e9e0(u32 a0);
extern void func_02006584(s32 arg);

s32 func_ov003_020dacd0(void)
{
    u8 *b = *(u8 **)data_ov003_020eb720;
    u8 *s = b + 0xb8dc;
    u32 h = *(u16 *)(b + 0x8736);

    if (h < 0x10) {
        if (h < 3)
            func_ov003_0204f0c8(*(u16 *)(b + 0x32));
        func_ov003_020dba30(0);
        (*(u16 *)(s + 0x314))++;
        (*(u16 *)(s + 0x318))++;
    } else {
        *(u32 *)0x04001000 = (*(u32 *)0x04001000 & ~0x1f00) | 0x1100;
        *(u16 *)(b + 0x86c2) = 0;
        if (*(u16 *)(b + 0x86ce) == 1) {
            *(u16 *)(b + 0x8718) = 0x1f4;
            *(u16 *)(b + 0x86c0) = 0x16;
            *(u16 *)(b + 0x350) = 3;
            func_0200e9e0(0);
        } else {
            *(u16 *)(b + 0x86c0) = 0x59;
            func_02006584(0x20);
            *(u16 *)(b + 0x8718) = 0;
            func_02006584(0x20);
        }
    }
    (*(u16 *)(b + 0x8736))++;
    return 1;
}