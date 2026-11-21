#include "types.h"

extern s32 func_02027e98(u32, u32, u32);
extern void WaitByLoop(u32);

void func_02030520(u32 a0)
{
    if (func_02027e98(0xd, a0, 0) == 0)
        return;
    do {
        WaitByLoop(1);
    } while (func_02027e98(0xd, a0, 0) != 0);
}