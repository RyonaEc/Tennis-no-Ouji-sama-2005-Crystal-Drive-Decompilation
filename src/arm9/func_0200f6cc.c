#include "types.h"

extern u8 data_0203cb6c[];
extern u8 data_0203cb6e[];
extern u8 data_02042a68[];

s32 func_0200f6cc(u32 a0)
{
    s32 off = a0 * 0x14;
    s32 start = *(u16 *)(data_0203cb6c + off);
    s32 end = *(u16 *)(data_0203cb6e + off);
    s32 i;
    s32 n = 0;

    for (i = start; i < start + end; i++) {
        if (*(u8 *)(*(u8 **)(*(u32 *)(data_02042a68 + 0x10) + 0x154) + i + 0xcc) != 0) n++;
    }
    return n;
}