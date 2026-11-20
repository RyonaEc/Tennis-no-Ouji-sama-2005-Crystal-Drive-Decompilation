#include "types.h"

extern void func_0202234c(u32 *);

void func_0201ab44(u8 *a0, u8 *a1)
{
    u32 buf[13];

    *(u32 *)(a0 + 0x1c) = (u32)a1;
    buf[0] = *(u32 *)a1;
    buf[1] = *(u32 *)(a1 + 4);
    buf[2] = 0;
    buf[3] = *(u32 *)(a1 + 8);
    buf[4] = *(u32 *)(a1 + 0xc);
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
    buf[8] = 0x1000;
    buf[9] = *(u32 *)(a1 + 0x10);
    buf[10] = *(u32 *)(a1 + 0x14);
    buf[11] = *(u32 *)(a0 + 0x24);
    func_0202234c(buf);
}