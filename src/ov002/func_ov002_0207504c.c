#include "types.h"

extern u8 *func_ov002_02076258(u32, u32, u32, u32, u32);
void func_ov002_0207504c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u16 *p = (u16 *)func_ov002_02076258(a0, a1, a2, a3, a4);
    if (p != 0) *p |= 0x200;
}
