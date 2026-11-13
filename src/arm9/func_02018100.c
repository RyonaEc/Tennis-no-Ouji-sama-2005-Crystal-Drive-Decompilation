#include "types.h"

u8 *func_02018100(u8 *list, u8 *node)
{
    u8 *prev = *(u8 **)(node + 8);
    u8 *next = *(u8 **)(node + 0xc);

    if (prev != 0) {
        *(u32 *)(prev + 0xc) = (u32)next;
    } else {
        *(u32 *)list = (u32)next;
    }
    if (next != 0) {
        *(u32 *)(next + 8) = (u32)prev;
    } else {
        *(u32 *)(list + 4) = (u32)prev;
    }
    return prev;
}
