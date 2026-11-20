#include "types.h"

extern void func_0202250c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);
extern void func_02022e28(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);

void func_ov009_0204dc70(void)
{
    func_0202250c(0, 0, 0x7fff, 0x3f, 0);
    *(u32 *)0x4000580 = 0xbfff0000;
    func_02022e28(0x800, 0xddb, 0x1555, 0x1000, 0x190000, 0x1000, 1, 0);
    *(u32 *)0x400044c = 0;
}