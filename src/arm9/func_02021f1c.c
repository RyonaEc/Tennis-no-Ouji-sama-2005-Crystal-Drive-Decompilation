#include "types.h"

extern void func_0202230c(u32, u32, u32);
extern void func_020221b8(u32, u32);

void func_02021f1c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6)
{
    func_0202230c(a0, 0x29,
                  a1 | (a2 << 4) | (a3 << 6) | a6 | (a4 << 24) | (a5 << 16));
    func_020221b8(a0, 1);
}