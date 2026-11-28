#include "types.h"

extern u8 data_0204ac68[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);

asm void func_0202b43c(void)
{
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    movs r4, r0
    ldreq r0, =data_0204ac68
    moveq r1, #0
    streqh r1, [r0, #0x30]
    addeq sp, sp, #4
    ldmeqia sp!, {r4, r5, lr}
    bxeq lr
    bl func_02026350
    ldrsh lr, [r4, #4]
    ldr r1, =0x04000280
    mov ip, #0
    ldr r3, =0x04000290
    strh ip, [r1, #0]
    mov r5, #0x10000000
    ldr r2, =0x04000298
    str r5, [r3, #0]
    str lr, [r2, #0]
    str ip, [r2, #4]
    ldrsh r3, [r4, #0]
    ldr r2, =data_0204ac68
    str r3, [r2, #0x18]
    ldrsh r3, [r4, #4]
    str r3, [r2, #0x1c]
Lwait1:
    ldrh r2, [r1, #0]
    ands r2, r2, #0x8000
    bne Lwait1
    ldr r3, =0x040002a0
    ldr r2, =data_0204ac68
    ldr r3, [r3, #0]
    mov lr, #0
    str r3, [r2, #0x20]
    ldrsh r5, [r4, #6]
    ldr r3, =0x04000290
    mov ip, #0x10000000
    strh lr, [r1, #0]
    ldr r1, =0x04000298
    str ip, [r3, #0]
    str r5, [r1, #0]
    str lr, [r1, #4]
    ldrsh r1, [r4, #2]
    str r1, [r2, #0x24]
    ldrsh r1, [r4, #6]
    str r1, [r2, #0x28]
    ldr r2, =0x04000280
Lwait2:
    ldrh r1, [r2, #0]
    ands r1, r1, #0x8000
    bne Lwait2
    ldr r2, =0x040002a0
    ldr r1, =data_0204ac68
    ldr r2, [r2, #0]
    str r2, [r1, #0x2c]
    bl func_02026364
    ldr r0, =data_0204ac68
    mov r1, #1
    strh r1, [r0, #0x30]
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
}