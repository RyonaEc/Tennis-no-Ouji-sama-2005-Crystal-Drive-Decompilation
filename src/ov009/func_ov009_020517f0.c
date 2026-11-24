#include "types.h"

extern u8 *data_ov009_02054f84;
extern u32 func_ov009_0205288c(u32 a0, u32 a1);
extern void func_02006584(s32 arg);

u32 func_ov009_020517f0(void)
{
    u8 *p = data_ov009_02054f84;
    u16 *c;

    if (*(u16 *)(p + 0x36e) < 0x10) {
        *(u16 *)(p + 0x36) = 2;
        func_ov009_0205288c(0, 2);
    } else {
        *(u16 *)(p + 0x2fa) = 0;
        *(u16 *)(p + 0x2f8) = 0x65;
        func_02006584(0x20);
        *(u16 *)(p + 0x350) = 0;
    }
    c = (u16 *)(p + 0x36e);
    *c = *c + 1;
    return 1;
}