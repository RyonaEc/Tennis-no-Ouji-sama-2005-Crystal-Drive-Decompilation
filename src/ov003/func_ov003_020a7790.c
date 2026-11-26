#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov003_020eb720[];
extern void func_02006504(u32 a0, s32 a1);

void func_ov003_020a7790(void)
{
    u8 *g = (u8 *)(*(u32 *)data_ov003_020eb720);
    u8 *b = g + 0xb8dc;

    if ((*(u16 *)(data_02042a68 + 0x36) & 1) == 0) {
        return;
    }
    if (*(u16 *)(b + 0x13a) < 0x20) {
        return;
    }
    if (*(u16 *)(b + 0xf4) == 0) {
        *(u16 *)(b + 0xf4) = 1;
        *(u16 *)(g + 0x250) = 2;
        *(u16 *)(b + 0x12a) = 1;
        func_02006504(1, 1);
        *(u16 *)(b + 0xf0) = 0x14;
        return;
    }
    if (*(u16 *)(b + 0xf4) != 2) {
        return;
    }
    if (*(u16 *)(b + 0x11c) != 0) {
        *(u16 *)(b + 0xf4) = 6;
    }
    *(u16 *)(b + 0xf0) = 0x14;
    func_02006504(1, 1);
}