#include "types.h"

extern u32 func_0203b578(void);

u32 func_0203b5dc(u32 a0, u8 *a1, u32 *a2)
{
    u32 i;
    u32 n = *a2;

    for (i = 0; i < n; i++)
    {
        a1[i] = func_0203b578();
        if (a1[i] == 0xd || a1[i] == 0xa)
        {
            *a2 = i + 1;
            break;
        }
    }
    return 0;
}