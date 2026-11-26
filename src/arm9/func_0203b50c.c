#include "types.h"

/* Repack both arguments into the single-word bit layout the accessor expects,
   then do the read-modify-write of func_0203b4e4 and return the repacked previous
   value with bit 30 set. */

extern u32 *func_0203b6d0(void);

asm void func_0203b50c(u32 a0, u32 a1)
{
    and   r2, r0, #0x1f
    and   r3, r0, #0x1f0000
    orr   r0, r2, r3, lsr #8
    and   r2, r1, #0x1f
    and   r3, r1, #0x1f0000
    orr   r1, r2, r3, lsr #8
    stmdb sp!, {r0, r1, r2, ip, lr}
    bl    func_0203b6d0
    mov   r3, r0
    ldmia sp!, {r0, r1, r2, ip, lr}
    ldr   r2, [r3]
    bic   ip, r2, r0
    eor   ip, ip, r1
    str   ip, [r3]
    and   r0, r2, #0x1f
    and   r2, r2, #0x1f00
    orr   r0, r0, r2, lsl #8
    orr   r0, r0, #0x40000000
    bx    lr
}