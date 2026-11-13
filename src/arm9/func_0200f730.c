#include "types.h"

extern u8 data_0203cb6e[];

u32 func_0200f730(u32 a0)
{
    return *(u16 *)((u8 *)((data_0203cb6e + (a0 * 0x14))));
}
