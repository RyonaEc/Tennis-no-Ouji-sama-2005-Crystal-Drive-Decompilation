#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_0200e99c(u8 *a0)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + 0xc8;

    *(Blk *)p = *(Blk *)a0;
}
