#include "types.h"

extern u32 data_0204ad00;

asm void func_0202c430(void)
{
    ldr ip, =data_0204ad00
loop:
    ldr r0, [ip, #0]
    cmp r0, #1
    beq loop
    bx lr
}