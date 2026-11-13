#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203cb6c[];
extern u8 data_0203cb6e[];
extern void func_02015368(void);
void func_02015304(void)
{
    func_02015368();
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) |= 8;
    *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x18) = 1;
    *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x12) = 1;
    *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x10) = *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x12);
    *(volatile u16 *)0x04000204 &= ~0x8000;
}
