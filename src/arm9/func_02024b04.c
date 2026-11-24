#include "types.h"

asm s32 func_02024b04(void)
{
    ldr r3, =0x027fffb0
    ldr r1, [r3, #0]
    clz r2, r1
    cmp r2, #32
    movne r0, #0x40
    bne L1
    add r3, r3, #4
    ldr r1, [r3]
    clz r2, r1
    cmp r2, #32
    ldr r0, =0xfffffffd
    bxeq lr
    mov r0, #0x60
L1: add r0, r0, r2
    mov r1, #0x80000000
    mov r1, r1, lsr r2
    ldr r2, [r3]
    bic r2, r2, r1
    str r2, [r3]
    bx lr
}