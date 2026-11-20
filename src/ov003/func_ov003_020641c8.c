#include "types.h"

extern s32 data_ov003_020dc744[];
extern void func_02021d98(u32, u32, u32);

void func_ov003_020641c8(u32 a0, u32 a1)
{
    func_02021d98(a0, 0x21, data_ov003_020dc744[a1]);
}
