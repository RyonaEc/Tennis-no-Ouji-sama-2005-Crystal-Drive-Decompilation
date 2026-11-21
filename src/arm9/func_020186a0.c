#include "types.h"

extern u8 data_02045650[];
extern s32 func_02018ba4(u8 *a0);
extern u32 *func_020187f8(u8 *a0);
extern void func_02018818(u8 *a0);

s32 func_020186a0(u32 a0, u32 a1, u32 a2, u32 a3)
{
    u8 *g = data_02045650;
    u32 *p;

    if (func_02018ba4(g) != 0)
        return 0;
    p = func_020187f8(g);
    p[0] = a0;
    p[1] = a2;
    p[2] = a1;
    p[3] = a3;
    func_02018818(g);
    *(u32 *)(g + 0x10) += p[3];
    return 1;
}