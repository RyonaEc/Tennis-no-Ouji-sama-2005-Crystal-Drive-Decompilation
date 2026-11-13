#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042b1c[];

u32 func_020176f0(u32 a0)
{
    if (*(u32 *)(data_02042b04) != 0x7) {
        *(u32 *)(data_02042b1c) = a0;
    }
    return a0;
}
