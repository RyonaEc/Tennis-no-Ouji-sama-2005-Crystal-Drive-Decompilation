#include "types.h"

extern u8 data_02042a68[];
extern u32 func_0202cc74(void);
extern void func_0202d564(void);
extern void func_0202d0b0(u32);
extern u8 *func_02017b60(u32, u32, u32);
extern u32 func_02005734(u32);
extern void func_02006160(void);
extern void func_02006324(void);
extern void func_02005958(u32, u32);

void func_0200543c(void)
{
    if (func_0202cc74() == 0) func_0202d564();
    func_0202d0b0(0xd01);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x10) + 0x154) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x800, 4);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x10) + 0x158) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x200, 4);
    *(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x10) + 0x15c) = (u32)func_02017b60(*(u32 *)data_02042a68, 0x400, 4);
    if (func_02005734(0) == 0)
    {
        func_02006160();
        func_02005958(0, 0);
    }
    if (func_02005734(1) == 0)
    {
        func_02006324();
        func_02005958(1, 0);
    }
    *(u8 *)(*(u32 *)((u8 *)*(u8 **)(data_02042a68 + 0x10) + 0x15c)) = 0;
}