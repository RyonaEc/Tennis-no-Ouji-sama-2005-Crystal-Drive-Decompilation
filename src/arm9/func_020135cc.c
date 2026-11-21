#include "types.h"

extern u8 data_02042a68[];
extern u16 func_02017360(void);

void func_020135cc(u32 a0, u32 a1)
{
    u8 *p = (u8 *)(*(u32 *)(data_02042a68 + 0x14));
    u8 *q = *(u8 **)(p + 0x104);
    u8 *r;

    q[1] = a0;
    r = q + 2;
    *(u16 *)(r + 2) = a1;
    *(u16 *)r = func_02017360();
}