#include "types.h"

extern void func_02025cbc(u32, u32);
extern void func_02025c5c(u32, u32);
extern void func_02026af4(u32, u32, u32);

void func_0202a598(u8 *o)
{
    u32 p = *(u32 *)(o + 4);
    u32 a = *(u32 *)(o + 8);
    u32 b = *(u32 *)(o + 0xc);
    u32 end = a + b;

    func_02025cbc(p, end);
    func_02025c5c(p, end);
    func_02026af4(p + a, 0, end - a);
}