#include "types.h"

void func_ov004_020534f4(s32 *p, s16 *a, s16 *b)
{
    if (*p)
    {
        *a = *(s16 *)((u8 *)p + 0x30);
        *b = *(s16 *)((u8 *)p + 0x32);
    }
}
