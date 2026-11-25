#include "types.h"

extern u32 func_020195dc(u32 a0, u32 a1);
extern void func_02025c94(u32 a0, u32 a1);
extern void func_02023f20(u32 a0, u32 a1, u32 a2);
extern void func_02023eb8(u32 a0, u32 a1, u32 a2);

void func_020192c8(u32 idx, u32 a, u32 b)
{
    u32 p;
    u16 n;
    u16 off;

    p = func_020195dc(idx, a);
    n = (u16)((b - a + 1) << 3);
    off = (u16)(a << 3);
    func_02025c94(p, n);
    switch (idx) {
    case 1:
        func_02023f20(p, off, n);
        return;
    case 2:
        func_02023eb8(p, off, n);
        return;
    }
}