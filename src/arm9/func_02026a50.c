#include "types.h"

#pragma arm on

/* NitroSDK MIi_CpuCopyFast (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026a50(register const void *srcp, register void *destp, register u32 size)
{
        stmfd   sp!, {r4-r10}

        add     r10, r1, r2             // r10:  destEndp = destp + size
        mov     r12, r2, lsr #5         // r12: destBlockEndp = destp + size/32*32
        add     r12, r1, r12, lsl #5

@50:
        cmp     r1, r12                 // while (destp < destBlockEndp)
        ldmltia r0!, {r2-r9}            // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
        stmltia r1!, {r2-r9}
        blt     @50
@51:
        cmp     r1, r10                 // while (destp < destEndp)
        ldmltia r0!, {r2}               // *((vu32 *)(destp)++) = *((vu32 *)(srcp)++)
        stmltia r1!, {r2}
        blt     @51

        ldmfd   sp!, {r4-r10}
        bx      lr
}