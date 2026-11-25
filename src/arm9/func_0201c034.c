#include "types.h"

extern u8 data_020475f0[];
extern u8 data_020475fc[];

extern void func_0201779c(u8 *node, u8 *base);
extern void func_020178e8(u8 *a0, u8 *a1);

void func_0201c034(u8 *n)
{
    u8 *r;

    if (*(u32 *)n != 0) {
        *(u32 *)*(u32 *)n = 0;
        *(u32 *)n = 0;
    }
    r = *(u8 **)(n + 4);
    func_0201779c(r, n);
    *(u32 *)(n + 4) = 0;
    if (*(u32 *)(n + 8) != 0) {
        func_020178e8(r + 0xc, *(u8 **)(n + 8));
        *(u32 *)(*(u32 *)(n + 8) + 0xc) = 0;
        *(u32 *)(n + 8) = 0;
    }
    func_0201779c(data_020475fc, n);
    func_020178e8(data_020475f0, n);
    *(u8 *)(n + 0x2c) = 0;
}