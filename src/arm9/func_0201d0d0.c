#include "types.h"

extern u8 data_02047eb0[];
extern void func_0201fc1c(u8 *);
extern u32 func_0201fc04(u8 *);
extern void func_0201cfd4(void);
extern s32 func_0201fc3c(u8 *);
extern void func_02027104(u32, s32, u32);

void func_0201d0d0(void)
{
    u8 *p = data_02047eb0;
    u8 *q;
    s32 v;

    if (*(u32 *)p == 0) return;
    if (*(u32 *)(p + 4) != 0) return;
    q = p + 0x38;
    func_0201fc1c(q);
    if (*(u32 *)(p + 0x48) != 0 && func_0201fc04(q) != 0)
    {
        func_0201cfd4();
        return;
    }
    v = func_0201fc3c(q) >> 8;
    if (v == *(s32 *)(p + 0x4c)) return;
    func_02027104(*(u32 *)(p + 0x20), v, 0);
    *(s32 *)(p + 0x4c) = v;
}