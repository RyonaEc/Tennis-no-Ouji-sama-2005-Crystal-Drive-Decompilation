#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02078208(s32 a0);

u32 func_ov002_020b5f08(s32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 off = a0 * 0x158;
    u32 t = *(u8 *)(base + off + 0x2e6);
    u8 *e = base + 0x2e4 + off;
    u32 r = 0;

    if (*(u32 *)(e + 0xd8) & 1) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x6a) = 0;
        *(u32 *)(e + 0x98) = *(u32 *)(e + 0xc);
        *(u32 *)(e + 0xa4) = 0x100000;
        *(s32 *)(e + 0xb0) = *(s32 *)(e + 0xa4) * 2 / 36;
        *(s32 *)(e + 0xbc) = -(*(s32 *)(e + 0xa4) * 2) / 1296;
        *(s32 *)(e + 0xc8) = -(*(s32 *)(e + 0xa4) * 2) / 324;
        func_ov002_0204d270(t, *(u8 *)(e + 1), 0x2d);
        func_ov002_02078208(a0);
        *(u8 *)(e + 0xd1) = 1;
        r = 1;
    }
    return r;
}