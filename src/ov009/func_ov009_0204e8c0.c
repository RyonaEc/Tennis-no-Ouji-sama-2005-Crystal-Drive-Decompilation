#include "types.h"

extern u8 *data_ov009_02054ebc;
extern u8 *data_ov009_02054f80;

extern void func_02029c8c(u8 *a0);
extern s32 func_02029930(u8 *a0, u8 *a1);
extern void func_02029828(u8 *a0, u8 *a1, u32 a2);
extern void func_020297bc(u8 *a0, u32 a1, u32 a2);
extern void func_020298e4(u8 *a0);

void func_ov009_0204e8c0(void)
{
    u8 *q;
    u8 *g = data_ov009_02054ebc;
    u8 b[0x48];

    func_02029c8c(b);
    if (func_02029930(b, g) == 0)
        return;
    q = data_ov009_02054f80;
    func_02029828(b, q, *(u32 *)(b + 0x24) - *(u32 *)(b + 0x20));
    func_020297bc(b, 0, 1);
    func_020298e4(b);
}