#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203cb6c[];
extern u8 data_0203cb6e[];
extern u32 func_020101b4(u32);
extern void func_02010198(u32, u32);
void func_0200f208(u16 a0)
{
    *(u16 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + 6) = a0;
    if (func_020101b4(0x13) != 0) return;
    if (*(u16 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + 6) < 0xa) return;
    func_02010198(0x13, 1);
}
