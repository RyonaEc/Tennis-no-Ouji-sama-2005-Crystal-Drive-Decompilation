#include "types.h"

extern u8 *func_020276e8(u32);
extern void func_020276b4(u8 *);

void func_02026ee4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u8 *p = func_020276e8(1);

    if (p == 0) return;
    *(u32 *)(p + 4) = a0;
    *(u32 *)(p + 8) = a1;
    *(u32 *)(p + 0xc) = a2;
    *(u32 *)(p + 0x10) = a3;
    *(u32 *)(p + 0x14) = a4;
    func_020276b4(p);
}