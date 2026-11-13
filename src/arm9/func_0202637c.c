#include "types.h"

void func_0202637c(void)
{
    asm { mrs r0, cpsr; orr r1, r0, #0xc0; msr cpsr_c, r1; and r0, r0, #0xc0 };
}
