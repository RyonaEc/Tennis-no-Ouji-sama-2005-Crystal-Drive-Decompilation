#include "types.h"

extern u8 data_02048d2c[];
extern u32 func_02024dc8(u32 a0, u32 a1, u32 a2);
extern u32 func_02024b8c(void);
extern void func_020048e8(u32 data, void *destp, u32 size);
extern u32 func_02024d28(u32 a0, u32 a1, u32 a2);

asm void func_02024e30(void)
{
    stmdb sp!, {lr}
    sub sp, sp, #4
    ldr r0, =data_02048d2c
    ldr r1, [r0, #0]
    cmp r1, #0
    addne sp, sp, #4
    ldmneia sp!, {lr}
    bxne lr
    mov r2, #1
    ldr r1, =0x027ffff0
    str r2, [r0, #0]
    mov r2, #0
    mov r0, #0x7e
    str r2, [r1, #0]
    bl func_02024dc8
    ldr r1, =0x027ffff0
    ldrh r0, [r1, #6]
    cmp r0, #0
    beq Lskip
Lwait:
    bl func_02024b8c
    ldrh r0, [r1, #6]
    cmp r0, #0
    bne Lwait
Lskip:
    ldr r2, =0x027fffb0
    mvn r3, #0
    mov r0, #0x10000
    ldr r1, =0x027fffb4
    str r3, [r2, #0]
    rsb r0, r0, #0
    str r0, [r1, #0]
    mov r0, #0
    str r0, [sp, #0]
    ldr r0, [sp, #0]
    ldr r1, =0x027fffc0
    mov r2, #0x28
    bl func_020048e8
    ldr ip, =0x04000204
    ldr r1, =0x027ffff0
    ldrh r3, [ip, #0]
    mov r0, #0x7e
    mov r2, #0
    orr r3, r3, #0x800
    strh r3, [ip, #0]
    ldrh r3, [ip, #0]
    orr r3, r3, #0x80
    strh r3, [ip, #0]
    bl func_02024d28
    ldr r1, =0x027ffff0
    mov r0, #0x7f
    mov r2, #0
    bl func_02024dc8
    add sp, sp, #4
    ldmia sp!, {lr}
    bx lr
}