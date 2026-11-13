#include "types.h"

void func_020260a0(void)
{
    asm { mrc p15, 0, r0, c1, c0, 0; bic r0, r0, #1; mcr p15, 0, r0, c1, c0, 0 };
}
