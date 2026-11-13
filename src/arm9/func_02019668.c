#include "types.h"

u32 func_02019668(u16 *p)
{
    u32 r = 0;
    if (p[2] <= p[1] + 1 && p[0] <= p[1]) r = 1;
    return r != 0;
}
