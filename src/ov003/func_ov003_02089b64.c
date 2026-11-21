#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern u8 *data_ov003_020eb720;

s32 func_ov003_02089b64(void)
{
    u8 *p = data_ov003_020eb720;
    u16 a = *(u16 *)(p + 0x170) + 1;

    if (*(u16 *)(p + 0x13c) != 0)
        return 4;
    if (*(u16 *)(p + 0x142) < a)
        a = *(u16 *)(p + 0x142);
    return a;
}