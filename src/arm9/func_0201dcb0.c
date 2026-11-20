#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32);
extern u32 func_0201d5c8(u32);
extern u32 func_0201d1e0(u32);
extern void func_0201d1c4(u32, u32);

void func_0201dcb0(u32 a0, u32 a1, u32 a2)
{
    u32 x2 = a2;
    u32 x1 = a1;
    u32 h;

    h = func_02026350();
    x1 = func_0201d5c8(x1);
    if (a0 == func_0201d1e0(x2)) func_0201d1c4(x2, 0);
    func_0201d5c8(x1);
    func_02026364(h);
}