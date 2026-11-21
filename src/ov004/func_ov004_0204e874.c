#include "types.h"

extern u8 *data_ov004_020690a8;

u8 *func_ov004_0204e874(s32 a0)
{
    u8 *g = data_ov004_020690a8;
    s32 off = a0 * 0xac;

    return *(u32 *)(g + off + 0x530) ? (u8 *)(g + 0x530) + off : 0;
}