#include "types.h"

extern void func_0201bebc(u32);

void func_0201cdb8(u8 *p)
{
    if (*(u32 *)(p + 0x44) == 0)
        return;
    func_0201bebc(*(u32 *)(p + 0x44));
    *(u32 *)(p + 0x44) = 0;
    *(u32 *)(p + 0x48) = 0;
}
