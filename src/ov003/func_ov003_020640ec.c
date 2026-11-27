#include "types.h"

extern u32 data_ov003_020dc764[];

u32 func_02021d98(u32 a0, u32 a1, u32 a2);

u32 func_ov003_020640ec(u32 a0, u32 a1, u32 sel) {
    if (sel == 0)
        return func_02021d98(a0, 0x22, data_ov003_020dc764[a1]);
    if (sel == 1)
        return func_02021d98(a0, 0x22, data_ov003_020dc764[a1 + 4]);
    if (sel == 2)
        return func_02021d98(a0, 0x22, data_ov003_020dc764[a1 + 8]);
    if (sel == 3)
        return func_02021d98(a0, 0x22, data_ov003_020dc764[a1 + 12]);
    if (sel == 4)
        return func_02021d98(a0, 0x22, data_ov003_020dc764[a1 + 16]);
}