#include "types.h"

extern u32 data_020475ec;
extern void func_02027164(u32, u32);

int func_0201bef4(u32 mask)
{
    if (mask == 0) return 1;
    if (mask & data_020475ec) return 0;
    func_02027164(mask, 0);
    data_020475ec |= mask;
    return 1;
}