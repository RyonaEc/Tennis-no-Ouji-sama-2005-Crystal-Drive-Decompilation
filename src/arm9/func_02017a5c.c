#include "types.h"

extern u8 data_02045644[];
extern u8 *func_02017a88(u8 *, s32);

u8 *func_02017a5c(s32 arg)
{
    u8 *r = data_02045644;
    u8 *hit = func_02017a88(r, arg);

    if (hit != 0)
        r = hit + 0xc;
    return r;
}
