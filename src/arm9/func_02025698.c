#include "types.h"

extern void func_0202ad9c(void);

asm void func_02025698(u32 a)
{
    stmdb sp!, {r0, lr}
    add   r0, r0, #0x48
    ldr   r1, =func_0202ad9c
    blx   r1
    ldmia sp!, {r0, lr}
    mrs   r1, cpsr
    bic   r1, r1, #0x1f
    orr   r1, r1, #0xd3
    msr   cpsr_c, r1
    ldr   r1, [r0], #4
    msr   spsr_fsxc, r1
    ldr   sp, [r0, #0x40]
    ldr   lr, [r0, #0x3c]
    ldmia r0, {r0-r14}^
    mov   r0, r0
    subs  pc, lr, #4
}