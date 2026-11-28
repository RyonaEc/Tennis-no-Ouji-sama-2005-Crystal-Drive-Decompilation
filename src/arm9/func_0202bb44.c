#include "types.h"

extern u8 data_0204acac[];
extern u8 data_0204acb4[];
extern void func_0202b97c(u32 a0);
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern u32 func_020248c8(u32 intr);
extern u32 func_02024948(u32 a0);
extern s32 func_020306a8(void);
extern u32 func_02020348(void);
extern s32 func_0202bda0(s32 *top, s32 *bottom);
extern s32 func_0202be6c(s32 a0, s32 a1);
extern void func_02020300(void);
extern s32 func_0202c104(u32 a0, u32 a1);
extern void func_020263b4(void);
extern void func_020263c0(u32 cycle);
extern s32 func_0202ba54(s32 sw, s32 led, s32 skip, s32 isSync);
extern void func_020202b4(void);
extern s32 func_0202be18(void);

asm void func_0202bb44(void)
{
    stmdb sp!, {r4, r5, r6, r7, r8, sb, r10, r11, lr}
    sub sp, sp, #0x1c
    ldr r3, =data_0204acac
    mov r10, r0
    ldr r0, [r3, #0]
    mov sb, r1
    mov r11, r2
    mov r8, #0
    bl func_0202b97c
    ldr r1, =0x04000208
    mov r0, r8
    ldrh r4, [r1, #0]
    strh r0, [r1, #0]
    bl func_02026350
    str r0, [sp, #0]
    ldr r0, =0x003fffff
    bl func_020248c8
    str r0, [sp, #4]
    mov r0, #0x40000
    bl func_02024948
    ldr r0, [sp, #0]
    bl func_02026364
    ldr r2, =0x04000208
    mov r1, #1
    ldrh r0, [r2, #0]
    ands r0, r10, #8
    strh r1, [r2, #0]
    beq Lskip1
    ldr r0, =0x027ffc40
    ldrh r0, [r0, #0]
    cmp r0, #2
    biceq r10, r10, #8
Lskip1:
    ands r0, r10, #0x10
    beq Lskip2
    bl func_020306a8
    cmp r0, #0
    biceq r10, r10, #0x10
Lskip2:
    bl func_02020348
    str r0, [sp, #8]
    mov r0, #0x4000000
    ldr r7, [r0, #0]
    ldr r2, =0x04001000
    add r0, sp, #0xc
    add r1, sp, #0x10
    ldr r6, [r2, #0]
    bl func_0202bda0
    mov r0, #2
    mov r1, #0
    bl func_0202be6c
    ldr r2, =0x027ffc3c
    ldr r0, [r2, #0]
    str r0, [sp, #0x14]
Lw1:
    ldr r1, [r2, #0]
    ldr r0, [sp, #0x14]
    cmp r0, r1
    beq Lw1
    ldr r0, [r2, #0]
    str r0, [sp, #0x14]
    bl func_02020300
    ldr r1, =0x04001000
    ldr r0, [r1, #0]
    bic r0, r0, #0x10000
    str r0, [r1, #0]
    ldr r2, =0x027ffc3c
Lw2:
    ldr r1, [r2, #0]
    ldr r0, [sp, #0x14]
    cmp r0, r1
    beq Lw2
    ldr r0, [r2, #0]
    str r0, [sp, #0x14]
    ldr r2, =0x027ffc3c
Lw3:
    ldr r1, [r2, #0]
    ldr r0, [sp, #0x14]
    cmp r0, r1
    beq Lw3
    ldr r0, [sp, #0xc]
    ldr r1, [sp, #0x10]
    orr r0, r10, r0, lsl #5
    orr r0, r0, r1, lsl #6
    mov r0, r0, lsl #16
    mov r5, r0, lsr #16
    orr r0, sb, r11
    mov r0, r0, lsl #16
    mov sb, r0, lsr #16
Lsend:
    mov r0, r5
    mov r1, sb
    bl func_0202c104
    cmp r0, #0
    bne Lsend
    bl func_020263b4
    ldr r0, =0x00332340
    bl func_020263c0
    ands r0, r10, #8
    beq Lskip3
    ldr r0, =0x04000214
    ldr r0, [r0, #0]
    ands r0, r0, #0x100000
    movne r8, #1
Lskip3:
    cmp r8, #0
    bne Lskip4
    mov r0, #1
    mov r1, r0
    mov r2, r0
    mov r3, #0
    bl func_0202ba54
    ldr r0, [sp, #8]
    cmp r0, #0
    beq Lskip5
    bl func_020202b4
Lskip5:
    mov r1, #0x4000000
    ldr r0, =0x04001000
    str r7, [r1, #0]
    str r6, [r0, #0]
Lskip4:
    bl func_02026350
    ldr r0, [sp, #4]
    bl func_02024948
    ldr r0, [sp, #0]
    bl func_02026364
    ldr r1, =0x04000208
    cmp r8, #0
    ldrh r0, [r1, #0]
    strh r4, [r1, #0]
    beq Lskip6
    bl func_0202be18
Lskip6:
    ldr r0, =data_0204acb4
    ldr r0, [r0, #0]
    bl func_0202b97c
    add sp, sp, #0x1c
    ldmia sp!, {r4, r5, r6, r7, r8, sb, r10, r11, lr}
    bx lr
}