#include "types.h"

extern u8 data_02042a68[];
extern void func_02026a04(u32, u32, u32);
extern void func_020101f8(u32);
extern void func_0201038c(u8 *);
extern void func_020102f0(u32, u8 *);
extern void func_02010340(u32);

void func_02006324(void)
{
    u8 a[8];
    u8 b[8];
    volatile u32 z;
    s32 i;
    u32 j;

    z = 0;
    func_02026a04(z, *(u32 *)((u8 *)*(u32 **)(data_02042a68 + 0x10) + 0x158), 0x200);
    for (i = 0; i < 0xa; i++) func_020101f8(i);
    a[0] = 1;
    a[1] = 0;
    a[2] = 1;
    a[3] = 3;
    a[4] = 1;
    a[5] = 0;
    a[6] = 0;
    a[7] = 0;
    func_0201038c(a);
    for (j = 0; j < 3; j = (j + 1) & 0xff)
    {
        b[4] = j;
        b[3] = b[4];
        b[2] = b[3];
        b[1] = b[2];
        b[0] = b[1];
        func_020102f0(j, b);
    }
    func_02010340(0);
}