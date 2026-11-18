#include "types.h"

void func_0202b9ac(u8 *owner, u8 *target)
{
    u8 *t;
    u8 *prev;

    if (owner == 0)
        return;
    t = *(u8 **)owner;
    prev = t;
    if (t == 0)
        return;
    for (;;) {
        if (t == target) {
            if (t == prev)
                *(u8 **)owner = *(u8 **)(t + 8);
            else
                *(u8 **)(prev + 8) = *(u8 **)(t + 8);
            return;
        }
        prev = t;
        t = *(u8 **)(t + 8);
        if (t == 0)
            return;
    }
}