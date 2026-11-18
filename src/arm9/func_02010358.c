#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_02010358(u8 *a0)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x158) + 0x1c;

    *(Blk *)a0 = *(Blk *)p;
}
