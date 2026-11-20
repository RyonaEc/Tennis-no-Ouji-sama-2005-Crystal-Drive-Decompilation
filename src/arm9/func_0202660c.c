#include "types.h"

extern void func_020263cc(void);

void func_0202660c(u32 a0, u32 a1)
{
    s32 i;
    u8 *p = (u8 *)0x040000b8;
    u32 v;

    for (i = 0; i < 3; i++, p += 0xc)
    {
        if (i == (s32)a0) continue;
        v = *(u32 *)p;
        if ((v & 0x80000000) == 0) continue;
        v = v & 0x38000000;
        if (v == a1) continue;
        if (v == 0x08000000 && a1 == 0x10000000) continue;
        if (v == 0x10000000 && a1 == 0x08000000) continue;
        if (v == 0x18000000 || v == 0x20000000 || v == 0x28000000 ||
            v == 0x30000000 || v == 0x38000000 || v == 0x08000000 ||
            v == 0x10000000)
            func_020263cc();
    }
}