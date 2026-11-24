#include "types.h"

asm void func_0203b578(u32 a)
{
    str lr, [sp, #-4]!
    mov r1, #0
    mov r0, #7
    swi 0x123456
    ldr pc, [sp], #4
}