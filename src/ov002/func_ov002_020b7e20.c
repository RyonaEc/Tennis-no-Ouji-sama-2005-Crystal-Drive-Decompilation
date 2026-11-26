#include "types.h"

#include "types.h"

typedef struct { u32 w[3]; } S3;

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02078208(s32 a0);

s32 func_ov002_020b7e20(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 v = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u8 *f = base + 0x1824;
    S3 t;
    s32 ret = 0;

    if ((*(u32 *)(e + 0xd8) & 8) != 0) {
    *(u16 *)(e + 0x66) = ret;
    t = *(S3 *)(f + 0x24);
    *(u16 *)(e + 0x8e) = *(u16 *)(f + 0xac);
    *(u16 *)(e + 0x8c) = *(u16 *)(f + 0xac);
    *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
    *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
    *(u32 *)(e + 0x9c) = t.w[0];
    *(u32 *)(e + 0xa0) = t.w[1];
    func_ov002_02078208(a0);
    func_ov002_0204d270(v, *(u8 *)(e + 1), 0x37);
    *(u8 *)(e + 0xd1) = 4;
    ret = 1;
    }

    return ret;

}

