#include "types.h"

void func_0202633c(void)
{
    asm { mrs r0, cpsr; bic r1, r0, #0x80; msr cpsr_c, r1; and r0, r0, #0x80 };
}
