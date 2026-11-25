#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_ov002_02080eac(u8 a0, s32 a1);
extern u32 func_ov002_0207ff34(u32 a0, s32 a1);
extern u32 func_ov002_02081138(u8 a0, u32 a1);

void func_ov002_0207fe90(void)
{
    u32 base = data_ov002_02102070[0];
    u32 p = base + 0x3654;
    s32 i;

    for (i = 0; i < 2; i++) {
        u32 q = p + i;

        if (*(u8 *)(q + 0x30) == 3 ||
            func_ov002_02080eac(i, 0x640000) == 0) {
            *(u8 *)(q + 0x32) = 0;
        } else {
            *(u8 *)(q + 0x32) += 1;
            if (*(u8 *)(q + 0x32) == 0x20) {
                func_ov002_0207ff34(i, 1);
                func_ov002_02081138(i, -0x640000);
            }
        }
    }
}