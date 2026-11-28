#include "types.h"

extern u8 data_0204b6f0[];
extern u8 data_0204b6ec[];
extern s32 func_02025864(u32 *mq, void **msg, s32 flags);
extern void func_02025c78(u32 a0, u32 a1);
extern void func_02025c5c(u32 a0, u32 a1);
extern void func_020268e0(u32 a0, u32 a1, u32 a2, u32 a3);
extern s32 func_02027e98(s32 fifotag, u32 data, s32 err);
extern u32 func_02025914(u32 *q, u32 a1, u32 a2);

asm void func_0202e2fc(void)
{
    stmdb sp!, {r0, r1, r2, r3}
    stmdb sp!, {r4, lr}
    sub sp, sp, #8
    mov r4, r0
    ldr r0, =data_0204b6f0
    add r1, sp, #0
    mov r2, #0
    bl func_02025864
    cmp r0, #0
    addeq sp, sp, #8
    moveq r0, #8
    ldmeqia sp!, {r4, lr}
    addeq sp, sp, #0x10
    bxeq lr
    ldrh r0, [sp, #0x14]
    cmp r0, #0
    bne Lskip
    ldr r0, =data_0204b6ec
    mov r1, #0x100
    ldr r0, [r0, #0]
    ldr r0, [r0, #0xc]
    bl func_02025c78
    ldr r0, [sp, #0]
    mov r1, #0x100
    bl func_02025c5c
    ldr r0, =data_0204b6ec
    ldr r2, [sp, #0]
    ldr r1, [r0, #0]
    mov r3, #0x100
    add r0, r1, #0x900
    ldrh r0, [r0, #0x60]
    ldr r1, [r1, #0xc]
    bl func_020268e0
Lskip:
    ldr r1, [sp, #0]
    add r0, sp, #0x14
    strh r4, [r1, #0]
    ldrh r2, [sp, #0x14]
    bic r0, r0, #3
    add r4, r0, #4
    cmp r2, #0
    mov r3, #0
    ble Ldone
Lloop:
    add r4, r4, #4
    ldr r0, [sp, #0]
    ldr r1, [r4, #-4]
    add r0, r0, r3, lsl #2
    add r3, r3, #1
    str r1, [r0, #4]
    cmp r3, r2
    blt Lloop
Ldone:
    ldr r0, [sp, #0]
    mov r1, #0x100
    bl func_02025c78
    ldr r1, [sp, #0]
    mov r0, #0xa
    mov r2, #0
    bl func_02027e98
    mov r4, r0
    ldr r1, [sp, #0]
    ldr r0, =data_0204b6f0
    mov r2, #1
    bl func_02025914
    cmp r4, #0
    movlt r0, #8
    movge r0, #2
    add sp, sp, #8
    ldmia sp!, {r4, lr}
    add sp, sp, #0x10
    bx lr
}