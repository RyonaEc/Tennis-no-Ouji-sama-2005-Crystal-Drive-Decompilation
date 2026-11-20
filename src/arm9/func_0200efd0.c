#include "types.h"

extern u32 func_020101d0(u32);

u32 func_0200efd0(s32 a0)
{
    switch (a0)
    {
    case 0xf:
        if (func_020101d0(3) != 0) return 1;
        break;
    case 0x1f:
        if (func_020101d0(2) != 0) return 1;
        break;
    case 0x2e:
        if (func_020101d0(1) != 0) return 1;
        break;
    case 0x30:
        if (func_020101d0(0) != 0) return 1;
        break;
    }
    return 0;
}