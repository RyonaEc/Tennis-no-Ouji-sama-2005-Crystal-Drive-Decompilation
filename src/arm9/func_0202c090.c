#include "types.h"

extern u8 data_0204acb8[];
extern u32 func_0202c3e0(void);
extern void func_0202bd70(u32);

u32 func_0202c090(u32 a0, u32 a1, u32 a2)
{
    if (func_0202c3e0() == 0) return 1;
    *(u32 *)(data_0204acb8 + 4) = a1;
    *(u32 *)(data_0204acb8 + 8) = a2;
    func_0202bd70(0x02006300 | ((a0 >> 16) & 0xff));
    func_0202bd70((a0 & 0xffff) | 0x01010000);
    return 0;
}