#include "types.h"

extern u8 data_ov005_020c9740[];
extern void func_020269b8(u16 v, u8 *dst, s32 count);
extern void func_02026a04(u32 data, void *destp, u32 size);
extern void func_ov005_0204f5a8(u16 v);
extern void func_ov005_02051b90(void);
extern void VBlankIntrWait(void);
extern void func_020202b4(void);

s32 func_ov005_02050974(void)
{
    u8 *base = *(u8 **)data_ov005_020c9740;
    {
        volatile u16 t = 0;
        func_020269b8(t, base, 4);
    }
    {
        volatile u32 z = 0;
        func_02026a04(z, (void *)0x06000000, 0x8000);
    }
    {
        volatile u32 z = 0;
        func_02026a04(z, (void *)0x06400000, 0x8000);
    }
    {
        volatile u32 z = 0;
        func_02026a04(z, (void *)0x06500000, 0x200);
    }
    {
        volatile u32 z = 0;
        func_02026a04(z, (void *)0x06500200, 0x200);
    }
    *(u16 *)(base + 0xe0) = 0;
    func_ov005_0204f5a8(*(u16 *)(base + 0xe0));
    *(u16 *)(base + 0x4332) = 0x0a;
    *(u16 *)(base + 0x4332) = 0x10;
    *(u16 *)(base + 0x4332) = 0x26;
    *(u16 *)(base + 0x4332) = 0x6f;
    *(u16 *)(base + 0x4332) = 0x74;
    *(u16 *)(base + 0xe0) = 0;
    *(u16 *)(base + 0x4332) = 0x96;
    *(u16 *)(base + 0x4332) = 0x99;
    *(u16 *)(base + 0x4332) = 0x9d;
    func_ov005_02051b90();
    *(u16 *)(base + 0x130) = 5;
    *(u16 *)(base + 0x130) = 6;
    *(u16 *)(base + 0x4388) = 0;
    *(u16 *)(base + 0x32) = 0;
    VBlankIntrWait();
    func_020202b4();
    *(u16 *)(base + 0x14) = 1;
    return 0;
}