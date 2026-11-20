#include "types.h"

typedef struct {
    u32 w[23];
} Blk;

extern u8 data_02042a68[];

void func_02006d24(Blk *dst)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10);
    Blk *src = *(Blk **)(p + 0x150);

    *dst = *src;
}