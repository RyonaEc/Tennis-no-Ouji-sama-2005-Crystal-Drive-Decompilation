#include "types.h"

extern u32 data_ov005_020c9744[];
extern u8 data_ov005_020c9604[];
extern u8 data_ov005_020c95e8[];

extern u32 func_02029c8c(void *a0);
extern s32 func_02029930(void *a0, u32 a1);
extern u32 func_02029828(void *a0, u32 a1, u32 a2);
extern u32 func_020297bc(void *a0, u32 a1, u32 a2);
extern s32 func_020298e4(void *a0);

void func_ov005_0204f504(u32 mode)
{
    u8 t[0x44];
    s32 ok;

    func_02029c8c(t);
    if (mode == 0)
        ok = func_02029930(t, (u32)data_ov005_020c9604);
    if (mode == 1)
        ok = func_02029930(t, (u32)data_ov005_020c95e8);
    if (ok == 0)
        return;
    func_02029828(t, (u32)((u8 *)data_ov005_020c9744[0] + 0xc0000),
                 *(u32 *)(t + 0x24) - *(u32 *)(t + 0x20));
    func_020297bc(t, 0, 1);
    func_020298e4(t);
}