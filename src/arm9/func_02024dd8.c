#include "types.h"

extern s32 func_02024ca8(u32, u32, u32, u32);
extern void func_02024b8c(void);

void func_02024dd8(u32 a0, u32 a1, u32 a2, u32 a3)
{
    if (func_02024ca8(a0, a1, a2, a3) <= 0) return;
    do
    {
        func_02024b8c();
    } while (func_02024ca8(a0, a1, a2, a3) > 0);
}