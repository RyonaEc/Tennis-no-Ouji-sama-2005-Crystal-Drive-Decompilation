#include "types.h"

extern u8 data_02042a68[];
extern void func_020146c8(s32);

void func_020136b0(u8 v)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x14) + 0x104);

    func_020146c8(1);
    p[2] = v;
}
