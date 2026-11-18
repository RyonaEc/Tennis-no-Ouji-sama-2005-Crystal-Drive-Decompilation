#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_02010280(s32 a0, u8 *a1)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x158) + a0 * 8 + 0x24;

    *(Blk *)p = *(Blk *)a1;
}
