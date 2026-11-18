#include "types.h"

void func_ov002_0208d8d0(u32 v, u32 k, u32 *dst, u32 *src, s32 flag)
{
    u32 i = 0;
    s32 n = flag == 0 ? 2 : 7;

    for (; i < n; i++)
    {
        u32 cur = src[i];

        if (cur == 0 || k == cur)
        {
            dst[i] = v;
            src[i] = k;
            return;
        }
    }
}