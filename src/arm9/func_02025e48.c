#include "types.h"

extern u8 data_027c0000[];

/* NitroSDK OS_GetInitArenaHi() (os_arena.c). The DTCM case branches on
   `OSi_SYS_STACKSIZE`, which is `((int)SDK_SYS_STACKSIZE)` - a linker symbol,
   so the SDK's own compile keeps both arms and the literal 0 the game's LCF
   resolved it to. No C spelling in this tree can keep that branch (a plain 0
   constant folds it away and an extern becomes a symbol load), so the unit is
   written as the assembled body retail contains. */
asm void func_02025e48(void)
{
    cmp r0, #6
    addls pc, pc, r0, lsl #2
    b Lfail
    b Lmain
    b Lfail
    b Lzero
    b Litcm
    b Ldtcm
    b Lshared
    b Lwram
Lmain:
    mov r0, #0x23c0000
    bx lr
Lzero:
    mov r0, #0
    bx lr
Litcm:
    mov r0, #0x2000000
    bx lr
Ldtcm:
    ldr r0, =data_027c0000
    ldr r1, =0
    ldr r2, =0x400
    add r3, r0, #0x3f80
    cmp r1, #0
    sub r2, r3, r2
    bne Lsys
    ldr r1, =0x027c0020
    cmp r0, r1
    movlo r0, r1
    bx lr
Lsys:
    cmp r1, #0
    sublt r0, r0, r1
    subge r0, r2, r1
    bx lr
Lshared:
    ldr r0, =0x027ff800
    bx lr
Lwram:
    ldr r0, =0x037f8000
    bx lr
Lfail:
    mov r0, #0
    bx lr
}