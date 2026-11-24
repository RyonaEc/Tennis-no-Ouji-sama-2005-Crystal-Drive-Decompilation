#include "types.h"

extern u8 *data_ov010_0205e500;
extern void func_02026a04(u32 a0, u32 a1, u32 a2);

void func_ov010_0204ee94(void)
{
    volatile u32 s0;
    volatile u32 s1;
    u8 *p;
    u32 j;

    s0 = 0xc0;
    p = data_ov010_0205e500;
    j = *(u32 *)(p + 0x1fb8);
    func_02026a04(s0, (u32)(p + 0x1fdc + (j << 10)), 0x400);
    s1 = 0xc0;
    func_02026a04(s1, (u32)(data_ov010_0205e500 + 0x27dc + (j << 10)), 0x400);
    *(u32 *)(data_ov010_0205e500 + 0x1fcc + j * 4) = 0;
    *(u32 *)(data_ov010_0205e500 + 0x1fc4 + j * 4) = *(u32 *)(data_ov010_0205e500 + 0x1fcc + j * 4);
    *(u32 *)(data_ov010_0205e500 + 0x1fc0) = 0x7f;
    *(u32 *)(data_ov010_0205e500 + 0x1fbc) = *(u32 *)(data_ov010_0205e500 + 0x1fc0);
    *(u32 *)(data_ov010_0205e500 + 0x1fd4 + j * 4) = 0x1f;
}