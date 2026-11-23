#include "types.h"

void func_02018cdc(s32 a0, s32 a1, s32 a2, s32 a3)
{
    volatile u32 *dat = (volatile u32 *)0x04000488;
    volatile u32 *ctl = (volatile u32 *)0x0400048c;
    u16 v0 = (u16)(s16)(a0 >> 8);
    u16 v1 = (u16)(s16)(a1 >> 8);
    u16 v2 = (u16)(s16)(a2 >> 8);
    u16 v3 = (u16)(s16)(a3 >> 8);

    *(volatile u32 *)0x04000500 = 1;
    *dat = v0 | (v2 << 16);
    *ctl = 0;
    *ctl = 0;
    *dat = v1 | (v2 << 16);
    *ctl = 0x1000;
    *ctl = 0;
    *dat = v1 | (v3 << 16);
    *ctl = 0x10001000;
    *ctl = 0;
    *dat = v0 | (v3 << 16);
    *ctl = 0x10000000;
    *ctl = 0;
    *(volatile u32 *)0x04000504 = 0;
}