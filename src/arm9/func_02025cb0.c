#include "types.h"

void func_02025cb0(void)
{
    asm { mov r0, #0; mcr p15, 0, r0, c7, c10, 4 };
}
