#include "types.h"

extern u8 *func_02018100(u8 *list, u8 *node);
extern u8 *func_020180a0(u32 *p, u16 v);
extern u8 *func_020180d0(u8 *list, u8 *node, u8 *prev);

typedef struct {
    u32 a;
    u32 b;
} Range;

s32 func_02017bfc(u8 **list, Range *bounds)
{
    Range r;
    u8 *node;
    u8 *prev;

    r = *bounds;
    prev = 0;
    for (node = *list; node != 0; node = *(u8 **)(node + 0xc)) {
        if ((u32)node < bounds->a) {
            prev = node;
            continue;
        }
        if ((u32)node == bounds->b) {
            r.b = *(u32 *)(node + 4) + (u32)(node + 0x10);
            func_02018100((u8 *)list, node);
        }
        break;
    }
    if (prev != 0) {
        u32 t = *(u32 *)(prev + 4) + (u32)(prev + 0x10);

        if (t == bounds->a) {
            r.a = (u32)prev;
            prev = func_02018100((u8 *)list, prev);
        }
    }
    if (r.b - r.a < 0x10)
        return 0;
    func_020180d0((u8 *)list, func_020180a0((u32 *)&r, 0x4652), prev);
    return 1;
}