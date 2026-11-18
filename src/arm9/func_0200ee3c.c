#include "types.h"

extern u8 data_0203bbe8[];

s32 func_0200ee3c(s32 a0)
{
    return (*(u8 *)(data_0203bbe8 + a0 * 8) & 2) != 0;
}
