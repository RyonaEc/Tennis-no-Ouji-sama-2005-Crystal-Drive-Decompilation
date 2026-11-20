#include "types.h"

extern s32 data_ov003_020dc744[];
extern void func_02021d98(u32, u32, u32);

void func_ov003_020641e4(u32 a0)
{
    *(u32 *)0x4000484 = data_ov003_020dc744[a0];
}
