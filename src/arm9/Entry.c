#include "types.h"

extern u8 data_027c0000[];
extern void main(void);
extern void func_01ff8000(void);
extern void func_02004a14(void);
extern void func_020048e8(u32 a0, u32 a1, u32 a2);
extern void func_020048fc(u32 a0);
extern void func_020049a0(void);
extern void func_0203b4b0(void);
extern void func_02004afc(void);
extern void func_0203b698(void);

asm void Entry(void)
{
    mov ip, #0x4000000
    str ip, [ip, #0x208]
    bl func_02004a14
    mov r0, #0x13
    msr cpsr_c, r0
    ldr r0, =data_027c0000
    add r0, r0, #0x3fc0
    mov sp, r0
    mov r0, #0x12
    msr cpsr_c, r0
    ldr r0, =data_027c0000
    add r0, r0, #0x3fc0
    sub r0, r0, #0x40
    mov sp, r0
    ldr r1, =0x400
    sub r1, r0, r1
    mov r0, #0x1f
    msr cpsr_fsxc, r0
    sub sp, r1, #4
    mov r0, #0
    ldr r1, =data_027c0000
    mov r2, #0x4000
    bl func_020048e8
    mov r0, #0
    ldr r1, =0x05000000
    mov r2, #0x400
    bl func_020048e8
    mov r0, #0x200
    ldr r1, =0x07000000
    mov r2, #0x400
    bl func_020048e8
    ldr r1, =0x02004b00
    ldr r0, [r1, #0x14]
    bl func_020048fc
    bl func_020049a0
    ldr r1, =0x027fff9c
    mov r0, #0
    str r0, [r1]
    ldr r1, =data_027c0000
    add r1, r1, #0x3fc0
    add r1, r1, #0x3c
    ldr r0, =func_01ff8000
    str r0, [r1]
    bl func_0203b4b0
    bl func_02004afc
    bl func_0203b698
    ldr r1, =main
    ldr lr, =0xffff0000
    bx r1
}
