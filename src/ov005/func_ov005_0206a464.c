#include "types.h"

extern u8 data_ov005_020c9740[];
extern u8 data_ov005_020790a0[];
extern u8 data_ov005_020c9744[];
extern void func_02021280(s32);
extern void func_020240b4(u8 *, u32, u32);
extern void func_02025c94(u8 *, u32);
extern void func_020239d8(u8 *, u32, u32);
extern void func_020269d0(u8 *, u32, u32);
extern void func_02023cd8(u8 *, u32, u32);
extern void func_ov005_020552fc(s32, s32);

s32 func_ov005_0206a464(s32 mode)
{
    u8 *base = *(u8 **)data_ov005_020c9740;
    u8 *src = *(u8 **)data_ov005_020c9744;
    u16 *dst = (u16 *)(base + 0x6a78);
    u16 *stbl;
    s32 i;
    s32 j;

    switch (mode) {
    case 0:
        func_02021280(2);
        *(volatile u16 *)0x0400000a = (*(volatile u16 *)0x0400000a & 0x43) | 0x1f10;
        *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & 0x43) | 0x1d90;
        *(volatile u16 *)0x0400000c = (*(volatile u16 *)0x0400000c & 0x43) | 0x5a00;
        *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & ~3) | 3;
        *(volatile u16 *)0x0400000a = *(volatile u16 *)0x0400000a & ~3;
        *(u16 *)(base + 0x6a5c) = 0;
        *(u16 *)(base + 0x6a5e) = 0;
        *(volatile u32 *)0x0400001c = 0;
        *(volatile u32 *)0x04000018 = 0;
        *(volatile u32 *)0x04000014 = (*(u16 *)(base + 0x6a5c) & 0x1ff)
                                    | ((*(u16 *)(base + 0x6a5e) << 16) & 0x1ff0000);
        break;
    case 1:
        func_020240b4(src + 0x204c0, 0x1e0, 0x20);
        func_02025c94(src + 0x204e0, 0x260);
        func_020239d8(src + 0x204e0, 0x40, 0x260);
        func_02025c94((u8 *)data_ov005_020790a0, 0x20);
        func_020269d0((u8 *)data_ov005_020790a0, 0x6010000, 0x40);
        stbl = (u16 *)(src + 0x20740);
        for (i = 0; i < 0x20; i++) {
            for (j = 0; j < 0x20; j++) {
                if (i < 3)
                    dst[j + i * 0x20] = stbl[j + i * 0x20] + 0xf002;
                else
                    dst[j + i * 0x20] = 0;
            }
        }
        func_02025c94((u8 *)dst, 0x800);
        func_02023cd8((u8 *)dst, 0, 0x800);
        if (*(u16 *)(base + 0x4388) < 0x1f4)
            func_ov005_020552fc(0, 0);
        break;
    }
    return 0;
}