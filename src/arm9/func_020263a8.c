#include "types.h"

void func_020263a8(void)
{
    asm { mrs r0, cpsr; and r0, r0, #0x1f };
}
