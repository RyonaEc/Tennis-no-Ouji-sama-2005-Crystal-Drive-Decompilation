#include "types.h"

extern u8 data_02047608[];
extern void func_0201bfd8(void);
extern u8 *func_0201da30(u32 a0, u32 a1, void *a2, u32 a3, u32 a4);
extern s32 func_0201db68(u8 *a0, u32 a1);
extern void func_020178e8(u8 *a0, u8 *a1);

s32 func_0201c59c(u32 a0, u32 a1, u32 a2)
{
    u8 *p;
    s32 r;

    p = func_0201da30(a1, a2 + 0x14, (void *)func_0201bfd8, 0, 0);
    if (p == 0)
        return 0;
    *(u32 *)(p + 0xc) = 0;
    *(u32 *)(p + 0x10) = a0;
    *(u32 *)(p + 8) = 0;
    r = func_0201db68(p + 0x14, a2);
    if (r == 0)
        return 0;
    *(u32 *)(p + 8) = r;
    func_020178e8(data_02047608 + a0 * 0x1c + 0xc, p);
    return 1;
}