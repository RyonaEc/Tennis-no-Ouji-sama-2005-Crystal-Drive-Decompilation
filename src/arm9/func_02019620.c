#include "types.h"

extern u8 data_02045668[];
extern u8 data_02045ba8[];
extern u8 data_020460e8[];

u8 *func_02019620(u32 a0)
{
    switch (a0)
    {
    case 1: return data_02045668;
    case 2: return data_02045ba8;
    case 3: return data_020460e8;
    }
    return 0;
}