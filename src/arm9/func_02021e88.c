#include "types.h"

extern void func_0202230c(u32, u32, u32);
extern void func_020221b8(u32, u32);
void func_02021e88(u32 a0, u32 a1, u32 a2)
{
    u32 s = 4 - (a2 == 2);
    func_0202230c(a0, 0x2b, a1 >> s);
    func_020221b8(a0, 1);
}
