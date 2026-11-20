#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32);
extern u32 func_0202e2d0(void);
extern u32 func_0202e2ec(void);

u32 func_0202e9c4(u32 a0, u32 a1, u32 a2)
{
    u32 h = func_02026350();
    u32 r = func_0202e2d0();
    u8 *t;

    if (r != 0)
    {
        func_02026364(h);
        return r;
    }
    t = (u8 *)func_0202e2ec() + a0 * 4;
    *(u32 *)(t + 0xb8) = a1;
    *(u32 *)(t + 0xf8) = a2;
    func_02026364(h);
    return 0;
}