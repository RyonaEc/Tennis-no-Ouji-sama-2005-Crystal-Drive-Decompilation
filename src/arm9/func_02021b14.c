#include "types.h"

void func_02021b14(u32 *p, u32 a, u32 b, u32 c, u32 d)
{
    *p = ((a | 0x40) | (b << 8)) | ((c | (d << 8)) << 16);
}