#include "types.h"

#pragma arm on

/* NitroSDK GXi_NopClearFifo128_ (g3x.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02022368(register void* pDest)
{
    mov     r1,  #0         // r0-r3, r12 need not be saved.
    mov     r2,  #0
    mov     r3,  #0
    mov     r12, #0
    
    // set 128 NOP command for 4 PACKED Shineness command. (32 * 4 NOP)
    
    stmia   r0, {r1-r3, r12}    // 0 - 15
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 16 - 31
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 32 - 47
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 48 - 63
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 64 - 79
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 80 - 95
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 96 - 111
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 112 - 127
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    
    bx      lr
}