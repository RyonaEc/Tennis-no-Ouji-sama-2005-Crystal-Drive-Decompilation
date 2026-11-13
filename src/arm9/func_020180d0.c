#include "types.h"

u8 *func_020180d0(u8 *list, u8 *node, u8 *prev)
{
    u8 *next;

    *(u32 *)(node + 8) = (u32)prev;
    if (prev != 0) {
        next = *(u8 **)(prev + 0xc);
        *(u32 *)(prev + 0xc) = (u32)node;
    } else {
        next = *(u8 **)list;
        *(u32 *)list = (u32)node;
    }
    *(u32 *)(node + 0xc) = (u32)next;
    if (next != 0) {
        *(u32 *)(next + 8) = (u32)node;
    } else {
        *(u32 *)(list + 4) = (u32)node;
    }
    return node;
}
