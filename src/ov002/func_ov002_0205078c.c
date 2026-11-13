#include "types.h"

s32 func_ov002_0205078c(u8 *a0)
{
    switch (*(u32 *)(a0 + 0xcc))
    {
    case 0x9d:
    case 0xa0:
    case 0xa2:
    case 0x174:
        return 1;
    }
    return 0;
}
