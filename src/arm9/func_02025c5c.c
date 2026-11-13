#include "types.h"

void func_02025c5c(u32 a0, u32 a1)
{
    asm {
        add r1, r1, r0
        bic r0, r0, #0x1f
    L:  mcr p15, 0, r0, c7, c6, 1
        add r0, r0, #0x20
        cmp r0, r1
        blt L
    };
}
