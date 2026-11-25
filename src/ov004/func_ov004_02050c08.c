#include "types.h"

extern u8 data_ov004_020690a8[];
extern s32 func_ov004_02051448(u8 *p, u32 v);
extern void func_ov004_02052808(u32 a0, u8 *p);
extern void func_020269d0(u8 *src, u8 *dst, s32 count);
extern void func_ov004_02050cbc(void);

s32 func_ov004_02050c08(u8 *p, u16 a1, u16 a2, u16 a3, s16 a4)
{
    u8 *g = *(u8 **)data_ov004_020690a8;
    u8 *tbl = g + 0x38b4;

    if (p == 0)
        return 0;
    func_ov004_02051448(p, (u32)func_ov004_02050cbc);
    *(u16 *)(p + 0x9c) = a1;
    *(u16 *)(p + 0x9e) = a2;
    *(u16 *)(p + 0xa0) = a3;
    *(u16 *)(p + 0xa2) = (u16)a4;
    *(u16 *)(p + 0xa4) = 0;
    *(u16 *)(p + 0xa6) = 0;
    func_020269d0(*(u8 **)(p + 0x30) + a1 * 32, tbl + 0x60, 0x20);
    func_ov004_02052808(a3, p);
    return 1;
}