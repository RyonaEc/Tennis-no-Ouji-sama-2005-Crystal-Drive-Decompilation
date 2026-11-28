#include "types.h"

extern u8 data_0204b6e8[];
extern u8 data_0204b6ec[];
extern u8 data_0204b6f0[];
extern u8 data_0204b704[];
extern u8 data_0204b780[];
extern u32 func_02027d68(void);
extern s32 func_02027f4c(s32 bit, s32 i);
extern void func_02025c5c(u32 a0, u32 a1);
extern void func_02026934(u32 ch, u32 src, u32 dst, u32 flags);
extern void func_0202de8c(void);
extern u32 func_0202e9c4(u32 a0, u32 a1, u32 a2);
extern u32 func_020259c0(u32 a0, u32 a1, u32 a2);
extern u32 func_02025914(u32 *q, u32 a1, u32 a2);
extern void func_02027f74(u32 tag, u32 *callback);
extern void func_0202dea8(void);

asm void func_0202e4a4(void)
{
    stmdb sp!, {r4, r5, r6, r7, lr}
    sub sp, sp, #4
    ldr r2, =data_0204b6e8
    mov r4, r0
    ldrh r0, [r2, #0]
    mov r5, r1
    cmp r0, #0
    addne sp, sp, #4
    movne r0, #3
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    mov r0, #1
    cmp r4, #0
    strh r0, [r2, #0]
    moveq r0, #0
    streqh r0, [r2, #0]
    addeq sp, sp, #4
    moveq r0, #6
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    cmp r5, #3
    movhi r0, #0
    strhih r0, [r2, #0]
    addhi sp, sp, #4
    movhi r0, #6
    ldmhiia sp!, {r4, r5, r6, r7, lr}
    bxhi lr
    ands r0, r4, #0x1f
    movne r0, #0
    strneh r0, [r2, #0]
    addne sp, sp, #4
    movne r0, #6
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    bl func_02027d68
    mov r0, #0xa
    mov r1, #1
    bl func_02027f4c
    cmp r0, #0
    ldreq r0, =data_0204b6e8
    moveq r1, #0
    streqh r1, [r0, #0]
    addeq sp, sp, #4
    moveq r0, #4
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    mov r0, r4
    mov r1, #0x1700
    bl func_02025c5c
    mov r0, r5
    mov r1, r4
    mov r2, #0
    mov r3, #0x1700
    bl func_02026934
    ldr r0, =data_0204b6ec
    add r1, r4, #0xa00
    str r4, [r0, #0]
    str r1, [r4, #0]
    ldr r2, [r0, #0]
    ldr r1, [r2, #0]
    add r1, r1, #0x100
    str r1, [r2, #4]
    ldr r2, [r0, #0]
    ldr r1, [r2, #4]
    add r1, r1, #0xa00
    str r1, [r2, #0xc]
    ldr r1, [r0, #0]
    ldr r0, [r1, #0xc]
    add r0, r0, #0x100
    str r0, [r1, #0x10]
    bl func_0202de8c
    ldr r1, =data_0204b6ec
    mov r4, #0
    ldr r0, [r1, #0]
    add r0, r0, #0x900
    strh r5, [r0, #0x60]
    ldr r0, [r1, #0]
    str r4, [r0, #0x13c]
    mov r5, r4
Lport:
    mov r0, r4, lsl #16
    mov r1, r5
    mov r2, r5
    mov r0, r0, lsr #16
    bl func_0202e9c4
    add r4, r4, #1
    cmp r4, #0x10
    blt Lport
    ldr r0, =data_0204b6f0
    ldr r1, =data_0204b704
    mov r2, #0xa
    bl func_020259c0
    ldr r6, =data_0204b780
    mov r7, #0
    ldr r5, =data_0204b6f0
    mov r4, #1
Lsend:
    mov r0, r5
    mov r1, r6
    mov r2, r4
    bl func_02025914
    add r7, r7, #1
    cmp r7, #0xa
    add r6, r6, #0x100
    blt Lsend
    ldr r1, =func_0202dea8
    mov r0, #0xa
    bl func_02027f74
    mov r0, #0
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
}