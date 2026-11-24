#include "types.h"

#pragma arm on

/* NitroSDK MIi_CpuCopy32 (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_020269ec(register const void *srcp, register void *destp, register u32 size)
{
        add     r12, r1, r2             // r12: destEndp = destp + size

@30:
        cmp     r1, r12                 // while (destp < destEndp)
        ldmltia r0!, {r2}               // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
        stmltia r1!, {r2}
        blt     @30

        bx      lr
}