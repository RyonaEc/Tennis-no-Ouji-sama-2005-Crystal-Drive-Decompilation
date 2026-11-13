#include "types.h"

extern void func_0201abd0(u8 *);

void func_02019b2c(u8 *p)
{
    func_0201abd0(p);
    *(u32 *)(p + 0x30) = 0;
    *(u32 *)(p + 0x34) = 0;
    *(u32 *)(p + 0x38) = 1;
    *(u32 *)(p + 0x3c) = 1;
    *(u32 *)(p + 0x40) = 0;
    *(u32 *)(p + 0x44) = 0;
    *(u16 *)(p + 0x48) = 0;
    *(u16 *)(p + 0x4a) = 0;
}
