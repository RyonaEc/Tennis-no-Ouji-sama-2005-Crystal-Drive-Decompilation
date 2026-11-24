#include "types.h"

#pragma arm on

/* NitroSDK MIi_CpuClear32 (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_020048e8(register u32 data, register void *destp, register u32 size)
{
        add     r12, r1, r2             // r12: destEndp = destp + size

@20:
        cmp     r1, r12                 // while (destp < destEndp)
        stmltia r1!, {r0}               // *((vu32 *)(destp++)) = data
        blt     @20
        bx      lr
}