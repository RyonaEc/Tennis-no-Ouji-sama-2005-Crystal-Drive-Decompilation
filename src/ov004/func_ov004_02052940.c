#include "types.h"

extern asm void func_02026af4(register void *dstp, register u8 data, register u32 size);
extern u32 func_ov004_02053314(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
extern u32 func_ov004_02052a00(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);

s32 func_ov004_02052940(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9)
{
    u8 *p = (u8 *)a9;
    u32 res;
    s32 r;

    func_02026af4(p, 0, 0xac);
    if (func_ov004_02053314(a0, a1, a6, a7, 1, (u32)p) == 0)
        return 0;
    res = func_ov004_02052a00(*(u32 *)(p + 0x24) / 20, 0, a2, a3, a4, a5, a8, (u32)p);
    if (res == 0)
        r = 0;
    else {
        *(u32 *)p = 1;
        *(u32 *)(p + 4) = a1;
        r = 1;
    }
    return r;
}