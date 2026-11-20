#include "types.h"

extern u32 func_0201829c(u32, u32, u32);
extern u32 func_0201d8b0(u32, u32);
extern void func_02018290(u32);

u32 func_0201db68(u32 a0, u32 a1)
{
    u32 end = a0 + a1;
    u32 s = (a0 + 3) & ~3;
    u32 n;
    u32 p;

    if (s > end) return 0;
    n = end - s;
    if (n < 0x10) return 0;
    p = func_0201829c(s + 0x10, n - 0x10, 0);
    if (p == 0) return 0;
    if (func_0201d8b0(s, p) != 0) return s;
    func_02018290(p);
    return 0;
}