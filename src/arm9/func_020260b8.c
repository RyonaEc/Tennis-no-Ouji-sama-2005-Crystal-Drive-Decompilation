#include "types.h"

extern u32 data_02048f8c;
extern void func_0202612c(void);

asm void func_020260b8(void)
{
    ldr ip, =data_02048f8c
    ldr ip, [ip]
    cmp ip, #0
    movne lr, pc
    bxne ip
    ldr ip, =0x02000000
    stmdb ip!, {r0, r1, r2, r3, sp, lr}
    and r0, sp, #1
    mov sp, ip
    mrs r1, cpsr
    and r1, r1, #0x1f
    teq r1, #0x17
    bne L10
    bl func_0202612c
    b usr_return
L10:
    teq r1, #0x1b
    bne usr_return
    bl func_0202612c
usr_return:
    ldr ip, =data_02048f8c
    ldr ip, [ip]
    cmp ip, #0
L1:
    beq L1
L2:
    mov r0, r0
    b L2
    ldmfd sp!, {r0, r1, r2, r3, ip, lr}
    mov sp, ip
    bx lr
}