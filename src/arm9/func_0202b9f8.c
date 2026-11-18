#include "types.h"

void func_0202b9f8(u8 *owner, u8 *node)
{
    u8 *t;

    if (owner == 0)
        return;
    t = *(u8 **)owner;
    if (t == 0) {
        *(u8 **)(node + 8) = 0;
        *(u8 **)owner = node;
        return;
    }
    while (*(u8 **)(t + 8) != 0)
        t = *(u8 **)(t + 8);
    *(u8 **)(node + 8) = *(u8 **)(t + 8);
    *(u8 **)(t + 8) = node;
}