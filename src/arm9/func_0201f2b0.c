#include "types.h"

extern u8 data_02047f14[];
extern void func_0201f21c(u8 *a0);
extern s32 func_020298e4(u8 *a0);
extern s32 func_02026350(void);
extern void func_02026364(s32 a0);
extern s32 func_02017784(u8 *a0, u8 *a1);
extern void func_0201779c(u8 *a0, u8 *a1);
extern u32 func_0201f138(u32 a0);

void func_0201f2b0(u8 *a0)
{
    s32 *q = *(s32 **)(a0 + 0x12c);
    s32 flag;
    s32 f;
    u8 *n;
    u8 *t;

    if (q != 0) {
        *q = 0;
        *(s32 **)(a0 + 0x12c) = 0;
    }
    func_0201f21c(a0);
    if (*(s32 *)(a0 + 0x68) & 0x10)
        flag = 1;
    else
        flag = 0;
    if (flag != 0)
        func_020298e4(a0 + 0x5c);
    f = func_02026350();
    n = (u8 *)func_02017784(data_02047f14, 0);
    if (n != 0) {
        do {
            t = (u8 *)func_02017784(data_02047f14, n);
            if (*(s32 *)(n + 8) == (s32)a0) {
                func_0201779c(data_02047f14, n);
                func_0201f138((u32)n);
            }
            n = t;
        } while (t != 0);
    }
    func_02026364(f);
    *(s32 *)(a0 + 0x108) &= ~1;
    *(s32 *)(a0 + 0x108) &= ~4;
    *(s32 *)(a0 + 0x108) &= ~2;
}