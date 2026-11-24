#include "types.h"

/* Clears one word at the first accessor and six words from the second. The
   original keeps a bare lr frame, so the calls and stores are written directly. */

extern u32 *func_0203b6d0(void);
extern u32 *func_0203b6dc(void);

asm void func_0203b4b0(void)
{
    stmdb sp!, {lr}
    bl    func_0203b6d0
    mov   r1, #0
    str   r1, [r0]
    bl    func_0203b6dc
    mov   r1, #0
    str   r1, [r0, #4]
    str   r1, [r0, #8]
    str   r1, [r0, #0xc]
    str   r1, [r0, #0x10]
    str   r1, [r0, #0x14]
    ldmia sp!, {lr}
    bx    lr
}