#include "types.h"

extern u8 *func_0202e2ec(void);
extern u32 func_0202e2d0(void);
extern void func_02025c5c(u32, u32);

u32 func_0202e718(void)
{
    u8 *p = func_0202e2ec();
    u16 v;

    if (func_0202e2d0() != 0) return 0;
    func_02025c5c(*(u32 *)(p + 4), 2);
    v = *(u16 *)(*(u32 *)(p + 4));
    switch (v)
    {
    case 9:
        func_02025c5c(*(u32 *)(p + 4) + 0x17e, 2);
        if (*(u16 *)(*(u32 *)(p + 4) + 0x17e) == 0) return 0;
        /* fall through */
    case 0xa:
    case 0xb:
        func_02025c5c(*(u32 *)(p + 4) + 0xb8, 2);
        return *(u16 *)(*(u32 *)(p + 4) + 0xb8);
    }
    return 0;
}