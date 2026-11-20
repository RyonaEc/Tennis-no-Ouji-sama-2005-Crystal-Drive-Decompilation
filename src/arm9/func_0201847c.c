#include "types.h"

extern void func_02029c8c(void *);
extern u32 func_02029930(void *, u32);
extern u32 func_020298e4(void *);

u32 func_0201847c(u32 a0)
{
    u8 buf[0x44];
    u32 r = 0;

    func_02029c8c(buf);
    if (func_02029930(buf, a0) != 0)
    {
        u8 *p = *(u8 **)(buf + 8);
        r = *(u32 *)(p + 0x58) + *(u32 *)(buf + 0x20);
        func_020298e4(buf);
    }
    return r;
}