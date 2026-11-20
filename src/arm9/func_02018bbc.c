#include "types.h"

extern u64 func_0203b2cc(u32 a0, u32 a1);

u32 func_02018bbc(u32 a0, u32 a1)
{
    u64 v;

    v = func_0203b2cc(a1 + 1, *(u32 *)(a0 + 4));
    return (u16)(v >> 32);
}