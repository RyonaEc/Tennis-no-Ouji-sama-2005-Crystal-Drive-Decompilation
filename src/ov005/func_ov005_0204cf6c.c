#include "types.h"

extern u8 *data_ov005_020c9740;
extern u32 data_ov005_020c9744;
extern u32 func_02029828(u32, u32, u32);
extern s32 func_020298e4(u8 *);

u32 func_ov005_0204cf6c(void)
{
    u8 *b = (u8 *)data_ov005_020c9740;

    *(u16 *)(b + 0x220) = 0;
    *(u32 *)(b + 0x228) = func_02029828((u32)(b + 0x23c),
        data_ov005_020c9744 + (((*(u32 *)(b + 0x234) << 12) + 0x26000) << 2), 0x4000);
    func_020298e4(b + 0x23c);
    *(u16 *)(b + 0x220) = 0;
    *(u32 *)(b + 0x234) = 0;
    return 1;
}