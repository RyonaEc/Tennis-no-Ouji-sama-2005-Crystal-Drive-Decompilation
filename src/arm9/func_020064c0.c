#include "types.h"

extern u8 data_02042a68[];
extern void func_0201e5f8(u8 *, s32, s32, s32, s32);

void func_020064c0(s32 sel, s32 arg)
{
    u8 *p = *(u8 **)(data_02042a68 + 8);

    func_0201e5f8(p + 0x94, sel ? 3 : 2, -1, -1, arg);
}