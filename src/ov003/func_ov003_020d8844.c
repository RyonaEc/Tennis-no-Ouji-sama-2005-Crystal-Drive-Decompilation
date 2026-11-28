#include "types.h"

extern u8 data_02042a68[];
extern u8 data_ov003_020eb720[];
extern void func_02006504(u32 a0, s32 a1);

void func_ov003_020d8844(void)
{
    u8 *b = *(u8 **)data_ov003_020eb720;
    u8 *p = b + 0xb8dc;
    u16 k = *(u16 *)(data_02042a68 + 0x30);

    if (k & 1) {
        if (*(u16 *)(p + 4) == 1)
            *(u16 *)p = 5;
        else
            *(u16 *)p = 4;
        func_02006504(1, 1);
        *(u16 *)(b + 0xb60c) = 0x14;
        return;
    }
    if (k & 0x10) {
        if (*(u16 *)(p + 4) == 0) {
            *(u16 *)(p + 4) = 1;
            func_02006504(1, 0);
        }
        if (*(u16 *)(data_02042a68 + 0x32) & 0x10)
            *(u16 *)(b + 0xb60c) = 0xe;
        else
            *(u16 *)(b + 0xb60c) = 7;
        return;
    }
    if ((k & 0x20) == 0)
        return;
    if (*(u16 *)(p + 4) == 1) {
        *(u16 *)(p + 4) = 0;
        func_02006504(1, 0);
    }
    if (*(u16 *)(data_02042a68 + 0x32) & 0x20)
        *(u16 *)(b + 0xb60c) = 0xe;
    else
        *(u16 *)(b + 0xb60c) = 7;
}