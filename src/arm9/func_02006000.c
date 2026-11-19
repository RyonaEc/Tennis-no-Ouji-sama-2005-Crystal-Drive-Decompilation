#include "types.h"

extern u32 func_02024b04(u32 a0);
extern void func_0202cc24(u32 a0);
extern u32 func_0202d144(void);
extern int func_0202d158(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
extern void func_0202cc14(u32 a0);
extern void func_02024b5c(u32 a0);

int func_02006000(u32 a0, u32 a1, u32 a2)
{
    u32 x = func_02024b04(a0);
    u16 h;
    int r;

    if (x == (u32)-3)
        return 0;
    h = (u16)x;
    func_0202cc24(h);
    if ((func_0202d144() & 0xff) == 1)
        r = func_0202d158(a0, a1, a2, 0, 0, 0);
    else
        r = 0;
    func_0202cc14(h);
    func_02024b5c(h);
    return r;
}