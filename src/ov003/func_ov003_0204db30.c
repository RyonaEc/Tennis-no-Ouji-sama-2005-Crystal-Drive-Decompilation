#include "types.h"

extern u8 *data_ov003_020eb720;
extern u32 data_ov003_020eb724;
extern u32 func_02029828(u32, u32, u32);
extern s32 func_020298e4(u8 *);

u32 func_ov003_0204db30(void)
{
    u8 *b = (u8 *)data_ov003_020eb720;

    *(u16 *)(b + 0x360) = 0;
    *(u32 *)(b + 0x368) = func_02029828((u32)(b + 0x37c),
        data_ov003_020eb724 + (((*(u32 *)(b + 0x374) << 12) + 0x32000) << 2), 0x4000);
    func_020298e4(b + 0x37c);
    *(u16 *)(b + 0x360) = 0;
    *(u32 *)(b + 0x374) = 0;
    return 1;
}