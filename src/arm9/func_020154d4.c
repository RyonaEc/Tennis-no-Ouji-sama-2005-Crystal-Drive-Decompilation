#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
void func_020154d4(u8 *a0)
{
    if (*(u16 *)(a0 + 2) != 0) return;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~1;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~2;
}
