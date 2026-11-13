#include "types.h"

void func_02022328(u32 *p, u32 v)
{
    if (v != 0) {
        if ((*p & 3) != 0) *(u32 *)((u8 *)p + 0x10) = 1;
    }
    *(u8 *)*p = v;
}
