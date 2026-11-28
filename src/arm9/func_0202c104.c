#include "types.h"

extern u8 data_0204aca4[];
extern u8 data_0204aca8[];
extern u32 func_0202c3e0(void);
extern void func_0202bd70(u32 a0);
extern s32 func_0202ba54(s32 sw, s32 led, s32 skip, s32 isSync);

asm void func_0202c104(void)
{
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    mov r5, r0
    mov r4, r1
    bl func_0202c3e0
    cmp r0, #0
    addeq sp, sp, #4
    moveq r0, #1
    ldmeqia sp!, {r4, r5, lr}
    bxeq lr
    ldr r1, =data_0204aca4
    mov r2, #0
    ldr r0, =0x03006000
    str r2, [r1, #0]
    bl func_0202bd70
    ldr r1, =data_0204aca4
Lwait:
    ldr r0, [r1, #0]
    cmp r0, #0
    beq Lwait
    mov r0, #0
    ldr ip, =data_0204aca8
    str r0, [r1, #0]
    mov r2, r0
    mov r3, r0
    mov r1, #2
    str r0, [ip, #0]
    bl func_0202ba54
    ldr r0, =0x02006100
    and r1, r5, #0xff
    orr r0, r1, r0
    bl func_0202bd70
    ldr r0, =0x0000ffff
    ldr r1, =0x01010000
    and r0, r4, r0
    orr r0, r0, r1
    bl func_0202bd70
    mov r0, #0
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
}