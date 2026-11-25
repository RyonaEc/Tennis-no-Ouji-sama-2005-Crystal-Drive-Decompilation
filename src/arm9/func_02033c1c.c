#include "types.h"

asm void func_02033c1c(register u32 f)
{
    ldr r1, [pc, #0x30]
    bx r1
    mrs r3, cpsr
    mov r1, r3
    and r1, r1, #0x80
    mov r2, #0x80
    tst r0, r0
    orreq r3, r3, r2
    bicne r3, r3, r2
    msr cpsr_fc, r3
    tst r1, r1
    moveq r0, #1
    movne r0, #0
    bx lr
    dcd 0x02033c24
}