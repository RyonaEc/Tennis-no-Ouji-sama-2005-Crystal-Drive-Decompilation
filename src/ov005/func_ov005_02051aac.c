#include "types.h"



u32 func_ov005_02051aac(void)
{
    *(u32 *)(0x40004c0) = (-2147483649);
    *(u32 *)(0x40004c4) = 0x7fff7fff;
    return 0x40004c4;
}
