#include "types.h"

extern u32 func_02018ba4(u8 *p);
extern u32 func_02018bbc(u8 *p, u32 v);

u32 func_02018818(u8 *p)
{
    if (func_02018ba4(p))
        return 0;
    *(u16 *)(p + 0xa) = func_02018bbc(p, *(u16 *)(p + 0xa));
    *(u16 *)(p + 0xc) += 1;
    return 1;
}