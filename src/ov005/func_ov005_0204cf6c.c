#include "types.h"

extern void *func_02029828(u8 *a0, u8 *a1, u32 a2);
extern void func_020298e4(u8 *a0);

s32 func_ov005_0204cf6c(void)
{
    u8 *g = *(u8 **)0x20c9740;
    u8 *b;

    *(u16 *)(g + 0x220) = 0;
    b = *(u8 **)0x20c9744;
    *(u32 *)(g + 0x228) = (u32)func_02029828(g + 0x23c, b + ((*(u32 *)(g + 0x234) << 12) + 0x26000) * 4, 0x4000);
    func_020298e4(g + 0x23c);
    *(u16 *)(g + 0x220) = 0;
    *(u32 *)(g + 0x234) = 0;
    return 1;
}