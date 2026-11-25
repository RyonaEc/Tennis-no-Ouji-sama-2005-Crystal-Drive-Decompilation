#include "types.h"

typedef struct {
    u8 type;
    u8 f1;
    u8 sel;
    u8 pad3[0xd1];
    u32 flags;
    u8 tail[0x80];
} Slot;

extern u32 data_ov002_02102070[];
extern u8 data_ov002_020fce60[];
extern s32 func_ov002_0207fdd0(s32 a0);
extern u32 func_ov002_0207fcf0(u32 a0, u32 a1, u32 a2);

void func_ov002_0207fc50(u32 idx)
{
    Slot *s = &((Slot *)((u8 *)data_ov002_02102070[0] + 0x2e4))[idx];
    s32 n = data_ov002_020fce60[s->type];
    u32 v;
    s32 i;

    v = func_ov002_0207fdd0(((u32)s->sel) >> 1);
    i = 0;
    if (n <= 0)
        return;
    do {
        u32 bit = 1 << i;

        if (s->flags & bit) {
            if (func_ov002_0207fcf0(v, s->type, i) == 0)
                s->flags &= ~bit;
        }
        i++;
    } while (i < n);
}