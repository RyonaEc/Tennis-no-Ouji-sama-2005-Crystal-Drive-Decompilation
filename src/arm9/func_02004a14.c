#include "types.h"

/* NitroSDK crt0.c init_cp15: protection unit, cache and TCM setup, written in
   the SDK as a hand-written arm asm block. The SET_PROTECTION_A/B macros expand
   to the literal values below. Body is retail byte for byte; 1.2/b56 assembles
   it without a per-file override. */
extern u8 data_027c0000;

asm void func_02004a14(void)
{
        mrc     p15, 0, r0, c1, c0, 0
        ldr     r1, =0x000f9005
        bic     r0, r0, r1
        mcr     p15, 0, r0, c1, c0, 0

        mov     r0, #0
        mcr     p15, 0, r0, c7, c5, 0
        mcr     p15, 0, r0, c7, c6, 0
        mcr     p15, 0, r0, c7, c10, 4

        ldr     r0, =0x04000033
        mcr     p15, 0, r0, c6, c0, 0
        ldr     r0, =0x0200002d
        mcr     p15, 0, r0, c6, c1, 0
        ldr     r0, =0x027c0023
        mcr     p15, 0, r0, c6, c2, 0
        ldr     r0, =0x08000035
        mcr     p15, 0, r0, c6, c3, 0
        ldr     r0, =data_027c0000
        orr     r0, r0, #0x1a
        orr     r0, r0, #1
        mcr     p15, 0, r0, c6, c4, 0
        ldr     r0, =0x0100002f
        mcr     p15, 0, r0, c6, c5, 0
        ldr     r0, =0xffff001d
        mcr     p15, 0, r0, c6, c6, 0
        ldr     r0, =0x027ff017
        mcr     p15, 0, r0, c6, c7, 0

        mov     r0, #0x20
        mcr     p15, 0, r0, c9, c1, 1
        ldr     r0, =data_027c0000
        orr     r0, r0, #0xa
        mcr     p15, 0, r0, c9, c1, 0

        mov     r0, #0x42
        mcr     p15, 0, r0, c2, c0, 1
        mov     r0, #0x42
        mcr     p15, 0, r0, c2, c0, 0
        mov     r0, #2
        mcr     p15, 0, r0, c3, c0, 0
        ldr     r0, =0x05100011
        mcr     p15, 0, r0, c5, c0, 3
        ldr     r0, =0x15111011
        mcr     p15, 0, r0, c5, c0, 2

        mrc     p15, 0, r0, c1, c0, 0
        ldr     r1, =0x0005707d
        orr     r0, r0, r1
        mcr     p15, 0, r0, c1, c0, 0
        bx      lr
}