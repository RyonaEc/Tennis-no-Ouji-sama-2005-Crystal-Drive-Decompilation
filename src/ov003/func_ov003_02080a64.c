#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_02021b14(u32 *p, u32 a, u32 b, u32 c, u32 d);

u32 func_ov003_02080a64(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;

    if (*(u16 *)(p + 0x86ce) == 2) {
        if (*(u16 *)(p + 0x8986) < 0x10) {
            u16 *t = (u16 *)(p + 0x8986);
            *t = *t + 1;
        }
        if (*(u16 *)(p + 0x897e) != 0) {
            u16 *u = (u16 *)(p + 0x897e);
            *u = *u - 1;
        }
        func_02021b14((u32 *)0x04001050, 0xe, 0x24, *(u16 *)(p + 0x897e),
                      *(u16 *)(p + 0x8986));
    } else {
        func_02021b14((u32 *)0x04001050, 0, 0, *(u16 *)(p + 0x897e),
                      *(u16 *)(p + 0x8986));
    }
    return 0;
}