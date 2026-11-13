#include "types.h"

void func_02025dac(u32 a0, u32 a1)
{
    *(u32 *)(0x027ff000 + a0 * 4 + 0xda0) = a1;
}
