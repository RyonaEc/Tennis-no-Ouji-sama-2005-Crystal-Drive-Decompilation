#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_020848b8(void);

void func_ov002_0208b9c4(void)
{
    u8 *rec = (u8 *)func_ov002_020848b8();
    u8 *base = (u8 *)data_ov002_02102070[0] + 0x4bc4;

    *(u32 *)rec |= 0x2e00;
    *(u16 *)(rec + 0x10) = *(u16 *)(base + 0x11a);
    *(u16 *)(rec + 0x1c) = 0;
    *(u16 *)(rec + 0x1e) = 0;
    *(u16 *)(rec + 0x12) = *(u16 *)(base + 0x11c);
    *(u16 *)(rec + 0x20) = (u16)((u32)-*(s16 *)(base + 0x40) & 0x1ff);
    *(u16 *)(rec + 0x22) = 0;
    *(u16 *)(rec + 0xe) = *(u16 *)(base + 0x11e);
    *(u16 *)(rec + 0x18) = (u16)((u32)-*(s16 *)(base + 0x40) & 0x1ff);
    *(u16 *)(rec + 0x1a) = 0;
}