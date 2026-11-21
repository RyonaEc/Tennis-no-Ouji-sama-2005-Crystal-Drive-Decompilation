#include "types.h"

extern void func_020179c8(u8 *, u32, void *, s32, s32);

u8 *func_0201842c(u8 *obj, s32 a, s32 b)
{
    u8 *p = obj + 0x24;

    func_020179c8(obj, 0x46524d48, p + 0xc, a, b);
    *(u32 *)p = *(u32 *)(obj + 0x18);
    *(u32 *)(p + 4) = *(u32 *)(obj + 0x1c);
    *(u32 *)(p + 8) = 0;
    return obj;
}