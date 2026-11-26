#include "types.h"

extern void func_ov004_02053218(u8 *p, s32 *a, s32 *b);
extern void func_02025c94(u32 a0, u32 a1);
extern s32 func_020186a0(u32 a0, u32 a1, u32 a2, u32 a3);

u32 func_ov004_0205312c(u8 *p)
{
    s32 a = 0;
    s32 b = 0;

    func_ov004_02053218(p, &a, &b);
    func_02025c94(*(u32 *)(p + 0x10), *(u32 *)(p + 0x28));
    switch (*(u32 *)(p + 0x14)) {
    case 0:
        if (a != 0) {
            func_020186a0(0xe, *(u32 *)(p + 0x68) << 5, *(u32 *)(p + 0x10),
                          *(u32 *)(p + 0x28));
        }
        if (b != 0) {
            func_020186a0(0x1e, *(u32 *)(p + 0x68) << 5, *(u32 *)(p + 0x10),
                          *(u32 *)(p + 0x28));
        }
        break;
    case 1:
        if (a != 0) {
            func_020186a0(0x10, *(u32 *)(p + 0x68) << 7, *(u32 *)(p + 0x10),
                          *(u32 *)(p + 0x28));
        }
        if (b != 0) {
            func_020186a0(0x20, *(u32 *)(p + 0x68) << 7, *(u32 *)(p + 0x10),
                          *(u32 *)(p + 0x28));
        }
        break;
    }
    return 1;
}
