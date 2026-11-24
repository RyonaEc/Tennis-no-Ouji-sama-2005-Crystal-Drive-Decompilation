#include "types.h"

extern u8 data_02048f84[];
extern u8 data_02048f88[];
extern u8 data_02048f90[];
extern void func_02026090(void);
extern void func_020260a0(void);

asm void func_020261d0(void)
{
    stmdb  sp!, {lr}
    sub    sp, sp, #4
    ldr    r0, =data_02048f84
    ldr    r0, [r0]
    cmp    r0, #0
    addeq  sp, sp, #4
    ldmeqia sp!, {lr}
    bxeq   lr
    mov    r0, sp
    ldr    r1, =0x9f
    msr    cpsr_fsxc, r1
    mov    sp, r0
    bl     func_02026090
    ldr    r1, =data_02048f88
    ldr    r0, =data_02048f84
    ldr    r1, [r1]
    ldr    r2, [r0]
    ldr    r0, =data_02048f90
    blx    r2
    bl     func_020260a0
    add    sp, sp, #4
    ldmia  sp!, {lr}
    bx     lr
}