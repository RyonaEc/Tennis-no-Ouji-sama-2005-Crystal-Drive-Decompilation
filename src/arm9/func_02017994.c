#include "types.h"

void func_02017994(u8 *p, u16 v)
{
    *(u32 *)p = 0;
    *(u32 *)(p + 4) = 0;
    *(u16 *)(p + 8) = 0;
    *(u16 *)(p + 0xa) = v;
}
