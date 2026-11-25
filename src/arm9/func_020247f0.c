#include "types.h"

extern u8 data_02048ccc[];
extern u8 data_02048cd0[];
extern u8 data_02048cd4[];
extern u8 data_02042374[];
extern u8 data_027c0000;
extern u32 func_020248c8(u32 intr);

void func_020247f0(u32 a0)
{
    u32 off = a0 * 0xc;
    u32 bit = *(u16 *)(data_02042374 + a0 * 2);
    u32 mask = 1 << bit;
    void (*handler)(u32) = *(void (**)(u32))(data_02048ccc + off);

    *(u32 *)(data_02048ccc + off) = 0;
    if (handler != 0)
        handler(*(u32 *)(data_02048cd4 + off));
    *(u32 *)((u8 *)&data_027c0000 + 0x3ff8) |= mask;
    if (*(u32 *)(data_02048cd0 + off) != 0)
        return;
    func_020248c8(mask);
}