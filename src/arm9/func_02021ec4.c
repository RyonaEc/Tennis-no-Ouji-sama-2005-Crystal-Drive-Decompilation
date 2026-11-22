#include "types.h"

extern void func_0202230c(u32 a0, u32 a1, u32 a2);
extern void func_020221b8(u32 a0, u32 a1);

void func_02021ec4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                   u32 a7, u32 a8)
{
    u32 w = (a8 >> 3) | (a1 << 26) | (a2 << 30) | (a3 << 20)
          | (a4 << 23) | (a5 << 16) | (a6 << 18) | (a7 << 29);

    func_0202230c(a0, 0x2a, w);
    func_020221b8(a0, 1);
}