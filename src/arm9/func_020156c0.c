#include "types.h"

extern u8 data_02042a68[];
extern u8 *func_02017b60(u32, u32, u32);
extern void func_020263cc(void);
extern void func_02026a04(u32, u8 *, u32);

void func_020156c0(void)
{
    volatile u32 v;

    *(u8 **)(data_02042a68 + 0x14) = func_02017b60(*(u32 *)data_02042a68, 0x10c, 4);
    if (*(u8 **)(data_02042a68 + 0x14) == 0) func_020263cc();
    v = 0;
    func_02026a04(v, *(u8 **)(data_02042a68 + 0x14), 0x10c);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x14) + 0xfc) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x120, 0x20);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x14) + 0x100) = (u32)func_02017b60(*(u32 *)data_02042a68, 0xf00, 0x20);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x14) + 0x104) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x100, 0x20);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x14) + 0x108) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x100, 0x20);
}