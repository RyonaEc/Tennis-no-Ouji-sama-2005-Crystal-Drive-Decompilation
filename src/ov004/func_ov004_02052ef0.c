#include "types.h"

extern void func_ov004_02053218(u8 *p, s32 *a, s32 *b);
extern void func_02025c94(u32 a0, u32 a1);
extern s32 func_020186a0(u32 a0, u32 a1, u32 a2, u32 a3);

s32 func_ov004_02052ef0(u8 *p)
{
    s32 a = 0;
    s32 b = 0;

    func_ov004_02053218(p, &a, &b);
    if (*(u32 *)(p + 0x7c) == 0)
        return 0;
    func_02025c94(*(u32 *)(p + 0x7c), *(u32 *)(p + 0x80));
    if (a != 0)
        func_020186a0(0x13, *(u32 *)(p + 0x64) << 6, *(u32 *)(p + 0x7c), *(u32 *)(p + 0x80));
    if (b != 0)
        func_020186a0(0x23, *(u32 *)(p + 0x64) << 6, *(u32 *)(p + 0x7c), *(u32 *)(p + 0x80));
    return 1;
}