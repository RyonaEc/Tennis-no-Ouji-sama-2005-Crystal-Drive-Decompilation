#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32);
extern void func_0201c914(u8 *, u32);
extern void func_02027244(u32, u32, u32, u32);

void func_0201c848(u8 *a0)
{
    u32 r;

    if (((s32)(*(u32 *)(a0 + 0x24) << 0x1e) >> 0x1f) == 0) return;
    while (*(u32 *)(a0 + 0x38) != 0)
    {
        r = func_02026350();
        func_0201c914(a0, 1);
        func_02026364(r);
    }
    func_02027244(*(u32 *)(a0 + 0x44), 0, 1u << *(u32 *)(a0 + 0x40), 0);
}