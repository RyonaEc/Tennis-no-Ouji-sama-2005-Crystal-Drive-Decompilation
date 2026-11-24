#include "types.h"

#pragma thumb on

/* NitroSDK MTX_RotZ33_ (fx_mtx33.c): rotation about the z-axis, a hand-written
   thumb asm block in the SDK itself, assembled by mwasmarm. Body is retail
   byte for byte under 1.2/b56. See notes/mwccarm-codegen-log.md. */
asm void func_0201fd00(register u32 *p, register u32 sinVal, register u32 cosVal)
{
    stmia r0!, {r2}
    mov r3, #0
    stmia r0!, {r1, r3}
    neg r1, r1
    stmia r0!, {r1, r2}
    mov r1, #1
    lsl r1, r1, #12
    str r3, [r0, #0]
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r1, [r0, #12]
    bx lr
}