#include "types.h"

extern void func_02033c1c(u32 f);

/* Two-entry branch stub feeding the IRQ-mask helper func_02033c1c: entry 0
   clears the mask, entry +4 sets it, and both tail-branch into the shared
   body (which returns the previous state). Same shape as func_02033c58 above
   and func_02024af8's `ldr/bx` trampoline, so it is written as asm rather than
   guessed at as C. */
asm void func_02033c0c(void)
{
    mov r0, #0
    b func_02033c1c
    mov r0, #1
    b func_02033c1c
}
