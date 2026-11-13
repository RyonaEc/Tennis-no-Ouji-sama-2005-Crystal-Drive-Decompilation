#include "types.h"

void func_020263b4(void)
{
    asm { mov r0, #0; mcr p15, 0, r0, c7, c0, 4 };
}
