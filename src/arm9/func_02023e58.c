#include "types.h"

extern u8 data_02042370[];
extern void func_020268e0(u32 a0, u32 a1, u32 a2, u32 a3);
extern void func_020269ec(const void *srcp, void *destp, u32 size);

asm void func_02023e58(void)
{
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    ldr r3, =data_02042370
    mvn ip, #0
    ldr lr, [r3, #0]
    mov r5, r0
    mov r4, r1
    mov r3, r2
    cmp lr, ip
    mov ip, #0x6400000
    beq Ldirect
    mov r0, lr
    mov r1, r5
    add r2, ip, r4
    bl func_020268e0
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
Ldirect:
    add r1, ip, r4
    bl func_020269ec
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
}