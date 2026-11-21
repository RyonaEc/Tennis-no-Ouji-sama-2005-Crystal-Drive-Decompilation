#include "types.h"

extern s32 func_02027e98(u32, u32, u32);
extern void WaitByLoop(u32);

void func_0202dd60(u32 a0, u32 a1)
{
    if (func_02027e98(0xe, a0, 0) == 0)
        return;
    do {
        WaitByLoop(a1);
    } while (func_02027e98(0xe, a0, 0) != 0);
}