#include "types.h"

extern void func_0201c140(u8 *a0, u32 a1, u32 a2);
extern void func_0201fc78(u8 *a0, u32 a1);
extern void func_0201bf74(u8 *a0, u32 a1);

void func_0201c2d4(u8 *a0, u32 a1)
{
    u8 v;

    if (a0 == 0)
        return;
    v = *(u8 *)(a0 + 0x2c);
    if (v == 0)
        return;
    if (a1 == 0) {
        func_0201c140(a0, v, a1);
        return;
    }
    func_0201fc78(a0 + 0x1c, 0);
    func_0201bf74(a0, 0);
    *(u8 *)(a0 + 0x2c) = 2;
}