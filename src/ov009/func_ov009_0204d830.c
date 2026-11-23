#include "types.h"

extern void func_02021aa8(void *, s32);

u32 func_ov009_0204d830(u8 *p)
{
    if (*(u16 *)(p + 0x18) == 0) {
        if (*(u16 *)(p + 0x10) < 0x10)
            (*(u16 *)(p + 0x10))++;
        func_02021aa8((void *)0x04000050, -(s32)*(u16 *)(p + 0x10));
    } else {
        if (*(u16 *)(p + 0x10) < 0x10)
            (*(u16 *)(p + 0x10))++;
        func_02021aa8((void *)0x04000050, *(u16 *)(p + 0x10));
    }
    return 0;
}