#include "types.h"

extern void func_ov004_02053518(u8 *p);
extern u8 *func_ov004_0204e874(s32 a0);
extern u32 func_ov004_020528c8(u32 a0);

void func_ov004_02054878(u8 *p)
{
    u8 *q;

    func_ov004_02053518(*(u8 **)(p + 0x30));
    q = func_ov004_0204e874(0x1f);
    if (q != 0)
        func_ov004_020528c8((u32)q);
    q = func_ov004_0204e874(0x20);
    if (q != 0)
        func_ov004_020528c8((u32)q);
    q = func_ov004_0204e874(0x21);
    if (q != 0)
        func_ov004_020528c8((u32)q);
    *(u32 *)0x04000000 = (*(u32 *)0x04000000 & ~0x1f00) | (*(u32 *)(p + 0x18) << 8);
}