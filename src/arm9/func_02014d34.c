#include "types.h"

extern u8 data_02042a68[];

u32 func_02014d34(u32 a0)
{
    return (0x18 * a0 + *(u32 *)((u8 *)(*(u32 *)(data_02042a68 + 20)) + 264));
}
