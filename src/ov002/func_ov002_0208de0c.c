#include "types.h"

void func_ov002_0208de0c(u32 a0, u32 *a1)
{
    s32 i;
    for (i = 0; i < 7; i++)
    {
        if (a1[i] == 0 || a1[i] == a0)
        {
            a1[i] = a0;
            return;
        }
    }
}
