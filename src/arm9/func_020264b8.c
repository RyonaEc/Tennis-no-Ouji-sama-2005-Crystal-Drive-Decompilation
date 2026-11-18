#include "types.h"

extern u16 data_02049028;
extern void func_020263cc(s32 a0);

void func_020264b8(s32 a0, u32 a1)
{
    u32 t = ((a1 & 0x7f00) << 8) >> 16;

    if (t == 0x10) {
        data_02049028 = 1;
        return;
    }
    func_020263cc(t);
}
