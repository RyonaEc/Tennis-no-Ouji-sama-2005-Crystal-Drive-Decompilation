#include "types.h"

#pragma arm on

/* NitroSDK MIi_CpuClearFast (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026a04(register u32 data, register void *destp, register u32 size)
{
        stmfd   sp!, {r4-r9}

        add     r9, r1, r2              // r9:  destEndp = destp + size
        mov     r12, r2, lsr #5         // r12: destBlockEndp = destp + size/32*32
        add     r12, r1, r12, lsl #5

        mov     r2, r0
        mov     r3, r2
        mov     r4, r2
        mov     r5, r2
        mov     r6, r2
        mov     r7, r2
        mov     r8, r2

@40:
        cmp     r1, r12                 // while (destp < destBlockEndp)
        stmltia r1!, {r0, r2-r8}        // *((vu32 *)(destp++)) = data
        blt     @40
@41:
        cmp     r1, r9                  // while (destp < destEndp)
        stmltia r1!, {r0}               // *((vu32 *)(destp++)) = data
        blt     @41

        ldmfd   sp!, {r4-r9}
        bx      lr
}