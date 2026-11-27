#include "types.h"

extern s16 data_ov003_020dc7b4[];
extern void func_02021d34(u32 *a0, u32 a1, u32 a2, u32 a3);

void func_ov003_02064200(u32 *a0, s32 a1, s32 a2)
{
    if (a2 == 0) {
        func_02021d34(a0, data_ov003_020dc7b4[a1 * 3],
                      data_ov003_020dc7b4[a1 * 3 + 1], 0);
        return;
    }
    if (a2 == 1) {
        func_02021d34(a0, data_ov003_020dc7b4[a1 * 3 + 0x18],
                      data_ov003_020dc7b4[a1 * 3 + 0x19], 0);
        return;
    }
    if (a2 == 2)
        func_02021d34(a0, data_ov003_020dc7b4[a1 * 3 + 0x30],
                      data_ov003_020dc7b4[a1 * 3 + 0x31], 0);
}