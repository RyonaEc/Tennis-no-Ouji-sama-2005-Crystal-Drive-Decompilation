#include "types.h"

#pragma arm on

/* NitroSDK MI_Copy36B (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026a88(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2, r3, r12}         // r0-r3, r12 need not saved
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}
    ldmia   r0!, {r2, r3, r12}
    stmia   r1!, {r2, r3, r12}

    bx      lr
}