#include "types.h"

extern void func_02027a14(u32 a0);
extern void func_02026ee4(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

void func_020271d4(u32 a0, u32 a1, u32 a2, u32 a3)
{
    s32 i = 0;
    u32 m = a2;

    while (i < 8 && m != 0) {
        if (m & 1)
            func_02027a14(i);
        i++;
        m >>= 1;
    }
    func_02026ee4(0xa, a0, a1, a2, a3);
}