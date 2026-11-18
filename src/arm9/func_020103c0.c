#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[8]; } Blk;

void func_020103c0(void)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x158) + 0x1c;
    u8 *q = *(u8 **)(data_02042a68 + 0x10) + 0xc8;

    *(Blk *)q = *(Blk *)p;
}
