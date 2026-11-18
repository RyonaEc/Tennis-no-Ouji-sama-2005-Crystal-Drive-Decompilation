#include "types.h"

u32 func_02020000(s32 *a, s32 *b)
{
    s64 sum = (s64)a[0] * b[0] + (s64)a[1] * b[1] + (s64)a[2] * b[2];

    return (u32)((sum + 0x800) >> 12);
}