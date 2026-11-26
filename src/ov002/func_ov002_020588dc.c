#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_0203b0c0(u32 a0, u32 a1);
extern void func_ov002_020781d8(u32 a0);
extern u32 func_ov002_0204d350(s32, s32, s32);

void func_ov002_020588dc(u32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *e = (u8 *)(0x158 * a0 + (base + 0x2e4));
    s32 t = *(s16 *)(e + 0x66);
    u32 a = *(u32 *)(e + 0x90);
    u32 b = *(u32 *)(e + 0x9c);
    u32 c, d;

    if (t > *(u16 *)(e + 0x8c))
        t = *(u16 *)(e + 0x8c);
    *(u32 *)(e + 4) = a + func_0203b0c0(t * (b - a), *(u16 *)(e + 0x8c));
    c = *(u32 *)(e + 0x94);
    d = *(u32 *)(e + 0xa0);
    *(u32 *)(e + 8) = c + func_0203b0c0(t * (d - c), *(u16 *)(e + 0x8c));
    if (*(s16 *)(e + 0x66) != *(u16 *)(e + 0x8c)) {
        s16 *p = (s16 *)(e + 0x66);

        *p = *p + 1;
        return;
    }
    func_ov002_020781d8(a0 & 0xff);
    func_ov002_0204d350(*(u8 *)(e + 2), *(u8 *)(e + 1), 0);
}