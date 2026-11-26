#include "types.h"

extern u32 data_02045640;
extern u8 data_02045644[];
extern void func_02017994(u8 *p, u16 v);
extern u8 *func_02017a5c(s32 arg);
extern void func_020178e8(u8 *a0, u8 *a1);

void func_020179c8(u8 *p, s32 a1, s32 a2, s32 a3, u16 a4)
{
    *(u32 *)(p + 0) = a1;
    *(u32 *)(p + 0x18) = a2;
    *(u32 *)(p + 0x1c) = a3;
    *(u32 *)(p + 0x20) = 0;
    *(u32 *)(p + 0x20) &= ~0xff;
    *(u32 *)(p + 0x20) |= a4 & 0xff;
    func_02017994(p + 0xc, 4);
    if (data_02045640 == 0) {
        func_02017994(data_02045644, 4);
        data_02045640 = 1;
    }
    func_020178e8(func_02017a5c((s32)p), p);
}