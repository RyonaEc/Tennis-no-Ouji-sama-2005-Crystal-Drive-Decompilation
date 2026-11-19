#include "types.h"

extern u8 data_02042a68[];

s32 func_ov006_0204d0c8(s32 a0)
{
    if (*(u16 *)(data_02042a68 + 0x30) & 1) {
        if (a0 & 1)
            return a0 - 1;
        a0 = a0 - 4;
        if (a0 < 0)
            a0 = 0;
        return a0;
    }
    return a0 - 1;
}