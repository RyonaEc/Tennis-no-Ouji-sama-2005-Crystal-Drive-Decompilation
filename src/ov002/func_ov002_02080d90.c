#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_02085fd4(u32 a0);
extern void func_ov002_020880f0(void);
extern void func_ov002_0207be74(void);
extern void func_ov002_02089f2c(void);

void func_ov002_02080d90(void)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *g = base + 0x3654;
    u16 i;
    u32 t;
    u8 b;

    if (func_ov002_02085fd4(2) == 1)
        return;
    for (i = 0; i < 2; i++) {
        if (((s32 *)g)[i] < 0x320000)
            ((s32 *)g)[i] = 0x320000;
    }
    if (!(*(u32 *)(g + 0x10) & 4))
        return;
    t = *(u32 *)(g + 0);
    *(u32 *)(g + 0) = *(u32 *)(g + 4);
    *(u32 *)(g + 4) = t;
    t = *(u32 *)(g + 0x28);
    *(u32 *)(g + 0x28) = *(u32 *)(g + 0x2c);
    *(u32 *)(g + 0x2c) = t;
    b = *(u8 *)(g + 0x30);
    *(u8 *)(g + 0x30) = *(u8 *)(g + 0x31);
    *(u8 *)(g + 0x31) = b;
    t = *(u32 *)(g + 0x40);
    *(u32 *)(g + 0x40) = *(u32 *)(g + 0x44);
    *(u32 *)(g + 0x44) = t;
    t = *(u32 *)(g + 0x48);
    *(u32 *)(g + 0x48) = *(u32 *)(g + 0x4c);
    *(u32 *)(g + 0x4c) = t;
    b = *(u8 *)(g + 0x50);
    *(u8 *)(g + 0x50) = *(u8 *)(g + 0x51);
    *(u8 *)(g + 0x51) = b;
    *(u32 *)(g + 0x10) &= ~4;
    func_ov002_020880f0();
    func_ov002_0207be74();
    func_ov002_02089f2c();
}