#include "types.h"

/* Read-modify-write of one word: clear the bits in the first argument, xor in the
   second, return the previous value. The whole argument block is saved across the
   accessor call, which is why the pushes cover r0-r2 and ip. */

extern u32 *func_0203b6d0(void);

asm void func_0203b4e4(u32 clr, u32 set)
{
    stmdb sp!, {r0, r1, r2, ip, lr}
    bl    func_0203b6d0
    mov   r3, r0
    ldmia sp!, {r0, r1, r2, ip, lr}
    ldr   r2, [r3]
    bic   ip, r2, r0
    eor   ip, ip, r1
    str   ip, [r3]
    mov   r0, r2
    bx    lr
}