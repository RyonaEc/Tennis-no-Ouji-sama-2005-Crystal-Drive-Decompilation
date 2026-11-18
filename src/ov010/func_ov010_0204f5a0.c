#include "types.h"

extern u8 data_ov010_0205e500[];
extern void func_02021af0(void *, s32, s32);
#define G (*(u8 **)data_ov010_0205e500)

void func_ov010_0204f5a0(s32 v)
{
    if (v == G[0x3b1])
        return;
    G[0x3b0] = 0x11;
    G[0x3b2] = v;
    func_02021af0((void *)0x04001050, 4, -16);
}