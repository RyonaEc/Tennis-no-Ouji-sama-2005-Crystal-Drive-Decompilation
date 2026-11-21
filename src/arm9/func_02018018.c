#include "types.h"

extern void func_020179c8(u8 *a0, u32 a1, u8 *a2, u32 a3, u32 a4);
extern u32 func_020180a0(u32 *a0, u32 a1);

u8 *func_02018018(u8 *a0, u32 a1, u32 a2)
{
    u8 *q = a0 + 0x24;
    u32 pair[2];
    u32 r;

    func_020179c8(a0, 0x45585048, q + 0x14, a1, a2);
    *(u16 *)(q + 0x10) = 0;
    *(u16 *)(q + 0x12) = 0;
    *(u16 *)(q + 0x12) &= ~1;
    pair[0] = *(u32 *)(a0 + 0x18);
    pair[1] = *(u32 *)(a0 + 0x1c);
    r = func_020180a0(pair, 0x4652);
    *(u32 *)(q + 0) = r;
    *(u32 *)(q + 4) = r;
    *(u32 *)(q + 8) = 0;
    *(u32 *)(q + 0xc) = 0;
    return a0;
}