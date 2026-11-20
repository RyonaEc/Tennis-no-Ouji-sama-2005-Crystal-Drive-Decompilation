#include "types.h"

extern u8 data_02047988[];
extern void func_0201c2d4(u8 *, u32);

void func_0201c548(u32 a0)
{
    s32 i;
    u8 *p = data_02047988;

    for (i = 0; i < 0x10; i++, p += 0x44) {
        if (p[0x2c])
            func_0201c2d4(p, a0);
    }
}