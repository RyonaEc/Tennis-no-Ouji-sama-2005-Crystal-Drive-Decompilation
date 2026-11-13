#include "types.h"

extern s32 func_0202e2d0(void);
u32 func_0202e7c0(void)
{
    if (func_0202e2d0() != 0) return 0x8000;
    return *(u16 *)0x027ffcfa;
}
