#include "types.h"

/* OS_SaveContext from the NitroSDK's os_context.c: save the CP context first,
   then CPSR, SP_svc (through a mode switch) and r0-r14, and set PC_plus4 to the
   instruction after the caller's return. Offsets are restated numerically:
   CP_CONTEXT +0x48, CPSR +0, PC_PLUS4 +0x40, SP_SVC +0x44 (reached as r1 = ctx+4
   with +0x40 / +0x3c). */

extern void func_0202ae08(u32 *cp_context);

asm u32 func_0202564c(u32 *context)
{
    stmfd  sp!, {r0, lr}
    add    r0, r0, #0x48
    ldr    r1, =func_0202ae08
    blx    r1
    ldmfd  sp!, {r0, lr}
    add    r1, r0, #0
    mrs    r2, cpsr
    str    r2, [r1], #4
    mov    r0, #0xd3
    msr    cpsr_c, r0
    str    sp, [r1, #0x40]
    msr    cpsr_c, r2
    mov    r0, #1
    stmia  r1, {r0-r14}
    add    r0, pc, #8
    str    r0, [r1, #0x3c]
    mov    r0, #0
    bx     lr
}