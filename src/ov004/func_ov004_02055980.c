#include "types.h"

s32 func_ov004_02055980(s32 *p, s32 *out)
{
    if (*p)
        return 0;
    *out = *(s16 *)((u8 *)p + 0x12);
    return 1;
}
