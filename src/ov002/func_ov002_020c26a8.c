#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

extern s32 func_ov002_0207a93c(void);

s32 func_ov002_020c26a8(u32 a0)
{
    u8 *e;
    u8 *r;
    u8 *base;
    u32 off;
    u32 *p;
    s32 done = 0;
    s32 ret = 0;

    base = B;
    off = a0 * 0x158;
    e = base + 0x2e4 + off;
    r = base + 0x274 + *(u8 *)(base + 0x2e6 + off) * 0x1c;
    p = (u32 *)(e + 0x90);
    if (func_ov002_0207a93c() == 1) {
        if (r[2] == 0xb || (u8)(r[2] + 0xf0) <= 1) {
            *p = 0;
            done = 1;
        }
        if (r[2] == 0 || r[2] == 0x20 || (u8)(r[2] + 0xff) <= 3) {
            if (*(u8 *)(base + 0x8c) == 1) {
                if (*(u32 *)(e + 0x4c) & 0x40000) {
                    *(u32 *)(e + 0xd4) |= 8;
                    ret = 1;
                }
            }
            done = 1;
            *p = 1;
        }
        if (done == 1) {
            if (*(u8 *)(base + a0 + 0x142) != 0) {
                *(u32 *)(e + 0xd4) |= 1;
                ret = 1;
            }
        }
    }
    return ret;
}