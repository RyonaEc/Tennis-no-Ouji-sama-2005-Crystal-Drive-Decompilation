#include "types.h"

#pragma arm on

/* NitroSDK MI_CpuFill8 (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026af4(register void *dstp, register u8 data, register u32 size)
{
    cmp     r2, #0
    bxeq    lr

    // 16-bit alignment of dstp
    tst     r0, #1
    beq     @_1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0, #-1]
#else
    LDRH_AD4( HALFW_CONDAL, 12, 0, 1 ) // *** for CW BUG
#endif
    and     r12, r12, #0x00FF
    orr     r3, r12, r1, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r0, #-1]
#else
    STRH_AD4( HALFW_CONDAL, 3, 0, 1 ) // *** for CW BUG
#endif
    add     r0, r0, #1
    subs    r2, r2, #1
    bxeq    lr
@_1:

    // 32bit alignment.
    cmp     r2, #2
    bcc     @_6
    orr     r1, r1, r1, lsl #8
    tst     r0, #2
    beq     @_8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r1, [r0], #2
#else
    STRH_AD1( HALFW_CONDAL, 1, 0, 2 ) // *** for CW BUG
#endif
    subs    r2, r2, #2
    bxeq    lr
@_8:
    // 32bit transfer.
    orr     r1, r1, r1, lsl #16
    bics    r3, r2, #3
    beq     @_10
    sub     r2, r2, r3
    add     r12, r3, r0
@_9:
    str     r1, [r0], #4
    cmp     r0, r12
    bcc     @_9

@_10:
    //  Last 16-bit transfer.
    tst     r2, #2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strneh  r1, [r0], #2
#else
    STRH_AD1( HALFW_CONDNE, 1, 0, 2 ) // *** for CW BUG
#endif

@_6:
    //  Last 8-bit transfer.
    tst     r2, #1
    bxeq    lr
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0]
#else
    LDRH_AD2( HALFW_CONDAL, 3, 0, 0 ) // *** for CW BUG
#endif
    and     r3, r3, #0xFF00
    and     r1, r1, #0x00FF
    orr     r1, r1, r3
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r1, [r0]
#else
    STRH_AD2( HALFW_CONDAL, 1, 0, 0 ) // *** for CW BUG
#endif
    bx      lr
}