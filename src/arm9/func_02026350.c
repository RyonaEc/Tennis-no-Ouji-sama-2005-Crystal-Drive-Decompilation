#include "types.h"

void func_02026350(void)
{
    asm { mrs r0, cpsr; orr r1, r0, #0x80; msr cpsr_c, r1; and r0, r0, #0x80 };
}
