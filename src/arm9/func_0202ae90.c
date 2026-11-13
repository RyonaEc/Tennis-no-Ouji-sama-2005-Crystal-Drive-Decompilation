#include "types.h"

extern u8 data_0204ac68[];
void func_0202ae90(u32 a0)
{
    do {
    } while ((*(volatile u16 *)(data_0204ac68 + 0x36) & a0) != 0);
}
