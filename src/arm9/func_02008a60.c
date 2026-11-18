#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_02008a60(u8 *a0)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + 0xc8;

    *(Blk *)a0 = *(Blk *)p;
}
