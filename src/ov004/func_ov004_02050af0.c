#include "types.h"

extern s32 func_ov004_02051448(u8 *p, u32 v);
extern void func_ov004_02053014(u32 a0, u32 a1, u8 *p);
extern void func_ov004_02052808(u32 a0, u8 *p);
extern void func_ov004_02050b70(void);

s32 func_ov004_02050af0(u8 *p, u16 a1, u16 a2, u16 a3, s16 a4)
{
    if (p == 0)
        return 0;
    func_ov004_02051448(p, (u32)func_ov004_02050b70);
    *(u16 *)(p + 0x9c) = a1;
    *(u16 *)(p + 0x9e) = a2;
    *(u16 *)(p + 0xa0) = a3;
    *(u16 *)(p + 0xa2) = (u16)a4;
    *(u16 *)(p + 0xa4) = 0;
    *(u16 *)(p + 0x3a) = a3;
    func_ov004_02053014(a1, 1, p);
    func_ov004_02052808(a3, p);
    return 1;
}