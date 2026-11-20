#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0204226c[];
extern void func_0201bdb4(void);
extern u8 *func_02017b60(u32, u32, u32);
extern void func_02026a04(u32, u8 *, u32);
extern u32 func_02025d24(u32, u32, u32);
extern u32 func_0201db68(u32, u32);
extern void func_020263cc(void);
extern void func_0201d7c0(u8 *, u8 *, u32, u32);
extern void func_0201e6c4(u32);
extern void func_0201c59c(u32, u32, u32);
extern void func_0201fac4(u32, u32);
extern u32 func_0201d9d8(u32);
extern void func_02006710(u32);

void func_02006640(void)
{
    volatile u32 v;
    u8 *s;

    func_0201bdb4();
    s = func_02017b60(*(u32 *)data_02042a68, 0xb8, 4);
    v = 0;
    *(u8 **)(data_02042a68 + 8) = s;
    func_02026a04(v, s, 0xb8);
    *(u32 *)s = func_0201db68(func_02025d24(0, 0xa0000, 0x10), 0xa0000);
    if (*(u32 *)s == 0) func_020263cc();
    func_0201d7c0(s + 4, data_0204226c, *(u32 *)s, 0);
    func_0201e6c4(*(u32 *)s);
    func_0201c59c(3, *(u32 *)s, 0x7800);
    func_0201fac4(0xa, *(u32 *)s);
    *(u32 *)(s + 0xb0) = func_0201d9d8(*(u32 *)s);
    func_02006710(1);
}