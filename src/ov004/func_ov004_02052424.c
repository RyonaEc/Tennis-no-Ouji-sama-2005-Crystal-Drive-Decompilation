#include "types.h"

extern void func_020048e8(u32, u8 *, u32);
void func_ov004_02052424(u8 *a0, s32 a1)
{
    volatile u32 z = 0;

    func_020048e8(z, a0, 0x70);
    *(s32 *)a0 = a1;
}
