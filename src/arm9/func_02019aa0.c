#include "types.h"

extern void func_02019cac(void);
extern void func_02019d64(void);
extern void func_02019dc4(void);
extern void func_02019e24(void);
extern void func_02019e78(void);

extern void func_020269b8(u16, u32, u32);

void func_02019aa0(u8 *a0)
{
    volatile u16 z = 0;

    func_020269b8(z, (u32)a0, 0x4c);
    *(u32 *)(a0 + 0x10) = 1;
    *(u32 *)(a0 + 0x14) = 3;
    *(u32 *)(a0 + 0x18) = (u32)func_02019e78;
    *(u32 *)(a0 + 0x1c) = (u32)func_02019e24;
    *(u32 *)(a0 + 0x20) = (u32)func_02019dc4;
    *(u32 *)(a0 + 0x24) = (u32)func_02019d64;
    *(u32 *)(a0 + 0x28) = (u32)func_02019cac;
}