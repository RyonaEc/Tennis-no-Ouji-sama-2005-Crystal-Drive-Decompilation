#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_02021b14(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

int func_ov003_020dba30(void)
{
    u8 *p = *(u8 **)data_ov003_020eb720;
    u16 i;

    func_02021b14(0x4001050, 0xe, 0x24, *(u16 *)(p + 0x897e),
                  *(u16 *)(p + 0x8986));
    for (i = 0; i < 2; i++) {
        if (*(u16 *)(p + 0x897e) != 0)
            *(u16 *)(p + 0x897e) -= 1;
        if (*(u16 *)(p + 0x8986) < 0x10)
            *(u16 *)(p + 0x8986) += 1;
    }
    return 0;
}