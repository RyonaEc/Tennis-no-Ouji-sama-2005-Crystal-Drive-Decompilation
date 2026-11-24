#include "types.h"

/* Two library calls in sequence with no other work: the original is a hand-written
   thunk, so the frame is a bare stmdb/ldmia pair with no 8-byte realignment. */

extern void func_02026140(void);
extern void func_020261d0(void);

asm void func_0202612c(void)
{
    stmdb sp!, {lr}
    bl    func_02026140
    bl    func_020261d0
    ldmia sp!, {lr}
    bx    lr
}