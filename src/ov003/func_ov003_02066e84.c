#include "types.h"

extern u8 *data_ov003_020eb720;
extern void func_02021b14(u16 *reg, u32 a, u32 b, u16 c, u16 d);

s32 func_ov003_02066e84(void)
{
    u8 *b = data_ov003_020eb720;
    u32 v = *(u16 *)0x04001048;

    v = (v & ~0x3f) | 0x1f;
    v = v | 0x20;
    *(u16 *)0x04001048 = v;

    func_02021b14((u16 *)0x04001050, 0xe, 0x24,
                  *(u16 *)(b + 0x897e), *(u16 *)(b + 0x8986));

    if (*(u16 *)(b + 0x897e) < 0x10)
        (*(u16 *)(b + 0x897e))++;
    if (*(u16 *)(b + 0x8986) != 0)
        (*(u16 *)(b + 0x8986))--;

    *(u32 *)0x04001000 = (*(u32 *)0x04001000 & ~0x1f00) | 0x1500;
    return 0;
}