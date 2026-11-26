#include "types.h"

extern u8 data_ov005_020c9740[];
extern u8 data_ov005_020c9744[];
extern void func_02025c94(u32 a0, u32 a1);
extern void func_020240b4(s32 a0, s32 a1, s32 a2);
extern void func_020269d0(u8 *a0, u8 *a1, s32 a2);
extern u32 func_02023858(u32 a0, u32 a1, u32 a2);
extern u32 func_02023b58(u32 a0, u32 a1, u32 a2);

s32 func_ov005_020552fc(void)
{
    u16 *src;
    u8 *buf;
    u16 j;
    u8 *base;
    u16 i;
    u16 tmp;
    u16 *arr;

    buf = *(u8 **)data_ov005_020c9740;
    arr = (u16 *)(buf + 0x6a78);
    tmp = 0;
    base = *(u8 **)data_ov005_020c9744;
    *(u16 *)base = 0;
    func_02025c94((u32)base, 0x100);
    func_020240b4((s32)base, 0, 0x100);
    func_020269d0((u8 *)&tmp, (u8 *)0x05000400, 2);
    func_02025c94((u32)(0x100 + base), 0xb980);
    func_02023858((u32)((u16 *)base + 0x80), 0x1900, 0xb980);
    src = (u16 *)(0xba80 + base);

    for (j = 0; j < 0x20; j++) {
        for (i = 0; i < 0x20; i++)
            arr[j * 0x20 + i] = 0;
    }
    for (j = 0; j < 0x18; j++) {
        for (i = 0; i < 0x20; i++)
            arr[j * 0x20 + i] = src[j * 0x20 + i] + 100;
    }
    func_02025c94((u32)arr, 0x600);
    func_02023b58((u32)arr, 0, 0x600);
    return 0;
}