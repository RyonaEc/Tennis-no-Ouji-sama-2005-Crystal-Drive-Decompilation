#include "types.h"

extern void func_02026af4(u8 *, s32, s32);

void func_ov004_020501e8(u8 *p, s32 a1, s32 a2, s32 a3,
                         s16 s0, s32 s1, s32 s2, s32 s3)
{
    func_02026af4(p, 0, 0x80);
    *(u16 *)(p + 0x14) = a1;
    *(u16 *)(p + 0x10) = a2;
    *(u16 *)(p + 0x12) = a3;
    *(s16 *)(p + 0x16) = s0;
    *(u32 *)(p + 4) = s1;
    *(u32 *)(p + 8) = s2;
    *(u32 *)(p + 0xc) = s3;
    *(u32 *)(p + 0x18) = -1;
}