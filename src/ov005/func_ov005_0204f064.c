#include "types.h"

u32 func_ov005_0204f064(u32 a0)
{
    switch (a0 & 0xc000c000) {
    case 0xc0000000:
        return 0x40;
    case 0x80000000:
        return 0x10;
    case 0x80004000:
        return 8;
    case 0x80008000:
        return 8;
    case 0x40004000:
        return 4;
    case 0x40008000:
        return 4;
    case 0x40000000:
        return 4;
    case 0x8000:
        return 2;
    case 0x4000:
        return 2;
    case 0xc0004000:
        return 0x20;
    case 0xc0008000:
        return 0x20;
    case 0x0:
        return 1;
    }
    return 0;
}