#include "types.h"

extern u8 data_02042a68[];
u32 func_02006c40(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10);
    if (*(u8 *)(p + 9) == 0) return 0;
    return *(u8 *)(p + 8);
}
