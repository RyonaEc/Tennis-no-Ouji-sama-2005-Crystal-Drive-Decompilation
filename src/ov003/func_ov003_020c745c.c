#include "types.h"

extern u8 data_ov003_020eb720[];
extern void func_02021b14(u32 *p, u32 a, u32 b, u32 c, u32 d);

s32 func_ov003_020c745c(void)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u16 i;

    for (i = 0; i < 2; i++) {
        if (*(u16 *)(base + 0x8986) != 0)
            *(u16 *)(base + 0x8986) -= 1;
        if (*(u16 *)(base + 0x897e) < 0xe)
            *(u16 *)(base + 0x897e) += 1;
    }
    func_02021b14((u32 *)0x04001050, 8, 9,
                  *(u16 *)(base + 0x897e), *(u16 *)(base + 0x8986));
    for (i = 0; i < 8; i++) {
        if (*(u16 *)(base + 0xadfa) < 0x200)
            *(u16 *)(base + 0xadfa) += 1;
        if (*(u16 *)(base + 0xadfe) != 0)
            *(u16 *)(base + 0xadfe) -= 1;
    }
    return 0;
}