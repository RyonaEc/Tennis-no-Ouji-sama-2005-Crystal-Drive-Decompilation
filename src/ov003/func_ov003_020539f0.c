#include "types.h"

extern u8 data_ov003_020eb720[];

u16 func_ov003_020539f0(void)
{
    return *(u16 *)(*(u8 **)data_ov003_020eb720 + 0x346) != 0;
}
