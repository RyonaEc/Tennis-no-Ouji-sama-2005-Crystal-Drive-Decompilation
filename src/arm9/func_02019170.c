#include "types.h"

typedef void (*FN_02019170)(u32 a0, u32 a1, u32 a2);

extern u32 data_02042340[];
extern u32 func_020195c0(u32 a0, u32 a1);

void func_02019170(u32 a0, u32 a1, u32 a2)
{
    u32 dst;
    u16 n;
    u16 src;
    u16 i;
    FN_02019170 fn;

    dst = func_020195c0(a0, a1);
    n = (u16)(a2 - a1 + 1);
    fn = (FN_02019170)data_02042340[a0];
    src = (u16)(a1 << 5);
    i = 0;
    if (n == 0)
        return;
    do {
        fn(dst + 6, (u32)src + 6, 2);
        fn(dst + 0xe, (u32)src + 0xe, 2);
        fn(dst + 0x16, (u32)src + 0x16, 2);
        fn(dst + 0x1e, (u32)src + 0x1e, 2);
        src = (u16)(src + 0x20);
        dst = dst + 0x20;
        i = i + 1;
    } while (i < n);
}