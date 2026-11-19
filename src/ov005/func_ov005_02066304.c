#include "types.h"

extern void func_02020d44(u32 a0);
extern void func_02020c50(u32 a0);
extern void func_ov005_0205526c(void);
extern void func_020245f4(void);
extern void func_02025c94(void *a0, u32 a1);
extern void func_02024498(void *a0, u32 a1, u32 a2);
extern void func_02024428(void);
extern void func_020243e0(void);
extern void func_0202436c(void *a0, u32 a1, u32 a2);
extern void func_02024314(void);
extern void func_ov005_02050ca8(u32 a0, void *a1);
extern u8 data_ov005_020c9744[];
extern u8 data_ov005_020c9740[];
extern u8 data_ov005_020c7eb0[];

s32 func_ov005_02066304(u32 a0)
{
    u8 *p = *(u8 **)data_ov005_020c9744;
    u8 *s = *(u8 **)data_ov005_020c9740;
    u16 i;
    u32 run1 = 0x2000;
    u32 run2 = 0x20;
    u8 *base = p + 0x50000;
    u32 size2;
    u16 j;

    if (a0 == 0) {
        func_02020d44(1);
        func_02020c50(0x40);
        func_ov005_0205526c();
        func_020245f4();
        for (i = 0; i < 0xa; i++) {
            u32 off = *(u32 *)(data_ov005_020c7eb0 + i * 16);
            u32 size = *(u32 *)(data_ov005_020c7eb0 + i * 16 + 8);
            u8 *dst = base + off;

            func_02025c94(dst, size);
            func_02024498(dst, run1, size);
            *(u32 *)(s + i * 4 + 0x734c) = size;
            run1 += size;
        }
        func_02024428();
        func_020243e0();
        for (j = 0; j < 0xa; j++) {
            u32 off = *(u32 *)(data_ov005_020c7eb0 + j * 16 + 4);
            u8 *dst;

            size2 = *(u32 *)(data_ov005_020c7eb0 + j * 16 + 12);
            dst = base + off;
            func_02025c94(dst, size2);
            func_0202436c(dst, run2, size2);
            *(u32 *)(s + j * 4 + 0x754c) = size2;
            run2 += size2;
        }
        func_02024314();
        *(u16 *)0x04000008 = (*(u16 *)0x04000008 & ~3) | 2;
    } else {
        *(u16 *)(s + 0x4654) = 0;
        *(u16 *)(s + 0x4650) = 0;
        *(s16 *)(s + 0x4652) = -0x20;
        func_ov005_02050ca8(0xc, s + 0x834c);
        func_ov005_02050ca8(0xc, s + 0x834c);
        func_ov005_02050ca8(0xc, s + 0x834c);
        func_ov005_02050ca8(0xc, s + 0x834c);
        func_ov005_02050ca8(0xc, s + 0x834c);
        func_ov005_02050ca8(0xc, s + 0x834c);
    }
    return 0;
}