#include "types.h"

#pragma arm on

/* NitroSDK GX_SendFifo48B (gxasm.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_0202474c(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2,r3,r12}           // r0-r3, r12 need not be saved.
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}

    bx      lr
}