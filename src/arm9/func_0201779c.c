#include "types.h"

void func_0201779c(u8 *node, u8 *base)
{
    u16 k = *(u16 *)(node + 0xa);
    u32 *l = (u32 *)(base + k);

    if (l[0] == 0)
        *(u32 *)(node + 0) = l[1];
    else
        *(u32 *)(l[0] + k + 4) = l[1];
    if (l[1] == 0)
        *(u32 *)(node + 4) = l[0];
    else
        *(u32 *)(l[1] + *(u16 *)(node + 0xa)) = l[0];
    l[0] = 0;
    l[1] = 0;
    {
        u16 *cnt = (u16 *)(node + 8);

        *cnt = *cnt - 1;
    }
}