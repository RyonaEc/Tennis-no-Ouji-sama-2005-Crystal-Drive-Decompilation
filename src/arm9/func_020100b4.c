#include "types.h"

#include "types.h"

extern u8 data_0203d06c[];
extern u8 data_02042a68[];
extern u32 func_0200efd0(u32);

u32 func_020100b4(u32 a0)
{
    u8 *d;

    if ((*(u8 *)(data_0203d06c + a0 * 0x18) & 4) != 0)
    {
        d = *(u8 **)(data_02042a68 + 0x10);
        d = *(u8 **)(d + 0x154);
        if (*(u8 *)(d + (a0 - 1) + 0x14) != 0)
        {
            if (func_0200efd0(a0 - 1) != 0) return 1;
        }
        return 0;
    }
    d = *(u8 **)(data_02042a68 + 0x10);
    d = *(u8 **)(d + 0x154);
    return *(u8 *)(d + a0 + 0x14) != 0;
}
