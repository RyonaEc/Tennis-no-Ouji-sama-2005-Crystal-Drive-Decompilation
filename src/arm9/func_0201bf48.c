#include "types.h"

extern u8 data_020475e4[];
extern u8 data_020475e8[];
extern u8 data_020475ec[];

u32 func_0201bf48(void)
{
    *(u32 *)(data_020475ec) = 0x0;
    *(u32 *)(data_020475e4) = 0x0;
    *(u32 *)(data_020475e8) = 0x0;
    return (u32)data_020475e8;
}
