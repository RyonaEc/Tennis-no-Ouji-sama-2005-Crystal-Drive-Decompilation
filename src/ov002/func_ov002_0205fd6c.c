#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
void func_ov002_0205fd6c(u32 a0, u32 a1)
{
    u8 *b = B;
    u32 n = a0 * 3;
    u8 *s = b + 0x224 + n;
    u8 *d = b + 0x225 + n;
    u8 t = *(u8 *)s;
    *(u8 *)s = *(u8 *)(d + a1);
    *(u8 *)(d + a1) = t;
}
