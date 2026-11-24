#include "types.h"

#pragma arm on

/* NitroSDK MI_UncompressLZ16 (mi_uncompress.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026d54(register const void *srcp, register void *destp)
{
                stmfd   sp!, {r4-r10, lr}

                mov     r3, #0                  // r3:  destTmp = 0
                ldr     r8, [r0], #4            // r10: destCount = *(u32 *)srcp >> 8
                mov     r10, r8, lsr #8         // r0:  srcp += 4
                mov     r2, #0                  // r2:  shift = 0

@31:            cmp     r10, #0                 // while (destCount > 0)
                ble     @36

                ldrb    r6, [r0], #1            // r6:  flags = *srcp++
                mov     r7, #8                  // for (i=8; --i>=0; )
@32:            subs    r7, r7, #1
                blt     @31

                tst     r6, #0x80               // if (!(flags & 0x80))
                bne     @33

                ldrb    r9, [r0], #1            //      destTmp |= *srcp++ << shift
                orr     r3, r3, r9, lsl r2
                sub     r10, r10, #1            //      destCount--

                eors    r2, r2, #8              // if (!(shift ^= 8))
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //     *destp++ = destTmp
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //      destTmp = 0
                b       @35

@33:            ldrb    r9, [r0, #0]            // r5:  length = (*srcp >> 4) + 3
                mov     r8, #3
                add     r5, r8, r9, asr #4
                ldrb    r9, [r0], #1            // r4:  offset = (*srcp++ & 0x0f) << 8
                and     r8, r9, #0xf
                mov     r4, r8, lsl #8
                ldrb    r9, [r0], #1            //      offset = (offset | *srcp++) + 1
                orr     r8, r9, r4
                add     r4, r8, #1
                rsb     r8, r2, #8              // r14: offset0_8 = (8 - shift)
                and     r9, r4, #1              //                ^ ((offset & 1) << 3)
                eor     r14, r8, r9, lsl #3
                sub     r10, r10, r5            //      destCount -= length

@34:            eor     r14, r14, #8            //      offset0_8 ^= 8
                rsb     r8, r2, #8              //      destTmp |= (destp[-((offset
                add     r8, r4, r8, lsr #3      //                   + ((8 - shift) >> 3)) >> 1)]
                mov     r8, r8, lsr #1
                mov     r8, r8, lsl #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                ldrh    r9, [r1, -r8]
#else
                dcd     0xe11190b8
#endif
                mov     r8, #0xff
                and     r8, r9, r8, lsl r14
                mov     r8, r8, asr r14
                orr     r3, r3, r8, lsl r2
                eors    r2, r2, #8              // if (!(shift ^= 8))
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //     *destp++ = destTmp
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //      destTmp = 0

                subs    r5, r5, #1              // while (--length > 0)
                bgt     @34

@35:            cmp     r10, #0                 // if (destCount <= 0)   break;
                movgt   r6, r6, lsl #1          // flags <<= 1
                bgt     @32
                b       @31

@36:            ldmfd   sp!, {r4-r10, lr}
                bx      lr
}