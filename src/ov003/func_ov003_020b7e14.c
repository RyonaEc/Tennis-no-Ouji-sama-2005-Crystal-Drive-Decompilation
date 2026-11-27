#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_ov003_0204f0c8(u32 a0);
extern u32 func_ov003_020b88fc(u32 a0);
extern void func_02006584(s32 arg);

u32 func_ov003_020b7e14(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;

    if (*(u16 *)(p + 0x8736) < 8) {
        func_ov003_0204f0c8(*(u16 *)(p + 0x32));
        if (((u16 *)(p + 0x8800))[4] < 0x20) {
            u16 *t = (u16 *)(p + 0x8808);
            *t = *t + 1;
        }
        func_ov003_020b88fc(0);
    } else {
        *(u16 *)(p + 0x86c2) = 0;
        if (*(u16 *)(p + 0x86ce) == 1) {
            *(u16 *)(p + 0x86c0) = 0x59;
        } else {
            *(u16 *)(p + 0x86c0) = 0x16;
            *(u16 *)(p + 0x86c0) = 0x59;
        }
        func_02006584(0x20);
        *(u16 *)(p + 0x8718) = 0;
    }
    {
        u16 *v = (u16 *)(p + 0x8736);
        *v = *v + 1;
    }
    return 1;
}