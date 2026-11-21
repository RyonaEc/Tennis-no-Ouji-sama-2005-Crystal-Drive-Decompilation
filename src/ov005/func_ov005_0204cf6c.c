#include "types.h"

extern u8 *data_ov005_020c9740;
extern u8 *data_ov005_020c9744;

extern void *func_02029828(u8 *a0, u8 *a1, u32 a2);
extern void func_020298e4(u8 *a0);

s32 func_ov005_0204cf6c(void)
{
    u8 *g = data_ov005_020c9740;
    u8 *b;

    *(u16 *)(g + 0x220) = 0;
    b = data_ov005_020c9744;
    *(u32 *)(g + 0x228) = (u32)func_02029828(g + 0x23c, b + ((*(u32 *)(g + 0x234) << 12) + 0x26000) * 4, 0x4000);
    func_020298e4(g + 0x23c);
    *(u16 *)(g + 0x220) = 0;
    *(u32 *)(g + 0x234) = 0;
    return 1;
}