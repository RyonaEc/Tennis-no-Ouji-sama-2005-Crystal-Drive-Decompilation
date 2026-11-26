#include "types.h"

extern u32 _s32_div_f(u32 a0, u32 a1);

void func_ov002_0204f808(u8 *a0, u8 *a1)
{
    s32 s = *(s16 *)(a0 + 6);
    s32 r4 = s + 0x320;
    s32 v = _s32_div_f(0x02f6ff7a, 0x90880 - r4 * 0xb0);
    s32 t = *(s16 *)(a0 + 2);
    s32 p = r4 * -0xf0;
    s32 r3 = v * (t + 0x80);
    s32 u;

    r3 = (r3 / 128) + 0x80 - v;
    u = *(s16 *)(a0 + 0xa);
    *(u16 *)a1 = (u16)r3;
    *(u16 *)(a1 + 2) = (u16)(_s32_div_f(0xeee8ff28, 0xfff79a00 - p) - 0x2a0);
    *(u16 *)(a1 + 4) = (u16)u;
}