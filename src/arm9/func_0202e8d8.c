#include "types.h"

extern u8 *func_0202e2ec(void);
extern u32 func_0202e1cc(u32, u32, u32);
extern void func_02025c5c(u32, u32);

u32 func_0202e8d8(void)
{
    u8 *p = func_0202e2ec();

    if (func_0202e1cc(2, 7, 8) != 0) return 0;
    func_02025c5c(*(u32 *)(p + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(p + 4) + 0x10) == 1) return 0;
    func_02025c5c(*(u32 *)(p + 4) + 0x18e, 4);
    return (*(u16 *)(*(u32 *)(p + 4) + 0x18e) + 0x1f) & ~0x1f;
}