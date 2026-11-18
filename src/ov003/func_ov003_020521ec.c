#include "types.h"

void func_ov003_020521ec(u8 *p, s32 a, s32 b)
{
    *(s16 *)(p + 6) = a >> 4;
    *(s16 *)(p + 0xe) = 0;
    *(s16 *)(p + 0x16) = 0;
    *(s16 *)(p + 0x1e) = b >> 4;
}
