#include "types.h"

extern u8 data_0204ab00[];

void func_02027a14(u32 a0)
{
    u8 *p = data_0204ab00 + a0 * 12;

    p[8]++;
}