#include "types.h"

extern u8 data_02042a68[];
extern void func_0201c58c(void *, s32);

void func_02006584(s32 arg)
{
    u8 *p = *(u8 **)(data_02042a68 + 8);

    func_0201c58c(p + 0x8c, arg);
    *(u32 *)(p + 0xa0) = 0xffffffff;
}
