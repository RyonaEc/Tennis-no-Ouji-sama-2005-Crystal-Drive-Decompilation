#include "types.h"

asm void func_0203b564(u32 a)
{
    str lr, [sp, #-4]!
    mov r1, r0
    mov r0, #3
    swi 0x123456
    ldr pc, [sp], #4
}