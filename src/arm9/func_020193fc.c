#include "types.h"

extern u32 func_020195c0(u32, u32);

void func_020193fc(u32 a0, s32 *a1, u32 a2)
{
    u8 *p = (u8 *)func_020195c0(a0, a2);
    *(u16 *)(p + 6) = a1[0] >> 4;
    *(u16 *)(p + 0xe) = a1[1] >> 4;
    *(u16 *)(p + 0x16) = a1[2] >> 4;
    *(u16 *)(p + 0x1e) = a1[3] >> 4;
}