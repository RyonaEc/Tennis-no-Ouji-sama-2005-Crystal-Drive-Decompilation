#include "types.h"

extern void func_ov002_0204da54(u32, s32);
void func_ov002_0204db34(u32 a0, s32 a1)
{
    if (a1 < 1) a1 = 1;
    else if (a1 > 0x100) a1 = 0x100;
    func_ov002_0204da54(a0, a1);
}
