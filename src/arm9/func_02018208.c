#include "types.h"

extern void func_02018318(u32);
extern void func_020182e8(u32);

void func_02018208(u32 a0, u32 a1)
{
    if (a1 & 1)
        func_02018318(a0);
    if (a1 & 2)
        func_020182e8(a0);
}