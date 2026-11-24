#include "types.h"

#pragma arm on

/* NitroSDK MI_CpuCopy8 (mi_memory.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026b88(register const void *srcp, register void *dstp, register u32 size)
{
    cmp     r2, #0
    bxeq    lr

    // 16-bit alignment of dstp.
    tst     r1, #1
    beq     @_1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r1, #-1]
#else
    LDRH_AD4( HALFW_CONDAL, 12, 1, 1 ) // *** for CW BUG
#endif
    and     r12, r12, #0x00FF
    tst     r0, #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrneh  r3, [r0, #-1]
#else
    LDRH_AD4( HALFW_CONDNE, 3, 0, 1 ) // *** for CW BUG
#endif
    movne   r3, r3, lsr #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldreqh  r3, [r0]
#else
    LDRH_AD2( HALFW_CONDEQ, 3, 0, 0 ) // *** for CW BUG
#endif
    orr     r3, r12, r3, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1, #-1]
#else
    STRH_AD4( HALFW_CONDAL, 3, 1, 1 ) // *** for CW BUG
#endif
    add     r0, r0, #1
    add     r1, r1, #1
    subs    r2, r2, #1
    bxeq    lr
@_1:

    // Check address fraction 16/32 bit synchronicity.
    eor     r12, r1, r0
    tst     r12, #1
    beq     @_2

    // Totally unsynchronized, so use irregular 16-bit transfer.
    //  tmp = *(u16*)src++ >> 8;
    //  while((size -= 2) >= 0) {
    //      tmp |= (*(u16*)src++ << 8);
    //      *(u16*)dst++ = (u16)tmp;
    //      tmp >>= 16;
    //  }
    bic     r0, r0, #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0], #2
#else
        LDRH_AD1( HALFW_CONDAL, 12, 0, 2 ) // *** for CW BUG
#endif
    mov     r3, r12, lsr #8
    subs    r2, r2, #2
    bcc     @_3
@_4:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 12, 0, 2 ) // *** for CW BUG
#endif
    orr     r12, r3, r12, lsl #8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r12, [r1], #2
#else
    STRH_AD1( HALFW_CONDAL, 12, 1, 2 ) // *** for CW BUG
#endif
    mov     r3, r12, lsr #16
    subs    r2, r2, #2
    bcs     @_4
    
@_3:
    //  if(size & 1)
    //      *dst = (u16)((*dst & 0xFF00) | tmp);
    //  return;
    tst     r2, #1
    bxeq    lr
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r12, [r1]
#else
    LDRH_AD2( HALFW_CONDAL, 12, 1, 0 ) // *** for CW BUG
#endif
    and     r12, r12, #0xFF00
    orr     r12, r12, r3
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r12, [r1]
#else
    STRH_AD2( HALFW_CONDAL, 12, 1, 0 ) // *** for CW BUG
#endif
    bx      lr

@_2:
    tst     r12, #2
    beq     @_5
    // 16bit transfer.
    bics    r3, r2, #1
    beq     @_6
    sub     r2, r2, r3
    add     r12, r3, r1
@_7:
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 3, 0, 2 ) // *** for CW BUG
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1], #2
#else
    STRH_AD1( HALFW_CONDAL, 3, 1, 2 ) // *** for CW BUG
#endif
    cmp     r1, r12
    bcc     @_7
    b       @_6

@_5:
    // 32bit alignment.
    cmp     r2, #2
    bcc     @_6
    tst     r1, #2
    beq     @_8
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r3, [r0], #2
#else
    LDRH_AD1( HALFW_CONDAL, 3, 0, 2 ) // *** for CW BUG
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r3, [r1], #2
#else
        STRH_AD1( HALFW_CONDAL, 3, 1, 2 ) // *** for CW BUG
#endif
    subs    r2, r2, #2
    bxeq    lr
@_8:
    // 32bit transfer.
    bics    r3, r2, #3
    beq     @_10
    sub     r2, r2, r3
    add     r12, r3, r1
@_9:
    ldr     r3, [r0], #4
    str     r3, [r1], #4
    cmp     r1, r12
    bcc     @_9

@_10:
    //  Last 16bit transfer.
    tst     r2, #2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrneh  r3, [r0], #2
    strneh  r3, [r1], #2
#else
    LDRH_AD1( HALFW_CONDNE, 3, 0, 2 ) // *** for CW BUG
    STRH_AD1( HALFW_CONDNE, 3, 1, 2 ) // *** for CW BUG
#endif

@_6:
    //  Last 8bit transfer.
    tst     r2, #1
    bxeq    lr
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    ldrh    r2, [r1]
    ldrh    r0, [r0]
#else
    LDRH_AD2( HALFW_CONDAL, 2, 1, 0 ) // *** for CW BUG
    LDRH_AD2( HALFW_CONDAL, 0, 0, 0 ) // *** for CW BUG
#endif
    and     r2, r2, #0xFF00
    and     r0, r0, #0x00FF
    orr     r0, r2, r0
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    strh    r0, [r1]
#else
    STRH_AD2( HALFW_CONDAL, 0, 1, 0 ) // *** for CW BUG
#endif
    bx      lr
}