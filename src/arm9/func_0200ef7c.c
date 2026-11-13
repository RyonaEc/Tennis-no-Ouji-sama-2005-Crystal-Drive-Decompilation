#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
extern u32 func_02010150(u32);
extern u32 func_0200efd0(u32);
u32 func_0200ef7c(u32 a0)
{
    if (func_02010150(a0) != 0
        && (*(u8 *)(data_0203d06c + a0 * 0x18) & 1)
        && func_0200efd0(a0) != 0)
        return a0 + 1;
    return 0x45;
}
