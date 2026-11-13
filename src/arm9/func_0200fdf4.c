#include "types.h"

typedef struct { u32 v[4]; } V;
extern u8 data_02042a68[];
void func_0200fdf4(u32 a0, V *dst)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154);
    *dst = *(V *)(p + a0 * 0x10 + 0x24c);
}
