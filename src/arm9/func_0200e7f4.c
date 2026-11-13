#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
u32 func_0200e7f4(u32 a0, u32 a1, u32 a2, u32 a3)
{
    if (*(u8 *)(data_0203d06c + a0 * 0x18) & 4) a0--;
    if (*(u8 *)(data_0203d06c + a1 * 0x18) & 4) a1--;
    if (*(u8 *)(data_0203d06c + a2 * 0x18) & 4) a2--;
    if (*(u8 *)(data_0203d06c + a3 * 0x18) & 4) a3--;
    if (a0 == a2 || a0 == a3) return 1;
    if (a1 == a2 || a1 == a3) return 2;
    return 0;
}
