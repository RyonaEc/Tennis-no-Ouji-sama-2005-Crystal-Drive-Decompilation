#include "types.h"

extern u8 data_02042a68[];
extern s32 func_0201f76c(void *);

s32 func_02006448(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 8);

    return func_0201f76c(p + 0x98) != 0;
}
