#include "types.h"

extern u8 data_0203bbe8[];
extern u8 data_02042a68[];
extern u32 func_0200ee58(u32);

u32 func_0200fe9c(u32 a0)
{
    u8 *d;

    if ((*(u8 *)(data_0203bbe8 + a0 * 8) & 2) != 0)
    {
        d = *(u8 **)(data_02042a68 + 0x10);
        d = *(u8 **)(d + 0x154);
        if (*(u8 *)(d + (a0 - 1) + 0x64) != 0)
        {
            if (func_0200ee58(a0 - 1) != 0) return 1;
        }
        return 0;
    }
    d = *(u8 **)(data_02042a68 + 0x10);
    d = *(u8 **)(d + 0x154);
    return *(u8 *)(d + a0 + 0x64) != 0;
}