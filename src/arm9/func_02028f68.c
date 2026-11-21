#include "types.h"

extern void func_020296dc(void);
extern void func_020296b0(void);

u32 func_02028f68(u32 *p, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7)
{
    p[7] = a1;
    p[9] = a3;
    p[0xc] = a2;
    p[8] = p[0xc];
    p[0xb] = a5;
    p[0xd] = a4;
    p[0xa] = p[0xd];
    p[0xf] = a6 != 0 ? a6 : (u32)func_020296dc;
    p[0x10] = a7 != 0 ? a7 : (u32)func_020296b0;
    p[0x11] = p[0xf];
    p[0xe] = 0;
    p[4] = p[4] | 2;
    return 1;
}