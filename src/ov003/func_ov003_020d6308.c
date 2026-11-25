#include "types.h"

extern u8 data_02042a68[];
extern u32 data_ov003_020eb720[];

extern void func_02006584(s32 arg);
extern void func_02006504(u32 a0, s32 a1);

void func_ov003_020d6308(void)
{
    u8 *base = (u8 *)data_ov003_020eb720[0];
    u16 *st = (u16 *)(base + 0xb8dc);
    u16 s = *(u16 *)(data_02042a68 + 0x32);

    if ((s & 1) == 0)
        return;
    if (*st == 0xe) {
        *st = 9;
        func_02006584(0);
    }
    if (*st == 0xf) {
        *st = 0xa;
        func_02006584(0);
    }
    *(u16 *)(base + 0xb60c) = 0xe;
    func_02006504(1, 1);
}