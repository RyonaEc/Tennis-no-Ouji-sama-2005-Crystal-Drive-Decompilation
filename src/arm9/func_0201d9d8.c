#include "types.h"

extern u32 func_020181b0(u32, u32);
extern u32 func_0201d860(u32);
extern void func_02018154(u32, u32);

s32 func_0201d9d8(u8 *a0)
{
    if (func_020181b0(*(u32 *)a0, *(u16 *)(a0 + 0xc)) == 0) return -1;
    if (func_0201d860((u32)a0) != 0) return *(u16 *)(a0 + 0xc) - 1;
    func_02018154(*(u32 *)a0, 0);
    return -1;
}