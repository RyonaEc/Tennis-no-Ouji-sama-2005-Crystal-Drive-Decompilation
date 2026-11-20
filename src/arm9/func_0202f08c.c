#include "types.h"

extern u8 *func_0202e2ec(void);
extern u32 func_0202e1cc(u32, u32, u32);
extern void func_02025c5c(u32, u32);
extern void func_0202e418(u32, u32);
extern u32 func_0202e2fc(u32, u32);

u32 func_0202f08c(u32 a0)
{
    u8 *p = func_0202e2ec();
    u32 r = func_0202e1cc(2, 9, 0xa);

    if (r != 0) return r;
    func_02025c5c(*(u32 *)(p + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(p + 4) + 0x10) == 0) return 3;
    func_0202e418(0x10, a0);
    if (func_0202e2fc(0x10, 0) == 0) return 2;
}