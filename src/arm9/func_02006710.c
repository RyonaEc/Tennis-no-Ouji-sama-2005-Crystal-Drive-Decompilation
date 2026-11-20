#include "types.h"

extern u8 data_02042a68[];
extern void func_020067fc(void);
extern void func_0201d900(u32, u32);
extern void func_0201dfd8(u32, u32);
extern void func_0201df30(u32, u32);
extern void func_0201c53c(u8 *);
extern void func_0201f8e0(u8 *);
extern u32 func_0201d9d8(u32);

void func_02006710(u32 a0)
{
    u8 *s = *(u8 **)(data_02042a68 + 8);

    if (*(u32 *)(s + 0xac) == a0) return;
    *(u32 *)(s + 0xac) = a0;
    func_020067fc();
    func_0201d900(*(u32 *)s, *(u32 *)(s + 0xb0));
    switch (*(u32 *)(s + 0xac))
    {
    case 1:
        func_0201dfd8(0, *(u32 *)s);
        func_0201df30(0, *(u32 *)s);
        *(u32 *)(s + 0xa4) = 0;
        *(u32 *)(s + 0xa8) = 0;
        break;
    case 2:
        func_0201dfd8(1, *(u32 *)s);
        func_0201df30(1, *(u32 *)s);
        *(u32 *)(s + 0xa4) = 1;
        *(u32 *)(s + 0xa8) = 1;
        break;
    }
    func_0201c53c(s + 0x8c);
    func_0201c53c(s + 0x90);
    func_0201c53c(s + 0x94);
    func_0201f8e0(s + 0x98);
    *(u32 *)(s + 0xb4) = func_0201d9d8(*(u32 *)s);
}