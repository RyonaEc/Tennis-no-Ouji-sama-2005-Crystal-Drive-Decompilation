#include "types.h"

extern u8 data_0204ac68[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern s32 func_02027e98(s32 fifotag, u32 data, s32 err);

asm void func_0202b248(void)
{
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r0
    mov r5, r1
    bl func_02026350
    ldr r1, =0x02000300
    mov r4, r0
    orr r1, r6, r1
    mov r0, #6
    mov r2, #0
    bl func_02027e98
    cmp r0, #0
    movlt r0, #0
    blt Ljoin
    ldr r1, =0x01010000
    mov r0, #6
    orr r1, r5, r1
    mov r2, #0
    bl func_02027e98
    cmp r0, #0
    movlt r0, #0
    movge r0, #1
Ljoin:
    cmp r0, #0
    bne Lok
    mov r0, r4
    bl func_02026364
    ldr r0, =data_0204ac68
    ldrh r1, [r0, #0x34]
    ldr r3, [r0, #0]
    orr r1, r1, #8
    strh r1, [r0, #0x34]
    cmp r3, #0
    ldmeqia sp!, {r4, r5, r6, lr}
    bxeq lr
    mov r0, #3
    mov r1, #4
    mov r2, #0
    blx r3
    ldmia sp!, {r4, r5, r6, lr}
    bx lr
Lok:
    ldr r1, =data_0204ac68
    mov r0, r4
    ldrh r2, [r1, #0x36]
    orr r2, r2, #8
    strh r2, [r1, #0x36]
    ldrh r2, [r1, #0x34]
    bic r2, r2, #8
    strh r2, [r1, #0x34]
    bl func_02026364
    ldmia sp!, {r4, r5, r6, lr}
    bx lr
}