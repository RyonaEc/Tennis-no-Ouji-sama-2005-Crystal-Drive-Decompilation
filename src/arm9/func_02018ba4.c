#include "types.h"

s32 func_02018ba4(u8 *p)
{
    return *(u16 *)(p + 0xc) == *(u32 *)(p + 4);
}
