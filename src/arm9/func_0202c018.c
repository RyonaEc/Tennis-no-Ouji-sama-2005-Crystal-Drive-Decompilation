#include "types.h"

extern u8 data_0204acdc[];
extern u8 data_0204acb8[];
extern u8 data_0204ace0[];
extern u32 func_0202c3e0(void);
extern void func_0202bd70(u32 a0);

asm void func_0202c018(void)
{
    stmdb sp!, {r4, r5, r6, r7, r8, lr}
    mov r7, r0
    mov r6, r1
    mov r5, r2
    mov r4, r3
    bl func_0202c3e0
    cmp r0, #0
    moveq r0, #1
    ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
    bxeq lr
    ldr ip, =data_0204acdc
    ldr r1, =data_0204acb8
    ldr r0, =0x03006500
    and r2, r7, #0xff
    mov lr, r7, lsl #3
    mov r8, #0
    ldr r3, =data_0204ace0
    strh r8, [ip, lr]
    orr r0, r2, r0
    str r5, [r1, #4]
    str r4, [r1, #8]
    str r6, [r3, r7, lsl #3]
    bl func_0202bd70
    mov r0, r8
    ldmia sp!, {r4, r5, r6, r7, r8, lr}
    bx lr
}