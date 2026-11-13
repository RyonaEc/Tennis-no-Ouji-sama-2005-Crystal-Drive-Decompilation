#include "types.h"

void func_02025c30(void)
{
    asm {
        mov r1, #0
    L0: mov r0, #0
    L1: orr r2, r1, r0
        mcr p15, 0, r2, c7, c14, 2
        add r0, r0, #0x20
        cmp r0, #0x400
        blt L1
        add r1, r1, #0x40000000
        cmp r1, #0
        bne L0
    };
}
