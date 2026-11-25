#include "types.h"

extern u32 data_ov003_020eb724[];
extern u8 data_ov003_020eb010[];

extern u32 func_02029c8c(void *a0);
extern s32 func_02029930(void *a0, u32 a1);
extern u32 func_02029828(void *a0, void *a1, u32 a2);
extern u32 func_020297bc(void *a0, u32 a1, u32 a2);
extern s32 func_020298e4(void *a0);
extern void func_02026d54(const void *srcp, void *destp);

void func_ov003_02057c7c(void)
{
    u8 *g = (u8 *)data_ov003_020eb724[0];
    u8 *src = g + 0x100000;
    u8 *dst = g + 0x120000;
    u8 t[0x44];

    func_02029c8c(t);
    if (func_02029930(t, (u32)data_ov003_020eb010) != 0) {
        func_02029828(t, (u8 *)data_ov003_020eb724[0] + 0x100000,
                      *(u32 *)(t + 0x24) - *(u32 *)(t + 0x20));
        func_020297bc(t, 0, 1);
        func_020298e4(t);
    }
    func_02026d54(src, dst);
}