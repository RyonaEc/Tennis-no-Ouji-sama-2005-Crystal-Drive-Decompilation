#include "types.h"

#pragma thumb on

/* NitroSDK MTX_RotX33_ (fx_mtx33.c): a 3x3 rotation about the x-axis, written
   as a hand-written thumb asm block in the SDK itself and assembled by
   mwasmarm, which is why no mwccarm build produces this shape from C - the
   `movs r3, #1` / `lsls r3, r3, #0xc` materialisation is the SDK's own code, not
   a compiler policy. Body below is retail byte for byte; 1.2/b56 assembles it
   without any per-file compiler override. See notes/mwccarm-codegen-log.md. */
asm void func_0201fcc8(register u32 *p, register u32 sinVal, register u32 cosVal)
{
    mov r3, #1
    lsl r3, r3, #12
    str r3, [r0, #0]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r3, [r0, #12]
    str r2, [r0, #16]
    str r1, [r0, #20]
    str r3, [r0, #24]
    neg r1, r1
    str r1, [r0, #28]
    str r2, [r0, #32]
    bx lr
}
