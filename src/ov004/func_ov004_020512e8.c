#include "types.h"

extern void func_ov004_02051448(u8 *, void *);
extern u8 func_ov004_02051348[];

s32 func_ov004_020512e8(u8 *p, s32 a1, s32 a2, s32 a3)
{
    if (p == 0)
        return 0;
    func_ov004_02051448(p, func_ov004_02051348);
    *(u16 *)(p + 0x9c) = a1;
    *(u16 *)(p + 0x9e) = a2;
    *(u16 *)(p + 0xa0) = a3;
    *(u16 *)(p + 0xa2) = 0;
    *(u16 *)(p + 0xa4) = 0;
    *(u16 *)(p + 0xa6) = 0;
    return 1;
}