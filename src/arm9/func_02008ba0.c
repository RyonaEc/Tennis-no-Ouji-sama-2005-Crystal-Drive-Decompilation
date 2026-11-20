#include "types.h"

extern u32 func_02010150(u32);
extern u8 data_0203d06c[];

u32 func_02008ba0(void)
{
    s32 i;
    u32 n;
    u8 *p;

    p = data_0203d06c;
    n = 0;
    for (i = 0; i < 0x3c; i++, p += 0x18)
    {
        if (func_02010150(i) != 0)
        {
            if (*(u8 *)p & 0x10) n++;
        }
    }
    return n;
}