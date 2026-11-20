#include "types.h"

u8 *func_ov004_0204e874(s32 a0)
{
    u8 *g = *(u8 **)0x020690a8;
    s32 off = a0 * 0xac;

    return *(u32 *)(g + off + 0x530) ? (u8 *)(g + 0x530) + off : 0;
}