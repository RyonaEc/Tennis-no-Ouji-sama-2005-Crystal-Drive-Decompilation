#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov009_02054f84[];
extern void func_02006504(u32, u32, void *, u32);

void func_ov009_020515c8(void)
{
    u8 *g = *(u8 **)data_ov009_02054f84;

    if (!(*(u16 *)(data_02042a68 + 0x32) & 1))
        return;
    *(u16 *)(g + 0x306) = 1;
    *(u16 *)(g + 0x30e) = 1;
    *(u16 *)(g + 0x3244) = 6;
    func_02006504(1, 1, g + 0x3200, 6);
}