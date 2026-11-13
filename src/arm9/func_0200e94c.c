#include "types.h"

extern u8 data_02042a68[];

u32 func_0200e94c(u32 a0)
{
    *(u8 *)(*(u32 *)((u8 *)data_02042a68 + 16) + 11) = a0;
    return a0;
}
