#include "types.h"

/* Hand-written asm stub pair for the CP15 control register (SCTLR bit 0, the
   MMU/cache enable): two entries one word apart, the first selecting "on" and
   the second "off", both branching into one shared tail. The tail reads c1,
   saves its low bit, then sets or clears it, writes c1 back and returns the
   previous state. Like the CPSR twins func_02033bd0/func_02033c1c this is
   assembled, not compiled: 1.2/b56 reproduces retail byte for byte with an
   `asm void` block, and the neighbour func_02033c88 shows the same module
   using single `asm { mrc ... }` statements. */
asm void func_02033c58(void)
{
    mov r0, #1
    b L_02033c58
    mov r0, #0
    b L_02033c58
L_02033c58:
    mrc p15, 0, r1, c1, c0, 0
    and r2, r1, #1
    tst r0, r0
    biceq r1, r1, #1
    orrne r1, r1, #1
    mcr p15, 0, r1, c1, c0, 0
    mov r0, r2
    bx lr
}
