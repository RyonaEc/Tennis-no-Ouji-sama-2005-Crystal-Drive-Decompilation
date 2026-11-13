#include "types.h"

extern void func_0202230c(u32, u32, u32);
extern void func_020221b8(u32, u32);
void func_02021f60(u32 a0, u32 a1, u32 a2)
{
    func_0202230c(a0, 0x25, (u16)a1 | ((u16)a2 << 16));
    func_020221b8(a0, 1);
}
