#include "types.h"

#pragma thumb on

/* NitroSDK MTX_RotY33_ (fx_mtx33.c): rotation about the y-axis, a hand-written
   thumb asm block in the SDK itself, assembled by mwasmarm. Body is retail
   byte for byte under 1.2/b56. See notes/mwccarm-codegen-log.md. */
asm void func_0201fce4(register u32 *p, register u32 sinVal, register u32 cosVal)
{
    str r2, [r0, #0]
    str r2, [r0, #32]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #12]
    str r3, [r0, #20]
    str r3, [r0, #28]
    neg r2, r1
    mov r3, #1
    lsl r3, r3, #12
    str r1, [r0, #24]
    str r2, [r0, #8]
    str r3, [r0, #16]
    bx lr
}