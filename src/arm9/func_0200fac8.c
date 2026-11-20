#include "types.h"

extern u32 func_0200fe54(void);
extern void func_0200fdf4(u32, void *);

u32 func_0200fac8(u32 a0)
{
    u8 buf[0x10];

    if (func_0200fe54() == 0) return 0;
    func_0200fdf4(a0, buf);
    return (*(u16 *)(buf + 4) + *(u16 *)(buf + 6) + *(u16 *)(buf + 8)) != 0;
}