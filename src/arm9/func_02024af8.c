#include "types.h"

extern void func_02024c78(void);

asm void func_02024af8(void)
{
    ldr r1, =func_02024c78
    bx  r1
}