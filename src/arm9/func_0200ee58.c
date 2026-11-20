#include "types.h"

extern u32 func_020101d0(u32 a0);

u32 func_0200ee58(u32 a0)
{
    switch (a0) {
    case 4:
        if (func_020101d0(0xf) != 0)
            return 1;
        break;
    case 7:
        if (func_020101d0(0x11) != 0)
            return 1;
        break;
    case 10:
        if (func_020101d0(0xb) != 0)
            return 1;
        break;
    case 12:
        if (func_020101d0(8) != 0)
            return 1;
        break;
    case 15:
        if (func_020101d0(0xc) != 0)
            return 1;
        break;
    }
    return 0;
}