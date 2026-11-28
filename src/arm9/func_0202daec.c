#include "types.h"

extern u8 data_0204aec0[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_0202514c(u8 *a0);
extern u32 func_02025118(u32 a0);

asm void func_0202daec(void)
{
    stmdb sp!, {r4, r5, r6, r7, lr}
    sub sp, sp, #4
    ldr r7, =data_0204aec0
    mov r1, #0
    ldr r6, [r7, #0x28]
    ldr r5, [r7, #0x2c]
    ldr r0, [r7]
    str r1, [r0]
    bl func_02026350
    add r2, r7, #0x34
    ldr r1, [r2]
    mov r4, r0
    bic r0, r1, #0xc
    str r0, [r2]
    add r0, r7, #0xd4
    bl func_0202514c
    ldr r0, [r7, #0x34]
    ands r0, r0, #0x10
    beq Ldone
    add r0, r7, #0x3c
    bl func_02025118
Ldone:
    mov r0, r4
    bl func_02026364
    cmp r6, #0
    addeq sp, sp, #4
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    mov r0, r5
    blx r6
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
}