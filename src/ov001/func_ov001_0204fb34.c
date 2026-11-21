#include "types.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *data_ov001_02051cc0;
extern void func_02026a04(u32 a, u8 *p, u32 b);

void func_ov001_0204fb34(void)
{
    volatile u32 v = 0xc0;

    func_02026a04(v, data_ov001_02051cc0 + 0x134c, 0x400);
    *(u32 *)(data_ov001_02051cc0 + 0x1c) = 0x7f;
}