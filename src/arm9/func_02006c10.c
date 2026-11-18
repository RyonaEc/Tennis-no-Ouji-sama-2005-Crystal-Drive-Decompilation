#include "types.h"

extern u8 data_02042a68[];

typedef struct { u8 b[12]; } Blk;

void func_02006c10(u8 *a0)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10) + 0xbc;

    *(Blk *)a0 = *(Blk *)p;
}
