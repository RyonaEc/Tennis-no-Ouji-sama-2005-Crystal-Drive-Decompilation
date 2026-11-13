#include "types.h"

extern u32 data_020475ec;
extern void func_02027138(u32, u32);
void func_0201bebc(u32 mask)
{
    if (mask == 0) return;
    func_02027138(mask, 0);
    data_020475ec &= ~mask;
}
