#include "types.h"

extern void func_02033bd0(u32 f);

/* Two-entry branch stub feeding the IRQ-mask helper func_02033bd0: entry 0
   clears the mask, entry +4 sets it, and both tail-branch into the shared
   body. Same shape as func_02033c0c above. */
asm void func_02033bc0(void)
{
    mov r0, #0
    b func_02033bd0
    mov r0, #1
    b func_02033bd0
}