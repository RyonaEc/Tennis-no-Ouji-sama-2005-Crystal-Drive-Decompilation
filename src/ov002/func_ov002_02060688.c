#include "types.h"

typedef struct {
    u32 v;
    u8 c;
} Buf;

extern u32 data_ov002_02102070[];
extern void func_0200fc58(s32 a0);
extern void func_020103f8(Buf *b);
extern u32 func_020052ec(u16 a0, u16 a1);

void func_ov002_02060688(u32 a0, u32 a1, u32 a2)
{
    u32 base = data_ov002_02102070[0];
    u32 p = base + 0x47d0;
    u32 id;

    if (*(u32 *)(base + 0x4000 + 0xbc8) & 4) {
        if (a2 == 0)
            func_0200fc58(*(u32 *)(base + (*(u8 *)(base + 0x106) << 2) + 0xd4));
        else
            func_0200fc58(*(u32 *)(p + (*(u8 *)(base + 0x106) << 2) + 8));
    }
    if (*(u8 *)(base + 0xbf) == 0) {
        Buf b;

        b.v = *(u32 *)(base + 8);
        b.c = *(u8 *)(base + 0xc);
        func_020103f8(&b);
    }
    func_020052ec((u16)a0, (u16)a1);
}