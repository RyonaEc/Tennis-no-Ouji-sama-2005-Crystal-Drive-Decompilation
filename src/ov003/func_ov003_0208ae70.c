#include "types.h"

extern u32 data_ov003_020eb720[];
extern void func_02021b14(u32 *p, u32 a, u32 b, u32 c, u32 d);

u32 func_ov003_0208ae70(void)
{
    u32 base = data_ov003_020eb720[0];

    if (*(u16 *)(base + 0x86ce) == 2) {
        func_02021b14((u32 *)0x04001050, 0xe, 0x24,
                      *(u16 *)(base + 0x897e), *(u16 *)(base + 0x8986));
        if (*(u16 *)(base + 0x897e) != 0)
            *(u16 *)(base + 0x897e) -= 1;
        if (*(u16 *)(base + 0x8986) < 0x10)
            *(u16 *)(base + 0x8986) += 1;
    }
    return 0;
}