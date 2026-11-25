#include "types.h"

/* NitroSDK crt0.c MIi_UncompressBackward: the decompressor for compressed
   modules in the autoload block. The SDK writes it as a hand-written arm asm
   block (ARM9 crt0.c), so no mwccarm build reproduces it from C. Body below is
   retail byte for byte; 1.2/b56 assembles it without a per-file override. */
asm void func_020048fc(register void *bottom)
{
#define data            r0
#define inp_top         r1
#define outp            r2
#define inp             r3
#define outp_save       r4
#define flag            r5
#define count8          r6
#define index           r7
#define len             r12
                cmp     bottom, #0
                beq     @exit
                stmfd   sp!,    {r4-r7}
                ldmdb   bottom, {r1-r2}
                add     outp,    bottom,  outp
                sub     inp,     bottom,  inp_top, LSR #24
                bic     inp_top, inp_top, #0xff000000
                sub     inp_top, bottom,  inp_top
                mov     outp_save, outp
@loop:
                cmp     inp, inp_top
                ble     @end_loop
                ldrb    flag, [inp, #-1]!
                mov     count8, #8
@loop8:
                subs    count8, count8, #1
                blt     @loop
                tst     flag, #0x80
                bne     @blockcopy
@bytecopy:
                ldrb    data, [inp, #-1]!
                strb    data, [outp, #-1]!
                b       @joinhere
@blockcopy:
                ldrb    len,   [inp, #-1]!
                ldrb    index, [inp, #-1]!
                orr     index, index, len, LSL #8
                bic     index, index, #0xf000
                add     index, index, #0x0002
                add     len,   len,   #0x0020
@patterncopy:
                ldrb    data,  [outp, index]
                strb    data,  [outp, #-1]!
                subs    len,   len,   #0x0010
                bge     @patterncopy

@joinhere:
                cmp     inp, inp_top
                mov     flag, flag, LSL #1
                bgt     @loop8
@end_loop:
                bic     inp,  inp_top, #0x1f
@cacheflush:
                mcr     p15, 0, inp, c7,  c5, 1
                mcr     p15, 0, inp, c7, c14, 1
                add     inp, inp, #0x20
                cmp     inp, outp_save
                blt     @cacheflush
                ldmfd   sp!, {r4-r7}
@exit
                bx      lr
}