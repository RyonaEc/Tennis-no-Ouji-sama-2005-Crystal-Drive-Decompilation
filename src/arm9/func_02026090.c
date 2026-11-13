#include "types.h"

void func_02026090(u32 a0)
{
    asm { mrc p15, 0, r0, c1, c0, 0; orr r0, r0, #1; mcr p15, 0, r0, c1, c0, 0 };
}
