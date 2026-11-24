#include "types.h"

#pragma arm on

/* NitroSDK MI_UncompressLZ8 (mi_uncompress.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_02026cc0(register const void *srcp, register void *destp)
{
                stmfd   sp!, {r4-r6, lr}

                ldr     r5, [r0], #4            // r2:  destCount = *(u32 *)srcp >> 8
                mov     r2, r5, lsr #8          // r0:  srcp += 4

@21:            cmp     r2, #0                  // while (destCount > 0)
                ble     @26

                ldrb    r14, [r0], #1           // r14: flags = *srcp++
                mov     r4, #8                  // for (i=8; --i>=0; )
@22:            subs    r4, r4, #1
                blt     @21

                tst     r14, #0x80              // if (!(flags & 0x80))
                bne     @23

                ldrb    r6, [r0], #1            //      *srcp++
                swpb    r6, r6, [r1]            // r1:  *destp++(For byte write)
                add     r1, r1, #1
                sub     r2, r2, #1              //       destCount--
                b       @25

@23:            ldrb    r5, [r0, #0]            // r3:  length = (*srcp >> 4) + 3
                mov     r6, #3
                add     r3, r6, r5, asr #4
                ldrb    r6, [r0], #1            // r12: offset = (*srcp++ & 0x0f) << 8
                and     r5, r6, #0xf
                mov     r12,r5, lsl #8
                ldrb    r6, [r0], #1            //      offset = (offset | *srcp++) + 1
                orr     r5, r6, r12
                add     r12, r5, #1
                sub     r2, r2, r3              //      destCount -= length

@24:            ldrb    r5, [r1, -r12]          //     *destp++ = destp[-offset]
                swpb    r5, r5, [r1]            //    (For byte write)
                add     r1, r1, #1
                subs    r3, r3, #1              // while (--length > 0)
                bgt     @24

@25:            cmp     r2, #0                  // if (destCount <= 0)   break;
                movgt   r14, r14, lsl #1        //      flags <<= 1
                bgt     @22
                b       @21

@26:            ldmfd   sp!, {r4-r6, lr}
                bx      lr
}