#include "types.h"

extern u32 func_0202e1cc(u32, u32);
extern void func_02025c78(u32, u32);
extern void func_0202e418(u32, u32);
extern u8 *func_0202e2ec(void);
extern void func_02026b88(u32, u8 *, u32);
extern void func_02026af4(u8 *, u32, u32);
extern u32 func_0202e2fc(u32, u32);

u32 func_0202eb28(u32 a0, u32 a1, u32 a2)
{
    u8 *d;
    u32 r = func_0202e1cc(1, 2);

    if (r != 0) return r;
    if (a1 == 0) return 6;
    func_02025c78(a1, 0xc0);
    func_0202e418(0xc, a0);
    d = *(u8 **)(func_0202e2ec() + 0xc);
    *(u16 *)d = 0xc;
    *(u32 *)(d + 4) = a1;
    if (a2 != 0) func_02026b88(a2, d + 8, 0x18);
    else func_02026af4(d + 8, 0, 0x18);
    if (func_0202e2fc(0xc, 0) == 0) return 2;
}