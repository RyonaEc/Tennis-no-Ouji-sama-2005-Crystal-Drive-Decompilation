#include "types.h"

extern u8 data_02042a68[];
void func_02009160(u32 a0, u32 a1)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10);
    *(u32 *)(p + a0 * 4 + 0x110) = a1;
}
