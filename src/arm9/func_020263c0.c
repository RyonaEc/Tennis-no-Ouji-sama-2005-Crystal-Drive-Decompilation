#include "types.h"

#pragma arm on

/* NitroSDK OS_SpinWait (os_system.c). Body transcribed from the SDK source; the
   instruction sequence is the original, not a C rewrite. */
asm void func_020263c0(u32 cycle)
{
_1: subs   r0, r0, #4   // 3 cycle
    bcs    _1           // 1 cycle
    bx     lr
}