#include "types.h"

extern u32 func_0200ec4c(u32);
extern void func_0200ec0c(void);

u32 func_0200eb4c(u32 a0)
{
    switch (a0 - 0xf)
    {
    case 3:
        return 0x3a;
    case 4:
        return 0x3b;
    case 5:
        return 0x3c;
    case 6:
        return 0x3d;
    case 7:
        return 0x3e;
    case 0:
        return 0x3f;
    }
    if (func_0200ec4c(a0) == 0x45) return 0x40;
    func_0200ec0c();
}