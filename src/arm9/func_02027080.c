#include "types.h"

extern void func_02026ee4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

void func_02027080(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6,
                   u32 p7, u32 p8, u32 p9)
{
    func_02026ee4(0xb,
                  p0 | (p8 << 16),
                  p2,
                  (p6 << 24) | (p7 << 22) | p5,
                  (p3 << 26) | (p1 << 24) | (p9 << 16) | p4);
}