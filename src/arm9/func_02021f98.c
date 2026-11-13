#include "types.h"

extern void func_020222e8(u32, u32, u32, u32);
extern void func_020221b8(u32, u32);
void func_02021f98(u32 a0, u32 a1, u32 a2, u32 a3)
{
    func_020222e8(a0, 0x23, (u16)a1 | ((u16)a2 << 16), (u16)a3);
    func_020221b8(a0, 2);
}
